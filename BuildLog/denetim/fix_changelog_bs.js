const fs = require('fs');
const BS = String.fromCharCode(92);
const file = 'docs/CHANGELOG.md';
let s = fs.readFileSync(file, 'utf8');
const pairs = [
  ['`MuServe Classic 5.2 LorenciaGameServer` (izlenmeyen), DS `Source...Release`.',
   '`MuServe Classic 5.2 Lorencia' + BS + 'GameServer` (izlenmeyen), DS `Source' + BS + '...' + BS + 'Release`.'],
  ['`BuildLog5Mainvc143.pdb` C1033', '`BuildLog' + BS + '5Main' + BS + 'vc143.pdb` C1033'],
  ['Kanıt: docs/33 · BuildLog/denetim/{build_server.log, build_main.log, viewport_layout_fresh.txt}',
   'Kanıt: docs/33 · BuildLog/denetim/{build_server.log, build_main.log, viewport_layout_fresh.txt}'],
];
for (const [a, b] of pairs) { if (!s.includes(a)) throw new Error('yok: ' + a.slice(0, 40)); s = s.split(a).join(b); }
fs.writeFileSync(file, s, 'utf8');
console.log('bs duzeltildi', s.length);
