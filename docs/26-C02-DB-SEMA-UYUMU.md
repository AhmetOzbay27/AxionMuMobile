# C-02 — DB Sema Uyumu Denetimi

**Tarih:** 03.10.2026
**Kapsam:** docs/03 kalemi C-02 — "tam şema uyumu kontrolü Faz 3.1'de yapılacak"
**Durum:** ✅ **KAPANDI** — eksik tablo sayısı 15 → **0**, 14/14 tablo fonksiyonel olarak doğrulandı.

---

## 1. Neden bu kalem sessizce düşmüştü

docs/03, C-02 satırına *"tam şema uyumu kontrolü Faz 3.1'de yapılacak"* notu koymuştu.
Faz 3.1 kapatıldı ancak kontrol **hiç yapılmadı**. Bu turda kapatıldı.

---

## 2. Yöntem

Denetim gerçek SQL'e karşı yapıldı:

| Bileşen | Dosya |
|---|---|
| Tablo çıkarıcı (v2) | `BuildLog/2e4/c02_schema_audit.js` |
| Gerekli sütun kümeleri | `BuildLog/2e4/c02_need_columns.js` |
| Canlı tablo listesi | `BuildLog/2e4/db_tables.txt` (sqlcmd ile üretilir) |
| Rapor | `BuildLog/2e4/c02_schema_report.txt` |
| Sütun listesi | `BuildLog/2e4/c02_missing_columns.txt` |
| Duman testi (SQL) | `BuildLog/2e4/c02_smoke.sql` |
| Duman testi (çıktı) | `BuildLog/2e4/c02_smoke_output.txt` |

Betik yalnızca `gQueryManager.ExecQuery("…")` kalıbındaki **çift tırnaklı SQL dize
literal'lerini** tarar; `Source/1..4` altındaki `.cpp/.h` dosyalarını gezer.

**Canlı DB:** `WIN-4TMUUQ42DNH\SQLEXPRESS` / `MuOnlineS6`

```
sqlcmd -S 'WIN-4TMUUQ42DNH\SQLEXPRESS' -d MuOnlineS6 -E -h -1 -W -Q "SELECT name FROM sys.tables"
```

---

## 3. Denetim sonucu — ÖNCE

```
taranan=743 literal=308 db=55 kullanilan=50 eksik=15 tam=35 fazla=20
```

**15 "eksik" tablo listesi:**

| # | Tablo | Durum |
|---|---|---|
| 1 | `dbo` | ❌ **Yanlış pozitif** — `INSERT INTO dbo.MEMB_INFO(...)` yazımında şema öneki tablo sanıldı |
| 2 | `cardphone` | ✅ gerçek boşluk |
| 3 | `customitembank` | ✅ gerçek boşluk |
| 4 | `customnpcquest` | ✅ gerçek boşluk (resmi `Update13` betiği var ama hiç uygulanmamış) |
| 5 | `datanapgame` | ✅ gerçek boşluk |
| 6 | `equipinventory` | ✅ gerçek boşluk |
| 7 | `eventinventory` | ✅ gerçek boşluk |
| 8 | `muuninventory` | ✅ gerçek boşluk |
| 9 | `murummycard` | ✅ gerçek boşluk |
| 10 | `murummydata` | ✅ gerçek boşluk |
| 11 | `pcpointdata` | ✅ gerçek boşluk |
| 12 | `pentagramjewel` | ✅ gerçek boşluk |
| 13 | `pshopitemvalue` | ✅ gerçek boşluk |
| 14 | `snsdata` | ✅ gerçek boşluk |
| 15 | `itemmarketdata` | ⚠️ **kısmen gerçek** — aşağıda |

> **Not:** Önceki değerlendirmede `ItemMarketData` "çalışma anında kendini oluşturuyor,
> boşluk değil" denmişti. **Bu tespit yanlıştı** — bkz. §5.

---

## 4. Uygulanan SQL yaması

Dosya: `ServerTools/MuServer_S6_2020/DB/SQL/Update 12 - C02 Missing Tables.sql`

13 tablo için `IF OBJECT_ID(...) IS NULL` korumalı `CREATE TABLE`. Tip türetme kuralları:

* hesap/karakter adı → `varchar(10)` (`Character.Name`, `MEMB_INFO.memb___id` ile aynı)
* sayaç/para/attr → `int`
* envanter → `varbinary(n)`, n = kapasite × 16 bayt:
  * `EquipInventory` → 16×16 = **256**
  * `EventInventory` → 32×16 = **512**
  * `MuunInventory` → 62×16 = **992**
* `SNSData.Data` → `varbinary(MAX)` (`GetAsBinary`)
* `CardPhone.timenap/timeduyet` → **`int`, tarih değil** — kaynakta `GetAsInteger` ile okunuyor

**`CustomNpcQuest` özel:** Resmi `Update13 - CustomNpcQuest Table.sql` zaten repoda duruyordu
ama hiç uygulanmamıştı. Yama o tanımı birebir kullanır (varsayılan değerler +
`Character(Name)` FK + `ON UPDATE/DELETE CASCADE`), böylece sema upstream ile aynı kalır.

### Uygulama

```
sqlcmd -S 'WIN-4TMUUQ42DNH\SQLEXPRESS' -d MuOnlineS6 -E -b ^
    -i "ServerTools/MuServer_S6_2020/DB/SQL/Update 12 - C02 Missing Tables.sql"
```

### Kanıt

| Ölçüm | Önce | Sonra |
|---|---|---|
| `sys.tables` sayısı | 55 | **69** |
| Denetimde *eksik* | 15 | **0** |
| Denetimde *tam* | 35 | **49** |

```
$ node BuildLog/2e4/c02_schema_audit.js .
taranan=743 literal=308 db=69 kullanilan=49 eksik=0 tam=49 fazla=20

=== B) KULLANILAN AMA DB'DE OLMAYAN — EKSIK TABLO (0) ===
```

**İdempotency kanıtı:** Betik **iki kez** üst üste çalıştırıldı, ikisinde de
`EXIT=0`, tablo sayısı 69'da kaldı, hiçbir hata yok.

---

## 5. Kaynakta bulunan GERÇEK hata — `ChoTroi.cpp`

`ItemMarketData` için önceki değerlendirme yanlıştı. `CChoTroi::CreateTable()`
tarafından **çalışma anında** oluşturuluyor — ama **eksik ve kırık**:

### Hata 1 — 3 sütun hiç oluşturulmuyor

`GDReqItemSell` (`ChoTroi.cpp:290`) şu INSERT'i yapıyor:

```sql
INSERT INTO ItemMarketData (Account, PriceType, PriceValue, Date, TypeItem, Name, Time, Pass) …
```

`TypeItem`, `Time`, `Pass` **`CreateTable()` içinde hiç yok** → `Invalid column name 'TypeItem'`.

### Hata 2 — `CREATE TABLE` koşulsuz

```cpp
gQueryManager.ExecQuery("CREATE TABLE [dbo].[ItemMarketData](…)");
gQueryManager.ExecQuery("ALTER TABLE [ItemMarketData] ADD [Account] …");
```

DataServer **ikinci kez** açıldığında `CREATE TABLE` başarısız olur ve arkasındaki
**tüm `ALTER`lar da** başarısız olur → tablo hiçbir zaman kendini tamamlayamaz.

### Düzeltme

`Source/2.DataServer/DataServer/ChoTroi.cpp` — her ifade artırılır şekilde:

```cpp
gQueryManager.ExecQuery("IF OBJECT_ID('dbo.ItemMarketData','U') IS NULL CREATE TABLE …");
gQueryManager.ExecQuery("IF COL_LENGTH('dbo.ItemMarketData','Account') IS NULL ALTER TABLE …");
…
// GDReqItemSell INSERT'inin kullandigi ama onceki surumde hic
// olusturulmayan sutunlar. Tipler kaynaktaki %d bicimleyicilere gore int.
gQueryManager.ExecQuery("IF COL_LENGTH('dbo.ItemMarketData','TypeItem') IS NULL ALTER TABLE … ADD [TypeItem] INT not null default(0)");
gQueryManager.ExecQuery("IF COL_LENGTH('dbo.ItemMarketData','Time')     IS NULL ALTER TABLE … ADD [Time]     INT not null default(0)");
gQueryManager.ExecQuery("IF COL_LENGTH('dbo.ItemMarketData','Pass')     IS NULL ALTER TABLE … ADD [Pass]     INT not null default(0)");
```

SQL yamasına da aynı şema eklendi (madde 14) → taze DB kurulumu da tutarlı.

---

## 6. Fonksiyonel duman testi (14/14)

Şema varlığı yetmez — kaynak kodun **gerçekten gönderdiği sorgular** çalışmalıdır.
`BuildLog/2e4/c02_smoke.sql`, 14 tabloya koddan birebir alınmış
INSERT / UPDATE / SELECT cümlelerini çalıştırır, sonuçları siler.

```
$ sqlcmd … -i BuildLog/2e4/c02_smoke.sql   →  EXIT=0
```

| # | Tablo | Kanıt satırı |
|---|---|---|
| 1 | ItemMarketData | `ChoTroi.cpp:290` INSERT'i birebir → 1 satır; `SELECT TOP 1 ID,…Status=0`, `SELECT Item`, UPDATE Status |
| 2 | CustomItemBank | SELECT ItemCount,AutoPick → `4, 1` |
| 3 | CustomNpcQuest | INSERT + `Count=Count+1` → Quest 987654, **Count=2**, MonsterCount 99999 (FK üzerinden) |
| 4 | EquipInventory | `DATALENGTH(Items)` = **256** |
| 5 | EventInventory | `DATALENGTH(Items)` = **512** |
| 6 | MuunInventory | `DATALENGTH(Items)` = **992** |
| 7 | MuRummyCard | SELECT Color,Number,Slot,Status,Sequence → `1,2,1,1,1` |
| 8 | MuRummyData | TotalScore = 55 |
| 9 | PcPointData | PcPoint = 12 |
| 10 | PentagramJewel | 17 sütunlu INSERT + UPDATE → Attribute 30, OptionLevelRank5 16 |
| 11 | PShopItemValue | JoB/JoS/JoC = 201/301/401 |
| 12 | SNSData | `DATALENGTH(Data)` = 16 |
| 13 | CardPhone | `Select Top 8 * … ORDER BY timenap DESC` → 12 sütun |
| 14 | DataNapGame | `Select Top 8 * … ORDER BY Checking DESC` → TienNap 11, Status 1 |

**Temizlik kanıtı:** test satırlarının hepsi silindi —
`CardPhone=0, DataNapGame=0, ItemMarketData=0, EquipInventory=0, EventInventory=0, MuunInventory=0, SNSData=0, PcPointData=0`

> `CustomNpcQuest` testi ilk çalıştırmada FK ihlali verdi (`Msg 547`).
> Bu **şema hatası değil**, test verisi kusuruydu: FK `Character(Name)`'e gidiyor ve
> `c02test` adlı bir karakter yok. Test gerçek bir karakter adı kullanacak şekilde
> düzeltildi → `EXIT=0`.

---

## 7. Denetim sırasında çıkan EK boşluk: DataServer hiç derlenmiyordu

`ChoTroi.cpp` değişikliğini doğrulamak için DataServer'ı derlemek gerekti ve
**24 derleme hatası** çıktı — hepsi **C-02 ile ilgisiz, önceden var olan** bir kırılma:

```
GuildMatching.cpp(84,7):  error C2039: 'transform': bir 'std' üyesi değil
GuildMatching.cpp(84,7):  error C3861: 'transform': tanımlayıcı bulunamadı
PartyMatching.cpp(84,7):  error C2039: 'transform': bir 'std' üyesi değil
…
```

Neden: `std::transform` kullanılıyor ama `<algorithm>` hiç **include** edilmemiş.
Aynı projede `CharacterManager.cpp` bunu doğru yapıyor (`#include "stdafx.h"` hemen
ardından `#include <algorithm>`). İki dosyaya da aynı satır eklendi.

**Taranıp doğrulandı:** DataServer'da `std::` algoritma çağrısı yapan 14 dosya tarandı,
`<algorithm>` içermeyen **yalnızca bu ikisi** vardı.

### Sonuç

```
MSBuild.exe "Source/2.DataServer/DataServer/DataServer.vcxproj" -p:Configuration=Release_EX803 -p:Platform=Win32
→ EXIT=0, 0 hata
→ Source/2.DataServer/DataServer/Release/DataServer.exe
   1.059.328 bayt · md5 b1e4e23fbcb94f85d97455d913ef64cb
```

Günlükler: `BuildLog/2e4/c02_dataserver_build_baseline.log` (hatalı) ve
`BuildLog/2e4/c02_dataserver_build.log` (temiz).

> **Bu, ayrı bir eksik görevdi:** DataServer kaynağı 2c fazında hiç derlenmemişti,
> dolayısıyla 2c'de yazılan 8 SPK modülü de derleme kanıtına sahip değildi.

---

## 8. Denetim betiğindeki düzeltme

`c02_schema_audit.js`, `INSERT INTO dbo.MEMB_INFO(...)` yazımında tablo adı olarak
`dbo` okuyordu (yanlış pozitif). Üç regex'e şema öneki desteği eklendi:

```js
// NOT: "(?:\[?dbo\]?\.)?" -> "INSERT INTO dbo.MEMB_INFO(...)" yaziminda
// tablo adi "dbo" degil "MEMB_INFO"dir. Sema oneki atilir (yanlis pozitif onlemi).
const TBL = /\b(?:from|into|update|join)\s+(?:\[?dbo\]?\s*\.\s*)?\[?([A-Za-z_][A-Za-z0-9_]*)\]?/gi;
```

Etki: `kullanilan` 50 → **49** (sahte `dbo` gitti), `MEMB_INFO` doğru sayıldı.

---

## 9. Kapanış özeti

| Kalem | Önce | Sonra |
|---|---|---|
| Canlı DB tablo sayısı | 55 | **69** |
| Kaynakta kullanılan ama DB'de olmayan tablo | 15 | **0** |
| Fonksiyonel olarak doğrulanmış yeni tablo | 0 | **14 / 14** |
| DataServer derleme hatası | 24 | **0** |
| DataServer.exe | üretilmiyordu | **1.059.328 B** |

**Kalan 20 "kullanılmayan" tablo** (`cashlog, defaultclasstype, gens_duprian,
gens_varnert, itemlog, marry, memb_stat, mk_server, mucastle_*, rankingbloodcastle,
rankingchaoscastle, rankingdevilsquare, rankingillusiontemple, rankingtvt, t_cguid,
wz_cw_info`) **sorun değildir** — bunlar sezon/olay altyapısının tabloları; kaynak kodda
doğrudan SQL gönderen modülleri henüz yazılmamış sistemler (A-02/A-03 kapsamı).

---

## 10. İlgili dosyalar

* `ServerTools/MuServer_S6_2020/DB/SQL/Update 12 - C02 Missing Tables.sql`
* `Source/2.DataServer/DataServer/ChoTroi.cpp`
* `Source/2.DataServer/DataServer/GuildMatching.cpp`
* `Source/2.DataServer/DataServer/PartyMatching.cpp`
* `BuildLog/2e4/c02_schema_audit.js`
* `BuildLog/2e4/c02_schema_report.txt`
* `BuildLog/2e4/c02_missing_columns.txt`
* `BuildLog/2e4/c02_smoke.sql` · `c02_smoke_output.txt`
* `BuildLog/2e4/c02_dataserver_build.log` · `c02_dataserver_build_baseline.log`
