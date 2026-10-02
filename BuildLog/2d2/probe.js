// probe.js — 2d.2 ön kontrol sondası
// 1) golden baseline ServerData.bmd decode: 0x4E0..0x560 alanları
// 2) canlı Engine.exe / player.bmd / golden dosyalar CRC32 (zlib) -> SPK_CRCFILE ile karşılaştır
// 3) Engine.exe içinde doğrulama mesajı / ilgili string taraması (ASCII + UTF-16LE)
'use strict';
const fs = require('fs');
const zlib = require('zlib');

const CLIENT = 'C:/Axion Mu Mobile/Client and Tools/Client';
const GOLDEN = 'C:/Axion Mu Source/BuildLog/2d1/golden';

function rd(p) { return fs.readFileSync(p); }
function crc32(buf) { return zlib.crc32 ? zlib.crc32(buf) >>> 0 : require('crypto').createHash('crc32b').update(buf).digest('hex'); }

console.log('=== 1) golden baseline ServerData decode alanlari ===');
const sd = Buffer.from(rd(GOLDEN + '/baseline_serverdata.bmd'));
for (let i = 0; i < sd.length; i++) sd[i] ^= 0x20;
console.log('size:', sd.length);
const hex = (o, n) => Array.from(sd.slice(o, o + n)).map(b => b.toString(16).padStart(2, '0')).join(' ');
console.log('0x4E0..0x4F8 :', hex(0x4E0, 0x18), '| ClientVersion="' + sd.slice(0x4E0, 0x4E8).toString('latin1') + '" ClientSerial="' + sd.slice(0x4E8, 0x4F8).toString('latin1') + '"');
console.log('0x4F9..0x530 :', hex(0x4F9, 0x37));
console.log('0x4FF (MENU_BUTTON_07):', sd[0x4FF]);
console.log('0x530..0x54C MaxAttackSpeed x7:', Array.from({ length: 7 }, (_, i) => sd.readInt32LE(0x530 + i * 4)).join(','));
console.log('0x54C ReconnectTime:', sd[0x54C]);
const u32_554 = sd.readUInt32LE(0x554);
console.log('0x554 (ClientName-dosya CRC): 0x' + u32_554.toString(16).toUpperCase().padStart(8, '0'));
console.log('0x558 CameraDefault:', sd.readFloatLE(0x558), ' 0x55C DefaultFPS:', sd.readFloatLE(0x55C));

console.log('=== 2) CRC32 karsilastirma ===');
const eng = rd(CLIENT + '/Engine.exe');
const engCrc = crc32(eng);
console.log('Engine.exe: size', eng.length, 'CRC32 0x' + engCrc.toString(16).toUpperCase().padStart(8, '0'), '| 0x554 ile esit mi?', engCrc === u32_554);
const player = rd(CLIENT + '/Data/Player/player.bmd');
console.log('player.bmd: size', player.length, 'CRC32 0x' + crc32(player).toString(16).toUpperCase().padStart(8, '0'));
const ciG = rd(GOLDEN + '/baseline_connectip.bmd');
const sdG = rd(GOLDEN + '/baseline_serverdata.bmd');
console.log('golden ConnectIP : CRC32 0x' + crc32(ciG).toString(16).toUpperCase().padStart(8, '0'));
console.log('golden ServerData: CRC32 0x' + crc32(sdG).toString(16).toUpperCase().padStart(8, '0'));
const ciL = rd(CLIENT + '/Data/SPK/ConnectIP.bmd');
const sdL = rd(CLIENT + '/Data/SPK/ServerData.bmd');
console.log('canli ConnectIP : CRC32 0x' + crc32(ciL).toString(16).toUpperCase().padStart(8, '0') + ' | golden ile ayni mi?', ciL.equals(ciG));
console.log('canli ServerData: CRC32 0x' + crc32(sdL).toString(16).toUpperCase().padStart(8, '0') + ' | golden ile ayni mi?', sdL.equals(sdG));
// canli ServerData decode: 2 fark bolgesi
const sdLd = Buffer.from(sdL); for (let i = 0; i < sdLd.length; i++) sdLd[i] ^= 0x20;
let diffs = [];
for (let i = 0; i < sd.length; i++) if (sd[i] !== sdLd[i]) diffs.push(i);
console.log('canli vs golden decode fark offsetleri (' + diffs.length + '):', diffs.map(o => '0x' + o.toString(16)).join(' '));

console.log('=== 3) Engine.exe string taramasi ===');
function findAscii(buf, str) {
  const s = Buffer.from(str, 'latin1'); const hits = []; let from = 0, i;
  while ((i = buf.indexOf(s, from)) >= 0) { hits.push(i); from = i + 1; if (hits.length >= 8) break; }
  return hits;
}
function findUtf16(buf, str) {
  const s = Buffer.from(str, 'utf16le'); const hits = []; let from = 0, i;
  while ((i = buf.indexOf(s, from)) >= 0) { hits.push(i); from = i + 1; if (hits.length >= 8) break; }
  return hits;
}
const terms = ['inconsistent', 'input data', '0x0000FF', '0000FF', 'ServerData', 'ConnectIP', 'CRCFILE', 'SPK_CRCFILE', 'please verify', 'Please verify', 'is inconsistent', 'Verify.bmd', 'Plugin', 'ClientVersion', 'ClientSerial', 'ServerData.bmd', 'ConnectIP.bmd', 'SPK.ini', 'crc', 'CRC'];
for (const t of terms) {
  const a = findAscii(eng, t), u = findUtf16(eng, t);
  if (a.length || u.length) console.log(JSON.stringify(t), 'ascii:', a.map(x => '0x' + x.toString(16)).join(','), 'utf16:', u.map(x => '0x' + x.toString(16)).join(','));
}
