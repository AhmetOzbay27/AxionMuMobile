// paket_imm_nearmiss.txt bloklarini SPK objelerine gore filtreler.
const fs = require('fs');
const path = require('path');
const dir = __dirname;
const spkFiles = new Set(fs.readFileSync(path.join(dir, 'canli_spk_klasoru.txt'), 'utf8').split(/\r?\n/).filter(Boolean).map(x => x.replace(/\.cpp$/, '.obj')));
const t = fs.readFileSync(path.join(dir, 'GameServer_canli.map'), 'utf8');
const re = /\?([^@\s]+)@([^@\s]+)@@/g; let m; const cls = {};
while ((m = re.exec(t)) !== null) { const c = m[2]; if (!cls[c]) { const le = t.indexOf('\n', re.lastIndex); const rest = t.slice(re.lastIndex, le); const om = rest.match(/([A-Za-z0-9_\-\.]+\.obj)\s*$/); if (om) cls[c] = om[1]; } }
const blocks = fs.readFileSync(path.join(dir, 'paket_imm_nearmiss.txt'), 'utf8').split('\n### ').slice(1);
const out = [];
for (const b of blocks) {
  const cm = b.match(/CANLI \?[^@]+@([^@]+)@@/);
  const obj = cm ? cls[cm[1]] : null;
  if (obj && spkFiles.has(obj)) out.push('### ' + b.trim() + '\n');
}
fs.writeFileSync(path.join(dir, 'paket_imm_nearmiss_spk.txt'), out.join('\n'));
console.log('SPK near-miss blok:', out.length);
console.log(out.map(x => x.split('\n')[0]).join('\n'));
