const fs = require('fs');
const CRLF = '\r\n';
const J = a => a.join(CRLF);
const file = 'docs/CHANGELOG.md';
let s = fs.readFileSync(file, 'utf8');
const anchor = '## [26.10.03 22:55] 2e.9';
if (!s.includes(anchor)) throw new Error('anchor yok');
const block = J([
  '## [26.10.04 02:05] Proje çapında denetim — amaç ↔ kaynak ↔ kanıt (docs/33)',
  '',
  '**Ne yapıldı**',
  '',
  '33 doküman, git durumu, taze derleme, canlı DB sorguları ve ikili md5 karşılaştırmasıyla',
  'proje çapında denetim. Rapor: `docs/33-DENETIM-PROJE-CAPINDA.md`; kanıt: `BuildLog/denetim/`.',
  '',
  '**KRİTİK bulgular**',
  '',
  '1. **Dağıtım ikilileri bayat (K1):** EX803 derlemeleri dağıtım yoluna akmıyor — GS',
  '   `MuServe Classic 5.2 Lorencia\GameServer` (izlenmeyen), DS `Source\...\Release`.',
  '   MuServer\'daki yığın 30.09-02.10 tarihli ve 2e.6-2e.9 düzeltmelerini içermiyor',
  '   (4 bileşenin md5\'i yeni derlemelerden farklı). H-018 sonrası Main.exe de yok',
  '   (`ClientFile/Main.exe` 03.10 01:00, H-018 öncesi) → çalışan çift eski.',
  '2. **Main derlemesi kırık (K2, 3C.0 işi, commit\'siz):** `SPKData.h:40`\'ta iki `#define`',
  '   tek satıra yapışmış (`SPK_CAMERA_FPS_OFFSET` yorum içinde) → C2065+C2660;',
  '   `SPKMenuBar.cpp:158/181` `DisplayWidth`/`g_pBCustomMenuInfo` tanımsız;',
  '   `BuildLog\5Main\vc143.pdb` C1033. `MSBuild "Global Release"` EXIT=1.',
  '',
  '**ORTA bulgular**',
  '',
  '3. Durum belgeleri (00/02/03) 2c.1 ve 2e.6-2e.9\'u yansıtmıyordu → bu commit\'te güncellendi.',
  '4. docs/30-31 sayımları tablolarıyla uyuşmuyordu (GS 11→10, istemci 8→7, docs/31 6→5);',
  '   docs/30\'daki bozuk ham-veri yolu düzeltildi + denetim notları eklendi.',
  '5. `GetMain/GetMainInfo.exe` çalışma ağacında 15:02\'de yeniden derlenmiş, commit\'siz',
  '   (md5 `1991c037…`; HEAD `c480e0ba…` = docs/20 iddiası). Commit edilmedi.',
  '6. H-018 kaynakta kapalı doğrulandı (4/4 elle) ama çalışan çiftte değil; `viewport_layout.js`',
  '   dört alanı eşleyemeyip yine "HIZALI" diyor (eşleme tablosu eksik).',
  '',
  '**DOĞRULANANLAR (taze ölçüm)**',
  '',
  '* Sunucu hattı: GS/DS/JS/CS `Release_EX803` → EXIT=0 ×4.',
  '* DB: MuOnlineS6 14/14 tablo; DataNapGame\'de STT yok; gcoin iki DB\'de de yok.',
  '* docs/32 opcode iddiası: `Protocol.cpp` benzersiz case = 166 (birebir).',
  '* HEAD == origin/main; sunucu süreçleri kapalı (§5.1); pano JSON geçerli (CR=0).',
  '',
  '**Değişen dosyalar:** docs/33 (yeni) · docs/00 · docs/02 · docs/03 · docs/30 · docs/31 ·',
  'docs/CHANGELOG.md · Dashboard/data/{sohbet,sonuc}.json · BuildLog/denetim/*',
  '',
  '**Kanıt:** docs/33 · BuildLog/denetim/{build_server.log, build_main.log, viewport_layout_fresh.txt}',
  '',
  '',
]);
s = s.replace(anchor, block + anchor);
fs.writeFileSync(file, s, 'utf8');
console.log('CHANGELOG yazildi', s.length);
