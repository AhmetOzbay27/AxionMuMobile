// Basit minidump okuyucu (node, bagimlilik yok)
// Kullanim: node dump_oku.js <dosya.dmp>
const fs = require('fs');

const path = process.argv[2];
const b = fs.readFileSync(path);

const magic = b.readUInt32LE(0);
if (magic !== 0x504D444D) { console.log('MDMP degil, magic=' + magic.toString(16)); process.exit(1); }

const nStreams = b.readUInt32LE(8);
const dirRva = b.readUInt32LE(12);

const streams = [];
for (let i = 0; i < nStreams; i++) {
  const o = dirRva + i * 12;
  streams.push({ type: b.readUInt32LE(o), size: b.readUInt32LE(o + 4), rva: b.readUInt32LE(o + 8) });
}

const names = {
  3: 'ThreadList', 4: 'ModuleList', 5: 'MemoryList', 6: 'Exception',
  7: 'SystemInfo', 9: 'Memory64List', 15: 'MiscInfo'
};
console.log('=== Streamlar ===');
for (const s of streams) console.log(`tip ${s.type} (${names[s.type] || '?'}) boyut ${s.size} rva 0x${s.rva.toString(16)}`);

let mods = [];
const modStream = streams.find(s => s.type === 4);
if (modStream) {
  const n = b.readUInt32LE(modStream.rva);
  for (let i = 0; i < n; i++) {
    const o = modStream.rva + 4 + i * 108;
    const base = Number(b.readBigUInt64LE(o));
    const size = b.readUInt32LE(o + 8);
    const nr = b.readUInt32LE(o + 20); // ModuleNameRva
    const len = b.readUInt32LE(nr);
    const nm = b.slice(nr + 4, nr + 4 + len).toString('utf16le');
    mods.push({ base, size, name: nm });
  }
}

const exStream = streams.find(s => s.type === 6);
if (exStream) {
  const r = exStream.rva;
  const threadId = b.readUInt32LE(r);
  const code = b.readUInt32LE(r + 8);
  const flags = b.readUInt32LE(r + 12);
  const addr = b.readUInt32LE(r + 24);
  console.log('\n=== Istisna ===');
  console.log('thread=' + threadId + ' kod=0x' + code.toString(16) + ' flags=0x' + flags.toString(16));
  console.log('adres=0x' + addr.toString(16));
  const hit = mods.find(m => addr >= m.base && addr < m.base + m.size);
  if (hit) console.log('modul=' + hit.name.split('\\').pop() + '  modul_rva=0x' + (addr - hit.base).toString(16));
  for (let i = 0; i < 8; i++) {
    const p = b.readUInt32LE(r + 32 + i * 4);
    if (p) console.log('  param[' + i + ']=0x' + p.toString(16));
  }
}

console.log('\n=== Moduller (' + mods.length + ') ===');
for (const m of mods) console.log(`  0x${m.base.toString(16).padStart(8, '0')} ${m.size.toString(16).padStart(7, '0')} ${m.name.split('\\').pop()}`);
