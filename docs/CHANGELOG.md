# CHANGELOG — Axion Mu Source

> **Kural:** Projede yapılan HER değişiklik buraya kaydedilir — ne, neden,
> nasıl doğrulandı. Amaç: kalan yerin başka bir yapay zeka / geliştirici
> tarafından sohbet bağlamı olmadan anlaşılması.
> Format: [YY.AA.GG] Başlık → Değişen dosyalar → Neden → Doğrulama → Commit.

---

## [26.09.30] İlerleme panosu — dış IP'den canlı takip aracı (Dashboard\)

**Ne yapıldı**
- `Dashboard\server.ps1`: bağımlılıksız PowerShell 5.1 HttpListener sunucusu
  (makinede php/python/node yok). Uçlar: `/` (UI), `/api/status` (git log -15,
  çalışma ağacı durumu, derleme boyutları, disk, docs listesi), `/api/doc/<ad>.md`
  (docs klasöründen salt-okunur okuma; Türkçe dosya adları dahil), `/api/log/N`,
  `/api/ping`. Yol geçiş koruması: yalnız docs\*.md, `..` reddedilir.
- `Dashboard\www\index.html`: tek dosya koyu-tema UI — faz durumu panosu (00 ile
  uyumlu), sıradaki adım kutusu, GameServer bizim↔canlı + Main + GetMainInfo
  boyut tablosu, son 15 commit, sistem durumu, tıklayınca açılan doküman okuyucu;
  30 sn'de bir otomatik yenileme.
- `Dashboard\start-dashboard.cmd`: çift tıkla başlatma (dış erişim modu).
- Firewall kuralı "Axion Mu Pano 8096" (TCP in 8096) + URL ACL `http://+:8096/`
  (Everyone) yönetici onayıyla eklendi.

**Neden** — kullanıcı sunucu ping'i nedeniyle makinede oturamıyor; tüm ilerlemeyi
(faz durumu, commit akışı, dokümanlar, derleme boyutları) dış IP'den tarayıcıyla
takip etmek istiyor.

**Doğrulama**
- `http://localhost:8096/api/ping` → ok; `http://45.87.120.29:8096/api/ping` → ok
  (dış IP'den erişim çalışıyor; HTTP.sys 0.0.0.0:8096 dinliyor).
- Tüm docs dosyaları (Türkçe İ̇ adlılar dahil) `/api/doc/` ile okunuyor.
- Yol geçiş testi: `/api/doc/..%2F..%2Fserver.ps1` → 404.

**Commit** — (bu kayıtla birlikte)

## [26.09.30] Faz 2b.0-E — 12 ezilen dosyanın karşılaştırma raporu (docs/09)

**Ne yapıldı**
- 05 §4 E-01..E-12 dosyaları üçlü analiz edildi: bizim kaynak ↔ MUIG donor ↔
  canlı kanıt (GameServer_canli.map sembolleri, canlı GS exe string taraması,
  gs_strings_canli/bizim.txt, canlı config boyutları).
- Yeni rapor: `docs\09-EZILEN-12-DOSYA-KARSILASTIRMA.md` — envanter matrisi,
  metot yüzeyi farkları, dosya başına strateji (Kolay 3 / Orta 5 / Zor 4),
  uygulama sırası (E-10 Reconnect → E-01 BossGuild), 6 config eksiği.
- 00/02/05 çapraz bağlandı.

**Neden** — 2b'nin "12 ezilen dosya" kaleminin girdisi: hangi dosyada donor
alımı, birleştirme veya canlıdan yeniden yazım yapılacağı karara bağlanmalı.

**Doğrulama** (kod değişikliği yok, analiz kaydı)
- Donörde olmayanlar: BossGuild, ChangeClass, ZenDrop (yalnız bizim + canlı kanıt).
- Normalize satır benzerliği %21 (BotAlchemist) - %85 (Reconnect) aralığı.
- Canlı revizyonun iki kaynağı da aştığı örnekler: BossGuild kill→skor/ödül
  (BONUS_POINT_MONSTER + HandleBossKill, map kanıtlı), BuyVip "configuration
  reloaded", Alchemist "data load error %s", OfflineMode dar log seti.
- Bizim MuServer'da 6 canlı config eksik/eski (ChangeClass.xml, CustomBuyVip.txt,
  CustomJewel.txt, ZenDrop.xml eski, EventTime.xml, ThuMuaDoExc.txt).

**Commit** — `8152b67ee`

## [26.09.30] Faz 2b.0 — 4 MUIG donor modülü GS'ye entegre edildi (AddBuffer, CAUTOHP, CCustomJewelBank, CSkillDamage)

**Ne yapıldı**
- Donör 8 dosya `Source\4.GameServer\GameServer\SPK\` altına alındı:
  AUTOHP.cpp/.h, CustomJewelBank.cpp/.h, SkillDamage.cpp/.h, PC_AddBuff.cpp/.h
  (AddBuffer sınıfı PC_AddBuff içinde). GameServer.vcxproj + .filters güncellendi
  (4 ClCompile + 4 ClInclude, Filter=SPK); include yolu için
  AdditionalIncludeDirectories'e `$(ProjectDir)` eklendi.
- Yardımcı eksikler: SPK paket struct'ları (PSBMSG_JEWELBANK*, JEWELFOX_GUI_REQ,
  SDHP_CUSTOM_JEWELBANK_INFO_RECV) donorde CustomJewelBank.h içinde olduğundan
  ayrıca taşınmadı. User.h'e `ItemBank[10]`/`AUTOHP`/`TradeDuel`, Message.h'e
  `GlobalText` alias, ItemManager.h/.cpp'e `CheckItemInventorySpaceCount` eklendi.
- 8 kanca dosyası: User.cpp (gAUTOHP.MainProc), Protocol.cpp (0x35 AUTOHP,
  0xF5/0xF6 JewelBank, 0x79/104 GuiNgocAll, 0xFC/104 AddBuff), ServerInfo.cpp
  (3 Load), Attack.cpp (SkillDamage hasar çarpanı), DSProtocol.cpp (0xF7/0x05
  JewelBankInfoRecv), ObjectManager.cpp (CustomJewelBankInfoSend),
  CommandManager.h/.cpp (COMMAND_ADDBUFF=86 + case).
- Config şablonları canlı Sub-1'den kopyalandı (salt okunur kaynak):
  `Data\SPK\AddBuff.txt` + `Data\Custom\BotSystem\CongHuong.txt`.

**Neden** — 05 envanteri P1'in 4 "MUIG donor" kalemi; canlı GS'de bu 4 modül
var, bizim GS'de yoktu (2a envanterinin 71 iş biriminin ilk 4'ü).

**Doğrulama**
- GS `Release_EX603|Win32` temiz derlendi → GameServer.exe **10.704.896 B**
  (önceki 10.690.560), pdb 18.182.144 B; `MuServer\4.GameServer\Sub 1`'e yazıldı.
- Derleme sırasında 2 hata çözüldü: (1) CommandManager.cpp'de Move.h include'u
  kazara silinmişti (C2065 MOVE_INFO/gMove); (2) 0x35 opcode çakışması → **H-007**:
  Protocol.cpp'de HAISLOTRING CGItemEquipRepairRecv `#if(0)` ile kapatıldı;
  kanıt: bizim SPK istemcisi 0x35 göndermiyor, AutoHP istemcide yerel
  (Protect.m_MainInfo.DelayAutoHP).
- PDB sembolleri: CAUTOHP(14)/CSkillDamage(17)/CCustomJewelBank(18)/AddBuffer(12)
  + gAUTOHP/gAddBuffer/gSkillDamage/gCustomJewelBank + GetSkillDamage/
  CommandAddBuff/JewelBankRecv/GuiNgocAll.

**Commit** — `599d568ef`

## [26.09.30] Faz 1 tamamlandı — Main ve GetMainInfo derlemeleri

**Ne yapıldı**
- `Source\5.Main` `"Global Release"|Win32` + v143 ile derlendi →
  `ClientFile\Main.exe` (12.023.808 B, PE32 GUI i386, linker 14.44).
- `Source\6.GetMainInfo` `Release|Win32` ile derlendi →
  `GetMain\GetMainInfo.exe` (3.693.568 B, PE32 Console i386).
- Kaynak düzeltmeleri (derlemeyi engelleyen hatalar):
  1. `Source\5.Main\source\CustomMessage.h` — windows.h `GetMessage` makrosu
     sınıf üyesini bozuyordu → sınıftan önce `#undef GetMessage`, sınıfta
     `GetMessageA` alias'ı korundu (çagrı noktaları makro genişlemesiyle uyumlu).
  2. `Source\5.Main\source\Winmain.cpp` — mesaj döngüsünde `GetMessage` →
     `GetMessageA` (CustomMessage.h makroyu kaldırdığı için).
  3. `Source\5.Main\source\stdafx.h` — (a) `#define NOMINMAX` KALDIRILDI:
     MUIG tabanı windows.h'yi NOMINMAX'sız include eder, ~100 çıplak
     `min()/max()` çağrısı makrolara dayanır; SPK katmanı ise `(std::max)`
     parantezli stilde yazıldığı için makrolarla uyumlu. (b) PC dalına
     `typedef unsigned long long Uint64;` eklendi (Android'de SDL sağlıyor).
     (c) Masaüstü dalına `#include "Platform/MobileTime.h"` eklendi.
  4. `Source\5.Main\source\Platform\MobileTime.h` — Win32 dalı eklendi:
     QPC tabanlı inline `MU_MobilePerfNow/Frequency/ToSeconds/ToMilliseconds/`
     `GetTicks/Sleep/MobileTimeInit`. Paylaşılan dosyalar (ZzzObject, ZzzCharacter,
     ZzzScene, ZzzLodTerrain) mobil perf telemetrisi bunu çağırıyor.
  5. `Source\5.Main\source\ZzzLodTerrain.cpp` — `TERRAIN_ATTRIBUTE` `inline`
     tanımdan dış bağlantılı tanıma çevrildi (donor MUIG'de header'da
     `extern inline` bildirimi vardı; çağıran TU'larda tanım yok, LNK2001).
  6. `Source\5.Main\Main.vcxproj` — `source\ScenePerfTelemetry.cpp` derleme
     listesine eklendi (LNK2001 `g_mainScenePerfSnapshot`).

**Neden** — Faz 1 çıkış kriteri: 6 bileşenin de kaynaktan derlenebilmesi.

**Doğrulama**
- Main.exe vs canlı `ClientBuild_192.168.99.200\Main.exe` (12.003.328 B):
  aynı varyant doğrulandı (CBGetMain.bin + License.json hattı); benzersiz
  string seti 6.189 vs 6.173, ~%99,7 parite; farklar revizyon sapması +
  gömülü IP (`171.235.182.88` vs `192.168.0.150`) — bkz. H-004.
- GetMainInfo: canlı pakette bu araç YOK; 369 KB referans SPK "GetEngine"
  varyantı — parite hedefi değil, Faz 2a.5'te karar (bkz. H-005).
- Analiz çıktıları: `BuildLog\main_strings_*.txt`, `BuildLog\gmi_strings_*.txt`.

**Commit** — `1339a290` · **Etiket** — `faz1-tamamlandi`

---

## [26.09.30] Faz 1 — GameServer derlemesi + Resource.h onarımı

**Ne yapıldı**
- `Source\4.GameServer` `Release_EX603|Win32` derlendi →
  `MuServer\4.GameServer\Sub 1\GameServer\GameServer.exe` (10.689.536 B =
  canlı PDB referans boyutu, birebir).
- `Source\4.GameServer\GameServer\Resource.h` onarıldı: UTF-16 hasarlı
  dosyadan 101 orijinal tanım kurtarıldı (`iconv -f UTF-16LE -t UTF-8`);
  eksikler eklendi: IDM_INVASION14-21=161-168, IDM_STARTBSV=169,
  IDM_EVENTS_CTCMINI=170, IDM_EVENTS_BOSSGUILD=171,
  ID_FAKEONLINE_RELOADDATA/ADDFAKEONLINE/DELFAKEONLINE=32800-32802,
  _APS_NEXT_COMMAND_VALUE=32803. (IDC_ARROW bilerek eklenmedi — winuser.h sağlar.)

**Neden** — GS derlemesi Resource.h bozuk olduğu için C2051 veriyordu.

**Doğrulama** — GS derlemesi hatasız; PE32 GUI i386.

**Commit** — `eb87f89` (Resource.h) · `c28026f` (GameServer.exe + pdb)

---

## [26.09.30] Faz 1 — CS / DS / JS derlemeleri

**Ne yapıldı**
- ConnectServer, DataServer, JoinServer `Release_EX603|Win32` + v143 ile
  derlendi → `MuServer\1.ConnectServer\ConnectServer.exe` (103.936 B),
  `MuServer\2.DataServer\DataServer.exe` (1.030.656 B),
  `MuServer\3.JoinServer\JoinServer.exe` (943.616 B).
- CS/DS/JS vcxproj'larına eksik `<PlatformToolset>v143` satırları eklendi.

**Neden** — Faz 1 hattı doğrulaması (ilk zafer CS ile).

**Doğrulama** — Boyutlar canlı PDB referanslarıyla birebir aynı.

---

## [26.09.30] Proje taşıma + repo kurulumu (Faz 0 bitişi)

**Ne yapıldı**
- Proje `C:\Axion Mu Mobile\New Source Code\Axion Mu Source\` →
  `C:\Axion Mu Source\` taşındı (4,1 GB, ~101.000 dosya); eski klasör silindi.
- Git repo kuruldu (main): `f530df5` ilk commit (97.114 dosya),
  `dd2fb78` ClientBuild dışlama (.gitignore).
- VS 2022 Community kuruldu: v143 + Win SDK 10.0.22621 + ATL
  (MFC gerekmez — hiçbir proje UseOfMfc=true değil).

**Neden** — Tek ve kalıcı proje kökü + sürüm takibi.

**Not** — Eski analiz raporları `C:\Axion Mu Mobile\analiz\` altındadır;
güncel dokümantasyon `docs\` klasöründedir.

---

## ŞABLON (yeni kayıt için kopyala)

```
## [YY.AA.GG] Başlık

**Ne yapıldı**
- ...

**Neden** — ...

**Doğrulama**
- ...

**Commit** — ...
```

---

## [26.09.30] Faz 2a.2-2a.4 — Canlı sistem envanteri (salt okunur tarama)

**Ne yapıldı**
- **2a.2:** Canlı `GameServer.map` (1,8 MB) `BuildLog\envanter\GameServer_canli.map`
  olarak arşivlendi; map'ten 242 sınıf/namespace çözümlendi
  (`map_siniflar.txt`). Kaynak karşılaştırması: **175 sınıf bizde var (%72),
  67 canlı-özel** → 9 CRT/obfuscation artığı ayıklandı → **58 gerçek eksik
  modül** (önceki tahmin 57 idi): 4'ü MUIG donor'da (AddBuffer, CAUTOHP,
  CCustomJewelBank, CSkillDamage), **54'ü hiçbir kaynak setinde yok (sıfırdan)**.
- **2a.3:** Canlı `Sub-1\Data\` envanteri: 450 dosya (~10 MB); `Data\SPK\`
  25 SPK modül config'i; `GameServer\SPK\` 225 dosya = SPK_ToolKitMain canlı
  logları (modülün canlıda aktif olduğunun kanıtı). Config tanınırlık tablosu:
  22 bizde / 4 MUIG'de / 24 SPK-özel (hiçbir kaynakta yok).
- **2a.4:** Gerçek SPK istemcisi tespit edildi:
  `Client and Tools\Client (eski adı 1Client)\Engine.exe` (9.201.152 B, 19.09.2026) —
  ConnectIP.bmd + ServerData.bmd + Data\SPK + SPK.ini hattı; AUTOHP istemci
  UI varlığı (`Btn_AutoHp.spk`). Bizim 5.Main kaynağında SPK istemci izi
  YOK (CBGetMain/MUIG hattı) → yeni plan adımı **2d.0** (SPK istemci format
  katmanı) açıldı.
- **Kritik keşifler:** (1) `C:\AxionMu\` farklı bir fork (57 modülden hiçbirini
  içermiyor; vcruntime140**d** = debug runtime) — parite hedefi değil.
  (2) Canlı GS v100 (VS2010) toolset'li (msvcp100/msvcr100 kanıtı).
  (3) **H-006 açıldı:** Faz 1 GS "canlıyla birebir" iddiası düzeltildi —
  eşleşme MuServer'daki eski referansla; canlı GS 6.979.072 B.

**Doküman güncellemeleri**
- Yeni: `docs\06-CANLI-SISTEM-ENVANTERI.md` (tüm bulgular + yeniden üretim komutları)
- `02-YOL-HARITASI.md`: 2a.2/2a.3/2a.4 ✅; 2d.0 eklendi
- `00-PROJE-HARITASI.md`: durum panosu güncellendi (aktif görev: 2a.1)
- `03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md`: A-01/A-02 (58 modül, kesin liste),
  A-03 alt kırılım, B-01/B-02 (SPK istemci) kalemleri
- `04-HATA-GUNLUGU.md`: H-006 kaydı

**Operasyon notu (disk)**
- Oturum ortasında C: diski %100 doldu (Temp'te 6,2 GB AI/araç çöpü);
  `cmp_axion-mu-dmncms`, `axion-spot-*`, `axion-probe-*`, Cline updater
  temizlendi → 5,7 GB boşluk açıldı. Yazma hatası nedeniyle 03 dokümanı
  boşalmıştı; yeniden yazıldı ve doğrulandı (5.540 B).

**Doğrulama**
- Ham çıktılar `BuildLog\envanter\` (gs_strings_*, map_siniflar, sinif_*,
  canlida_bizde_yok, muigden_alinabilir, sifirdan_yazilacak)
- `git fsck` temiz; tüm docs boyut kontrolü yapıldı.

**Commit** — (bu kayıtla birlikte)

---

## [26.09.30] Faz 2a.1 — SPK modül envanteri (iş emri) tamamlandı

**Ne yapıldı**
- Canlı `GameServer.pdb` (27,3 MB) derin analiz edildi:
  - Gömülü linker komut satırı bulundu (`pdb_linker_cmd.txt`): VS2022
    14.44.35207 (**v143** — önceki v100 tahmini düzeltildi), /LTCG,
    SPKThemeManiFest.xml addon'u, ara yapı `D:\BuildMU\Android\ExGameServer\`.
  - Kaynak dosya yolları çıkarıldı: **367 benzersiz cpp** (277'si canlı GS
    projesi; 1'i proje dışı: `Source\Include\Math.cpp`).
  - **Canlı GS projesi 277 cpp; 218'i bizim kaynakla isim paritesinde;
    59 dosya sadece canlıda** (map'in 58 sınıf tahminini 59'a düzeltir:
    SkillDamage.cpp map deseninde yakalanamamış).
- **Canlı SPK mimarisi kesinleşti:** tüm SPK modülleri
  `GameServer\SPK\` alt klasöründe (57 cpp); 12'si bizim kaynağımızda da
  aynı adda var (canlıda bu dosyalar SPK sürümüyle EZİLİYOR: BossGuild,
  BotAlchemist, BotBuffer, ChangeClass, CustomBuyVip, CustomEventTime,
  CustomJewel, CustomRankUser, OfflineMode, Reconnect, ThuMuaDoExc, ZenDrop).
- `docs\05-SPK-MODUL-ENVANTERI.md` yazıldı: 59 modülün kategori tabloları
  (SPK_ çekirdek 20 / SPKViet sistemleri / bot / event / UI / ekonomi),
  canlı config eşlemeleri, P1/P2/P3 öncelik tanımları, MUIG transfer +
  sıfırdan yazım stratejisi, modül başına 4 adımlı doğrulama yöntemi.

**Doküman güncellemeleri**
- Yeni: `docs\05-SPK-MODUL-ENVANTERI.md`
- `02-YOL-HARITASI.md`: 2a.1 ✅ (Faz 2a'da kalan: 2a.5)
- `00-PROJE-HARITASI.md`: aktif görev 2a.5'e çekildi
- `03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md`: A-02 sayısı 59'a güncellendi

**Yeni ham veriler** (`BuildLog\envanter\`): pdb_cpp_yollari.txt (367),
pdb_obj_yollari.txt, pdb_linker_cmd.txt, canli_gs_s.txt (277),
canli_spk_s.txt (57), bizim_gs_s.txt (281), ortak_dosyalar.txt (218),
canli_ek_dosyalar.txt (59), bizim_ek_dosyalar.txt (63).

**Not** — Extractor üç denemede hazırlandı: PowerShell .NET regex'i PDB
string'inde sessiz başarısız oldu; çözüm IndexOf + geriye-yürüme (printable
run) algoritması oldu (pdb_scan.ps1).

**Commit** — (bu kayıtla birlikte)

---

## [26.09.30] Faz 2a.5 — GetMainInfo varyant kararı: TEK HAT = SPK GETENGINE

**Ne yapıldı (kanıt zinciri)**
1. `GetMain\GetEngine.ini` düz UTF-8 çıktı (önceki oturumda UTF-16 sanılmış;
   mojibake iconv hatalısından): IpAddress=45.87.120.29, IpAddressPort=44405,
   ClientVersion=1.03.34 (= Client\SPK.ini MainCode ile aynı), ClientName=Engine.exe,
   AntPort=58584, MaxAttackSpeed*=67000, DefaultFPS=24, Türkçe yorumlar
   (Axion'un kendi konfig aracı olduğu kesinleşti), MENUBUTTON_* Vietnamca
   SPK modül menü etiketleri, JewelBankTab, MaxLevel seti.
2. `Client\Data\SPK\ConnectIP.bmd` (36 B) formatı çözüldü:
   **XOR 0x20(IP string) + 0x20 pad + 4 B CRC** → decode = 45.87.120.29 =
   GetEngine.ini değeri. GetMainInfo bu dosyayı üretiyor.
3. `ServerData.bmd` (1.089.576 B): aynı gizleme tekniği + veri blokları
   (0xDF dolguları blok sınırlarını işaretliyor).
4. `GetMain\SPK_CRCFILE.ini`: GetMainInfo'nun bütünlük raporu ("Code by
   SuperHung": SPK_*.bmd CRC + FOUND/NOT FOUND listesi) — araç doğrulayıcı.
5. **KARAR (A varyantı):** SPK GetEngine hattı benimsendi; GetMainInfo
   GetEngine davranışına geliştirilecek (2d.1), Main'e SPK format okuma
   katmanı (2d.0) eklenecek; MUIG CBGetMain hattı bayrakla devre dışı.
   H-005 kararlandı (uygulama/kapanış 2d'de).

**Doküman güncellemeleri**
- `03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md`: "2a.5 KARARI" bölümü (kanıt zinciri + 4 maddelik uygulama kararı + B-01 ek kanıtları)
- `04-HATA-GUNLUGU.md`: H-005 → karar alındı
- `02-YOL-HARITASI.md`: 2a.5 ✅ → **Faz 2a TAMAMLANDI**

**Operasyon notu**
- `Client and Tools\Client (eski adı 1Client)\` → `Client and Tools\Client\` olarak yeniden
  adlandırılmış (dış müdahale; içerik aynı). Dokümanlardaki yollar güncel
  konumu ile yazılacak.

**Commit** — (bu kayıtla birlikte)

---

## [26.09.30] 2d.0 önhazırlığı — ConnectIP.bmd / ServerData.bmd format çözümlemesi

**Ne yapıldı**
- Örnek dosyalar `BuildLog\envanter\bmd\` altına arşivlendi (ConnectIP 36 B,
  ServerData 1.089.576 B).
- **ServerData.bmd XOR 0x20 KANITLANDI:** decode sonrası tam okunur —
  `Axion Mu`/`Axion Mu 2..4` (4×32 B sunucu slotu, 0x200+),
  `Engine.exe`/`AxionMu`/`Axion Mu` (0x2A0+), `Photos\Screen(...).jpg`,
  `1.03.34` + `!571Axion@Mobile` (0x4E0), 7×int32=65000 hız limitleri (0x52C),
  float 45.0 (=CameraDefault) + float 240.0, kanat kataloğu (340 B stride:
  Wing200..WingCustom15, SPK\Item\*.tga yolları), item opsiyon tablosu
  (260 B stride: "[+9] Damage: +100" formatı), LEVEL tabloları (100 B stride),
  1.510 printable string. **GetEngine.ini'nin binary aynası.**
- **ConnectIP.bmd:** 12 B IP (XOR 0x20) + 20 B pad + 4 B CRC. Standart CRC32
  ve 8 basit varyant test edildi → EŞLEŞMEDİ (özel tablo/seed). Geçici çözüm
  dokümante edildi: üreticide CRC alanına 0x00 yazılabilir, 2d.0'da
  Engine.exe crc tablosundan çözülecek.
- `docs\07-SPK-BMD-FORMAT.md` yazıldı: alan tabloları + üretici/okuyucu sözde
  kodu + 5 açık kalem (CRC varyantı, opsiyon tuple semantiği, footer, port
  konumu). Arıza notu: PS byte döngüsü 5 dk timeout → openssl AES-ECB hilesi
  (yanlış sonuç, silindi) → perl tr // ile 1 saniyede doğru decode.

**Doküman güncellemeleri**
- Yeni: `docs\07-SPK-BMD-FORMAT.md`
- Ham veri: `BuildLog\envanter\bmd\` (orijinal + decode)
- `02-YOL-HARITASI.md`: 2e.0→2d.0 tutarlılığı

**Commit** — (bu kayıtla birlikte)

---

## [26.09.30] Faz 2a.5 (tamamlama) — GetMainInfo tek modül tasarımı (08 dokümanı)

**Ne yapıldı**
- **Zincirin son halkası kanıtlandı:** `GetMain\Data\CustomWing.txt` satırları
  (Wing200-202, ConquerorWing, cape_of_death→KF_Death_clka/clkb, Wing401-405,
  WingCustom1-15, ChristmasW6 ve tga yol çiftleri) ServerData.bmd kanat
  kataloğuyla (340 B kayıtlar) BİREBİR eşleşiyor. → GetMainInfo =
  GetEngine.ini + GetMain\Data\*.txt'i .bmd'ye DERLEYEN araç (kesin).
- **Varyant karşılaştırma tablosu** (08 dokümanı §1): 10 özellikte yan yana —
  girdi/çıktı/format/istemci/kanıt durumu; A (SPK GetEngine) benimsendi,
  MUIG MainInfo bayrakla devre dışı (kaynak korunur).
- **Tek GetMainInfo tasarımı** (08 dokümanı §3): tek exe, iki mod
  (--mode:spk varsayılan / --mode:muig legacy bayraklı); yeni SPK modülü
  (GetEngineConfig, CrcPatch, ConnectIPWriter, ServerDataWriter,
  CrcFileReport, main_spk); şablon-kopya stratejisi (bilinmeyen baytlar canlı
  örnekten aynen); VMP SDK import'u kaldırılacak; 5 adımlı doğrulama
  (boyut, çapraz diff, Engine.exe kabul testi, --check modu).
- **D1-D9 görev kırılımı** ile 2d.1 iş planı hazır.

**Doküman güncellemeleri**
- Yeni: `docs\08-GETMAININFO-TASARIM.md`
- `03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md`: B-03 → tasarım tamam/2d.1
- CHANGELOG: bu kayıt

**Commit** — (bu kayıtla birlikte)

---

## [26.09.30] Faz 2a.1 (yeniden yazım) — 05 iş emri temiz ve kesin hale getirildi

**Neden**
- İlk yazımda (tek seferde heredoc) tablo ID'lerinde düzeltme izleri ve
  58/59 karışık sayılar kalmıştı; kullanıcı isteği üzerine temiz tek kaynaklı
  iş emri olarak yeniden yazıldı.

**Ne yapıldı**
- `docs\05-SPK-MODUL-ENVANTERI.md` baştan yazıldı (10,4 KB):
  - Kesin sayılar: 59 yeni dosya (45 SPK\ + 14 kök) + 12 ezilen dosya =
    **71 iş birimi**; ortak 218; sadece bizde 63.
  - **59 modülün numaralı kartları** tek tabloda: P1=19, P2=28, P3=12
    (denetim: 19+28+12=59 ✓); her kartta dosya+konum, map sınıfı,
    canlı config, kaynak stratejisi (sıfırdan / MUIG donor×3).
  - **12 ezilen dosya** ayrı bölümde (E-01..E-12) canlı davranış kanıtıyla.
  - Modül başına 6 adımlı uygulama akışı, Faz 2c kabul kriterleri,
    4 açık soru (MessLang alt birimleri, EventMainManager büyüklüğü vb.).
- Sayı tutarlılığı: 58 (map) → 59 (PDB dosya) düzeltmesi korunuyor.

**Doküman güncellemeleri**
- `05-SPK-MODUL-ENVANTERI.md`: temiz yeniden yazım
- CHANGELOG: bu kayıt

**Commit** — (bu kayıtla birlikte)
