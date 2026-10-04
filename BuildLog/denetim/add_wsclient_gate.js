const fs = require('fs');
function repl(s, needle, value, expected, label) {
  const c = s.split(needle).length - 1;
  if (c !== expected) throw new Error(label + ' capa=' + c + ' beklenen=' + expected);
  return s.split(needle).join(value);
}
function detect(raw) { return raw.includes('\r\n') ? '\r\n' : '\n'; }

// --- 1) WSclient.cpp: 603 derleme kapisi ---
const p1 = 'Source/5.Main/source/WSclient.cpp';
let c1 = fs.readFileSync(p1, 'latin1');
const NL1 = detect(c1);
const block = fs.readFileSync('BuildLog/denetim/wsclient_gate.block.txt', 'latin1')
  .replace(/\r?\n$/, '').split('\n').join(NL1);
const a1 = ['#include "CB_OffTrade.h"', '#endif', '', '#define MAX_DEBUG_MAX 10'].join(NL1);
c1 = repl(c1, a1, ['#include "CB_OffTrade.h"', '#endif', '', block, '', '#define MAX_DEBUG_MAX 10'].join(NL1), 1, 'WSclient.cpp');
fs.writeFileSync(p1, c1, 'latin1');
console.log('WSclient.cpp: kapi eklendi; NL=' + JSON.stringify(NL1));

// --- 2) WSclient.h: bayat EX803 yorumlari ---
const p2 = 'Source/5.Main/source/WSclient.h';
let c2 = fs.readFileSync(p2, 'latin1');
const NL2 = detect(c2);
c2 = repl(c2, 'EX803 cifti icin derlemede /DGAMESERVER_UPDATE=803 verin.',
  'Farkli ciftler Main projesinde DERLENEMEZ (kapi: WSclient.cpp, docs/37).', 1, 'WSclient.h-1');
c2 = repl(c2, 'EX803 cifti icin /DGAMESERVER_UPDATE=803 verin.',
  'Farkli cift derlemesi Main projesinde kapali (docs/37).', 2, 'WSclient.h-2');
fs.writeFileSync(p2, c2, 'latin1');
console.log('WSclient.h: yorumlar guncellendi; NL=' + JSON.stringify(NL2));

// --- 3) Defined_Global.h: bayat EX803 yorumu ---
const p3 = 'Source/5.Main/source/Defined_Global.h';
let c3 = fs.readFileSync(p3, 'latin1');
const NL3 = detect(c3);
c3 = repl(c3, '// Istatemci varsayilani canli ile aynidir; EX803 test cifti icin derleme',
  '// Istatemci varsayilani canli ile aynidir. Farkli ciftler (or. EX803) Main projesinde', 1, 'Defined_Global-1');
c3 = repl(c3, '// satirina /DGAMESERVER_UPDATE=803 ekleyin.',
  '// DERLENEMEZ: derleme kapisi WSclient.cpp (docs/37).', 1, 'Defined_Global-2');
fs.writeFileSync(p3, c3, 'latin1');
console.log('Defined_Global.h: yorum guncellendi; NL=' + JSON.stringify(NL3));
