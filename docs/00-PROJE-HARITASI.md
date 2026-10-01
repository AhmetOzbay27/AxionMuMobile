# AXION MU SOURCE — PROJE HARİTASI

> **Bu dosya projenin ana giriş noktasıdır.** Her yapay zeka / geliştirici oturuma
> BURADAN başlar. Diğer dokümanlara buradan yönlendirilir. Bir iş bitince bu
> dosyadaki durum panosu GÜNCELLENMEK ZORUNDADIR.

---

## 1. NİHAİ HEDEF

`C:\Axion Mu Mobile\4.MuServer\Sub-1\` altındaki **canlı SPK sunucusunun** ve
SPK istemcisinin **birebir paritesini** kendi kaynak kodumuzla üretmek
(`C:\Axion Mu Source`). Canlı sunucu **referans ve asıl hedeftir**; dokunulmaz.

- Tüm içerik ve özellikler canlı SPK ile **eşitlenene** kadar adım adım düzeltme/ekleme.
- Eşitleme **tam ve eksiksiz** tamamlandıktan sonra **v2** aşamasına geçilir.
- Çalışma disiplini: **adım adım, oradan oraya atlama yok.** Her adım
  derleme/doğrulama + CHANGELOG kaydı ile kapanır.

---

## 2. DURUM PANOSU

| Aşama | İçerik | Durum |
|-------|--------|-------|
| Faz 0 | Ortam kurulumu (VS 2022, git, klasörler) | ✅ TAMAMLANDI |
| Faz 1 | CS/DS/JS/GS/GetMainInfo/Main derlemeleri | ✅ TAMAMLANDI (`faz1-tamamlandi` etiketi, commit `1339a290`) |
| Faz 2a | Canlı SPK envanteri (2a.1-2a.5) | ✅ TAMAMLANDI — iş emri: [05](05-SPK-MODUL-ENVANTERI.md), bulgular: [06](06-CANLI-SISTEM-ENVANTERI.md), karar: 03 "2a.5" |
| Faz 2b | MUIG (daha yeni) ortak dosya entegrasyonu (161 dosya + 4 modül) | ✅ TAMAMLANDI (2b.0-2b.3, 01.10.2026) — 2b.2: 7 dalga, 213 dosyanın 66'sı alındı/147'si korundu (docs/11); 2b.3: 29 MUIG-özel modül çapraz kontrol → docs/12 |
| Faz 2c | Eksik 58 modülün yeniden yazımı (54 sıfırdan) | ⏳ bekliyor |
| Faz 2d | GetMainInfo birleşimi + SPK istemci format katmanı (2d.0) | ⏳ bekliyor |
| Faz 2e | IP/config hizalama + istemci paketleme | ⏳ bekliyor |
| Faz 3 | Uçtan uca test (ayrı test sunucusu + DB restore) | ⏳ bekliyor |
| Faz 4 | Android port doğrulaması | ⏳ bekliyor |
| Faz 5 | Canlıya geçiş | ⏳ bekliyor |
| v2    | Parite sonrası yeni geliştirme aşaması | 🔒 kapalı (parite bitmeden açılmaz) |

**Şu anki tek aktif görev:** Faz 2b **TAMAMLANDI** (01.10.2026) —
2b.0 ✅ 4 donor modül · 2b.0-E ✅ 12 ezilen dosya raporu → [09](09-EZILEN-12-DOSYA-KARSILASTIRMA.md) ·
2b.1 ✅ diff matrisi → [10](10-DIFF-MATRISI.md) · 2b.2 ✅ 7 dalga + P0 → [11](11-PROTOCOL-P0-DIFF-TABLOSU.md) ·
2b.3 ✅ 29 MUIG-özel modül çapraz kontrol → [12](12-MUIG68-CAPRAZ-KONTROL.md)
(7 parite-tamam · 3 iş-kalemi 2c'ye · 19 canlıda-yok/OFF; YENİ keşif: EventGvG canlıda VAR, bizde eksik).
**Sıradaki: Faz 2c.1** — modül envanterini önceliğe dizecek (60 modül: 59 + EventGvG).
**✅ 2c.1 dizimi tamam (26.10.01 14:05):** [13-2C1-ONCELIK-IS-EMRI.md](13-2C1-ONCELIK-IS-EMRI.md) —
A=24 çekirdek · B=9 event · C=21 QoL · D=3 anticheat · E=3 kosmetik. Sıradaki: **2c.1-A** (Harmony).

> **26.10.01 13:50 — E-kalemleri kapatıldı** (E-01/E-02/E-04/E-06/E-09/E-11,
> bkz. [09](09-EZILEN-12-DOSYA-KARSILASTIRMA.md)): donor taban E-02,
> canlı config paritesi E-04 (`SPK\ChangeClass.xml`) + E-11 (canlı TXT) +
> E-01 (`Event\BossGuild.xml` yolu), canlı sessizlik E-09 (7 notice) +
> E-06 (CEventName hattı). Kalan 2c kalemleri docs/09 §4/§6'ta belgelendi.
bkz. [02-YOL-HARITASI.md](02-YOL-HARITASI.md).

> **2b.2-M ek dalga (01.10.2026 07:40):** MapManager zinciri donor'dan alındı
> (SPK GetMapNonPK zinciri + canlı 16-kolon Load adaptasyonu korunarak),
> CustomPick.cpp tam donör oldu, canlı MapManager.txt deploy ağacına kopyalandı.
> GS temiz → 10.775.552 B. Detay: CHANGELOG 2b.2-M.
> **2b.2-N ek dalga (01.10.2026 09:25):** 'configuration reloaded' ailesi
> Gate/MoveSummon/Notice/ResetTable/Skill modüllerine uygulandı (m_Path +
> Reload, E-05 deseni; canlı birebir `[CSınıf] ... reloaded.` logları);
> `/reload move|skill` mevcut ServerInfo zincirleri korundu (zincir Load'ları
> artık m_Path saklıyor). GS temiz → 10.784.256 B. Detay: CHANGELOG 2b.2-N.

---

## 3. KLASÖR HARİTASI

### Proje kökü — `C:\Axion Mu Source\`
| Klasör | İçerik |
|--------|--------|
| `Source\1.ConnectServer` | CS kaynağı (Release_EX603\|Win32, v143) |
| `Source\2.DataServer` | DS kaynağı (Release_EX603\|Win32, v143) |
| `Source\3.JoinServer` | JS kaynağı (Release_EX603\|Win32, v143) |
| `Source\4.GameServer` | GS kaynağı (8 konfig, v143; Resource.h onarıldı) |
| `Source\5.Main` | İstemci kaynağı ("Global Release"\|Win32, v143; OutDir → `..\..\ClientFile`) |
| `Source\6.GetMainInfo` | Info aracı (Release\|Win32, v143; OutDir → `..\..\GetMain`) |
| `Source\EncryptBMD`, `Source\Util` | Yardımcı araçlar |
| `MuServer\` | Referans binary'ler + bizim derlemelerimiz (218 MB, repoda) |
| `ServerTools\` | Tools + MuServer_S6_2020 + DB_SQL_12.bak |
| `ClientBuild_192.168.99.200\` | **Canlı test istemci paketi (git dışı, dokunulma-kopyalanabilir)** |
| `ClientFile\` | Main derleme çıktısı (Main.exe repoda, ara ürünler hariç) |
| `GetMain\` | GetMainInfo derleme çıktısı |
| `android\`, `sokol-master\` | Mobil katman bağımlılıkları |
| `BuildLog\` | Derleme logları + string-parite analiz çıktıları |
| `Dashboard\` | **İlerleme panosu + ajan köprüsü** — bağımlılıksız PowerShell HTTP sunucusu + tek dosya UI; port 8096. Başlat: `Dashboard\start-dashboard.cmd` (dış erişim `http://45.87.120.29:8096/`, localhost-only için `-Published 0`). Firewall kuralı "Axion Mu Pano 8096" + URL ACL http://+:8096/ eklendi. **Ajan köprüsü:** `data\oneriler.json` (ajan önerileri, ajan yazar), `data\komut.json` (kullanıcının verdiği komut kuyruğu + geçmişi), `data\sonuc.json` (ajanın son mesajı), `data\pin.txt` (POST PIN'i, git dışı). Kullanıcı panodan komut verir → sohbete **"pano"** yazan ajan kuyruğu işler, sonucu sonuc.json'a yazar. |
| `docs\` | **Proje dokümantasyonu (bu klasör)** — 06: canlı sistem envanteri |

### Dış referans konumları (proje dışı, salt okunur)
| Konum | İçerik |
|--------|--------|
| `C:\Axion Mu Mobile\4.MuServer\Sub-1\` | **CANLI SPK SUNUCUSU — ASLA DOKUNULMAZ** |
| `C:\Axion Mu Mobile\New Source Code\Source\Source\` | MUIG donor kaynağı (Main5.2 + sunucu bileşenleri + Encoder) |
| `C:\Axion Mu Mobile\analiz\` | Eski analiz raporları (içerik bu docs'a taşındı) |
| `C:\Axion Mu Mobile\Client and Tools\GetMain\` | SPK GetEngine varyant referansı (369 KB) |
| `C:\Axion Mu Mobile\New Source Code\Source\Source\Main5.2\Release\` | MUIG referans Main.exe + Main.pdb (korunur) |

---

## 4. DOKÜMAN İNDEKSİ

| Dosya | Amaç | Ne zaman güncellenir |
|-------|------|----------------------|
| [00-PROJE-HARITASI.md](00-PROJE-HARITASI.md) | Bu dosya — giriş + durum panosu | Her adım sonunda |
| [01-ARASTIRMA-REHBERI.md](01-ARASTIRMA-REHBERI.md) | Tüm önceki araştırmanın derlemesi | Yeni mimari bilgi edinilince |
| [02-YOL-HARITASI.md](02-YOL-HARITASI.md) | Adım adım çalışma planı | Adım tamamlandı/başladığında |
| [03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md](03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md) | Eksikler + kaynak eşlemesi | Her entegrasyon tamamlandığında |
| [04-HATA-GUNLUGU.md](04-HATA-GUNLUGU.md) | Açık/kapalı hata kayıtları | Hata bulunduğunda/çözüldüğünde |
| [09-EZILEN-12-DOSYA-KARSILASTIRMA.md](09-EZILEN-12-DOSYA-KARSILASTIRMA.md) | 12 ezilen dosyanın bizim↔donor↔canlı analizi + 2b.2 uygulama sırası | Her E-kalem entegre edildiğinde |
| [10-DIFF-MATRISI.md](10-DIFF-MATRISI.md) | 2b.1 çıktısı: 213 ortak dosyanın diff matrisi, G1-G5 risk grupları | 2b.2 grup alımlarında |
| [CHANGELOG.md](CHANGELOG.md) | Tüm değişikliklerin kaydı | **Her değişiklikte** |

Dış plan dosyası: `C:\Axion Mu Mobile\analiz\SPK-UYGULAMA-PLANI.md` (eski
kayıtlar; güncel bilgi bu docs setidir).

---

## 5. AI ÇALIŞMA PROTOKOLÜ (her oturumda uygulanır)

1. **Bu dosyayı oku** → durum panosundan aktif görevi bul.
2. **02-YOL-HARITASI.md**'de aktif fazın adım listesine git; ilk açık adımı seç.
3. Adım çalışmadan önce: ilgili doküman satırını "🔄 DEVAM EDİYOR" yap.
4. Adım bitince: derleme/doğrulama çalıştır → **CHANGELOG'a kayıt yaz** →
   durum panosunu güncelle → git commit (tek adım = tek commit tercihen).
5. Hata çıkarsa: **04-HATA-GUNLUGU.md**'ye aç, çözünce kapat (kök neden + fix).
6. **Atlama yok:** bir adım tamamlanmadan sonrakine geçilmez. Kısmi iş bırakmak
   zorunluysa adım satırına kalan kısmı net yaz.

### Kritik kurallar
- `C:\Axion Mu Mobile\4.MuServer\` **ASLA** değiştirilmez/kopyalanmaz üzerine.
- `ClientBuild_192.168.99.200\` git dışıdır; içindekiler değiştirilmez.
- MuServer\ referans binary'ler üzerine yazılmaz; yeni çıktılar konumlarına
  bilinçli olarak (Faz 1'de yapıldığı gibi) yazılır ve commit'lenir.
- VS 2022 Community, toolset **v143**, Win SDK 10.0.22621, ATL kurulu.
  MFC kullanılmaz (hiçbir projede UseOfMfc=true yok).
- MSYS2 bash'te MSBuild çağrıları için `export MSYS2_ARG_CONV_EXCL='*'` şart.
- Disk ~5 GB boş: büyük temizlik yapmadan yeni workload açma.
- Git: stale `index.lock` görülürse `rm -f .git/index.lock` (timeout sonrası).

---

## 6. İKİ KAYNAK HATTI (özet — detay: 01-ARASTIRMA-REHBERI)

| Hat | Konum | Rol |
|-----|-------|-----|
| **SPK / Takumi** | Canlı PDB kanıtı: `D:\Mu-Mobile\MuSPK\Source\ExGameServer\GameServer\` | **Referans/HEDEF** — canlı sunucu bu hattın binary'si |
| **MUIG** | `C:\Axion Mu Mobile\New Source Code\Source\Source\` | **DONÖR** — 161 ortak dosyada daha yeni + 68 özel modül |

Bizim taban kaynak: SPK/Takumi (Android portlu, Platform/Scenes katmanlı) +
MUIG'den seçilmiş güncellemeler. Canlıda olan ama hiçbir kaynak setinde
olmayan 57 modül (SPK_* 20 dahil) Faz 2c'de sıfırdan yazılacak.
