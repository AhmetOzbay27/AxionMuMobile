# 11 — 2b.2-P0: PROTOKOL DOSYALARI OPCODE/STRUCT DIFF TABLOSU

> Tarih: 30.09.2026. Kapsam: 2b.1 matrisinin G4/G5 bandındaki 10 protokol dosyası
> (Protocol, DSProtocol, CSProtocol, JSProtocol, ESProtocol, SocketManager,
> SocketManagerUdp, Connection, IpManager + referans PacketManager). H-007 dersi
> gereği **tablosuz donor alımı yasaktır** — bu tablo donor alım ön şartıdır.
> Ham karşılaştırma ölçümleri: `BuildLog\envanter\2b1_diff_matrisi.csv`.

---

## 1. YÖNETİCİ ÖZETİ

- **Dış opcode (head) dispatch seti 5 dosyada birebir aynı:** Protocol (9),
  DSProtocol (4), CSProtocol (1), JSProtocol (1), ESProtocol (1). İstemciden
  gelen head kodları iki tarafta aynı — **istemci protokolü aynı revisions.**
- **Farklar tamamen gövde düzeyinde:** metot yüzeyi farkı **sıfır** (8 dosyada).
  Bizim-tekil handler'lar SPK özellikleri (CTCMini, RuudToken, AutoNapThe,
  LockWindow, CustomRanking, CB_NewQuest, XULY); donor-tekil handler'lar donor
  özellikleri (INVENTORY_HOLY_JOH, PMSG_DATA_PACKAGE/CGReqtimeState, SNS,
  DS 0x41 sub- bloğu). **Donör otomatik alınmaz; satır satır birleştirme (G4
  kuralı) uygulanır.**
- **Canlı kanıt bizim tarafı doğruluyor:** canlı `Sub-1\Data\Event\CTCMini\`
  (CongThanhChien.xml + GuildWin.ini) → bizim ServerInfo:503
  `gCTCMini.Load("Event\CTCMini\CongThanhChien.xml")` birebir canlı yol.
  Canlı exe'de BlackList.txt, Event\CTCMini\, Notice.txt string'leri mevcut.
- **Donör SocketManager.cpp bağımlılık istiyor:** `BlackList.h` + `APIGameGuard.h`
  (bizde yok) + `ServerDisplayer` (bizde VAR). BlackList *donör içinde de modül
  dosyası yok* (yalnız include) → **SocketManager donor alımı 2c'ye ertelendi.**
- **Karar: P0 sonucu donör protokol alımı ŞİMDİLİK YOK.** Yalnız güvenli
  whitespace-normalize katmanı (Connection, SocketManagerUdp) donor'dan
  alınabilir; içerik farkı olan dosyalarda bizim sürüm taban kalır.

## 2. DOSYA DOSYA TABLO

| Dosya | Bizim↔Donör satır | ben% (CSV) | Dış opcode seti | Metot yüzey farkı | Gövde fark karakteri | Karar |
|-------|-------------------|-----------:|-----------------|-------------------|----------------------|-------|
| PacketManager.cpp/h | 648↔648 | 100 (birebir) | — | — | YOK (birebir) | ✅ işlem gerekmez |
| Protocol.cpp/h | 8499↔7700 | 32 | **BİREBİR (9)** | SIFIR | Bizim-tekil case: 0x1A (LockWindow), 0x2E, 0x39 (CTCMini), 0x75 (CB_NewQuest), 0x7B (XULY), 0x7C (AutoNapThe), 0xEE (RuudToken), 0xFF (post-item); Donör-tekil case: 0x4F, 0x71 (CGReqtimeState/PMSG_DATA_PACKAGE), 0x8/0x9 (INVENTORY_HOLY_JOH), 0xFB (SNS, UP≥801) | 🚫 **bizim taban; donor-tekil handler'lar istenirse tek tek canlı-kanıtla** |
| DSProtocol.cpp/h | 6407↔5933 | 31 | **BİREBİR (4)** | SIFIR | Bizim-tekil: 0x40 (CustomRanking), 0x76 (DWarehouseGuildOpen), 0x7A/0x7B (AutoNapGame), 0xD3/0xD9/0xFB/0xFE sub-blok farkları; Donör-tekil: 0x41 (sub-blok), 0x75 (GDWarehouseGuild**Close**), 0xF8 (donorde **comment'li/pasif**!) | 🚫 **bizim taban** (donör 0xF8 zaten kapalı — donor avantajı yok) |
| CSProtocol.cpp/h | 1122↔1110 | 45 | **BİREBİR (1)** | SIFIR | Hunk 12; satır eşitliği 34↔34 → yer değişimi/stil | 🔄 whitespace-normalize adayı (G3'te) |
| JSProtocol.cpp/h | 819↔850 | 37 | **BİREBİR (1)** | SIFIR | Donör +9 satır (hunk 2) — küçük | 🔍 G3'te satır satır |
| ESProtocol.cpp/h | 1946↔1912 | 41 | **BİREBİR (1)** | SIFIR | Bizim +10 satır (hunk 1) — SPK eki | 🚫 bizim taban |
| SocketManager.cpp/h | 998↔1023 | 34 | — (dispatch yok) | SIFIR | **Bizim buffer 5×:** MAX_MAIN 40960↔16384, MAX_SIDE 81920↔16384, WORKER 16↔8 (SPK ölçek kararı — **KORUNUR**); donor ek: BlackList.h + APIGameGuard.h include + m_port girintisi; BlackList/APIGameGuard **bizde yok** | 🚫 **bizim taban; donor 2c'de bağımlılıklarıyla değerlendirilir** |
| SocketManagerUdp.cpp/h | 270↔270 | 30 | — | SIFIR | Hunk 7, satır eşit — stil | 🔄 whitespace-normalize adayı (G3'te) |
| Connection.cpp/h | 317↔317 | 27 | — | SIFIR | Hunk 8, satır eşit — yalnız boşluk/yerleşim (Init/Connect gövdesi birebir) | ✅ whitespace-normalize zararsız (G3'te) |
| IpManager.cpp/h | 172↔94 | 33 | — | IpTime/IpTime2/IpBlocked alan seti farkı | Bizim sürümde **FLOOD koruması** (2 bağlantı <1 sn → IP ban, LOG_ANTIFLOOD — Log.h:19'da enum mevcut) + İspanyolca yorumlar; donor temel sürüm | 🚫 **bizim taban (SPK güvenlik eki)** |

## 3. STRUCT SET FARKLARI (Protocol.h, 185↔172)

- **Bizim-tekil 20 struct (SPK)** — korunur:
  `CGPACKET_LOCKWINDOW, PMSG_POST_ITEM_RECV/SEND, PWMSG_HEAD2, PMSG_NEW_PET_CHARSET_SEND/2, NEW_PET_CHARSET/2, AUTOMOVE_REQ, PMSG_AUTORESET_INFO_RECV, CSENDGS_DOIMK(+INFOSAVE), DSGS_DOIMK_KQ, DKRS_COUNTLIST/ITEMINFO, SEND_COUNTLIST, PMSG_COUNTLIST_VIEWCHAR, GETINFOCHAR_DATA, RESETCODE_SEND, RECV_CUSTOMTIMECOUNT`.
  Opcode taşıma biçimi: struct yorumu (`// C1:78`, `// C2:F3:E5`).
- **Donör-tekil 7 struct** — istenirse canlı-kanıt şartıyla tek tek:
  `PMSG_PING_SEND (PSBMSG_HEAD, C1:F3:F1), PMSG_CHARACTER_OFF_RECV, SDHP_COMMON_SEND, DATA_VIEW_CHARINFO_SEND1 (CGReqtimeState — donor 0x71), PMSG_CUSTOM_VIEWITEM_RECV1, PMSG_INFO_STATUE_AND_GATE, PMSG_DATA_PACKAGE`.
  Bunların **hiçbiri canlı Data konfig kanıtı taşımıyor** (dosya yok, ini anahtarı yok).
- DSProtocol.h: donor-eklenen 16 / bizim-olan 66 (hunk 21) — bizim set zengin (SPK DS modülleri).

## 4. CANLI EXE / DATA KANIT TABLOSU

| Kanıt | Canlı konum | Desteklediği karar |
|-------|-------------|--------------------|
| `Event\CTCMini\CongThanhChien.xml` + `GuildWin.ini` | Sub-1\Data\Event\CTCMini\ | Bizim 0x39 CTCMini handler + ServerInfo:503 yolu canlıda aktif |
| `Data\BlackList.txt` | Sub-1\Data\ | Donör SocketManager BlackList hattı canlıda **yüklü** — modül 2c'de donor ile birlikte alınmalı |
| `Data\Util\Notice.txt` | Sub-1\Data\Util\ | Notice hattı canlıda donör-vari kullanımda (G3 Notice işi ile çakışma kontrol edilecek) |
| LOG_ANTIFLOOD enum | bizim Log.h:19 | IpManager flood koruması bizim derlememizde gerçek — koru |

## 5. P0 SONUCU VE 2b.2-C'YE ETKİ

1. **Donör protokol dosyası bu fazda ALINMAZ** (G4 kuralı onaylandı). Taban:
   bizim SPK sürümü — canlı kanıt bizimle.
2. **Donör-tekil handler/struct'lardan canlı-kanıt taşıyan yok** → 2c'de tek tek
   değerlendirilecek (PING/pong işlevi hariç — GCSendPing zaten bizim 0xFF bloğunda var).
3. **SocketManager donör alım ön şartı (2c):** BlackList.cpp/h + APIGameGuard
   modüllerinin donorde var olması gerekir — donor GameServer klasöründe yok;
   **donör ağacının tamamında aranmalı** (2c.1'de).
4. G2 bandındaki **MemScript(50)** diff'i protokol değildir (parser) → 2b.2-C'de
   normal G2 kuralıyla işlenir.
5. PacketManager birebir olduğu için **paket→protokol yönlendirme katmanı riski
   yok** — head setleri de aynı olduğundan istemci uyumluluğu iki tarafta eşit.

---

## 6. YENİDEN DOĞRULAMA (01.10.2026 00:55)

P0'dan sonra 10 protokol dosyasında **hiçbir değişiklik olmadığı** `git diff
fe4bc2505..HEAD` ile teyit edildi; tablo geçerliliğini koruyor. Canlı exe
(6.979.072 B) string kanıtları yeniden doğrulandı:
- `Event\CTCMini` canlı **VE** bizim derlemede mevcut (CTCMini handler kararı);
- `Notice.txt` iki tarafta da mevcut;
- `ThuMuaDoExc.txt` iki tarafta da mevcut;
- `BlackList.txt` canlıda `Sub-1\Data\` altında FİZİKSEL olarak VAR (string
  exe'de taşınmıyor — config varlığı kanıt; BlackList modül hattı 2c notu geçerli).

*P0 kapandı: tablo docs/11'de, donor protokol alımı kararı belgelendi. 2b.2-C (G2) onaylı.*
