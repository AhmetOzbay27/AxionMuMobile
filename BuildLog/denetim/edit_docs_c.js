const fs = require('fs');
const CRLF = '\r\n';
const J = a => a.join(CRLF);
const file = 'docs/31-ISTEMCI-EKSENI-IS-EMI.md';
let s = fs.readFileSync(file, 'utf8');
const a = '**20 slotun 6\'sı bizde karşılık buluyor (%30); 14\'ü hiç yazılmamış.**';
if (!s.includes(a)) throw new Error('anchor yok');
s = s.split(a).join(J([
  '**20 slotun 5\'i bizde karşılık buluyor (%25); 15\'i hiç yazılmamış.**',
  '',
  '> **Denetim düzeltmesi (04.10.2026):** Tablo sayımı 5 ✅ (01/05/06/07/08); "6" ve "14"',
  '> tabloyla uyuşmuyordu. Slot 04\'ün canlı varlığı (`ingame_Bt_Reset.ozt`) var ama bizim',
  '> istemcide kod/varlık yok — sayılmadı.',
]));
fs.writeFileSync(file, s, 'utf8');
console.log('31 yazildi', s.length);
