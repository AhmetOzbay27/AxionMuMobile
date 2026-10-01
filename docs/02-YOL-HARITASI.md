# 02 — YOL HARİTASI (adım adım çalışma planı)

> **Ana hedef:** Canlı SPK sunucu + istemcisiyle birebir parite. Parite tam
> olmadan v2 açılmaz. Her adım: uygula → derle/doğrula → CHANGELOG → commit →
> burada işaretle. **Adım atlamak yasak.**

Durum kodları: ⬜ bekliyor · 🔄 devam ediyor · ✅ tamamlandı · ⏸ engelli

---

## FAZ 2 — PARİTE (aktif faz)

### Faz 2a — Canlı SPK envanteri
> ✅ **TAMAMLANDI** (2a.1-2a.5, 30.09.2026) → sıradaki faz: **2b**.
- ✅ **2a.1** Canlı `GameServer.pdb` + `GameServer.map` analizi tamam →
  [05-SPK-MODUL-ENVANTERI.md](05-SPK-MODUL-ENVANTERI.md) yazıldı (kesin sayı
  **59 eksik dosya**; PDB'den tam dosya yolları çıkarıldı; 57 modül
  `GameServer\SPK\` alt klasöründe; kategori + öncelik + kaynak stratejisi hazır).
- ✅ **2a.2** Canlı GS exe'sinden string/resource envanteri → tamam; asıl verim
  canlı `GameServer.map`'ten 242 sınıf envanteri oldu (175 bizde var, 58 eksik).
  Detay: [06-CANLI-SISTEM-ENVANTERI.md](06-CANLI-SISTEM-ENVANTERI.md)
- ✅ **2a.3** Canlı `MuServer` config envanteri → tamam: Sub-1\Data 450 dosya,
  Data\SPK 25 config; tanınırlık tablosu (22 bizde / 4 MUIG'de / 24 SPK-özel).
- ✅ **2a.4** Canlı istemci envanteri → tamam: gerçek SPK istemcisi
  `Client (eski adı 1Client)\Engine.exe` (ConnectIP.bmd/ServerData.bmd/Data\SPK/SPK.ini hattı);
  bizim 5.Main'de SPK istemci desteği yok → yeni adım **2d.0** açıldı.
- ✅ **2a.5** GetMainInfo varyant analizi → **KARAR: tek hat = SPK GetEngine.**
  Kanıtlar: GetEngine.ini (IP 45.87.120.29:44405, v1.03.34, Türkçe yorumlu) =
  ConnectIP.bmd XOR 0x20 decode; SPK_CRCFILE.ini doğrulayıcı rapor. Uygulama
  2d.0/2d.1'de. Detay: 03 listesi "2a.5 KARARI" bölümü.

### Faz 2b — MUIG ortak dosya güncellemesi (161 dosya)
- ✅ **2b.0** İlk entegrasyon dalgası (30.09.2026): 4 MUIG donor modülü GS'ye
  eklendi — AddBuffer (PC_AddBuff), CAUTOHP, CCustomJewelBank, CSkillDamage;
  8 kanca dosyası (User, Protocol, ServerInfo, Attack, DSProtocol,
  ObjectManager, CommandManager.cpp/.h) + config şablonları (AddBuff.txt,
  CongHuong.txt). GS derlendi → 10.704.896 B; semboller PDB ile doğrulandı.
  H-007: 0x35 opcode'u donor paritesiyle AUTOHP'ye verildi.
- ✅ **2b.0-E** 12 "ezilen" dosyanın (E-01..E-12) üçlü karşılaştırma raporu
  (30.09.2026) → [09-EZILEN-12-DOSYA-KARSILASTIRMA.md](09-EZILEN-12-DOSYA-KARSILASTIRMA.md):
  bizim↔donor↔canlı map/exe/config analizi; dosya başına strateji
  (Kolay 3 / Orta 5 / Zor 4), uygulama sırası ve 6 config eksiği tespit edildi.
  Canlı revizyonun iki kaynağı da aştığı doğrulandı (BossGuild skor/ödül,
  BuyVip reload, Alchemist load koruması hiçbir kaynakta yok).
- ✅ **2b.1** Ortak dosya diff matrisi (30.09.2026) →
  [10-DIFF-MATRISI.md](10-DIFF-MATRISI.md): 213 gerçek ortak dosya analiz edildi
  (218 − 5 donorde-yok). Sonuç: 77 birebir, 30 G1 (temiz donor alımı), 25 G2
  (kontrollü), 10 G3 (manuel birleşim), 40 G4 (SPK-özel şüpheli — toplu alım
  yasak), 31 G5 (iki farklı dal). 2b.2 sırası: G1 hızlı kazanç → G2 → protokol
  P0 diff tablosu → G3/G4 (SPK korumalı) → G5. Ham veri:
  `BuildLog\envanter\2b1_diff_matrisi.csv`.
- 🔄 **2b.2** Risk gruplarına göre grup grup entegrasyon (grup tanımı:
  [10-DIFF-MATRISI.md](10-DIFF-MATRISI.md) G1-G5 + [09 raporu](09-EZILEN-12-DOSYA-KARSILASTIRMA.md) E-kalemleri).
  - ✅ **2b.2 / E-10 Reconnect** (30.09.2026): donör parti-slot düzeltmesi alındı
    (Index[1..4] → tüm slotlar), AutoResetEnable'daki bizim
    m_CommandResetAutoEnable kontrolü korundu (canlı konfig =1 kanıtıyla).
    GS Rebuild temiz → 10.704.896 B (özdeş boyut, /LTCG); PDB doğrulandı.
  - ✅ **2b.2-A dalga 1** (30.09.2026): G1'in 22 dosyası donör alındı; 8 dosya
    bağımlılık nedeniyle ertelendi (detay: CHANGELOG 2b.2-A).
  - ✅ **2b.2-B dalga 2** (30.09.2026): ertelenen 8 dosya + bağımlılık modülleri
    alındı — BonusManager, MossMerchant, MonsterAI, ImperialGuardian,
    RaklionSelupan, Crywolf, Raklion (+h'ler) ve yeni **CEventName modülü**
    (EventName.xml minimal config ile). MonsterSetBase'e array-tabanlı
    GetMonsterMap uyarlaması, Monster.cpp'ye gObjMonsterClearExpiredDamage eklendi.
    GS derlemesi temiz → 10.781.696 B.
  - ✅ **2b.2-P0** (30.09.2026): protokol opcode/struct diff tablosu
    → [11-PROTOCOL-P0-DIFF-TABLOSU.md](11-PROTOCOL-P0-DIFF-TABLOSU.md). Bulgu:
    dış opcode seti 5 dosyada birebir, metot yüzeyi farkı sıfır; fark gövdede
    (bizim-tekil SPK handler'lar vs donor-tekil Holy/SNS/Reqtime). Canlı kanıt
    (CTCMini klasörü) bizim tarafı doğruluyor → **donör protokol alımı yok**,
    bizim taban korunur. Donör SocketManager bağımlılığı (BlackList+APIGameGuard)
    2c'ye not edildi.
  - ✅ **2b.2-C dalga 3** (30.09.2026): G2'nin 24 dosyası incelendi → 7 alındı
    (CrywolfStatue, CrywolfAltar, Gate+h, ItemDrop+h, ItemMove+h, Command +
    Util'e PackFileEncrypt zinciri), 15 gerekçeli korundu (HackCheck güvenlik
    anahtarları, ChaosCastle/Kalima NORMALIZE_LEVEL chain'i yok, CashShop/Duel/
    EffectManager/CustomCombo/CustomNpcQuest/MapManager/CustomMove bağımlılık,
    Warehouse/ItemOption/MemScript G3'e, ItemValue/ItemValueTrade Level-stacking
    kanıtsız). Detay: CHANGELOG 2b.2-C. GS derlemesi temiz → 10.781.184 B.
  - ✅ **2b.2-E kalemleri dalga 4** (30.09.2026): E-05 yol geçişi
    (Custom\ → canlı SPK\ yolları: CustomBuyVip.txt + AddBuff.txt), E-08 bizim
    taban, E-12 ✅; E-01/E-02/E-03/E-04/E-06/E-09/E-11 gerekçeli 2c erteleme
    (canlı-kanıt şartı). Deploy config tamamlandı: Data\SPK\ (3 dosya) +
    EventTime.xml + BossGuild.xml + CustomJewel.txt asıl ağaca kopyalandı
    (config-pasifizasyonundaki test-ağacı hatası düzeltildi). GS temiz →
    10.781.184 B. Detay: CHANGELOG 2b.2-E.
  - ✅ **2b.2-D dalga 5** (30.09.2026): G3 manuel birleştirme — 4 dosya alındı
    (CustomDeathMessage, EventKillAll, CustomQuest +h'leri, EventRunAndCatch;
    CustomQuest GetLevel ve RAC gate=1 adaptasyonları), 6 gerekçeli korundu
    (CustomMonster CB_BXHDMG, GameMain G5-include'ları, DefaultClassInfo global
    ad-zinciri, ItemOption enum kayması, Warehouse DS-çifti, MemScript parser).
    GS temiz → 10.776.064 B.
  - ✅ **2b.2-F dalga 6** (30.09.2026): G4 bandı — 8 dosya alındı (GameMaster,
    ItemLevel, Log, EventHideAndSeek, ShopManager, SetItemType, 380ItemOption,
    ItemBagEx — whitespace+güvenli declare), 19 gerekçeli korundu (SPK-imleç:
    ChaosBox/CustomAttack/GameServer/User/Viewport; NORMALIZE_LEVEL:
    BloodCastle/DevilSquare/IllusionTemple; cascade: Helper/QuestReward derlemeyle
    yakalandı-geri alındı; enum/layout: SetItemOption/SocketItemOption;
    CastleSiege 2c'ye). GS temiz → 10.776.064 B.
  - ✅ **2b.2-G dalga 7** (30.09.2026): G5 bandı — 2 dosya alındı (JewelMix
    semantik-özdeş, 380ItemType+h Export-zinciri tamamlandı), 17 gerekçeli
    korundu (CustomStore/Protect/ServerDisplayer/ResetTable SPK-imleç,
    DarkSpirit/ItemOptionRate/Party/ItemBagManager modül-bağlantılı,
    Move/Quest GetLevel-zinciri). GS temiz → 10.775.552 B.
    **2b.2 TAMAMLANDI (7 dalga): 66 dosya alındı, 147 gerekçeli korundu,
    0 belirsiz.**
- ✅ **2b.3** MUIG-özel modül çapraz kontrolü (01.10.2026, 06:27–07:00) →
  [12-MUIG68-CAPRAZ-KONTROL.md](12-MUIG68-CAPRAZ-KONTROL.md): "68" etiketi
  netleştirildi → 62 donor-özel dosya (29 cpp modülü; 2b.0'ın 4 modülü düşülünce).
  6 kanıt hattı (vcxproj/sınıf/map+obj/PDB/exe-string/config+log) ile sınıflandırma:
  **7 canlıda VAR-parite tamam** (AUTOHP, PC_AddBuff, CustomJewelBank, SkillDamage
  = 2b.0 ✅; B_MocNap→bizde MocNap+CB_AutoNapGame paraleli; BotTrade→bizde
  BotTrader+ThuMuaDoExc; SkillDamageConfig→rate katmanı), **3 iş-kalemi 2c'ye**
  (EventGvG — YENİ KEŞİF: canlı ServerInfo 7 anahtar kanıtlı, bizde eksik;
  AntiSkillDelay→canlı SkillManager içi, bizde VAR; ThuongDanhBoss→canlıda sıfır iz),
  **19 canlıda YOK → OFF/taşınmaz** (APIGameGuard, CGMHardwareId, CGMEarring,
  CGMFlagNat, CGMPet, CMixGoblinExpansion, CharacterAdvance, ChatManager,
  ConsoleDebug, CustomExchangeCoin, EventFindPath, GMHolyItem, LogToFile,
  MasterResetTable, MultiLanguage, MyTimer, SendMessage, WindowsConsole,
  BlackList-donör). Kritik netleştirme: canlı BlackList.txt = IpManager
  (bizde VAR), donör CBlackList değil. Ham veri: BuildLog\envanter\muig68_*.
  **FAZ 2b TAMAMLANDI → sıradaki 2c.1.**

### Faz 2c — 57 eksik modülün yeniden yazımı
- ⬜ **2c.1** 05-SPK-MODUL-ENVANTERI.md'yi öncelik sırasına diz (oyun akışı
  etkisine göre: core gameplay > event > QoL > anticheat > kosmetik).
- ⬜ **2c.2..N** Modül modül: iskelet yaz → GS derle → config dosyasını üret →
  istemci tarafı ihtiyacı varsa Main'e ekle → test → CHANGELOG + commit.
  (Her modül kendi satırını alacak; plan onayından sonra buraya açılır.)

### Faz 2d — GetMainInfo birleşimi
- ⬜ **2d.0** (2a.4'te açıldı) **SPK istemci format katmanı:** 5.Main'e
  ConnectIP.bmd/ServerData.bmd/SPK.ini/Data\SPK okuma desteği eklenmesi;
  referans binary `Client (eski adı 1Client)\Engine.exe` (9,2 MB) + `Data\SPK\Config\*.bmd`.
  Bu çalışma 2d.1'den önce yapılır — istemci, SPK sunucuyla aynı veri hattını
  konuşmadan parite testi mümkün değil.
- ⬜ **2d.1** Seçilen varyanta göre kaynağı düzenle (2a.5 kararı doğrultusunda).
- ⬜ **2d.2** Üretilen veri dosyalarının canlı istemciyle uyum testi.

### Faz 2e — Hizalama ve paketleme
- ⬜ **2e.1** Gömülü IP/config hizalama (kaynakta `171.235.182.88` vs canlı
  `192.168.0.150`) — hangi config'den okunacağı netleştirilip tek noktaya bağlanır.
- ⬜ **2e.2** Derleme çıktılarının canlı paket yapısına göre kurulması
  (Main.exe + DLL'ler + Data) — ClientBuild kopyası üzerinde test.
- ⬜ **2e.3** Sunucu tarafı: bizim GS/CS/DS/JS çıktılarının test klasörüne
  kurulması (canlıya dokunmadan), DB restore: `ServerTools\DB_SQL_12.bak`.

### ÇIKIŞ KRİTERİ (Faz 2 → 3 geçişi)
Tüm 2a-2e adımları ✅ + GS/Main/CS/DS/JS derlemeleri hatasız + modül
envanterinde açık kalem kalmamış.

---

## FAZ 3 — UÇTAN UCA TEST
- ⬜ 3.1 Test sunucusunu ayağa kaldır (CS/DS/JS/GS bizim derlemeler, test DB).
- ⬜ 3.2 ClientBuild kopyasıyla bağlantı, login, karakter yaratma, kısa oyun akışı.
- ⬜ 3.3 Canlı ile davranış karşılaştırma listesi (event, drop, skill vb.).
- ⬜ 3.4 Sorunları 04-HATA-GUNLUGU.md'ye işle → düzelt → yeniden test.

## FAZ 4 — ANDROID PORT DOĞRULAMASI
- ⬜ 4.1 android\ katmanının güncel kaynakla derleme kontrolü.
- ⬜ 4.2 PC/Android protokol ve içerik parite kontrolü.

## FAZ 5 — CANLIYA GEÇİŞ (kullanıcı onayı olmadan BAŞLAMAZ)
- ⬜ 5.1 Yedek + geçiş planı + geri dönüş planı.
- ⬜ 5.2 Canlı binary değişimi ve doğrulama.

## v2 — YENİ GELİŞTİRME AŞAMASI
🔒 Parite testleri (Faz 3) kullanıcıca onaylanmadan açılmaz.
