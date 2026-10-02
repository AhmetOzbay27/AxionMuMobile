# 22 — 2e.1 GÖMÜLÜ IP/CONFIG HİZALAMA + 2e.2 PAKET KURULUMU

> Adım: **2e.1 + 2e.2** ([02-YOL-HARITASI.md](02-YOL-HARITASI.md) Faz 2e).
> Bu rapor 02.10.2026 akşamı yapılan kod + derleme + **fiili paket çalıştırma**
> testinin kanıtlarını toplar. 2d.2 testi ayrı: [21-2D2-KABUL-TESTI.md](21-2D2-KABUL-TESTI.md).

## 1. Özet

- **2e.1 tamam:** kaynakta gömülü `171.235.182.88` kaldırıldı; derleme varsayılanı
  canlı SPK paketiyle aynı (**`45.87.120.29:44405`**), çalışma anında **ConnectIP.bmd
  ezer** (IP + IP portu + AntiPort).
- **2e.2 tamam (asgari varlık setiyle):** `Main.exe` + DLL kapanışı (`wzAudio.dll` →
  `ogg.dll` + `vorbisfile.dll`) + `SPK.ini` + `Data\SPK\{ConnectIP,ServerData}.bmd` +
  `Data\SPK\Config` + MUIG→SPK yol eşlemesi kuruldu; **istemci kendi penceresini
  (`Axion Mu`) SPK paket düzeninde açtı** (kanıt: `BuildLog\2e2\shots\`).
  Kalan: tam `Data` ağacı (disk %100) + **SPK-first varlık çözümleme katmanı** (2e.4).
- Yol boyunca 4 **gerçek dağıtım hatası** bulundu/çözüldü (H-004, H-008, H-009, H-010)
  ve 2 dayanıklılık işi açıldı (H-011 CSprite AV, H-012 SPK-first eşleme katmanı).

## 2. 2e.1 — Gömülü IP / config hizalama

### 2.1 Ölçüm (canlı kanıt)

| Kanıt | Değer |
|-------|-------|
| `Client\Data\SPK\ConnectIP.bmd` ilk 12 bayt (XOR 0x20) | `45.87.120.29` |
| Aynı dosya 0x20/0x22 (u16 LE) | `44405` / `55858` |
| `Client and Tools\GetMain\GetEngine.ini` | `IpAddressPort=44405`, `AntiPort=55858` |
| 2d.2 run2/run3 TCP hedefi | `45.87.120.29:44405` (+55858) |

→ **Canlı istemcinin gerçek hedefi `45.87.120.29:44405`**'tir. (docs/03 B-04 satırındaki
"canlı değer 192.168.0.150" notu o dönemki varsayımdı; canlı paketteki ConnectIP bunu
çürüttü. Bu düzeltme 04-HATA-GUNLUGU H-004 kapanışına işlendi.)

### 2.2 Değişiklikler

| Dosya | Satır | Ne |
|-------|-------|----|
| `Source\5.Main\source\Winmain.cpp` | 1567-1568 | `szServerIpAddress = "45.87.120.29"`, `g_ServerPort = 44405` (**derleme varsayılanı**) |
| `Source\5.Main\source\GameConfig\GameConfigConstants.h` | 58-59 | `CfgDefaultServerIP = L"45.87.120.29"`, port 44405 |
| `Source\5.Main\source\Scenes\SceneCore.cpp` | 56 | aynı değerler + "bu dosya vcxproj'da YOK, derlenmez" notu |
| `Source\5.Main\source\MainLoad.cpp` | 60-95 | SPK alan birleşimi: IP + **IpAddressPort (0x20)** + AntiPort (0x22) ConnectIP'ten; anahtar sırası düzeltildi (bkz. §3.3) |

Çalışma-anı önceliği: `Data\SPK\ConnectIP.bmd` → (yoksa) derleme varsayılanı.
Komut satırı IP:port override'ı (`GetConnectServerInfo`) aynen korunur.

## 3. 2e.2 — Paket kurulumu

### 3.1 Canlı paket düzeni (referans)

Kök (17 dosya): `Engine.exe`, `Launcher.exe`, `SPK.ini`, `iU.spk`, `GameVoice.dll`,
`OpenGL32.dll`, `QeffectsGL.ini`, `glew32.dll`, `msvcp100(.d).dll`, `msvcr100(.d).dll`,
`ntdll.dll`, `ogg.dll`, `vorbisfile.dll`, `wzAudio.dll` + `Data\` + `Scripts\`.
**Canlı pakette OLMAYANLAR (2e.2'nin özü):** `APICB.dll`, `FreeImage.dll`, `nvapi.dll`.

### 3.2 İçe aktarma (import) düzeltmeleri

| Sorun | Çözüm | Kanıt |
|-------|-------|-------|
| `APICB.dll` importu (canlı pakette yok) | `APICB.{h,cpp}` `#if (CB_ANTIHACKGGNEW)` (bizde 0) → yerel no-op; `#pragma comment(lib/APICB.dll)` o blokta | yeni exe importlarında `apicb.dll` **yok** (`BuildLog\2e2\main_imports_build3.txt`) |
| `FreeImage.dll` importu (canlı pakette yok) | `CB_USE_FREEIMAGE = 0`; `ConvertToJPEG` GDI+ ile (gdiplus.dll = Windows bileşeni) | importlarında `freeimage.dll` **yok**; `gdiplus.dll` var (sistem) |
| CRT bağımlılığı | v143 statik (exe'de `msvcp140/vcruntime140` importu yok) | import listesi |

### 3.3 Kod düzeltmeleri (paket çalıştırması sırasında çıkanlar)

| # | Bulgu | Düzeltme |
|---|-------|----------|
| 1 | **`GetGPUUse = 1`** (donör "Test GPU" kalıntısı): NVIDIA `nvapi.dll` yoksa `MessageBox` → istemci açılışta kilitleniyordu (canlı `Engine.exe`'de nvapi bağımlılığı **yok**) | `Defined_Global.h`: `GetGPUUse 0` + gerekçe yorumu |
| 2 | `Data\Local\Mix.bmd` **ölümcül** kontrol (constructor'da `exit(0)`); canlı SPK paketinde o yol yok — karşılığı `Data\SPK\Config\Mix.bmd` (canlı `Engine.exe` stringi: `Data\SPK\Config%s\Mix.bmd`) | `MixMgr.{h,cpp}`: `OpenRecipeFileSpkFirst()` — SPK\Config → Local → (SPK paketinde değilse) fatal değil, boş reçete ile devam |
| 3 | `gProtect.LoadEncDec()` SPK alan birleşiminden **önce** çağrılıyordu (CustomerName+ClientSerial SPK'dan geliyorsa yanlış anahtar) | `MainLoad.cpp`: `LoadEncDec()` birleşimden **sonra** |
| 4 | GS port aralığı: `CBGetMain.bin` yoksa `GSPortMin/Max = 0` → `CheckSocketPort` tüm portları reddeder | `SPKData`: `[SPK] GSPortMin/GSPortMax` (opsiyonel) + yoksa `SPK_DEFAULT_GS_PORT_MIN/MAX` (55901-55999) |
| 5 | FPS: SPK `ServerData 0x55C` ham değer (240.0), MUIG yolu /10 idi | `SPKData`/`MainLoad`: SPK modunda `FpsLimit = (DWORD)m_DefaultFps` |

### 3.4 DLL kapanışı (paket içeriği)

`Main.exe` importları: 23 DLL — hepsi **sistem** (IMM32, DSOUND, OPENGL32, GLU32, WINMM,
WS2_32, VERSION, KERNEL32, USER32, GDI32, ADVAPI32, SHELL32, ole32, CRYPT32, WLDAP32,
Normaliz, gdiplus, WININET, urlmon, SHLWAPI, PSAPI, dbghelp) + **paketlenmesi gereken
tek zincir**:

```
wzAudio.dll  →  ogg.dll  →  vorbisfile.dll
```

Bağımlılık kapanışı betiği: `BuildLog\2e2\dep_closure.js` (çıktı: `dep_closure.txt`).
**Ders:** `vorbisfile.dll` pakette yoksa Windows yükleyicisi **süreç içinde pencere
açmadan** ölümcül sistem hatası verir — kutu `csrss.exe`'ye aittir, bu yüzden süreç
pencerelerini numaralandıran test koşucuları onu **görmez**. Teşhis yolu (kayıt için):
ana iş parçacığı `NtRaiseHardError` içinde bekliyordu (`Win32_PerfFormattedData_PerfProc_Thread`
ile `ThreadWaitReason=UserRequest`), masaüstündeki **tüm** pencereler taranınca
`Main.exe - Sistem Hatası / Kod yürütülmesi devam edemiyor çünkü vorbisfile.dll bulunamadı`
kutusu bulundu. Betikler: `BuildLog\2e2\{ps_walk.ps1, ps_stackwalk.ps1, ps_code_scan.ps1}`.

### 3.5 Paket kurulum betiği

`BuildLog\2e2\deploy_spk_package.sh` (canlıya dokunmaz, `BuildLog\2e2\deploy\` üretir):

1. `Main.exe` + `SPK.ini` + `wzAudio.dll` + `ogg.dll` + `vorbisfile.dll`
2. `Data\SPK\{ConnectIP,ServerData}.bmd` — **2d.2 üretimimiz**
3. `Data\SPK\Config\` — canlı paketten (31 dosya, 2,6 MB)
4. `Data\Local\` — canlı paketteki ek dosyalar (CBGetMain.bin / CBTextInfo.bin **yok**)
5. **MUIG→SPK yol eşlemesi:** `Data\SPK\Config\<Ad>.bmd` → `Data\Local\<Ad>.bmd` **ve**
   `Data\Local\Eng\<Ad>_Eng.bmd` (istemci adları `Data\Local\<Lang>\<Ad>_<Lang>.bmd`
   kalıbıyla kuruyor); özel: `Config\Mix.bmd`, `Config\Gate.bmd`, `Config\Slide.bmd`
6. `--with-assets` ile görsel set: `Interface` + `Logo` (+ `Player` denenebilir),
   `Data\Custom` yalnız kök config dosyaları

### 3.6 Çalıştırma kanıtı (02.10.2026 18:5x)

| Koşu | Paket | Pencere | Diyalog / çıkış |
|------|-------|---------|-----------------|
| run1 | DLL'siz (yalnız wzAudio) | — | pencere yok, CPU %0, `NtRaiseHardError` (vorbisfile) |
| run4 | +ogg +vorbisfile | `Axion Mu` (t≈0.8 s) | `Data\Local\Mix.bmd - File not exist.` |
| run5 | +`Data\SPK\Config` | `Axion Mu` | `Data\Local\Eng\Slide_Eng.bmd - File not exist.` |
| run6 | +dosya bazlı eşleme | `Axion Mu` | `JewelOfHarmonyOption.bmd && JewelOfHarmonySmelt.bmd file not found.` |
| run7 | +genel eşleme (31 dosya) | `Axion Mu` (t≈1.5-2.7 s) | diyalog yok; `Data\Interface\*` eksik → **`CSprite::Create` AV (0xC0000005)** |
| **run8** | +`Interface`+`Logo` | **`Axion Mu`** | `Player.bmd file does not exist.` |
| **run9** | +`Player` | **`Axion Mu`** (t≈0.8-2.9 s) | diyalog yok; `Data\Effect\Skill\Monster\Item\NPC` eksik → AV (aynı aile) |

Sonuç dosyaları: `BuildLog\2e2\results\run*.json`; ekran görüntüleri
`BuildLog\2e2\shots\`; istemcinin **kendi** günlükleri
`BuildLog\2e2\evidence\{KEN_run9.txt, STACK_ERROR\stack_20261002_1846.log}`.

İstemci kendi çağrı zincirini de yazdı (STACK_ERROR):

```
Sprite.cpp(55) CSprite::Create ← GameCensorship.cpp(39) CGameCensorship::Create ←
UIMng.cpp(153) ← ZzzScene.cpp(450) WebzenScene ← MainLoop(Winmain.cpp 1626) ← WinMain(2064)
```

→ Yani **bizim istemci kodu çalışıyor**; çökme, pakete henüz konmamış bir bitmap'in
boş sprite üretmesinden geliyor (H-011).

## 4. Sınırlar (dürüst durum)

1. **Disk:** C: **%100 dolu** (test anında ~100 MB boş). Canlı `Data` ağacı 1,6 GB;
   tam kopya **mümkün olmadı** (kopya denemesi `No space left on device` ile durdu).
   Bu yüzden pakete yalnız `Interface`+`Logo`+`Player`+`Data\SPK\Config` kondu ve
   istemci eksik ailelerin bitmap'lerinde çöküyor. Tam paket testi için önce yer açılmalı
   (adaylar: `BuildLog\5Main` 161 MB, `BuildLog\envanter` 117 MB, `BuildLog\2e2` 100 MB,
   `BuildLog\4GS` 81 MB, `C:\temp`, `C:\AxionMuSEASON 20 STABIL` 352 MB).
2. **MUIG ↔ SPK varlık düzeni farkı (2e.4):** `Main.exe` 21 `Data\...` yol sabitinin
   yalnız 5'i canlı pakette aynı yolda var; 12 tablo canlıda **`Data\SPK\Config\<Ad>.bmd`**
   olarak duruyor. Betikteki eşleme geçici çözümdür; kalıcı çözüm istemciye
   **SPK-first yol çözümleyici** eklemektir (audit: `BuildLog\2e2\asset_audit.txt`).
3. `JewelOfHarmonySmelt*.bmd` canlı pakette **hiçbir biçimde yok** → bu tabloyu isteyen
   UI canlı istemcide ya kapalı ya da farklı kaynaktan besleniyor (araştırma kalemi).
4. `SPK_CRCFILE.ini` içindeki `SPK_MEXE` (exe CRC) hâlâ **Engine.exe**'yi işaret ediyor;
   `Main.exe` pakete girerse D6 raporunun yeniden üretilmesi gerekir (tek satır: GetMainInfo
   `--report`).

## 5. Dosya listesi

- Kaynak (2e.1+2e.2): `Source\5.Main\source\{SPKData.h,SPKData.cpp,MainLoad.cpp,APICB.h,
  APICB.cpp,Defined_Global.h,stdafx.h,CB_AutoNapGame.cpp,Winmain.cpp,MixMgr.h,MixMgr.cpp,
  GameConfig\GameConfigConstants.h,Scenes\SceneCore.cpp}`
- Derleme: `ClientFile\Main.exe` — **12.026.880 B, md5 `71b008ad6d1d8e9e08c546859401dc92`**
  (`Global Release|Win32`, v143; yalnız LNK4099 uyarıları)
- Kanıt araçları: `BuildLog\2e2\{deploy_spk_package.sh, dep_closure.js, pe_imports.js,
  asset_audit.js, dbgview.ps1, ps_walk.ps1, ps_stackwalk.ps1, ps_code_scan.ps1,
  run_client_test2.ps1}`
