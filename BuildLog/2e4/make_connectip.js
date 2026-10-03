// make_connectip.js - ConnectIP.bmd uretici (36 B): 12 B IP (XOR 0x20) + 20 B dolgu + u16 ipPort(0x20) + u16 antiPort(0x22)
// Kullanim: node make_connectip.js <ip> <port> <antiPort> <cikti>
const fs = require('fs');
const [, , ip, portS, antiS, out] = process.argv;
if (!ip || !portS || !out) { console.error('kullanim: node make_connectip.js <ip> <port> <antiPort> <cikti>'); process.exit(1); }
const port = parseInt(portS, 10);
const anti = parseInt(antiS || '55858', 10);
const buf = Buffer.alloc(36, 0x20);
if (Buffer.byteLength(ip) > 12) { console.error('IP 12 bayti asiyor'); process.exit(1); }
buf.write(ip, 0, 'latin1');
buf.writeUInt16LE(port, 0x20);
buf.writeUInt16LE(anti, 0x22);
// XOR 0x20 tumuyle (canli dosya davranisi)
for (let i = 0; i < buf.length; i++) buf[i] ^= 0x20;

// dogrulama: geri coz
const dec = Buffer.from(buf);
for (let i = 0; i < dec.length; i++) dec[i] ^= 0x20;
const ipOut = dec.slice(0, 12).toString('latin1').replace(/\0+$/, '');
console.log('decode: ip=' + ipOut + ' ipPort=' + dec.readUInt16LE(0x20) + ' antiPort=' + dec.readUInt16LE(0x22) + ' (dosya ' + buf.length + ' B)');

if (out && out !== '-') { fs.writeFileSync(out, buf); console.log('yazildi: ' + out); }
