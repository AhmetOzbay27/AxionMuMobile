const fs = require('fs');
let fail = 0;
const H = 'Source/5.Main/source/SPKData.h';
const M = 'Source/5.Main/source/SPKMenuBar.cpp';

// --- SPKData.h: glued #define split ---
let s = fs.readFileSync(H, 'latin1');
const i1 = s.indexOf('#define SPK_OFF_CAMERA_DEFAULT');
const i2 = s.indexOf('#define SPK_CAMERA_FPS_OFFSET');
if (i1 < 0 || i2 < 0 || i2 < i1) { console.log('FAIL: anchors missing'); fail = 1; }
else {
  const between = s.slice(i1, i2);
  if (between.includes('\n')) { console.log('SKIP: already split'); }
  else if (!between.endsWith('tahminiydi)')) { console.log('FAIL: unexpected between text: ' + JSON.stringify(between.slice(-40))); fail = 1; }
  else {
    s = s.slice(0, i2) + '\n' + s.slice(i2);
    fs.writeFileSync(H, s, 'latin1');
    console.log('OK: SPKData.h define split');
  }
}

// --- SPKMenuBar.cpp: include + DisplayWin ---
s = fs.readFileSync(M, 'latin1');
if (s.split('DisplayWidth').length !== 2) { console.log('FAIL: DisplayWidth count != 1'); fail = 1; }
else { s = s.split('(DisplayWidth - barW)').join('(DisplayWin - barW)'); if (s.includes('DisplayWidth')) { console.log('FAIL: DisplayWidth remains'); fail = 1; } else console.log('OK: DisplayWidth -> DisplayWin'); }
if (s.split('#include "NewUISystem.h"').length !== 1) { console.log('FAIL: NewUISystem include exists'); fail = 1; }
else if (s.split('#include "NewUIBCustomMenu.h"').length !== 2) { console.log('FAIL: NewUIBCustomMenu anchor != 1'); fail = 1; }
else { s = s.split('#include "NewUIBCustomMenu.h"').join('#include "NewUIBCustomMenu.h"\n#include "NewUISystem.h"'); console.log('OK: include added'); }
fs.writeFileSync(M, s, 'latin1');

// --- verify hygiene ---
for (const f of [H, M]) {
  const t = fs.readFileSync(f, 'latin1');
  if (t.includes('\uFFFD')) { console.log('FAIL: UFFFD in ' + f); fail = 1; }
  const bad = t.match(/[\x00-\x08\x0b\x0c\x0e-\x1f]/g);
  if (bad) { console.log('FAIL: control chars in ' + f + ': ' + bad.length); fail = 1; }
}
process.exit(fail);
