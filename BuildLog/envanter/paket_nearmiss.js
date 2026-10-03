// Near-miss triaji: ayni fonksiyonda, iki tarafin header kayitlari arasinda
// TEK alani farkli olan ciftleri bulur (marker,size,head,sub dortlusunde tam 1 fark).
// Bunlar cogunlukla ayni kaynak satirinin farkli degere derlendigi durumlardir.
const fs = require('fs');
const path = require('path');
const dir = __dirname;

function parse(file) {
  const lines = fs.readFileSync(path.join(dir, file), 'utf8').split(/\r?\n/);
  const funcs = [];
  let cur = null;
  for (const ln of lines) {
    if (/^\?/.test(ln)) {
      const label = ln.trim();
      const m = label.match(/^\?([^@]+)(?:@[^@]*)?@@(.*)$/);
      cur = { label, name: m ? m[1] : label, sig: m ? m[2] : '', entries: [] };
      funcs.push(cur);
    } else if (cur && /^\s+C[1-4] /.test(ln)) {
      const e = ln.match(/^\s+(C[1-4]) size=(\d+) head=([0-9A-F]{1,2}|\?) sub=([0-9A-F]{1,2}|\?)/);
      if (e) cur.entries.push({ marker: e[1], size: e[2], head: e[3], sub: e[4], raw: ln.trim() });
    }
  }
  return funcs;
}
const live = parse('paket_header_canli.txt'), ours = parse('paket_header_bizim.txt');
const key = f => f.name + '|' + f.sig;
const mapL = new Map(), mapB = new Map();
for (const f of live) { const k = key(f); if (!mapL.has(k)) mapL.set(k, []); mapL.get(k).push(f); }
for (const f of ours) { const k = key(f); if (!mapB.has(k)) mapB.set(k, []); mapB.get(k).push(f); }

const out = [];
for (const [k, lf] of mapL) {
  const bf = mapB.get(k);
  if (!bf || lf.length !== 1 || bf.length !== 1) continue;
  const A = lf[0].entries, B = bf[0].entries;
  const usedA = new Set(), usedB = new Set();
  for (let i = 0; i < A.length; i++) {
    for (let j = 0; j < B.length; j++) {
      if (usedA.has(i) || usedB.has(j)) continue;
      const a = A[i], b = B[j];
      let d = 0, fields = [];
      if (a.marker !== b.marker) { d++; fields.push('marker'); }
      if (a.size !== b.size) { d++; fields.push('size'); }
      if (a.head !== b.head) { d++; fields.push('head'); }
      if (a.sub !== b.sub) { d++; fields.push('sub'); }
      if (d === 1) {
        usedA.add(i); usedB.add(j);
        out.push(`### ${k}   [fark: ${fields[0]}]`);
        out.push(`  CANLI ${lf[0].label}\n    L: ${a.raw}`);
        out.push(`  BIZIM ${bf[0].label}\n    B: ${b.raw}`);
        out.push('');
      }
    }
  }
}
fs.writeFileSync(path.join(dir, 'paket_nearmiss.txt'), out.join('\n'));
console.log('near-miss kayit sayisi:', out.filter(x => x.startsWith('### ')).length);
console.log(out.filter(x => x.startsWith('### ')).map(x => x.slice(4, 140)).join('\n'));
