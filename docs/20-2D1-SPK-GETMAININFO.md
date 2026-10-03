# 20 — 2d.1 SPK GetMainInfo (ilk dalga: D1-D3, D6, D7) UYGULAMASI

Tarih: 2026-10-02 (13:05–14:05 oturumu)
Kapsam isteği: "2d.1 — Seçilen varyanta göre kaynağı düzenle (2a.5 kararı doğrultusunda)."
(2a.5 kararı: tek hat = **SPK GetEngine**; iş planı docs/08 §4 D1-D9.)

> **Yöntem farkı:** Bu dalga varsayımla değil, **canlı üreticiyi sandbox'ta koşturup
> ölçerek** yapıldı. Canlı `GetMain\GetMainInfo.exe` (SPK/GetMain MU 5.2 By SPK,
> 369.664 B, 18.09.2026) `BuildLog\2d1\sandbox\` içinde birebir dizin yapısıyla
> (Client + GetMain + Source/ExMain_SPK) koşturuldu; girdiler sentinel değerlerle
> değiştirilip çıktılar bayt düzeyinde diff'lendi. Tüm eşleme tabloları ölçümdür.

---

## 1. Sonuç özeti

| # | İş | Durum | Kanıt |
|---|-----|-------|-------|
| D1 | `SPK/GetEngineConfig` ini okuyucu | ✅ | `SPK/GetEngineConfig.{h,cpp}`; tüm anahtarlar sentinel deneyiyle doğrulandı |
| D2 | `SPK/ConnectIPWriter` (CRC'siz!) | ✅ | 36 B; 3 senaryoda canlı araçla **bayt-birebir** |
| D3 | `SPK/ServerDataWriter` (şablon + ini yaması) | ✅ (başlık tam) | 1.089.576 B; 3 senaryoda **bayt-birebir**; D4/D5 katalog bölgeleri şablondan |
| D4 | CustomWing.txt → kanat kayıtları | ⬜ sıradaki | kayıt düzeni ölçüldü (§3.3) |
| D5 | CustomItem.txt → item/LEVEL katalogları | ⬜ sıradaki | — |
| D6 | `SPK_CRCFILE.ini` rapor üretici | ✅ | canlı raporla birebir (zaman satırı hariç) |
| D7 | CLI (`--ini/--client/--out/--template/--check`) | ✅ | `SPK/main_spk.{h,cpp}` |
| D8 | Çapraz doğrulama | ✅ (ilk yarı) | 3 senaryo: baseline / round A / round B — **byte-identical** |
| D9 | Doküman + CHANGELOG | ✅ | docs/20 (bu), docs/07 rev, docs/08, docs/02, docs/03, CHANGELOG |

**Derleme:** `Source\6.GetMainInfo\GetMainInfo\GetMainInfo.vcxproj` Release|Win32 (v143,
LTCG) → **0 hata**; çıktı `GetMain\GetMainInfo.exe` **3.723.776 B**, md5
`c480e0baf79cd1bf32c8bcfc79c1aed1` (14:02). Yeni SPK obj'leri: ConnectIPWriter,
CrcFileReport, GetEngineConfig, ServerDataWriter, SpkUtil, main_spk.

**Mod seçimi:** SPK modu artık **varsayılan**; eski MUIG akışı `--mode:muig` ile aynen
korunur (CBGetMain/CBTextInfo/License üretimi).

---

## 2. Canlı araç davranışı (ölçülen)

### 2.1 Sandbox kurulumu ve determinizm
`BuildLog\2d1\sandbox\` = `Client\` (Engine.exe, player.bmd, Data\SPK kopyaları) +
`GetMain\` (canlı exe + Data + GetEngine.ini) + `Source\ExMain_SPK\...`. Koşular
`< /dev/null` ile otomatik (araç `system("pause")` çağırır ama çıktılar önce yazılır).

- **Determinizm:** aynı girdilerle iki koşu **birebir aynı** (ConnectIP `8ac74a5b…`,
  ServerData `e3617db7…`).
- **Tam jeneratör:** ConnectIP/ServerData silinip koşulduğunda araç ikisini de
  **yeniden ve aynı** üretti → şablon/önceki-dosya bağımlılığı YOK (dosyanın geri
  kalanı Custom*.txt girdilerinden derleniyor).
- **Yan çıktılar:** `..\Client\Data\SPK\{ConnectIP,ServerData}.bmd`,
  `..\Client\Data\SPK\Config\Info\RenderEffect.bmd` (Data\RenderEffect\*.txt'den),
  `..\Source\ExMain_SPK\android\app\src\main\assets\spk\{connectip,serverdata}.bmd`
  (Android kopyası), `.\SPK_CRCFILE.ini`.
- **stdout:** UTF-16; başlık, kimlik alanları ve "MAIN CRC DETECTOR" bölümü
  (Engine/Player/ConnectIP/ServerData CRC'leri) yazdırır.

### 2.2 Girdi→çıktı haritası (sentinellerle ölçüldü)
Sentineller: round A (tüm sayısal anahtarlar benzersiz 21..120) ve round B
(tüm metinler RN1..RN10). Çıktı farkları:

| Girdi | ServerData decode offset | Biçim |
|-------|--------------------------|-------|
| ServerName_1..4 | 0x200 / 0x220 / 0x240 / 0x260 | 32 B slot, 0 doldurmalı |
| ClientName | 0x2A0 | 32 B slot |
| CustomerName | 0x2C0 | 32 B slot |
| WindowName | 0x2E0 | 32 B slot |
| ScreenShotPath | 0x3E0 | 64 B slot |
| ClientVersion | 0x4E0 | 8 B slot |
| ClientSerial | 0x4E8 | 16 B slot (tam 16 karakterde NUL yok) |
| MENU_BUTTON_01..20 | 0x4F9..0x50C | byte (value & 0xFF) |
| ButtonCharracter | 0x50D | byte |
| ButtonShopJwBless/JwSoul/Chaos | 0x50E / 0x50F / 0x510 | byte |
| ButtonShopWcoinC/P/G | 0x511 / 0x512 / 0x513 | byte |
| ButtonShopZens | 0x515 | byte |
| Ranking1..7 | 0x516..0x51C | byte |
| SkillManaPet | 0x51D | byte |
| EnableCoinTitle | 0x51E | byte |
| TextTips3Line | 0x51F | byte |
| RF_GLOVE / MG_HELM | 0x521 / 0x522 | byte |
| CreateCharSeason / ButtonClassUP / JewelBankTab | 0x524 / 0x525 / 0x526 | byte |
| MaxLevelDanhHieu/QuanHam/TuChan/HonHoan | 0x52B..0x52E | byte |
| MaxGameInstances | 0x52F | byte |
| DW..RFMaxAttackSpeed | 0x530..0x54B | 7 × int32 LE |
| ReconnectTime | 0x54C | byte |
| (ClientName dosyası) | 0x554 | u32 = CRC32(`<client>\<ClientName>`) yoksa 0 |
| CameraDefault | 0x558 | float (ham) |
| DefaultFPS | 0x55C | float (ham; ini "240.0" → 240.0) |
| IpAddress | ConnectIP 0x00..0x0B | 12 B'lik slot |
| IpAddressPort | ConnectIP 0x20 | u16 LE (canlı 0xAD75 = 44405) |
| AntiPort | ConnectIP 0x22 | u16 LE (canlı 0xDA32 = 55858) |

**Etkisiz ölçülenler:** `TabInfoStats` (hiçbir bayt değişmedi). Rezerve/henüz
anlamlandırılmayan baytlar: 0x514, 0x520, 0x523, 0x527..0x52A, 0x550..0x553 —
şablondan korunur (açık kalem).

### 2.3 İni anahtar adı tuzağı
Canlı ini'de anahtar **`ButtonShopChaos`** (Jw öneki YOK; Bless/Soul'da VAR).
İlk derlemede `ButtonShopJwChaos` okunmuştu → round A karşılaştırmasında tek bayt
(0x510) farkı yakalandı ve düzeltildi. Sentinelsiz fark edilemezdi.

---

## 3. Kesinleşen format bilgileri (docs/07 düzeltmeleri)

### 3.1 ConnectIP.bmd — "CRC" EFSANESİ ÇÖZÜLDÜ
Decode edilmiş 36 bayt:

```
0x00..0x0B : IpAddress (12 B slot; kısa dize kalanı 0x00)
0x0C..0x1F : 20 B sıfır dolgu
0x20..0x21 : IpAddressPort  (u16 LE)  <-- canlı 0x75AD decode = 44405
0x22..0x23 : AntiPort       (u16 LE)  <-- canlı 0x32DA decode = 55858
```

- Diskte tüm baytlar XOR 0x20. **Dosyada CRC YOKTUR.** docs/07 §1.3/§2/§6-1'de
  "CRC32 varyantı çözülemedi" sanılan son 4 bayt, iki porttur; 0x55 8D 12 FA ham
  dizi = decode `75 AD 32 DA` = port çifti.
- `SPK_CRCFILE.ini` içindeki `SPK_CIBMD`, dosyanın **tamamının** standart CRC32'sidir
  (CrcFileReport üretir) — dosyaya gömülmez.

### 3.2 ServerData.bmd header (0x4F9+ dahil)
docs/07 §3a tablosuna eklenenler: 0x4F9..0x52F ini bayt alanları, 0x554
ClientName-dosyası CRC'si, 0x558/0x55C float (ham değer). Detay tablo §2.2'de.

### 3.3 Kanat kaydı düzeni (D4 için başlangıç ölçümü)
- Kayıt tabanı: **0x6B8 + i × 0x154** (340 B stride); +0x00 = IIndex (u16),
  +0x54 = ModelName (36 B string).
- CustomWing.txt ilk satırındaki `IIndex` değişimi 0x6B8'i, `"Wing200"` adı 0x70C'yi,
  ikinci kaydın adı 0x860'ı değiştirdi (izole deney: sadece kanat dosyası değişti).

### 3.4 SPK_CRCFILE.ini
- CRC'ler **standart CRC32** (CCRC32; zlib ile birebir doğrulandı):
  `SPK_MEXE` = `<client>\Engine.exe` (sabit ad; ClientName değişse de Engine.exe),
  `SPK_PBMD` = `<client>\Data\Player\player.bmd`,
  `SPK_SDBMD`/`SPK_CIBMD` = üretilen dosyaların tamamı.
- Biçim: `%-36s= 0x%08X` — ancak **0 değeri `0x0`** yazılır (canlı davranış; dosya
  yokken ölçüldü).
- Girdi kontrolleri (9): CustomPetEffect, CustomPetGlow, JCItemToolTip,
  JCTextTooltip, CustomClaws, CustomPet, CustomBowCross, CustomItem, CustomWing.
- Config\Info kontrolleri (12): CustomMonster.bmd **iki kez**, RenderEffect,
  CustomIconBuff, CustomItemColorName, CustomItemPosition, CustomJewel,
  CustomModelNpc, CustomMonsterGold, CustomNpcName, CustomRingPen, CustomSetEffect.
  Yol: `<client>\Data\SPK\Config\Info\<ad>`.
- Zaman satırı: `; <Weekday>, HH:MM:SS dd/mm/yyyy` (İngilizce gün adı).

---

## 4. Doğrulama (çapraz testler)

Yöntem: **canlı araç** referans çıktılarına karşı **bizim araç** çıktıları;
bizim koşularda şablon = canlı istemcinin ESKİ dosyası (`53982bf8…`/`8ac74a5b…`),
yani yamamızın tüm alanları gerçekten üretmesi gerekti.

| Senaryo | ConnectIP | ServerData | Rapor |
|---------|-----------|------------|-------|
| Baseline (canlı girdiler) | `8ac74a5b…` = `8ac74a5b…` ✅ | `e3617db7…` = `e3617db7…` ✅ | 27 satır birebir (zaman hariç) ✅ |
| Round A (sayısal sentineller) | `193b21f3…` = `193b21f3…` ✅ | `3cf20916…` = `3cf20916…` ✅ | ✅ |
| Round B (metin sentinelleri) | `c2da1433…` = `c2da1433…` ✅ | `07e889c5…` = `07e889c5…` ✅ | ✅ |

- `--check` modu: round B çıktılarında 14 kontrol → **0 hata**, EXIT=0.
- "Çıktı dosyaları yokken" koşusu: canlı araç sıfırdan üretiyor; bizim araçta
  şablon yoksa **açık hata** verir (D4/D5 gelene kadar tasarım gereği; exit 2).

---

## 5. Uygulama (kod)

| Dosya | İçerik |
|-------|--------|
| `SPK/GetEngineConfig.{h,cpp}` | D1: `SPK_ENGINE_CONFIG` + `SPK_LoadEngineConfig` |
| `SPK/ConnectIPWriter.{h,cpp}` | D2: 36 B üretim (port çifti; CRC yok) |
| `SPK/ServerDataWriter.{h,cpp}` | D3: şablon + ini yaması + 0x554 CRC + XOR encode |
| `SPK/CrcFileReport.{h,cpp}` | D6: SPK_CRCFILE.ini (birebir biçim) |
| `SPK/SpkUtil.{h,cpp}` | CRC32 + tam dosya IO |
| `SPK/main_spk.{h,cpp}` | D7: CLI + akış + `--check` |
| `GetMainInfo.cpp` | `_tmain`: SPK varsayılan; `--mode:muig` eski akış |
| `GetMainInfo.vcxproj(.filters)` | SPK dosya girdileri + `$(ProjectDir)` include |

Kullanım (GetMain klasöründen):
```
GetMainInfo.exe                           # üretim (ini .\GetEngine.ini, client ..\Client)
GetMainInfo.exe --template:.\ServerData.template.bmd
GetMainInfo.exe --check                   # mevcut çıktıları ini ile doğrula
GetMainInfo.exe --mode:muig               # eski MUIG akışı
```

---

## 6. Açık kalemler

1. **D4/D5 tam jeneratör** — kanat (0x6B8+, stride 0x154) ve item opsiyon/LEVEL
   katalogları (0x2CC54+, 0xABB94+) hâlâ **şablondan** gelir; txt ayrıştırıcıları
   yazılınca şablon bağımlılığı kalkar. Şablonsuz üretim o güne kadar hata verir.
2. **RenderEffect.bmd üretimi** — canlı araç `Data\RenderEffect\*.txt`'lerden
   üretiyor (md5'i canlıyla aynı çıktı). Bizim araçta yok; rapor kontrolü
   dosya-varlığına bakar (rapor semantiği birebir).
3. Rezerve baytlar: 0x514, 0x520, 0x523, 0x527..0x52A, 0x550..0x553 (şablondan).
4. `TabInfoStats` ini anahtarı canlı araçta etkisiz ölçüldü (yazılmıyor).
5. 2d.0 okuyucusu `m_DefaultFps`'i ÷10 yorumluyor; üretici ham değer yazıyor
   (240.0). İstemci kullanımı Engine E2E'sinde netleşecek (2d.2).
6. Engine.exe kabul testi (2d.2): üretilen dosyalarla canlı istemci açılışı.

---

## 7. Kanıt arşivi (`BuildLog\2d1\`)

- `sandbox\` — canlı araç + dizin yapısı; `run_*.txt` stdout kayıtları
- `golden\baseline_{connectip,serverdata}.bmd` + `baseline_crcfile.ini`
- `roundA\{serverdata,connectip}.bmd`, `roundA\roundB_*` — canlı araç sentinel çıktıları
- `roundA\wingonly_serverdata.bmd` — izole kanat deneyi (W2)
- `ourtool\` — bizim araçla aynı senaryolar; `run_ours*` çıktıları
- `make_variant.js`, `map_sentinel.js`, `diff_dec.js` — deney araçları
