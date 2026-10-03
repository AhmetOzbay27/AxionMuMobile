const fs = require('fs');
const CRLF = '\r\n';
const BS = String.fromCharCode(92);
const good = 'BuildLog' + BS + 'envanter' + BS + 'parite_ozellik_kaniti.tsv';
function fix(file, pairs) {
  let s = fs.readFileSync(file, 'utf8');
  for (const [a, b] of pairs) {
    if (!s.includes(a)) throw new Error(file + ' anchor yok: ' + a.slice(0, 50));
    s = s.split(a).join(b);
  }
  fs.writeFileSync(file, s, 'utf8');
  console.log(file, 'duzeltildi', s.length);
}
fix('docs/30-PARITE-MANIFESTI.md', [
  ['`BuildLogenvanterparite_ozellik_kaniti.tsv` (2.816 B)', '`' + good + '` (2.816 B)'],
  ['- `BuildLogenvanterparite_ozellik_kaniti.tsv` — bu tablonun makine-okunur hali',
   '- `' + good + '` — bu tablonun makine-okunur hali'],
]);
fix('docs/31-ISTEMCI-EKSENI-IS-EMI.md', [
  ['**20 slotun 5\'i bizde karşılık buluyor (%25); 15\'i hiç yazılmamış.**\r\n\r\n> **Denetim düzeltmesi (04.10.2026):** Tablo sayımı 5 ✅ (01/05/06/07/08); "6" ve "14"\r\n> tabloyla uyuşmuyordu. Slot 04\'ün canlı varlığı (`ingame_Bt_Reset.ozt`) var ama bizim\r\n> istemcide kod/varlık yok — sayılmadı. 17–20 canlıda kullanılmıyor → kapsam dışı.',
   '**20 slotun 5\'i bizde karşılık buluyor (%25); 15\'i hiç yazılmamış.** 17–20 canlıda kullanılmıyor → kapsam dışı.\r\n\r\n> **Denetim düzeltmesi (04.10.2026):** Tablo sayımı 5 ✅ (01/05/06/07/08); "6" ve "14"\r\n> tabloyla uyuşmuyordu. Slot 04\'ün canlı varlığı (`ingame_Bt_Reset.ozt`) var ama bizim\r\n> istemcide kod/varlık yok — sayılmadı.'],
]);
console.log('TAMAM');
