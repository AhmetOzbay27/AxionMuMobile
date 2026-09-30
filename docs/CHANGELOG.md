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
