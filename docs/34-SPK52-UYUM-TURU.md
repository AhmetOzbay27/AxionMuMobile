# 34 - SPK 5.2 %100 uyum turu (viewport tel duzeni + gizli derleme kusurlari)

> **Tarih:** 04.10.2026 · **Kapsam:** SPK 5.2 canli (EX603 + sunucu HAISLOTRING 0)
> ile kaynak esitleme · **Bagli:** docs/27 (H-018), docs/28, docs/29, docs/33
> **Durum:** ✅ Tel duzeni 3 konfigurasyonda 4/4 HIZALI; 2 gizli derleme kusuru giderildi;
> Main derlemesi hala baska ajanin dosyalarinda kirik (K2).

---

## 1. OZET

Istek: "SPK 5.2 %100 uyumlu olmasi icin kontroller yap; eksik/hatali bolumleri tamamla."

Canli SPK 5.2 sunucusunun derlenmis paket duzeni (docs/27 PDB kaniti) = `Release_EX603`
+ `HAISLOTRING=0`: PLAYER 36 B, CHANGE 38 B, MONSTER 20 B (CurHp/Level/Life), SUMMON 20 B.
Bu turda:

1. Istemci `PCREATE_*` yapilari sunucudaki `Viewport.h` kosullariyla **birebir ayni**
   kosullara baglandi (yeni `GAMESERVER_HAISLOTRING` tel makrosu; istemcinin kendi
   `HAISLOTRING`'i UI icindir ve tel duzenini artik etkilemez).
2. Varsayilanlar canliya cevrildi: istemci `GAMESERVER_UPDATE=603` (+ `GAMESERVER_HAISLOTRING=0`),
   sunucu `Release_EX603` konfigurasyonuna `HAISLOTRING=0` eklendi.
3. Canli duzen **derleme zamani kilitleriyle** korunuyor: `Viewport.cpp` icinde
   `static_assert(sizeof(PMSG_VIEWPORT_*) == 36/38/20/20)` (yalniz 603 + HAISLOTRING=0 icin).
4. EX603 + HAISLOTRING=0 konfigurasyonu bu kaynakta **derlenmiyordu** (gizli kusur:
   `Protocol.cpp` icinde `EquipInventory` uyesi `#if(HAISLOTRING)` disinda kullanimda) - duzeltildi.
5. Istemcinin `GAMESERVER_UPDATE>=701` derlemesi **derlenmiyordu** (gizli kusur:
   `PacketManager.h` icindeki `using namespace CryptoPP;` global `Singleton`'i belirsizlestiriyor,
   C2872) - duzeltildi.
6. docs/28'de "gercek kusur" olarak kayitli `UuidCreateSequential` donus degeri yok sayimi
   4 sunucuda giderildi; docs/17 B3'un eksik respawn sayaci cagrisi eklendi; docs/29 B6
   (gcoin) icin TypeDB=1 yolu uyari ile isaretlendi.

---

## 2. TEL DUZENI (KONTROL + DUZELTME)

### 2.1 Kanit (degismedi - canli PDB)

`BuildLog/2e7/live_pdb_viewport_layout.txt` + docs/27 §3: PLAYER 36 / CHANGE 38 /
MONSTER 20 / SUMMON 20; count ofsetleri +35 / +37 / +16 / +19.

### 2.2 Sunucu kosullari (Viewport.h) ve istemci karsiligi

| Yapi | Sunucu kosulu | Istemci (yeni) |
|---|---|---|
| PLAYER / CHANGE (`MuunItem` <701) | `#if(HAISLOTRING) && (GAMESERVER_UPDATE<701)` | `#if(GAMESERVER_HAISLOTRING) && (GAMESERVER_UPDATE<701)` |
| PLAYER / CHANGE (attribute blogu) | `#if(GAMESERVER_UPDATE>=701)` | ayni |
| PLAYER / CHANGE (`MuunItem` ic) | `#if(GAMESERVER_UPDATE>=803)` | ayni |
| MONSTER / SUMMON | `#if(GAMESERVER_UPDATE>=701) ... #else CurHp/Level/Life` (MONSTER) | ayni |

`GAMESERVER_HAISLOTRING` (WSclient.h, varsayilan 0) = **sunucunun** HAISLOTRING degeri.
Istemcinin kendi `HAISLOTRING` makrosu (Defined_Global.h, 1) UI ozelliklerini yonetir;
tel duzenine karismaz. Boylece istemci UI'yi kapatmadan canliyla ayni kabloyu konusur.

Alan erisimleri de korumalandi (`WSclient.cpp`):
- PLAYER/SUMMON `c->Level = MAKE_NUMBERW(Data2->Level...)` -> `#if(GAMESERVER_UPDATE>=701)`
  (603 kablosunda PLAYER/SUMMON'da level alani YOK; MONSTER'da var, dokunulmadi).
- MuunItem pet bloklari -> `#if(HAISLOTRING) && (VIEWPORT_HAS_MUUNIT)`.

### 2.3 Olcum (viewport_layout.js - guncellendi)

Arac iki ayri cfg ile calisir (sunucu: `HAISLOTRING`; istemci: `GAMESERVER_HAISLOTRING`),
artik `attribute/level/MaxHP/CurHP/CurHp/Level/Life` alanlarini da esler ve
hizalama yoksa exit 1 doner.

| Konfigurasyon | PLAYER | CHANGE | MONSTER | SUMMON | Sonuc | exit |
|---|---|---|---|---|---|---|
| **603 / 0 (canli SPK 5.2)** | **36** | **38** | **20** | **20** | TUM PAKETLER HIZALI | 0 |
| 603 / 1 (sunucu HAISLOTRING=1) | 38 | 40 | 20 | 20 | TUM PAKETLER HIZALI | 0 |
| 803 / 1 (test cifti) | 49 | 51 | 21 | 31 | TUM PAKETLER HIZALI | 0 |

Kanit dosyalari: `BuildLog/denetim/layout_603_0.txt`, `layout_603_1.txt`, `layout_803_1.txt`.

### 2.4 Derleme zamani kilidi (sunucu)

`Source/4.GameServer/GameServer/Viewport.cpp` (include blogunun hemen alti):

```cpp
#if (GAMESERVER_UPDATE == 603) && !(HAISLOTRING)
static_assert(sizeof(PMSG_VIEWPORT_PLAYER)  == 36, "SPK 5.2 PLAYER 36B olmali");
static_assert(sizeof(PMSG_VIEWPORT_CHANGE)  == 38, "SPK 5.2 CHANGE 38B olmali");
static_assert(sizeof(PMSG_VIEWPORT_MONSTER) == 20, "SPK 5.2 MONSTER 20B olmali");
static_assert(sizeof(PMSG_VIEWPORT_SUMMON)  == 20, "SPK 5.2 SUMMON 20B olmali");
#endif
```

---

## 3. GIZLI DERLEME KUSURLARI (bu turda bulunan + giderilen)

### 3.1 Sunucu EX603+HAISLOTRING=0 derlenmiyordu (Protocol.cpp)

`User.h:730-741`: `EquipInventory*` uyeleri `#if(HAISLOTRING)` icinde.
`Protocol.cpp` (RecvGetInfoChar yaniti, ~6186-6203): dongu `#if(HAISLOTRING) 15; #else 12;`
seklinde ama `else { ... EquipInventory[n] ... }` blogu **kosulsuzdu**. HAISLOTRING=0'da
dongu 12'de biter (else olu kod) fakat derleyici yine de derler -> C2039/C2660.
Duzeltme: else blogu `#if(HAISLOTRING)` icine alindi (HAISLOTRING=1 davranisi degismedi).

Kanit: duzeltme oncesi taze MSBuild EX603 -> `Protocol.cpp(6200) error C2039 'EquipInventory'`;
duzeltme sonrasi EXIT=0 (`BuildLog/denetim/gs_603.log`).

### 3.2 Istemci GAMESERVER_UPDATE>=701 derlenmiyordu (PacketManager.h)

`PacketManager.h:13` `using namespace CryptoPP;` -> `CSQuest.h`/`CDirection.h` icindeki global
`class CSQuest : public Singleton<CSQuest>` (istemcinin kendi `Singleton.h`'i) CryptoPP::Singleton
ile belirsizlesiyor: `WSclient.cpp(CSQuest.h:9) error C2872 'Singleton': belirsiz simge`.
Bu yuzden istemci hicbir zaman 701+ tanimiyla derlenemiyordu (803 cifti dahil).
Duzeltme: `using namespace CryptoPP;` kaldirildi, tipler nitelendirildi
(`CryptoPP::ECB_Mode<CryptoPP::DES_XEX3>`).

Kanit: `BuildLog/denetim/obj/wsclient_803.obj` (4.969.038 B, EXIT=0) ve
`wsclient_603.obj` (4.850.853 B, EXIT=0).

---

## 4. BILINEN ACIK KUSURLARIN KAPATILMASI

| Kayit | Kusur | Yapilan | Dogrulama |
|---|---|---|---|
| docs/28 §8 | `UuidCreateSequential` donus degeri yok sayiliyor (4 sunucu `Protect.cpp`) | `RPC_STATUS` denetimi; basarisizsa `UuidCreate`; `RPC_S_UUID_LOCAL_ONLY` basari sayilir | GS EX603+EX803, DS EX803 EXIT=0 (`build_spk52.log`) |
| docs/17 §6 (B3) | Canli respawn yolu `monster_add(class,true)` cagiriyor (0x538687); bizde yok | `ObjectManager.cpp` respawn yoluna eklendi (+ `#include "CB_ActiveInvasions.h"`) | GS EXIT=0; E2E zinciri ayni fonksiyon |
| docs/29 (B6) | `MEMB_INFO.gcoin` kolonu iki semada da yok; `TypeDB=1` yolu hata verir | `CBAutoNapGame::LoadConfig` icinde TypeDB=1 icin LOG_RED uyarisi (davranis degismedi) | DS EXIT=0 |

---

## 5. DOGRULAMA (taze kosumlar)

| Kontrol | Komut | Sonuc |
|---|---|---|
| Sunucu EX603 (canli duzeni) | MSBuild Release_EX603 + v143 | **EXIT=0** (`BuildLog/denetim/gs_603.log`); cikti: `MuServer\4.GameServer\Sub 1\GameServer\GameServer.exe` (10.656.256 B) |
| Sunucu EX803 (test cifti) | MSBuild Release_EX803 | **EXIT=0**; cikti: `MuServe Classic 5.2 Lorencia\GameServer\GameServer.exe` |
| DataServer EX803 | MSBuild Release_EX803 | **EXIT=0** (`BuildLog/denetim/build_spk52.log`) |
| Istemci 603 (varsayilan) | cl.exe WSclient.cpp | **EXIT=0**, 4.850.853 B obj |
| Istemci 803 | cl.exe /DGAMESERVER_UPDATE=803 | **EXIT=0**, 4.969.038 B obj |
| Layout 603/0 · 603/1 · 803/1 | `node BuildLog/2e7/viewport_layout.js` | **4/4 HIZALI x3, exit 0 x3** |
| Statik kilit | EX603 derlemesi (static_assert) | Gecti (aksi halde EXIT=1 olurdu) |

NOT: Main projesinin **tam** derlemesi hala EXIT=1 (K2: `SPKData.h:40` yapismis `#define`,
`SPKMenuBar.cpp:158/181` tanimsiz simgeler). Bunlar baska ajanin 3C.0 dosyalaridir;
bu turda DOKUNULMADI. Istemci degisiklikleri bu yuzden tek dosya derlemesiyle dogrulandi.

---

## 6. DEGISEN DOSYALAR

Sunucu:
- `Source/4.GameServer/GameServer/stdafx.h` - `HAISLOTRING` `#ifndef` ile disaridan verilebilir
- `Source/4.GameServer/GameServer/GameServer.vcxproj` - Release_EX603: `HAISLOTRING=0`
- `Source/4.GameServer/GameServer/Viewport.cpp` - 603/0 icin 4 `static_assert`
- `Source/4.GameServer/GameServer/Protocol.cpp` - EquipInventory else blogu kosullu
- `Source/4.GameServer/GameServer/ObjectManager.cpp` - B3 respawn `monster_add(class,true)`
- `Source/{1.ConnectServer,2.DataServer,3.JoinServer,4.GameServer}/*/Protect.cpp` - UUID denetimi
- `Source/2.DataServer/DataServer/CB_AutoNapGame.cpp` - TypeDB=1 gcoin uyarisi

Istemci:
- `Source/5.Main/source/WSclient.h` - tel duzeni makrolari + birebir kosullar
- `Source/5.Main/source/WSclient.cpp` - Level/MuunItem erisim korumalari
- `Source/5.Main/source/Defined_Global.h` - `GAMESERVER_UPDATE` varsayilani 603
- `Source/5.Main/source/PacketManager.h` - CryptoPP nitelendirme (C2872)

Arac / kanit:
- `BuildLog/2e7/viewport_layout.js` - iki cfg + ek alan esleme + exit kodu
- `BuildLog/denetim/` - layout_603_0/603_1/803_1.txt, gs_603.log, build_spk52.log, obj/*.obj, edit scriptleri

---

## 7. KALAN ISLER / KARARLAR

1. **K2 (karar: 3C.0 sahibi ajan):** Main tam derlemesi kirik. Giderilmeden "calisan cift"
   yenilenemez.
2. **K1 (karar: kullanici):** EX803 ciktilari `MuServe Classic 5.2 Lorencia\` (izlenmeyen)
   altina dusuyor; EX603 ciktilari `MuServer\` (izlenen test yigini) altina. Canli hedef
   EX603 oldugu icin test yigini artik dogru konfigurasyonla beslenir; EX803 dagitim yolu
   ayri karar.
3. **B3 §6 fazla cagri noktalari (karar: kullanici):** canli `send_list_to_client()` void
   overload'unu ve giris push'unu icermiyor (canli = istemci-ceker model). Bu turda
   davranis degistirilmedi.
4. **gcoin (B6):** Canli semada kolon yok; TypeDB=1 kullanilacaksa kolon eklenmeli.
5. Canli **istemci** derlemesinin (Main.exe) hangi HAISLOTRING'le uretildigini gosteren
   PDB elimizde yok; tel duzeni kaniti sunucu PDB'sidir ve 603+0'dir.

---

## 8. ARTEFAKTLAR

- `BuildLog/denetim/layout_603_0.txt` / `layout_603_1.txt` / `layout_803_1.txt`
- `BuildLog/denetim/gs_603.log`, `build_spk52.log`
- `BuildLog/denetim/obj/wsclient_603.obj`, `wsclient_803.obj`
- `BuildLog/denetim/edit_spk52_*.js`, `fix_tabs_wsclient.js`, `rebuild_wsclient.js`,
  `patch_layout.js`, `fix_protocol_haijslot.js`, `edit_spk52_d.js`
- Yeni derlenen ikili: `MuServer\4.GameServer\Sub 1\GameServer\GameServer.exe` (EX603)
