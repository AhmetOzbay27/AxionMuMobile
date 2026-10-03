// update_docs.js - 2e.7 H-018 kapanis dokuman guncellemeleri (UTF-8 korumali)
const fs = require('fs');
const path = require('path');
const D = path.resolve(__dirname, '..', '..', 'docs');
let fail = 0;
const rd = (f) => fs.readFileSync(path.join(D, f), 'utf8');
const wr = (f, s) => fs.writeFileSync(path.join(D, f), s, 'utf8');

// ------------------------------------------------------------------ docs/04
{
  const f = '04-HATA-GUNLUGU.md';
  let s = rd(f);
  const rowStart = s.indexOf('| H-018 | 03.10.2026 |');
  const rowEnd = s.indexOf('\n', rowStart);
  if (rowStart < 0 || rowEnd < 0) { console.log('HATA docs/04 H-018 satiri bulunamadi'); fail++; }
  else {
    const newRow = '| H-018 | 03.10.2026 | GameServer ↔ Main (protokol) | **Viewport paket düzeni uyuşmazlığı** — GS `Release_EX803` derlemesi 49 baytlık `PMSG_VIEWPORT_PLAYER` gönderiyordu; istemcinin `PCREATE_CHARACTER` yapısı ise `attribute`/`level`/`MaxHP`/`CurHP` alanlarını içermediği için pet tipi `attribute`\'tan, `s_BuffCount` muun item\'ın düşük baytından okunuyor ve viewport karakterleri kayıyordu | ✅ **KAPANDI (2e.7)** — canlı `GameServer.pdb` (DIA) ile canlı düzen çıkarıldı, istemci yapıları sunucuyla bayt-bayt hizalandı, `viewport_layout.js` 4/4 HİZALI (exit 0). Kanıt: [docs/27](27-H018-VIEWPORT-PAKET-DUZENI.md) |';
    s = s.slice(0, rowStart) + newRow + s.slice(rowEnd);
  }

  const secStart = s.indexOf('### H-018 hakkında bilinenler ve bu turdaki karar');
  const secEnd = s.indexOf('**Kapanış notları (03.10.2026, 2e.5 turu):**');
  if (secStart >= 0 && secEnd > secStart) {
    const closure = `### H-018 KAPANIŞ NOTLARI (03.10.2026, 2e.7 turu)

**Kök neden — derleme yapılandırması seçilememişti.** Bu projenin GameServer'ı
8 ayrı \`GAMESERVER_UPDATE\` varyantıyla derleniyor; \`Viewport.h\` her sürüm için
farklı alanlar ekliyor. İstemci ise **tek** düzen okuyor ve o düzen \`< 701\`
dönemine ait. 2e.6'da GameServer \`Release_EX803\` ile derlendiği için iki taraf
konuşmuyordu.

**Canlı sunucunun ne gönderdiği ölçüldü** (tel yakalaması mümkün değildi: canlı
yığın kapalı, 44405/55858 \`ECONNREFUSED\`; \`4.MuServer/Sub-1/\` içinde yalnız
GameServer var). Onun yerine canlı \`GameServer.exe\`'ın **kendi PDB'si** DIA ile
okundu (\`BuildLog/2e7/pdbtype.cpp\`) ve disassembly ile çapraz doğrulandı:

| Yapı | Canlı (PDB) | 2e.6 öncesi bizim |
|---|---|---|
| \`PMSG_VIEWPORT_PLAYER\` | 36 B | 49 B |
| \`PMSG_VIEWPORT_CHANGE\` | 38 B | 51 B |
| \`PMSG_VIEWPORT_MONSTER\` | 20 B (CurHp/Level/Life) | 21 B |
| \`PMSG_VIEWPORT_SUMMON\` | 20 B | 31 B |

Canlı derleme yapılandırması kanıttan çıkarıldı: \`HAISLOTRING=0\`
(MuunItem yok) + \`GAMESERVER_UPDATE>=402\` (disassembly'de
\`GetDuelArenaBySpectator\` çağrısı var) → **Release_EX603**.

**Düzeltme (kullanıcı kararı: EX803 korunur, eksik alanlar istemciye eklenir).**
Dört \`PCREATE_*\` yapısına sunucunun gönderdiği blok aynı kabloda sırayla eklendi:
\`Attribute\` → \`MuunItem[2]\` → \`Level[2]\` → \`MaxHP[4]\` → \`CurHP[4]\`.
Ayrıca canlı \`< 701\` \`PMSG_VIEWPORT_MONSTER\` varyantı (\`CurHp\`/\`Level\`/\`Life\`)
kaynağa eklendi.

**Doğrulama:** \`BuildLog/2e7/viewport_layout.js\` gerçek başlık dosyalarını okuyup
iki tarafın bayt haritasını karşılaştırıyor. Dağıtım yapımızda (803/1)
**\`### TUM PAKETLER HIZALI\`, exit 0**. Aynı araç \`603/0\` ile çalıştırıldığında
canlı PDB'nin raporladığı değerleri **birebir** üretiyor.

**Kabul edilen fark:** dağıtım yapımız canlıdan PLAYER/CHANGE'te 13, MONSTER'de 1,
SUMMON'da 11 bayt farklıdır. Bayt-bayt canlı parite istenirse tek gereken
\`Release_EX603\` + \`HAISLOTRING=0\` — kaynak bunu zaten destekliyor.

`;
    s = s.slice(0, secStart) + closure + s.slice(secEnd);
  } else { console.log('HATA docs/04 H-018 bolumu bulunamadi'); fail++; }
  wr(f, s);
  console.log('docs/04 guncellendi');
}

// ------------------------------------------------------------------ docs/30
{
  const f = '30-PARITE-MANIFESTI.md';
  let s = rd(f);
  const oldLine = '| 4 — Protokol | 5 | 60 | dış opcode seti birebir (docs/11); modül opcode kaydı yok |';
  const i = s.indexOf(oldLine);
  if (i < 0) { console.log('HATA docs/30 protokol satiri bulunamadi'); fail++; }
  else {
    const nw = '| 4 — Protokol | 6 | 60 | dış opcode seti birebir (docs/11); **viewport paket düzeni bayt-bayt doğrulandı ve hizalandı (H-018, 2e.7 — canlı PDB kanıtı, [docs/27](27-H018-VIEWPORT-PAKET-DUZENI.md))**; modül opcode kaydı yok |';
    s = s.slice(0, i) + nw + s.slice(i + oldLine.length);
    s = s.replace('bugün ~`30` kutu yeşil.', 'bugün ~`31` kutu yeşil.');
    wr(f, s);
    console.log('docs/30 guncellendi');
  }
}

// -------------------------------------------------------------- CHANGELOG.md
{
  const f = 'CHANGELOG.md';
  let s = rd(f);
  const anchor = '\n## [26.10.03 15:30]';
  const i = s.indexOf(anchor);
  if (i < 0) { console.log('HATA CHANGELOG girisi bulunamadi'); fail++; }
  else {
    const entry = `
## [26.10.03 20:50] H-018 KAPANDI — viewport paketleri sunucu↔istemci arasında bayt-bayt hizalandı

**Ne yapıldı**

**(A) Canlı sunucunun paket düzeni ölçüldü**
- İstek "canlı SPK sunucusundan paketleri yakala" idi; **yakalama imkânsızdı**:
  canlı yığın kapalı (44405 ve 55858 \`ECONNREFUSED\`, \`netstat\`'ta yok) ve
  \`4.MuServer/Sub-1/\` içinde yalnız \`GameServer\` var (\`Main\`/\`Connect\`/\`Join\` yok).
- Bunun yerine **canlı \`GameServer.exe\`'ın kendi PDB'si** okundu. Araç:
  VS DIA SDK ile \`BuildLog/2e7/pdbtype.cpp\` → \`pdbtype.exe\`; çıktı
  \`BuildLog/2e7/live_pdb_viewport_layout.txt\`. PDB ile exe aynı derleme
  (zaman damgası \`6AAE6177\` = 19.09.2026).
- \`dumpbin /disasm\` + \`GameServer.map\` ile çapraz doğrulama
  (\`live_gs_GCViewportPlayerSend.asm\`): sabit kısım \`0x6D→0x90\` = **36 bayt**,
  ardından \`GenerateEffectList\` çıktısı.

**(B) Kök neden: derleme yapılandırması**
- Canlı: \`HAISLOTRING=0\` (MuunItem yok) + \`GAMESERVER_UPDATE>=402\`
  (disassembly'de \`GetDuelArenaBySpectator\` çağrısı) → **Release_EX603**.
- Bizim 2e.6 derlemesi \`Release_EX803\`'tü: sunucu 49 bayt gönderiyor,
  istemcinin \`PCREATE_*\` yapıları \`attribute\`/\`level\`/\`MaxHP\`/\`CurHP\`
  alanlarını içermiyordu → pet tipi \`attribute\`'tan, \`s_BuffCount\` muun
  item'ın düşük baytından okunuyor, viewport bozuk geliyordu.

**(C) Düzeltme** (kullanıcı kararı: EX803 korunur, alanlar istemciye eklenir)
- \`Source/5.Main/source/WSclient.h\`: dört \`PCREATE_*\` yapısına
  \`Attribute\` → \`MuunItem[2]\` → \`Level[2]\` → \`MaxHP[4]\` → \`CurHP[4]\`
  kabloda aynı sırayla eklendi.
- \`Source/5.Main/source/WSclient.cpp\`: üç viewport alıcısında \`c->Level\`
  artık \`level[2]\` kablodan okunuyor.
- \`Viewport.h\` + \`Viewport.cpp\`: canlı \`< 701\` \`PMSG_VIEWPORT_MONSTER\`
  varyantı (\`CurHp\`/\`Level[2]\`/\`Life\`) eklendi (EX803 dalı değişmedi).

**Yeni araçlar:** \`BuildLog/2e7/{pdbtype.cpp, viewport_layout.js, apply_h018.js}\`

**Nasıl doğrulandı**
- \`node BuildLog/2e7/viewport_layout.js 803 1\` → **\`### TUM PAKETLER HIZALI\`,
  exit 0** (\`layout_803_1.txt\`).
- Aynı araç \`603 0\` ile çalıştırıldığında canlı PDB'nin değerlerini
  **birebir** üretiyor (PLAYER 36/count@35, CHANGE 38/count@37,
  MONSTER CurHp@9-Level@10-Life@12-count@16-20, SUMMON name@9/count@19).
- GameServer \`Release_EX803\`: **\`GS_EXIT=0\`**, 11.294.208 B,
  md5 \`fd7e2c14f891ddbf9599967741468e2e\`.
- \`WSclient.cpp\` tek başına derlendi: **\`WSCLIENT_BUILD_EXIT=0\`**, 4.852.503 B.
  (Main projesinin tam derlemesi, başka bir ajanın aynı anda düzenlediği
  \`SPKData.cpp\` / \`SPKMenuBar.cpp\` dosyalarındaki 4 hatada duruyor;
  \`ClCompile\` varsayılanı \`ErrorAndStop\` olduğu için derleme oraya kadar
  gelmiyor. Bu dosyalara dokunulmadı.)

**Kabul edilen fark:** dağıtım yapımız canlıdan PLAYER/CHANGE 13, MONSTER 1,
SUMMON 11 bayt farklı (bilinçli). Tam parite için \`Release_EX603\` +
\`HAISLOTRING=0\` yeterli — kaynak bunu destekliyor.

**Commit:** H-018 kapanışı (kod + kanıt + docs)
`;
    s = s.slice(0, i + 1) + entry + s.slice(i);
    wr(f, s);
    console.log('CHANGELOG guncellendi');
  }
}

console.log(fail === 0 ? '### DOKUMAN GUNCELLEMESI TAMAM' : `### ${fail} HATA`);
process.exit(fail === 0 ? 0 : 1);