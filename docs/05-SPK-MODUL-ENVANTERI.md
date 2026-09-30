# 05 — SPK MODÜL ENVANTERİ (Faz 2a.1 çıktısı — FAZ 2c İŞ EMRİ)

> Tarih: 30.09.2026. Kaynak kanıtları: canlı `GameServer.map` (242 sınıf) +
> canlı `GameServer.pdb` (367 cpp yolu, 277 GS proje dosyası) + 06 envanteri
> config bulguları. Ham veriler `BuildLog\envanter\` altında (canli_gs_s.txt,
> bizim_gs_s.txt, canli_ek_dosyalar.txt, ortak_dosyalar.txt, bizim_ek_dosyalar.txt,
> canli_spk_s.txt, pdb_cpp_yollari.txt, pdb_linker_cmd.txt).

---

## 1. CANLI GS PROJE YAPISI (PDB'den kesin)

- Canlı derleme yolu: `D:\Mu-Mobile\MuSPK\Source\ExGameServer\GameServer\`
- **277 cpp** derleniyor; **218'i bizim kaynağımızda da VAR** (isim paritesi).
- **SPK modüllerinin tamamı `GameServer\SPK\` alt klasöründe (57 cpp).**
- Ara yapı: `D:\BuildMU\Android\ExGameServer\` (+ spk_messlang.obj yolu →
  SPK_MessLang tek birim değil, alt katman da var).
- Linker: VS2022 14.44.35207 (v143!), /LTCG /OPT:REF /OPT:ICF, SPKThemeManiFest.xml.
- Canlı-özel dosyalardan 1'i proje dışı klasörde: `Source\Include\Math.cpp`.

## 2. KARŞILAŞTIRMA ÖZETİ

| Ölçüm | Değer |
|-------|-------|
| Canlı GS cpp (277) | bizim 281 ile karşılaştırıldı |
| Ortak (isim paritesi) | 218 |
| **Sadece canlıda (eksik modül)** | **59** |
| Sadece bizde (bizim eklerimiz) | 63 |

> **Sayı düzeltmesi:** Map analizi 58 demişti; PDB dosya analizi **59 dosya**
> verdi (SkillDamage.cpp map'te yakalanamamıştı). Kesin sayı: **59**.

---

## 3. EKSİK 59 MODÜL — KATEGORİLİ İŞ EMRİ

### K-1) SPK_ önekli çekirdek modüller (20) — `GameServer\SPK\` içinden
| # | Dosya | Sınıf (map) | Canlı config | Öncelik |
|---|-------|-------------|--------------|---------|
| 1 | SPK_EventMainManager.cpp | EventMainManager | — | P1 |
| 2 | SPK_AddBuff.cpp | AddBuffer* | AddBuff.txt | P1 |
| 3 | SPK_AutoHp.cpp | CAUTOHP | — | P1 |
| 4 | SPK_Harmony.cpp | CustomHarmony | CustomHarmony.xml | P1 |
| 5 | SPK_TuLuyen.cpp | cTuLuyen | TuLuyen.xml | P1 |
| 6 | SPK_QuanHam.cpp | cQuanHam | QuanHam.xml | P1 |
| SPK-15 | SPK_HonHoan.cpp | cHonHoan | HonHoan.xml | P1 |
| SPK-16 | SPK_DanhHieu.cpp | cDanhHieu | DanhHieu.xml | P1 |
| SPK-17 | SPK_Relife.cpp | TaiSinh | Relife.xml | P1 |
| SPK-18 | SPK_DungLuyen.cpp | cDungLuyen | DungLuyen.xml | P1 |
| SPK-19 | SPK_ExtendShop.cpp | CExtendShop | ExtendShop.xml | P2 |
| SPK-20 | SPK_NewXShop.cpp | NewCashShop | CustomShop.xml | P2 |
| SPK-21 | SPK_ItemTrader.cpp | BotThuMua(er) | ThuMuaDoExc.txt | P2 |
| SPK-22 | SPK_MessLang.cpp | cMessageNew | Message.xml (yeni bloklar) | P1 |
| SPK-23 | SPK_MonsterSkill.cpp | CCustomMonsterSkill | CustomMonsterSkill.txt | P2 |
| SPK-24 | SPK_CmdSocket.cpp | CCommandUI | CustomCommandSocket.xml | P2 |
| SPK-25 | SPK_CustomNameColor.cpp | cCustomNameColor | CustomNameColor.ini | P2 |
| SPK-26 | SPK_StatsInfo.cpp | CSGetInfoCharacter | CharOption.xml | P2 |
| SPK-27 | SPK_CastleEvent.cpp | CastleStartGuild | — | P2 |

### K-2) SPKViet özel sistemler (14) — SPK\ klasörü
| # | Dosya | Sategori | Canlı config | Öncelik |
|---|-------|----------|--------------|---------|
| V-01 | B_MocNap / B_MocNap.cpp | ödeme/recharge (móc nap) | CustomMocNap.xml | P1 |
| V-02 | BEventThanMa.cpp | CThanMaChien (savaş) | — | P2 |
| V-03 | CharOption.cpp | CCustomCharOption | CharOption.xml | P1 |
| K-04 | ChangePass.cpp + PassLock.cpp | ChangePassOption + ItemPassLocker | — | P2 |
| V-03b | ChecklevelVip.cpp | CheckItemVip | — | P3 |
| V-04 | CustomDameBoss.cpp | VPDameBoss | — | P2 |
| V-05 | CustomItemPro.cpp | SystemItemChanger | CustomItemPro.xml | P2 |
| V-06 | CustomItemSetPro.cpp | CustomSetDameItem | CustomItemSetPro.xml | P2 |
| MoveOptionNew | (dosya yok; MoveOptionNew sınıfı mevcut dosyalara gömülü) | — | MoveOptionNew | P3 |
| V-07 | CustomJewelBank.cpp | CCustomJewelBank | CustomJewelBank.xml | P1 |
| V-07b | CustomLuckySpin.cpp | CCustomLuckySpin | CustomVongQuay.xml | P2 |
| V-08 | CustomRenameChar.cpp | CCustomRenameChar | — | P3 |
| V-09 | CustomNewBuff.cpp | CustomNewBuff | CustomNewBuff.xml | P2 |
| V-10 | GuildUpgrade.cpp | cSystemGuildUpgrade | GuildUpgrade.txt | P2 |
| V-11 | RankingServer.cpp | cRanking | — | P2 |
| V-12 | ResetChange.cpp | CResetChange | ResetChange.txt | P1 |
| V-13 | ResetLimiter.cpp | ResetLitmitLock | — | P1 |
| V-14 | SkyEvent.cpp | cSkyEvent | — | Tema |
| V-15 | TEventGreatPK.cpp | CCustomGreatPK | — | P2 |
| V-16 | ExWinQuestSystem.cpp | ExWinQuestSystem | CreationQuest*.{ini,xml} | P2 |
| V-17 | CustomReadGuildServer.cpp | (guild okuma) | — | P3 |
| V-18 | GetLicenseID.cpp | lisans | — | P3 (opsiyonel) |
| V-19 | ZzzToolKit.cpp | SPK_ToolKitMain + yardımcılar | — | P1 |
| V-20 | ItemExcellentOptionRate.cpp | CItemExOptionRate | — | P3 |
| V-21 | LogErrorForm.cpp | CLogErrorForm | — | P3 |
| V-22 | ShopManagerUI.cpp | CShopManagerUI | — | P3 |
| V-23 | EventItemBagUI.cpp | CEventItemBagUI(+ManagerUI) | EventItemBagManager.txt | P3 |
| V-24 | CustomStartItemDame.cpp | CCustomStartItemDame | CustomStartItemDame.txt | P3 |
| V-25 | CustomStartSetItemDame.cpp | CCustomStartSetItemDame | CustomStartSetItemDame.txt | P3 |
| V-26 | CongHuong.cpp | CongHuong (bot yön) | CongHuong.txt | P3 |
| V-27 | BotOnline.cpp | ObjBotOnline | BotOnline.txt | P2 |
| V-28 | BotTradeMix.cpp + CBotMixSystem.cpp | BotTradeMixCore + CBotMixSystem | BotTradeMix.txt | P2 |
| V-29 | SkillDamage.cpp | CSkillDamage | — | P1 (MUIG'de hazır) |
| V-30 | CustomJewelBank.cpp | CCustomJewelBank | CustomJewelBank.xml | P1 (MUIG'de hazır) |
| V-31 | SPK_AutoHp.cpp | CAUTOHP | — | P1 (MUIG'de hazır) |

K-1/K-2 tablolarında tekrar eden satırlar birleştirilacak; kesin liste:
`BuildLog\envanter\canli_ek_dosyalar.txt` (59 satır).

**Öncelik tanımları:**
- **P1 = Çekirdek oyun:** EventMainManager (event iskeleti), AddBuffer,
  SPK_AutoHp (oyuncu korunma), Harmony (item güçlendirme), TuLuyen/QuanHam/
  HonHoan/DanhHieu/Relife (karakter gelişim sistemi), ZzzToolKit (SPK alt yapı),
  MocNap (ödeme), ResetChange/ResetLimiter (reset sistemi), SkillDamage +
  CustomJewelBank (MUIG'den transfer).
- **P2 = Önemli içerik:** shop/economy (ExtendShop, NewXShop, ItemTrader),
  bot sistemi (BotOnline, BotTradeMix, CBotMixSystem), CustomEventTime,
  TEventGreatPK, BEventThanMa, CustomDameBoss, CustomItemPro/SetPro, GuildUpgrade,
  RankingServer, CharOption, CustomNewBuff, MessLang (dil blokları), CmdSocket,
  CustomNameColor, StatsInfo, CastleEvent, MonsterSkill, ChangePass/PassLock,
  CustomLuckySpin, CustomRenameChar.
- **P3 = Tamamlama:** UI sınıfları (ShopManagerUI, EventItemBagUI, LogErrorForm),
  ItemExcellentOptionRate, ChecklevelVip, GetLicenseID, MoveOptionNew (gömülü),
  CustomReadGuildServer, CustomStartItemDame/SetSetItemDame, CongHuong.

---

## 4. KAYNAK STRATEJİSİ

### 4a. MUIG donor transferi (hızlı kazanç — Faz 2b ile birlikte)
| Modül | MUIG donor konumu |
|-------|-------------------|
| SkillDamage.cpp/h | `.../Source/Source/GameServer/` |
| CustomJewelBank.cpp/h | `.../Source/Source/GameServer/` |
| CAUTOHP (SPK_AutoHp benzeri) | `.../Source/Source/GameServer/` |
| AddBuffer | `.../SPK_AddBuff benzeri işlev — MUIG'de AddBuffer sınıfı var` |
| *Not:* MUIG modülü canlı SPK sürümünden ESKİ olabilir → parite için davranış karşılaştırması gerekir (event/protocol izleri) | |

### 4b. Sıfırdan yazım (54-55 modül)
- **Birim adı:** tek modül = tek PR benzeri adım: header + cpp + vcxproj ekleme
  + config üretimi + derleme + CHANGELOG + commit.
- **Format çözümlemesi:** her modül için canlı config dosyası (Sub-1\Data\SPK\)
  birebir okunacak (XML/INI/txt yapısı korunur); davranış kanıtı: canlı loglar
  (GameServer\SPK\*.txt), map sembol listesi (modülün public API yüzü),
  Main.exe/Engine.exe protokol string'leri.
- **Entegrasyon noktaları:** map sembolleri hangi çekirdek dosyalara dokunduğunu
  gösterir (ör. cTuLuyen çağrıları User.cpp/Protocol.cpp içinde aranacak).

## 5. DOĞRULAMA YÖNTEMİ (her modül için)

1. Kaynak derleniyor (GS tam derleme hatasız).
2. Config dosyası canlı formatında üretilir, GS açılışında hatasız okunur.
3. Modülün map sembolleri yeni binary'de belirir (yeni derlemenin map'inden
   sınıf adı grep'i).
4. Cekirdek dosyalara eklenen cagri noktalari listelenir (CHANGELOG).
