// find_serverdata.js - tum ServerData/ConnectIP kopyalarini bul, CRC32'leri hesapla
'use strict';
const fs = require('fs'), path = require('path'), zlib = require('zlib');
const roots = ['C:/Axion Mu Mobile', 'C:/Axion Mu Source'];
const hits = [];
function walk(d, depth) {
  if (depth > 10) return;
  let ents;
  try { ents = fs.readdirSync(d, { withFileTypes: true }); } catch (e) { return; }
  for (const e of ents) {
    if (e.name === '.git' || e.name === 'node_modules') continue;
    const p = path.join(d, e.name);
    if (e.isDirectory()) { walk(p, depth + 1); continue; }
    if (!e.isFile()) continue;
    if (!/(serverdata|connectip)\.bmd$/i.test(e.name)) continue;
    try {
      const b = fs.readFileSync(p);
      hits.push([p, b.length, zlib.crc32(b) >>> 0, require('crypto').createHash('md5').update(b).digest('hex').slice(0, 8)]);
    } catch (err) { }
  }
}
roots.forEach(r => walk(r, 0));
hits.sort((a, b2) => a[2] - b2[2] || a[0].localeCompare(b2[0]));
for (const [p, len, crc, md5] of hits) console.log(crc.toString(16).toUpperCase().padStart(8, '0'), String(len).padStart(8), md5, p);
