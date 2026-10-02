// dep_closure.js - Main.exe DLL bagimlilik kapanisi (2e.2 paket icerigi analizi)
// Kullanim: node dep_closure.js <main.exe> <paket-kok-dizin> [<paket-kok-dizin2> ...]
'use strict';
const fs = require('fs');
const path = require('path');

const main = process.argv[2];
const roots = process.argv.slice(3);            // paket icinde aranacak dizinler
const SYS = ['C:/Windows/SysWOW64', 'C:/Windows/System32'];

function importsOf(file) {
  const buf = fs.readFileSync(file);
  const pe = buf.readUInt32LE(0x3C);
  if (buf.toString('latin1', pe, pe + 4) !== 'PE\0\0') return [];
  const optSize = buf.readUInt16LE(pe + 20);
  const secOff = pe + 24 + optSize;
  const nsec = buf.readUInt16LE(pe + 6);
  const magic = buf.readUInt16LE(pe + 24);
  const dd = pe + 24 + (magic === 0x20b ? 112 : 96);
  const impRva = buf.readUInt32LE(dd + 8); // import dir index 1
  if (!impRva) return [];
  const r2o = (rva) => {
    for (let i = 0; i < nsec; i++) {
      const o = secOff + i * 40;
      const va = buf.readUInt32LE(o + 12), vs = buf.readUInt32LE(o + 8), rs = buf.readUInt32LE(o + 16), rp = buf.readUInt32LE(o + 20);
      if (rva >= va && rva < va + Math.max(vs, rs)) return rp + (rva - va);
    }
    return -1;
  };
  let o = r2o(impRva);
  const names = [];
  for (let g = 0; g < 512 && o > 0; g++) {
    const nameRva = buf.readUInt32LE(o + 12);
    if (!nameRva && !buf.readUInt32LE(o) && !buf.readUInt32LE(o + 16)) break;
    const no = r2o(nameRva);
    if (no < 0) break;
    let e = no; while (buf[e] !== 0) e++;
    names.push(buf.toString('latin1', no, e));
    o += 20;
  }
  return names;
}
function findDll(name) {
  const lower = name.toLowerCase();
  for (const r of roots) {
    const p = path.join(r, name);
    if (fs.existsSync(p)) return { p, where: 'PAKET:' + r };
  }
  for (const s of SYS) {
    const p = path.join(s, name);
    if (fs.existsSync(p)) return { p, where: 'SISTEM' };
  }
  return null;
}
const seen = new Map();
const queue = [main];
console.log('kök :', main);
while (queue.length) {
  const f = queue.shift();
  let deps;
  try { deps = importsOf(f); } catch (e) { console.log('  HATA okunamadi', f, e.message); continue; }
  for (const d of deps) {
    const key = d.toLowerCase();
    if (seen.has(key)) continue;
    const hit = findDll(d);
    const w = hit ? hit.where : 'BULUNAMADI';
    seen.set(key, w);
    console.log('  ' + d.padEnd(24), w, hit ? hit.p : '');
    if (hit && !/^(api-ms-|ext-ms-)/i.test(d)) queue.push(hit.p);
  }
}
const pkg = [...seen.entries()].filter(([k, v]) => v.startsWith('PAKET'));
console.log('\n=== PAKETLE TASINMASI GEREKENLER (' + pkg.length + ') ===');
for (const [k, v] of pkg) console.log('  ' + k + '   <- ' + v);
const missing = [...seen.entries()].filter(([k, v]) => v === 'BULUNAMADI');
if (missing.length) { console.log('\n=== BULUNAMADI (' + missing.length + ') ==='); for (const [k] of missing) console.log('  ' + k); }
