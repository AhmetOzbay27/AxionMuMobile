# 13 — 2c.1 ÖNCELİK İŞ EMRİ (60 modül · oyun akışı etkisine göre dizilim)

> **Tarih:** 01.10.2026 14:05 · **Faz:** 2c.1
> **Kaynaklar:** [05-SPK-MODUL-ENVANTERI.md](05-SPK-MODUL-ENVANTERI.md) §3
> (59 modül + #60 EventGvG) + [12-MUIG68-CAPRAZ-KONTROL.md](12-MUIG68-CAPRAZ-KONTROL.md)
> §4/§6 (B grubu 2c kalemleri, açık kalemler, C grubu OFF kararı).
> **Önceki dizim** (05 §3'teki P1/P2/P3) içerik-temelli idi; bu iş emri
> **oyun akışı etkisine göre** yeniden dizer — kural: kullanıcı kararı
> (01.10.2026 14:04): **core gameplay > event > QoL > anticheat > kosmetik**.

## 1. SINIF TANIMLARI

| Sınıf | Tanım | Tipik modüller |
|---|---|---|
| **A — Core gameplay** | Oynanışın işleyişi: savaş/hasar mekaniği, karakter gelişim zinciri, reset, çekirdek ekonomi/ödeme, görev, ağ altyapısı | Harmony, TuLuyen ailesi, ResetChange, MocNap, MonsterSkill |
| **B — Event** | Zamanlı/katılımlı etkinlikler ve event iskeleti | EventMainManager, GvG, Sky, GreatPK |
| **C — QoL** | Deneyim kolaylığı ve ikincil içerik: shop genişlemesi, botlar, komut/UI, sıralama, yerelleştirme | ExtendShop, BotOnline, RankingServer |
| **D — Anticheat** | Güvenlik: hesap/eşya/limit koruması | ResetLimiter, PassLock, ChangePass |
| **E — Kosmetik** | Görsel/teknik süs: isim rengi, log formları, lisans | CustomNameColor, LogErrorForm |

> **Sıralama kuralı:** önce sınıf (A→B→C→D→E), sınıf içinde uygulama
> mantığı (mekanik → gelişim → ekonomi → altyapı; event iskeleti → tekil
> event'ler; …). Aynı sınıf içindeki sıra, bağımlılık/erişim sıklığına göre
> öneridir; uygulama sırasında kaynak durumu (donor hazır / sıfırdan) göre
> esnetilebilir.

---

## 2. A GRUBU — CORE GAMEPLAY (24 modül · EN YÜKSEK ÖNCELİK)

| Sıra | # | Modül | Sınıf(map) | Canlı config | Kaynak | Not |
|---:|---:|---|---|---|---|---|
| A1 | 4 | SPK_Harmony | CustomHarmony | CustomHarmony.xml | sıfırdan | item güçlendirme — ✅ **2c.1-A TAMAM** (canlı CustomHarmony.xml şeması + dispatcher eşlemesi; `8af2a6f3a` sonrası ayrı commit, CHANGELOG'a bak) |
| A2 | 23 | SPK_MonsterSkill | CCustomMonsterSkill | CustomMonsterSkill.txt | sıfırdan | savaş mekaniği |
| A3 | 37 | CustomItemSetPro | CustomSetDameItem | CustomItemSetPro.xml | sıfırdan | set bonus hasarı |
| A4 | 36 | CustomItemPro | SystemItemChanger | CustomItemPro.xml | sıfırdan | item dönüştürme |
| A5 | 38 | CustomNewBuff | CustomNewBuff | CustomNewBuff.xml | sıfırdan | buff |
| A6 | 55 | CustomStartItemDame | CCustomStartItemDame | CustomStartItemDame.txt | sıfırdan | hasar dengesi |
| A7 | 56 | CustomStartSetItemDame | CCustomStartSetItemDame | CustomStartSetItemDame.txt | sıfırdan | hasar dengesi |
| A8 | 57 | ItemExcellentOptionRate | CItemExOptionRate | — | sıfırdan | excelente oran |
| A9 | 5 | SPK_TuLuyen | cTuLuyen | TuLuyen.xml | sıfırdan | gelişim |
| A10 | 6 | SPK_QuanHam | cQuanHam | QuanHam.xml | sıfırdan | gelişim |
| A11 | 7 | SPK_HonHoan | cHonHoan | HonHoan.xml | sıfırdan | gelişim |
| A12 | 8 | SPK_DanhHieu | cDanhHieu | DanhHieu.xml | sıfırdan | unvan/gelişim |
| A13 | 9 | SPK_Relife | TaiSinh | Relife.xml | sıfırdan | reincarnation |
| A14 | 10 | SPK_DungLuyen | cDungLuyen | DungLuyen.xml | sıfırdan | geliştirme |
| A15 | 15 | ResetChange | CResetChange | ResetChange.txt | sıfırdan | reset |
| A16 | 13 | B_MocNap | CSystemMocNap | CustomMocNap.xml | sıfırdan | ödeme — **+ §6.2 birleşim (bizim MocNap + CB_AutoNapGame, canlı LOG_MOC_NAP)** |
| A17 | — | MasterReset bağlantısı | (map'ten çözülecek) | — | canlı reverse | docs/12 §6.3: DGCommandMasterResetRecv → tablo kaynağı |
| A18 | 40 | GuildUpgrade | cSystemGuildUpgrade | GuildUpgrade.txt | sıfırdan | guild ilerleme |
| A19 | 50 | ExWinQuestSystem | ExWinQuestSystem | CreationQuestSystem.xml + CreationQuestData_1.ini | sıfırdan | quest akışı |
| A20 | 44 | SocketConnection | CConnection | — | sıfırdan | ağ altyapısı |
| A21 | 45 | SocketManagerModern | CSocketManager (modern) | — | sıfırdan | ağ altyapısı |
| ✅ | 18 | SkillDamage | CSkillDamage | — | **2b.0 tam** | parite hazır |
| ✅ | 2 | SPK_AddBuff | AddBuffer | AddBuff.txt | **2b.0 tam** | parite hazır |
| ✅ | 3 | SPK_AutoHp | CAUTOHP | — | **2b.0 tam** | parite hazır |
| ✅ | 19 | CustomJewelBank | CCustomJewelBank | CustomJewelBank.xml | **2b.0 tam** | parite hazır |

**A toplam: 21 iş kalemi + 4 tamamlanmış = 25 kayıt (60'ın 24'ü + MasterReset ek kalem).**

---

## 3. B GRUBU — EVENT (9 modül)

| Sıra | # | Modül | Sınıf(map) | Kaynak | Not |
|---:|---:|---|---|---|---|
| B1 | 1 | SPK_EventMainManager | EventMainManager | sıfırdan | **event iskeleti** — diğer event'lerin taşıyıcısı, B grubunun ilk işi |
| B2 | 60 | **EventGvG** (YENİ keşif) ✅ **iskelet kuruldu** (26.10.01 14:31) | CGvGEvent | sıfırdan (canlı 7 anahtar) | ServerInfo 7 anahtar okunuyor (exe'de 7/7), donor header birebir, state machine iskeleti User.cpp MainProc'de; tam uygulama (NPC/Dialog/rank) 2c.1-B2 |
| B3 | 31 | ActiveInvasions | CActiveInvasions | sıfırdan | invasion takibi |
| B4 | 27 | SPK_CastleEvent | CastleStartGuild | sıfırdan | castle etkinliği |
| B5 | 32 | BEventThanMa | CThanMaChien | sıfırdan | ThanMaChien |
| B6 | 33 | SkyEvent | cSkyEvent | sıfırdan | Sky |
| B7 | 34 | TEventGreatPK | CCustomGreatPK | sıfırdan | GreatPK |
| B8 | 35 | CustomDameBoss | VPDameBoss | sıfırdan | boss hasar etkinliği |
| B9 | 17 | ReiDoMu | CReiDoMU | sıfırdan | ReiDoMU — **E-06'da gEventName hattına bağlandı (işlevsel hazır iskelet)** |

---

## 4. C GRUBU — QoL (21 modül)

| Sıra | # | Modül | Sınıf(map) | Kaynak | Not |
|---:|---:|---|---|---|---|
| C1 | 22 | SPK_ItemTrader | BotThuMua(er) | sıfırdan | ⚠️ **E-11 ile işlev paritesi alındı** (canlı TXT Read + Alchemy); bu kalem fark-kapatma olarak çalışır |
| C2 | 47 | CongHuong | CongHuong (bot) | sıfırdan | ⚠️ E-02 ile canlı CongHuong.txt deploy edildi; modül gövdesi bekliyor |
| C3 | 20 | SPK_ExtendShop | CExtendShop | sıfırdan | shop genişleme |
| C4 | 21 | SPK_NewXShop | NewCashShop | sıfırdan | CustomShop.xml |
| C5 | 28 | BotOnline | ObjBotOnline | sıfırdan | bot |
| C6 | 29 | BotTradeMix | BotTradeMixCore | sıfırdan | bot ticaret — canlı BotTradeMix.txt yolu ServerInfo'da zaten geçiyor |
| C7 | 30 | CBotMixSystem | CBotMixSystem | sıfırdan | bot mix (BotTradeMix ile paylaşımlı) |
| C8 | 41 | RankingServer | cRanking | sıfırdan | sıralama |
| C9 | 24 | SPK_CmdSocket | CCommandUI | sıfırdan | komut UI |
| C10 | 26 | SPK_StatsInfo | CSGetInfoCharacter | CharOption.xml (paylaşımlı) | karakter bilgi |
| C11 | 46 | ViewInfoItem | ViewItemPlayer | sıfırdan | eşya görüntüleme |
| C12 | 14 | CharOption | CCustomCharOption | CharOption.xml | karakter opsiyon formu |
| C13 | 39 | CustomLuckySpin | CCustomLuckySpin | CustomVongQuay.xml | şans çarkı |
| C14 | 48 | PartySetPass | CPartySetPass | sıfırdan | party şifre |
| C15 | 49 | ChecklevelVip | CheckItemVip | sıfırdan | VIP kontrol |
| C16 | 51 | CustomRenameChar | CCustomRenameChar | sıfırdan | yeniden adlandırma |
| C17 | 52 | CustomReadGuildServer | (guild veri) | sıfırdan | guild veri okuma |
| C18 | 53 | EventItemBagUI | CEventItemBagUI | EventItemBagManager.txt | UI katmanı |
| C19 | 54 | ShopManagerUI | CShopManagerUI | ShopManager.txt | UI katmanı |
| C20 | 11 | SPK_MessLang | cMessageNew | Message.xml (yeni bloklar) | yerelleştirme |
| C21 | 12 | ZzzToolKit | SPK_ToolKitMain | — (SPK\ log) | SPK log/araç altyapısı |

---

## 5. D GRUBU — ANTICHEAT (3 modül)

| Sıra | # | Modül | Sınıf(map) | Kaynak | Not |
|---:|---:|---|---|---|---|
| D1 | 16 | ResetLimiter | ResetLitmitLock | sıfırdan | reset sınır kilidi (exploit engeli) |
| D2 | 43 | PassLock | ItemPassLocker | sıfırdan | eşya kilidi (hırsızlık koruması) |
| D3 | 42 | ChangePass | ChangePassOption | sıfırdan | hesap şifre güvenliği |

---

## 6. E GRUBU — KOSMETİK (3 modül · EN DÜŞÜK ÖNCELİK)

| Sıra | # | Modül | Sınıf(map) | Kaynak | Not |
|---:|---:|---:|---|---|---|
| E1 | 25 | SPK_CustomNameColor | cCustomNameColor | sıfırdan | isim rengi (CustomNameColor.ini) |
| E2 | 58 | LogErrorForm | CLogErrorForm | sıfırdan | hata log formu |
| E3 | 59 | GetLicenseID | (lisans) | sıfırdan | opsiyonel lisans — parite gerekmiyor, en sonda |

---

## 7. SAYIM + KAPSAM DIŞI

**Sınıflandırma toplamı: 24 (A) + 9 (B) + 21 (C) + 3 (D) + 3 (E) = 60 ✓**
(A'da 4'ü ✅ 2b.0 tam + 21 iş + MasterReset ayrı bağlantı kalemi; B'de
EventGvG #60 dahil.)

- **Zaten tam (iş emrinde bekleyen iş değil):** SkillDamage, AddBuff, AutoHp,
  CustomJewelBank (2b.0 ✅); ThuMuaDoExc/CongHuong config'leri E-11/E-02 ile
  deploy edildi (C1/C2 fark-kapatma).
- **C grubu OFF (docs/12 §6.4):** 19 MUIG-özel modül (APIGameGuard, CGMHardwareId,
  ChatManager, MasterResetTable, MultiLanguage, BlackList-donör, …) — canlıda
  iz yok, **taşınmaz** (varsayılan karar; onay gerekirse ayrı iş emri).
- **Parite kapsamı dışı:** ThuongDanhBoss (docs/12 §4 — canlıda sıfır iz).

## 8. UYGULAMA SIRASI ÖNERİSİ (dalga planı)

1. **2c.1-A dalgası:** A1→A21 (mekanik → gelişim → ekonomi → altyapı).
   İlk üç iş: Harmony (config şeması hazır), MonsterSkill (txt), SetPro (xml).
2. **2c.1-B dalgası:** EventMainManager iskeleti → sonra tekil event'ler
   (GvG config anahtarları docs/12 §4'te).
3. **2c.1-C dalgası:** C1-C2 (E-11/E-02 fark-kapatma) → shop/bot → UI.
4. **2c.1-D/E:** anticheat → kosmetik (opsiyonel).

Her iş: uygula → derle → canlı config/kanıt doğrula → docs + CHANGELOG saat
damgası → commit (proje standardı).

---

**Hazırlayan:** Buffy (Codebuff) · **01.10.2026 14:05**
