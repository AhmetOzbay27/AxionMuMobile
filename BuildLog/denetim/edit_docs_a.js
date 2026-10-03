const fs = require('fs');
const CRLF = '\r\n';
const J = a => a.join(CRLF);
function edit(file, fn) {
  let s = fs.readFileSync(file, 'utf8');
  const before = s.length;
  s = fn(s);
  fs.writeFileSync(file, s, 'utf8');
  console.log(file, before, '->', s.length);
}
function splice(s, a, b, block) {
  const i = s.indexOf(a), j = s.indexOf(b);
  if (i < 0 || j < 0) throw new Error('anchor yok: ' + (i<0?a:b));
  return s.slice(0, i) + block + s.slice(j + b.length);
}

edit('docs/00-PROJE-HARITASI.md', s => splice(s,
  '**Şu anki tek aktif görev:**',
  'Sıradaki: **2c.1-A** (Harmony).',
  J([
    '**Şu anki tek aktif görev:** Faz 2c **devam ediyor** — 2c.1 dizimi + ilk 6 kalem tamam',
    '(A1 Harmony, A2 MonsterSkill, A3 CustomItemSetPro · B1 EventMainManager, B2 EventGvG, B3 ActiveInvasions;',
    'raporlar [15](15-EVENTGVG-E2E-DOGRULAMA.md)-[18](18-SPK-CUSTOMITEMSETPRO-A3.md); sıra listesi [13](13-2C1-ONCELIK-IS-EMRI.md)).',
    'Faz 2d ✅ (D4/D5 hariç) · Faz 2e.1-2e.9 ✅ ([22](22-2E1-2E2-PAKET.md)-[29](29-14-TABLO-KALICI-VERI-KATMANI.md)).',
    '**04.10.2026 — PROJE ÇAPINDA DENETİM → [33](33-DENETIM-PROJE-CAPINDA.md):** 2 kritik bulgu',
    '(dağıtım ikilileri bayat; Main derlemesi kırık), durum belgeleri bu turda güncellendi.',
    '**Parite ölçümü artık 5 eksen:** [30](30-PARITE-MANIFESTI.md) · [31](31-ISTEMCI-EKSENI-IS-EMRI.md) · [32](32-PROTOKOL-KAYDI.md).',
    '**Sıradaki:** 3C.0 Main derlemesinin açılması + 2c kalan kalemleri.',
  ])));

edit('docs/02-YOL-HARITASI.md', s => {
  s = splice(s,
    '- ⬜ **2c.1** 05-SPK-MODUL-ENVANTERI.md\'yi öncelik sırasına diz (oyun akışı',
    '(Her modül kendi satırını alacak; plan onayından sonra buraya açılır.)',
    J([
      '- ✅ **2c.1** 01.10.2026: 60 modül oyun-akışı önceliğine dizildi → [13](13-2C1-ONCELIK-IS-EMRI.md)',
      '  (A=24 çekirdek · B=9 event · C=21 QoL · D=3 anticheat · E=3 kosmetik).',
      '- 🔄 **2c.2..N** Modül modül: iskelet yaz → GS derle → config dosyasını üret →',
      '  istemci tarafı ihtiyacı varsa Main\'e ekle → test → CHANGELOG + commit.',
      '  **Tamamlanan kalemler:** A1 Harmony, A2 MonsterSkill ([16](16-SPK-MONSTERSKILL-A2.md)),',
      '  A3 CustomItemSetPro ([18](18-SPK-CUSTOMITEMSETPRO-A3.md)), B1 EventMainManager,',
      '  B2 EventGvG ([15](15-EVENTGVG-E2E-DOGRULAMA.md)), B3 ActiveInvasions ([17](17-CB-ACTIVEINVAISIONS-E2E.md)).',
      '  **03.10.2026 ölçüm güncellemesi:** parite 5 eksene ayrıldı → [30](30-PARITE-MANIFESTI.md);',
      '  istemci ekseni iş emri [31](31-ISTEMCI-EKSENI-IS-EMRI.md); opcode kaydı [32](32-PROTOKOL-KAYDI.md).',
    ]));
  s = s.replace('### ÇIKIŞ KRİTERİ (Faz 2 → 3 geçişi)',
    J([
      '- ✅ **2e.6** (03.10.2026) C-02 DB şema denetimi kapandı; sunucu hattı derlenebilir hale getirildi',
      '  (14 eksik tablo oluşturuldu, 5 bileşen 0 hata). H-018 açık risk olarak kayda geçti. Rapor: [26](26-C02-DB-SEMA-UYUMU.md).',
      '- ✅ **2e.7** (03.10.2026) H-018 kapandı: canlı GameServer.pdb (DIA) ile viewport düzenleri ölçüldü',
      '  (36/38/20/20 B); istemci yapıları sunucuyla hizalandı; `viewport_layout.js` 4/4 HİZALI. Kabul edilen',
      '  fark: EX803 dağıtımı canlıdan 13/1/11 bayt farklı — canlı bayt paritesi için EX603+HAISLOTRING=0. Rapor: [27](27-H018-VIEWPORT-PAKET-DUZENI.md).',
      '- ✅ **2e.8** (03.10.2026) Sunucu kaynaklarında okuma/mesaj hatası taraması: 4 gerçek kusur düzeltildi;',
      '  kapsam sınırı ölçülerek yazıldı (102 ek çağrı açık). Rapor: [28](28-OKUMA-MESAJ-HATASI-TARAMASI.md).',
      '- ✅ **2e.9** (03.10.2026) 14 tablo için kalıcı veri katmanı (CDataStore) + MEMB_INFO tutarlılığı:',
      '  59/59 test geçti; 2 gerçek kusur düzeltildi. Rapor: [29](29-14-TABLO-KALICI-VERI-KATMANI.md).',
      '',
      '### ÇIKIŞ KRİTERİ (Faz 2 → 3 geçişi)',
    ]));
  s = s.replace('envanterinde açık kalem kalmamış.',
    J([
      'envanterinde açık kalem kalmamış.',
      '',
      '> **Denetim notu (04.10.2026):** Çıkış kriteri HENÜZ sağlanmadı — Main derlemesi kırık',
      '> (3C.0 işi) ve modül envanteri açık (bkz. [30](30-PARITE-MANIFESTI.md)). Faz 3.1/3.2',
      '> kullanıcı talebiyle kısmen koşuldu; tam geçiş önce bu kriterin kapanmasına bağlı.',
      '> Ayrıntı: [33](33-DENETIM-PROJE-CAPINDA.md).',
    ]));
  return s;
});

edit('docs/03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md', s => {
  s = s.replace('## A. SUNUCU (GameServer) — 58 eksik modül (map tabanlı kesin sayı)',
    J([
      '> **04.10.2026 denetim güncellemesi:** 2c ilerlemesi burada geride kalmıştı — tamamlanan ilk',
      '> kalemler: A1/A2/A3 · B1/B2/B3 (C-01b ✅). Ölçüm artık 5 eksen: [30](30-PARITE-MANIFESTI.md).',
      '> Denetim raporu: [33](33-DENETIM-PROJE-CAPINDA.md).',
      '',
      '## A. SUNUCU (GameServer) — 58 eksik modül (map tabanlı kesin sayı)',
    ]));
  s = s.replace('| ⬜ Faz 2c (öncelikler 05 dokümanında hazır) |',
    '| 🔄 2c devam — ilk 6 kalem tamam (A1/A2/A3/B1/B2/B3); sıra: docs/13 · 5-eksen: docs/30 |');
  s = s.replace('| ⬜ 2c (05 listesine 60. kalem) |',
    '| ✅ **2c.1-B2 (02.10.2026)** — donor birebir port + E2E; rapor: docs/15 |');
  return s;
});
console.log('TAMAM');
