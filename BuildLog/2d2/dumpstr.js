// dumpstr.js <exe> <startHex> <endHex> - verilen aralikta ASCII + UTF-16LE string dokumu
'use strict';
const fs = require('fs');
const p = process.argv[2];
const a = parseInt(process.argv[3], 16);
const b = parseInt(process.argv[4], 16);
const buf = fs.readFileSync(p).slice(a, b);

function scanAscii(x, base) {
  const out = []; let start = -1;
  for (let i = 0; i <= x.length; i++) {
    const c = i < x.length ? x[i] : 0;
    const ok = c >= 0x20 && c < 0x7f;
    if (ok) { if (start < 0) start = i; }
    else { if (start >= 0 && i - start >= 4) out.push([base + start, 'A', x.slice(start, i).toString('latin1')]); start = -1; }
  }
  return out;
}
function scanU16(x, base) {
  const out = []; let start = -1;
  for (let i = 0; i + 2 <= x.length; i += 2) {
    const w = x[i] | (x[i + 1] << 8);
    const ok = w >= 0x20 && w < 0x7f;
    if (ok) { if (start < 0) start = i; }
    else { if (start >= 0 && i - start >= 8) out.push([base + start, 'U', x.slice(start, i).toString('utf16le')]); start = -1; }
  }
  if (start >= 0 && x.length - start >= 8) out.push([base + start, 'U', x.slice(start).toString('utf16le')]);
  return out;
}
const all = [...scanAscii(buf, a), ...scanU16(buf, a)].sort((m, n) => m[0] - n[0]);
for (const [off, enc, s] of all) console.log(off.toString(16).padStart(6, '0'), enc, JSON.stringify(s));
