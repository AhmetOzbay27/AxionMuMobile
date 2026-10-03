# 04 — HATA GÜNLÜĞÜ

> Açık hata yapmayın: bulduğunuz her hata önce buraya AÇILIR, çözülünce
> KAPATILIR (kök neden + çözüm + doğrulama yöntemi ile). Kapananlar
> sayfada kalır — bilgi bankası görevi görür.

Durum: 🔴 AÇIK · 🟢 ÇÖZÜLDÜ · 🟡 ERTELENDİ

---

## AÇIK HATALAR

| ID | Tarih | Bileşen | Hata | Durum |
|----|-------|---------|------|-------|
| H-018 | 03.10.2026 | GameServer ↔ Main (protokol) | **Viewport paket düzeni uyuşmazlığı:** GS `PMSG_VIEWPORT_PLAYER` / `PMSG_VIEWPORT_CHANGE` (`Viewport.h`) `MuunItem[2]` alanını `attribute`'tan SONRA (upstream 8.03 bloğu) koyar; istemcinin karşılığı olan `PCREATE_CHARACTER` (`WSclient.h:564`) ise `Path`'ten SONRA, `s_BuffCount`'tan ÖNCE konumlandırır ve `Equipment[]`/`attribute`/`level`/`MaxHP`/`CurHP` alanlarını **içermez**. İki yazıcı da `info.MuunItem`'a yazıyor ama **farklı veri** koyuyor: HAISLOTRING bloğu `EquipInventory[EQUIPMENT_PET_1]` (pet ekipmanı), 803 bloğu `MuunInventory[0]` (muun envanteri). İstemci okuduğu alanı `m_dwPetType = Type + 1171` olarak **pet modeli** için kullanıyor | 🔴 **AÇIK — doğrulama gerekiyor** |

### H-018 hakkında bilinenler ve bu turdaki karar

Bu turda **derleme kırılması** giderildi (aşağıda), ancak **protokol doğruluğu
derinlemesine doğrulanmadı** — çünkü doğrulamanın tek güvenilir yolu canlı pakette
izleme (Faz 3.3).

| Konu | Durum |
|---|---|
| Derleme | ✅ Çözüldü — `Viewport.h`'te iki blok da `HAISLOTRING && GAMESERVER_UPDATE<701` / `GAMESERVER_UPDATE>=803` ile karşılıklı dışlandı. **Paket boyutu her konfigürasyonda aynı (tek 2 bayt alan)** |
| Yazıcı tutarlılığı | ✅ `Viewport.cpp` 4 yazıcı bloğu da aynı koşulla eşitlendi; `>=701`'de `MuunInventory[0]`, daha eski sürümlerde pet ekipmanı gönderilir |
| GS ↔ istemci düzen eşleşmesi | ❌ **Doğrulanmadı** — yukarıdaki tablo |
| Pet ekipmanı **ve** muun envanteri ayrı ayrı gönderilmeli mi? | ❓ Bilinmiyor. İstemci tek alan okuyor; ikinci alanı okuyorsa doğru yerleşim başka olmalı |
| Karar verilecek yer | **Faz 3.3** — canlı GS ile paket yakalama (aynı karakter için `PMSG_VIEWPORT_PLAYER` baytları canlıyla karşılaştırılacak) |

> **Neden "tahminle çözülmedi":** iki seçenek de tel üzerinde farklı sonuç verir.
> Alanı teke indirmek (yapılan) paketi derlenebilir ve boyut sabit tutar; ama
> pet ekipmanı bilgisi ile muun envanteri bilgisinden hangisinin canlıda
> gönderildiği ancak canlı izleme ile kesinleşir. Bu yüzden risk **gizlenmedi**,
> `docs/04` açık hata listesine alındı.



**Kapanış notları (03.10.2026, 2e.5 turu):**
- **H-005** 🟢 → uygulama tamamlandı: SPK `GetEngine` hattı benimsendi ve **2d.0/2d.1**
  ile bitti (SPKData katmanı + canlı üreticiyle bayt-birebir ConnectIP/ServerData üretimi,
  docs/19/20); 2e.5'te doğrulama kanıtı aynı hat üzerinden alındı (ConnectIP 36 B, port
  0x20, canlı 44405/55858 eşleşmesi, `make_connectip.js`).
- **H-006** 🟢 → anlayış düzeltmesi kalıcı: parite hedefi **işlevsel**, boyut değil; GS
  tarafında canlı çift (6.979.072 B, v100 toolset) referans alınarak 2c/2e modülleri
  işlendi; docs/30 parite manifesti ekseni tanımına bu kural yazıldı.
- **H-007** 🟢 → kapandı: AUTOHP `case 0x35` donör paritesiyle korundu, HAISLOTRING
  tamircisi `Protocol.cpp` `#if(0)` (ölü kod, kanıt notuyla); docs/32 opcode tahsis
  tablosunda 0x35 çakışması gerekçe olarak kayıtlı ve yeni tahsisler kullanılan
  opcode'lardan kaçınılarak yapıldı.

## ÇÖZÜLEN HATALAR (arşiv)

| ID | Tarih | Bileşen | Hata | Kök neden | Çözüm | Doğrulama |
|----|-------|---------|------|-----------|-------|-----------|
| H-004 | 30.09.2026 | Main (Client) | Kaynak içine gömülü IP `171.235.182.88` canlıdaki `192.168.0.150` ile uyuşmuyor; config'ten okuma düzeni net değil | Canlı değer varsayımı yanlıştı: gerçek hedef `ConnectIP.bmd`'den okunuyor — `45.87.120.29`, port 44405 (AntiPort 55858) | 3 dosyada gömülü değer hizalandı (`Winmain.cpp`, `GameConfigConstants.h`, `SceneCore.cpp`); `MainLoad` IP+portları ConnectIP'ten uygular; SPK yoksa derleme varsayılanı | 2e.1; derlenen PC hattında `grep 171.235.182.88` = 0 (Winmain/GameConfigConstants/SceneCore); yeni exe'de `45.87.120.29` gömülü (`BuildLog\2e2` string taraması). Derleme dışı mobil dosyalarda (android_main.cpp 3 kod, LauncherHelper.h örnek yorumu) eski **mobil** fallback (`171.235.182.88:63000`) bilinçli duruyor — hiçbir vcxproj'da yok, exe'ye girmiyor |
| H-008 | 02.10.2026 | Main (Client) | `GetGPUUse = 1` (donör "Test GPU") `nvapi.dll` arıyor; GPU'suz/VM makinede `MessageBox` → istemci açılışta kilitleniyor (canlı `Engine.exe`'de nvapi bağımlılığı **yok**) | Donör kalıntısı derleme anahtarı; paket paritesinde nvapi.dll taşınmıyor | `Defined_Global.h`: `GetGPUUse 0` (gerekçe yorumu) | Yeni exe'de `nvapi.dll` stringi yok; paket koşusunda artık kilitlenme yok |
| H-009 | 02.10.2026 | Main (Client) / paket | `wzAudio.dll`'in bağımlılıkları (**`vorbisfile.dll`**, `ogg.dll`) pakette yoksa Windows yükleyicisi ölümcül sistem hatası veriyor: kutu **csrss.exe**'ye ait (süreç pencerelerini numaralandıran test bunu görmez), ana iş parçacığı `NtRaiseHardError`'da bekler | Paket içeriği eksikti: DLL kapanışı çıkarılmamıştı | `BuildLog\2e2\dep_closure.js` ile kapanış çıkarıldı, DLL'ler pakete eklendi (kurulum betiği) | run4+: pencere açılıyor; teşhis yöntemi docs/22 §3.4 |
| H-010 | 02.10.2026 | Main (Client) | `Data\Local\Mix.bmd` ölümcül kontrolü (`CMixRecipeMgr` ctor → `exit(0)`); canlı SPK paketinde o yol yok → istemci pakette açılamıyordu | Kalıp farkı: canlı Engine yolu `Data\SPK\Config%s\Mix.bmd` | `MixMgr`: `OpenRecipeFileSpkFirst()` (SPK\Config → Local → SPK paketinde fatal değil) | Canlı `Config\Mix.bmd` 87.960 B = 14*4 + 134*656 (format birebir); run5+ istemci kendi penceresini açtı |
| H-013 | 02.10.2026 | Süreç (2d.2 kabul testi) | **İçerik doğrulaması yetersizdi:** kabul testinde "pencere açıldı + TCP bağlandı" görüntüsü içerik kanıtı sayıldı; negatif kontrolde (bozuk ServerData) canlı istemci yine pencere açıp bağlandı | Runtime davranışı tek başına içerik doğrulamaz; ilk koşuda dosya-seviyesi karşılaştırma yapılmamıştı | Dosya-seviyesi bayt karşılaştırması (2d.1, docs/20) birincil kanıt kabul edildi; kabul testine negatif kontrol koşusu eklendi | run2 (üretim) vs run3 (bozuk) karşılaştırması; docs/20 md5-birebir tablosu; docs/21 §3.5 |
| H-011 | 02.10.2026 | Main (Client) | **Eksik bitmap → erişim ihlali:** pakette olmayan bir bitmap (`Data\Interface\gamecensorship_*.tga`, `Data\Effect\*`, `Data\Skill\*` vb.) yüklenemeyince `CSprite::Create` (Sprite.cpp:55) boş sprite ile AV (0xC0000005) veriyordu | Boş doku denetimi yoktu; `FindTexture` başarısız olsa da geçerli `m_nTexID` ile sprite kurulmaya çalışılıyordu | `Sprite.cpp` `CSprite::Create`: doku yoksa log + `m_nTexID = -1` (temiz kurulum dalı işler) | run12/13/14: onlarca `LoadBitmap Failed` satırına rağmen AV/çökme yok (run9'daki AV'nin tersi) |
| H-012 | 02.10.2026 | Main (Client) | **MUIG ↔ SPK varlık düzeni farkı:** istemci `Data\Local\*` (ve `Data\Local\<Lang>\<Ad>_<Lang>.bmd`) bekliyordu; canlı SPK paketi aynı tabloları `Data\SPK\Config\*` altında taşıyor | İstemci sabitleri canlı paket düzeninden önce yazılmıştı; kalıcı çözümleyici yoktu (geçici: deploy betiğinde yol eşlemesi) | `SPKAsset.cpp` (`SPK_ResolveAssetPath`/`SPK_AssetExists`) 20+ çağrı noktasına bağlandı; deploy eşlemesi artık yalnız yedek | run11→run14: tooltip/quest/pet/slide/… yükleyicileri SPK'dan çözüyor; run14 diyalogsuz (docs/24) |
| H-014 | 02.10.2026 | Main (Client) | **VIPCharRank fatal diyalogu:** `Data\Custom\VIPCharRank.txt` yoksa `CBInterfaceVIPChar` PC dalı MemScript hatası + `ErrorMessageBox` → `ExitProcess` | Dosya canlı pakette de yok (audit `LIVE-`); Android dalı zaten loglayıp dönüyordu, PC dalı fatal'dı | PC dalında dosya varlığı guard'ı: yoksa log + `return` (fatal değil) | run13/14: diyalog yok (run12'de `[Data\Custom\VIPCharRank.txt] Could not open file` görülmüştü) |
| H-016 | 03.10.2026 | Test düzeneği (Faz 3) | **İstemci `127.0.0.1` hedefini kasten reddediyor:** `CWsctlc::Connect` (WSctlc.cpp:230-234) loopback adresinde hiç SYN göndermeden `FALSE` döner; `CreateSocket` `MESSAGE_SERVER_LOST` popup'ı açar. ConnectIP `127.0.0.1` yapılan run18/19/20'de ne TCP izleyici ne 63000'deki trap dinleyici **tek paket** görmedi | Canlı istemcinin kendi koruması (parite gereği korunur); test düzeneği adresi yanlıştı | Test hedefi makinenin gerçek IPv4'ü yapıldı (`45.87.120.29:63000`); `BuildLog\2e4\make_connectip.js` üreticisi kullanıldı | run21/22/26: bağlantı t≈4,4 s'de **ESTABLISHED** ve sunucu seçim ekranı; kural docs/25 §4.2 |
| H-017 | 03.10.2026 | Test düzeneği (Faz 3) | **Bağlantısı kesilmiş oturumda UI otomasyonu imkânsız:** ajan süreci `qwinsta` session 2 `Disc`; `GetForegroundWindow() == 0`. İstemci girdisi `g_bWndActive` koşuluna bağlı (Input.cpp) ve pencere aktif hale getirilemiyor → `PostMessage` (run22), gerçek fare (`mouse_event`, run23), ALT+`SetForegroundWindow` + `WM_ACTIVATE`/`WM_ACTIVATEAPP` (run24), `SetWindowPos(TOPMOST)` + diyagnostik (run25) hiçbiri sunucu butonunu işletmedi | Oturumda etkin pencere kavramı yok (disconnected RDP oturumu) | Sınır kabul edildi: sunucu seç → GS/login adımı **etkileşimli masaüstü oturumunda** koşulacak; otomasyon betikleri (`run22_e2e.ps1`) hazır bırakıldı | run22-25: `foreground=false`, GS'e bağlantı yok, ekran değişmedi (docs/25 §4.3) |
| H-015 | 02.10.2026 | Main (Client) | **Harita/nesne yol uyumsuzluğu:** kaynak `Data\World%d` + `Data\Object*` istiyordu; canlı paket `Data\Map\World%d` + `Data\Map\Object*` taşıyor → `EncTerrain74.map file corrupted1 (74/-1)` ölümcül diyalogu | Canlı Engine stringleri `Map\World%d` + `Data\Map\Object%d\` düzeninde üretilmiş; bizim literaller MUIG düzeninde kalmıştı | WorldName üretimi 4 dosyada `Map\World%d`; `AccessModel`/`OpenTexture`/`LoadBitmap` merkezi dönüşümleri; Minimap SPK-first | run14: harita yüklendi, diyalog yok, istemci canlı sunucuya bağlanma aşamasına ulaştı (docs/24 §6-7) |
| H-001 | 30.09.2026 | GameServer | `Resource.h` UTF-16 hasarlı; `IDM_INVASION12+` tanımları yok → C2051 | Bozuk kodlama kaybı | UTF-8 dönüşümü + 101 orijinal tanım kurtarıldı, eksik IDM_/ID_FAKEONLINE_ tanımları eklendi | GS `Release_EX603\|Win32` derlendi, boyut 10.689.536 B (canlı PDB ile birebir) |
| H-002 | 30.09.2026 | Main | C2535 `GetMessageA` redefinition (CustomMessage.h) | windows.h `#define GetMessage GetMessageA` makrosu sınıf üyesini genişletiyor | Sınıftan ÖNCE `#undef GetMessage` + sınıfta `GetMessageA` alias; `Winmain.cpp` mesaj döngüsünde açık `GetMessageA` | Main derlemesi bu hatasız geçti |
| H-003 | 30.09.2026 | Main | C3861 `min`/`max` + `Uint64` + `MU_MobilePerfNow` + LNK2001 `g_mainScenePerfSnapshot`/`TERRAIN_ATTRIBUTE` | a) yanlış eklenen `NOMINMAX` (donor ortam makro bekliyor) b) PC dalında SDL tipi yok c) MobileTime Win32 dali yok d) vcxproj'da ScenePerfTelemetry.cpp eksik e) inline tanım başka TU'dan çağrılıyor | NOMINMAX kaldırıldı; `Uint64` typedef eklendi; `Platform/MobileTime.h` QPC dali; ScenePerfTelemetry.cpp vcxproj'a; TERRAIN_ATTRIBUTE dış bağlantı | Main.exe 12.023.808 B derlendi; canlı ile ~%99,7 string paritesi |

### Hata ekleme şablonu
```
| H-XXX | TT.AA.YYYY | Bileşen | Hata özeti | Durum |
```
Çözümde arşiv tablosuna şu sütunlarla taşınır: Kök neden / Çözüm / Doğrulama.
