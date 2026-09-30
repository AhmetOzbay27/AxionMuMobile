# 10 — 2b.1 DIFF MATRİSİ: 213 ORTAK DOSYA (SPK taban ↔ MUIG donor)

> Tarih: 30.09.2026. Kapsam: 2a.1 envanterindeki 218 ortak dosyadan 5'i
> donorde bulunmadığı için (BossGuild, ChangeClass, CustomPet, FakeOnline,
> ZenDrop — bunlar 09 raporunun E-kalemleri) çıkarıldı; **213 gerçek ortak
> dosya** analiz edildi. Ham veri: `BuildLog\envanter\2b1_diff_matrisi.csv`,
> script: `2b1_analiz.sh`.

---

## 1. METODOLOJİ

Her dosya için ölçümler:
- **birebir** — byte-byte aynı (cmp)
- **benzer%** — normalize satır kümesi (girinti/boşluk/yorum arındırılmış,
  tekilleştirilmiş) ortak/birleşik oranı → *yapısal eşdeğerliğin alt sınırı*
- **metot yüzeyi** — `Sınıf::Metot` kümesi: ortak / bizim-tekil / donor-tekil
- **ham diff** — donorde eklenen (+) / bizimde olan ama donörde olmayan (−) satır

Önemli yorum kuralı: **düşük benzerlik ≠ donor daha yeni.** Bizim taban SPK
hattı + MUIG seleksiyonudur; SPK-özel değişiklikler (Türkçe konfig anahtarları,
H-007 tipi opcode kararları, Android port uyarlamaları) benzerliği düşürür.
Donör otomatik alınması yalnız **G1'de** güvenlidir.

## 2. ÖZET SAYILAR

| Bant | Adet | Yorum |
|------|-----:|-------|
| Birebir aynı | 77 | işlem gerekmez |
| %90-100 (aynı değil) | 30 | **G1 — temiz donor alımı** |
| %70-89 | 25 | **G2 — kontrollü donor alımı** |
| %50-69 | 10 | **G3 — manuel birleştirme** |
| %30-49 | 40 | **G4 — SPK-özel şüpheli; dosya dosya karar** |
| <%30 | 31 | **G5 — iki farklı dal; satır satır çözümleme** |

Toplam: 213 ✓ (77+30+25+10+40+31)

## 3. RİSK GRUPLARI VE 2b.2 STRATEJİSİ

### G1 — Temiz donor alımı (30 dosya)
Donör sürüm al, derle, commit. Liste (ben% / diff):
ArcaBattle(100), CustomTop(100), CustomWingMix(100), DoubleGoer(98),
ImperialGuardian(98), KanturuBattleOfMaya(98), KanturuBattleStanby(97),
KanturuBattleOfNightmare(96), KanturuTowerOfRefinement(96), CastleDeep(95),
ComboSkill(95), LifeStone(91), BonusManager(90), Crywolf(90), CustomPick(90)
+ 15 dosya daha (CSV'de G1 filtresi). *Toplam diff etkisi küçük (≤30 satır/dosya).*

### G2 — Kontrollü donor alımı (25 dosya)
Donör alınır ama 2b.0 kancaları ve SPK-özel satırlar diff ile korunur.
Öne çıkanlar: CashShop(86), HackCheck(86), CrywolfStatue(88), ChaosCastle(87),
Duel(83), CrywolfAltar(85), JewelOfHarmonyOption(83), MemoryAllocator(82),
CustomCombo(82), CustomNpcQuest(81), Kalima(80), Reconnect(85 — 09 raporu
E-10 ile aynı kalem), ItemDrop(75), MapManager(75), Gate(73), Warehouse(77),
ItemValue(71), ItemMove(69), ItemOption(78), NpcTalk(78), EffectManager(65),
Command(61), Item(58), CustomMove(55), MemScript(50).

### G3 — Manuel birleştirme (10 dosya)
Her iki tarafta gerçek gelişme var: CustomDeathMessage(51),
CustomRankUser(52 — 09/E-08 ile birleşir), GameMain(52),
DefaultClassInfo(53), CustomMonster(77→dikkat: SPK alanları var),
CustomQuest(79), EventRunAndCatch(79), EventKillAll(84).

### G4 — SPK-özel şüpheli (40 dosya) — **toplu donor alımı YASAK**
Bizim tabanın SPK-özel içeriği yoğun; her dosya için satır satır "bu fark
SPK-özellik mi, eskimelik mi?" kararı. Kritik alt küme:
- **Protokol katmanı (dokunma-önce-çözümle):** Protocol(32), DSProtocol(31),
  CSProtocol(45), JSProtocol(37), ESProtocol(41), SocketManager(34),
  SocketManagerUdp(30), Connection(27→G5 sınırında), IpManager(33).
  *PacketManager zaten birebir aynı.* → **2b.2-P0: protokol diff'i özel
  oturumda opcode tablosuyla karşılaştırılacak.**
- **2b.0 kanca dosyaları (kancalarımızı bozma):** ServerInfo(7), ItemManager(13),
  CommandManager(18), ObjectManager(19), Attack(18), Protocol(32),
  DSProtocol(31), User(37). Bu 8 dosyada donor alımı yalnız "bizim kancalar +
  SPK satırları korunarak" yapılır.
- Diğerleri: BloodCastle, ChaosBox, Helper, Viewport, DevilSquare, Monster,
  IllusionTemple, ItemBag(Ex), CustomAttack, Guild, Log, ShopManager,
  SetItemType/ItemOption, MonsterManager, QuestReward, Fruit, SocketItemOption,
  GameMaster, OfflineMode(→09/E-09), 380ItemOption, EventHideAndSeek,
  GameServer, BotBuffer(→09/E-03), ItemStack, CastleSiege, ItemLevel, MapItem,
  MasterSkillTree, ThuMuaDoExc(→09/E-11), BotAlchemist(→09/E-02).

### G5 — İki farklı dal (31 dosya) — satır satır çözümleme
En büyük sapmalar: ServerInfo(7 — SPK konfig okuma hattı tümüyle farklı,
bizim 2b.0 kancalarımız burada), CustomStartItem(7), ItemManager(13),
ResetTable(11), CustomMix(15), Protect(15 — SPK güvenlik katmanı),
ItemBagManager(17), MonsterSetBase(17), Attack(18), CommandManager(18),
ObjectManager(19), CustomStore(18), SkillManager(21), PersonalShop(21),
BotAlchemist(21), ItemOptionRate(21), ServerDisplayer(22), Util(22),
CustomWing(23), DarkSpirit(23), JewelMix(23), CustomBuyVip(→09/E-05),
380ItemType(26), CustomEventTime(→09/E-06), Trade(26), CustomJewel(29→09/E-07),
CustomRankUser vb. Bu grupta 09 raporundaki E-kalemleriyle birleşik çalışılır.

## 4. 2b.2 UYGULAMA SIRASI (öneri)

1. **2b.2-A (hızlı kazanç):** G1'in 30 dosyası — 3 grup halinde al + derle +
   commit (risksiz, ~saatlik iş).
2. **2b.2-B:** G2 — dosya başına diff incelemesi + kanca koruması.
3. **2b.2-P0 (paralel, öncelikli):** protokol dosyalarının (Protocol/DS/CS/JS/ES/
   Socket*) diff tablosu — opcode/struct karşılaştırması; canlı exe ile
   doğrulama. Bu tablo olmadan hiçbir protokol dosyasına donor alınamaz.
4. **2b.2-C:** G3 + G4 (SPK-koruma listesiyle); 09 E-kalemleriyle birleşik.
5. **2b.2-D:** G5 — dosya dosya; 09 raporu stratejileri geçerli.

## 5. KURALLAR (2b.2 sırasında geçerli)

- 2b.0 kancaları (8 dosyadaki `// SPK (Faz 2b)` blokları) her alımda korunur.
- H-007 (0x35) kararı gibi opcode kararları tekrar bozulmaz; protokol diff
  tablosu (2b.2-P0) çıkmadan protokol dosyasına donor alımı yapılmaz.
- Her grup alımı: derle → GS boyut kaydı → CHANGELOG → commit (grup başına 1).
- SPK-özel satır işaretleri: `SPK`, Türkçe konfig anahtarları
  (GioiHan*, ChinhDame* vb.), `#if(GAMESERVER_UPDATE` blokları korunur.
