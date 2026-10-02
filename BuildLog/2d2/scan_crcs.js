// scan_crcs.js - bilinen CRC32 sabitleri Engine.exe / Plugin.bmd / Verify.bmd icinde aranir
'use strict';
const fs = require('fs');
const files = [
  'C:/Axion Mu Mobile/Client and Tools/Client/Engine.exe',
  'C:/Axion Mu Mobile/Client and Tools/Client/Data/SPK/Plugin.bmd',
  'C:/Axion Mu Mobile/Client and Tools/Client/Data/SPK/Verify.bmd',
  'C:/Axion Mu Mobile/Client and Tools/Client/Launcher.exe'
];
const crcs = {
  'CFF0D000 Engine.exe': 0xCFF0D000,
  '7D56FCEB player.bmd': 0x7D56FCEB,
  'C2C46DFE golden ServerData': 0xC2C46DFE,
  '36413E5C live ServerData': 0x36413E5C,
  '4FE3C770 manifest ServerData': 0x4FE3C770,
  '07E682CD ConnectIP': 0x07E682CD,
  '4FE3C770? alt': 0x4FE3C770
};
for (const f of files) {
  let b;
  try { b = fs.readFileSync(f); } catch (e) { console.log(f, 'YOK'); continue; }
  console.log('=== ' + f + ' (' + b.length + ') ===');
  for (const name in crcs) {
    const v = crcs[name];
    const le = Buffer.alloc(4); le.writeUInt32LE(v >>> 0, 0);
    const hits = [];
    let i = 0;
    while ((i = b.indexOf(le, i)) >= 0 && hits.length < 6) { hits.push(i); i++; }
    if (hits.length) console.log('  ', name, '->', hits.map(h => '0x' + h.toString(16)).join(','));
  }
  // ServerData boyut sabiti 1089576 = 0x10A028
  const sz = Buffer.alloc(4); sz.writeUInt32LE(1089576, 0);
  const szHits = []; let j = 0;
  while ((j = b.indexOf(sz, j)) >= 0 && szHits.length < 6) { szHits.push(j); j++; }
  if (szHits.length) console.log('   size 1089576 ->', szHits.map(h => '0x' + h.toString(16)).join(','));
}
