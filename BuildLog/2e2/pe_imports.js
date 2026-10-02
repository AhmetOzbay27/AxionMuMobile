// pe_imports.js - PE32 import + delay-import tablosu okuma (2e.2 dogrulama)
// Kullanim: node pe_imports.js <exe>
'use strict';
const fs = require('fs');

const path = process.argv[2];
if (!path) { console.error('kullanim: node pe_imports.js <exe>'); process.exit(1); }
const buf = fs.readFileSync(path);

function rvaToOff(rva) {
  const nsec = buf.readUInt16LE(peOff + 6);
  const optSize = buf.readUInt16LE(peOff + 20);
  const secOff = peOff + 24 + optSize;
  for (let i = 0; i < nsec; i++) {
    const o = secOff + i * 40;
    const va = buf.readUInt32LE(o + 12);
    const vsize = buf.readUInt32LE(o + 8);
    const rawSize = buf.readUInt32LE(o + 16);
    const rawPtr = buf.readUInt32LE(o + 20);
    const size = Math.max(vsize, rawSize);
    if (rva >= va && rva < va + size) return rawPtr + (rva - va);
  }
  return -1;
}
function cstr(off) {
  if (off < 0) return '<rva disi>';
  let end = off;
  while (end < buf.length && buf[end] !== 0) end++;
  return buf.slice(off, end).toString('latin1');
}

const peOff = buf.readUInt32LE(0x3C);
if (buf.toString('latin1', peOff, peOff + 4) !== 'PE\0\0') { console.error('PE imzasi yok'); process.exit(1); }
const machine = buf.readUInt16LE(peOff + 4);
const optOff = peOff + 24;
const magic = buf.readUInt16LE(optOff);
const is64 = magic === 0x20b;
const ddOff = optOff + (is64 ? 112 : 96);

console.log('dosya :', path);
console.log('boyut :', buf.length, 'b');
console.log('md5   :', require('crypto').createHash('md5').update(buf).digest('hex'));
console.log('machine: 0x' + machine.toString(16), is64 ? '(PE32+)' : '(PE32)');

function dumpDir(idx, label) {
  const rva = buf.readUInt32LE(ddOff + idx * 8);
  const size = buf.readUInt32LE(ddOff + idx * 8 + 4);
  const names = [];
  if (rva && size) {
    let o = rvaToOff(rva);
    for (let guard = 0; guard < 256; guard++) {
      const desc = buf.readUInt32LE(o + 12); // Name RVA
      if (desc === 0 && buf.readUInt32LE(o) === 0 && buf.readUInt32LE(o + 16) === 0) break;
      names.push(cstr(rvaToOff(desc)));
      o += 20;
    }
  }
  console.log(`--- ${label} (RVA 0x${rva.toString(16)}, ${names.length}) ---`);
  for (const n of names) console.log('   ', n);
  return names;
}

const normal = dumpDir(1, 'IMPORT');
const delay = dumpDir(13, 'DELAY IMPORT');
const all = normal.concat(delay).map(s => s.toLowerCase());
const check = ['apicb.dll', 'freeimage.dll', 'gdiplus.dll'];
console.log('--- kontrol ---');
for (const c of check) console.log('   ', c, ':', all.includes(c) ? 'VAR !!!' : 'yok');
