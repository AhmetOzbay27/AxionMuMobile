# 03 — EKSİK İÇERİK VE ENTEGRASYON LİSTESİ

> Canlı SPK'da VAR, bizim kaynakta EKSİK olan her şey + nereden
> entegre edileceği. Her satır: kaynak → hedef → durum. Tamamlananlar ✅
> ile işaretlenir ve CHANGELOG'a kaydedilir.

Durum kodları: ⬜ eksik · 🔄 işlemde · ✅ entegre · ❓ araştırılacak

---

## A. SUNUCU (GameServer) — 57 eksik modül

> Kaynak kanıtı: canlı `GameServer.pdb` (D:\Mu-Mobile\MuSPK\... yolları).
> Tam envanter Faz 2a.1'de çıkarılacak; aşağıdaki tablo o çalışma ile
> doldurulacak. Bilinen başlıklar:

| # | Modül / Özellik | Kaynak | Hedef | Durum |
|---|-----------------|--------|-------|-------|
| A-01 | SPK_* modülleri (20 adet) | ❓ yok — sıfırdan yazılacak (PDB + davranış analizi) | `Source\4.GameServer\` | ⬜ (envanter bekleniyor) |
| A-02 | Diğer 37 eksik GS modülü | ❓ yok — aynı şekilde | `Source\4.GameServer\` | ⬜ (envanter bekleniyor) |
| A-03 | MuServer config seti (canlı Sub-1 ini/txt/dat) | Canlı `C:\Axion Mu Mobile\4.MuServer\Sub-1\` (SALT OKUNUR kopya) | `MuServer\4.GameServer\...` | ⬜ 2a.3 |

## B. İSTEMCİ (Main)

| # | Modül / Özellik | Kaynak | Hedef | Durum |
|---|-----------------|--------|-------|-------|
| B-01 | Canlı client içerik farkları (Data\ bmd/ini) | Canlı ClientBuild (kopya üzerinden) | `ClientBuild` test kopyası | ⬜ 2a.4 envanteri |
| B-02 | GetMainInfo varyant birleşimi | SPK GetEngine referansı + MUIG MainInfo kaynağı | `Source\6.GetMainInfo\` | ⬜ 2a.5 karar |
| B-03 | Gömülü IP / config okuma düzeni | Canlı değer: 192.168.0.150 | `Source\5.Main\` | ⬜ 2e.1 |
| B-04 | MUIG 68 özel modülünün canlı karşılığı kontrolü | MUIG donor | `Source\5.Main\` | ⬜ 2b.3 |

## C. ORTAK / ALTYAPI

| # | Kalem | Kaynak | Hedef | Durum |
|---|-------|--------|-------|-------|
| C-01 | MUIG'in 161 daha yeni ortak dosyası | MUIG donor | `Source\` (SPK taban) | ⬜ 2b.1-2b.2 diff matrisi sonrası grup grup |
| C-02 | DB şeması (DB_SQL_12.bak) ile GS beklentileri uyumu | `ServerTools\DB_SQL_12.bak` | test DB | ⬜ Faz 3 öncesi |

## D. BİLİNEN KÜÇÜK SİMİTLER (parite sapmaları, düşük risk)

| # | Kalem | Detay | Durum |
|---|-------|-------|-------|
| D-01 | String sapmaları (Main) | `FCBad item index.` vs `Bad item index.` gibi ön-ek farkları — MUIG/SPK revizyon kaçağı; 2b'de donor revizyonu alınca çoğu kapanır | ⬜ |
| D-02 | `7 SO BAO MAT` stringi | Yalnız bizim derlemede (MUIG VN kaynak izi); canlıda yok. 2b'de değerlendirilecek | ⬜ |

---

### KAYNAK KONUMLARI (hızlı erişim)
- MUIG donor: `C:\Axion Mu Mobile\New Source Code\Source\Source\` (+Main5.2, +Encoder)
- SPK GetEngine referans binary: `C:\Axion Mu Mobile\Client and Tools\GetMain\`
- Canlı sunucu (salt okunur): `C:\Axion Mu Mobile\4.MuServer\Sub-1\`
- Canlı istemci (git dışı): `C:\Axion Mu Source\ClientBuild_192.168.99.200\`
- Eski analiz raporları: `C:\Axion Mu Mobile\analiz\`
