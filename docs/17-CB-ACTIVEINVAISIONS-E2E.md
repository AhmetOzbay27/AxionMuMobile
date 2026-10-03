# 17 — CB_ActiveInvasions E2E DOĞRULAMA (2c.1-B3 kapanış turu)

Tarih: 2026-10-02 (11:20–11:56 oturumu)
Kapsam isteği: "CB_ActiveInvasions çağrı zincirlerini bir test kurulumunda uçtan uca
doğrula: giriş push'u, F7 sub 0x02 isteği, spawn/ölüm sayaç akışı."

> Bu tur **kod paritesi** turu değil, **çalıştırma doğrulaması** turudur. Ek olarak
> canlı binary'de çağrı noktası sayımı yapıldı ve **dört parite farkı** bulundu (§6).
> Kod değiştirilmedi (sürücü dışında); bulgular karar için rapora yazıldı.

---

## 1. Sonuç özeti

| # | Doğrulama kalemi | Sonuç | Kanıt (LOG\2026-10-02.txt) |
|---|------------------|-------|----------------------------|
| 1 | **F7 sub 0x02** → `ProtocolCore` → `send_list_to_client(aIndex)` | ✅ | satır 6080: `birebir=1`, paket `C2 00 1E F3 98 02 …` |
| 2 | **Giriş push'u** (`gObjSecondProc` `CacheSendOnlogin` dalı) | ✅ | satır 6085: push bulundu + `CacheSendOnlogin=1` |
| 3 | Push **gate**'i (2. tik'te paket yok) | ✅ | satır 6086: `yeni liste=YOK` |
| 4 | **Spawn artışı** (`SetState(START)` → `SetMonster` → `monster_add`) | ✅ | satır 6107: `{43:(8,8)}` birebir, msb eşleşmesi=8 |
| 5 | **Ölüm azalışı** (`MonsterDieProc` → `monster_del(true)`) | ✅ | satır 6109: `C1 10 F3 99 {43,7,0}` birebir |
| 6 | **SetState(EMPTY)** temizliği (`ClearMonster` → `monster_del`) | ✅ | satır 6111: 20 sınıf kaydı silindi, liste `{701,702}` birebir |
| 7 | **Sayaç semantiği** (çift artış/azalış, `count==0` erase, yeniden (1,1)) | ✅ | satır 6113/6115/6117 birebir |
| 8 | **Negatif yol** (haritada olmayan id → paket yok) | ✅ | satır 6118: `yeni paket=0` |
| 9 | Sürücü kaldırıldı → temiz derleme + temiz açılış | ✅ | 11:54 build, 11:55:20 açılış; exe'de `AITEST` **yok**; bayrak dosyası artık tüketilmiyor |

Ek olarak: **30 cb paketi** yakalandı, **çökme yok**, test sonrası yığın kapatıldı (kural §5.1).

---

## 2. Kurulum (test ortamı)

- **Yığın:** `docs/00 §5.1` sırasıyla ConnectServer → DataServer → JoinServer → GameServer
  (`C:\Axion Mu Source\MuServer\...`), test bitince tamamı kapatıldı (11:56:22, `kalan: 0`).
- **Test build:** `Release_EX603` + geçici `#define CB_ActiveInvasions_TEST 1` (stdafx.h).
- **Tetik:** `..\Data\CB_ActiveInvasions_selftest.flag` (EventMainManager 1 sn tik dosyayı
  görüp siler, `CB_ActiveInvasionsSelfTest()` çalışır).
- **Paket yakalama:** `Util.cpp` `DataSend` (tek istemci) + `DataSendAll` (herkese, `idx=-1`)
  kancası; yalnız `C2 … F3 98` (liste) ve `C1 .. F3 99` (sayaç) paketleri kaydedilir/loglanır.
  Bu kanca **gerçek gönderim yolunun başında** durur (şifreleme/soket korumasından önce).
- **Sentetik istemci slotu:** `MAX_OBJECT-1` (9999). Kurulum **sunucunun kendi yaşam
  döngüsüyle** yapıldı: `gObjAdd(INVALID_SOCKET,"127.0.0.1",9999)` → `gObjAllocData`
  (slota özel `OBJECTSTRUCT` + `Alloc/Bind`: `VpPlayer/Skill/Inventory` + `PShopTrade` CS)
  + `gObjCharZeroSet`; ardından `Connected=OBJECT_ONLINE`, `IsBot=1`, `Map=1`, `Name/Account`.
  Kapanış: `Connected=OBJECT_OFFLINE` + `gObjFreeData` (canlı `gObjDel` kullanıcı-dışı kısmı).

---

## 3. Doğrulanan çağrı zincirleri (kanlı kanıt eşlemesi)

| Bizim kaynak | Canlı kanıt | Testte çalıştırılan gerçek fonksiyon |
|---|---|---|
| `Protocol.cpp:1386-1387` (`case 0xF7`/`0x02` → `send_list_to_client(aIndex)`) | `ProtocolCore` 0x54FA37, `call` 0x54FA38 | `ProtocolCore(0xF7, {C1 04 F7 02}, 4, 9999, 0, -1)` — recv yolu argümanlarıyla (`SocketManager` C1 dalı: `head=lpMsg[2]`, `encrypt=0`, `serial=-1`) |
| `User.cpp:3481-3487` (`CacheSendOnlogin` dalı) | canlı `gObjSecondProc`'ta **bu dal YOK** (bkz. §6) | `gObjSecondProc()` (test-only daraltma: yalnız 9999 işlenir + push bloğundan sonra erken `continue`) |
| `InvasionManager.cpp:579-580` (`SetMonster` → `monster_add(class,false)`) | `SetMonster` 0x4F1EBF (`push 0`) | `gInvasionManager.SetState(&m_InvasionInfo[2], INVASION_STATE_START)` — gerçek `SetMonster`/`gObjAddMonster`/`gObjSetMonster` zinciri |
| `InvasionManager.cpp:646-647` (`MonsterDieProc` → `monster_del(class,true)`) | `MonsterDieProc` 0x4F2094 (satır içi iki-sayaç düşüşü + `update_by_monster_id`) | `gInvasionManager.MonsterDieProc(&gObj[1216], &gObj[1216])` |
| `InvasionManager.cpp:517-519` (`ClearMonster` → `monster_del(class)`) | `SetState_EMPTY` içinde satır içi blok 0x4F1C0C/0x4F1C1B | `gInvasionManager.SetState(&m_InvasionInfo[2], INVASION_STATE_EMPTY)` |

**Invasion verisi:** canlı `Data\Event\InvasionManager.dat` index 2 (Boss Vàng) — Group 0 için
`Map 0 / Value 0`; eşleşen `MonsterSetBase.txt` **Type 3** kayıtları (EVENT2 bloğu, class 43
Spider dâhil 20 sınıf). Testte determinizm için yalnız `RespawnInfo[0]` tek girdiye indirildi;
`StartTime` geçici olarak boşaltıldı; tur sonunda **tüm alanlar geri yüklendi**.

---

## 4. Paket kanıtları (birebir baytlar)

Test penceresi: **satır 6075-6121** (30 paket). Önemli olanlar:

```
#1  F7 isteği  : C2 00 1E F3 98 02 BD 02 00 00 02 00 00 00 02 00 00 00 BE 02 00 00 01 00 00 00 01 00 00 00
                 -> {701:(2,2)} {702:(1,1)}   (tek istemci idx=9999, 30 B)
#2  giriş push : aynı baytlar (tek istemci idx=9999)
#3  push toplu : aynı baytlar (DataSendAll idx=-1)   [SendThongTinSauKhiVaoGame içindeki çağrı]
#5  spawn list : C2 00 2A F3 98 03 2B 00 00 00 08 00 00 00 08 00 00 00 BD 02 … -> {43:(8,8)} {701:(2,2)} {702:(1,1)}
#6..#24        : 4..22 kayıtlı liste yayınları (20 sınıf sırayla eklenir; 54→270 bayt)
#25 ölüm       : C1 10 F3 99 2B 00 00 00 07 00 00 00 00 00 00 00   -> {43, 7, 0}
#26 EMPTY      : C2 00 1E F3 98 02 BD 02 … (20 sınıf kaydı silinmiş; {701,702})
#27/#28        : {703,1,0} (add true) ve {703,2,0} (del true)
#29 yeniden    : {701:(2,2)} {702:(1,1)} {703:(1,1)}  -> erase sonrası max SIFIRLANMIŞ
#30 boş liste  : C2 00 06 F3 98 00
```

**Sayaç yorumu:** `monster_add` hem `count` hem `max_count`'u artırır (canlı 0x41E99F);
`monster_del` ikisini birlikte azaltır; `count==0` → kayıt **erase** edilir (max korunmaz) →
yeniden eklenirse (1,1) ile başlar. #29 bunu kanıtlar.

**Spawn ölçeği:** `SetState(START)` gerçek `SetMonster`'ı 20 sınıf için çağırdı → **108 obje**
spawn edildi (`GetMonsterCount`), cb sayacı yalnız class 43 için 8 (msb eşleşmesi) ve paket
birebir. `SetState(EMPTY)` sonrası **kalan obje = 0** (108'in tamamı temizlendi).

---

## 5. Test koşusu notları (dürüstlük)

İlk iki koşu sürücü hatası yüzünden çöktü; üçüncü koşu temiz geçti. Kayıt için:

1. **Koşu 1 (11:21:27, satır 5739-5806):** `ProtocolCore` çağrısında `head` parametresi
   yanlış verildi (`lpMsg[0]=0xC1`); dispatch `case 0xC1` = `FriendAddRequest` yoluna gitti
   (4 baytlık paketle) → sonradan AV. Ayrıca sentetik slot ham `memset` ile kurulmuştu.
2. **Koşu 2 (11:32:56, satır 5898-5966):** `head` düzeltildi; ancak `OBJECTSTRUCT_HEADER`
   (**User.h:1338-1351**) slotlarının varsayılan olarak **tek paylaşılan `CommonStruct`**'ı
   gösterdiği fark edilmediği için ham `memset` paylaşılan belleği bozdu →
   `gObjClearViewport`'ta AV (dump RVA `0x1526F6`).
3. **Koşu 3 (11:42:48, satır 6075-6121):** sentetik slot `gObjAdd`/`gObjAllocData` ile
   kuruldu; temiz geçti, çökme yok.

Bu iki hata **sürücü kaynaklıdır**; üretim kodunda değişiklik gerektirmedi. Ayrıca test
izolasyonu için `gObjSecondProc`'a test-only daraltma eklendi (yalnız sentetik slot işlenir +
push bloğundan sonra erken çıkış) — bu, ilgisiz user alt sistemlerinin (CharacterAutoRecuperation,
gCustomAttack, gCustomVongQuay vb.) sentetik veriyle çalışmasını engelledi.

**Kapsam dışı kalanlar (dürüst sınır):** gerçek istemci soketi yok — `C1/C2` şifreleme/soket
katmanı, hack-check tablosu (`HackPacketCheck.txt` 247 satırı `Encrypt=0` doğrulandı, kod
yolu test edilmedi; çözümleme/serial sayacı yok), ve gerçek bir istemcinin paketi ayrıştırması
test edilmedi. F7 isteği `ProtocolCore` girişinde enjekte edildi (recv yolu argümanlarıyla),
soket okuma/şifre çözme adımı atlandı.

---

## 6. PARİTE BULGULARI (canlı binary vs bizim kaynak)

Canlı çağrı noktası sayımı (`BuildLog\envanter\live_disasm.txt` + `GameServer_canli.map`):

| Canlı adres | Kapsayan fonksiyon | Çağrı | Argüman |
|---|---|---|---|
| 0x4F1EBF | `CInvasionManager::SetMonster` | `monster_add` | `push 0` (broadcast yok) |
| 0x4F1EFC | `CInvasionManager::SetMonster` (**ikinci dal**, BossInfo map) | `monster_add` | `push 1` (broadcast var) |
| 0x538687 | `CObjectManager::ObjectSetStateProc` (respawn/boss-regen) | `monster_add` | `push 1` |
| 0x4F2094 | `CInvasionManager::MonsterDieProc` | `update_by_monster_id` | (satır içi iki-sayaç düşüşü) |
| 0x54FA38 | `ProtocolCore` (F7 sub 0x02) | `send_list_to_client(int)` | `aIndex` |

**Bizde FAZLA olan çağrılar (canlıda yok):**
- `send_list_to_client()` **void overload'u** — canlıda bu sembol **hiç yok** (map'te yok,
  disasm'da çağrı yok). Bizde 3 yerde kullanılıyor: `DSProtocol.cpp:1660`
  (`SendThongTinSauKhiVaoGame`), `InvasionManager.cpp:527` (ClearMonster sonu),
  `InvasionManager.cpp:615` (SetMonster sonu). Canlı yalnız **tekil** listeyi gönderir.
- **Giriş push'u** (`User.cpp:3484`) — canlı `gObjSecondProc` gövdesinde `CActiveInvasions`
  referansı **yok** (disasm taraması); canlıda tekil `send_list_to_client(int)`'in **tek**
  çağrı noktası `ProtocolCore` (F7 sub 0x02). Yani canlı tasarım **istemci-çeker** (pull).
- `SendThongTinSauKhiVaoGame` — canlı binary'de **hiç çağrılmıyor** (0 çağrı noktası); bu fonksiyon
  ve çağrısı MUIG/donör hattından geliyor.

**Bizde EKSİK olan çağrı (canlıda var):**
- `CObjectManager::ObjectSetStateProc` içindeki respawn yolunda `monster_add(class,true)`
  (canlı 0x538687, `push 1` ardından `gObjViewportListProtocolCreate`). Bizim
  `ObjectManager.cpp`'de bu çağrı **yok** → boss-regen ile doğan invasyon canavarı sayaca
  yansımıyor.

**Ek not:** canlıda `monster_del` out-of-line sembol değil; canlı `SetState_EMPTY` içinde
satır içi iki-sayaç düşüşü (0x4F1C0C/0x4F1C1B) ve `MonsterDieProc`'ta düşüş + broadcast
(0x4F2094) bulunuyor — bizim `monster_del` semantiği **davranış olarak** bu bloklarla uyumlu
(E2E'de birebir doğrulandı), ama fonksiyon olarak canlıda ayrı bir kopyası yok.

**Öneri (karar kullanıcının):** birebir parite için (a) void overload + 4 fazla çağrı noktası
kaldırılmalı, (b) `ObjectSetStateProc` respawn yoluna `monster_add(class,true)` eklenmeli.
Mobil istemci liste için yalnız F7 02'ye güveniyorsa (a) sorunsuz; güvenmiyorsa giriş push'u
bilinçli bir sapma olarak belgelenmeli.

---

## 7. Artefaktlar ve son durum

| Yol | İçerik |
|---|---|
| `BuildLog\2c1\CB_ActiveInvasions_E2E_driver_snapshot.cpp` | Sürücü + kancaların tam hâli (727 satır) |
| `BuildLog\2c1\CB_ActiveInvasions_E2E_hooks.patch` | 6 dosyadaki test eklemelerinin diff'i (688 satır) |
| `BuildLog\2c1\CB_ActiveInvasions_E2E_log.txt` | `[AITEST]` log satırları (46 satır) |
| `BuildLog\4GS\bizim_disasm_TEST.txt` | Test exe disasm'ı (çökme adresi çözümü için, 17.3 MB) |
| `MuServer\4.GameServer\Sub 1\GameServer\2026-10-2_11h21m27s.dmp`, `2026-10-2_11h32m56s.dmp` | Koşu 1-2 çökme dump'ları (sürücü hatası; arşiv) |

**Temizlik durumu:** test kodunun tamamı kaldırıldı (`grep AITEST` → exe'de 0; bayrak dosyası
tüketilmiyor), temiz build **10.819.072 B**, md5 `d03a0a919bd2cb25f9589e044a40f2b5` (11:54),
temiz açılış 11:55:20 OK, yığın **11:56:22'de kapatıldı**. Commit atılmadı.

**Sıradaki adımlar:** §6 kararı (parite düzeltmesi), ardından `docs/14`'ün 7 SPK paket
düzeltmesi, B4 SPK_CastleEvent, B5 BEventThanMa.
