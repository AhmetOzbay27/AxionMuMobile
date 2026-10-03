const fs = require('fs');
const CRLF = '\r\n';
const J = a => a.join(CRLF);
function edit(file, fn) {
  let s = fs.readFileSync(file, 'utf8');
  s = fn(s);
  fs.writeFileSync(file, s, 'utf8');
  console.log(file, 'yazildi', s.length);
}
function rep(s, a, b) { if (!s.includes(a)) throw new Error('yok: ' + a.slice(0,40)); return s.split(a).join(b); }

edit('docs/30-PARITE-MANIFESTI.md', s => {
  s = rep(s, '> Ham veri: `BuildLogenvanterparite_ozellik_kaniti.tsv`',
    '> Ham veri: `BuildLog\envanter\parite_ozellik_kaniti.tsv` (2.816 B)');
  s = rep(s, '| 1 — GS kaynağı | 11 | 60 | %18 |', '| 1 — GS kaynağı | 10 | 60 | %17 |');
  s = rep(s, '| 3 — İstemci | 8 | 60 | %13 |', '| 3 — İstemci | 7 | 60 | %12 |');
  s = rep(s, '**Sonuç:** Parite = ',
    J([
      '> **Denetim düzeltmesi (04.10.2026):** Başlık sayıları bu tablodan üretilemiyordu; düzeltildi.',
      '> Tablo sayımı: GS 10 (6 ana + §3\'teki 4) · istemci karşılığı dolu satır 7 (A1/A8/B3/C47/C39/C41/C49;',
      '> "core MU" parantezliler hariç). A16 satırındaki istemci karşılığı "YOK" yanlış — `CB_AutoNapGame`',
      '> istemcide var ve [31](31-ISTEMCI-EKSENI-IS-EMI.md) slot 08\'de sayıyor. "~31 kutu" değeri config',
      '> ekseni dışlanarak hesaplanmış görünüyor (10+7+6+6=29); sayım kuralı belirsiz.',
      '',
      '**Sonuç:** Parite = ',
    ]));
  s = rep(s, '- `BuildLogenvanterparite_ozellik_kaniti.tsv` — bu tablonun makine-okunur hali',
    '- `BuildLog\envanter\parite_ozellik_kaniti.tsv` — bu tablonun makine-okunur hali');
  return s;
});

edit('docs/31-ISTEMCI-EKSENI-IS-EMI.md', s => {
  s = rep(s, '**20 slotun 6\'sı bizde karşılık buluyor (%30); 14\'ü hiç yazılmamış.**',
    J([
      '**20 slotun 5\'i bizde karşılık buluyor (%25); 15\'i hiç yazılmamış.**',
      '',
      '> **Denetim düzeltmesi (04.10.2026):** Tablo sayımı 5 ✅ (01/05/06/07/08); "6" ve "14"',
      '> tabloyla uyuşmuyordu. Slot 04\'ün canlı varlığı (`ingame_Bt_Reset.ozt`) var ama bizim',
      '> istemcide kod/varlık yok — sayılmadı.',
    ]));
  return s;
});
console.log('TAMAM');
