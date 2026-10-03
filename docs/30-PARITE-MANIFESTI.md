# 30 — PARİTE MANİFESTİ (özellik sözleşmesi)

> **Tarih:** 03.10.2026 · **Kapsam:** canlı SPK ile birebir parite · **Kaynak:** docs/13 (56 + 4 özellik) + otomatik kanıt taraması
> Ham veri: `BuildLog\envanter\parite_ozellik_kaniti.tsv` (2.816 B)

---

## 0. KURAL — BİR ÖZELLİK 5 EKSENLİDİR

Bir özellik **ancak beş eksenin tamamı yeşil** ise paritedir:

| # | Eksen | Kanıt |
|---|---|---|
| 1 | **GS** | sunucu modülü kaynakta var + canlı map/PDB/disasm ile denetlenmiş |
| 2 | **CFG** | config dosyası canlı şemayla birebir üretilmiş |
| 3 | **CLI** | istemci penceresi + kod karşılığı yazılmış |
| 4 | **PROTO** | paket yapısı + opcode kayıtlı, iki uçta doğrulanmış |
| 5 | **E2E** | yerel yığında uçtan uca koşmuş, log kanıtı var |

> 2c planı yalnız **1. ekseni** (GS modülü) sayıyordu; 60 kalemlik plan dolmasına rağmen parite olmuyordu, çünkü **istemci ekseni hiç ölçülmemişti.** Bu tablo ölçümü beş eksene taşır.

---

## 1. DURUM PANOSU (03.10.2026)

| Eksen | Yeşil | Toplam | Oran |
|---|---|---|---|
| 1 — GS kaynağı | 10 | 60 | %17 |
| 2 — Config | 28 | 28 | %100 |
| 3 — İstemci | 7 | 60 | %12 |
| 4 — Protokol | 6 | 60 | dış opcode seti birebir (docs/11); **viewport paket düzeni bayt-bayt doğrulandı ve hizalandı (H-018, 2e.7 — canlı PDB kanıtı, [docs/27](27-H018-VIEWPORT-PAKET-DUZENI.md))**; modül opcode kaydı yok |
| 5 — E2E | 6 | 60 | %10 (6'sı sunucu içi; **istemci içeren 1** — B-08 tooltip zinciri, 2e.5: `ToolTipText.txt` → istemci tooltip tablosu → runtime kanıtı) |

> **Denetim düzeltmesi (04.10.2026):** Başlık sayıları bu tablodan üretilemiyordu; düzeltildi.
> Tablo sayımı: GS 10 (6 ana + §3'teki 4) · istemci karşılığı dolu satır 7 (A1/A8/B3/C47/C39/C41/C49;
> "core MU" parantezliler hariç). A16 satırındaki istemci karşılığı "YOK" yanlış — `CB_AutoNapGame`
> istemcide var ve [31](31-ISTEMCI-EKSENI-IS-EMRI.md) slot 08'de sayıyor. "~31 kutu" değeri config
> ekseni dışlanarak hesaplanmış görünüyor (10+7+6+6=29); sayım kuralı belirsiz.

**Sonuç:** Parite = `5 eksen × 60 özellik = 300 kutu`; bugün ~`31` kutu yeşil. **En büyük boşluk: 3. eksen (istemci) ve 4. eksen (protokol kaydı).**

> **Güncelleme 03.10.2026 (2e.6):** E2E eksenindeki "istemci içeren 0" ifadesi 2e.5 sonrası
> bayatlamıştı; B-08 tooltip hattı istemci içinde uçtan uca koşturulup log kanıtı alındığı
> için istemci içeren E2E sayısı 1'dir. Tam login → karakter → harita akışı ise
> **H-017** nedeniyle (bağlantısı kesilmiş oturum, `GetForegroundWindow()==0`) hâlâ 0'dır ve
> etkileşimli masaüstü oturumunda koşulacaktır.

---

## 2. ANA TABLO

### A — ÇEKİRDEK OYNANIŞ (oyun akışı)

| # | Özellik | GS kaynak | Config | Config durumu | İstemci karşılığı | Durum |
|---|---|---|---|---|---|---|
| 4 | **SPK_Harmony** | SPK/SPK_Harmony | CustomHarmony.xml + CustomHarmony.xml | CANLI | YOK (core MU: UIJewelHarmony) | ✅ A1 |
| 23 | **SPK_MonsterSkill** | MonsterSkillElement, MonsterSkillElement | CustomMonsterSkill.txt + CustomMonsterSkill.txt | CANLI | YOK | ✅ A2 |
| 37 | **CustomItemSetPro** | CustomItemSetPro, CustomItemSetPro | CustomItemSetPro.xml | CANLI | YOK | ✅ A3 |
| 36 | **CustomItemPro** | — | CustomItemPro.xml | CANLI | YOK (core MU: CB_LockItem) | ⬜ |
| 38 | **CustomNewBuff** | — | CustomNewBuff.xml | CANLI | YOK | ⬜ |
| 55 | **CustomStartItemDame** | — | CustomStartItemDame.txt | CANLI | YOK | ⬜ |
| 56 | **CustomStartSetItemDame** | — | CustomStartSetItemDame.txt | CANLI | YOK | ⬜ |
| 57 | **ItemExcellentOptionRate** | — | — | — | YOK | ⬜ |
| 5 | **SPK_TuLuyen** | — | TuLuyen.xml | CANLI | YOK | ⬜ |
| 6 | **SPK_QuanHam** | — | QuanHam.xml | CANLI | YOK | ⬜ |
| 7 | **SPK_HonHoan** | — | HonHoan.xml | CANLI | YOK | ⬜ |
| 8 | **SPK_DanhHieu** | DanhHieu, DanhHieu | DanhHieu.xml | CANLI | CB_DanhHieu | ⬜ |
| 9 | **SPK_Relife** | — | Relife.xml | CANLI | YOK | ⬜ |
| 10 | **SPK_DungLuyen** | — | DungLuyen.xml | CANLI | YOK | ⬜ |
| 15 | **ResetChange** | — | ResetChange.txt | CANLI | YOK | ⬜ |
| 13 | **B_MocNap** | — | CustomMocNap.xml | CANLI | YOK | ⬜ |
| 40 | **GuildUpgrade** | — | GuildUpgrade.txt | CANLI | YOK | ⬜ |
| 50 | **ExWinQuestSystem** | — | CreationQuestSystem.xml + CreationQuestData_1.ini | CANLI | YOK | ⬜ |
| 44 | **SocketConnection** | — | — | — | YOK | ⬜ |
| 45 | **SocketManagerModern** | — | — | — | YOK | ⬜ |

### B — EVENT

| # | Özellik | GS kaynak | Config | Config durumu | İstemci karşılığı | Durum |
|---|---|---|---|---|---|---|
| 1 | **SPK_EventMainManager** | EventMainManager, EventMainManager | — | — | YOK | ✅ B1 |
| 60 | **EventGvG** | EventGvG, EventGvG | Monster.ini | CANLI | YOK | ✅ B2 |
| 31 | **ActiveInvasions** | CB_ActiveInvasions, CB_ActiveInvasions | — | — | CB_ActiveInvasions | ✅ B3 |
| 27 | **SPK_CastleEvent** | — | — | — | YOK | ⬜ |
| 32 | **BEventThanMa** | — | — | — | YOK | ⬜ |
| 33 | **SkyEvent** | — | — | — | YOK | ⬜ |
| 34 | **TEventGreatPK** | — | — | — | YOK | ⬜ |
| 35 | **CustomDameBoss** | — | — | — | YOK | ⬜ |
| 17 | **ReiDoMu** | ReiDoMU, ReiDoMU | — | — | YOK | ⬜ |

### C — QoL / İKİNCİL İÇERİK

| # | Özellik | GS kaynak | Config | Config durumu | İstemci karşılığı | Durum |
|---|---|---|---|---|---|---|
| 22 | **SPK_ItemTrader** | — | — | — | YOK | ⬜ |
| 47 | **CongHuong** | CBCongHuong, CBCongHuong | CongHuong.txt | CANLI | CB_CongHuong | ⬜ |
| 20 | **SPK_ExtendShop** | — | — | — | YOK | ⬜ |
| 21 | **SPK_NewXShop** | — | CustomShop.xml | CANLI | YOK | ⬜ |
| 28 | **BotOnline** | — | — | — | YOK | ⬜ |
| 29 | **BotTradeMix** | — | BotTradeMix.txt | CANLI | YOK | ⬜ |
| 30 | **CBotMixSystem** | — | — | — | YOK | ⬜ |
| 41 | **RankingServer** | CustomRanking, CustomRanking | — | — | CustomRanking, NewUIGensRanking | ⬜ |
| 24 | **SPK_CmdSocket** | — | — | — | YOK | ⬜ |
| 26 | **SPK_StatsInfo** | — | CharOption.xml | CANLI | YOK | ⬜ |
| 46 | **ViewInfoItem** | — | — | — | YOK | ⬜ |
| 14 | **CharOption** | — | CharOption.xml | CANLI | YOK | ⬜ |
| 39 | **CustomLuckySpin** | — | CustomVongQuay.xml | CANLI | VongQuay | ⬜ |
| 48 | **PartySetPass** | — | — | — | YOK | ⬜ |
| 49 | **ChecklevelVip** | — | — | — | CBInterfaceVIPChar | ⬜ |
| 51 | **CustomRenameChar** | — | — | — | YOK | ⬜ |
| 52 | **CustomReadGuildServer** | — | — | — | YOK | ⬜ |
| 53 | **EventItemBagUI** | — | EventItemBagManager.txt | CANLI | YOK | ⬜ |
| 54 | **ShopManagerUI** | — | ShopManager.txt | CANLI | YOK | ⬜ |
| 11 | **SPK_MessLang** | — | Message.xml | CANLI | YOK | ⬜ |
| 12 | **ZzzToolKit** | — | — | — | YOK | ⬜ |

### D — ANTİCHEAT

| # | Özellik | GS kaynak | Config | Config durumu | İstemci karşılığı | Durum |
|---|---|---|---|---|---|---|
| 16 | **ResetLimiter** | — | — | — | YOK | ⬜ |
| 43 | **PassLock** | — | — | — | YOK | ⬜ |
| 42 | **ChangePass** | — | — | — | YOK | ⬜ |

### E — KOSMETİK

| # | Özellik | GS kaynak | Config | Config durumu | İstemci karşılığı | Durum |
|---|---|---|---|---|---|---|
| 25 | **SPK_CustomNameColor** | — | CustomNameColor.ini | CANLI | YOK | ⬜ |
| 58 | **LogErrorForm** | — | — | — | YOK | ⬜ |
| 59 | **GetLicenseID** | — | — | — | YOK | ⬜ |

## 3. 2b.0 KAPSAMINDAKİLER (E2E + istemci ekseni eksik)

| Özellik | GS | Config | İstemci |
|---|---|---|---|
| SkillDamage | ✅ | ✅ | ⬜ |
| SPK_AddBuff | ✅ | ✅ | ⬜ |
| SPK_AutoHp | ✅ | ✅ | ⬜ |
| CustomJewelBank | ✅ | ✅ | ⬜ |

---

## 4. İŞÇİ / SÜPERVİZÖR / PATRON PROTOKOLÜ

| Rol | Yetki | Görev |
|---|---|---|
| **Patron** | yalnız komut | `Dashboarddatakomut.json` kuyruğuna yazar; her tur başında okunur |
| **İşçi** | **yalnız dosya düzenleme** | bir özelliği 5 eksende tamamlar: GS → CFG → CLI → PROTO → E2E |
| **Süpervizör** | düzenleme yok | kanıt doğrular (derleme boyutu, config md5, paket dump, log satırı), patrona rapor yazar; kanıt yoksa kalemi iptal eder |

**Kabul kuralı:** Süpervizör onayı olmadan hiçbir özellik manifestte yeşile dönmez ve commit atılmaz.

---

## 5. HAM VERİ

- `BuildLog\envanter\parite_ozellik_kaniti.tsv` — bu tablonun makine-okunur hali

