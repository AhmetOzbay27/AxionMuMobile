// paket_diff.txt'teki farklari canli map uzerinden obj dosyasina esler;
// SPK klasoru + kok-yeni modul listesindekileri isaretler.
const fs = require('fs');
const path = require('path');
const dir = __dirname;

const spkFiles = new Set(fs.readFileSync(path.join(dir, 'canli_spk_klasoru.txt'), 'utf8').split(/\r?\n/).filter(Boolean).map(x => x.replace(/\.cpp$/, '.obj')));

// map: sinif adi -> obj dosyasi (canli)
const mapText = fs.readFileSync(path.join(dir, 'GameServer_canli.map'), 'utf8');
const classObj = new Map();
const re = /\?([^@\s]+)@([^@\s]+)@@/g;
let m;
while ((m = re.exec(mapText)) !== null) {
  const cls = m[2];
  // obj adi satirin sonunda
  const lineEnd = mapText.indexOf('\n', re.lastIndex);
  const rest = mapText.slice(re.lastIndex, lineEnd);
  const objm = rest.match(/([A-Za-z0-9_\-\.]+\.obj)\s*$/);
  if (objm && !classObj.has(cls)) classObj.set(cls, objm[1]);
}

const diffText = fs.readFileSync(path.join(dir, 'paket_diff.txt'), 'utf8');
const blocks = diffText.split('\n### ').slice(1);
const rows = [];
for (const b of blocks) {
  const first = b.split('\n')[0].trim();
  const lm = b.match(/^  CANLI \?([^@]+)@([^@]+)@@/m);
  const bm = b.match(/^  BIZIM \?([^@]+)@([^@]+)@@/m);
  const cls = lm ? lm[2] : (bm ? bm[2] : '?');
  const obj = classObj.get(cls) || classObj.get(bm ? bm[2] : '') || '?';
  rows.push({ key: first, lcls: lm ? lm[2] : '?', bcls: bm ? bm[2] : '?', obj });
}
const spkRows = rows.filter(r => spkFiles.has(r.obj));
const otherRows = rows.filter(r => !spkFiles.has(r.obj));
const fmt = r => `${r.key.split('|')[0].padEnd(38)} L=${r.lcls.padEnd(26)} B=${r.bcls.padEnd(26)} obj=${r.obj}`;
fs.writeFileSync(path.join(dir, 'paket_diff_spk.txt'),
  '## SPK KLASORU FARKLARI\n' + spkRows.map(fmt).join('\n') + '\n\n## SPK DISI FARKLAR\n' + otherRows.map(fmt).join('\n') + '\n');
console.log('SPK-ici fark:', spkRows.length, ' SPK-disi fark:', otherRows.length);
console.log('--- SPK ICI ---');
console.log(spkRows.map(fmt).join('\n'));
