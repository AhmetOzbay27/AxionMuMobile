# 15 — EventGvG E2E DOĞRULAMA (2c.1-B2 kapanış turu)

Tarih: 2026-10-02 (08:00–08:40 arası oturum · turbo)
Kapsam isteği: "EventGvG'yi test sunucusunda uçtan uca doğrula: Switch=1, zaman
tablosu, NPC dialog girişi, STAND→START geçişi, UserDieProc puanlama ve CalcRank
kazanan akışı." + önceki turdan devreden "2c.1-A dalgası A2 MonsterSkill kalemi"
(sonraki tura bırakıldı).

> Bu tur **kod paritesi** turu değil, **çalıştırma doğrulaması** turudur. Yine de
> sunucu açılışını engelleyen 4 gerçek hata bulundu ve düzeltildi (aşağıda kanıtlarıyla).

---

## 1. Sonuç özeti

| # | Doğrulama kalemi | Sonuç | Kanıt (LOG\2026-10-02.txt) |
|---|------------------|-------|----------------------------|
| 1 | `EventGvGSwitch=1` okunuyor | ✅ | her tick: `[GVGTEST] ... switch=1` |
| 2 | Zaman tablosu (GvGEvent.dat) | ✅ | `remain` tam olarak `HH:MM:00`'a geri saydı; tur sonrası tablo ertesi güne kuruldu (`remain=86339`) |
| 3 | NPC dialog girişi | ✅ | `NPC MATCH idx=16 class=479 map=0 x=130 y=133 type=3` + `Dialog() cagrisi=1 ... guildsiz user yolu (mesaj 872)` |
| 4 | STAND→START geçişi | ✅ | `08:31:01 state=2 remain=60 guilds=2` → `08:32:01 state=3 remain=120 guilds=2` |
| 5 | `UserDieProc` puanlama | ✅ | `UserDieProc x2 -> guildA(1).Point=0 guildB(2).Point=2 (beklenen: A=0 B=2)` |
| 6 | `CalcRank` kazanan akışı | ✅ | `CalcRank -> Winner=2 (beklenen B=2) \| A rank=2 point=0 \| B rank=1 point=2` |
| 7 | Ödül coin yapılandırması | ✅ | `odul coinleri: Coin1=1 Coin2=0 Coin3=0` (dat'tan) |

Ek olarak doğrulanan **negatif yol** (canlı mantığı): `08:22:01 state=2 (STAND) →
08:22:02 state=1 (EMPTY), remain=86399` — yani STAND anında `GetGuildCount() < 2`
ise etkinlik iptal edilir ve EMPTY'ye döner (mesaj 875). Bu, tabloyu kimse
girmediğinde beklenen davranıştır; sonraki tur (2 guild kayıtlı) START'a geçti.

---

## 2. Bu turda düzeltilen AÇILIŞ (boot) ENGELLERİ

Sunucu bu turdan önce **hiç açılmıyordu** (modal "Error" kutuları / AV).
Dört ayrı hata bulundu; dördü de canlı kanıtla düzeltildi.

### 2.1 `CUSTOM_JEWEL_INFO` içindeki `std::map` + `memset` → AV (0xC0000005)

- Belirti: açılış `gCustomJewel.Load("Custom\\CustomJewel.txt")` çağrısında AV.
- Kanıt (minidump `2026-10-2_7h54m44s.dmp`, dump okuyucu `BuildLog\4GS\dump_oku.js`):
  `kod=0xc0000005 adres=0x012F3D7E` → modül RVA `0x63D7E`. Bizim `/disasm`'da bu adres
  `??$_Copy@...@?$_Tree@V?$_Tmap_traits@HUCUSTOM_JEWEL_UPDATE_INFO@@...` yani
  **`std::map` kopya kurucusu**: `mov eax,[esi]` (kaynak `_Myhead`) → `push [eax+4]`.
  Kaynak `_Myhead == NULL` çünkü `memset(&info,0,sizeof(info))` map'in iç işaretçisini sıfırlıyor,
  sonra `SetInfo(info)` struct'ı **değerle** kopyalıyor.
- Düzeltme (`CustomJewel.cpp`, `.txt` yolu): tam-struct `memset` kaldırıldı; yalnız POD
  alt alanlar (`SuccessInfo`, `FailureInfo`) sıfırlanıyor, map varsayılan kurucuyla kalıyor.
- Not: aynı `memset` hatası `LoadXML` yolunda yok (orada struct memset edilmiyor).

### 2.2 `.txt` şeması canlıdan 1 alan fazla okuyordu (`EnableSlotRing`)

- Canlı kanıt (`live_disasm.txt`):
  - `?GetInfoByItem@CCustomJewel@@` @0047AB70: `add eax,0D4h` (eleman = **212 bayt**),
    `cmp ecx,0Eh` → **15 eleman**; dizi aralığı `0B453F0..0B4605C`.
  - `?Load@CCustomJewel@@` @0047A796: `strcpy_s(dst=struct+0x54, size=0x40, src)` →
    **ModelName ofset 84, boyut 64**.
  - 212 = 16×4 (sayısal alan) + 16 (SuccessRate[4]) + 4 (SalePrice) + 64 (ModelName)
    + 32 (SuccessInfo) + 32 (FailureInfo) → canlı struct'ta **`EnableSlotRing` yok**,
    `UpdateInfo` map'i yok, ModelName[64], MAX=15.
- Canlı veri `Data\Custom\CustomJewel.txt` satırları 12 joker taşır (= 16 sayısal alan);
  bizim `.txt` okuyucumuz 13 joker okuyordu → satırlar kayıyordu.
- Düzeltme: `.txt` yolunda `info.EnableSlotRing = -1;` (kanal kapalı = canlı davranışı;
  canlıda ring gate'i hiç yok). XML yolu etkilenmedi.

### 2.3 `.txt` okuyucusunda **bölüm 3** eksikti (dosya sonu taşması)

- Belirti: AV düzeltildikten sonra modal kutu:
  `[..\Data\Custom\CustomJewel.txt] The file were not configured correctly`
  (= `GetToken` 1 sn zaman aşımı; ayrıştırıcı dosya sonundan sonra sonsuz döngü).
- Canlı kanıt: `?Load@CCustomJewel@@` içinde `0047AA60 cmp eax,3` → canlı **bölüm 3'ü de** okur;
  satırı **4 sayı** olarak okuyup **16 baytlık** struct'ı `0B4605Ch` adresindeki
  `std::vector<CUSTOM_JEWEL_UPGRADE_INFO>`'e ekler (sembol adı disasm'da birebir görünüyor).
- Canlı veri dosyasında bölüm 3 gerçekten var: `//Index ItemIndex CreateItemIndex Type`.
- Düzeltme: `CUSTOM_JEWEL_UPDATE_INFO`'ya `int Type;` eklendi (canlı 4 alan) ve `.txt`
  okuyucusuna `else if(section == 3)` dalı yazıldı (4 sayı → `SetUpdateInfo`).

### 2.4 (B1 kalemi) SkyEvent `Monster.ini` şeması

- Belirti: `[..\Data\Event\SkyEvent\Monster.ini] The file were not configured correctly`
  (aynı token zaman aşımı) — B1 turunda yazdığımız `SPK\EventMainManager.cpp`.
- Canlı veri şeması: satır = **5 kolon** `Stage Class X Y Dir` (486 satır, 5 bölüm,
  her bölüm `end` ile biter). Bizim okuyucu 4 kolon okuyordu.
- Düzeltme: satırın 1. kolonu `Stage` olarak okunuyor (`GetNumber` zaten okunmuş token),
  kalan 4 kolon `GetAsNumber`; bölüm numarası yalnız yapısal ayırıcı.
- Doğrulama: `[EventMainManager] SkyEvent loaded (Stage:0, Win:5, Monster-groups:5)`
  (5 grup = dosyadaki 5 bölüm).

---

## 3. Test ortamı ve yöntem

- **Derleme:** `MSBuild GameServer.vcxproj -p:Configuration=Release_EX603 -p:Platform=Win32
  -p:PlatformToolset=v143` → `MuServer\4.GameServer\Sub 1\GameServer\GameServer.exe`
  (temiz build 10.817.536 B, 08:36).
- **Veritabanı:** `MuOnline` DSN (32-bit) + yeni `MuOnlineJoin` DSN `Trusted_Connection=Yes`
  ile kuruldu; `DataServer.ini`/`JoinServer.ini` kullanıcı-şifresi boşaltıldı
  (canlı referans `AxionMuMobile` DSN'i ile aynı desen). DataServer açılışta SQL'e bağlandı.
- **Bağlantı adresleri (test):** `GameServerInfo - Common.ini` içinde `DataServerAddress` /
  `JoinServerAddress` = `127.0.0.1` (canlı veri `192.168.99.200`'dır; bu dosya **test
  ortamı** değişikliğidir, parite verisi değildir).
- **Etkinlik anahtarları (test):** `GameServerInfo - Event.ini`:
  `EventGvGSwitch=1, EventGvGNpc=479, EventGvGNpcMap=0, X=130, Y=133, MinUsers=0, MaxUsers=20`.
  - Canlı exe stringleri: `EventGvGSwitch/Npc/NpcMap/NpcX/NpcY/MinUsers/MaxUsers` →
    **bizim anahtar adlarımız canlı binary ile aynı**.
  - Canlı **veri** dosyası eski/stale: `EventGvGSwitch=0`, `EventGvGNpc=380`, `Y=122`,
    ayrıca `EventGvGMinGuild/MaxGuild` (canlı binary bu iki anahtarı hiç okumuyor).
    Yani canlı sunucuda GvG kapalı; bizim anahtar seti binary'ye göre doğru.
- **Zaman tablosu (test):** `Sub 1\Data\Event\GvGEvent.dat` bölüm 1 takvimi tur boyunca
  "şimdi+N dk" olarak güncellendi; bölüm 0 test değerleri
  `AlarmTime=5 StandTime=1 EventTime=2 CloseTime=0` (tam döngü ~4 dk), bölüm 2
  `0/0/0/30/94/97/*...`, bölüm 3 `Coin1=1`.
  - Canlıda **`GvGEvent.dat` yok** (canlı `Data\Event\` altında GvG/TvT dosyası yok) →
    bu dosya bizim kendi verimizdir; değerler test kolaylığı için küçültülmüştür.
- **Test sürücüsü (geçici):** `EventGvG.cpp` içine `GvGSelfTest()` + `MainProc` 1 sn tick
  tetikleyicisi eklendi. Tetik dosyaları (tick dosyayı görüp siler):
  - `..\Data\Event\GvG_selftest.flag` → NPC eşleşmesi + `Dialog()` çağrısı + AddGuild +
    UserDieProc ×2 + CalcRank + Init.
  - `..\Data\Event\GvG_testguilds.flag` → yalnız 2 test guild'i kaydeder (zaman tablosu
    E2E'si için: STAND geçişi `GetGuildCount()>=2` ister).
  - **Sürücü tur sonunda kaynaktan KALDIRILDI** (parite kaynağı temiz). Anlık görüntü + tam
    fark: `BuildLog\2c1\EventGvG_E2E_driver_snapshot.{cpp,h}` ve
    `BuildLog\2c1\EventGvG_full_diff.patch` (yeniden doğrulama gerektiğinde bu iki dosya
    geri kopyalanır; tek kalıcı fark `UserDieProc` 874 argüman sırasıdır).

---

## 4. Tur boyunca bulunan diğer notlar (bu turda DEĞİŞTİRİLMEDİ)

1. **`UserDieProc` 874 mesaj argüman sırası** (kalıcı düzeltme): canlı metin
   `[GvGEvent] Guild %s scored %d points!` → çağrı `(lpTarget->GuildName, lpGuild->Point)`.
   Donor ters gönderiyordu. (B2 turundan devreden düzeltme, bu turda korunuyor.)
2. **`CUSTOM_JEWEL_INFO` tam parite farkı (backlog):** canlı struct 212 B/15 eleman/
   `ModelName[64]`/bölüm 3 için global `std::vector<CUSTOM_JEWEL_UPGRADE_INFO>` (16 B, 4 int).
   Bizde 25 eleman, `ModelName[32]`, `std::map UpdateInfo`, `EnableSlotRing` var.
   Davranışsal etkisi yok (dosya şeması uyumlu hale getirildi), ama tam parite için
   struct'ın canlı şekline çekilmesi gerekiyor. Ayrıca canlıda `LoadXML` **hiç yok**
   (yalnız `?Load@`), bizde ölü `LoadXML` duruyor.
3. **`[BossGuild] Ko tao duoc NPC` her saniye** loglanıyor (NpcManager/BossGuild NPC
   üretimi her tick tekrar deniyor gibi). Parite/performans açısından ayrı tur hak ediyor.
4. **`GHRSReset.ini`** sunucu çalışırken kendini güncelledi (çalışma-zamanı artefaktı,
   elle düzenlenmedi).
5. Bu turun test dosyaları (commit edilmemesi önerilir, test ortamı):
   `MuServer\2.DataServer\DataServer.ini` (DSN adı), `GameServerInfo - Common.ini`
   (127.0.0.1), `GHRSReset.ini`.

---

## 5. Sıradaki adımlar

1. `docs\14` SPK paket başlık farklarının **uygulama turu** (7 kesin kalem) — B4/B5 ile birlikte.
2. B4 `SPK_CastleEvent` (`SendKillCTCMini F3 33 → 43`) ve B5 `BEventThanMa`.
3. `CUSTOM_JEWEL_INFO` struct'ının canlı şekline çekilmesi (madde 4.2) + `LoadXML` ölü kod temizliği.
4. `BossGuild` NPC üretim spam'i incelemesi (madde 4.3).
