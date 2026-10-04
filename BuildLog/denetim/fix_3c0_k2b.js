const fs = require('fs');
const H = 'Source/5.Main/source/SPKData.h';
let s = fs.readFileSync(H, 'latin1');
const want = '#define SPK_CAMERA_FPS_OFFSET\t4\t\t// 0x55C: float = 240.0 (FPS x10, canli)';
const lines = s.split('\n');
let hit = 0;
for (let i = 0; i < lines.length; i++) {
  if (lines[i].includes('SPK_CAMERA_FPS_OFFSET') && lines[i].startsWith('#define')) {
    if (lines[i] !== want) { lines[i] = want; hit = 1; }
  }
}
if (!hit) { console.log('NO_CHANGE'); } else { fs.writeFileSync(H, lines.join('\n'), 'latin1'); console.log('OK: line normalized to HEAD format'); }
