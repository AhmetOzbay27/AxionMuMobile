# 29 — 14 Tablo için Kalıcı Veri Katmanı + JoinServer/DataServer Doğrulaması

**Tarih:** 03.10.2026 · **Tur:** 2e.9

---

## 1. Önce bir düzeltme: istekteki "senkron akışı" varsayımı

İstek, *JoinServer ile DataServer arasındaki senkron akışının* uçtan uca
doğrulanmasını istedi. Ölçüm bunun **olmadığını** gösterdi:

| Gerçek topoloji | Kanıt |
|---|---|
| JoinServer → **ConnectServer**'a bağlanır (127.0.0.1:63001) | `MuServer/3.JoinServer/JoinServer.ini` |
| DataServer, GameServer'dan **63002** portundan istek alır | `MuServer/2.DataServer/DataServer.ini` |
| İkisi birbirine soket **açmaz** | tüm `.cpp` içinde karşılıklı bağlantı kodu yok |

İkisi **aynı veritabanına** bakar, bu yüzden doğrulanabilir ortak yüzey
`MEMB_INFO` tablosudur. Kullanıcı kararı: **MEMB_INFO veri tutarlılığı**
doğrulanacak.

| DSN | Hedef | Kullanan |
|---|---|---|
| `MuOnline` | veritabanı **MuOnline** | DataServer (eski config) |
| `MuOnlineJoin` | veritabanı **MuOnline** | JoinServer |
| `MuOnlineS6` (yeni, bu turda) | veritabanı **MuOnlineS6** | DataStore (veri katmanı) |

> `MuOnlineJoin` bir **veritabanı değil, DSN adıdır** ve `MuOnline`'a işaret
> eder. İlk ölçümde "veritabanı yok" sanılmıştı; DSN'e bakılınca düzeldi.

---

## 2. Veri katmanı — `CDataStore`

`Source/2.DataServer/DataServer/DataStore.h` (284 satır) /
`.cpp` (775 satır).

* 14 tablo için **tipli kayıt yapıları** — alan tipleri/boyutları semadan
  birebir (`BuildLog/2e9/schema_14.txt`).
* **Hazırlanmış ifade** (`SQLPrepare` + `SQLBindParameter` + `SQLExecute`).
  Hesap/karakter adları artık SQL metnine gömülmüyor; parametre olarak gidiyor.
* Tek bağlantı, tek ifade tamponu, `SQLSetStmtAttr(SQL_ATTR_QUERY_TIMEOUT,15)`.
* `SchemaCheck()` açılışta 14 tablonun tamamını yoklar; eksikse sunucu
  bozuk semada sessizce çalışmaya devam etmez.
* **Yeni eklenen Delete yolları** (karakter silinince gerekir):
  `DeleteCardPhone`, `DeleteEquipInventory`, `DeleteEventInventory`,
  `DeleteMuunInventory`, `DeleteMuRummyData`, `DeletePcPointData`, `DeleteSNSData`.

`DataServer.cpp` açılışta `gDataStore.Open(...)` + `SchemaCheck(...)` çağırır,
 kapanışta `Close()`. Yeni **kullanıcı DSN**'i `MuOnlineS6` →
 `WIN-4TMUUQ42DNH\SQLEXPRESS / MuOnlineS6` (yönetici hakkı gerekmeden).

---

## 3. Katmanın ortaya çıkardığı GERÇEK KUSUR 1 — `DataNapGame.STT`

`CB_AutoNapGame.cpp` üç yerde **`STT` kolonunu** kullanıyordu; oysa semada
`DataNapGame` yalnızca `Account, Name, TienNap, Checking, Status` içeriyor.

Doğrulama (çalışma anı hatası, canlı DB):

```
Msg 207, Level 16, State 1 — Invalid column name 'STT'.
```

* `GetAsInteger("STT")` → semada olmayan kolon okunuyordu
* `UPDATE ... and STT='%d'` (×2) → her çalıştırmada Msg 207

**Düzeltme:** `STT` yalnız `CardPhone` tablosunda vardır; `DataNapGame`
satırı artık **(Account, Name, Checking)** üçlüsüyle tanımlanıyor. `DataStore`
da aynı kuralı uyguluyor ve nedenini yorumda taşıyor.

---

## 4. GERÇEK KUSUR 2 — `BindInt` ölü adres bağlıyordu *(uçtan uca testin bulduğu)*

İlk yazımda:

```cpp
void CDataStore::BindInt(int idx,int val) {
    SQLBindParameter(this->m_Stmt,idx,...,&val,0,...);   // &val YEREL KOPYA
}
```

ODBC sürücüsü parametre tamponunu **`SQLExecute` anında** okur; `val` o ana
kadar stack'ten silinmiştir. Sonuç: **bütün tam sayı parametreleri bozuk**
gönderiliyordu — `UPDATE` çalışıyor görünüyor, hiçbir değeri değiştirmiyordu.

İlk uçtan uca koşuda 13 kontrolün 4'ü bundan dolayı düşüyordu
(`UPDATE sonrası LOAD` eski değeri getiriyordu). Düzeltme: değer
`m_ParamInt[]` üyesine kopyalanıp adresi bağlanıyor.

---

## 5. GERÇEK KUSUR 3 — açık imleç üzerinde `SQLExecDirect` sessizce iptal

ODBC'de handle üzerinde açık bir sonuç kümesi varken `SQLExecDirect`
çağrılırsa sürücü **`SQL_SUCCESS_WITH_INFO (01001)` "SQL statement was
cancelled"** döner. `SQL_SUCCEEDED()` **doğru** sonuc verir, ama ifade hiç
çalışmamıştır. `CDataStore::Prepare()` zaten `SQLFreeStmt(SQL_CLOSE)` ile
bunu engelliyor; test koşumunun kendi ham `SQLExecDirect` çağrıları düzeltildi.

---

## 6. Uçtan uca doğrulama — 59 kontrol, 0 hata

`BuildLog/2e9/dstest/ds_e2e.cpp` → gerçek `MuOnlineS6` + `MuOnline`
üzerinde. Çıktı: `BuildLog/2e9/e2e_result.txt` · **TEST_EXIT=0**

| Bölüm | Kapsam | Sonuç |
|---|---|---|
| A0 | Sema denetimi 14/14 | ✅ |
| A1–A14 | Her tablo için INSERT → LOAD → UPDATE → LOAD doğrula → DELETE | ✅ (13 tablo) |
| A3 | `CustomNpcQuest` FK testi | ⏭ **ATLA** — `MuOnlineS6.Character` boş |
| B0–B7 | `MEMB_INFO` JoinServer↔DataServer tutarlılığı | ✅ |

### MEMB_INFO'da ölçülen tutarlılık

| # | Ölçüm | Sonuç |
|---|---|---|
| B2 | İki sunucunun sorgusu **aynı şifreyi** okuyor | ✅ |
| B3 | Hesap araması **BÜYÜK/küçük harf duyarlı** (`COLLATE Latin1_General_BIN`) | ✅ |
| B4 | Yazma diğer sunucunun sorgusuyla **görünür** | ✅ |
| B5 | Şifre yazımı diğer sunucunun sorgusunda görünür | ✅ |
| B7 | Olmayan hesapta iki sunucu **aynı davranış** (boş sonuç) | ✅ |

> **B3 bilgi:** `COLLATE Latin1_General_BIN` nedeniyle `E2E9_ACC` ile
> `e2e9_acc` **farklı hesap** sayılır. İki sunucu da aynı ifadeyi kullandığı
> için davranışları zaten tutarlı; test bunu ölçerek sabitliyor.

### Bulgu B6 — `gcoin` kolonu hiçbir DB'de yok

`CB_AutoNapGame.cpp` içindeki `select/update gcoin from MEMB_INFO`
sorguları, `gcoin` kolonu **ne `MuOnline`'da ne `MuOnlineS6`'da** bulunmadığı
için çalışma anında hata verir. Bu turda düzeltilmedi (ayrı kalem), kayıt altına
alındı.

---

## 7. Derleme

`MSBUILD_EXIT=0` — `DataServer.vcxproj`, `Release_EX803`.
`DataServer.exe` 1.062.912 B. Kanıt: `BuildLog/2e9/build_ds.log`.

---

## 8. Kalan sınırlar

* **A3 atlandı:** `MuOnlineS6.Character` tablosu boş; `CustomNpcQuest` FK
  yolu gerçek bir karakter olmadan test edilemiyor.
* **`gcoin` bulgusu** (§6) açık kalem.
* Katman **yeni**; mevcut 391 `ExecQuery` çağrısının yalnızca 14 tabloya
  dokunan kısmı hedef alındı, göç bu turda yapılmadı.
* `MuOnlineS6` DSN'i **kullanıcı** kapsamında yazıldı (yönetici hakkı
  gerekmedi). Başka makinede kurulum gerekir.

---

## 9. ÇALIŞMA ANI KANITI (son kontrolde eklendi)

Derlenmiş `DataServer.exe` **gerçekten çalıştırıldı** — `BuildLog/2e9/runtest`
dizini, dağıtılan `DataServer.ini` (`DataServerODBC = MuOnlineS6`) ile:

```
22:53:01 [DataStore] MuOnlineS6 kalici veri katmani acildi (ODBC=MuOnlineS6)
22:53:01 [DataStore] Sema denetimi: 14/14 tablo yerinde
```

Kanıt: `BuildLog/2e9/runtime_datastore.log`. Yani katman yalnızca derlenmiş
ve test harness'ında çalışmış değil; **üretim ikilisi** de aynı kod yolundan
geçip gerçek veritabanında 14/14 tabloyu doğrulamış.

Bu sırada doğrulanan ikinci bir nokta: `CLog::Output` `Active == 0` iken
erken dönüyor. `gServerDisplayer.Init()` (→ `gLog.AddLog`) `DataServer.cpp:68`
de, `gDataStore.Open()` ise `:123`'te çağrıldığı için **log kanalı store
açılmadan önce etkinleşmiş** durumdadır; katmanın logları gerçekten yazılır.
