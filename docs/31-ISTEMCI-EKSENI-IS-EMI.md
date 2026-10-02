# 31 — İSTEMCİ EKSENİ İŞ EMİRİ (manifest 3. eksen)

> **Tarih:** 03.10.2026 · **Kapsam:** 3. eksen (CLI) — istemci pencereleri, menü sistemi, protokol karşılığı
> **Kanıt:** canlı `GetEngine.ini` (MENU_BUTTON dizisi) + canlı `Client/Data/SPK` varlıkları + bizim `Source/5.Main` envanteri
> **Bağlı:** [30-PARITE-MANIFESTI.md](30-PARITE-MANIFESTI.md) · [13-2C1-ONCELIK-IS-EMRI.md](13-2C1-ONCELIK-IS-EMRI.md)

---

## 0. ANA BULGU

Canlı SPK istemcisinin özellik listesi gizli değil: `GetEngine.ini` içinde **20 slotluk `MENU_BUTTON_xx` dizisi** olarak duruyor (Türkçe etiketli). Bizim istemci kaynağında bu anahtarların **hiçbir referansı yok** (`grep MENU_BUTTON` = 0).

Sonuç: sunucu modülleri yazılsa bile **oyuncu bu özelliklere erişemiyor**, çünkü menü sistemi ve özellik pencereleri istemcide hiç yazılmamış. Paritenin en büyük ve en sessiz eksiği budur.

---

## 1. CANLI SÖZLEŞME — 20 MENÜ SLOTU

| Slot | Canlı | Özellik | Sunucu modülü | Canlı istemci varlığı | Bizim istemcimiz |
|---|---|---|---|---|---|
| 01 | 0 | Sıralama | RankingServer | CustomRanking / NewUIGensRanking | ✅ var |
| 02 | 0 | Etkinlik Saati | SPK_EventMainManager | — | ⬜ yok |
| 03 | 0 | Yeniden Canlanma (Relife) | SPK_Relife | — | ⬜ yok |
| 04 | 0 | Reset Değişimi | ResetChange | ingame_Bt_Reset.ozt | ⬜ yok |
| 05 | 0 | Ünvan / Nişan (Danh Hieu) | SPK_DanhHieu | CB_DanhHieu + Data/Interface/RankTitle | ✅ var |
| 06 | 0 | Çark (Spin) | CustomLuckySpin | VongQuay | ✅ var |
| 07 | 1 | VIP / ID Seviyesi | ChecklevelVip | CBInterfaceVIPChar | ✅ var |
| 08 | 0 | Bağış / Bakiye Yükleme (Moc Nap) | B_MocNap | CB_AutoNapGame | ✅ var |
| 09 | 0 | Sınıf Değiştirme | ChangeClass (E-04 GS'te kapalı) | — | ⬜ yok |
| 10 | 0 | Şifre Değiştirme | ChangePass | — | ⬜ yok |
| 11 | 0 | Ruh Döngüsü (Hon Hoan) | SPK_HonHoan | — | ⬜ yok |
| 12 | 0 | X-Shop / Yeni Mağaza | SPK_NewXShop | newui_item_subpet.spk, iMoney.ozt | ⬜ yok |
| 13 | 0 | Yetiştirme (Tu Luyen) | SPK_TuLuyen | RankTitle/TL-21 | ⬜ yok |
| 14 | 0 | Askeri Rütbe (Quan Ham) | SPK_QuanHam | RankTitle/TL-22 | ⬜ yok |
| 15 | 0 | Yıldız Seçeneği | CustomItemPro | — | ⬜ yok |
| 16 | 0 | Mücevher Mağazası | SPK_ExtendShop | ItemSlotShop.ozt, freecoin.ozt | ⬜ yok |
| 17 | 0 | Kullanılmıyor | — (canlıda kullanılmıyor) | — | ⬜ yok |
| 18 | 0 | Kullanılmıyor | — (canlıda kullanılmıyor) | — | ⬜ yok |
| 19 | 0 | Kullanılmıyor | — (canlıda kullanılmıyor) | — | ⬜ yok |
| 20 | 0 | Kullanılmıyor | — (canlıda kullanılmıyor) | — | ⬜ yok |

**20 slotun 6'sı bizde karşılık buluyor (%30); 14'ü hiç yazılmamış.** 17–20 canlıda kullanılmıyor → kapsam dışı.

Ek canlı anahtarlar: `Ranking1..7` (7 sıralama türü) · `MaxLevelDanhHieu=20` · `MaxLevelQuanHam=12` · `MaxLevelTuChan=22` · `MaxLevelHonHoan=50` · `JewelBankTab=2` · `ButtonClassUP` · shop birim butonları (Wcoin C/P/G, JwBless, JwSoul, Chaos, Zens) · `CreateCharSeason` · `RF_GLOVE` / `MG_HELM`

---

## 2. CANLI VARLIK → ÖZELLİK EŞLEMESİ (`Data/SPK`)

| Varlık | Özellik |
|---|---|
| `ai_newui_skill2.ozj` | beceri listesi AI |
| `ai_newui_skill3.ozj` | beceri listesi AI |
| `Arrow.ozt` | menü oku |
| `Banking.ozt` | banka görseli |
| `BgID.ozt` | arka plan |
| `btn_AutoHp.spk` | AutoHp butonu (SPK_AutoHp) |
| `Btn_AutoPK.spk` | AutoPK butonu |
| `Btn_A_Inv.spk` | envanter butonu |
| `CTCMiniMap.ozj` | minimap |
| `dollars.spk` | dolar göstergesi |
| `formskillist.spk` | SPK beceri listesi formu |
| `freecoin.ozt` | freecoin (shop birimi) |
| `game-icon_android.ozt` | platform ikonu (Android) |
| `game-icon_ios.ozt` | platform ikonu (iOS) |
| `game-icon_pc.ozt` | platform ikonu (PC) |
| `game-mic.ozt` | mikrofon ikonu |
| `game-mic_push.ozt` | mikrofon ikonu (push) |
| `iBankButton.spk` | JewelBank sekmesi |
| `IconPkON.spk` | PK ikonu |
| `IconSale.spk` | satış ikonu |
| `iMoney.ozt` | shop para birimi |
| `ingame_Bt_Reset.ozt` | Reset butonu (slot 04) |
| `ItemSlotShop.ozt` | shop slotları |
| `jt_rbutton_attack.spk` | atak round butonu |
| `liliang.spk` | para birimi göstergesi |
| `MasterBoxSS5.ozt` | master kutusu |
| `MenuBarCenter.spk` | menü çubuğu orta blok |
| `MenuBarLeft.spk` | menü çubuğu sol blok |
| `MenuBarRight.spk` | menü çubuğu sağ blok |
| `Minimap_positionB.ozt` | minimap pozisyon |
| `newui_btn_empty_small.spk` | küçük boş buton |
| `newui_item_subpet.spk` | shop alt-pet öğesi |
| `newui_menu01A.ozt` | menü buton seti 1 |
| `newui_menu02A.ozt` | menü buton seti 2 |
| `newui_menu03A.ozt` | menü buton seti 3 |
| `newui_number2.spk` | sayı göstergesi |
| `SquareSkillList.spk` | kare beceri listesi |

> Kök dizinde 62 giriş, alt dizinlerle birlikte 2432 dosya (37 tanesi özelliğe eşlendi). Bunlar canlı istemcinin SPK arayüzünü oluşturuyor; pakete kopyalamak (2e.2) yetmez — **kodları** da yazılmalı.

---

## 3. MİMARİ TESPİT

**İyi haber:** `Source/5.Main/source/NewUIBCustomMenu.cpp` (1.182 satır) SPK tarzı pencereler için hazır widget katmanı içeriyor:

`gDrawWindowCustom` · `DrawWindowCustomMini` · `DrawButton` · `DrawButtonGUI` · `RenderCheckBox(Mini)` · `RenderCheckOption` · `RenderInputBox` · `RenderGroupBox` · `gItemBoxInv` · `RederBarOptionW` · `Openning/ClosingProcess`

Sıfırdan UI motoru yazmaya gerek yok: her özellik penceresi bu katmanı kullanarak birkaç yüz satırda yazılabilir.

| Katman | Durum |
|---|---|
| Widget çizim katmanı | ✅ var (NewUIBCustomMenu) |
| 16 slotlu özellik menüsü | ❌ yok |
| Özellik pencereleri (14 adet) | ❌ yok |
| SPK paket/opcode kaydı (istemci tarafı) | ❌ yok |
| Menü görselleri (MenuBar*.spk, newui_menu*.ozt) | ✅ canlı pakette mevcut |

---

## 5. İŞ EMİRİ

| Adım | İş | Bağımlılık | Kabul kanıtı |
|---|---|---|---|
| **3C.0** | `SPKMenuBar` — 20 slot, `GetEngine.ini` okuma, `MainFrame`/`NewUIManager` entegrasyonu, `MenuBar*.spk` + `newui_menu*.ozt` çizimi | NewUIBCustomMenu | Menü açılır, slotlar canlıyla aynı sırada, tıklamada pencere açılır |
| **3C.1** | **Protokol kaydı** — `SPKFeature.h`: her özellik için paket yapısı + opcode bölgesi; GS tarafıyla eşleme | docs/11 | 14 özellik için gönder/al listesi |
| **3C.2** | Slot 03 **Relife** dikey dilimi (pencere + paket + GS) | 3C.0, 3C.1 | Kendi sunucumuzda E2E: buton → paket → GS yanıtı → ekran güncellemesi |
| **3C.3** | Slot 04/05/06/07/08 (Reset, DanhHieu, Spin, VIP, MocNap) | 3C.2 şablonu | her biri kendi E2E kanıtı |
| **3C.4** | Slot 11/13/14 (HonHoan, TuLuyen, QuanHam) — gelişim zinciri | 3C.2 | config + ekran birlikte |
| **3C.5** | Slot 12/16 (XShop, ExtendShop) — ekonomi | 3C.1 | ürün listesi canlı config ile aynı |
| **3C.6** | Slot 01/02 (Ranking 7 tür, Event saati) | 3C.2 | sıralama listesi canlı ile aynı |
| **3C.7** | Slot 09/10/15 (ChangeClass, ChangePass, Yıldız) | 3C.2 | — |

**Ölçek notu:** 3C.0 + 3C.1 bir kez yapılır; sonraki her özellik ortalama 1 pencere + 1 paket + 1 sunucu kancası. İlk üçü (Relife, Reset, DanhHieu) şablonu doğrulayacak.
