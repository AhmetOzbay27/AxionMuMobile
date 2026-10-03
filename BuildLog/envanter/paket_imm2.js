// Near-miss immediate tarayicisi: ayni fonksiyonda, canli ve bizim immediate
// kumelerinde TEK bayti farkli olan word/dword ciftlerini bulur (MocNap tipi
// 'head/sub' farklarini yakalar). Ek olarak ayni word'de tam eslesme yoksa
// "sadece L"/"sadece B" listelerini de sinif bazinda yazar.
const fs = require('fs');
const path = require('path');
const dir = __dirname;

function parseDisasm(file) {
  const lines = fs.readFileSync(path.join(dir, file), 'utf8').split(/\r?\n/);
  const funcs = new Map();
  let cur = null;
  for (const ln of lines) {
    if (/^\?/.test(ln) && /:\s*$/.test(ln)) {
      const label = ln.trim().replace(/:$/, '');
      const m = label.match(/^\?([^@]+)(?:@[^@]*)?@@(.*)$/);
      const f = { label, name: m ? m[1] : label, sig: m ? m[2] : '', words: new Set(), dwords: new Set() };
      cur = f;
      const k = f.name + '|' + f.sig;
      if (!funcs.has(k)) funcs.set(k, []);
      funcs.get(k).push(f);
      continue;
    }
    if (!cur) continue;
    const im = ln.match(/^\s+[0-9A-F]{8}: [0-9A-F ]+\s+(\S+)\s*(.*)$/);
    if (!im) continue;
    const ops = im[2];
    const re = /,([0-9A-F]+)h\b/g;
    let mm;
    while ((mm = re.exec(ops)) !== null) {
      const hex = mm[1].replace(/^0+/, '') || '0';
      const v = parseInt(mm[1], 16);
      if (hex.length <= 4) cur.words.add(v);
      else if (hex.length <= 8) cur.dwords.add(v);
    }
  }
  return funcs;
}
const live = parseDisasm('live_disasm.txt');
const ours = parseDisasm('bizim_disasm.txt');
const out = [];
let found = 0;
for (const [k, lf] of live) {
  const of = ours.get(k);
  if (!of || lf.length !== 1 || of.length !== 1) continue;
  const a = lf[0], b = of[0];
  // word near-miss: tek bayt fark
  const Lw = [...a.words], Bw = [...b.words];
  const hits = [];
  for (const v of Lw) {
    if (Bw.includes(v)) continue;
    for (const w of Bw) {
      if (Lw.includes(w)) continue;
      if (v > 0xFFFF || w > 0xFFFF) continue;
      const diff = (v ^ w) & 0xFFFF;
      if (diff && (diff & (diff - 1)) === 0) { // tek bit -> tek bayt degil ama yaklasik; bayt kontrolu:
      }
      const by = ((v >> 8) & 0xFF) !== ((w >> 8) & 0xFF), bl = (v & 0xFF) !== (w & 0xFF);
      if (by !== bl) hits.push(`0x${v.toString(16).toUpperCase()} <-> 0x${w.toString(16).toUpperCase()}`);
    }
  }
  // dword near-miss: tam 1 bayt fark
  const Ld = [...a.dwords], Bd = [...b.dwords];
  const dhits = [];
  for (const v of Ld) {
    if (Bd.includes(v)) continue;
    for (const w of Bd) {
      if (Ld.includes(w)) continue;
      let nb = 0;
      for (let i = 0; i < 4; i++) if (((v >> (8 * i)) & 0xFF) !== ((w >> (8 * i)) & 0xFF)) nb++;
      if (nb === 1) dhits.push(`0x${v.toString(16).toUpperCase()} <-> 0x${w.toString(16).toUpperCase()}`);
    }
  }
  if (!hits.length && !dhits.length) continue;
  found++;
  out.push('### ' + k);
  out.push('  CANLI ' + a.label);
  out.push('  BIZIM ' + b.label);
  if (hits.length) out.push('  word: ' + [...new Set(hits)].join('  '));
  if (dhits.length) out.push('  dword: ' + [...new Set(dhits)].join('  '));
  out.push('');
}
fs.writeFileSync(path.join(dir, 'paket_imm_nearmiss.txt'), out.join('\n'));
console.log('near-miss fonksiyon sayisi:', found);
console.log(out.filter(x => x.startsWith('### ')).map(x => x.slice(4)).join('\n'));
