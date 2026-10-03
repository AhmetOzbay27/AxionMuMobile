# 08 — GETMAININFO TEK MODÜL TASARIMI (Faz 2a.5 karar + 2d.1 iş planı)

> Tarih: 30.09.2026. Bu doküman, 2a.5 varyant karşılaştırmasını ve tek
> GetMainInfo tasarımını kesinleştirir. Format detayları:
> [07-SPK-BMD-FORMAT.md](07-SPK-BMD-FORMAT.md) · Canlı kanıtlar:
> [06](06-CANLI-SISTEM-ENVANTERI.md) · İş emri: [05](05-SPK-MODUL-ENVANTERI.md).
>
> **Durum (02.10.2026):** 2d.1 ilk dalgası uygulandı — **D1, D2, D3 (başlık), D6, D7**
> tamam; canlı `GetMainInfo.exe` ile 3 senaryoda (baseline + 2 sentinel) ConnectIP,
> ServerData ve rapor **bayt-birebir**. Şablon stratejisi (§3.4) artık yalnız
> katalog bölgeleri için geçerli. Kalan: **D4/D5** (kanat/item/LEVEL tam jeneratör) +
> RenderEffect.bmd üretimi. Rapor: [20-2D1-SPK-GETMAININFO.md](20-2D1-SPK-GETMAININFO.md).

---

## 1. İKİ VARYANT — KARŞILAŞTIRMA TABLOSU

| Özellik | SPK GetEngine (GetMain\) | MUIG MainInfo (bizim kaynak) |
|---|---|---|
| Çıktı dosyaları | **ConnectIP.bmd + ServerData.bmd + SPK_CRCFILE.ini** | CBGetMain.bin / CBTextInfo.bin / License.json |
| Girdi | GetEngine.ini + GetMain\Data\*.txt | MainInfo.ini + Data\Local\*.bin |
| Veri hattı | **Engine.exe** (SPK) | Main.exe (MUIG) |
| Boyut | 369.664 B | 3.693.568 B |
| Veri formatı | XOR 0x20 sabit slotlu BMD | CBGetMain özel binary + JSON |
| Sunucu listesi | 4 slot, ServerData.bmd içinde | CBGetMain.bin içinde |
| İstemci doğrulama | binary içi ("input data is inconsistent!") | License.json doğrulama |
| Bağlantı | IP:Port ini'den, bmd'ye gömülü | MainInfo.ini'den |
| **Kanıt durumu** | **Canlıda aktif** (Client\ + Engine.exe + Engine.ini) | Canlı pakette yok |
| Karar | **BENİMSENİR** | **Bayrakla devre dışı** (kaynak korunur) |

**Kanıt zinciri (özet):** GetEngine.ini (IP 45.87.120.29:44405, v1.03.34,
Türkçe yorumlu = Axion'un kendi konfig'i) ≡ ConnectIP.bmd XOR 0x20 decode;
CustomWing.txt ↔ ServerData.bmd kanat kataloğu birebir; SPK_CRCFILE.ini =
üretici doğrulama raporu; canlı pakette MainInfo hattı dosyası yok.

---

## 2. SON KANIT: GetMain\Data = ServerData.bmd İÇERİK KAYNAĞI

`GetMain\Data\CustomWing.txt` satırları, ServerData.bmd'deki 340 B'lik kanat
kayıtlarıyla birebir: Wing200/201/202, ConquerorWing, angel_devil_wing,
cape_of_death → KF_Death_clka/clkb.tga, Wing401-405 → KF_darklordwing4de/dd.tga,
Wing407 → KF_ragefighterwing4db.tga, Wing410 → cKdLLV1Dcmaker.tga,
ChristmasW6 → GiftDcMakerckManto/Faixa.tga, WingCustom1-15.

**GetMainInfo mimarisi (kesinleşti):**
```
GetEngine.ini + GetMain\Data\*.txt  ──(GetMainInfo)──▶  ConnectIP.bmd
                                                        ServerData.bmd
                                                        SPK_CRCFILE.ini
                                                        ──▶ Client\Data\SPK\
```
Sabit 1.089.576 B boyut, sabit slotlar ve 0xDF ayraçlar = GetMainInfo'nun
"compile" çıktısı: metin kaynaklardan derlenip şablon dolduruluyor.

---

## 3. TEK GETMAININFO TASARIMI (2d.1 uygulama planı)

### 3.0 Mimari prensip
- Tek çalıştırılabilir, **iki iş modu**: `--mode:spk` (yeni varsayılan) ve
  `--mode:muig` (legacy, bayrakla derlenir — varsayılan kapatılır).
- Mevcut MUIG Custom* ayrıştırıcılar **korunur** (2d.2'de spk moduna
  yeniden kullanılır: CustomWing/CustomItem ayrıştırıcıları ortaklaşa).
- Çıktı dizini parametrik: `--out:"C:\...\Client\Data\SPK"`.

### 3.1 Yeni SPK modülü (Source\6.GetMainInfo\GetMainInfo\SPK\)
```
SPK/GetEngineConfig.h/.cpp  — GetEngine.ini okuyucu (UTF-8; SuperKhung bölümleri;
                              değer çiftleri; yorum ';' satır içi)
SPK/CrcPatch.h/.cpp         — CRC çözümü: Engine.exe'den tablo çıkarımı;
                              çözülmeden once CRC alanina 0x00000000
SPK/ConnectIPWriter.cpp     — 36 B üretim (07 dok. §2)
SPK/ServerDataWriter.cpp    — header (ini aynası) + kanat kayıtları (CustomWing.txt
                              ayrıştırıcı → 340 B kayıt) + item opsiyon kayıtları
                              (CustomItem.txt → 260 B) + LEVEL tabloları;
                              sabit boyut doldurma (0x20 disk dolgusu), footer
SPK/CrcFileReport.cpp       — SPK_CRCFILE.ini raporu (FOUND/NOT FOUND + CRC'ler)
SPK/main_spk.cpp            — mod giriş noktasi; hata kodlari; --check modu
```

### 3.2 Veri akışı
```
GetEngine.ini ─┐
CustomWing.txt ─┼─▶ [ayrıştır] ─▶ [şablon doldur] ─▶ XOR 0x20 ─▶ .bmd + CRC raporu
CustomItem.txt ─┤
diğer Data\*.txt ┘
```

### 3.3 Doğrulama stratejisi (2d.1 kabul testi)
1. **Girdi = canlı girdiler** (GetEngine.ini + GetMain\Data\*.txt kopyaları).
2. Çıktı: ConnectIP.bmd → boyut 36; ServerData.bmd → boyut 1.089.576.
3. **Çapraz doğrulama:** bizim çıktı vs canlı örnek `ServerData_Client.bmd`:
   header alanları (0x200+ blok) byte-birebir, kanat kayıtları içerik-eşdeğer
   (boş slotlar farklı olabilir), CRC alanı hariç.
4. Engine.exe kabul testi: üretilen dosyaları `Client\Data\SPK\` test kopyasına
   koy → istemci hatasız açılır (aşağıdaki bilinen risk).
5. `--check` modu: mevcut .bmd'yi okuyup GetEngine.ini ile tutarlılık raporu.

### 3.4 Bilinen riskler
- **CRC varyantı:** çözülmeden üretilen dosyada CRC=0; Engine.exe reddederse
  CRC algoritması 2d.0'da Engine.exe'den türetilmeden SPK modu tamamlanamaz.
- ServerData bilinmeyen baytları (0x5A0+ bloklar, opsiyon tuple semantiği):
  şablon kopya stratejisi ile bypass — canlı örneğin bilinmeyen baytları
  aynen kopyalanır, yalnızca bilinen alanlar ini/txt'den güncellenir.
- Host ile: **VMP SDK (VMProtectSDK32.dll) import'u ÇIKARILACAK** (Axion
  build'i korumasız; canlıda da import yok).

### 3.5 GetMainInfo'nun MUIG tarafı görevleri (korunur)
- CBGetMain.bin üretimi (legacy mod) — bayraklı; CBTextInfo.bin, License.json
  üretimi aynı kalır.

---

## 4. GÖREV KIRILIMI (2d.1 iş adımları)

| # | İş | Çıktı kriteri |
|---|-----|----------------|
| D1 | `SPK/GetEngineConfig` okuyucu + unit test (ini parse) | ini değerleri struct'a |
| D2 | ConnectIPWriter + 0x00-CRC | 36 B; decode = ini IP |
| D3 | ServerDataWriter: header + şablon kopya | 1.089.576 B; header alanları ini ile tutarlı |
| D4 | CustomWing.txt ayrıştırıcı → 340 B kayıtlar | kanat adları/tga yolları canlıyla eş |
| D5 | CustomItem.txt → 260 B opsiyon kayıtları + LEVEL tabloları | "[+9] Damage" format canlıyla eş |
| D6 | SPK_CRCFILE.ini rapor üretici | FOUND/NOT FOUND + CRC listesi |
| D7 | CLI: --mode:spk/--mode:muig/--out/--check | iki mod ayrı ayrı derlenir |
| D8 | Çapraz doğrulama + Engine.exe kabul testi | istemci açılır; CRC kararı netleşir |
| D9 | CHANGELOG + 03/05/07 doküman güncelleme | — |

**Sıralama:** D1→D2→D3→D4→D5→D6→D7→D8 (CRC çözümü D3'e kadar gerekmez;
D8'de gerekirse Engine.exe analizine dönülür).

## 5. DOKÜMAN/İZLEME ETKİSİ
- 03 listesi: B-03 → tasarım tamam, uygulama 2d.1 (D1-D9).
- 05 dokümanı: etkilenmez (GS tarafı).
- 07 format dokümanı: D1-D9 çıktılarıyla güncellenecek (CRC, opsiyon semantiği).
- CHANGELOG: her D adımı ayrı kayıt + commit.
