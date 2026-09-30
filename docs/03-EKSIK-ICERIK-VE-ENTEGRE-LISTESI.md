# 03 — EKSİK İÇERİK VE ENTEGRASYON LİSTESİ

> Canlı SPK'da VAR, bizim kaynakta EKSİK olan her şey + nereden entegre
> edileceği. Her satır: kaynak → hedef → durum. Tamamlananlar ✅ ile
> işaretlenir ve CHANGELOG'a kaydedilir.

Durum kodları: ⬜ eksik · 🔄 işlemde · ✅ entegre · ❓ araştırılacak

---

## A. SUNUCU (GameServer) — 58 eksik modül (map tabanlı kesin sayı)

> Kaynak kanıtı: canlı `GameServer.map` sembol analizi (bkz.
> [06-CANLI-SISTEM-ENVANTERI.md](06-CANLI-SISTEM-ENVANTERI.md)). Ham listeler:
> `BuildLog\envanter\canlida_bizde_yok.txt`, `muigden_alinabilir.txt`,
> `sifirdan_yazilacak.txt`.

| # | Modül / Özellik | Kaynak | Hedef | Durum |
|---|-----------------|--------|-------|-------|
| A-01 | 4 MUIG donor modülü: AddBuffer, CAUTOHP, CCustomJewelBank, CSkillDamage | MUIG donor GS | `Source\4.GameServer\` | ⬜ Faz 2b |
| A-02 | 54 SPK-özel modül — tam liste 06 envanter §2: SPKViet (CThanMaChien, cTuLuyen, cQuanHam, cHonHoan, cDanhHieu, cRanking, cSkyEvent, cSystemGuildUpgrade, cMessageNew, cCustomNameColor, cZenDrop, cCItemLevel, CSystemMocNap, CReiDoMU, TaiSinh, ExWinQuestSystem, CustomHarmony) · Bot (BotTradeMixCore, CBotMixSystem, ObjBotOnline) · Event (EventMainManager, CActiveInvasions, CEventHideAndSeek, CEventRunAndCatch, CEventKillAll, CastleStartGuild) · UI (CCommandUI, CEffectManagerUI, CEventItemBagUI, CEventItemBagManagerUI, CMapManagerUI, CShopManagerUI, CLogErrorForm) · Item/Dmg (CustomDameItem, CustomSetDameItem, CCustomStartItemDame, CCustomStartSetItemDame, CItemExOptionRate, CItemSubMix, CustomNewBuff, CheckItemVip, ItemPassLocker, SystemItemChanger, MoveOptionNew) · Karakter (CCustomCharOption, CCustomChangeClass, CCustomRenameChar, CCustomLuckySpin, CCustomGreatPK, CPartySetPass, CResetChange, ResetLitmitLock, CSGetInfoCharacter, ViewItemPlayer) · Ekonomi (CExtendShop, NewCashShop, VPDameBoss) · Diğer (CSocketMaker, SPK_ToolKitMain) | ❓ yok — sıfırdan (map + PDB + canlı config analizi) | `Source\4.GameServer\` | ⬜ Faz 2c (2a.1 ile önceliklenecek) |
| A-03 | MuServer config seti (canlı 450 dosya; 24 SPK-özel config dahil) | Canlı `Sub-1\Data\` (SALT OKUNUR kopya) | `MuServer\...\Data\` | ⬜ modül modül 2c ile |

### A-03 alt kırılım — Data\SPK config tanınırlığı
- **Bizim GS zaten tanıyor (22):** BotTrader.xml, ChangeItem.xml, CustomBuyVip.txt, CustomCombo.txt, CustomCongHuong.xml, CustomDeathMessage.txt, CustomJewel.txt, CustomMix.txt, CustomMocNap.xml, CustomMonster.txt, CustomMonsterSkill.txt, CustomMove.txt, CustomNpcQuest.txt, CustomPet.txt, CustomPick.txt, CustomQuest.txt, CustomStartItem.txt, CustomTop.txt, CustomVongQuay.xml, CustomWing.txt, CustomWingMix.txt, ZenDrop.xml
- **MUIG donor'da var (4):** AddBuff.txt (AddBuffer), CongHuong.txt, CustomJewelBank.xml (CCustomJewelBank), ThuMuaDoExc.txt (BotThuMua)
- **Hiçbir kaynakta yok (24 — sıfırdan modül config'leri):** BotOnline.txt, BotTradeMix.txt, ChangeClass.xml, CharOption.xml, CreationQuestData_1.ini, CreationQuestSystem.xml, CustomCommandSocket.xml, CustomHarmony.xml, CustomItemPro.xml, CustomItemSetPro.xml, CustomNameColor.ini, CustomNewBuff.xml, CustomShop.xml, CustomStartItemDame.txt, CustomStartSetItemDame.txt, DanhHieu.xml, DungLuyen.xml, ExtendShop.xml, GuildUpgrade.txt, HonHoan.xml, QuanHam.xml, Relife.xml, ResetChange.txt, TuLuyen.xml

## B. İSTEMCİ (Main)

| # | Modül / Özellik | Kaynak | Hedef | Durum |
|---|-----------------|--------|-------|-------|
| B-01 | **SPK istemci format katmanı**: ConnectIP.bmd, ServerData.bmd, SPK.ini, Data\SPK okuma | `1Client\Engine.exe` + `Data\SPK\Config\*.bmd` analizi (yeni adım **2d.0**) | `Source\5.Main\` | ⬜ |
| B-02 | SPK client içerik varlıkları (Btn_AutoHp.spk, Btn_AutoPK.spk, ai_newui_skill*.ozj, Config\Info\) | `1Client\Data\SPK\` | istemci paketi | ⬜ 2d.0 sonrası |
| B-03 | GetMainInfo varyant birleşimi | SPK GetEngine referansı + MUIG MainInfo kaynağı | `Source\6.GetMainInfo\` | ⬜ 2a.5/2d karar |
| B-04 | Gömülü IP / config okuma düzeni (H-004) | Canlı değer: 192.168.0.150 | `Source\5.Main\` | ⬜ 2e.1 |
| B-05 | MUIG 68 özel modülünün canlı karşılığı kontrolü | MUIG donor | `Source\5.Main\` | ⬜ 2b.3 |

## C. ORTAK / ALTYAPI

| # | Kalem | Kaynak | Hedef | Durum |
|---|-------|--------|-------|-------|
| C-01 | MUIG'in 161 daha yeni ortak dosyası | MUIG donor | `Source\` (SPK taban) | ⬜ 2b.1-2b.2 diff matrisi sonrası grup grup |
| C-02 | DB şeması (DB_SQL_12.bak) ile GS beklentileri uyumu | `ServerTools\DB_SQL_12.bak` | test DB | ⬜ Faz 3 öncesi |

## D. BİLİNEN KÜÇÜK SAPMALAR (parite, düşük risk)

| # | Kalem | Detay | Durum |
|---|-------|-------|-------|
| D-01 | String sapmaları (Main) | `FCBad item index.` vs `Bad item index.` gibi ön-ek farkları — revizyon kaçağı; 2b'de donor revizyonu alınca çoğu kapanır | ⬜ |
| D-02 | `7 SO BAO MAT` stringi | Yalnız bizim derlemede (MUIG VN kaynak izi); canlıda yok | ⬜ |

---

### KAYNAK KONUMLARI (hızlı erişim)
- MUIG donor: `C:\Axion Mu Mobile\New Source Code\Source\Source\` (+Main5.2, +Encoder)
- SPK GetEngine referans binary: `C:\Axion Mu Mobile\Client and Tools\GetMain\`
- **SPK istemcisi (gerçek):** `C:\Axion Mu Mobile\Client and Tools\1Client\` (Engine.exe + SPK.ini + Data\SPK)
- Canlı sunucu (salt okunur): `C:\Axion Mu Mobile\4.MuServer\Sub-1\` (GameServer.map dahil)
- Farklı fork (parite hedefi DEĞİL): `C:\AxionMu\`
- Eski analiz raporları: `C:\Axion Mu Mobile\analiz\`
