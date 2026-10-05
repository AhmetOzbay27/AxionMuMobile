# 40 — H-018 Runtime Doğrulama: Giriş/Çıkış Sahnesi + Paket Düzeni

> **Tarih:** 05.10.2026 · **Kapsam:** EX603 GameServer + yeni Main ile uçtan uca
> sahne testi ve H-018 viewport paketlerinin **runtime** doğrulaması
> **Bağlı:** docs/27 (H-018 kök neden), docs/34 (SPK 5.2), docs/35 (Main 603)
> **Durum:** ✅ **PASS** — giriş → karakter → dünya → çıkış akışı geçti;
> 13/13 paket kontrolü PASS; iki bağımsız kayıt birebir aynı.

---

## 1. ÖZET

H-018'in kaynak düzeyindeki kapanışı (docs/27) **statik kanıta** dayanıyordu:
canlı `GameServer.pdb` + disassembly + `viewport_layout.js`. Eksik kalan tek şey
**gerçek tel üzerinden** doğrulamaydı. Bu turda:

1. Yerel test yığını (CS 44405 / proxy 55901 / GS 55902 / DS 63002 / JS 63003)
   ayakta doğrulandı.
2. Sürücülü istemci (`Main_h018.exe`, `H018_TDRV`) gerçek sunucu akışını
   uçtan uca geçti: sunucu seçimi → GS bağlantısı → giriş → **karakter listesi**
   → **dünya (MAIN_SCENE)** → çıkış(1) → karakter sahnesi → çıkış(2) → giriş sahnesi.
3. Yakalanan **gerçek paketler** (istemci düz metni + proxy tel kaydı) H-018
   layout spec'i ile **alan alan** karşılaştırıldı: **13/13 PASS**.

Sonuç: `GAMESERVER_UPDATE=603 + HAISLOTRING=0` düzeni artık yalnız kaynaktan
değil, **telden akan baytlarla** doğrulanmış durumda.

---

## 2. YIĞIN DURUMU (test öncesi doğrulama)

| Bileşen | Port | PID | Konum |
|---|---|---|---|
| ConnectServer | 44405 | 11488 | `BuildLog/h018/deploy/1.ConnectServer/` |
| node proxy | 55901 | 4172 | `BuildLog/h018/gs_proxy.js` |
| GameServer (EX603) | 55902 | 12056 | `BuildLog/h018/deploy/4.GameServer/Sub 1/GameServer/` |
| DataServer | 63002 | 11548 | `BuildLog/h018/deploy/2.DataServer/` |
| JoinServer | 63003 | 3032 | `BuildLog/h018/deploy/3.JoinServer/` |

- GS↔DS (`63002`) ve GS↔JS (`63003`) bağlantıları **ESTABLISHED**.
- DS ve JS, SQL'de **MuOnlineS6** veritabanına bağlı (`sys.dm_exec_sessions` ile
  doğrulandı: `host_process_id=11548/3032 → db=MuOnlineS6`).
- Deploy GS ikilisinin md5'i `02f695e2df973766d1592cb9bbd3cfc2`; bu,
  `MuServer/4.GameServer/Sub 1/GameServer/GameServer.exe` (Release_EX603 çıktısı)
  ile **birebir aynı**.

### DSN bulgusu (özet)

Deploy ini'leri `MuOnlineS6ODBC` istiyor; bu DSN **HKLM 32-bit** altında yok,
**HKCU altında** var (`HKCU\SOFTWARE\ODBC\ODBC.INI\MuOnlineS6ODBC`). Servisler
yerel sistem hesabıyla koştuğu için kullanılan DSN yolu farklı olabilir; ancak
**DS/JS gerçekte MuOnlineS6'ya bağlandığı** SQL oturumlarından doğrulandı.
Yani DSN adı çelişkisi bu turda bir engel **değildi**.

---

## 3. KÖK NEDEN: BOŞ KARAKTER LİSTESİ

Önceki koşuda giriş başarılıydı (`AddAccount [e2etest] Login!`) ama karakter
listesi **boş** dönüyordu; sürücü sahne 4'te takılıp `FAIL rc=2` veriyordu.

Kök neden `Source/2.DataServer/DataServer/DataServerProtocol.cpp` içinde:

```
L998   SELECT Id FROM AccountCharacter WHERE Id='%s'   (yoksa INSERT)
L1013  SELECT * FROM AccountCharacter WHERE Id='%s'
L1027  GetAsString("GameID1", CharacterName[0], ...)
L1050  SELECT cLevel,Class,Inventory,CtlCode,... FROM Character
       WHERE AccountID='%s' AND Name='%s'
```

Yani DataServer karakter listesini **`Character` tablosundan değil,
`AccountCharacter.GameID1..5` slotlarından** kuruyor. `e2etest` hesabında:

| Tablo | Durum |
|---|---|
| `MEMB_INFO` | ✅ var |
| `Character` | ✅ `H018Test` kaydı var (lvl 1, class 16) |
| `AccountCharacter` | ⚠️ satır var ama **GameID1..5 hepsi NULL** |

Slot boş olduğu için liste boş dönüyordu. Karakter "yok" değildi, **hesaba
bağlı değildi**.

**Düzeltme** (`BuildLog/h018/fix_account_char.sql`, kalıcı):

```sql
UPDATE dbo.AccountCharacter SET GameID1='H018Test' WHERE Id='e2etest';
```

---

## 4. UÇTAN UCA SAHNE TESTİ

Sürücü: `BuildLog/h018/drv/H018Drv.cpp` → `Main_h018.exe` (yalnız `H018_TDRV`
derlemesinde). Kancalar `TranslateProtocol` (paket dökümü) ve `Scene(HDC)`
(sahne durum makinesi). Koşu dizini `BuildLog/h018/client/`.

`BuildLog/h018/client/H018DRV.log` (bu koşu, özet):

```
[t=35641]  asama0 OK: grup_sira=1 sunucu_idx=1 connect_idx=0 nonpvp=0x01 -> SendRequestServerAddress
[t=38812]  asama1 OK: GS baglantisi kuruldu (protokol=2) -> SendRequestLogIn('e2etest')
[t=42922]  asama2 OK: karakter listesi geldi secilen idx=0 id='H018Test' -> StartGame()
[t=43406]  asama3 OK: MAIN_SCENE dunya yuklendi | karakter='H018Test' idx=0 konum=(0,0)
[t=80812]  asama4 OK: dunya beklemesi bitti (canli_oyuncu=2 canli_yaratik=0) -> SendRequestLogOut(1)
[t=86812]  asama5 OK: cikis(1) sonrasi CHARACTER_SCENE (protokol=0) -> SendRequestLogOut(2)
[t=88016]  asama6 OK: tam cikis - LOG_IN_SCENE (protokol=0)
[t=88016]  SONUC=PASS rc=0
```

| Aşama | Ne oldu | Sonuç |
|---|---|---|
| 0 | Sunucu listesi → seçim → `SendRequestServerAddress` | ✅ |
| 1 | GS bağlantısı → `SendRequestLogIn('e2etest')` | ✅ |
| 2 | **Karakter listesi geldi** (`H018Test`) → `StartGame()` | ✅ |
| 3 | **MAIN_SCENE dünya yüklendi** | ✅ |
| 4 | Dünyada ~37 sn kalındı (`canli_oyuncu=2`) → `SendRequestLogOut(1)` | ✅ |
| 5 | Çıkış(1) → `CHARACTER_SCENE` → `SendRequestLogOut(2)` | ✅ |
| 6 | Çıkış(2) → `LOG_IN_SCENE` → PASS | ✅ |

İstemci `WM_CLOSE` ile temiz kapandı; artık süreç kalmadı.

**Not:** `canli_yaratik=0` — dünya sahnesinde canlı yaratık görülmedi (dwell
süresi kısa ve spawn çevresinde yaratık yoktu); viewport MONSTER kaydı yine de
tel kaydında mevcut (bkz. §5).

---

## 5. RUNTIME PAKETLERİ ↔ LAYOUT SPEC (ASIL DOĞRULAMA)

İki **bağımsız** kayıt kullanıldı:

| Kayıt | Kaynak | Ne kanıtlar |
|---|---|---|
| `BuildLog/h018/client/H018DRV_recv.bin` | İstemcinin `TranslateProtocol`'a verdiği **düz metin** (sürücü kancası, `enc=0`) | İstemcinin gerçekten gördüğü baytlar |
| `BuildLog/h018/capture/s2c.bin` | Proxy **tel kaydı** (GS→istemci), `K1/K2` ile çözüldü | Sunucunun gönderdiği baytlar |

Şifre anahtarı `GameServerInfo - Common.ini`'den türetildi:
`CustomerName='AxionMu'`, `ServerSerial='!571Axion@Mobile'` → `K1=0x3F K2=0x24`.

Araç: `BuildLog/h018/crosscheck_runtime_spec.js`

```
node BuildLog/h018/crosscheck_runtime_spec.js
```

Girdi spec'i: `node BuildLog/2e7/viewport_layout.js 603 0` →
`BuildLog/h018/capture/layout_spec_603_0.txt` (güncel kaynakla üretildi).

### Çözülen kayıtlar (tel kaydından)

```
PLAYER  +0 index=10403  +2 x=143  +3 y=122  +4 CharSet=0x2000...  +22 name="H018Test"
        +32 tx=143  +33 ty=122  +34 DirAndPkLevel=3  +35 count=0
PLAYER  +0 index=16415  +2 x=136  +3 y=126  +4 CharSet=0x58FF...  +22 name="BufferElfa"
        +32 tx=136  +33 ty=126  +34 DirAndPkLevel=51  +35 count=0
MONSTER +0 index=2816  +2 type=61440  +4 x=146  +5 y=110  +6 tx=146  +7 ty=110
        +8 DirAndPkLevel=48  +9 CurHp=100  +10 Level=512  +12 Life=75  +16 count=0
```

### Kontroller — **13/13 PASS**

```
PASS  0x12 çerçeve boyu = 4+1+36*n   [size=41 n=1 gövde=36]     (x2 paket)
PASS  0x13 çerçeve boyu = 4+1+20*n   [size=25 n=1 gövde=20]
PASS  PLAYER name@+22 = H018Test (kendi karakter)
PASS  PLAYER CharSet@+4 (18B) canlı
PASS  PLAYER count@+35 = 0 (buff yok)
PASS  MONSTER CurHp@+9 = 100
PASS  MONSTER count@+16 = 0
PASS  istemci düz metni = proxy tel kaydı (PLAYER 36B birebir)   [istemci=2 tel=2]
PASS  istemci düz metni = proxy tel kaydı (MONSTER 20B birebir)  [istemci=1 tel=1]

SONUC=PASS  (FAIL=0, kontrol=13)
```

Her iki kaynakta da **72 çerçeve, 0 sapan bayt**. Çerçeveleme, gövde boyları ve
tüm alan ofsetleri spec ile birebir; iki bağımsız kayıt da birbiriyle bayt
düzeyinde aynı.

### Spec ile karşılaştırma

| Yapı | Spec (603/0) | Runtime'da görülen | Sonuç |
|---|---|---|---|
| `PMSG_VIEWPORT_PLAYER` | 36 B; name@+22, count@+35 | 36 B; name@+22='H018Test' | ✅ |
| `PMSG_VIEWPORT_MONSTER` | 20 B; CurHp@+9, Level@+10, Life@+12, count@+16 | 20 B; CurHp@+9=100, Level@+10, Life@+12, count@+16 | ✅ |
| `PMSG_VIEWPORT_CHANGE` | 38 B | bu koşuda gönderilmedi (0 kayıt) | — |
| `PMSG_VIEWPORT_SUMMON` | 20 B | bu koşuda gönderilmedi (0 kayıt) | — |

CHANGE/SUMMON bu koşuda tetiklenmedi (dönüşüm/pet yok, görünür alanda başka
oyuncu değişimi yok); **spec tarafı** bu iki yapı için de kaynak düzeyinde
hizalı (bkz. §6). İleride bir dönüşüm veya pet sahnesi ile tetiklenip
doğrulanabilir.

---

## 6. KAYNAK DÜZEYİ (statik) DOĞRULAMA — yeniden üretildi

```
node BuildLog/2e7/viewport_layout.js 603 0
### TUM PAKETLER HIZALI   (exit 0)
```

Bu, doküman 27'deki sonucun güncel kaynakla yeniden üretimidir: dört yapının
tamamı (PLAYER/CHANGE/MONSTER/SUMMON) iki taraf arasında bayt-bayt hizalı.

---

## 7. ARAÇ DÜZELTMESİ

`BuildLog/h018/parse_gs_wire.js` içindeki `iniVal()` fonksiyonu hatalıydı:
JS string'i içinde `'\s*'` tek backslash ile yazıldığı için regex `^s*`
oluyordu ve `CustomerName`/`ServerSerial` **hiçbir zaman okunamıyordu** (araç
`HATA: CustomerName/ServerSerial okunamadi` verip exit 2 ile duruyordu).
Fonksiyon regex'siz (satır bölme tabanlı) sürümle değiştirildi; araç artık
anahtarları okuyup tel kaydını çözüyor.

---

## 8. KANIT DOSYALARI

| Dosya | İçerik |
|---|---|
| `BuildLog/h018/client/H018DRV.log` | Uçtan uca sahne akışı (PASS rc=0) |
| `BuildLog/h018/client/H018DRV_pkt.log` | İstemcinin gördüğü paket başlıkları (55 satır) |
| `BuildLog/h018/client/H018DRV_recv.bin` | İstemci düz metin paket akışı (3310 B) |
| `BuildLog/h018/capture/s2c.bin` | Proxy tel kaydı (3675 B) |
| `BuildLog/h018/capture/c2s.bin` | Proxy istemci→sunucu kaydı (647 B) |
| `BuildLog/h018/capture/marks.jsonl` | Bağlantı sınırları (CONN#4 = bu koşu) |
| `BuildLog/h018/capture/wire_summary.txt` | Tel doğrulama özeti (6/6 PASS) |
| `BuildLog/h018/capture/runtime_spec_crosscheck.txt` | **Runtime ↔ spec çapraz kontrol (13/13)** |
| `BuildLog/h018/capture/layout_spec_603_0.txt` | Güncel kaynaktan spec |
| `BuildLog/h018/crosscheck_runtime_spec.js` | Çapraz kontrol aracı |
| `BuildLog/h018/fix_account_char.sql` | Karakter slot düzeltmesi |
| `BuildLog/h018/drv/h018_hooks.patch` | İstemci kancaları (kaynak geri alındı) |
| `BuildLog/h018/drv/H018Drv.cpp` | Sürücü kaynağı |

---

## 9. YENİDEN ÜRETİM

```bash
# 1) Yığın ayakta mı? (CS/GS/DS/JS + proxy)
netstat -ano | grep LISTENING | grep -E ':(44405|55901|55902|63002|63003)\b'

# 2) Hesabı karaktere bağla (bir kez)
sqlcmd -E -S "WIN-4TMUUQ42DNH\SQLEXPRESS" -i BuildLog/h018/fix_account_char.sql

# 3) Kancaları uygula + sürücüyü derle (MSBuild, v143)
cd BuildLog/h018/drv && node hook.js        # kancalar (kaynak temizse)
# ... MSBuild Main projesi + h018drv.props ile Main_h018.exe

# 4) İstemciyi koş (proxy tel kaydını da yazar)
cd BuildLog/h018/client && Engine.exe        # ~90 sn; H018DRV.log -> PASS

# 5) Doğrula
node BuildLog/2e7/viewport_layout.js 603 0 > BuildLog/h018/capture/layout_spec_603_0.txt
node BuildLog/h018/parse_gs_wire.js --s2c=BuildLog/h018/capture/s2c.bin
node BuildLog/h018/crosscheck_runtime_spec.js   # 13/13 PASS beklenir
```

---

## 10. KALAN İŞLER

1. **CHANGE/SUMMON runtime kanıtı:** dönüşüm/pet içeren bir sahne ile
   tetiklenip aynı çapraz kontrolden geçirilmeli (spec tarafı hazır).
2. **`canli_yaratik=0`:** dünya sahnesinde yaratık görülmedi; MONSTER paketi
   telde var ve doğrulandı, ancak istemci tarafında canlı yaratık sayımı
   yapılabilmesi için spawn çevresinde bekleme/dwell ayarı gerekebilir.
3. **İstemci kancaları:** `h018_hooks.patch` olarak saklandı; üretim derlemesi
   etkilenmez (`#ifdef H018_TDRV`). Kalıcılık istenirse ayrı karar.
4. K1 (EX803 dağıtım yolu) ve B3/gcoin kararları hâlâ kullanıcıda.

---

**İlgili:** docs/27 (H-018 kök neden + düzeltme), docs/34 (SPK 5.2 uyum),
docs/35 (Main 603 derleme), docs/39 (push öncesi kanca).