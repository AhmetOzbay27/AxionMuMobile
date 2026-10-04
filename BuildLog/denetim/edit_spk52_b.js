const fs = require('fs');
const f = 'Source/4.GameServer/GameServer/Viewport.cpp';
let s = fs.readFileSync(f, 'utf8');
const re = /#include "BattleSurvivor\.h"\r?\n/;
const m = s.match(re);
if (!m) throw new Error('include blogu yok');
const nl = m[0].endsWith('\r\n') ? '\r\n' : '\n';
const add = m[0] + [
  '// 04.10.2026 (docs/34): SPK 5.2 canli bayt sayilari derleme zamaninda kilitli.',
  '// Canli PDB kaniti: BuildLog/2e7/live_pdb_viewport_layout.txt (36/38/20/20).',
  '#if (GAMESERVER_UPDATE == 603) && !(HAISLOTRING)',
  'static_assert(sizeof(PMSG_VIEWPORT_PLAYER) == 36, "SPK 5.2 PLAYER 36B olmali");',
  'static_assert(sizeof(PMSG_VIEWPORT_CHANGE) == 38, "SPK 5.2 CHANGE 38B olmali");',
  'static_assert(sizeof(PMSG_VIEWPORT_MONSTER) == 20, "SPK 5.2 MONSTER 20B olmali");',
  'static_assert(sizeof(PMSG_VIEWPORT_SUMMON) == 20, "SPK 5.2 SUMMON 20B olmali");',
  '#endif',
  '',
].join(nl);
s = s.replace(re, add);
fs.writeFileSync(f, s, 'utf8');
console.log('Viewport.cpp 608/0 assertleri eklendi (nl=' + JSON.stringify(nl) + ')');
