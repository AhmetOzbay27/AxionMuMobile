// scanner_selfcheck.js - 2e.8 tarayicisinin NEGATIF KONTROLü.
// "OKUMA 0 · MESAJ 0" ancak tarayici gercekten yakaliyorsa anlamlidir.
// Bu betik: (1) taban sayimi, (2) bilerek kacak yerlastirilmis sentinel ile
// sayimin artmasi, (3) sentinel kaldirilinca sayimin tabana donmesi
// kosullarini dogrular. Bir kosul saglanmazsa EXIT=1.
const fs = require('fs');
const path = require('path');
const cp = require('child_process');

const REPO = path.resolve(__dirname, '..', '..');
const SCAN = path.join(__dirname, 'scan_ignored_returns.js');
const SENTINEL = path.join(REPO, 'Source', '4.GameServer', 'GameServer', '_2e8_selfcheck_sentinel.cpp');

function scan() {
  const out = cp.execFileSync(process.execPath, [SCAN], { cwd: REPO, encoding: 'latin1' });
  const n = (cat) => {
    const m = out.match(new RegExp('^===== ' + cat + ' \\((\\d+)\\)', 'm'));
    return m ? parseInt(m[1], 10) : NaN;
  };
  return { okuma: n('OKUMA'), mesaj: n('MESAJ'), diger: n('YAZMA/yardimci'), toplam: parseInt(out.match(/### .*: (\d+)/)[1], 10) };
}

const TAB = String.fromCharCode(9);
const SENT = [
  '#include <windows.h>',                              // sentinel: gecici, vcxproj listesinde YOK, derlenmez
  'void selfcheck(HANDLE h, HWND w, LPARAM lp)',
  '{',
  TAB + 'DWORD n=0; BYTE b[8];',
  TAB + 'ReadFile(h,b,8,&n,0);',                        // [1] OKUMA -> YAKALANMALI
  TAB + 'SendMessage(w,WM_NULL,0,lp);',                // [2] MESAJ -> YAKALANMALI
  TAB + 'int ok = (int)SendMessage(w,WM_NULL,0,lp);',  // [3] sonuc alindi -> YAKALANMAMALI
  TAB + '(void)SendMessage(w,WM_NULL,0,lp);',          // [4] bilincli yok sayma -> YAKALANMAMALI
  TAB + 'if(SendMessage(w,WM_NULL,0,lp) == 0) { }',     // [5] denetimli -> YAKALANMAMALI
  '}',
  ''
].join(String.fromCharCode(10));

let fail = 0;
function ok(cond, label, got) {
  console.log((cond ? '  GECTI  ' : '  KALDI  ') + label + (cond ? '' : '  (got=' + got + ')'));
  if (!cond) fail++;
}

console.log('=== 2e.8 tarayici negatif kontrolu ===');

const taban = scan();
console.log('  taban: OKUMA=' + taban.okuma + ' MESAJ=' + taban.mesaj + ' diger=' + taban.diger + ' toplam=' + taban.toplam);
ok(taban.okuma === 0, 'tabanda OKUMA = 0', taban.okuma);
ok(taban.mesaj === 0, 'tabanda MESAJ = 0', taban.mesaj);

fs.writeFileSync(SENTINEL, SENT, 'latin1');
let kurulu;
try {
  kurulu = scan();
  console.log('  sentinel: OKUMA=' + kurulu.okuma + ' MESAJ=' + kurulu.mesaj + ' toplam=' + kurulu.toplam);
  ok(kurulu.okuma === taban.okuma + 1, 'sentinel OKUMA +1 tespit edildi', kurulu.okuma);
  ok(kurulu.mesaj === taban.mesaj + 1, 'sentinel MESAJ +1 tespit edildi', kurulu.mesaj);
  ok(kurulu.toplam === taban.toplam + 2, 'sentinel toplam +2 (atlanan 3 form gercekten elendi)', kurulu.toplam);
} finally {
  fs.unlinkSync(SENTINEL);
}

const son = scan();
console.log('  sentinel silindi: OKUMA=' + son.okuma + ' MESAJ=' + son.mesaj + ' toplam=' + son.toplam);
ok(son.okuma === 0, 'sentinel sonrasi OKUMA = 0', son.okuma);
ok(son.mesaj === 0, 'sentinel sonrasi MESAJ = 0', son.mesaj);
ok(son.toplam === taban.toplam, 'sentinel sonrasi toplam tabaya dondu', son.toplam);
ok(!fs.existsSync(SENTINEL), 'sentinel dosyasi silindi');

console.log(fail === 0 ? '### TARAYICI CANLI — 0/0 SONUCI GERCEK' : `### ${fail} KOSUL BASARISIZ`);
process.exit(fail === 0 ? 0 : 1);