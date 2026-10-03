// update_docs_2e8.js - CHANGELOG + pano (44. kayit)
const fs = require('fs');
const path = require('path');
const R = path.resolve(__dirname, '..', '..');
const DD = path.join(R, 'Dashboard', 'data');
const TS = '2026-10-03 21:40';
let fail = 0;

// ---------------------------------------------------------------- CHANGELOG
{
  const p = path.join(R, 'docs/CHANGELOG.md');
  let s = fs.readFileSync(p, 'utf8');
  const anchor = '\n## [26.10.03 20:50]';
  const i = s.indexOf(anchor);
  if (i < 0) { console.log('HATA CHANGELOG girisi yok'); fail++; }
  else {
    const entry = `
## [26.10.03 21:40] Sunucu kaynaklarında okuma + mesaj hatası taraması — yok sayılan dönüş değerleri düzeltildi

**Ne yapıldı**

Tarama tahminle değil ölçümle yapıldı: \`BuildLog/2e8/scan_ignored_returns.js\`
ConnectServer/DataServer/JoinServer/GameServer kaynak ağaçlarını tarar ve
dönüş değeri yakalanmamış API çağrılarını listeler. İlk envanter: **121**
(kapsama \`ReadFile\`, \`ReadProcessMemory\`, \`recv\`, \`fread\`, \`SendMessage\`,
\`TranslateMessage\`, \`FindWindow\`, \`closesocket\`, \`WriteFile\` vb. aileler alındı).

**GERÇEK KUSUR 1 — \`CPacketManager::LoadKey\` (okuma hatası)**
Dört \`ReadFile\` dönüşü **hiç denetlenmiyordu**. Kırpık anahtar dosyasında
\`HeaderInfo\`/\`table[]\` stack artığıyla dolup \`LoadEncryptionKey\` yine de
\`1\` dönüyor; sunucu sessizce yanlış şifreleme tablosuyla çalışıyordu.
→ \`CPacketManager::ReadExact\` yardımcısı: dönüş değeri **ve** okunan bayt
sayısı denetleniyor (kırpık dosyada \`ReadFile\` TRUE dönebilir), hata halinde
log + \`CloseHandle\` + \`return 0\`.

**GERÇEK KUSUR 2 — combo kutusu (mesaj hatası)**
\`LB_ADDSTRING\` başarısız olduğunda \`LB_ERR\` (-1) dönüyor, ama bu değer
\`LB_SETITEMDATA\`'ya olduğu gibi geçiriliyor ve onun dönüşü de yok sayılıyordu.
→ \`LB_ERR\` denetimi, \`LB_SETITEMDATA\` hatasında \`LB_DELETESTRING\` ile geri alma,
geri alma da \`LB_ERR\` dönerse loglama.

**Gerçek hata sinyali olanlar kontrol edildi**
- \`SB_SETPARTS\`: başarıda 0 döner, sıfır dışı hatadır → kontrol + log eklendi
  (GameServer.cpp, ServerDisplayer.cpp).

**Hata sinyali olmayanlarda uydurma kontrol yazılmadı**
- \`SB_SETTEXT\` önceki metnin uzunluğunu döndürür (\`0\` = "önceki metin yok"),
  \`TranslateMessage\` yalnız karakter mesajlarında TRUE döner. Bunlarda
  \`(void)\` cast'i ile "bilinçli yok sayma" kodda görünür hâle getirildi ve
  gerekçesi yorumda yazıldı.

**Nasıl doğrulandı**
- \`node BuildLog/2e8/scan_ignored_returns.js\` → **OKUMA 0, MESAJ 0**
  (yardımcı sınıf 98; okuma/mesaj değil, §6'da gerekçesiyle listelendi).
- Dört sunucu da temiz derlendi: GameServer / DataServer / JoinServer /
  ConnectServer **EXIT=0** (\`BuildLog/2e8/build_all_2e8.log\`).
- \`gLog\` için \`PacketManager.cpp\` ve \`GameServer.cpp\`'ye \`#include "Log.h"\`
  eklendi (stdafx.h içermiyor; proje düzeni: \`Connection.cpp\` böyle yapıyor).

**Kanıt:** docs/28-OKUMA-MESAJ-HATASI-TARAMASI.md · BuildLog/2e8/
`;
    s = s.slice(0, i + 1) + entry + s.slice(i);
    fs.writeFileSync(p, s, 'utf8');
    console.log('CHANGELOG guncellendi');
  }
}

// ---------------------------------------------------------------- pano
{
  const SP = path.join(DD, 'sohbet.json');
  const s = JSON.parse(fs.readFileSync(SP, 'utf8'));
  s.updated = TS;
  s.entries.unshift({
    ts: TS, kim: 'ajan',
    text: `Sunucu kaynaklarında OKUMA ve MESAJ hataları tarandı, yok sayılan dönüş değerleri düzeltildi. (istek: "sunucu kaynaklarındaki tüm mesaj hatalarını ve okuma hatalarını tara, yok sayılan dönüş değerlerini düzelt")

TARAMA
 • BuildLog/2e8/scan_ignored_returns.js — 4 sunucu ağacı, 121 yok sayılan çağrı
   (ReadFile/ReadProcessMemory/recv/fread · SendMessage/TranslateMessage/FindWindow
    · WriteFile/closesocket/CreateDirectory yardımcı sınıf dahil).
 • İlk envanter: OKUMA 4 · MESAJ 19 · yardımcı 98.

GERÇEK KUSUR 1 — okuma: CPacketManager::LoadKey'de 4 ReadFile dönüşü denetlenmiyordu.
 Kırpık anahtar dosyasında tablolar stack artığıyla dolup LoadEncryptionKey yine 1
 dönüyor; sunucu sessizce yanlış şifreleme tablosuyla çalışıyordu.
 → ReadExact yardımcısı: dönüş değeri VE okunan bayt sayısı denetleniyor
   (kırpık dosyada ReadFile TRUE dönebilir), hata halinde log + CloseHandle + 0.

GERÇEK KUSUR 2 — mesaj: LB_ADDSTRING LB_ERR(-1) döndüğünde bu değer olduğu gibi
 LB_SETITEMDATA'ya geçiyor, onun dönüşü de yok sayılıyordu.
 → LB_ERR denetimi; LB_SETITEMDATA hatasında LB_DELETESTRING ile geri alınıyor;
   geri alma da hata dönerse loglanıyor.

GERÇEK HATA SİNYALİ OLAN → SB_SETPARTS başarıda 0 döner, sıfır dışı hata: kontrol+log eklendi.

HATA SİNYALİ OLMAYAN → SB_SETTEXT (önceki metin uzunluğu) ve TranslateMessage
 (yalnız karakter mesajlarında TRUE) için uydurma kontrol YAZILMADI; (void) cast'i
 ile bilinçli yok sayma görünür hâle getirildi ve gerekçesi yorumda.

DOĞRULAMA
 • Tarayıcı tekrar: OKUMA 0 · MESAJ 0 (yardımcı 98, kapsam dışı — docs/28 §6)
 • GameServer / DataServer / JoinServer / ConnectServer: dördü de EXIT=0
 • GameServer.exe 11.294.720 B · DataServer.exe 1.059.328 B
   JoinServer.exe · ConnectServer.exe 103.936 B
 • gLog için PacketManager.cpp ve GameServer.cpp'ye #include "Log.h" (stdafx.h içermiyor)

KAPSAM DIŞI (docs/28 §6): closesocket 63 · fclose/fseek 14 · CreateDirectory 12 ·
 fwrite/WriteFile 8 · CreateThread 1 — okuma/mesaj sınıfında değil, ayrı kalem.

Kanıt: docs/28-OKUMA-MESAJ-HATASI-TARAMASI.md · BuildLog/2e8/`
  });
  if (s.entries.length > 15) s.entries = s.entries.slice(0, 15);
  fs.writeFileSync(SP, JSON.stringify(s, null, 2), 'utf8');

  const RP = path.join(DD, 'sonuc.json');
  const r = JSON.parse(fs.readFileSync(RP, 'utf8'));
  r.updated = TS; r.status = 'ok';
  r.text = s.entries[0].text;
  fs.writeFileSync(RP, JSON.stringify(r, null, 2), 'utf8');

  for (const f of [SP, RP]) fs.writeFileSync(f, fs.readFileSync(f, 'utf8').replace(/\n+$/, ''), 'utf8');
  console.log('pano guncellendi · kayit=' + s.entries.length);
}

console.log(fail === 0 ? '### DOKUMAN+PANO TAMAM' : `### ${fail} HATA`);
process.exit(fail === 0 ? 0 : 1);