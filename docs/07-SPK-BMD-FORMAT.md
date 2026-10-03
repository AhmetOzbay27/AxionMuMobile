# 07 — SPK BMD FORMAT DOKÜMANI (ConnectIP.bmd + ServerData.bmd)

> Tarih: 30.09.2026 — Faz 2d.0 önhazırlığı. Tersine mühendislik kaynağı:
> `Client and Tools\Client\Data\SPK\` örnek dosyaları (arşiv:
> `BuildLog\envanter\bmd\`) + `GetMain\GetEngine.ini` çapraz doğrulaması.
> Amaç: GetMainInfo ÜRETİCİSİ ve Main OKUYUCUSU bu formata birebir uyacak.
>
> **Rev. 02.10.2026 (2d.0):** §3a sayısal ofsetler canlı dosya hex dökümüyle
> düzeltildi (0x52C→**0x530**, ~0x55A→**0x558**/**0x55C**). Okuyucu canlıda:
> `Source\5.Main\source\SPKData.cpp`; doğrulama: `BuildLog\5Main\spktest\` + docs/19.
>
> **Rev. 02.10.2026 (2d.1):** ConnectIP "CRC"i port çifti olarak çözüldü (§1.3);
> 0x4F9–0x554 ini alan haritası eklendi; üretici `Source\6.GetMainInfo\GetMainInfo\SPK\*`
> canlı araçla 3 senaryoda bayt-birebir doğrulandı (docs/20).

---

## 1. GENEL KURALLAR (iki dosya için ortak)

1. **Tüm dosya baytları XOR 0x20 ile gizlenmiştir** (tek bayt anahtar 0x20).
   - Diskte boşluk (0x20) = decode'da 0x00; dolgu baytı 0x20'dir.
   - Decode doğrulaması: harf durumları ters görünür ('aXION mU' → 'Axion Mu',
     '\' ↔ '|', 'pHOTOS|sCREEN JPG' → 'Photos\Screen....jpg').
   - Encode = decode (XOR simetrik).
2. **Çok baytlı sayılar little-endian.** String'ler ASCII + 0x00 sonlandırma,
   sabit slot uzunluklarında (kalanı 0x00).
3. **ConnectIP.bmd son 4 bayt = CRC DEĞİL, port çiftidir** (2d.1, 02.10.2026'da
   canlı `GetMainInfo.exe` sandbox deneyleriyle çözüldü — docs/20 §3.1):
   `[0x20]=IpAddressPort u16 LE, [0x22]=AntiPort u16 LE`. Canlı örnek decode
   `75 AD 32 DA` = 44405 / 55858. Daha önce "çözülemeyen CRC32" sanılan baytlar
   buydu; **dosyada hiçbir CRC yoktur**. (Standart CRC32 dosya bütünlüğü değeri
   yalnızca üretici raporu `SPK_CRCFILE.ini` içindeki `SPK_CIBMD`/`SPK_SDBMD`'dir.)

---

## 2. ConnectIP.BMD (36 bayt)

| Offset (dec) | Uzunluk | Decode içerik | Not |
|--------------|---------|----------------|-----|
| 0–11 | 12 | `"45.87.120.29"` | IP string, XOR 0x20 (kısa dize kalanı 0x00) |
| 12–31 | 20 | 0x00 dolgu | diskte 0x20 |
| 32–33 | 2 | **IpAddressPort** u16 LE = 44405 | ini `IpAddressPort` (2d.1) |
| 34–35 | 2 | **AntiPort** u16 LE = 55858 | ini `AntiPort` (2d.1) |

- Örnek ham: `14 15 0E 18 17 0E 11 12 10 0E 12 19 | 20×20 | 55 8D 12 FA`
- Decode: `34 35 2E 38 37 2E 31 32 30 2E 32 39` = `45.87.120.29` ✓
  (GetEngine.ini `IpAddress` ile birebir — üretici ini'den okuyor.)
- Trailer decode: `75 AD 32 DA` = `0xAD75` (44405) + `0xDA32` (55858) ✓
  — canlı ini `IpAddressPort`/`AntiPort` değerleriyle birebir (2d.1 ölçümü).
- Boyut sabit 36 B; IP alanı 12 B'lik sabit slot (12 karakter tam doldurur,
  NUL zorlanmaz; kısa IP kalanı 0x00).

---

## 3. ServerData.BMD (1.089.576 bayt)

**GetEngine.ini'nin binary aynası + içerik katalogları.** Tüm ofsetler
DECODE (XOR 0x20 uygulanmış) durumdadır.

### 3a. Header (0x000–0x6FF)
| Offset | Alan | Örnek (canlı) | GetEngine.ini karşılığı |
|--------|------|----------------|--------------------------|
| 0x000–0x1FF | 256 B sıfır pad | — | — |
| 0x200/0x220/0x240/0x260 | ServerName[4] × 32 B | `Axion Mu`, `Axion Mu 2..4` | ServerName_1..4 |
| 0x2A0 | ClientExeName[32] | `Engine.exe` | ClientName |
| 0x2C0 | CustomerName[32] | `AxionMu` | CustomerName |
| 0x2E0 | WindowName[32] | `Axion Mu` | WindowName |
| 0x3E0 | ScreenShotPath[~64] | `Photos\Screen(%02d_%02d-%02d-%02d)-%04d.jpg` | ScreenShotPath |
| 0x4E0 | Version[8] | `1.03.34` | ClientVersion (= SPK.ini MainCode) |
| 0x4E8 | Serial[16] | `!571Axion@Mobile` | ClientSerial |
| 0x4F9–0x50C | 20 × bayt | `MENU_BUTTON_01..20` | Menü buton bayrakları (2d.1) |
| 0x50D–0x513, 0x515 | 8 × bayt | ButtonCharracter, JwBless, JwSoul, **Chaos** (Jw'siz anahtar), WcoinC/P/G, Zens | 2d.1 |
| 0x516–0x51C | 7 × bayt | `Ranking1..7` = 1 | 2d.1 |
| 0x51D–0x51F | 3 × bayt | SkillManaPet, EnableCoinTitle, TextTips3Line | 2d.1 |
| 0x521–0x522 | 2 × bayt | RF_GLOVE, MG_HELM | 2d.1 |
| 0x524–0x526 | 3 × bayt | CreateCharSeason, ButtonClassUP, JewelBankTab | 2d.1 |
| 0x52B–0x52E | 4 × bayt | MaxLevelDanhHieu/QuanHam/TuChan/HonHoan | 2d.1 |
| 0x52F | 1 × bayt | MaxGameInstances = 10 | 2d.1 |
| 0x530 | 7 × int32 = 65000 | sınıf hız limitleri | MaxAttackSpeed* ailesi |
| 0x54C | 1 × bayt | ReconnectTime = 1 | 2d.1 |
| 0x554 | u32 | CRC32(`<client>\Engine.exe` benzeri: ini `ClientName` dosyası) | yoksa 0 (2d.1) |
| 0x558 | float 45.0 | CameraDefault=45 ✓ | CameraDefault (ham değer) |
| 0x55C | float 240.0 | ini `DefaultFPS = 240.0` **ham** yazılır (2d.1) | DefaultFPS (ham; ×10 değil) |
| 0x5A0+ | karma int blokları (`ffff ffff` = -1 işaretleri) | — | 2d.0'da alan alan |

> **Ofset düzeltmesi (02.10.2026, 2d.0):** hız bloğu dword'leri **0x530–0x54B**
> (0x52C değil), kamera **0x558**, FPS **0x55C** (0x55A değil). Kanıt: canlı
> `ServerData.bmd` node hex dökümü; kullanım: `SPKData.h` sabitleri, istemci
> Main.exe 13:30 derlemesi (docs/19).
>
> **Alan haritası kesinleşti (02.10.2026, 2d.1):** yukarıdaki 0x4F9–0x554 satırları
> canlı `GetMainInfo.exe`'nin sentinel ini deneyleriyle ölçüldü (docs/20 §2.2);
> üretici `Source\6.GetMainInfo\...\SPK\ServerDataWriter.cpp` bu haritayı uygular.

### 3b. Kanat/Item kataloğu (0x70C–~0x2CBxx) — kayıt stride **340 B (0x154)**
| Kayıt içi offset | Alan | Örnek |
|------------------|------|-------|
| +0x00 | int32 tür/kimlik (0x6C=108, 0x04…) | — |
| +0x04 | int32 (0x04 / 0x02) | — |
| +0x10 | name[36] | `Wing200`, `ConquerorWing`, `ChristmasW6` |
| +0x34 | tga yolu[104] | `SPK\Item\KF_Death_clka.tga` |
| +0x9C | tga yolu 2[104] | `SPK\Item\KF_darklordwing4dd.tga` |
| +0x108 | (int32 tip, int32 değer) çiftleri ×14 | opsiyon listesi |

Örnek kayıt çiftleri: `0x02,0x18B97` / `0x04,0x8B` / `0x02,0x3D` / `0x04,0x54`…
(tip-değer option tuple'ları; anlamları 2d.0'da istemci testiyle eşlenecek).

### 3c. Item opsiyon tablosu (0x2CC54–) — kayıt stride **260 B (0x104)**
- +0x00: name string (boş bırakılabilir): `[+9] Damage: +100`,
  `[+15] Damage: +500 & Str x2: +2%` (4'lü gruplar +9/+11/+13/+15, tier tier artar)
- +0x1xx: binary opsiyon alanları (değer çiftleri)
- Aralık en az `0x2CC54` → `0x31880+` (dizide binlerce kayıt).

### 3d. Seviye tabloları (0x0ABB94–) — kayıt stride **100 B**
- +0x00: `LEVEL 5 WEAPONS` … `LEVEL 8 WEAPONS` (grup başına 8 kayıt ritmi).

### 3e. Footer
- Son baytlar: `… 00…00 FF FF FF FF 00 00 00 00 00…00` — bitiş işareti/CRC
  alanı; 2d.0'da Engine.exe okuma akışıyla netleşecek.

### 3f. Toplam string istatistiği
1.510 adet ≥5 karakter printable string (decode sonrası) — tamamı yukarıdaki
tabloların alan içerikleri; rastgele veri yok (format tamamen yapılandırılmış).

---

## 4. ÜRETİCİ (GetMainInfo → 2d.1) SÖZDE KODU

```
oku GetEngine.ini:
  IpAddress, IpAddressPort, ClientName, CustomerName, WindowName,
  ScreenShotPath, ClientVersion, ClientSerial, ServerName_1..4,
  CameraDefault, MaxAttackSpeed*, DefaultFPS, MENUBUTTON_*, ...

# ConnectIP.bmd  (CRC YOK — 2d.1: son 4 bayt port çifti)
body = zero(36)
copy body[0..11] = IpAddress (en çok 12 B)
body[0x20] = u16le(IpAddressPort); body[0x22] = u16le(AntiPort)
write xor0x20(body)                             # 36 B

# ServerData.bmd
buf = zeros(1_089_576)
yaz 0x200: ServerName_1..4  (32 B slot, ascii+NUL, xor0x20)
yaz 0x2A0: ClientName; 0x2C0: CustomerName; 0x2E0: WindowName
yaz 0x3E0: ScreenShotPath
yaz 0x4E0: ClientVersion[8]; 0x4E8: ClientSerial[16]
yaz 0x4F9..0x52F: MENU_BUTTON_01..20, ButtonShop*/Ranking/Level/... baytları (docs/20 §2.2)
yaz 0x530: 7 × int32(MaxAttackSpeedDefault); 0x54C: ReconnectTime
yaz 0x554: u32(CRC32(ClientName dosyası)) ya da 0
yaz 0x558: float32(CameraDefault); 0x55C: float32(DefaultFPS)   # ham değerler
yaz 0x70C+: kanat kayıtları (340 B stride, §3b)
yaz 0x2CC54+: item opsiyon kayıtları (260 B stride, §3c)
yaz 0x0ABB94+: LEVEL tabloları (100 B stride, §3d)
footer: ...FFFFFFFF + zeros
encode: her bayt ^= 0x20
```

## 5. OKUYUCU (Main → 2d.0) SÖZDE KODU

```
d = read_all("Data\SPK\ConnectIP.bmd")
ip = xor0x20(d[0..11]) -> string (ilk NUL'e dek)
sv = read_all("Data\SPK\ServerData.bmd"); sv ^= 0x20
server_names = sv[0x200,32] ×4   # sunucu seçim listesi
version = sv[0x4E0,8]; serial = sv[0x4E8,16]   # bağlantı paketinde kullanılır
camera = float32(sv[0x558]); fps = float32(sv[0x55C]); ...   # 2d.1: ham değer
port = u16le(dec(ci)[0x20]); anti = u16le(dec(ci)[0x22])      # ConnectIP trailer
```

## 6. AÇIK KALEMLER (2d.0'da kapatılacak)

1. ~~ConnectIP CRC32 varyantı~~ **KAPANDI (2d.1):** CRC yoktu; son 4 bayt
   `IpAddressPort + AntiPort` (§1.3, §2).
2. Kanat kaydı int çiftlerinin (tip,değer) semantiği; 340 B stride içinde
   kullanılmayan alanlar (2d.1: kayıt tabanı 0x6B8 + i×0x154, +0x00 IIndex,
   +0x54 name — docs/20 §3.3; alan eşlemesi D4 işi).
3. Item opsiyon kaydının binary bölümü (260 B stride'ın string dışı kısmı) + LEVEL
   tabloları (2d.1: hâlâ şablon stratejisinde; D5 işi).
4. ServerData footer işaretinin anlamı (boyut kontrolü mü CRC mi).
5. ~~IpAddressPort'un ServerData'daki konumu~~ **KAPANDI (2d.1):** ServerData'da
   yok; ConnectIP.bmd 0x20'de u16 LE olarak duruyor (§2).

---

## 7. EKLEME: Engine.exe OKUMA İPUÇLARI (ilk tarama, 30.09.2026)

- `Data\SPK\Connect.P.bmd` (string, nokta=aynı byte) — IP yükü "Connect" +
  "IP" ayrı segmentlerde; muhtemelen dosya adı iki parça halinde birleştiriliyor
  (anti-string taktiği). Okuma noktası: `[SPK] Data\SPK\Connect?P.bmd`.
- `Data\SPK\ServerData.bmd` doğrudan geçiyor; komşusu: `Error 0x0000FF —
  "The input data is inconsistent! Please verify..."` → dosya açılışta
  **okunuyor ve doğrulanıyor** (yanlış içerik = açılış hatası). Bu, 2d.0'da
  ürettiğimiz dosyanın canlı istemcide kabul testinin nasıl yapılacağını
  gösterir: Engine.exe'yi dekode edilmiş ServerData ile başlatmak.
- `Data\SPK\Config%s\Info\*.bmd` deseni: Config altında dinamik alt klasör
  (muhtemelen dil/tema değişkeni %s) — SPK_CRCFILE.ini'deki FOUND/NOT FOUND
  listesiyle uyumlu.
- `SPK_CRCFILE.ini` Engine.exe'de YOK → CRC raporu yalnızca GetMainInfo
  (üretici) tarafının aracı; istemci doğrudan binary doğrulama yapıyor.
- Kaynak imza: "8.5.8 - Website: http://mu-spk.info [SPK]" (motor sürüm satırı).
