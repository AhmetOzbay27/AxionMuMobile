# 27 — H-018 · Viewport paket düzeni (kanıt, kök neden, düzeltme)

> **Tarih:** 03.10.2026 (2e.7) · **Kapsam:** GameServer ↔ Main viewport paketleri
> **Bağlı:** docs/04 (H-018 kaydı) · docs/30 (4. eksen) · BuildLog/2e7
> **Durum:** ✅ **KAPANDI** — 4/4 viewport paketi bayt-bayt hizalı.

---

## 1. ÖZET

Viewport paketleri (`PMSG_VIEWPORT_PLAYER` 0x12, `PMSG_VIEWPORT_CHANGE` 0x45,
`PMSG_VIEWPORT_MONSTER` 0x13, `PMSG_VIEWPORT_SUMMON` 0x1F) sunucudan istemciye
gider ve istemci bunları `PCREATE_CHARACTER` / `PCREATE_TRANSFORM` /
`PCREATE_MONSTER` / `PCREATE_SUMMON` yapılarıyla okur. İki taraf **bayt bayt aynı
sırada** olmak zorundadır.

H-018, 2e.6'da derlemenin kırılması giderilirken tespit edilmişti: sunucu
`Release_EX803` yapılandırmasında derleniyordu, istemci ise `EX603` dönemindeki
düzeni okuyordu. Bu turda **canlı sunucunun derlenmiş paket düzeni** çıkarılarak
kök neden kanıtlandı ve iki taraf hizalandı.

---

## 2. NEDEN TEL ÜZERİNDEN YAKALAMA YAPILMADI

İstek "canlı SPK sunucusundan paketleri yakala" idi. Yakalama **teknik olarak
imkânsızdı**:

| Kontrol | Sonuç |
|---|---|
| `netstat -ano \| grep LISTENING` | 44405 ve 55858 **yok** |
| `45.87.120.29:44405` (GameServer) | `ECONNREFUSED` |
| `45.87.120.29:55858` (AntiPort) | `ECONNREFUSED` |
| `4.MuServer/Sub-1/` içeriği | yalnız `Data/` ve `GameServer/` — `Main.exe` / `ConnectServer` / `JoinServer` **yok** |

Yani canlı yığın **kapalı**; dolayısıyla canlı GS ↔ canlı istemci trafiği
üretilemez. Bunun yerine **canlı `GameServer.exe` binary'sinin kendi PDB'si**
okundu. Bu, tel yakalamasından daha kesin bir kanıttır: pakette gerçekte hangi
baytların bulunduğunu değil, **derlendiği hâliyle ne gönderildiğini** gösterir.

### Kullanılan iki kanıt kaynağı

1. **`GameServer.pdb` (27.283.456 B, 19.09.2026)** — VS DIA SDK ile okundu.
   Araç: `BuildLog/2e7/pdbtype.cpp` → `pdbtype.exe`.
   Çıktı: `BuildLog/2e7/live_pdb_viewport_layout.txt`
2. **`GameServer.exe` disassembly** (`dumpbin /disasm`, 40 MB) + `GameServer.map`
   ile sembol adresleri eşleştirildi. Çıktı:
   `BuildLog/2e7/live_gs_GCViewportPlayerSend.asm`

PDB ile exe aynı derlemeye ait: her ikisinin de zaman damgası **6AAE6177
(19.09.2026 13:18:31)**.

---

## 3. CANLI SUNUCUNUN DERLENMİŞ PAKET DÜZENİ

`pdbtype.exe "...GameServer.pdb" "PMSG_VIEWPORT"` çıktısı:

```
PMSG_VIEWPORT_PLAYER   size=36
  +0     index[2]      +2   x          +3   y
  +4     CharSet[18]   +22  name[10]  +32  tx
  +33    ty            +34  DirAndPkLevel   +35 count
PMSG_VIEWPORT_CHANGE   size=38
  +0 index[2]  +2 x  +3 y  +4 skin[2]  +6 name[10]  +16 tx  +17 ty
  +18 DirAndPkLevel  +19 CharSet[18]  +37 count
PMSG_VIEWPORT_MONSTER  size=20
  +0 index[2] +2 type[2] +4 x +5 y +6 tx +7 ty +8 DirAndPkLevel
  +9 CurHp  +10 Level(2B)  +12 Life(4B)  +16 count
PMSG_VIEWPORT_SUMMON   size=20
  +0 index[2] +2 type[2] +4 x +5 y +6 tx +7 ty +8 DirAndPkLevel
  +9 name[10]  +19 count
PBMSG_HEAD=3   PWMSG_HEAD=4   PSWMSG_HEAD=5
```

Disassembly bunu bağımsız olarak doğruluyor (`GCViewportPlayerSend`,
`005EEBC0`): gövde başlangıcı `0x0068DW`, sayaç `0x006C`, sabit kısım
**0x6D → 0x90 = 36 bayt**, ardından `GenerateEffectList` çıktısı ekleniyor ve
`edi = 36 + ekBaytSayısı` kadar ilerletiliyor.

### Canlı derleme yapılandırmasının tespiti

| Kanıt | Çıkarım |
|---|---|
| `PMSG_VIEWPORT_PLAYER` 36 B (MuunItem yok) | `HAISLOTRING = 0` |
| `attribute` / `level` / `MaxHP` / `CurHP` yok | `GAMESERVER_UPDATE < 701` |
| Disassembly'de `call CDuel::GetDuelArenaBySpectator` **var** (005EEC65) | `GAMESERVER_UPDATE >= 402` → **603** |
| `PMSG_VIEWPORT_GUILD.owner` mevcut | `>= 401` |

→ **Canlı sunucu = `Release_EX603` + `HAISLOTRING 0`.**

---

## 4. KÖK NEDEN

Bu projenin GameServer'ı 8 ayrı `GAMESERVER_UPDATE` varyantıyla derleniyor ve
`Viewport.h` her sürüm için farklı alanlar ekliyor. **İstemci tarafı tek bir
düzen okuyor** ve o düzen `< 701` dönemine ait:

```
PCREATE_CHARACTER = KeyH,KeyL,PosX,PosY,Class,Equipment[17],ID[10],
                    TargetX,TargetY,Path, MuunItem[2], s_BuffCount, s_BuffEffectState[32]
                    0        1    2    3    4     5..21        22..31 32     33   34   35..36     37
```

`Release_EX803` ile derlenen sunucu ise 49 bayt gönderiyor ve 35. bayttan
sonrası istemcide **hiç karşılığı yok**:

```
sunucu (EX803) : ... DirAndPkLevel(34) attribute(35) MuunItem(36-37) level(38-39)
                        MaxHP(40-43) CurHP(44-47) count(48)
istemci        : ... Path(34)          MuunItem(35-36)        s_BuffCount(37)
```

Bunun canlıdaki sonucu: istemci pet tipini `ElementalAttribute`'tan okuyor,
`s_BuffCount`'u muun item'ın düşük baytından okuyor ve `level/MaxHP/CurHP/count`
alanlarını **buff durumu sanıp** kaydırılmış karakterler çiziyordu.

**H-018'in özü yapılandırma seçimi değil, iki tarafın aynı düzeni konuşmamasıydı.**

---

## 5. UYGULANAN DÜZELTME

### 5.1 İstemci (`Source/5.Main/source/WSclient.h`)

Dört `PCREATE_*` yapısına sunucunun gönderdiği blok, **kabloda aynı sırayla**
eklendi:

| Yapı | Eklenen alanlar |
|---|---|
| `PCREATE_CHARACTER` | `Attribute` → `MuunItem[2]` → `Level[2]` → `MaxHP[4]` → `CurHP[4]` |
| `PCREATE_TRANSFORM` | `Attribute` → `MuunItem[2]` → `Level[2]` → `MaxHP[4]` → `CurHP[4]` |
| `PCREATE_SUMMON` | `Attribute` → `Level[2]` → `MaxHP[4]` → `CurHP[4]` |
| `PCREATE_MONSTER` | `Attribute` → `Level[2]` → `MaxHP[4]` → `CurHP[4]` |

### 5.2 İstemci (`Source/5.Main/source/WSclient.cpp`)

`ReceiveCreatePlayerViewport`, `ReceiveCreateMonsterViewport` ve
`ReceiveCreateSummonViewport` içinde `c->Level` artık kablonun `level[2]`
alanından okunuyor (`MAKE_NUMBERW`).

### 5.3 Sunucu (`Viewport.h` + `Viewport.cpp`) — canlı `MONSTER` varyantı

Canlı `< 701` derlemesi `PMSG_VIEWPORT_MONSTER` içinde 7 baytlik
`CurHp`/`Level`/`Life` bloğu gönderiyor; bizim `< 701` kaynağımız 9 bayt
gönderiyordu. Yapı ve yazıcı canlıyla eşitlendi (EX803 dalı **dokunulmadı**).

---

## 6. DOĞRULAMA

Araç: `BuildLog/2e7/viewport_layout.js` — gerçek başlık dosyalarını okur,
`#if` bloklarını verilen `(GAMESERVER_UPDATE, HAISLOTRING)` ile çözer ve her iki
tarafın bayt haritasını alan alan karşılaştırır.

```
node BuildLog/2e7/viewport_layout.js <GAMESERVER_UPDATE> <HAISLOTRING>
```

| Yapılandırma | PLAYER | CHANGE | MONSTER | SUMMON | Sonuç | exit |
|---|---|---|---|---|---|---|
| **803 / 1** (dağıtım yapımız) | 49 | 51 | 21 | 31 | **HİZALI** | 0 |
| 803 / 1 — *düzeltme öncesi* | 51\* | 53\* | 21 | 31 | 2/4 kayma | 1 |
| 603 / 1 | 38 | 40 | 20 | 20 | 2/4 kayma (istemci EX803 biçimi) | 1 |
| **603 / 0** (canlı) | **36** | **38** | **20** | **20** | canlıyla bayt-bayt aynı | — |

\* araç sürümündeki `#if` ayrıştırma hatası sonrası düzeltildi; yukarıdaki
"düzeltme öncesi" satırı o araç hatasının ürettiği değerlerdir, esas ölçüm
aşağıdaki `layout_803_1.txt` dosyasıdır.

### Aracın kendi doğrulaması (canlı PDB'ye karşı)

`node viewport_layout.js 603 0` çıktısı, **canlı PDB'nin** raporladığı değerlerle
birebir aynıdır:

| Yapı | Araç (603/0) | Canlı PDB |
|---|---|---|
| PLAYER sizeof / count | 36 / +35 | 36 / +35 |
| CHANGE sizeof / count | 38 / +37 | 38 / +37 |
| MONSTER CurHp/Level/Life/count | +9/+10/+12/+16, sizeof 20 | +9/+10/+12/+16, sizeof 20 |
| SUMMON name/count | +9 / +19 | +9 / +19 |

Bu, hem düzeltmeyi hem de aracı doğrulayan bir çapraz kontroltür.

### Derleme kanıtı

| Bileşen | Sonuç | Kanıt |
|---|---|---|
| GameServer `Release_EX803` | ✅ `GS_EXIT=0` · 11.294.208 B · md5 `fd7e2c14f891ddbf9599967741468e2e` | `BuildLog/2e7/gameserver_build_2e7.log` |
| Main `Global Release` | ⚠️ `WSclient.cpp`/`WSclient.h` **hatasız derlendi**; başka bir ajanın aynı anda düzenlediği `SPKData.cpp` ve `SPKMenuBar.cpp` 4 hata verdi (H-018 ile ilgisiz, o dosyalara dokunulmadı) | `BuildLog/2e7/main_build_2e7.log` |

---

## 7. KABUL EDİLEN CANLI FARKI

Kullanıcı kararı (bu tur): **`EX803` korunur, eksik alanlar istemciye eklenir.**

Bunun sonucu olarak dağıtım yapımız canlıdan şu kadar farklıdır ve bu
**bilinçli**dir:

| Paket | Bizim (EX803) | Canlı (EX603) | Fark |
|---|---|---|---|
| PLAYER | 49 B | 36 B | +13 B (`attribute`, `MuunItem`, `level`, `MaxHP`, `CurHP`) |
| CHANGE | 51 B | 38 B | +13 B |
| MONSTER | 21 B | 20 B | +1 B (`attribute`) |
| SUMMON | 31 B | 20 B | +11 B |

Canlıyla **bayt-bayt** parite istenirse tek gereken şey dağıtım yapılandırmasını
`Release_EX603`'e çevirmek ve `HAISLOTRING`'ı 0 yapmak; kaynak zaten bunu
destekliyor (`< 701` blokları `Viewport.h`'te mevcut ve doğrulandı).

---

## 8. KALAN İŞ

- `MaxHP`/`CurHP`/`Attribute` alanları şu an yalnızca **hizalama** için alan
  rezervasyonu; istemci `CHARACTER` sınıfında `Life`/`MaxLife` üyesi yok.
  Başkalarının can barı için ayrı bir iş emri gerekir (docs/31'e eklenmeli).
- `< 701` istemci varyantı (`HAISLOTRING=0` iken `PCREATE_*`) yazılmadı; şu an
  istemci yalnızca dağıtım yapımızla (EX803/HAISLOTRING=1) hizalıdır.
- Canlı `PMSG_VIEWPORT_MONSTER`'ın `CurHp` değerinin **canlı formülü** statik
  analizle doğrulanamadı; `< 701` dalında `Life*100/MaxLife` yazıldı ve
  `< 701` derlemesi dağıtımda kullanılmadığı için etkisizdir.

---

## 9. YENİDEN ÜRETİM

```bash
# 1) Canlı PDB'den gerçek düzenleri oku
cd BuildLog/2e7
cmd /c "C:\Axion Mu Source\BuildLog\2e7\build_pdbtype.bat"
./pdbtype.exe "C:\Axion Mu Mobile\4.MuServer\Sub-1\GameServer\GameServer.pdb" "PMSG_VIEWPORT" 1

# 2) Kaynak düzenlerini hesapla ve karşılaştır
node viewport_layout.js 803 1        # dağıtım yapımız  -> HİZALI (exit 0)
node viewport_layout.js 603 0        # canlı yapılandırma

# 3) Canlı disassembly
bash dis_live_gs.sh                 # /tmp/live_gs.dis
```