// corrupt_gen.js - negatif kontrol: orijinal ServerData'yi alip dogrulama alanlarini bozar
// 0x4FF MENU_BUTTON_07 -> 0x7F (tutarsiz), 0x554 Engine CRC -> 0xDEADBEEF (yanlis),
// 0x4E0 ClientVersion -> "9.99.99" (SPK.ini MainCode ile uyumsuz). Sonra XOR 0x20 encode.
'use strict';
const fs = require('fs');
const src = process.argv[2] || 'C:/Axion Mu Source/BuildLog/2d2/backup/ServerData.bmd';
const dst = process.argv[3] || 'C:/Axion Mu Source/BuildLog/2d2/results/negative_serverdata.bmd';
const b = Buffer.from(fs.readFileSync(src));
if (b.length !== 1089576) { console.error('beklenmeyen boyut', b.length); process.exit(1); }
for (let i = 0; i < b.length; i++) b[i] ^= 0x20;   // decode

console.log('once: 0x4FF=' + b[0x4FF], '0x554=0x' + b.readUInt32LE(0x554).toString(16).toUpperCase(), 'ver="' + b.slice(0x4E0, 0x4E8).toString('latin1').replace(/\0+$/, '') + '"');
b[0x4FF] = 0x7F;
b.writeUInt32LE(0xDEADBEEF, 0x554);
Buffer.from('9.99.99\0').copy(b, 0x4E0);
console.log('sonra: 0x4FF=' + b[0x4FF], '0x554=0x' + b.readUInt32LE(0x554).toString(16).toUpperCase(), 'ver="' + b.slice(0x4E0, 0x4E8).toString('latin1').replace(/\0+$/, '') + '"');

for (let i = 0; i < b.length; i++) b[i] ^= 0x20;   // encode
fs.writeFileSync(dst, b);
console.log('yazildi:', dst, b.length);