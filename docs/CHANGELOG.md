# CHANGELOG — Axion Mu Source

> **Kural:** Projede yapılan HER değişiklik buraya kaydedilir — ne, neden,
> nasıl doğrulandı. Amaç: kalan yerin başka bir yapay zeka / geliştirici
> tarafından sohbet bağlamı olmadan anlaşılması.
> Format: [YY.AA.GG] Başlık → Değişen dosyalar → Neden → Doğrulama → Commit.

---

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
  `Client and Tools\1Client\Engine.exe` (9.201.152 B, 19.09.2026) —
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
- `Client and Tools\1Client\` → `Client and Tools\Client\` olarak yeniden
  adlandırılmış (dış müdahale; içerik aynı). Dokümanlardaki yollar güncel
  konumu ile yazılacak.

**Commit** — (bu kayıtla birlikte)
