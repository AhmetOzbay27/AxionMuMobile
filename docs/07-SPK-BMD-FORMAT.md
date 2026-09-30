# 07 — SPK BMD FORMAT DOKÜMANI (ConnectIP.bmd + ServerData.bmd)

> Tarih: 30.09.2026 — Faz 2d.0 önhazırlığı. Tersine mühendislik kaynağı:
> `Client and Tools\Client\Data\SPK\` örnek dosyaları (arşiv:
> `BuildLog\envanter\bmd\`) + `GetMain\GetEngine.ini` çapraz doğrulaması.
> Amaç: GetMainInfo ÜRETİCİSİ ve Main OKUYUCUSU bu formata birebir uyacak.

---

## 1. GENEL KURALLAR (iki dosya için ortak)

1. **Tüm dosya baytları XOR 0x20 ile gizlenmiştir** (tek bayt anahtar 0x20).
   - Diskte boşluk (0x20) = decode'da 0x00; dolgu baytı 0x20'dir.
   - Decode doğrulaması: harf durumları ters görünür ('aXION mU' → 'Axion Mu',
     '\' ↔ '|', 'pHOTOS|sCREEN JPG' → 'Photos\Screen....jpg').
   - Encode = decode (XOR simetrik).
2. **Çok baytlı sayılar little-endian.** String'ler ASCII + 0x00 sonlandırma,
   sabit slot uzunluklarında (kalanı 0x00).
3. **ConnectIP.bmd CRC** (32..35, LE): standart CRC32 (poly 0xEDB88320,
   init/final 0xFFFFFFFF) hiçbir basit varyantla eşleşmedi —
   raw/dec × {32,28,12} + BE/LE toplam testleri negatif.
   → Özel tablo/seed'li CRC ya da adres tabanlı sağlama. 2d.0'da Engine.exe
   içindeki crc32 tablosundan çözülecek; O ZAMANA KADAR ÜRETİCİDE CRC ALANI
   0x00000000 YAZILABİLİR (okuyucu muhtemelen zorunlu tutmuyor — istemci mevcut
   CRC'li dosyayla çalışıyor, sıfırlı dosyanın davranışı 2d.0 testinde görülür).

---

## 2. ConnectIP.BMD (36 bayt)

| Offset (dec) | Uzunluk | Decode içerik | Not |
|--------------|---------|----------------|-----|
| 0–11 | 12 | `"45.87.120.29"` | IP string, XOR 0x20 |
| 12–31 | 20 | 0x00 dolgu | diskte 0x20 |
| 32–35 | 4 | CRC32 (LE) | algoritma açıklanacak (§1.3) |

- Örnek ham: `14 15 0E 18 17 0E 11 12 10 0E 12 19 | 20×20 | 55 8D 12 FA`
- Decode: `34 35 2E 38 37 2E 31 32 30 2E 32 39` = `45.87.120.29` ✓
  (GetEngine.ini `IpAddress` ile birebir — üretici ini'den okuyor.)
- Boyut sabit 36 B; IP alanı 12 B'lik sabit slot (daha uzun IP → 16 B alan
  olasılığı 2d.0'da test edilir; 15 karakterlik IPv4 sınırı).

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
| 0x516 | 7 × bayt 0x01 | enable bayrakları | (7 = S6 sınıf sayısı) |
| 0x52C | 7 × int32 = 65000 | sınıf hız limitleri | MaxAttackSpeed* ailesi (ini'de 67000; server değeri farklı) |
| ~0x55A | float 45.0 + float 240.0 | CameraDefault=45 ✓ | CameraDefault (+FPS×10?) |
| 0x5A0+ | karma int blokları (`ffff ffff` = -1 işaretleri) | — | 2d.0'da alan alan |

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

# ConnectIP.bmd
ip = pad_right(IpAddress, 12, 0x00)
body = xor0x20(ip) + xor0x20(bytes(0x20, 20))   # dolgu diskte 0x20 kalır
write body + crc32_placeholder(4)               # CRC §1.3 çözülünce gerçek

# ServerData.bmd
buf = zeros(1_089_576)
yaz 0x200: ServerName_1..4  (32 B slot, ascii+NUL, xor0x20)
yaz 0x2A0: ClientName; 0x2C0: CustomerName; 0x2E0: WindowName
yaz 0x3E0: ScreenShotPath
yaz 0x4E0: ClientVersion[8]; 0x4E8: ClientSerial[16]
yaz 0x516: 7 × 0x01; 0x52C: 7 × int32(MaxAttackSpeedDefault)
yaz ~0x55A: float32(CameraDefault); float32(DefaultFPS*10)
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
camera = float32(sv[~0x55A]); ...
```

## 6. AÇIK KALEMLER (2d.0'da kapatılacak)

1. ConnectIP CRC32 varyantı (§1.3) — Engine.exe crc tablosundan / sıfır-CRC
   kabul testi.
2. Kanat kaydı int çiftlerinin (tip,değer) semantiği; 340 B stride içinde
   kullanılmayan alanlar.
3. Item opsiyon kaydının binary bölümü (260 B stride'ın string dışı kısmı).
4. ServerData footer işaretinin anlamı (boyut kontrolü mü CRC mi).
5. `IpAddressPort`'un ServerData'daki konumu (header'da bulunamadı; port
   muhtemelen ConnectIP'de değil — bağlantı portu ini'den gelmeye devam
   edebilir; Engine.exe testiyle netleşecek).
