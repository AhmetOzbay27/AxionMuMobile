# 23 — 2e.3 SUNUCU TEST KLASÖRÜ (GS/CS/DS/JS)

> Adım: **2e.3** ([02-YOL-HARITASI.md](02-YOL-HARITASI.md) Faz 2e).
> Amaç: bizim `Release_EX603|Win32` sunucu derlemelerimizin **test klasörüne**
> kurulması ve oradan **canlı yığın ayağa kalkıyor mu** sorusunun fiilen
> kanıtlanması — canlı sunucuya (`C:\Axion Mu Mobile\...`) dokunmadan.
> Tarih: 02.10.2026.

## 1. Özet

- **Test klasörü:** `BuildLog\2e3\deploy\` (25 MB) — `BuildLog\2e3\deploy_server_test.sh`
  betiğiyle **tek komutta** yeniden kurulur.
- **Kurulan bizim çıktılarımız** (hepsi `Release_EX603|Win32`, v143):
  | Program | Bizim exe | md5 |
  |---|---|---|
  | ConnectServer | 103.936 B | `4c1f91bf0bf1b9d970a3db4db3af56cb` |
  | DataServer | 1.030.656 B | `916e96259b831c709b6f421cf500b3dd` |
  | JoinServer | 943.616 B | `91ee93d7a7837a90b01d4fdb659c5598` |
  | GameServer | 10.824.704 B | `43f086d939575ac34eb373f7d527b8e9` |
- **DB restore:** `ServerTools\MuServer_S6_2020\DB\DB_SQL_12.bak` → yerel
  `SQLEXPRESS` `MuOnlineS6` (ONLINE, 55 tablo). Betik idempotent: hedef varsa
  üzerine yazmaz. (`BuildLog\2e3\restore_test_db.sh`)
- **Smoke test (fiili):** test klasöründen CS→DS→JS→GS başlatıldı; dört süreç
  ayakta kaldı, portlar dinlendi, **GS→DS/JS bağlantıları kuruldu** (log kanıtı);
  ardından `Stop_TestServer.bat` ile **tamamen kapatıldı** (kalıntı süreç yok).
  Kanıt: `BuildLog\2e3\results\{smoke1.json, GameServer_LOG_smoke1.txt,
  GameServer_LOG_CONNECT_smoke1.txt}`.

## 2. Envanter — bizim çıktılar vs canlı (işlevsel parite hedefi)

| Program | Bizim (Release_EX603, v143) | Canlı (Sub-1 / üst klasörler) | Not |
|---|---|---|---|
| ConnectServer | 103.936 B | 876.032 B (v100) | boyut farkı beklenen (H-006 dersi: parite işlevsel) |
| DataServer | 1.030.656 B | 936.960 B | — |
| JoinServer | 943.616 B | 946.688 B | canlı boyuta en yakın |
| GameServer | 10.824.704 B | 6.979.072 B (v100) | modül eklemeleri bizde (A2/A3/B3) |

Import kapanışı (PE taraması, `BuildLog\2e2\pe_imports.js`): dördü de
`MSVCP140 + VCRUNTIME140` (+ system UCRT), `ODBC32` (DS/JS), `WS2_32`;
`apicb.dll`/`freeimage.dll`/`nvapi.dll` bağımlılığı **yok**.

vcxproj OutDir eşlemesi (2e.3'ün kaynağı): CS→`MuServer\1.ConnectServer`,
DS→`MuServer\2.DataServer`, JS→`MuServer\3.JoinServer`,
GS→`MuServer\4.GameServer\Sub 1\GameServer` — hepsi `Release_EX603|Win32`.

## 3. Test klasörü içeriği

```
BuildLog\2e3\deploy\
├─ 1.ConnectServer\  ConnectServer.exe + .ini + ServerList.ini + msvcp140/vcruntime140
├─ 2.DataServer\     DataServer.exe + .ini + AllowableIpList.txt + BadSyntax.txt + ConfigAutoNap.ini + DLL
├─ 3.JoinServer\     JoinServer.exe + .ini + AllowableIpList.txt + DLL
├─ 4.GameServer\Sub 1\
│   ├─ Data\         (9,9 MB: Event, Item, Monster, Custom, SPK …)
│   └─ GameServer\   GameServer.exe + Data\(8 ini) + AutoTrain.xml + GHRSReset.ini + DLL + LOG\
├─ Start_TestServer.bat   (sıra: CS → DS → JS → GS; %~dp0 göreli)
├─ Stop_TestServer.bat    (taskkill ×4)
└─ README.txt             (md5 listesi + notlar)
```

**Bilinçli atlananlar:** `*.pdb` (18 MB GS), `*.dmp` (çökme dökümleri), `LOG`
içeriği, `msvcp100/msvcr100`, `JoinServer_Real.exe`, XShield/AntiServer
(2e.3 kapsamı yalnız GS/CS/DS/JS). Canlı ağaca hiçbir yazma yapılmadı; tüm
kopyalar repo içindeki `MuServer\` ağacından alındı.

## 4. DB restore (2e.3'ün ikinci yarısı)

- Betik: `BuildLog\2e3\restore_test_db.sh` — yalnız yerel örneğe bağlanır
  (`.\SQLEXPRESS`), hedef DB (`MuOnlineS6`) varsa atlar, yoksa `RESTORE …
  WITH MOVE` ile yerel veri dizinine geri yükler, sonra durum + tablo sayısı +
  DSN kontrolü yapar.
- Sonuç: `MuOnlineS6` **ONLINE, 55 tablo** (backup 2017 SQL 2008R2'den
  yükseltildi; 405 sayfa, 0,04 s).
- DSN'ler (makine geneli, önceden kurulu): `MuOnline` → `…\SQLEXPRESS`,
  Database=`MuOnline`; `MuOnlineJoin` → aynı örnek. DS/JS config'leri bu
  DSN adlarını kullanır (`DataServerODBC=MuOnline`, `JoinServerODBC=MuOnlineJoin`).
- **Sınır:** test klasörü kendi DSN'ini kurmaz (makine geneli DSN'lere bağlıdır);
  sıfırdan makine kurulumu için adımlar README + bu bölümde.

## 5. Smoke test kanıtı (fiili koşu)

`BuildLog\2e3\deploy\Start_TestServer.bat` ile başlatıldı (19:36:27), ~3 dk
çalıştı, `Stop_TestServer.bat` ile kapatıldı (19:39:2x). Kalıntı süreç yok.

`results\smoke1.json` (süreç yolları + portlar):

```
ConnectServer  pid 4168  BuildLog\2e3\deploy\1.ConnectServer\ConnectServer.exe
DataServer     pid 5812  BuildLog\2e3\deploy\2.DataServer\DataServer.exe
JoinServer     pid 2024  BuildLog\2e3\deploy\3.JoinServer\JoinServer.exe
GameServer     pid 1476  BuildLog\2e3\deploy\4.GameServer\Sub 1\GameServer\GameServer.exe

TCP 0.0.0.0:63000 LISTENING (CS)     UDP 0.0.0.0:63001 (CS)
TCP 0.0.0.0:63002 LISTENING (DS)     TCP 0.0.0.0:63003 LISTENING (JS)
GS → 127.0.0.1:63002 ve :63003  ESTABLISHED
```

`results\GameServer_LOG_CONNECT_smoke1.txt` (GS'nin kendi günlüğü):

```
19:36:27 [SocketManager] Server started at port [55901]
19:36:28 [JoinServer] connecting to 127.0.0.1:63003 → connected
19:36:28 [DataServer] connecting to 127.0.0.1:63002 → connected
```

Ana log (`GameServer_LOG_smoke1.txt`, 201 satır): config yüklemeleri
(CashShop/ChaosMix/Character/Custom/Command …) + periyodik döngü satırları —
**çökme/dump yok**.

## 6. Sınırlar / sonraki adım

1. **XShield (anti-hack) 2e.3 kapsamı dışı** — istemci paketinde de yok; ayrı
   kalem (5.AntiServer / 5.Antihack).
2. `ServerList.ini` içindeki adresler **192.168.99.200** (parite gereği
   korundu); istemciyle uçtan uca bağlantı için test makinesinin adresi
   yazılmalı (Faz 3.1/3.2).
3. DSN'ler makine geneli; test klasörü taşınınca DSN kurulumu gerekir.
4. `BuildLog\2e3\deploy\` **commit dışı** (25 MB, betikle yeniden üretilir);
   kanıt dosyaları `results\` altında commit'lidir.
5. Sıradaki: **3.1** (bu test klasöründen CS/DS/JS/GS + test DB ile login →
   karakter → kısa oyun akışı), ardından 3.2 istemci E2E.

## 7. Dosya listesi

- Betikler: `BuildLog\2e3\{deploy_server_test.sh, restore_test_db.sh,
  ps_smoke_capture.ps1}`
- Kanıt: `BuildLog\2e3\results\{smoke1.json, GameServer_LOG_smoke1.txt,
  GameServer_LOG_CONNECT_smoke1.txt}`
- Test klasörü (untracked, yeniden üretilebilir): `BuildLog\2e3\deploy\`
- İlgili: docs/02 (2e.3 ✅), docs/03 C-02 (DB restore notu), CHANGELOG.
