# 19 — SPK İSTEMCİ FORMAT KATMANI (2d.0) UYGULAMASI

Tarih: 2026-10-02 (13:0x–13:3x oturumu)
Kapsam isteği: "2d.0 (2a.4'te açıldı) SPK istemci format katmanı: 5.Main'e
[ConnectIP.bmd / ServerData.bmd / SPK.ini / Data\SPK okuma desteği eklenmesi;
referans binary `Client\Engine.exe`]" (docs/02 Faz 2d — "2d.1'den önce yapılır;
istemci, SPK sunucuyla aynı veri hattını konuşmadan parite testi mümkün değil").

> Kural: docs/12 §4 — parite kararlarında canlı kanıt donor'un önünde. Bu modülde
> donor yok; format zaten docs/07'de belgelenmişti, ofset düzeltmeleri canlı
> dosyadan yapıldı.

---

## 1. Sonuç özeti

| # | Kalem | Sonuç |
|---|-------|-------|
| 1 | Modül dosyası | `Source\5.Main\source\SPKData.{h,cpp}` — `CSPKData` + global `gSPKData` (87 + 244 satır) |
| 2 | Kapsam | ConnectIP.bmd (36 B, XOR 0x20, 12 B IP), ServerData.bmd (1.089.576 B, XOR 0x20, header alanları), SPK.ini ([Version]/[FontConfig]/[SPK]), `Data\SPK` varlık + yol yardımcıları |
| 3 | Entegrasyon | `MainLoad::Load()` — `LoadEncDec()` sonrası, `CheckPluginFile()` öncesi (MainLoad.cpp ~satır 55–82) |
| 4 | Fallback | Dosya yok/bozuksa ilgili bayrak `false` kalır; MUIG (`CBGetMain.bin`) hattı korunur. IP her zaman override; Version/Serial koşulsuz; kimlik/hız alanları yalnız doluysa yazılır |
| 5 | **Ofset düzeltmesi** | Hız bloğu **0x530–0x54B** (7×int32=65000; eski tahmin 0x52C), kamera **0x558** float 45.0, FPS **0x55C** float 240.0 → 24.0 (×10) — canlı dosya hex dökümü kanıtı |
| 6 | Derleme | `Global Release` · v143 · Win32 → **0 hata**; Main.exe **12.027.904 B**, md5 `39f2af32f35114241c7feb586cdd92ad` (02.10 13:30). Obj disasm: `530h/558h/55Ch` var, eski `52Ch/55Ah` yok |
| 7 | Test | Standalone koşucu + **gerçek proje obj'si link testi** (`BuildLog\5Main\spktest\`): canlı dosyalarla tüm alanlar doğru (§4); olmayan dizin → `Load=false`, EXIT=1 |
| 8 | Exe kanıtı | `grep -a` ile Main.exe'de `ConnectIP.bmd`, `ServerData.bmd`, `BODY_X20`, `MainCode` literalleri (yeni kod link'te) |

**Bekleyen:** Engine.exe kabul testi (üretilen dosyaların canlı `Client\Data\SPK\` ile
açılışı) → 2d.2; ConnectIP CRC algoritması hâlâ açık (docs/07 §6/1 — okuyucu CRC'yi
atlar); kanat/item/LEVEL katalogları bu katmanın dışında (B-02 varlık katmanı).

---

## 2. Canlı kanıt

### 2.1 Kullanılan canlı dosyalar (bayt-birebir kopyalar test edildi)

| Dosya | Boyut | md5 | Not |
|-------|-------|-----|-----|
| `Client\Data\SPK\ConnectIP.bmd` | 36 | `8ac74a5b79244943ab5607d408049353` | decode: `45.87.120.29` |
| `Client\Data\SPK\ServerData.bmd` | 1.089.576 | `53982bf8423f731cef2df6de714e78ff` | header §2.3 |
| `Client\SPK.ini` | 256 | `763ee1123dc19f6b8a6ed5a46e584b68` | 17 satır, CRLF |

Test kopyaları: `BuildLog\5Main\spktest\Data\SPK\` (md5'ler canlıyla aynı).

### 2.2 SPK.ini canlı içerik (okunanlar)

```
[Version]    MainCode = 1.03.34
[FontConfig] FontName=Arial, FontWeight=0, FontHeight=16, FontAlias=3,
             FpsWingMotion=1, Wide=0, WindowsSD=1, Resolution=5, OpenGL4=0, Lang=En
[AudioConfig] MusicVolumeLevel=1
[SPK]        BODY_X20 = 13
```

Okunanlar: MainCode, FontName, FontHeight, Resolution, Lang, BODY_X20.
Not: canlı ini'de `MainCode` = ServerData `ClientVersion` = `1.03.34` → `m_VersionMismatch=0`.

### 2.3 ServerData.bmd header — doğrulanan ofsetler ve düzeltme

Canlı dosya node hex dökümüyle (decode) doğrulandı; docs/07'deki iki tahmin yanlıştı:

| Offset (decode) | İçerik | Canlı değer | Bizim alan |
|-----------------|--------|-------------|------------|
| 0x200+32×i | ServerName[4] | `Axion Mu` … `Axion Mu 4` | `m_ServerName[4][33]` |
| 0x2A0 | ClientExeName[32] | `Engine.exe` | `m_ClientName` |
| 0x2C0 | CustomerName[32] | `AxionMu` | `m_CustomerName` |
| 0x2E0 | WindowName[32] | `Axion Mu` | `m_WindowName` |
| 0x3E0 | ScreenShotPath[64] | `Photos\Screen(%02d_%02d-%02d-%02d)-%04d.jpg` | `m_ScreenShotPath` |
| 0x4E0 | Version[8] | `1.03.34` | `m_ClientVersion` |
| 0x4E8 | Serial[16] | `!571Axion@Mobile` | `m_ClientSerial` |
| **0x530–0x54B** | 7 × int32 = 65000 | sınıf hız limitleri | `m_MaxAttackSpeed[7]` |
| **0x558** | float32 = 45.0 | kamera varsayılanı | `m_CameraDefault` |
| **0x55C** | float32 = 240.0 | FPS ×10 (→ 24.0) | `m_DefaultFps` |

> Önceki docs/07 tahminleri: `0x52C` (7×int32) ve `~0x55A` (45.0+240.0 birleşik).
> Gerçek: hızlar 0x530'da başlar (7. dword 0x548'de biter = 0x54B son bayt);
> 0x558 ve 0x55C ayrı iki float. docs/07 "Rev. 02.10.2026" notuyla güncellendi.

---

## 3. Okuyucu tasarımı

### 3.1 API (`SPKData.h`)

```
CSPKData gSPKData;
bool Load(char* path);            // ".\Data\SPK" — en az bir dosya okunursa true
// bayraklar: m_DataSPKExists, m_ConnectIPLoaded, m_ServerDataLoaded, m_SPKIniLoaded, m_VersionMismatch
// alanlar: IP / 4 sunucu adı / 5 kimlik dizesi / Version / Serial / 7 hız / kamera / FPS
// ini: MainCode, FontName, FontHeight, Resolution, Lang, BodyX20
char* GetPath(name,out,size);         // ".\Data\SPK\<name>"
char* GetConfigPath(name,out,size);   // ".\Data\SPK\Config\<name>"
```

Sabitler: `SPK_SERVER_DATA_SIZE 1089576`, `SPK_IP_SLOT_SIZE 12`, header ofsetleri
(`SPK_OFF_*`) — tümü tek yerde.

### 3.2 Okuma kuralları

- ConnectIP: 36 B tam okunur, XOR 0x20; 12 B slot NUL'a kadar; printable kontrolü.
- ServerData: 1.089.576 B tam boyut şart (eksik/fazla dosya reddedilir), XOR 0x20;
  slot okuma + sağ trim; hızlar `0 < v < 1.000.000` aralık kontrolü; kamera
  `0 < v < 360`, FPS `0 < v < 1000` (÷10) — mantık dışı değer 0'a çekilir.
- SPK.ini: `GetPrivateProfileString/Int` (A sürümü); değerler trim edilir.
- Varlık kanıtı: `m_VersionMismatch` yalnız iki taraf da doluysa hesaplanır.

### 3.3 MainLoad entegrasyonu (fallback matrisi)

| Alan | Kural |
|------|-------|
| `IpAddress` | ConnectIP okunduysa **her zaman** override |
| `ClientVersion`, `ClientSerial` | ServerData okunduysa koşulsuz |
| `ClientName`, `CustomerName`, `WindowName`, `ScreenShotPath` | yalnız değer doluysa |
| `DW/DK/FE/MG/DL/SU/RFMaxAttackSpeed` | yalnız değer != 0 ise |
| Diğer tüm MainInfo alanları | MUIG hattından aynen (SPK.ini JSON'u vb. etkilenmez) |

Dosyalar hiç yoksa `Load=false` → blok atlanır; davranış 2d.0 öncesiyle birebir aynı.
Kapsam dışı: `Config<Lang>` seçimi, kanat/item/LEVEL katalogları, ConnectIP CRC
doğrulaması, `IpAddressPort` (ServerData'da yok; docs/07 §6/5 — ini hattında kalıyor).

---

## 4. Test kanıtı (standalone koşucu)

`BuildLog\5Main\spktest\build_test.bat`: gerçek `SPKData.cpp`'den sed ile
`#include "stdafx.h"` satırını sistem başlıklarıyla değiştirip `SPKData_standalone.cpp`
üretir; `cl /EHsc /MT /W3` ile `spkdata_test.cpp` (konsol yazıcısı) ile derler.
Böylece **gerçek istemci kod yolu** (proje pragma/lib'leri olmadan) test edilir.

### 4.1 Test 1 — canlı dosyalar (`test_output_live.txt`)

```
Load(".\\Data\\SPK") = true
flags: dir=1 connectip=1 serverdata=1 spkini=1 mismatch=0
ip=[45.87.120.29]
server=[Axion Mu] [Axion Mu 2] [Axion Mu 3] [Axion Mu 4]
client=[Engine.exe] customer=[AxionMu] window=[Axion Mu]
screenshot=[Photos\Screen(%02d_%02d-%02d-%02d)-%04d.jpg]
version=[1.03.34] serial=[!571Axion@Mobile]
speeds=[65000] x7
camera=45.00 fps=24.00
maincode=[1.03.34] font=[Arial] height=16 resolution=5 lang=[En] bodyx20=13
path(Config\Item.bmd)=[.\Data\SPK\Config\Item.bmd]
EXIT=0
```

### 4.2 Test 2 — olmayan dizin (`test_output_missing.txt`)

```
Load(".\\Data\\SPK_YOK") = false
flags: dir=0 connectip=0 serverdata=0 spkini=0 mismatch=0   (tüm alanlar boş/0)
EXIT=1
```

### 4.3 Obj / exe kanıtı

- `BuildLog\5Main\SPKData.obj` (13:30) `dumpbin /disasm`: `530h`, `558h`, `55Ch`
  kullanımları; eski `52Ch`/`55Ah` **yok** (düzeltme derlemeye girdi).
- `ClientFile\Main.exe` (13:30, md5 `39f2af32…`): `ConnectIP.bmd`, `ServerData.bmd`,
  `BODY_X20`, `MainCode` literal'ları gömülü.

### 4.4 Gerçek proje obj'siyle link testi (`build_realobj_test.bat`)

`BuildLog\5Main\SPKData.obj` (projenin Global Release intermediate'ı) konsol
koşucusuyla link edildi: `link /LIBPATH:…\source\ExternalObject
/LIBPATH:…\dependencies\lib /FORCE:UNRESOLVED`. Obj'nin relative DEFAULTLIB yolları
(`..\ExternalObject\…`) bu LIBPATH'lerle çözüldü (link, libdir + relative yolu arar);
yalnız proje içi `g_render_lock`/`g_protocol_lock` (`/include` zorlaması — test yolu
dokunmaz) ve LNK2011 PCH uyarısı için `/FORCE` gerekti.

Koşucu **canlı dosyalarla aynı sonuçları** verdi (`test_output_realobj_live.txt`,
EXIT=0) ve olmayan dizin senaryosu `Load=false`/EXIT=1
(`test_output_realobj_missing.txt`). → Karar: test kanıtı artık **hem gerçek proje
obj'siyle hem standalone kopyayla** (§4.1/4.2) çift kanatlı; “gerçek obj link
edilemiyor” kısıtı kapandı (yalnız otomatik süreç için standalone kopya pratik).

---

## 5. Değişen / eklenen dosyalar

| Dosya | Değişiklik |
|-------|-----------|
| `Source\5.Main\source\SPKData.h` | YENİ — CSPKData bildirimi, `SPK_OFF_*` sabitleri |
| `Source\5.Main\source\SPKData.cpp` | YENİ — tam uygulama (XOR 0x20, slot/inil okuma, fallback) |
| `Source\5.Main\source\MainLoad.cpp` | `#include "SPKData.h"` + SPK bloğu (satır ~55–82) |
| `Source\5.Main\Main.vcxproj` + `.filters` | SPKData girdileri (MU\Client\BCustomLoad filtresi) |
| `ClientFile\Main.exe` | Yeniden derlendi (12.027.904 B, md5 `39f2af32…`) |
| `docs\07-SPK-BMD-FORMAT.md` | Rev. 02.10.2026 — §3a ofset düzeltmeleri, §4/§5 sözde kod, §6/1 not |
| `docs\02-YOL-HARITASI.md` | 2d.0 ⬜ → ✅ |
| `docs\03-…ENTEGRE-LISTESI.md` | B-01 ⬜ → ✅ 2d.0 |
| `BuildLog\5Main\spktest\*` | Test koşucusu + kanıt çıktıları (`test_output_{live,missing}.txt`) |

---

## 6. Açık kalemler

1. **Engine.exe kabul testi** — üretilen ConnectIP.bmd (CRC alanı 0) + ServerData
   ile canlı Engine açılışı; CRC zorunlu mu sorusunun cevabı burada. → 2d.2.
2. **CRC32 varyantı** — hâlâ çözülmedi (docs/07 §6/1); okuyucumuz CRC'yi okumaz,
   üretici 0 yazabilir.
3. `IpAddressPort` ServerData'da yok (docs/07 §6/5) — port ini hattında.
4. Kanat/item opsiyon/LEVEL katalog kayıtları (§3b–3d) — B-02 varlık katmanı.
5. 2d.1 üreticisi (`Source\6.GetMainInfo`) bu katmanın ters yönü — docs/08 D1–D9.

---

## 7. Kanıt arşivi

- `BuildLog\5Main\spktest\test_output_live.txt`, `test_output_missing.txt`
- `BuildLog\5Main\spktest\test_output_realobj_{live,missing}.txt` (gerçek obj koşusu)
- `BuildLog\5Main\spktest\build_realobj_test.bat` + `realobj\spkdata_realobj_test.exe`
- `BuildLog\5Main\spktest\SPKData_standalone.cpp` (üretilen kopya)
- `BuildLog\5Main\spkdata_obj_disasm.txt` (SPKData.obj disasm)
- `BuildLog\5Main\spktest\Data\SPK\*` (canlı bayt-birebir kopyalar)
- `docs\07` Rev. 02.10.2026 + docs/02 + docs/03 güncellemeleri
