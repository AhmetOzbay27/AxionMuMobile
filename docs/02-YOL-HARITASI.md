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
- ✅ **2c.1** 01.10.2026: 60 modül oyun-akışı önceliğine dizildi → [13](13-2C1-ONCELIK-IS-EMRI.md)
  (A=24 çekirdek · B=9 event · C=21 QoL · D=3 anticheat · E=3 kosmetik).
- 🔄 **2c.2..N** Modül modül: iskelet yaz → GS derle → config dosyasını üret →
  istemci tarafı ihtiyacı varsa Main'e ekle → test → CHANGELOG + commit.
  **Tamamlanan kalemler:** A1 Harmony, A2 MonsterSkill ([16](16-SPK-MONSTERSKILL-A2.md)),
  A3 CustomItemSetPro ([18](18-SPK-CUSTOMITEMSETPRO-A3.md)), B1 EventMainManager,
  B2 EventGvG ([15](15-EVENTGVG-E2E-DOGRULAMA.md)), B3 ActiveInvasions ([17](17-CB-ACTIVEINVAISIONS-E2E.md)).
  **03.10.2026 ölçüm güncellemesi:** parite 5 eksene ayrıldı → [30](30-PARITE-MANIFESTI.md);
  istemci ekseni iş emri [31](31-ISTEMCI-EKSENI-IS-EMI.md); opcode kaydı [32](32-PROTOKOL-KAYDI.md).

### Faz 2d — GetMainInfo birleşimi
- ✅ **2d.0** (2a.4'te açıldı) **SPK istemci format katmanı:** 5.Main'e
  ConnectIP.bmd/ServerData.bmd/SPK.ini/Data\SPK okuma desteği eklenmesi;
  referans binary `Client (eski adı 1Client)\Engine.exe` (9,2 MB) + `Data\SPK\Config\*.bmd`.
  Bu çalışma 2d.1'den önce yapılır — istemci, SPK sunucuyla aynı veri hattını
  konuşmadan parite testi mümkün değil.
  **Bitti (02.10.2026):** `Source\5.Main\source\SPKData.{h,cpp}` (XOR 0x20 çözümü;
  IP/liste/kimlik/hız/kamera/FPS + SPK.ini), `MainLoad` entegrasyonu ve istemci
  derlemesi (Main.exe 13:30, 12.027.904 B, md5 `39f2af32f35114241c7feb586cdd92ad`).
  Rapor: docs/19; ofset düzeltmesi docs/07 Rev. 02.10.2026. Engine.exe kabul testi 2d.2'de.
- 🔄 **2d.1** Seçilen varyanta göre kaynağı düzenle (2a.5 kararı doğrultusunda).
  **İlk dalga bitti (02.10.2026):** `Source\6.GetMainInfo\GetMainInfo\SPK\*` —
  D1, D2, D3 (başlık), D6, D7 tamam; canlı `GetMainInfo.exe` ile 3 senaryoda
  (baseline + sayısal/metin sentinel) ConnectIP + ServerData + rapor **bayt-birebir**.
  Derleme: `GetMain\GetMainInfo.exe` 3.723.776 B, md5 `c480e0ba…`; SPK modu varsayılan,
  eski MUIG akışı `--mode:muig`. Kalan: **D4/D5** (kanat/item/LEVEL tam jeneratör —
  şablon bağımlılığı) + RenderEffect.bmd üretimi. Rapor: docs/20.
- ✅ **2d.2** Üretilen veri dosyalarının canlı istemciyle uyum testi.
  **Bitti (02.10.2026):** canlı `Engine.exe` bizim üretimimizle 4 koşuda çalıştırıldı
  (kontrol / üretim / bozuk-negatif); üretim koşusunda pencere **0,8 s**'de açıldı ve
  **t≈12 s `45.87.120.29:44405` SynSent** görüldü. Negatif kontrol dersi: pencere+TCP
  tek başına içerik kanıtı değil → birincil kanıt bayt karşılaştırması (docs/20).
  Rapor: docs/21; kanıt `BuildLog\2d2\{results,shots}\`.

### Faz 2e — Hizalama ve paketleme
- ✅ **2e.1** Gömülü IP/config hizalama (kaynakta `171.235.182.88` vs canlı
  `192.168.0.150`) — hangi config'den okunacağı netleştirilip tek noktaya bağlanır.
  **Bitti (02.10.2026):** canlı değer ölçüldü: `ConnectIP.bmd` ilk 12 bayt =
  **`45.87.120.29`**, port 44405 (AntiPort 55858); kaynakta üç yerde gömülü değer
  hizalandı, çalışma anında ConnectIP ezer. (B-04/H-004 kapanışı; docs/22 §2.)
- ✅ **2e.2** Derleme çıktılarının canlı paket yapısına göre kurulması
  (Main.exe + DLL'ler + Data) — ClientBuild kopyası üzerinde test.
  **Bitti (02.10.2026, asgari varlık setiyle):** `BuildLog\2e2\deploy_spk_package.sh`
  canlı paket düzenini kurar (Main.exe + wzAudio/ogg/vorbisfile + SPK.ini +
  `Data\SPK\*` + Config + MUIG→SPK yol eşlemesi); istemci pakette **kendi penceresini
  açtı** (`Axion Mu`, 0,8 s) ve kendi günlüklerini yazdı. ClientBuild klasörü artık
  diskte yok (silinmiş); test `BuildLog\2e2\deploy\` üzerinde yapıldı. Kalan: tam
  `Data` kopyası (disk %100) + SPK-first varlık katmanı (2e.4). Rapor: docs/22.
- ✅ **2e.4** *(2e.2 sırasında keşfedildi)* **SPK-first varlık çözümleme katmanı +
  harita/nesne yol paritesi.**
  **Bitti (02.10.2026):** `Source\5.Main\source\SPKAsset.cpp` çözümleyicisi
  (`Data\Local\*` → `Data\SPK\Config\*`) 20+ çağrı noktasına bağlandı; canlı Engine
  string kanıtıyla harita/nesne yolları canlı düzene çekildi (`Map\World%d`,
  `Data\Map\Object*`); H-011 (CSprite boş doku AV), H-014 (VIPCharRank fatal) ve
  H-015 (harita yolu) kapandı. Kanıt: run14 + (kaynaktaki 26 bozuk EUC-KR
  satırının bayt onarımı sonrası) run15–17 — istemci saf SPK-first pakette
  **diyalogsuz** çalıştı ve canlı sunucuya bağlanma aşamasına ulaştı
  (`45.87.120.29:44405` SynSent; nihai derleme md5 `6dc52f5f…`). Kalan: içerik/şema eşlemesi
  (itemtooltip*/Minimap içeriği → docs/03 B-08). Rapor: docs/24.
- ✅ **2e.5** **B-08 kapanışı (SPK metin/tooltip şeması)** + **Faz 3 E2E bağlantı kanıtı.**
  **Bitti (03.10.2026):** canlı paketin `Data\SPK\Config\ToolTipText.txt` tablosu
  (19 kayıt, düz metin) yeni `load_item_tooltip_text_spk()` yükleyicisiyle istemci
  tooltip tablosuna aktarıldı (markup temizliği; `type=-1`); çağrı noktası
  `ZzzOpenData` (itemtooltiptext yoksa çözümleyici üzerinden). Derleme `c49383bf…`
  (12.031.488 B). E2E: test yığını (CS/DS/JS/GS + `MuOnlineS6` + `MuOnlineS6ODBC`)
  ayakta; istemci **ConnectServer'a TCP ESTABLISHED** (t≈4,4 s) ve **sunucu seçim
  ekranı** görüntülendi; sunucu yığını sonra kapatıldı. Yeni bulgu: istemci
  `127.0.0.1` hedefini kasten reddediyor (WSctlc.cpp:230) → test hedefi makinenin
  gerçek IPv4'ü olmalı. Kalan: sunucu seç → GS/login adımı etkileşimli oturum
  gerektiriyor (bağlantısı kesilmiş oturumda fare girdisi işlenmiyor). Rapor: docs/25.
- ✅ **2e.3** Sunucu tarafı: bizim GS/CS/DS/JS çıktılarının test klasörüne
  kurulması (canlıya dokunmadan), DB restore: `ServerTools\DB_SQL_12.bak`.

  **Bitti (02.10.2026):** `BuildLog\2e3\deploy_server_test.sh` test klasörünü kurar
  (25 MB: bizim `Release_EX603` exe'leri + DLL kapanışı + Data + start/stop betikleri);
  `restore_test_db.sh` `DB_SQL_12.bak`'ı yerel `MuOnlineS6`'ya restore eder (idempotent).
  Smoke test: CS→DS→JS→GS ayakta; GS→DS/JS bağlantıları kuruldu; sonra kapatıldı. Rapor: docs/23.

- ✅ **2e.6** (03.10.2026) C-02 DB şema denetimi kapandı; sunucu hattı derlenebilir hale getirildi
  (14 eksik tablo oluşturuldu, 5 bileşen 0 hata). H-018 açık risk olarak kayda geçti. Rapor: [26](26-C02-DB-SEMA-UYUMU.md).
- ✅ **2e.7** (03.10.2026) H-018 kapandı: canlı GameServer.pdb (DIA) ile viewport düzenleri ölçüldü
  (36/38/20/20 B); istemci yapıları sunucuyla hizalandı; `viewport_layout.js` 4/4 HİZALI. Kabul edilen
  fark: EX803 dağıtımı canlıdan 13/1/11 bayt farklı — canlı bayt paritesi için EX603+HAISLOTRING=0. Rapor: [27](27-H018-VIEWPORT-PAKET-DUZENI.md).
- ✅ **2e.8** (03.10.2026) Sunucu kaynaklarında okuma/mesaj hatası taraması: 4 gerçek kusur düzeltildi;
  kapsam sınırı ölçülerek yazıldı (102 ek çağrı açık). Rapor: [28](28-OKUMA-MESAJ-HATASI-TARAMASI.md).
- ✅ **2e.9** (03.10.2026) 14 tablo için kalıcı veri katmanı (CDataStore) + MEMB_INFO tutarlılığı:
  59/59 test geçti; 2 gerçek kusur düzeltildi. Rapor: [29](29-14-TABLO-KALICI-VERI-KATMANI.md).

### ÇIKIŞ KRİTERİ (Faz 2 → 3 geçişi)
Tüm 2a-2e adımları ✅ + GS/Main/CS/DS/JS derlemeleri hatasız + modül
envanterinde açık kalem kalmamış.

> **Denetim notu (04.10.2026):** Çıkış kriteri HENÜZ sağlanmadı — Main derlemesi kırık
> (3C.0 işi) ve modül envanteri açık (bkz. [30](30-PARITE-MANIFESTI.md)). Faz 3.1/3.2
> kullanıcı talebiyle kısmen koşuldu; tam geçiş önce bu kriterin kapanmasına bağlı.
> Ayrıntı: [33](33-DENETIM-PROJE-CAPINDA.md).

---

## FAZ 3 — UÇTAN UCA TEST
- ✅ 3.1 Test sunucusunu ayağa kaldır (CS/DS/JS/GS bizim derlemeler, test DB).
  **Bitti (03.10.2026):** yığın kuruldu, yamalar uygulandı (GS ServerVersion 1.03.34 /
  ServerSerial `!571Axion@Mobile`, DS/JS `MuOnlineS6ODBC` DSN, CS `ServerList.ini` →
  gerçek IPv4:55901), `e2etest` hesabı eklendi; CS→DS/JS ve GS→DS/JS bağlantıları
  ESTABLISHED. Test sonrası yığın kapatıldı (docs/25 §5).
- 🟡 3.2 ClientBuild kopyasıyla bağlantı, login, karakter yaratma, kısa oyun akışı.
  **Kısmen bitti (03.10.2026):** bağlantı + sunucu listesi ✅ (istemci ConnectServer'a
  TCP ESTABLISHED, sunucu seçim ekranı render edildi, diyalog/çökme yok — docs/25 §4);
  **kalan:** sunucu seç → GameServer/login → karakter akışı, etkileşimli masaüstü
  oturumunda elle koşulmalı (bağlantısı kesilmiş oturumda UI girdisi işlenmiyor —
  H-017).
- ⬜ 3.3 Canlı ile davranış karşılaştırma listesi (event, drop, skill vb.).
- 🟡 3.4 Sorunları 04-HATA-GUNLUGU.md'ye işle → düzelt → yeniden test.
  **Bu turda:** H-016 (127.0.0.1 reddi → test hedefi kuralı) ve H-017 (disconnected
  oturum → UI otomasyonu sınırı) kaydedildi; H-005/H-006/H-007 kapatıldı.

## FAZ 4 — ANDROID PORT DOĞRULAMASI
- ⬜ 4.1 android\ katmanının güncel kaynakla derleme kontrolü.
- ⬜ 4.2 PC/Android protokol ve içerik parite kontrolü.

## FAZ 5 — CANLIYA GEÇİŞ (kullanıcı onayı olmadan BAŞLAMAZ)
- ⬜ 5.1 Yedek + geçiş planı + geri dönüş planı.
- ⬜ 5.2 Canlı binary değişimi ve doğrulama.

## v2 — YENİ GELİŞTİRME AŞAMASI
🔒 Parite testleri (Faz 3) kullanıcıca onaylanmadan açılmaz.
