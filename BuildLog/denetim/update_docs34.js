const fs = require('fs');
const ts = '2026-10-04 03:10';

// ---- CHANGELOG ----
const cf = 'docs/CHANGELOG.md';
let ch = fs.readFileSync(cf, 'utf8');
if (ch.split('## [26.10.04 03:10]').length !== 1) throw new Error('CHANGELOG cipasi');
const entry = [
'## [26.10.04 03:10] SPK 5.2 %100 uyum turu - tel duzeni + 2 gizli derleme kusuru (docs/34)',
'',
'**Ne yapildi**',
'',
'Istemci `PCREATE_*` yapilari sunucu `Viewport.h` kosullariyla birebir ayni hale getirildi',
'(yeni `GAMESERVER_HAISLOTRING` tel makrosu; istemcinin kendi `HAISLOTRING` UI makrosu tel',
'duzenine karismaz). Varsayilan = canli SPK 5.2: istemci `GAMESERVER_UPDATE=603`, sunucu',
'`Release_EX603` + `HAISLOTRING=0`. Canli boyutlar derleme zamaninda kilitlendi',
'(`static_assert` 36/38/20/20, yalniz 603+0).',
'',
'**Gizli kusurlar (bu turda bulundu)**',
'',
'1. `Release_EX603`+`HAISLOTRING=0` bu kaynakta HIC derlenmiyordu: `Protocol.cpp` icindeki',
'   `else { ...EquipInventory[n]... }` blogu kosulsuzdu (uye `User.h`\'de `#if(HAISLOTRING)`',
'   icinde) -> C2039/C2660. Blok kosullu yapildi; GS EX603 simdi EXIT=0.',
'2. Istemci `GAMESERVER_UPDATE>=701` HIC derlenmiyordu: `PacketManager.h` icindeki',
'   `using namespace CryptoPP;` global `Singleton`\'i belirsizlestiriyordu (C2872).',
'   Nitelendirildi; WSclient.cpp 603 ve 803 tanimlariyla EXIT=0.',
'',
'**Bilinen kusurlarin kapatilmasi**',
'',
'- docs/28: `UuidCreateSequential` donus degeri denetimi (4 sunucu `Protect.cpp`).',
'- docs/17 B3: respawn yoluna canli 0x538687 karsiligi `monster_add(class,true)` eklendi.',
'- docs/29 B6: `TypeDB=1` (gcoin) yolu icin LOG_RED uyarisi; kolon iki semada da yok.',
'',
'**Dogrulama**',
'',
'- `viewport_layout.js` 603/0, 603/1, 803/1: 4/4 HIZALI, exit 0 x3 (`BuildLog/denetim/layout_*.txt`).',
'- GS EX603 + GS EX803 + DS EX803: EXIT=0 (`BuildLog/denetim/build_spk52.log`, `gs_603.log`).',
'- Istemci tek dosya: `wsclient_603.obj` (4.850.853 B) + `wsclient_803.obj` (4.969.038 B), EXIT=0.',
'- Main tam derlemesi hala K2 (baska ajanin 3C.0 dosyalari) -> EXIT=1; dokunulmadi.',
'',
'**Degisen dosyalar**',
'',
'`Source/4.GameServer/GameServer/{stdafx.h,GameServer.vcxproj,Viewport.cpp,Protocol.cpp,ObjectManager.cpp}`',
'`Source/{1.ConnectServer,2.DataServer,3.JoinServer,4.GameServer}/*/Protect.cpp`',
'`Source/2.DataServer/DataServer/CB_AutoNapGame.cpp`',
'`Source/5.Main/source/{WSclient.h,WSclient.cpp,Defined_Global.h,PacketManager.h}`',
'`BuildLog/2e7/viewport_layout.js`, `BuildLog/denetim/*`, `docs/34-SPK52-UYUM-TURU.md`',
'',
'**Rapor:** `docs/34-SPK52-UYUM-TURU.md`',
'',
].join('\n');
ch = ch.replace('## [26.10.04 02:05]', entry + '## [26.10.04 02:05]');
fs.writeFileSync(cf, ch, 'utf8');
console.log('CHANGELOG guncellendi');

// ---- PANO ----
const text = [
'SPK 5.2 %100 UYUM TURU (docs/34). (istek: "SPK 5.2 %100 uyumlu olmasi icin kontroller yap; eksik ve hatali bolumler varsa tamamla")',
'',
'YAPILANLAR',
' - Tel duzeni birebir: istemci PCREATE_* yapilari sunucu Viewport.h kosullarina baglandi (yeni GAMESERVER_HAISLOTRING tel makrosu). Varsayilan = canli: istemci 603+0, sunucu Release_EX603+HAISLOTRING=0.',
' - Canli boyutlar derleme zamaninda kilitli: Viewport.cpp static_assert 36/38/20/20 (603+0).',
' - GIZLI KUSUR 1: EX603+HAISLOTRING=0 hic derlenmiyordu -> Protocol.cpp EquipInventory else blogu kosullu yapildi; GS EX603 artik EXIT=0.',
' - GIZLI KUSUR 2: istemci GAMESERVER_UPDATE>=701 hic derlenmiyordu (PacketManager.h "using namespace CryptoPP" -> Singleton C2872) -> duzeltildi; WSclient.cpp 603 ve 803 EXIT=0.',
' - docs/28 UUID bulgusu giderildi (4 sunucu); docs/17 B3 eksik respawn monster_add(class,true) eklendi; docs/29 B6 gcoin icin TypeDB=1 uyarisi eklendi.',
'',
'DOGRULAMA',
' - Layout 603/0 + 603/1 + 803/1: 4/4 HIZALI, exit 0 x3 (BuildLog/denetim/layout_*.txt).',
' - GS EX603=EXIT0 (MuServer\4.GameServer\Sub 1), GS EX803=EXIT0, DS EX803=EXIT0.',
' - Istemci tek dosya: 603 obj 4.850.853 B, 803 obj 4.969.038 B, EXIT=0.',
'',
'KALAN',
' - K2 (baska ajanin 3C.0 dosyalari): Main tam derlemesi hala EXIT=1 -> calisan cift yenilenemedi.',
' - Karar bekleyen: K1 EX803 dagitim yolu; B3 §6 fazla cagri noktalari; gcoin kolonu.',
'',
'Kanit: docs/34-SPK52-UYUM-TURU.md - BuildLog/denetim/',
].join('\n');

const sb = JSON.parse(fs.readFileSync('Dashboard/data/sohbet.json', 'utf8'));
sb.entries.unshift({ ts, kim: 'ajan', text });
sb.entries = sb.entries.slice(0, 15);
sb.updated = ts;
fs.writeFileSync('Dashboard/data/sohbet.json', JSON.stringify(sb, null, 2), 'utf8');
const so = { updated: ts, status: 'ok', text };
fs.writeFileSync('Dashboard/data/sonuc.json', JSON.stringify(so, null, 2), 'utf8');
console.log('pano guncellendi; entries=' + sb.entries.length);
