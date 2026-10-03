// Rapor turu kaydi: docs/14 CRLF + CHANGELOG girisi + pano (sonuc/sohbet)
const fs = require('fs');
const path = require('path');
const root = path.resolve(__dirname, '..', '..');
const rd = p => fs.readFileSync(path.join(root, p), 'utf8');
const wr = (p, s) => fs.writeFileSync(path.join(root, p), s.replace(/\r?\n/g, '\r\n'));

// 1) docs/14 -> CRLF
const rep = rd('docs/14-SPK-PAKET-BASLIK-TARAMASI.md');
wr('docs/14-SPK-PAKET-BASLIK-TARAMASI.md', rep);
console.log('docs/14 CRLF satir:', rep.split(/\r?\n/).length);

// 2) CHANGELOG girisi (en uste; baslik kural blogundan sonraki --- ayracindan sonra)
const cl = rd('docs/CHANGELOG.md').replace(/\r\n/g, '\n');
const entry = `## [26.10.02 06:50] SPK paket başlık taraması — F3 98/99 dışı 7 kesin fark + 4 aday (salt-okunur, kod değişikliği yok)

**Ne yapıldı** (kullanıcı isteği: "canlı ve bizim exe'de istemci paket tabanını
karşılaştır: F3 98/99 dışında taslakta yanlış başlıkla kalmış diğer SPK
paketlerini tara ve raporla")
- Canlı \`GameServer.exe\` (40,7 MB /disasm) ile bizim Release_EX603 exe
  (25,2 MB /disasm) paket başlığı düzeyinde karşılaştırıldı: 320 canlı + 331
  bizim fonksiyon eşleşti; 138 birebir, 67 farklı, **9'u SPK klasöründe**.
- **Kesin bulgular (ham bayt kanıtı, 7 kalem):** MocNap \`D3 9A/9B → 16/17\`,
  Harmony \`SendListItemPoint D3 24 → 0C\`, JewelBank InfoSend
  \`C1 30 F3 F5 (48 B) → C2 0084 F3 F5 (132 B / 30 slot)\`, BotMix
  \`SendDataIsTrade D3 2E → 23\`, LuckySpin \`MakeItem+ActionVongQuay
  D3 8C → 21\`, CastleEvent \`SendKillCTCMini F3 33 → 43\` (B4 girdisi),
  Buff \`GC_BuffInfo F3 13/52 B → F3 26/64 B\` (\`F3 13\` çakışma uyarısı).
- **Orta güven 4 aday:** Harmony ProcMix (\`F7 04\` + JewelBank yenileme
  çağrısı), SetStateInterface (\`F3 13\`), SendInfoItemCache (\`D3 00\`),
  Store OnPShopBuyItemRecv (ek \`18 06\` paketi).
- Yeni rapor: \`docs\\14-SPK-PAKET-BASLIK-TARAMASI.md\` (yöntem, kanıt
  adresleri, limitler, düzeltme sırası).
- Yeni araçlar (BuildLog\\envanter): \`paket_triaj.js\`, \`paket_spk_filtre.js\`,
  \`paket_imm.js\`, \`paket_imm2.js\`, \`paket_imm_spk.js\`,
  \`paket_nearmiss.js\`; çıktılar \`paket_diff.txt\`, \`paket_diff_spk.txt\`,
  \`paket_imm_nearmiss_spk.txt\`.

**Neden** — 2c.1-B dalgasında paket başlıkları parite kuralına (canlı kanıt >
donor) göre tek tek doğrulanıyor; F3 98/99 (B3) dışında kalan yanlış
başlıkların tespiti B4/B5 ve düzeltme turunun girdisi.

**Doğrulama**
- Her kesin bulgu, iki exe'nin ham \`/disasm\` satırıyla (adres + bayt) rapora
  işlendi; kod şekli birebir, fark yalnız ilgili immediate.
- Yeniden-kurma artefaktları ayıklandı (size taşması, blok birleşmesi) ve
  raporda ayrıca listelendi.
- \`paket_spk_filtre.js\` ile farklar canlı \`.map\` sınıf→\`.obj\` eşlemesi
  üzerinden SPK klasörüne indirgendi (9 kayıt).

**Durum** — Kod değişikliği YOK (istek: tara + raporla). Düzeltmeler ayrı iş
emri; bu turda commit yok.

---

`;
const anchor = '---\n\n';
const bi = cl.indexOf(anchor);
const out = cl.slice(0, bi + anchor.length) + entry + cl.slice(bi + anchor.length);
wr('docs/CHANGELOG.md', out);
console.log('CHANGELOG yeni satir:', out.split(/\r?\n/).length);

// 3) pano sonuc.json
const sonuc = {
  updated: '2026-10-02 06:50:00',
  status: 'ok',
  text: 'TAMAM: SPK paket başlık taraması (canlı vs bizim exe) tamamlandı — kod değişikliği yapılmadı (istek: tara+raporla). 320 canlı + 331 bizim paket fonksiyonu eşleşti; 67 farkın 9\'u SPK klasöründe. Kesin (ham bayt kanıtı): MocNap D3 9A/9B→16/17; Harmony SendListItemPoint D3 24→0C; JewelBank InfoSend C1 30 F3 F5 (48B)→C2 0084 F3 F5 (132B, 30 slot); BotMix SendDataIsTrade D3 2E→23; LuckySpin MakeItem/ActionVongQuay D3 8C→21; CastleEvent (B4) SendKillCTCMini F3 33→43; Buff GC_BuffInfo F3 13/52B→F3 26/64B (F3 13 çakışma uyarısı). Orta güven 4 aday: Harmony ProcMix (F7 04 + JewelBank yenileme), SetStateInterface (F3 13), SendInfoItemCache (D3 00), Store ek 18 06 paketi. Rapor: docs/14-SPK-PAKET-BASLIK-TARAMASI.md; araçlar BuildLog/envanter/paket_*.js; tam liste paket_diff.txt. Sırada: düzeltme turu + B4 SPK_CastleEvent / B5 BEventThanMa.'
};
wr('Dashboard/data/sonuc.json', JSON.stringify(sonuc, null, 2) + '\n');

// 4) pano sohbet.json (en uste 2 kayit, son 15)
const soh = JSON.parse(rd('Dashboard/data/sohbet.json'));
soh.updated = '2026-10-02 06:50:00';
soh.entries.unshift(
  { ts: '2026-10-02 06:50', kim: 'ajan', text: 'TAMAM: SPK paket başlık taraması (canlı vs bizim exe) — kod değişikliği yok. 320/331 fonksiyon eşleşti; 67 farkın 9\'u SPK klasöründe. Kesin 7 bulgu: MocNap D3 9A/9B→16/17, Harmony SendListItemPoint D3 24→0C, JewelBank C1 30 F3 F5→C2 0084 F3 F5 (30 slot), BotMix D3 2E→23, LuckySpin D3 8C→21, CastleEvent SendKillCTCMini F3 33→43, Buff GC_BuffInfo F3 13→F3 26 (F3 13 çakışması). 4 aday: Harmony ProcMix/SetStateInterface/SendInfoItemCache + Store 18 06. Rapor: docs/14-SPK-PAKET-BASLIK-TARAMASI.md.' },
  { ts: '2026-10-02 06:50', kim: 'kullanıcı', text: 'Canlı ve bizim exe\'de istemci paket tabanını karşılaştır: F3 98/99 dışında taslakta yanlış başlıkla kalmış diğer SPK paketlerini tara ve raporla.' }
);
soh.entries = soh.entries.slice(0, 15);
wr('Dashboard/data/sohbet.json', JSON.stringify(soh, null, 2) + '\n');
console.log('pano guncellendi; sohbet kayit:', soh.entries.length);
