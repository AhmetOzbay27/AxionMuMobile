// add_crosscheck_note.js - docs/27'ye otomatik capraz kontrol blogunu ekler
const fs = require('fs');
const path = require('path');
const p = path.resolve(__dirname, '..', '..', 'docs/27-H018-VIEWPORT-PAKET-DUZENI.md');
let s = fs.readFileSync(p, 'utf8');
const anchor = 'Bu, hem düzeltmeyi hem de aracı doğrulayan bir çapraz kontroltür.';
if (s.indexOf('crosscheck_live_parity') >= 0 && s.indexOf(anchor) < 0) {
  console.log('ATLANDI (zaten ekli)');
  process.exit(0);
}
if (s.indexOf(anchor) < 0) { console.log('HATA: anchor yok'); process.exit(1); }
const BT = String.fromCharCode(96);
const add = anchor + '\n\n' +
  'Bu kontrol **elle değil, otomatik** olarak da doğrulanır —\n' +
  BT + 'node BuildLog/2e7/crosscheck_live_parity.js' + BT + ' (çıktı:\n' +
  BT + 'crosscheck_live_parity.txt' + BT + '). Araç dört yapının **her alanının ofsetini**\n' +
  'canlı PDB kaydıyla tek tek karşılaştırır ve eşleşmezse ' + BT + 'exit 1' + BT + ' verir:\n\n' +
  BT.repeat(3) + '\n' +
  '  ESIT   PMSG_VIEWPORT_PLAYER  sizeof 36/36  alan 9\n' +
  '  ESIT   PMSG_VIEWPORT_CHANGE  sizeof 38/38  alan 10\n' +
  '  ESIT   PMSG_VIEWPORT_MONSTER sizeof 20/20  alan 11 (CurHp,Level,Life dahil)\n' +
  '  ESIT   PMSG_VIEWPORT_SUMMON  sizeof 20/20  alan 9\n' +
  '### CANLI PARITE DOGRULANDI (tum yapilar bayt-bayt ayni)\n' +
  BT.repeat(3);
s = s.replace(anchor, add);
fs.writeFileSync(p, s, 'utf8');
console.log('docs/27 capraz kontrol blogu eklendi · U+FFFD kontrolu=' + (Buffer.from(add, 'utf8').includes(Buffer.from([0xef, 0xbf, 0xbd])) ? 'VAR' : 'yok'));