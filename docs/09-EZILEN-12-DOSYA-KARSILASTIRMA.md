# 09 — 12 EZİLEN DOSYA KARŞILAŞTIRMA RAPORU (Faz 2b iş emri)

> Tarih: 30.09.2026. Kapsam: 05 envanteri §4'teki E-01..E-12 "SPK revizyonuyla
> ezilen" dosyaların **bizim sürüm ↔ MUIG donor ↔ canlı SPK kanıtı** üçlü
> karşılaştırması. Amaç: her dosya için entegrasyon stratejisi ve uygulama
> sırası üretmek (2b.2 girdisi).
>
> Kanıt hatları:
> 1. **Canlı map:** `BuildLog\envanter\GameServer_canli.map` (sınıf/metot
>    sembolleri + `BossGuild.obj` vb. obj eşlemeleri).
> 2. **Canlı GS exe:** `Sub-1\GameServer\GameServer.exe` (6.979.072 B) string
>    taraması — config adları, log biçimleri.
> 3. **String dizinleri:** `gs_strings_canli.txt` / `gs_strings_bizim.txt`
>    (envanter klasörü; canlı GS vs bizim Faz-1 GS derlemesi).
> 4. **Kaynak üçlüsü:** bizim `Source\4.GameServer\GameServer\`, MUIG donor
>    `C:\Axion Mu Mobile\New Source Code\Source\Source\GameServer\GameServer\`,
>    canlı config `Sub-1\Data\` (SALT OKUNUR).
> 5. **Canlı config seti:** 05 §3 tablolarındaki eşlemeler + bu raporda
>    doğrulanan dosya boyutları.

---

## 1. YÖNETİCİ ÖZETİ

- 12 dosyanın 9'u bizde de donorde de VAR ve **ikisi de canlıdan eski/farklı**;
  3'ü (BossGuild, ChangeClass, ZenDrop) **donörde hiç yok** — bunlarda tek
  kaynak bizim dosya + canlı kanıt.
- Canlı SPK revizyonu her iki kaynak setini de aşmış durumda: canlıda görünen
  özelliklerden bazıları (BossGuild ödül/skor sistemi, BuyVip hot-reload,
  Alchemist hata koruması, OfflineMode canlı yaptırımı) **hiçbir kaynakta yok**.
- Satır kümesi benzerliği (whitespace/yorum normalize) %21 (BotAlchemist) ile
  %85 (Reconnect) arasında: **dosya dosya karar şart**, toplu donor alımı yanlış olur.
- Entegrasyon sırası önerisi: **Kolay (3) → Orta (5) → Zor (4)** (§4 tablo).
- **Dalga 2 notu (2b.2-B, 30.09.2026):** BonusManager + MossMerchant donör
  alındı — canlıda görünen Bonus event zamanı için yeni **CEventName modülü**
  (EventName.xml) donör'dan alındı; dosya canlıda yok → minimal boş config
  `Data\Event\EventName.xml` oluşturuldu (davranış dosyasız haliyle birebir).
  MonsterSetBase'e array-tabanlı GetMonsterMap, Monster.cpp'ye
  gObjMonsterClearExpiredDamage eklendi; ImperialGuardian/Raklion*/Crywolf donör
  alındı (Crywolf G3 bandından erken). E-01 BonusManager bağımlılığı hazır.

## 2. ENVANTER DÜZEYİ MATRİS

| # | Dosya | Bizim (satır) | Donor (satır) | Bizim↔donor normalize benzerlik | Donörde var mı |
|---|-------|--------------:|--------------:|--------------------------------:|----------------|
| E-01 | BossGuild.cpp | 1578 | — | — | ✅ **2b.2-O: canlı `Event\BossGuild.xml` yol geçişi** (exe string kanıtlı; deploy dosyası canlıyla birebir, CR hariç) + HandleBossKill/skor/ödül yeniden yazımı **2c** (§4) |
| E-02 | BotAlchemist.cpp | 892 | 933 | %21 | ✅ **2b.2-O: donor taban alındı** + canlı `BotAlchemist data load error %s` koruması + canlı CongHuong.txt deploy; canlı exe WcoinC/WcoinP kanıtı → Coin1/Coin2 alanları (PCPoints hattı kaldırıldı) |
| E-03 | BotBuffer.cpp | 641 | 644 | %43 | ✅ → bizim korundu (canlı BotBuffer.txt YOK; donor 5-skill format uyumsuz) |
| E-04 | ChangeClass.cpp | 202 | — | — | ✅ **2b.2-O: canlı SPK\ChangeClass.xml okuyucu** (pugixml; ini hattı kaldırıldı — canlıda `ChangeClass.ini` YOK) + ClearMasterChangeClass adımı + XML mesajları (disabled/no-coin/invalid) + canlı `[ChangeClass] Config Saved & Reloaded` logu |
| E-05 | CustomBuyVip.cpp | 197 | 258 | %24 | ✅ **2b.2-E'de yol geçişi: Custom\ → SPK\ (canlı yolu)** |
| E-06 | CustomEventTime.cpp | 142 | 99 | %26 | ✅ **2b.2-O: donor alındı + CEventName hattı** (11 event-yazıcı dosya gEventName'e geçti; ServerDisplayer painter'ları donor hattında) |
| E-07 | CustomJewel.cpp | 750 | 706 | %29 | ✅ (.txt hattı; deploy-config düzeltmesiyle dosya yerinde) |
| E-08 | CustomRankUser.cpp | 227 | 206 | %52 | ✅ **bizim taban kararlı (2b.2-E)** |
| E-09 | OfflineMode.cpp | 899 | 940 | %40 | ✅ **2b.2-O somut:** 7 notice canlı sessizliğe çekildi (canlıda hiçbiri yok; kısıt return'leri korunur) + `OnlineRewardOfflineSystems` anahtarı eklendi (canlı exe kanıtlı); donor tabanın davranış farkları (RenderAttack distance/flagwalk, PickUP, Start close-mekanizması) **2c** (canlı reverse gerekli — IsFakeOnline/InSafeZone/UI eklerimiz donor'da yok) |
| E-10 | Reconnect.cpp | 152 | 151 | %85 | ✅ **2b.2'de entegre (30.09.2026)** |
| E-11 | ThuMuaDoExc.cpp | 798 | 732 | %38 | ✅ **2b.2-O: Read canlı TXT'ye çevrildi** (donör taban + canlı 3-bölüm şeması: 0=NPC/1=Allow/2=Reward, GP kolonu atlandı, hata stringi `ThuMuaDoExc error: Index %d out of range!` canlı birebir) + donor Alchemy alındı (TradeOk ona bağlandı; XuLyItemThuMua katmanı korundu-pasif) + 4 log sessizleştirildi + canlı ThuMuaDoExc.txt deploy |
| E-12 | ZenDrop.cpp | 72 | — | — | ✅ config-pasifizasyonunda (kod aynı) |

Not: Benzerlik = normalize (girinti/boşluk/yorum arındırılmış) satır kümelerinin
ortak / birleşik oranı; yapısal eşdeğerliğin alt sınırını verir, çıktı
karşılaştırmasının üst sınırını değil.

## 3. METOT YÜZEYİ FARKLARI (sınıf::metot, bizim ↔ donor)

| Dosya | Sadece bizim | Sadece donor |
|-------|--------------|--------------|
| BotAlchemist | — | gObjInventoryInsertItemPos, getNumberOfExcOptions |
| BotBuffer | — | — (5/5 metot ortak) |
| CustomBuyVip | BuyVip, BuyVipDone, Load | AddTime, LoadData, LoadTxtData, LoadXmlData, ReceiveBuyAccountVip (+pugi sarmalayıcıları) |
| CustomEventTime | CCustomEventTime(ctor), LoadData, LoadFileXML, AddDataEventsTime | — |
| CustomJewel | LoadXML, GetUpdateInfo, SetUpdateInfo | — |
| CustomRankUser | — | — (4/4 ortak) |
| OfflineMode | RenderAutoPote, regresar | GetTarget, MovePickItem, Unpack |
| Reconnect | — | — (9/9 ortak) |
| ThuMuaDoExc | GetCoinDoiItem, GetInfoDoiItem, GetInfoDoiItemList, GetMessage, XuLyItemThuMua | Alchemy |

Canlı map sembol sayıları: CBossGuild=37 (12 gerçek metot + vtable/RTTI), diğer
sınıflar çoğunlukla 1-3 sembol (LTCG /OPT:ICF inline → yüzeysel sayı kanıt değil).

## 4. DOSYA DOSYA BULGULAR VE STRATEJİ

Öncelik grupları: **K**olay (temiz donor alımı veya küçük ekleme),
**O**rta (birleştirme/hibrit), **Z**or (canlı davranışın çözümlenmesi şart).

### E-01 BossGuild.cpp — **Zor** (bizim 1578 satır, donor yok)
- Canlı map: singleton `?Instance@CBossGuild@@SAPAV1@XZ` + metotlar: Clear, Load,
  MainProc, ProcState_EMPTY, SetState, CGPacketBossGuild(BOSSGUILD_CGPACKET*,int),
  SendTimeBossGuild(int,int), CheckPlayerTarget, SpawnBoss(int,int,int,char*),
  ClearMonster, HandleBossKill(...,BONUS_POINT_MONSTER*,char*,int), MonsterDie(int,int).
- Canlı log stringleri: `[BossGuild] Boss %s (Class:%d) was killed by %s (Guild:%s)`,
  `Guild [%s] Score: %d / %d (KillBoss)`, `Spawn Boss next (Class:%d)...`,
  `Winning guild member reward: %s`.
- Bizim sınıf: ProcState_BLANK/START, Guild/Char listeleri (SetGuild/AddGuild/
  GetCongVao...), Dialog — event iskeleti benzer ama **canlıdaki üç ek özellik
  hiçbir kaynakta yok: kill→guild skor tablosu, ödüllü bitirme (BONUS_POINT_MONSTER
  parametresi), sıradaki boss spawn duyurusu**.
- **Strateji:** Bizim dosya taban tutulur; HandleBossKill/MonsterDie/SpawnBoss
  davranışı canlı log+map izinden yeniden yazılır; BOSSGUILD_CGPACKET protokol
  alanı (canlı GS + istemci paket boyutu) tersine çıkarılır. Faz 2c kalitesi
  çözümleme gerektirir — 2b'de tek başına en büyük kalem.

### E-02 BotAlchemist.cpp — **Zor** (bizim 892, donor 933)
- Canlı kanıt: `BotAlchemist data load error %s` (exe + string dizini) —
  hatalı/eksik config'te kontrollü çıkış; **bizim ve donorde bu string yok**.
- Donör ek iki metot (envanter pozisyonu, EXC opsiyon sayacı) load akışını
  değiştiriyor; satır benzerliği %21 → iki sürüm de ciddi revize.
- **Strateji:** Donor taban + canlı `data load error` korumasının eklenmesi;
  config okuma alanları canlı BotSystem config'iyle alan alan doğrulanır.

### E-03 BotBuffer.cpp — **Orta** (bizim 641, donor 644)
- 5/5 metot ortak; benzerlik %43 (çoğunlukla gövde farkları). Canlıda sınıf adı
  exe'de geçiyor ama özel log yok.
- **Strateji:** Donor alımı + bizim/donor diff'inde canlı BotSystem davranışına
  uymayan dal farklarının elenmesi; düşük risk.

### E-04 ChangeClass.cpp — **Orta-Zor** (bizim 202, donor yok)
- Canlı kanıt: `Data\SPK\ChangeClass.xml` (241 B, canlıda VAR — bizde YOK),
  stringler: `[CommandChangeClass][%s][%s] - (ClassNum: %d)`,
  `CommandChangeClassTo{DK,DL,DW,ELF,MG,RF,SU}` ve **`ClearMasterChangeClass`
  (sadece canlıda — master skill tree sıfırlama adımı)**.
- Bizim sürüm 7 sınıfa komut üretiyor; SU dahil; master temizleme yok.
- **Strateji:** Bizim taban + canlı XML okuyucu (SPK\ChangeClass.xml şeması
  canlıdan şablon alınır) + ClearMasterChangeClass adımı eklenir.

### E-05 CustomBuyVip.cpp — **Orta** (bizim 197, donor 258)
- Canlı kanıt: `Data\SPK\CustomBuyVip.txt` (135 B, bizde YOK), loglar:
  `[%s][%s] BuyVip (Level: %d)` (iki tarafta da VAR), **`[SPK] CustomBuyVip
  configuration reloaded.` (sadece canlıda → çalışma anında config yeniden
  yükleme özelliği)**.
- Donör XML tabanlı (LoadXmlData/ReceiveBuyAccountVip/AddTime), bizim txt
  tabanlı (Load/BuyVip/BuyVipDone). Canlı dosya **.txt** → canlı taraf bizim
  formatıma daha yakın ama "reloaded" özelliği ikisinde de yok.
- **Strateji:** Bizim txt tabanı korunur (canlı config formatı); AddTime/Receive
  akışı donörden; reload komutu kanala eklenir (SPK öneki canlı konsol komut
  düzeniyle). Son karar 2b.2'de canlı txt içeriği okununca netleşir.

### E-06 CustomEventTime.cpp — **Orta** (bizim 142, donor 99)
- Canlı kanıt: `Data\Event\EventTime.xml` (2714 B, bizde YOK); string dizininde
  `</EventTime>` / `<EventTime>` satırları → canlı exe string tablosunda XML
  kapanış/açılış etiketleri duruyor (parser'ın şema doğrulaması izi).
- Bizim metot seti daha geniş (AddDataEventsTime vs.); donor tek Load.
- **Strateji:** Donor alınıp bizim ek alan işleyicileri taşınır; şema canlı
  EventTime.xml ile alan alan eşlenir (D9.0-B/C kanıt olarak kullanılır).

### E-07 CustomJewel.cpp — **Orta** (bizim 750, donor 706)
- Canlı kanıt: `Data\Custom\CustomJewel.txt` (5379 B, bizde YOK); bizim ServerInfo
  413: `gCustomJewel.LoadXML(...CustomJewel.xml)` ama bizim MuServer'da ne .txt ne
  .xml var → **bizim modül şu an sessiz düşüyor**.
- Bizim ek metotlar: LoadXML, Get/SetUpdateInfo (canlı güncelleme protokolü izi).
- **Strateji:** Canlı .txt formatı referans; Load (.txt) hattı donörden,
  Get/SetUpdateInfo bizden korunur; ServerInfo çağrısı .txt'e çevrilir.

### E-08 CustomRankUser.cpp — **Kolay-Orta** (bizim 227, donor 206)
- En yüksek benzerlik ikinci sırada (%52). Canlı string kanıtı zayıf
  (CustomRankUserSwitch/Type her iki tarafta da var).
- Bizim ekstralar: NoticeToAll/NoticeToUser/RewardSwitch (duyuru + ödül).
- **Strateji:** Bizim taban korunur; canlı GS 6.9 MB string setinde ek iz
  çıkmadıkça bizim sürüm canlıya yakın kabul edilir; yalnız config anahtar adları
  canlı Custom config'iyle eşlenir.

### E-09 OfflineMode.cpp — **Zor** (bizim 899, donor 940)
- Canlı string seti FAZLA DAR: sadece `CustomAttackOfflineGPGain` +
  `OnlineRewardOfflineSystems` (serverinfo anahtarları). Bizim derlemedeki
  `[OfflineMode] Disable in ...` uyarıları canlıda YOK → canlı ya logları
  kısaltmış ya farklı yaptırım kullanıyor.
- Metot farkları: bizim RenderAutoPote/regresar (AutoPotion çizimi + dönüş),
  donor GetTarget/MovePickItem/Unpack (hedef + item toplama + paket açma).
- **Strateji:** Donor taban + canlı yaptırım modelinin (map/level kısıtları)
  `OnlineRewardOfflineSystems` anahtarı üzerinden çözümlenmesi; UI çizim
  kısmı bizden kalabilir (istemci tarafı zaten ayrı).

### E-10 Reconnect.cpp — **Kolay** (bizim 152, donor 151) — ✅ **2b.2'de entegre (30.09.2026)**
- %85 benzerlik, 9/9 metot ortak; canlı GetEngine.ini `ReconnectTime=1`
  (07 dokümanı). Fark 7+4 satır.
- **Uygulanan:** Donörün parti-slot düzeltmesi alındı (ResumeParty'de
  Index[1..4] sabit temizliği yerine `for i=1..MAX_PARTY_USER` — 10 kişilik
  partide eski kod slot 5-9'da eski üye ID bırakıyordu). AutoResetEnable'daki
  bizim `m_CommandResetAutoEnable[AccountLevel]` kontrolü **korundu** (donörde
  yok; konfig bizim canlıda =1, donör paketinde =0 → canlı davranış). Ek fark:
  donör `//gParty.GCPartyListSend2` yorum satırını temizlemiş (nötr).
- **Doğrulama:** GS temiz derlendi (Rebuild, 0 error) → 10.704.896 B
  (değişmedi — /LTCG özdeş boyut); CReconnect/ResumeParty/SetReconnectInfo
  PDB'de doğrulandı.

### E-11 ThuMuaDoExc.cpp — **Zor** (bizim 798, donor 732)
- Canlı kanıt: `Data\Custom\BotSystem\ThuMuaDoExc.txt` (554 B, bizde YOK;
  2b.0'da CongHuong.txt kopyalandı ama bu hâlâ eksik); canlı exe'de
  `ThuMuaDoExc` 2 kez (config adı + log etiketi). Canlı string dizininde
  `[BotThuMua]`/`[ThuMuaEx]` logları YOK (bizim derlemede VAR) → canlı log
  sessizliği farklı enstrümantasyon demek.
- Metot: bizim 5 ek metot (GetCoinDoiItem, GetInfoDoiItem(List), XuLyItemThuMua,
  GetMessage), donor 1 ek (Alchemy).
- **Strateji:** Donor Alchemy + bizim info/coin sorgu katmanı birleşir; canlı
  txt şeması alan alan eşlenir; log seviyesi canlı sessizliğe çekilir.

### E-12 ZenDrop.cpp — **Kolay-Orta** (bizim 72 satır, donor yok)
- Canlı kanıt: `Data\Custom\ZenDrop.xml` (7224 B canlı vs bizim 6884 B — canlıda
  ek kayıt/kural), string `ZenDropSystem` (root etiketi, iki tarafta aynı).
- Bizim sürüm küçük ve pugixml tabanlı; canlı XML'i şema uyumlu okur.
- **Strateji:** Kod değişikliği minimal; canlı ZenDrop.xml şablonu bizim
  MuServer'a kopyalanır (şema farkı diff'lenir), gerekirse alan eşlemesi yapılır.

## 5. CONFIG EKSİKLERİ (canlıda var, bizim MuServer'da yok) — ✅ **2b.2'de kapatıldı (30.09.2026)**

| Canlı dosya | Boyut | Bizde (önce) | İlgili modül | Durum |
|-------------|------:|-------|--------------|-------|
| `Data\SPK\ChangeClass.xml` | 241 B | ❌ | E-04 | ✅ kopyalandı — **NOT:** kodumuz hâlâ `Custom\ChangeClass.ini` okuyor (ServerInfo:433); XML okuma E-04 entegrasyonunda eklenecek (bkz. §4 E-04) |
| `Data\SPK\CustomBuyVip.txt` | 135 B | ❌ | E-05 | ✅ kopyalandı; kod `Custom\CustomBuyVip.txt` okuyor (ServerInfo:405) — **yol farkı**: canlı `SPK\`, bizim `Custom\`; içerik şeması aynı (Index/Exp/Drop/Days/Coin/VipName) |
| `Data\Custom\CustomJewel.txt` | 5379 B | ❌ | E-07 | ✅ kopyalandı + ServerInfo:411 `LoadXML(.xml)` → `Load(.txt)` çevrildi (E-07 stratejisi); bizim Load parser'ı `*` → -1 destekliyor (MemScript '*' kuralı) → şema uyumlu |
| `Data\Custom\ZenDrop.xml` | 7224 B (bizimki 6884) | ⚠️ eski | E-12 | ✅ canlı sürümle güncellendi; kod aynı (pugi, `ZenDropSystem` root) |
| `Data\Event\EventTime.xml` | 2714 B | ❌ | E-06 | ✅ kopyalandı — **NOT:** okuyan modül canlı SPK'nın EventMain hattı (exe'de `Event\EventTime.xml` string'i var); bizim karşılığı `Custom\CustomEventTime.xml` (farklı şema, ServerInfo:449) — E-06 entegrasyonunda birleştirilecek |
| `Data\Custom\BotSystem\ThuMuaDoExc.txt` | 554 B | ❌ | E-11 | ✅ kopyalandı + ServerInfo:833 `.xml` → `.txt` çevrildi (canlı exe `ThuMuaDoExc.txt` string'i kanıt); donör `Read` zaten MemScript/txt parser |

Ek bulgu (2b.2 config pasifizasyonu sırasında): canlı exe config yolları
`SPK\AddBuff.txt`, `SPK\ChangeClass.xml`, `SPK\CustomBuyVip.txt`,
`Event\EventTime.xml`, `Event\BossGuild.xml` (bizim: `Event\BossGuild\BossGuild.xml`,
4146 B vs canlı 2739 B — E-01'de işlenecek). Yol farkları (Custom\ vs SPK\)
E-kalem entegrasyonlarında koda yansıtılacak.

## 6. ÖNERİLEN UYGULAMA SIRASI (2b.2 girdisi)

| Sıra | Dosya | Grup | Gerekçe |
|------|-------|------|---------|
| 1 | E-10 Reconnect | Kolay | %85 benzer, 11 satırlık fark |
| 2 | E-12 ZenDrop | Kolay | Kod neredeyse değişmez; config kopyası |
| 3 | E-08 CustomRankUser | Kolay-Orta | %52 benzer, bizim sürüm zengin |
| 4 | E-03 BotBuffer | Orta | Metot yüzeyi aynı, gövde farkları |
| 5 | E-05 CustomBuyVip | Orta | txt format korunur, reload eklenir |
| 6 | E-06 CustomEventTime | Orta | XML şema eşlemesi |
| 7 | E-07 CustomJewel | Orta | .txt hattı + UpdateInfo korunması |
| 8 | E-02 BotAlchemist | Zor | %21 benzerlik + canlı koruma |
| 9 | E-04 ChangeClass | Zor | Donör yok + XML okuyucu + master temizleme |
| 10 | E-11 ThuMuaDoExc | Zor | İki sürümün birleşimi + canlı şema |
| 11 | E-09 OfflineMode | Zor | Canlı yaptırım modeli çözülmeli |
| 12 | E-01 BossGuild | Zor | Skor/ödül/spawn sistemi canlıdan yeniden yazılır |

## 7. AÇIK SORULAR

- Canlı `CustomBuyVip.txt`/`ChangeClass.xml` şemaları 2b.2'de dosya başına
  açılıp alan listesi rapora işlenecek (D9.0-B/C akışı ile aynı oturum).
- BossGuild ödül sistemi (BONUS_POINT_MONSTER) muhtemelen BonusManager ile
  ortak yapı — E-01 işlenirken BonusManager canlı sembolleri de taranacak.
- `FilterRaname.cpp` (canlı yazım hatası) bu raporun kapsamı dışında; 05 §7'de
  2b.1'e bırakıldı.

## 8. RELOAD AİLESİ — KALAN PARÇA (2b.2-R, 01.10.2026 18:49)

2b.2-N'de "GameMaster/ExperienceTable hariç" notuyla kapatılan dal + SPK
alt kümesi; canlı kanıtlar exe string taramasından:

| Modül | Canlı log (birebir) | Canlı config | Bizim config | Durum |
|-------|--------------------|--------------|--------------|-------|
| GameMaster | `[CGameMaster] GameMaster configuration reloaded` | `Util\GameMaster.xml` (XML: GameMasterList/GameMaster/Character/IP) | `Util\GameMaster.txt` (MemScript) | ✅ m_Path+Reload+`/reload gamemaster`; **format farkı kalan** (xml↔txt) |
| ExperienceTable | `[CExperienceTable] ExperienceTable configuration reloaded` | `Util\ExperienceTable.txt` | aynı | ✅ tamam (yol da birebir) |
| AddBuff (AddBuffer) | `[SPK] AddBuff configuration saved and reloaded` | `SPK\AddBuff.txt` | aynı (E-05 hizalı) | ✅ tamam |
| CustomMonsterSkill | `[SPK] CustomMonsterSkill configuration saved and reloaded` | `SPK\CustomMonsterSkill.txt` | `Custom\CustomMonsterSkill.txt` | ✅ m_Path+Reload+`/reload custommonsterskill`; **yol farkı kalan** (bkz. aşağı) |
| CustomShop | `[SPK] CustomShop configuration reloaded` (nokta yok, "saved and" yok) | `SPK\CustomShop.xml` | **modül yok** | ⛔ Reload bağlanacak sınıf bizde de donor'da da YOK — sadece `m_CustomShopMessageBox` INI anahtarı var; modül portajı ayrı iş kalemi |
| ResetChange | `[SPK] ResetChange configuration saved and reloaded` | `SPK\ResetChange.txt` | **modül yok** | ⛔ Aynı — sınıf iki tarafta da yok (canlı-özel SPK modülü); portaj ayrı iş kalemi |

Notlar:
- Canlıda `saved and reloaded` tam 4 kez geçer: CustomMonsterSkill,
  ResetChange, AddBuff + `[EffectManagerUI] Settings saved and reloaded!`
  (editör; kapsam dışı). Canlıda `gamemaster`/`experiencetable` komut-adı
  stringi YOK → canlıda bu reload'lar editör-sürüklü (dosya kaydedilince
  otomatik); bizim `/reload` dalları aynı davranışı operatöre açar (2b.2-N
  deseni).
- CustomMonsterSkill yol farkı bilinçli ertelendi: kod `Custom\...`
  okumaya devam eder (deploy'da dosya orada); `SPK\` hizalaması dosya
  taşıması gerektirdiğinden ayrı turda. GameMaster format farkı (MemScript
  txt ↔ canlı pugixml) aynı şekilde ayrı.
- 2b.2-N'den kalıtsal bug bu turda düzeltildi: `CNotice::Reload` geri
  yüklemede `m_count`'u geri koymuyordu (eski veri dönse de Notice döngüsü
  boş kalırdı). Diğer 2b.2-N modülleri konteyner-tabanlı — etkilenmedi.

## 9. E-KALEMLERİ KALAN PARÇALAR — DURUM (01.10.2026 19:xx turu)

2b.2-O temel entegrasyonundan sonra araştırma turunun §4 denetiminden
çıkan alt parçalar:

| Kalem | Alt parça | Karar | Kayıt |
|-------|-----------|-------|-------|
| E-04 | ClearMasterChangeClass INI anahtarı + koşul | ✅ uygulandı — canlı `Command.ini:270 ClearMasterChangeClass = 0`, koşulsuz çağrı koşula bağlandı (default off = canlı) | `0c0f57431` |
| E-02 | 5 Vietnamese hata stringi → canlı `BotAlchemist error:` formatı | ✅ uygulandı (oyuncu-yönelimli Vietnamese mesajlar korundu — işlev) | `d75ca6f2a` |
| E-09 | OnlineRewardOfflineSystems donor deseni | ⛔ entegre EDİLMEDİ — donor'un tam OnlineReward sistemi (`m_OnlineRewardCoin1Delay` + PcPoint/User yazıcıları) canlıda YOK; canlı kanıt sadece config anahtarı (bizde de okunuyor ✓) + alan yazıcıları için kanıt yok; tüketici de yok → ölü yazı eklemek parite değil risk | `5417f5d02` |
| E-09 | `OfflineMode::regresar` (canlıda metot yok) | ✅ KORUNDU — GameMain.cpp:223'ten çağrılan çalışan QoL (offline karakteri koordinatına geri yürütür); kaldırmak işlev kırar, canlı kanıt yalnız yokluk | `5417f5d02` |
| E-09 | RenderAttack distance 6 vs donor 8 | ✅ değişiklik YOK — canlı 6, bizim 6 zaten tutarlı (§4'teki 8 iddiası donor farkıydı) | `5417f5d02` |
| E-11 | kalan | ✅ KAPALI — ek doğrulama: `ThuMuaDoExc error` live=1 ours=1, txt şeması ✓, `[BotThuMua]`/`AllowLuck`/`Alchemy` iki tarafta da sessiz ✓; iş yok | `5417f5d02` |
| E-06 | canlı EventTime.xml mimarisi (1/2) | ✅ uygulandı — Load(EventTime.xml)+GetEventTime+store (MESSAGE_INFO map + 42×46B + slot bayt + 0x1E tablo), ServerInfo bağlantısı, deploy XML+INI (switch 1→0 canlı); kalan: canlı GCReqEventTime paket çözümü (146450+ disasm) + invasion tablo dolumu | `8af2a6f3a` |
| E-01 | BossGuild parite (1/2) | ✅ uygulandı — 7 kill-notice + 7 Start Boss + Winning×2 + Finish 1/2 canlı format; GuildWin.ini yolu → Event\CTCMini\ (4 kullanım); kalan (2/2): Score/RewardItem/SpawnBossNext + SpawnBoss/HandleBossKill/MonsterDie mimari yeniden-yazım (canlı HandleBossKill disasm okunmadı) | `6c27fbcd8` |
