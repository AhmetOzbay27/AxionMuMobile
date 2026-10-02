// asset_audit.js - Main.exe'deki Data\... yol sabitleri <-> canli SPK paketi eslesme denetimi (2e.2)
// Kullanim: node asset_audit.js <Main.exe> <canli-istemci-kok> [<deploy-kok>]
'use strict';
const fs = require('fs');
const path = require('path');

const exe = process.argv[2];
const live = process.argv[3];
const deploy = process.argv[4] || '';
const buf = fs.readFileSync(exe);

// ASCII string cikar
const strings = [];
let cur = '';
for (let i = 0; i < buf.length; i++) {
  const c = buf[i];
  if (c >= 32 && c < 127) cur += String.fromCharCode(c);
  else { if (cur.length >= 6) strings.push({ s: cur, off: i - cur.length }); cur = ''; }
}
if (cur.length >= 6) strings.push({ s: cur, off: buf.length - cur.length });

const paths = new Map();
for (const { s } of strings) {
  // Data\ ile baslayan ya da icinde Data\ gecen yol benzeri stringler
  const m = s.match(/Data\\[A-Za-z0-9_\\ .%-]{2,80}/g);
  if (!m) continue;
  for (let p of m) {
    if (!/\.(bmd|ozj|ozt|ozb|OZB|spk|jpg|png|txt|dat|ini)$/i.test(p)) continue;
    if (p.includes('%')) continue;             // printf sablonlari atla
    paths.set(p, (paths.get(p) || 0) + 1);
  }
}
const list = [...paths.keys()].sort();
console.log('yol sabiti sayisi:', list.length, '(exe:', path.basename(exe) + ')');
let okLive = 0, okDeploy = 0, missLive = [], missDeploy = [];
for (const p of list) {
  const inLive = fs.existsSync(path.join(live, p));
  const inDeploy = deploy ? fs.existsSync(path.join(deploy, p)) : false;
  if (inLive) okLive++; else missLive.push(p);
  if (inDeploy) okDeploy++; else missDeploy.push(p);
}
console.log('canli pakette VAR :', okLive, '/', list.length);
console.log('deploy pakette VAR:', okDeploy, '/', list.length);
console.log('\n=== CANLI PAKETTE OLMAYANLAR (' + missLive.length + ') ===');
for (const p of missLive) console.log('  ' + p);
console.log('\n=== DEPLOY PAKETINDE OLMAYANLAR (' + missDeploy.length + ') ===');
for (const p of missDeploy.slice(0, 60)) console.log('  ' + p);
if (missDeploy.length > 60) console.log('  ... +' + (missDeploy.length - 60) + ' adet daha');
fs.writeFileSync('BuildLog/2e2/asset_audit.txt',
  'Main.exe yol sabitleri\n' + list.map(p => (fs.existsSync(path.join(live, p)) ? 'LIVE+ ' : 'LIVE- ') + (deploy && fs.existsSync(path.join(deploy, p)) ? 'DEPLOY+ ' : 'DEPLOY- ') + p).join('\n') + '\n');
console.log('\ndetay: BuildLog/2e2/asset_audit.txt');
