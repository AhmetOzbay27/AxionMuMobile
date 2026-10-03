// update_dashboard.js - 2e7 pano kaydi (43.) + sonuc.json
// Kurallar: BOM'suz, LF, son satir newline'siz; sohbet.json semasi {updated,note,entries:[...]}
const fs = require('fs');
const path = require('path');
const DD = path.resolve(__dirname, '..', '..', 'Dashboard', 'data');
const TS = '2026-10-03 20:55';

const ENTRY = `H-018 KAPANDI — viewport paketleri sunucu↔istemci arasında bayt-bayt hizalandı. (istek: "canlı SPK sunucusundan GameServer viewport paketlerini yakalayıp bizimkiyle karşılaştırarak H-018 protokol uyuşmazlığını kapat")

NEDEN TEL ÜZERİ YAKALAMA YAPILMADI
 • Canlı yığın KAPALI: 44405 ve 55858 ECONNREFUSED, netstat'ta yok.
 • 4.MuServer/Sub-1/ içinde yalnız GameServer var (Main/Connect/Join yok) → trafiğin
   üretilmesi teknik olarak imkânsız.
 • Yerine daha kesin kanıt: canlı GameServer.exe'ın KENDİ PDB'si okundu (27 MB, 19.09.2026).
   PDB ile exe aynı derleme (zaman damgası 6AAE6177).

CANLI SUNUCUNUN DERLENMİŞ PAKET DÜZENİ (DIA ile)
 • PMSG_VIEWPORT_PLAYER  36 B  (index,x,y,CharSet[18],name[10],tx,ty,DirAndPkLevel,count@35)
 • PMSG_VIEWPORT_CHANGE  38 B  (count@37)
 • PMSG_VIEWPORT_MONSTER 20 B  (CurHp@9, Level[2]@10, Life@12, count@16)
 • PMSG_VIEWPORT_SUMMON  20 B  (name@9, count@19)
 • dumpbin /disasm + GameServer.map çapraz doğrulaması: sabit kısım 0x6D→0x90 = 36 bayt,
   ardından GenerateEffectList çıktısı.

KÖK NEDEN — derleme yapılandırması
 • Canlı = HAISLOTRING 0 + GAMESERVER_UPDATE>=402 (disassembly'de GetDuelArenaBySpectator
   çağrısı var) → Release_EX603.
 • Bizim 2e.6 derlemesi Release_EX803'tü: sunucu 49 bayt gönderiyor, istemcinin PCREATE_*
   yapılarında attribute/level/MaxHP/CurHP alanları YOKTU → pet tipi attribute'tan,
   s_BuffCount muun item'ın düşük baytından okunuyor, viewport karakterleri kayıyordu.

DÜZELTME (kullanıcı kararı: EX803 korunur, eksik alanlar istemciye eklenir)
 • WSclient.h: dört PCREATE_* yapısına Attribute → MuunItem[2] → Level[2] → MaxHP[4]
   → CurHP[4] kabloda aynı sırayla eklendi.
 • WSclient.cpp: ReceiveCreatePlayer/Monster/SummonViewport → c->Level artık level[2]'den.
 • Viewport.h + Viewport.cpp: canlı <701 PMSG_VIEWPORT_MONSTER varyantı (CurHp/Level/Life)
   eklendi; EX803 dalı değiştirilmedi.

DOĞRULAMA
 • node BuildLog/2e7/viewport_layout.js 803 1 → "### TUM PAKETLER HIZALI", exit 0.
 • Aynı araç 603 0 ile canlı PDB'nin değerlerini BİREBİR üretiyor (aracın kendi doğrulaması).
 • GameServer Release_EX803: GS_EXIT=0 · 11.294.208 B · md5 fd7e2c14f891ddbf9599967741468e2e.
 • WSclient.cpp tek başına derlendi: WSCLIENT_BUILD_EXIT=0 · 4.852.503 B.
   (Main tam derlemesi başka ajanın aynı anda düzenlediği SPKData.cpp/SPKMenuBar.cpp'deki
   4 hatada duruyor; o dosyalara dokunulmadı.)

KABUL EDİLEN FARK: dağıtım yapımız canlıdan PLAYER/CHANGE 13, MONSTER 1, SUMMON 11 bayt
 farklı (bilinçli). Tam parite = Release_EX603 + HAISLOTRING 0; kaynak bunu destekliyor.

Kanıt: docs/27-H018-VIEWPORT-PAKET-DUZENI.md · BuildLog/2e7/ (pdbtype.cpp, viewport_layout.js,
live_pdb_viewport_layout.txt, live_gs_GCViewportPlayerSend.asm, layout_*.txt, build logları)`;

const SP = path.join(DD, 'sohbet.json');
const s = JSON.parse(fs.readFileSync(SP, 'utf8'));
s.updated = TS;
s.entries.unshift({ ts: TS, kim: 'ajan', text: ENTRY });
// panel son 15 kaydi gosterir
if (s.entries.length > 15) s.entries = s.entries.slice(0, 15);
fs.writeFileSync(SP, JSON.stringify(s, null, 2), 'utf8');

const RP = path.join(DD, 'sonuc.json');
const r = JSON.parse(fs.readFileSync(RP, 'utf8'));
r.updated = TS;
r.status = 'ok';
r.text = ENTRY;
fs.writeFileSync(RP, JSON.stringify(r, null, 2), 'utf8');

// pano dosyalari: son satir newline'siz
for (const f of [SP, RP]) {
  const b = fs.readFileSync(f, 'utf8').replace(/\n+$/, '');
  fs.writeFileSync(f, b, 'utf8');
}
console.log('pano guncellendi · kayit sayisi=' + s.entries.length);