# 28 — Sunucu kaynaklarında "okuma" ve "mesaj" hataları taraması

> **Tarih:** 03.10.2026 (2e.8) · **Kapsam:** ConnectServer, DataServer, JoinServer, GameServer
> **Bağlı:** docs/30 (manifest) · BuildLog/2e8
> **Durum:** ✅ Okuma **0**, Mesaj **0** — dönüş değeri yok sayılan çağrı kalmadı.

---

## 1. NEDEN TARAMA YAPILDI

H-018 kapanışından sonra kalan risk listesi gözden geçirildiğinde, sunucu
tarafında **okuma hatalarının** ve **mesaj hatalarının** dönüş değerlerinin
denetlenmediği görüldü. Bu iki sınıf, sessizce bozulan davranış üretir: okuma
hataları yarım veriyle devam eder, mesaj hataları ise arayüz/bağlantı durumunun
gerçeği yansıtmamasına yol açar.

## 2. YÖNTEM

Tahminle değil, ölçümle: `BuildLog/2e8/scan_ignored_returns.js` dört sunucu
kaynak ağacını tarar ve **dönüş değeri yakalanmamış** çağrıları listeler.

Sınıflandırma kuralı: satır bir API çağrısıyla başlıyor, sonucu `=` ile
alınmıyor, `if/while/for/return/switch` içinde değil ve `(void)` ile açıkça
yok sayılmıyor → **yok sayılmış** sayılır.

Taranan API aileleri:

| Aile | Kapsam |
|---|---|
| **Okuma** | `ReadFile`, `ReadProcessMemory`, `ReadConsole`, `fread`, `_read`, `recv`, `PeekNamedPipe`, `GetOverlappedResult` … |
| **Mesaj** | `SendMessage`, `PostMessage`, `SendMessageTimeout`, `FindWindow(Ex)`, `GetMessage`, `PeekMessage`, `TranslateMessage`, `DispatchMessage` … |
| Yardımcı (rapor amaçlı) | `WriteFile`, `fwrite`, `CreateDirectory`, `closesocket`, `CreateThread`, `socket/bind/listen/accept/connect` … |

---

## 3. İLK ENVANTER

`BuildLog/2e8/scan_ignored_returns.txt`

```
### Sunucu kaynaklarında dönüş değeri yok sayılan çağrı: 121
===== OKUMA (4) =====
PacketManager.cpp:166  ReadFile     ReadFile(file,&HeaderInfo,sizeof(HeaderInfo),&size,0);
PacketManager.cpp:187  ReadFile     ReadFile(file,table,sizeof(table),&size,0);
PacketManager.cpp:194  ReadFile     ReadFile(file,table,sizeof(table),&size,0);
PacketManager.cpp:201  ReadFile     ReadFile(file,table,sizeof(table),&size,0);
===== MESAJ (19) =====
 4 × TranslateMessage   (ConnectServer/DataServer/JoinServer/GameServer mesaj döngüsü)
 14 × SendMessage SB_SETPARTS / SB_SETTEXT (GameServer.cpp, ServerDisplayer.cpp)
  1 × SendMessage LB_SETITEMDATA (GameServer.cpp)
```

**98 "yardımcı" kalem** (`closesocket` 63, `fclose` 12, `CreateDirectory` 12,
`fwrite`/`WriteFile` 8, `fseek` 2, `CreateThread` 1) taraya dahil edildi ki
sessizce düşmesinler; bunlar okuma/mesaj sınıfında **değildir** ve bu turun
kapsamı dışındadır (§6).

---

## 4. GERÇEK KUSURLAR VE DÜZELTMELER

### 4.1 `CPacketManager::LoadKey` — dört denetimsiz `ReadFile` (okuma hatası)

`LoadKey`, paket şifreleme tablolarını (`Modius`, `Key`, `Xor`) bir dosyadan
okuyordu ve **hiçbir `ReadFile` dönüşünü denetlemiyordu**. Dosya kırpıksa
`HeaderInfo`/`table[]` **stack artığıyla** doldurulur; `LoadEncryptionKey`
yine de `1` döner ve sunucu **sessizce yanlış** şifreleme tablosuyla çalışır.

Düzeltme — `CPacketManager::ReadExact` yardımcısı eklendi
([PacketManager.h](Source/4.GameServer/GameServer/PacketManager.h#L52)):

- `ReadFile` dönüşü `0` ise hata,
- **okunan bayt sayısı istenenden azsa** da hata (kırpık dosyada `ReadFile`
  `TRUE` dönebilir),
- her başarısızlıkta `gLog.Output(LOG_GENERAL, …)` + `CloseHandle` + `return 0`.

Dört okuma da bu yolla denetlenir; başarısızlık artık sunucuyu **durmaz ama
sessizce bozulmaz da** — anahtar yüklenmez, paket şifreleme devre dışı kalır.

### 4.2 `GameServer.cpp` — combo kutusu (mesaj hatası)

```c
int pos = SendMessage(hWndComboBox, LB_ADDSTRING, 0, …);
SendMessage(hWndComboBox, LB_SETITEMDATA, pos, …);
```

`LB_ADDSTRING` başarısız olduğunda **`LB_ERR` (-1)** döner ve `LB_SETITEMDATA`
`-1` ile çağrılır; dönüşü de yok sayıldığı için hata görünmez olur.

Düzeltme:

1. `pos == LB_ERR || pos < 0` → `continue`,
2. `LB_SETITEMDATA == LB_ERR` → `LB_DELETESTRING` ile öge geri alınır,
3. `LB_DELETESTRING` de `LB_ERR` dönerse loglanır.

### 4.3 `SB_SETPARTS` — gerçek hata sinyali var

MSDN'e göre `SB_SETPARTS` başarıda **`0`** döner; sıfır dışı değer hatadır.
Bu yüzden **gerçek bir kontrol** eklendi (GameServer.cpp + ServerDisplayer.cpp).

### 4.4 `SB_SETTEXT` ve `TranslateMessage` — hata sinyali **yok**

- `SB_SETTEXT` önceki metnin **uzunluğunu** döndürür; `0` "önceki metin yok"
  demektir, hata değil.
- `TranslateMessage` yalnızca karakter mesajlarında `TRUE` döner; hata kodu
  değildir.

Bu çağrılarda **uydurma bir kontrol yazmak** yanlış olurdu. Bunun yerine
`SB_SETTEXT` için (void)SendMessage(…)`, `TranslateMessage` için
`(void)TranslateMessage(…)` kullanıldı: dönüş değeri **bilinçli** olarak yok
sayıldığı kodda görünür hâle gelir ve gerekçesi yorumda yazılıdır.

---

## 5. DOĞRULAMA

```
$ node BuildLog/2e8/scan_ignored_returns.js
### Sunucu kaynaklarında dönüş değeri yok sayılan çağrı: 98
===== OKUMA (0) =====
===== MESAJ (0) =====
===== YAZMA/yardimci (98) =====
```

Derleme kanıtı (`BuildLog/2e8/build_all_2e8.log`) — dördü de temiz:

| Bileşen | Sonuç | Çıktı |
|---|---|---|
| GameServer `Release_EX803` | **EXIT=0** | 11.294.720 B · `e327e3499fd54d57972e19a97da2820e` |
| DataServer `Release_EX803` | **EXIT=0** | 1.059.328 B · `475c985e64bdda4ad2af8428e470090a` |
| JoinServer `Release_EX803` | **EXIT=0** | `Release/JoinServer_EX803/JoinServer.exe` |
| ConnectServer `Release_EX803` | **EXIT=0** | 103.936 B · `35a4b7364886b1a64858051e6afe9502` |

`gLog` kullanımı için `PacketManager.cpp` ve `GameServer.cpp`'ye
`#include "Log.h"` eklendi (projedeki mevcut düzen: `Connection.cpp` böyle
yapıyor; `stdafx.h` bunu içermiyor).

---

## 6. KAPSAM DIŞI BIRAKILANLAR (bilinçli)

Tarayıcı "yardımcı" sınıfında 98 kalem buldu. Bunlar okuma/mesaj hatası
**değildir** ve bu turda düzeltilmedi:

| Kalem | Adet | Neden kapsam dışı |
|---|---|---|
| `closesocket` | 63 | Soket kapatma sonucunun yok sayılması Win32'de olağandır; hata akışı zaten izleniyor |
| `fclose` / `fseek` | 14 | CRT dosya işlemleri; ayrı turun kalemi |
| `CreateDirectory` | 12 | Kurulum/veri hazırlığı; sunucu çalışma zamanı hatası değil |
| `fwrite` / `WriteFile` | 8 | **Yazma** hatası — istek "okuma" ile sınırlıydı |
| `CreateThread` | 1 | Handle sızıntısı riski ayrı bir kalem; DS `CB_AutoNapGame.cpp:546` |

Bunlar `scan_ignored_returns_after.txt`'te sayı olarak kayıtlıdır; istenirse
ayrı bir turda ele alınabilir.

## 7. ETKİLENEN DOSYALAR

| Dosya | Değişiklik |
|---|---|
| `Source/4.GameServer/GameServer/PacketManager.h` | `ReadExact` beyanı |
| `Source/4.GameServer/GameServer/PacketManager.cpp` | `ReadExact` uygulaması + 4 denetimli okuma + `Log.h` |
| `Source/4.GameServer/GameServer/GameServer.cpp` | combo kutusu, `SB_SETPARTS`, `SB_SETTEXT`×6, `TranslateMessage` + `Log.h` |
| `Source/4.GameServer/GameServer/ServerDisplayer.cpp` | `SB_SETPARTS`, `SB_SETTEXT`×6 |
| `Source/1.ConnectServer/ConnectServer/ConnectServer.cpp` | `TranslateMessage` |
| `Source/2.DataServer/DataServer/DataServer.cpp` | `TranslateMessage` |
| `Source/3.JoinServer/JoinServer/JoinServer.cpp` | `TranslateMessage` |