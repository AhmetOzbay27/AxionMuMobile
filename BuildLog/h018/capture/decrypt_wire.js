// decrypt_wire.js - H-018: gProtect stream sifresi cozucu (CONN#1 = bizim istemci).
//   sifre: enc: b=(b+K2)&0xFF; b^=K1    dec: b^=K1; b=(b-K2)&0xFF
//   (K1,K2) bilinen cift ile bulunur: s2c[0:12] <-> C1 0C F1 00 01 23 28 "10334"
const fs = require('fs');
const dir = 'BuildLog/h018/capture/';
const c2s = fs.readFileSync(dir + 'c2s.bin');
const s2c = fs.readFileSync(dir + 's2c.bin');
const C1_S2C = Buffer.from([0xC1,0x0C,0xF1,0x00,0x01,0x23,0x28,0x31,0x30,0x33,0x33,0x34]);
function dec(b, k1, k2) { return (((b ^ k1) - k2) & 0xFF); }
let hits = [];
for (let k1 = 0; k1 < 256; k1++) for (let k2 = 0; k2 < 256; k2++) {
  let ok = true;
  for (let i = 0; i < C1_S2C.length && ok; i++) if (dec(s2c[i], k1, k2) !== C1_S2C[i]) ok = false;
  if (ok) hits.push([k1, k2]);
}
console.log('anahtar adaylari: ' + JSON.stringify(hits));

const K1 = hits[0][0], K2 = hits[0][1];
console.log('K1=0x' + K1.toString(16) + ' K2=0x' + K2.toString(16) + ' (esdeger cift sayisi=' + hits.length + ', ayni sonuc)');
function parse(pt, label) {
  console.log('--- ' + label + ' (' + pt.length + ' bayt duz) ---');
  console.log('hex: ' + pt.toString('hex').toUpperCase().replace(/(..)/g, '$1 ').trim());
  let off = 0, n = 0;
  while (off + 4 <= pt.length) {
    const h = pt[off];
    if (h !== 0xC1 && h !== 0xC2 && h !== 0xC3 && h !== 0xC4) { console.log('  #' + n + ' [' + off + '] baslik degil: 0x' + h.toString(16)); if (n === 0) break; return; }
    const size = (h === 0xC1 || h === 0xC3) ? pt[off + 1] : ((pt[off + 2] << 8) | pt[off + 1]);
    const head = (h === 0xC1 || h === 0xC3) ? pt[off + 2] : pt[off + 3];
    const sub  = (h === 0xC1 || h === 0xC3) ? pt[off + 3] : pt[off + 4];
    const partial = (size < 3 || off + size > pt.length);
    console.log('  #' + n + ' [' + off + '] tip=0x' + h.toString(16) + ' boyut=' + size + ' head=0x' + head.toString(16) +
                ' sub=0x' + sub.toString(16) + (partial ? '  <KISMI: elde ' + (pt.length - off) + ' bayt>' : ''));
    if (partial) return;
    off += size; n++;
  }
}
parse(Buffer.from(s2c.slice(0, 12).map(b => dec(b, K1, K2))), 'CONN#1 s2c (GS -> istemci)');
parse(Buffer.from(s2c.slice(12, 24).map(b => dec(b, K1, K2))), 'CONN#2 s2c (harici istemci)');
parse(Buffer.from(c2s.slice(0, 24).map(b => dec(b, K1, K2))), 'CONN#1 c2s (istemci -> GS)');
parse(Buffer.from(c2s.slice(24, 267).map(b => dec(b, K1, K2))), 'CONN#2 c2s (harici istemci)');
fs.writeFileSync(dir + 'conn1_c2s_dec.bin', Buffer.from(c2s.slice(0, 24).map(b => dec(b, K1, K2))));
fs.writeFileSync(dir + 'conn1_s2c_dec.bin', Buffer.from(s2c.slice(0, 12).map(b => dec(b, K1, K2))));
fs.writeFileSync(dir + 'conn2_c2s_dec.bin', Buffer.from(c2s.slice(24, 267).map(b => dec(b, K1, K2))));
console.log('cozulmus dosyalar yazildi: conn1_c2s_dec.bin / conn1_s2c_dec.bin / conn2_c2s_dec.bin');
