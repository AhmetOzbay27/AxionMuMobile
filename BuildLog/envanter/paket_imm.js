// Per-function immediate diff: canli vs bizim dumpbin /disasm ciktisi.
// Amac: paket basligi (marker/size/head/sub) dahil tum immediate store farklarini
// fonksiyon bazinda cikarmak (header-reconstruction'in kacirdigi durumlar icin).
const fs = require('fs');
const path = require('path');
const dir = __dirname;

function parseDisasm(file) {
  const text = fs.readFileSync(path.join(dir, file), 'utf8');
  const lines = text.split(/\r?\n/);
  const funcs = new Map(); // label -> {label,name,sig,imms:Map}
  let cur = null;
  const labelRe = /^(\S.*):\s*$/;      // '?Foo@@YAXXZ:' veya 'sub_...:' benzeri
  const insnRe = /^\s+[0-9A-F]{8}: [0-9A-F ]+\s+(\S+)\s*(.*)$/;
  for (const ln of lines) {
    const lm = ln.match(labelRe);
    if (lm && /^\?/.test(lm[1])) {
      const label = lm[1];
      const m = label.match(/^\?([^@]+)(?:@[^@]*)?@@(.*)$/);
      const f = { label, name: m ? m[1] : label, sig: m ? m[2] : '', imms: new Map() };
      cur = f;
      const k = f.name + '|' + f.sig;
      if (!funcs.has(k)) funcs.set(k, []);
      funcs.get(k).push(f);
      continue;
    }
    if (!cur) continue;
    const im = ln.match(insnRe);
    if (!im) continue;
    const ops = im[2];
    let mm;
    const re = /,([0-9A-F]+)h\b/g;
    while ((mm = re.exec(ops)) !== null) {
      const v = parseInt(mm[1], 16);
      cur.imms.set(v, (cur.imms.get(v) || 0) + 1);
    }
  }
  return funcs;
}

const live = parseDisasm('live_disasm.txt');
const ours = parseDisasm('bizim_disasm.txt');

const out = [];
let pairs = 0, diffs = 0;
for (const [k, lf] of live) {
  const of = ours.get(k);
  if (!of || lf.length !== 1 || of.length !== 1) continue;
  pairs++;
  const a = lf[0].imms, b = of[0].imms;
  const onlyL = [], onlyB = [];
  for (const [v, c] of a) { const cb = b.get(v) || 0; if (cb < c) onlyL.push(v.toString(16).toUpperCase() + (c - cb > 1 ? 'x' + (c - cb) : '')); }
  for (const [v, c] of b) { const ca = a.get(v) || 0; if (ca < c) onlyB.push(v.toString(16).toUpperCase() + (c - ca > 1 ? 'x' + (c - ca) : '')); }
  if (!onlyL.length && !onlyB.length) continue;
  diffs++;
  out.push('### ' + k);
  out.push('  CANLI ' + lf[0].label);
  out.push('    L-only: ' + (onlyL.sort().join(' ') || '-'));
  out.push('  BIZIM ' + of[0].label);
  out.push('    B-only: ' + (onlyB.sort().join(' ') || '-'));
  out.push('');
}
fs.writeFileSync(path.join(dir, 'paket_imm_diff.txt'),
  `eslesen fonksiyon=${pairs} immediate-farki-olani=${diffs}\n\n` + out.join('\n'));
console.log(`eslesen=${pairs} farkli=${diffs}`);
