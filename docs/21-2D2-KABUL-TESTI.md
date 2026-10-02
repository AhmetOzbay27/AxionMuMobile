# 21 — 2d.2 KABUL TESTİ: ÜRETİLEN SPK VERİSİ CANLI İSTEMCİYLE (Engine.exe) E2E

> Adım: **2d.2** (bkz. [02-YOL-HARITASI.md](02-YOL-HARITASI.md) Faz 2d).
> Amaç: 2d.1'de üretilen `ConnectIP.bmd` + `ServerData.bmd` dosyalarının **canlı
> istemci binary'si** (9.200.152 B `Engine.exe`) tarafından gerçekten tüketildiğini
> göstermek. Test 02.10.2026 14:2x-14:4x arasında **fiilen koşuldu**; bu doküman
> kanıtları toplar. (2e adımı sırasında yazıldı.)

## 1. Düzenek

| Bileşen | Yol / sürüm |
|---------|-------------|
| Test hedefi (canlı istemci, kopya değil) | `C:\Axion Mu Mobile\Client and Tools\Client\` |
| Koşucu 1 (canlı istemci) | `BuildLog\2d2\run_client_test.ps1` (süreç ağacı + pencere numaralandırma + PrintWindow ekran görüntüsü + TCP kaydı + süreç öldürme) |
| Koşucu 2 (SPK dosya değişimi) | `BuildLog\2d2\run_accept.sh` (dosyaları yerleştir → koş → **orijinalleri geri koy** → md5 doğrula) |
| Üretim dosyaları (bizim) | `BuildLog\2d2\deploy\{ConnectIP.bmd, ServerData.bmd, SPK_CRCFILE.ini}` |
| Canlı yedek | `BuildLog\2d2\backup\` — ConnectIP `8ac74a5b79244943ab5607d408049353`, ServerData `53982bf8423f731cef2df6de714e78ff` |

Canlı istemci orijinal dosyaları test sonunda geri yüklendi ve md5 ile doğrulandı
(`run_accept.sh` içindeki `RESTORE OK` satırları).

## 2. Koşular ve sonuçlar

| Koşu | Yerleştirilen ServerData | Pencere | TCP (Get-NetTCPConnection) | Sonuç dosyası |
|------|--------------------------|---------|-----------------------------|----------------|
| run0 | (yok — ön kontrol) | — | — | `results\run0-control.json` |
| run1 | canlı/yedek (kontrol) | `Axion Mu` (t≈0.8 s, sınıf `Axion Mu`, görünür) | koşu sürümünde kayıt yok | `results\run1-spklaunch-control.json` |
| **run2** | **bizim üretim `e3617db7…`** | **`Axion Mu` (t≈0.8 s, görünür)** | **`45.87.120.29:44405` — SynSent (t≈12 s, pid=8284)** | `results\run2-ours-accept.json` |
| run3 | bozuk ServerData (`results\negative_serverdata.bmd`) | `Axion Mu` (t≈0.8 s) | `45.87.120.29:44405` **ve** `45.87.120.29:55858` (AntiPort) SynSent (t≈12.7 s) | `results\run3-negative.json` |

Ekran görüntüsü kanıtı: `BuildLog\2d2\shots\run2-ours-accept-win-0x3103AC.png`
(pencere başlığı `Axion Mu`, sınıf `Axion Mu`).

## 3. Bulgular

1. **Pozitif kanıt:** Canlı `Engine.exe`, bizim üretim `ConnectIP.bmd` + `ServerData.bmd`
   dosyalarıyla açıldı ve **`45.87.120.29:44405` adresine bağlantı kurmaya çalıştı**
   (`SynSent`). Bağlantı hedefi doğrudan ConnectIP.bmd'den okunur → dosya tüketildi.
2. **ConnectIP birebir:** Bizim üretim ConnectIP, canlı dosyanın aynısıdır
   (`8ac74a5b…`) — 2d.1 D2 dalgası canlı çıktıyla bayt-birebir.
3. **ServerData beklendiği gibi 4 bayt farklı:** canlı `53982bf8…` ↔ bizim `e3617db7…`;
   farklar 0x4FF (MENU_BUTTON_07) ve 0x554 (ClientName-dosyası CRC32) — yani *içerik*
   değil, **üreten ortamın imzası**. docs/20 §4 tablosuyla uyumlu.
4. **AntiPort da kullanılıyor:** run2'de yalnız 44405 görülürken run3'te iki port
   (44405 + 55858) görüldü. ConnectIP 0x20/0x22 alanları bu portları taşıyor
   (docs/07 §1.3); 2e.2'de 5.Main'e bu portların okunması eklendi.
5. **İçerik doğrulaması zayıf çıktı (negatif kontrol dersi):** bozuk ServerData ile
   (run3) canlı istemci yine pencere açtı ve bağlandı — yani "pencere + TCP" tek
   başına *içerik* doğrulaması değildir; dosya-seviyesi bayt karşılaştırması
   (2d.1, docs/20) bu yüzden birincil kanıttır. 04-HATA-GUNLUGU'na işlendi (H-012).

## 4. Sınırlar / sonraki adımlar

- Bu tur **canlı istemci binary'si** ile yapıldı; bizim `Main.exe`'nin paket içinde
  koşması **2e.2**'nin konusudur (bkz. [22-2E1-2E2-PAKET.md](22-2E1-2E2-PAKET.md)).
- `SPK_CRCFILE.ini` raporu (D6) üretildi; canlı istemci **onu bir kabul kriteri olarak
  okumuyor** (dosya yalnız GetMainInfo çıktısıdır) — 5.Main'de bu dosyayı okuyan kod yok.
- Test sonunda canlı istemci dizininde `ErrorLoadFile.txt` oluştu (canlı istemcinin
  kendi hata günlüğü; run3 negatif kontrolünde yazıldı). Canlı dizine başka dosya
  yazılmadı; dokunulmazlık kuralı korundu (yalnız SPK veri dosyaları geçici değiştirildi
  ve geri konuldu).

## 5. Yeniden koşma

```bash
cd "/c/Axion Mu Source"
bash BuildLog/2d2/run_accept.sh run2-ours-accept BuildLog/2d2/deploy/ServerData.bmd BuildLog/2d2/deploy/ConnectIP.bmd 22
# run0/run1/run3 için: kaynak parametrelerini değiştir (backup/ ya da results/negative_serverdata.bmd)
```
