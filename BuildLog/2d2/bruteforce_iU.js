// bruteforce_iU.js - iU.spk basit kodlama cozumleme denemesi
'use strict';
const fs = require('fs');
const files = [
  'C:/Axion Mu Mobile/Client and Tools/Client/iU.spk',
  'C:/Axion Mu Mobile/Client and Tools/Update/Client/iU.spk',
  'C:/Axion Mu Mobile/7.StartUp/iU.spk'
];
function score(b) {
  let n = 0;
  for (const c of b) if ((c >= 0x20 && c < 0x7f) || c === 0x0a || c === 0x0d || c === 0x09) n++;
  return n / b.length;
}
for (const f of files) {
  let b;
  try { b = fs.readFileSync(f); } catch (e) { console.log(f, 'YOK'); continue; }
  console.log('=== ' + f + '  size ' + b.length + ' ===');
  console.log('raw head:', b.slice(0, 48).toString('hex'));
  const cands = [];
  for (let k = 0; k < 256; k++) {
    const d = Buffer.from(b); for (let i = 0; i < d.length; i++) d[i] ^= k;
    cands.push([score(d), k, d]);
  }
  cands.sort((x, y) => y[0] - x[0]);
  for (const [sc, k, d] of cands.slice(0, 5)) {
    console.log('  key 0x' + k.toString(16).padStart(2, '0'), 'printable', (sc * 100).toFixed(0) + '%', JSON.stringify(d.slice(0, 100).toString('latin1')));
  }
  // xor 0x20 disi: 16-bit xor denemeleri
  for (const k of [0x2020, 0x8080, 0xFFFF]) {
    const d = Buffer.from(b);
    for (let i = 0; i + 1 < d.length; i += 2) { const w = (d[i] | (d[i + 1] << 8)) ^ k; d[i] = w & 0xff; d[i + 1] = w >> 8; }
    if (score(d) > 0.85) console.log('  16-bit xor 0x' + k.toString(16), JSON.stringify(d.slice(0, 100).toString('latin1')));
  }
}
