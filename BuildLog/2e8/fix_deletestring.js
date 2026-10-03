// fix_deketstring.js - LB_DELETESTRING dönüş değeri de denetlensin
// (geri alma yolunun kendisi yeni bir "mesaj hatası" bırakmasın).
const fs = require('fs');
const path = require('path');
const p = path.resolve(__dirname, '..', '..', 'Source/4.GameServer/GameServer/GameServer.cpp');
let s = fs.readFileSync(p, 'latin1');
const BT = String.fromCharCode(9);

const oldStr = BT.repeat(7) + 'SendMessage(hWndComboBox, LB_DELETESTRING, (WPARAM)pos, 0);';
const newStr = BT.repeat(7) + 'if(SendMessage(hWndComboBox, LB_DELETESTRING, (WPARAM)pos, 0) == LB_ERR)' + '\n' +
  BT.repeat(7) + '{' + '\n' +
  BT.repeat(8) + 'gLog.Output(LOG_GENERAL,"[GameServer] LB_DELETESTRING basarisiz (idx=%d)",pos);' + '\n' +
  BT.repeat(7) + '}';

if (s.indexOf(newStr.replace(/\n/g, '\r\n')) >= 0 || s.indexOf(newStr) >= 0) {
  console.log('ATLANDI (zaten uygulanmis)');
  process.exit(0);
}
if (s.indexOf(oldStr.replace(/\n/g, '\r\n')) >= 0) {
  s = s.replace(oldStr.replace(/\n/g, '\r\n'), newStr.replace(/\n/g, '\r\n'));
} else if (s.indexOf(oldStr) >= 0) {
  s = s.replace(oldStr, newStr);
} else {
  console.log('HATA: anchor bulunamadi');
  process.exit(1);
}
fs.writeFileSync(p, Buffer.from(s, 'latin1'));
console.log('LB_DELETESTRING donusu denetlendi');