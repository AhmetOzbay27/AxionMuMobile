# CHANGELOG — Axion Mu Source

> **Kural:** Projede yapılan HER değişiklik buraya kaydedilir — ne, neden,
> nasıl doğrulandı. Amaç: kalan yerin başka bir yapay zeka / geliştirici
> tarafından sohbet bağlamı olmadan anlaşılması.
> Format: [YY.AA.GG] Başlık → Değişen dosyalar → Neden → Doğrulama → Commit.---






## [26.10.04 14:15] build_all.sh --help duzeltmesi: log dosyalari artik sifirlanmiyor

**Ne yapildi**

`--help` ve bilinmeyen arguman yollarinda arg ayristirma, log kirpmasindan ONCEYE
alindi; boylece yardim / yanlis kullanim cagrilari kanit loglarini bosaltmiyor.
Kanit loglari (`build_all.log`, `build_all_summary.txt`, `build_all_console.log`)
duzeltilmis script ile 14:11 kosusunda yeniden uretildi: **12/12 PASS, SCRIPT_EXIT=0**.

**Dogrulama**

- `bash BuildLog/denetim/build_all.sh --help` oncesi/sonrasi log md5 leri birebir
  ayni: log `d18c958c`, ozet `710b355c`, konsol `154a02ab`; `HELP_EXIT=0`.
- Bilinmeyen arguman (`--bogus`): `BOGUS_EXIT=1`, loglar yine degismedi.
- 14:11 ozeti: client 12.034.048 / `91fa8da9...`, cs603 103.936 / `516d6a49...`,
  js603 943.616 / `4698cff7...`, ds603 1.034.240 / `4fe2cda2...`, gs603 10.656.256 /
  `02f695e2...`; cs803 `7e5775f8...`, js803 `504a1908...`, ds803 `dfddd76d...`,
  gs803 `d1cd380b...`; layout 603/0 + 803/1 "TUM PAKETLER HIZALI".

**Rapor:** `docs/38-TOPLU-DERLEME-SCRIPTI.md`
## [26.10.04 14:07] Tek komutlu dogrulanmis build script i: EX603 istemci + EX603/EX803 sunucu (docs/38)

**Ne yapildi**

`BuildLog/denetim/build_all.sh` yeniden yazildi: 9 derleme (Main + CS/JS/DS/GS x
EX603/EX803), hedef bazli loglar, tel duzeni + GS config makro dogrulamalari ve
md5 li PASS/FAIL ozeti; exit kodu sonucu yansitir. Test kosusu: 12/12 PASS, EXIT=0.

**Bulgular (kosu)**

- cs603/js603/ds603 izlenen ikilileri bayatti (30.09; docs/34 UUID duzeltmesi yoktu):
  yeniden derleme kod farki getirdi (CS .text 23 B, JS .text 31 B, DS +3.584 B) ->
  ikililer guncellendi.
- Main.exe kod ozdes (13 bayt meta) -> HEAD e geri alindi (docs/36 kurali).
- gs603 zaten gunceldi; degismedi.

**Rapor:** `docs/38-TOPLU-DERLEME-SCRIPTI.md`
## [26.10.04 13:33] 603 tel duzeni derleme-zamani kapisi: Main yalniz canli 603+0 derlenir (docs/37)

**Ne yapildi**

`verify_603_layout` denetimi urun kaynagina tasindi: `WSclient.cpp` icine `#error`
makro kilidi + 11 `static_assert` eklendi (tel govde 36/38/20/20, count ofsetleri
+35/+37/+19/+16, MONSTER CurHp/Level/Life +9/+10/+12). Bayat EX803 yorumlari
guncellendi (`WSclient.h` x3, `Defined_Global.h`).

**Kapsam (kullanici karari)**

Yalniz canli 603+0 kabul edilir; 603+1 / 803+1 / 803+0 ve digerleri C1189 ile kirilir.
EX803 istemci derlemesi artik bilincli kapi degisikligi gerektirir.

**Dogrulama**

- Canli makroyla tek dosya derleme: EXIT=0 (`BuildLog/denetim/gate_live603.log`).
- `/DGAMESERVER_HAISLOTRING=1` ve `/DGAMESERVER_UPDATE=803`: C1189 + EXIT=2
  (`gate_bad_hais1.log`, `gate_bad_803.log`).
- MSBuild Main Global Release|Win32 v143 (incremental): 0 error, EXIT=0
  (`main_gate_build.log`); WSclient.cpp yeniden derlendi, Main.exe linklendi.
- Projeye enjekte yanlis makro (gecici Directory.Build.props + SelectedFiles): C1189 +
  EXIT=1 (`gate_bad_msbuild.log`).
- Yeni Main.exe vs HEAD: kod bolumleri ozdes (yalniz 13 bayt meta) -> HEAD e geri
  alindi (docs/36 kurali).
- Harici denetim `verify_603_layout.cpp` tekrar: EXIT=0.

**Rapor:** `docs/37-603-DERLEME-KAPISI.md`
## [26.10.04 12:08] GetMain ikili karari: sahipsiz yeniden derleme HEAD e geri alindi (docs/36)

**Ne yapildi**

`GetMain/GetMainInfo.exe` + `GetMainInfo.pdb` (03.10 15:02 artigi) incelendi. Farkin
tamami derleme meta verisi (16 bayt = COFF ts 3B + 4x debug-dir ts 12B + RSDS age 4->5
1B); `.text` dahil tum kod bolumleri HEAD ile birebir ayni. Islevsel delta yok ->
`git checkout --` ile HEAD e geri alindi; ikililer zaten `32f5cdcb0` soydan geliyor.

**Dogrulama**

- `cmp -l`: tam 16 bayt fark (`BuildLog/denetim/getmain_cmp_exe.txt`).
- PE bolum denetimi (`BuildLog/denetim/pe_cmp.js`): fark yalniz HEADER=3 + .rdata=13;
  diger tum bolumler + dosya uzunlugu birebir ayni.
- Geri alma sonrasi md5 = HEAD: exe `c480e0ba...`, pdb `c1e9fe73...`; `git diff` bos.
- Yeniden derleme kopyasi `BuildLog/Getmain/GetMainInfo.rebuilt.{exe,pdb}` arsivde.

**Kural:** Kaynak degismeden ikili yeniden derleme commit edilmez.

**Rapor:** `docs/36-GETMAIN-IKILI-KARARI.md`
## [26.10.04 06:50] Main 603 tam derlemesi EXIT=0 - K2 kapatildi (docs/35)

**Ne yapildi**

K2 (Main tam derlemesi) kapatildi; istemci canli SPK 5.2 tel duzeniyle (603+0)
derlenip `ClientFile/Main.exe` yenilendi. Duzeltmeler: (1) `SPKData.h` icinde iki
`#define` tek satirda yapisikti (SPK_CAMERA_FPS_OFFSET yorumun icinde kaliyordu) ->
ayrildi; (2) `SPKMenuBar.cpp` `DisplayWidth` -> `DisplayWin` (CBInterface.h:21);
(3) `SPKMenuBar.cpp` icin `NewUISystem.h` include edildi (g_pBCustomMenuInfo makrosu,
CBChoTroi.cpp ile ayni desen). Ayrica bozuk `BuildLog/5Main/vc143.pdb` (C1033) kenara
alindi (kilitli degil, bozuktu); build PDB yi sifirdan yazdi.

**Dogrulama**

- MSBuild Global Release|Win32 v143 /m: 407 .cpp, 0 error, EXIT=0
  (`BuildLog/denetim/main_603_build.log`); 40 LNK4099 (detours.pdb) + 1 MSB8004
  disinda uyari yok.
- Yeni ikili: `ClientFile/Main.exe` 12.034.048 B, md5 91fa8da9d5e4b57f863504069d0e4f61
  (onceki 12.031.488 B / c49383bf...). Icinde 3C.0 etiketleri var("Etkinlik Saati"
  vb.) -> kod baglandi.
- 603 kilit: proje tlog unda GAMESERVER_UPDATE/HAISLOTRING override i YOK;
  `verify_603_layout.cpp` proje bayraklariyla EXIT=0 -> 603/0 + tel govde
  36/38/20/20 + count@+35/+37/+19/+16 + MONSTER +9/+10/+12 (canli PDB ile ayni).

**Degisen dosyalar**

`Source/5.Main/source/{SPKData.h,SPKMenuBar.cpp}` (K2 duzeltmeleri)
`Source/5.Main/source/{SPKData.cpp,SPKMenuBar.h,CBInterface.cpp}` + `Main.vcxproj(.filters)`
(3C.0 ajanindan devralinan, derleme icin zorunlu; ilk kez commit li)
`ClientFile/Main.exe` (izlenen ikili guncellendi)
`BuildLog/denetim/*` (build + verify kanitlari), `docs/35-MAIN-603-DERLEME.md`

**Rapor:** `docs/35-MAIN-603-DERLEME.md`
## [26.10.04 03:10] SPK 5.2 %100 uyum turu - tel duzeni + 2 gizli derleme kusuru (docs/34)

**Ne yapildi**

Istemci `PCREATE_*` yapilari sunucu `Viewport.h` kosullariyla birebir ayni hale getirildi
(yeni `GAMESERVER_HAISLOTRING` tel makrosu; istemcinin kendi `HAISLOTRING` UI makrosu tel
duzenine karismaz). Varsayilan = canli SPK 5.2: istemci `GAMESERVER_UPDATE=603`, sunucu
`Release_EX603` + `HAISLOTRING=0`. Canli boyutlar derleme zamaninda kilitlendi
(`static_assert` 36/38/20/20, yalniz 603+0).

**Gizli kusurlar (bu turda bulundu)**

1. `Release_EX603`+`HAISLOTRING=0` bu kaynakta HIC derlenmiyordu: `Protocol.cpp` icindeki
   `else { ...EquipInventory[n]... }` blogu kosulsuzdu (uye `User.h`'de `#if(HAISLOTRING)`
   icinde) -> C2039/C2660. Blok kosullu yapildi; GS EX603 simdi EXIT=0.
2. Istemci `GAMESERVER_UPDATE>=701` HIC derlenmiyordu: `PacketManager.h` icindeki
   `using namespace CryptoPP;` global `Singleton`'i belirsizlestiriyordu (C2872).
   Nitelendirildi; WSclient.cpp 603 ve 803 tanimlariyla EXIT=0.

**Bilinen kusurlarin kapatilmasi**

- docs/28: `UuidCreateSequential` donus degeri denetimi (4 sunucu `Protect.cpp`).
- docs/17 B3: respawn yoluna canli 0x538687 karsiligi `monster_add(class,true)` eklendi.
- docs/29 B6: `TypeDB=1` (gcoin) yolu icin LOG_RED uyarisi; kolon iki semada da yok.

**Dogrulama**

- `viewport_layout.js` 603/0, 603/1, 803/1: 4/4 HIZALI, exit 0 x3 (`BuildLog/denetim/layout_*.txt`).
- GS EX603 + GS EX803 + DS EX803: EXIT=0 (`BuildLog/denetim/build_spk52.log`, `gs_603.log`).
- Istemci tek dosya: `wsclient_603.obj` (4.850.853 B) + `wsclient_803.obj` (4.969.038 B), EXIT=0.
- Main tam derlemesi hala K2 (baska ajanin 3C.0 dosyalari) -> EXIT=1; dokunulmadi.

**Degisen dosyalar**

`Source/4.GameServer/GameServer/{stdafx.h,GameServer.vcxproj,Viewport.cpp,Protocol.cpp,ObjectManager.cpp}`
`Source/{1.ConnectServer,2.DataServer,3.JoinServer,4.GameServer}/*/Protect.cpp`
`Source/2.DataServer/DataServer/CB_AutoNapGame.cpp`
`Source/5.Main/source/{WSclient.h,WSclient.cpp,Defined_Global.h,PacketManager.h}`
`BuildLog/2e7/viewport_layout.js`, `BuildLog/denetim/*`, `docs/34-SPK52-UYUM-TURU.md`

**Rapor:** `docs/34-SPK52-UYUM-TURU.md`
## [26.10.04 02:05] Proje çapında denetim — amaç ↔ kaynak ↔ kanıt (docs/33)

**Ne yapıldı**

33 doküman, git durumu, taze derleme, canlı DB sorguları ve ikili md5 karşılaştırmasıyla
proje çapında denetim. Rapor: `docs/33-DENETIM-PROJE-CAPINDA.md`; kanıt: `BuildLog/denetim/`.

**KRİTİK bulgular**

1. **Dağıtım ikilileri bayat (K1):** EX803 derlemeleri dağıtım yoluna akmıyor — GS
   `MuServe Classic 5.2 Lorencia\GameServer` (izlenmeyen), DS `Source\...\Release`.
   MuServer'daki yığın 30.09-02.10 tarihli ve 2e.6-2e.9 düzeltmelerini içermiyor
   (4 bileşenin md5'i yeni derlemelerden farklı). H-018 sonrası Main.exe de yok
   (`ClientFile/Main.exe` 03.10 01:00, H-018 öncesi) → çalışan çift eski.
2. **Main derlemesi kırık (K2, 3C.0 işi, commit'siz):** `SPKData.h:40`'ta iki `#define`
   tek satıra yapışmış (`SPK_CAMERA_FPS_OFFSET` yorum içinde) → C2065+C2660;
   `SPKMenuBar.cpp:158/181` `DisplayWidth`/`g_pBCustomMenuInfo` tanımsız;
   `BuildLog\5Main\vc143.pdb` C1033. `MSBuild "Global Release"` EXIT=1.

**ORTA bulgular**

3. Durum belgeleri (00/02/03) 2c.1 ve 2e.6-2e.9'u yansıtmıyordu → bu commit'te güncellendi.
4. docs/30-31 sayımları tablolarıyla uyuşmuyordu (GS 11→10, istemci 8→7, docs/31 6→5);
   docs/30'daki bozuk ham-veri yolu düzeltildi + denetim notları eklendi.
5. `GetMain/GetMainInfo.exe` çalışma ağacında 15:02'de yeniden derlenmiş, commit'siz
   (md5 `1991c037…`; HEAD `c480e0ba…` = docs/20 iddiası). Commit edilmedi.
6. H-018 kaynakta kapalı doğrulandı (4/4 elle) ama çalışan çiftte değil; `viewport_layout.js`
   dört alanı eşleyemeyip yine "HIZALI" diyor (eşleme tablosu eksik).

**DOĞRULANANLAR (taze ölçüm)**

* Sunucu hattı: GS/DS/JS/CS `Release_EX803` → EXIT=0 ×4.
* DB: MuOnlineS6 14/14 tablo; DataNapGame'de STT yok; gcoin iki DB'de de yok.
* docs/32 opcode iddiası: `Protocol.cpp` benzersiz case = 166 (birebir).
* HEAD == origin/main; sunucu süreçleri kapalı (§5.1); pano JSON geçerli (CR=0).

**Değişen dosyalar:** docs/33 (yeni) · docs/00 · docs/02 · docs/03 · docs/30 · docs/31 ·
docs/CHANGELOG.md · Dashboard/data/{sohbet,sonuc}.json · BuildLog/denetim/*

**Kanıt:** docs/33 · BuildLog/denetim/{build_server.log, build_main.log, viewport_layout_fresh.txt}

## [26.10.03 22:55] 2e.9 — 14 tablo için kalıcı veri katmanı + MEMB_INFO tutarlılığı doğrulaması (docs/29)

**Ne yapıldı**

`CDataStore` (DataStore.h/.cpp, 284+775 satır): 14 tablo için tipli kayıt
yapıları, hazırlanmış ifade (prepared statement) ve parametre bağlama. Hesap/
karakter adları artık SQL metnine gömülmüyor. Açılışta `SchemaCheck()` 14
tabloyu yoklar; eksikse sunucu bozuk semada sessizce çalışmaz. Yeni kullanıcı
DSN'i `MuOnlineS6` + DataServer.ini buna çevrildi.

**Önce düzeltilen varsayım:** JoinServer ile DataServer arasında protokol
akışı **yok** — JoinServer ConnectServer'a (63001), DataServer GameServer'dan
(63002) besleniyor. Ortak yüzey aynı veritabanındaki `MEMB_INFO`. Kullanıcı
kararıyla o doğrulandı.

**Bulunan ve düzeltilen GERÇEK KUSURLAR**

1. **DataNapGame.STT yok** — `CB_AutoNapGame.cpp` semada olmayan `STT`
   kolonunu okuyordu ve iki UPDATE'de `and STT='%d'` kullanıyordu; canlı DB'de
   `Msg 207 Invalid column name 'STT'`. Satır artık (Account,Name,Checking)
   üçlüsüyle tanımlanıyor.
2. **BindInt ölü adres** (uçtan uca test buldu) — parametre `&val` yerel
   kopyasına bağlanıyordu; ODBC tamponu `SQLExecute` anında okuduğu için
   **bütün tam sayı parametreleri bozuk** gidiyordu. `m_ParamInt[]` üyesine
   kopyalandı.

**Doğrulama**

* `BuildLog/2e9/dstest/ds_e2e.exe` gerçek MuOnlineS6 + MuOnline üzerinde:
  **TEST_EXIT=0 · 59 GECTI · 0 KALDI** (BuildLog/2e9/e2e_result.txt).
  14 tablo INSERT→LOAD→UPDATE→LOAD→DELETE; MEMB_INFO için iki sunucunun
  sorgusunun aynı değeri okuduğu, yazmaların birbirini gördüğü, olmayan
  hesapta aynı davrandığı doğrulandı.
* DataServer derlemesi **MSBUILD_EXIT=0** (DataServer.exe 1.062.912 B).

**AÇIK KALANLAR (düzeltilmedi)**
* `gcoin` kolonu ne MuOnline'da ne MuOnlineS6'da yok →
  `CB_AutoNapGame.cpp` gcoin sorguları çalışma anında hata veriyor.
* `MuOnlineS6.Character` boş → `CustomNpcQuest` FK testi ATLANDI.
* Mevcut 391 `ExecQuery` çağrısının 14 tabloya dokunan kısmı henüz katmana
  taşınmadı.

**Kanıt:** docs/29-14-TABLO-KALICI-VERI-KATMANI.md · BuildLog/2e9/

## [26.10.03 21:40] Sunucu kaynaklarında okuma + mesaj hatası taraması — yok sayılan dönüş değerleri düzeltildi

**Ne yapıldı**

Tarama tahminle değil ölçümle yapıldı: `BuildLog/2e8/scan_ignored_returns.js`
ConnectServer/DataServer/JoinServer/GameServer kaynak ağaçlarını tarar ve
dönüş değeri yakalanmamış API çağrılarını listeler. İlk envanter: **121**
(kapsama `ReadFile`, `ReadProcessMemory`, `recv`, `fread`, `SendMessage`,
`TranslateMessage`, `FindWindow`, `closesocket`, `WriteFile` vb. aileler alındı).

**GERÇEK KUSUR 1 — `CPacketManager::LoadKey` (okuma hatası)**
Dört `ReadFile` dönüşü **hiç denetlenmiyordu**. Kırpık anahtar dosyasında
`HeaderInfo`/`table[]` stack artığıyla dolup `LoadEncryptionKey` yine de
`1` dönüyor; sunucu sessizce yanlış şifreleme tablosuyla çalışıyordu.
→ `CPacketManager::ReadExact` yardımcısı: dönüş değeri **ve** okunan bayt
sayısı denetleniyor (kırpık dosyada `ReadFile` TRUE dönebilir), hata halinde
log + `CloseHandle` + `return 0`.

**GERÇEK KUSUR 2 — combo kutusu (mesaj hatası)**
`LB_ADDSTRING` başarısız olduğunda `LB_ERR` (-1) dönüyor, ama bu değer
`LB_SETITEMDATA`'ya olduğu gibi geçiriliyor ve onun dönüşü de yok sayılıyordu.
→ `LB_ERR` denetimi, `LB_SETITEMDATA` hatasında `LB_DELETESTRING` ile geri alma,
geri alma da `LB_ERR` dönerse loglama.

**Gerçek hata sinyali olanlar kontrol edildi**
- `SB_SETPARTS`: başarıda 0 döner, sıfır dışı hatadır → kontrol + log eklendi
  (GameServer.cpp, ServerDisplayer.cpp).

**Hata sinyali olmayanlarda uydurma kontrol yazılmadı**
- `SB_SETTEXT` önceki metnin uzunluğunu döndürür (`0` = "önceki metin yok"),
  `TranslateMessage` yalnız karakter mesajlarında TRUE döner. Bunlarda
  `(void)` cast'i ile "bilinçli yok sayma" kodda görünür hâle getirildi ve
  gerekçesi yorumda yazıldı.

**Nasıl doğrulandı**
- `node BuildLog/2e8/scan_ignored_returns.js` → **OKUMA 0, MESAJ 0**
  (yardımcı sınıf 98; okuma/mesaj değil, §6'da gerekçesiyle listelendi).
- Dört sunucu da temiz derlendi: GameServer / DataServer / JoinServer /
  ConnectServer **EXIT=0** (`BuildLog/2e8/build_all_2e8.log`).
- `gLog` için `PacketManager.cpp` ve `GameServer.cpp`'ye `#include "Log.h"`
  eklendi (stdafx.h içermiyor; proje düzeni: `Connection.cpp` böyle yapıyor).

**Tarayıcının kendisi de sınandı (negatif kontrol)**
`BuildLog/2e8/scanner_selfcheck.js` bilerek kaçak bir `ReadFile` ve bir
`SendMessage` yerleştirip sayımın her birini +1 arttırdığını, buna karşılık
atama / `(void)` / `if` biçimlerini atladığını (toplam +2, +5 değil)
doğruluyor → **EXIT=0**. "0/0" sonucunun bozuk bir tarayıcıdan gelmediği kanıtı.

**"OKUMA 0 · MESAJ 0" ifadesinin ölçülmüş sınırı**
Bu sonuç tarayıcının tanımlı API aileleri içindir. Aynı kural, listelerde
olmayan okuma ailesine uygulandığında (`read_gap_probe.js`) **102** çağrı
daha çıkıyor: `GetPrivateProfileString` 88 (sistematik sessiz ayar hatası —
`ReadFile` kusuruyla aynı sınıf), `UuidCreateSequential` 4 (gerçek kusur:
`UUID` başlatılmıyor, donanım kimliği stack artığından türetiliyor),
`localtime` 4 / `localtime_s` 2 / `ungetc` 4 (kusur değil). Ayrıntı docs/28 §8.

**Kanıt:** docs/28-OKUMA-MESAJ-HATASI-TARAMASI.md · BuildLog/2e8/

## [26.10.03 20:50] H-018 KAPANDI — viewport paketleri sunucu↔istemci arasında bayt-bayt hizalandı

**Ne yapıldı**

**(A) Canlı sunucunun paket düzeni ölçüldü**
- İstek "canlı SPK sunucusundan paketleri yakala" idi; **yakalama imkânsızdı**:
  canlı yığın kapalı (44405 ve 55858 `ECONNREFUSED`, `netstat`'ta yok) ve
  `4.MuServer/Sub-1/` içinde yalnız `GameServer` var (`Main`/`Connect`/`Join` yok).
- Bunun yerine **canlı `GameServer.exe`'ın kendi PDB'si** okundu. Araç:
  VS DIA SDK ile `BuildLog/2e7/pdbtype.cpp` → `pdbtype.exe`; çıktı
  `BuildLog/2e7/live_pdb_viewport_layout.txt`. PDB ile exe aynı derleme
  (zaman damgası `6AAE6177` = 19.09.2026).
- `dumpbin /disasm` + `GameServer.map` ile çapraz doğrulama
  (`live_gs_GCViewportPlayerSend.asm`): sabit kısım `0x6D→0x90` = **36 bayt**,
  ardından `GenerateEffectList` çıktısı.

**(B) Kök neden: derleme yapılandırması**
- Canlı: `HAISLOTRING=0` (MuunItem yok) + `GAMESERVER_UPDATE>=402`
  (disassembly'de `GetDuelArenaBySpectator` çağrısı) → **Release_EX603**.
- Bizim 2e.6 derlemesi `Release_EX803`'tü: sunucu 49 bayt gönderiyor,
  istemcinin `PCREATE_*` yapıları `attribute`/`level`/`MaxHP`/`CurHP`
  alanlarını içermiyordu → pet tipi `attribute`'tan, `s_BuffCount` muun
  item'ın düşük baytından okunuyor, viewport bozuk geliyordu.

**(C) Düzeltme** (kullanıcı kararı: EX803 korunur, alanlar istemciye eklenir)
- `Source/5.Main/source/WSclient.h`: dört `PCREATE_*` yapısına
  `Attribute` → `MuunItem[2]` → `Level[2]` → `MaxHP[4]` → `CurHP[4]`
  kabloda aynı sırayla eklendi.
- `Source/5.Main/source/WSclient.cpp`: üç viewport alıcısında `c->Level`
  artık `level[2]` kablodan okunuyor.
- `Viewport.h` + `Viewport.cpp`: canlı `< 701` `PMSG_VIEWPORT_MONSTER`
  varyantı (`CurHp`/`Level[2]`/`Life`) eklendi (EX803 dalı değişmedi).

**Yeni araçlar:** `BuildLog/2e7/{pdbtype.cpp, viewport_layout.js, apply_h018.js}`

**Nasıl doğrulandı**
- `node BuildLog/2e7/viewport_layout.js 803 1` → **`### TUM PAKETLER HIZALI`,
  exit 0** (`layout_803_1.txt`).
- `node BuildLog/2e7/crosscheck_live_parity.js` → **exit 0**; dört yapının her
  alan ofseti canlı PDB ile otomatik karşılaştırıldı (`crosscheck_live_parity.txt`).
  Kanıt dosyası `pdbtype ... 1` (derinlik 1) ile alan ofsetlerini içerecek şekilde yeniden üretildi.
- Aynı araç `603 0` ile çalıştırıldığında canlı PDB'nin değerlerini
  **birebir** üretiyor (PLAYER 36/count@35, CHANGE 38/count@37,
  MONSTER CurHp@9-Level@10-Life@12-count@16-20, SUMMON name@9/count@19).
- GameServer `Release_EX803`: **`GS_EXIT=0`**, 11.294.208 B,
  md5 `fd7e2c14f891ddbf9599967741468e2e`.
- `WSclient.cpp` tek başına derlendi: **`WSCLIENT_BUILD_EXIT=0`**, 4.852.503 B.
  (Main projesinin tam derlemesi, başka bir ajanın aynı anda düzenlediği
  `SPKData.cpp` / `SPKMenuBar.cpp` dosyalarındaki 4 hatada duruyor;
  `ClCompile` varsayılanı `ErrorAndStop` olduğu için derleme oraya kadar
  gelmiyor. Bu dosyalara dokunulmadı.)

**Kabul edilen fark:** dağıtım yapımız canlıdan PLAYER/CHANGE 13, MONSTER 1,
SUMMON 11 bayt farklı (bilinçli). Tam parite için `Release_EX603` +
`HAISLOTRING=0` yeterli — kaynak bunu destekliyor.

**Commit:** H-018 kapanışı (kod + kanıt + docs)

## [26.10.03 15:30] C-02 KAPANDI — DB şema denetimi + sunucu hattı **tamamen derlenebilir** hale getirildi

**Ne yapıldı**

**(A) C-02 — DB şema uyumu denetimi (sessizce düşmüş görev)**
- docs/03, C-02'ye *"tam şema uyumu kontrolü Faz 3.1'de yapılacak"* notu koymuştu;
  Faz 3.1 kapandı ama kontrol **hiç yapılmamıştı**. Bu turda kapatıldı.
- Denetim gerçek SQL'e karşı yapıldı: 743 kaynak dosyası / 308 SQL dize literal'i tarandı
  (`BuildLog\2e4\c02_schema_audit.js`), canlı `MuOnlineS6`'daki 55 tabloyla karşılaştırıldı.
- **Bulgu:** kaynak kodun kullandığı **15 tablodan 14'ü DB'de yoktu** (1'i `dbo` şema
  önekinin yanlış okunmasıydı). Bu tablolar kullanıldığında "Invalid object name" verir.
- **Yama:** `ServerTools\MuServer_S6_2020\DB\SQL\Update 12 - C02 Missing Tables.sql`
  (13 tablo, `IF OBJECT_ID(...) IS NULL` korumalı). `CustomNpcQuest` için resmi
  `Update13` betiğindeki tanım birebir kullanıldı (varsayılanlar + `Character(Name)` FK).
- **Kaynakta 2 gerçek hata düzeltildi:**
  1. `ChoTroi.cpp` — `ItemMarketData` çalışma anında oluşturuluyor ama
     **eksik ve kırıktı**: `TypeItem`/`Time`/`Pass` sütunları hiç oluşturulmuyordu
     (`GDReqItemSell` INSERT'i bunları yazıyor → `Invalid column name`), ve `CREATE TABLE`
     koşulsuz olduğu için DataServer ikinci kez açıldığında tüm `ALTER`'lar da
     başarısız oluyordu. Tüm ifadeler `IF OBJECT_ID` / `IF COL_LENGTH` korumalı yapıldı.
  2. `c02_schema_audit.js` — `INSERT INTO dbo.MEMB_INFO(...)` yazımında tablo adı `dbo`
     sanılıyordu; üç regex'e şema öneki desteği eklendi.

**(B) Sunucu hattı hiç derlenmiyordu (94 hata, 4 ayrı kırılma)**
- `DataServer`: **24 hata** — `std::transform` bulunamıyordu; `GuildMatching.cpp` ve
  `PartyMatching.cpp` `<algorithm>` include etmiyordu (aynı projede `CharacterManager.cpp`
  doğru yapıyor). İkisi tarandı: `<algorithm>` içermeyen tek bu iki dosyaydı.
- `GameServer`: **68 derleme hatası**
  - 8 × C1083 — `SPK\` altındaki 8 yeni modül kök dizindeki `stdafx.h`/`Protocol.h`/`User.h`
    dosyalarını bulamadı; `AdditionalIncludeDirectories` yalnız `Release_EX603` ve
    `Debug_EX603`'te tanımlıydı. `GameServer.vcxproj`'a koşulsuz `$(ProjectDir)` eklendi.
  - 56 × C2086 — `Viewport.h`'te `MuunItem[2]` **iki kez** tanımlıydı (`HAISLOTRING` bloğu +
    `GAMESERVER_UPDATE>=803` bloğu). İki blok karşılıklı dışlandı; paket boyutu her
    konfigürasyonda **tek 2 bayt alan** olarak aynı kaldı. `Viewport.cpp`'deki 4 yazıcı
    bloğu da aynı koşulla eşitlendi.
  - 4 × C2065 — `BotAlchemist.cpp` `MUUN_INVENTORY_SIZE` kullanıyor, `MuunSystem.h`
    include edilmemiş → include eklendi.
- `GameServer`: **2 link hatası**
  - LNK1104 `cryptlib.lib` yok → `cryptlib.vcxproj` v100 istiyor, makinede yok;
    `-p:PlatformToolset=v143` ile yeniden derlendi (30.742.380 B).
  - LNK2001 `_fprintf` / `___iob_func` — `mapm.lib` 2015'te VC6 ile üretilmiş
    (`MKALLMSC.BAT`: `cl /c /O2 /W3 /Zl map*.c`); `mapmutil.c` `fprintf`/`stderr`
    kullandığı için v100 adlarıyla derlenmiş. `BuildLog\2e4\build_mapm.bat` ile aynı
    bayraklarla yeniden üretildi. Orijinal `mapm.lib.v100.bak` olarak korundu.
- `ConnectServer` ve `JoinServer` **aynen temiz** derlendi (dokunulmadı).

**Doğrulama**
- SQL: `sqlcmd -S 'WIN-4TMUUQ42DNH\SQLEXPRESS' -d MuOnlineS6 -E -i "...\Update 12 - C02 Missing Tables.sql"`
  → `EXIT=0`. Tablo sayısı **55 → 69**. Betik **iki kez** çalıştırıldı → ikisinde de
  `EXIT=0`, sayı 69'da kaldı (**idempotency kanıtı**).
- Yeniden denetim: `node BuildLog\2e4\c02_schema_audit.js .` →
  `taranan=743 literal=308 db=69 kullanilan=49 **eksik=0** tam=49 fazla=20`.
- Fonksiyonel duman testi (`BuildLog\2e4\c02_smoke.sql`, `EXIT=0`): 14 tabloya kaynak
  kodun **birebir** gönderdiği INSERT/UPDATE/SELECT cümleleri koşuldu — hepsi geçti
  (envanter blob'ları tam boyut: 256/512/992 bayt; `CustomNpcQuest` FK üzerinden;
  `PentagramJewel` 17 sütun). Test satırlarının hepsi silindi (8 tabloda 0 satır doğrulandı).
- Derleme kanıtı: `BuildLog\2e6\build_evidence.txt`

| Bileşen | Yapılandırma | Boyut | md5 |
|---|---|---|---|
| GameServer.exe | Release_EX803 | 11.294.208 | `3f8f5723c8ae3be1ea645dcce3fc68a4` |
| DataServer.exe | Release_EX803 | 1.059.328 | `b1e4e23fbcb94f85d97455d913ef64cb` |
| JoinServer.exe | Release_EX803 | 943.616 | `2c7f74bbb9a9d541bb988bf28eabddcd` |
| ConnectServer.exe | Release_EX803 | 103.936 | `8f6e6a78d5fa8fdadfd6fce933aa4fcb` |
| GetMainInfo.exe | Release | 3.723.776 | `1991c037ead6b59129f77e51c0774ae7` |

**Neden önemli**
- C-02 açık kalemi kapandı; 14 özellik "Invalid object name" hatasıyla çalışmıyordu.
- Kaynağın **hiçbir** sunucu konfigürasyonu daha önce derlenmemişti. Bu, 2c fazında
  yazılan 8 SPK modülünün de derleme kanıtı olmadığı anlamına geliyordu.
- **Açık risk kayda geçti:** `docs/04` **H-018** — GS `PMSG_VIEWPORT_PLAYER` paket düzeni
  ile istemcinin `PCREATE_CHARACTER` düzeni aynı değil (`MuunItem` konumu, `attribute`/
  `level`/`MaxHP`/`CurHP` alanlarının yokluğu; pet ekipmanı vs muun envanteri semantiği).
  Derleme kırılması giderildi ama protokol doğruluğu **tahminle çözülmedi**; canlı paket
  yakalama ile Faz 3.3'te doğrulanacak.

**Değişen dosyalar** — `docs/26-C02-DB-SEMA-UYUMU.md` (yeni) ·
`ServerTools/MuServer_S6_2020/DB/SQL/Update 12 - C02 Missing Tables.sql` (yeni) ·
`Source/2.DataServer/DataServer/ChoTroi.cpp`, `GuildMatching.cpp`, `PartyMatching.cpp` ·
`Source/4.GameServer/GameServer/GameServer.vcxproj`, `Viewport.h`, `Viewport.cpp`,
`BotAlchemist.cpp` · `Source/Util/mapm/mapm.lib` (yeniden üretildi) ·
`BuildLog/2e4/{c02_schema_audit.js,c02_need_columns.js,c02_bindcol.js,c02_smoke.sql,
c02_smoke_output.txt,db_tables.txt,c02_schema_report.txt,c02_missing_columns.txt,
c02_dataserver_build*.log,build_mapm.bat}` ·
`BuildLog/2e6/{build_evidence.txt,*.log,dashboard_patch.js}` ·
`docs/03`, `docs/04` (H-018), `docs/30`, `Dashboard/data/{sohbet,sonuc}.json`

**Commit:** `c74b2343f`

---



## [26.10.03 06:30] GITHUB — kaynak GitHub'a yüklendi (remote `origin`)

**Ne yapıldı**
- `git remote add origin https://github.com/AhmetOzbay27/AxionMuMobile.git`; `main` dalı
  uzak depoya gönderildi. Depo public, default branch `main`, push öncesi boştu (`size: 0`,
  branch yok) → force gerekmedi.
- **Kimlik doğrulama:** `credential.helper=manager` (Git Credential Manager) kayıtlı tokenı
  kullandı; PAT gerekmedi, token hiçbir dosyaya veya `.git/config`'e yazılmadı.
- **Güvenlik taraması (push öncesi):** izlenen dosyalarda parolalar bulundu —
  `MuServer\3.JoinServer\JoinServer.ini` `GlobalPassword = KENDEV2039`,
  `ServerTools\MuServer_S6_2020\JoinServer\JoinServer.ini` `GlobalPassword = AFgRVFTYDd`,
  `MuServer\6.DatEditor\Config.ini` `Password=AIUEWUROE12`, MuEditor `config.ini`
  `Password=2n2zjw13`, `AutoTrain.xml` bot hesap/parolaları. Kullanıcı bilgilendirildi ve
  "olduğu gibi push et" kararı verildi: bunlar MuServer'ın herkese açık dağıtımlarındaki
  varsayılanlar, GitHub/kişisel hesap parolası değil. (Paralel ajan `3142bb439` bu sırada
  `DataServer.ini`'deki gerçek DB parolasını zaten temizlemişti.)

**Neden** — 2e.5 ve öncesi tüm çalışma (117 commit, ~742 MB paket) yalnızca yerel `.git`
içindeydi; `.git/config` boştu, `gh` CLI ve SSH anahtarı yoktu. Kullanıcı talebi: "githuba yükle".

**Doğrulama** — `git ls-remote origin` → `refs/heads/main = c0e41b24b` (2e.5 commit'i) =
yerel HEAD; GitHub tree API 50.474+ blob döndürdü (üst sınır nedeniyle kırpılmış);
`raw.githubusercontent.com` üzerinden `docs/25` indirildi → HTTP 200, 11.071 B = yerel
dosya boyutuyla birebir. Paralel ajanların 7 commit'i de GitHub'da; son durum
`main == origin/main == 564a69c69` (0 ileri / 0 geri).

**Commit** — `8de544f8d` (GitHub push kaydı: remote origin + pano + CHANGELOG).


## [26.10.03 01:20] 2e.5 — B-08 (SPK ToolTipText yükleyicisi) + Faz 3 E2E bağlantı kanıtı (docs/25)

**Ne yapıldı**
- **B-08 uygulaması:** canlı pakette `itemtooltiptext_<Lang>.bmd` yok; aynı tablo düz metin
  olarak `Data\SPK\Config\ToolTipText.txt` (19 kayıt) içinde. Yeni yükleyici
  `load_item_tooltip_text_spk()` (`Source\5.Main\source\ZzzInfomation.cpp`) bu tabloyu
  `tooltip_text_data[]`'ya aktarır (markup `<f c='#RRGGBB'>…</f>` temizliği, `type=-1`),
  ardından gelen `set_item_text_tooltip()` map'i kurar. Çağrı noktası: `ZzzOpenData.cpp`
  — `itemtooltiptext` çözümlemesi başarısızsa `ToolTipText_<Lang>.txt` çözümleyiciyle
  (`SPK_ResolveAssetPath` → `Data\SPK\Config\ToolTipText.txt`) yüklenir. Runtime kanıtı
  KEN.txt'ye yazılır (Release'te `g_ErrorReport` dosyaya yazmaz).
- **Faz 3.1 test yığını:** CS/DS/JS/GS (bizim derlemeler) + `MuOnlineS6` DB +
  `MuOnlineS6ODBC` DSN; GS `ServerVersion=1.03.34`/`ServerSerial=!571Axion@Mobile`;
  CS `ServerList.ini` gerçek IPv4:55901; `e2etest` hesabı; istemci paketinde
  `ConnectIP.bmd` = 45.87.120.29:63000 (`make_connectip.js` üreticisi).
- **Faz 3.2 (kısmi):** istemci ConnectServer'a **TCP ESTABLISHED** (t≈4,4 s, 100 ms
  örnekleme + PID bazlı netstat) ve **sunucu seçim ekranı** render edildi; diyalog/çökme yok.
- **Yeni bulgular:** (H-016) istemci `127.0.0.1` hedefini kasten reddediyor
  (`WSctlc.cpp:230` → `MESSAGE_SERVER_LOST`, hiç SYN yok) → E2E hedefi makinenin gerçek
  IPv4'ü olmalı; (H-017) ajan oturumu `disconnected` (`GetForegroundWindow()==0`) →
  UI tıklaması otomatikleştirilemiyor, sunucu seç → GS/login adımı etkileşimli oturum ister.
- **Kapanış:** H-005/H-006/H-007 kapatıldı (docs/04); B-08 durumu docs/03'te güncellendi;
  Faz 3 durumu docs/02'de işlendi. Test yığını kapatıldı (§5.1) ve geçici kopyalar
  silindi (`2e2\deploy`, `2e3\deploy`, `5Main` → ~513 MB).

**Neden** — B-08, canlı paketin tooltip metin katmanını bizim istemci şemasına bağlayan
son içerik kalemiydi; E2E turu da istemcinin gerçek sunucu yığınına bağlanabildiğini
kanıtlamak için yapıldı (Faz 3 çıkış kriteri).

**Doğrulama** — Derleme: `BuildLog\2e4\build_2e5.log` + `build_2e5b.log` (hata yok,
LNK4099 hariç), exe md5 `c49383bf62c412bf53adf1ec62c67f77` (12.031.488 B).
Runtime: run26 KEN.txt → `[SPK] ToolTipText: 19 kayit`; `run21/22/26` TCP kanıtı
(ESTABLISHED 45.87.120.29:63000) + sunucu seçim ekranı ekran görüntüsü; negatif kanıt
run19/run20 (127.0.0.1 → trap'e hiç bağlantı yok + popup). Rapor: **docs/25**;
kanıt: `BuildLog\2e4\{results,shots,evidence}`.

**Commit** — `010c41da8` (2e.5 B-08 SPK ToolTipText yükleyicisi + Faz 3 E2E bağlantı kanıtı).


## [26.10.03 00:40] PROTOKOL KAYDI (docs/32) — 16 özelliğe opcode tahsisi

**Ne yapıldı**
- GS `Protocol.cpp` opcode envanteri tarandı: 166 kullanılı / 90 boş.
- `PBMSG_HEAD3` konvansiyonu (0xC1 + boyut + head + sub) ve istemci `ProtocolSend.h` kabuğu belgelendi.
- 16 SPK özelliğine C→S + S→C opcode çiftleri tahsis edildi; iki tarafı kayıt kuralı (GS case + veri yapısı + istemci alıcı + istemci gönderici) kabul ölçütü tanımlandı.
- H-007 (0x35 çift tanım) bu kuralın gerekçesi olarak tabloya not edildi.

**Neden** — İstemci tarafı yazılırken opcode tahsisi sözlü kararla yapılırsa çakışma ve "yarım özellik" riski var; tek kayıt belgesi bu riski kapatır.

**Doğrulama** — opcode listesi `Protocol.cpp` üzerinden regex ile üretildi (166/90); tahsis edilen değerlerin hiçbiri kullanılan listede değil. Kod değişikliği yok.



## [26.10.03 00:30] PARİTE ÖLÇÜMÜ — 5 eksenli manifest (docs/30) + istemci ekseni iş emri (docs/31)

**Ne yapıldı**
- `docs/30-PARITE-MANIFESTI.md` (yeni): parite tanımı **5 eksene** çıkarıldı
  (GS / CFG / CLI / PROTO / E2E). 56 özellik satırı `docs/13`'ten otomatik ayrıştırıldı,
  her satıra canlı kanıt (canlı `Data` ağacı config varlığı, bizim GS kaynak dosyamız,
  istemci dosya eşleşmesi) eklendi. Makine-okunur hali:
  `BuildLog/envanter/parite_ozellik_kaniti.tsv`.
- `docs/31-ISTEMCI-EKSENI-IS-EMI.md` (yeni): 3. eksen için iş emri. **Ana keşif:** canlı
  istemcinin özellik listesi `GetEngine.ini` içindeki 20 slotluk `MENU_BUTTON_xx` dizisidir;
  bizim istemci kaynağında bu anahtarların 0 referansı var.
- İşçi / süpervizör / patron rol-ve-yetki protokolü docs/30 §4'e yazıldı (düzenleme
  yetkisi yalnız işçide; süpervizör kanıt doğrular).

**Ne ölçüldü (03.10.2026)**
| Eksen | Yeşil | Toplam |
|---|---|---|
| 1 GS kaynağı | 11 | 60 |
| 2 Config (canlı şema) | 28 | 56 |
| 3 İstemci karşılığı | 8 | 60 |
| 4 Protokol kaydı | 5 | 60 |
| 5 E2E | 6 | 60 |

**Neden** — 2c iş emri yalnız 1. ekseni (sunucu modülü) ölçüyordu; bu yüzden 60 kalemlik
plan ilerlerken parite gelmiyordu. İstemci tarafında 14 özellik penceresi hiç yazılmamış
durumdaydı ve bu hiçbir dokümanda görünmüyordu. Manifest ölçümü 5 eksene taşıyarak
eksik işi görünür kılar.

**Doğrulama** — tüm sayılar dosya sistemi taramasından üretildi (docs/13 ayrıştırma,
canlı `4.MuServer/Sub-1/Data` recursive ad taraması, `Source/5.Main/source` dosya adı
eşleşmesi, `GetEngine.ini` MENU_BUTTON regex'i); ham çıktı `BuildLog/envanter/*.tsv`'de.
Kod değişikliği yok → derleme etkilenmez.

## [26.10.02 22:10] 2e.4 SPK-first varlık çözümleyici + harita/nesne yol paritesi (H-011/H-012/H-014/H-015)

**Ne yapıldı**
- `Source\5.Main\source\SPKAsset.cpp` (yeni) + `SPKData.h` bildirimleri:
  `SPK_ResolveAssetPath` / `SPK_AssetExists` — `Data\Local\<Ad>.bmd` →
  `Data\SPK\Config\<Ad>.bmd`, `Data\Local\<Lang>\<Ad>_<Lang>.<ext>` → `Config\<Ad>.<ext>`,
  `NpcName(<Lang>)`, `Data\Gate.bmd`; küçük harf + `/` ayraç normalize; karşılık
  bulunamazsa istek aynen döner. 20+ çağrı noktası (ZzzOpenData, QuestMng,
  CreditWin, ItemAddOptioninfo, ServerListManager, w_PetProcess, NewUISlideWindow,
  UIJewelHarmony, w_BuffScriptLoader, CSItemOption) çözümleyiciye bağlandı; tooltip
  yükleyicileri `SPK_AssetExists` ile ölümcül olmayan atlama yapar.
- H-011: `Sprite.cpp` `CSprite::Create` boş doku guard'ı (log + `m_nTexID = -1`).
- H-014: `CBInterfaceVIPChar` PC dalında `VIPCharRank.txt` yoksa log + `return`
  (dosya canlıda da yok — fatal diyalog kaldırıldı).
- H-015 (yeni keşif, run13): canlı Engine string kanıtıyla (`Map\World%d`,
  `Data\Map\Object*`) harita/nesne yol paritesi — WorldName üretimi 4 dosyada
  düzeltildi (MapManager/GMBattleCastle/GMCrywolf1st/GM_Kanturu_3rd);
  `AccessModel`/`OpenTexture`/`LoadBitmap` merkezi dönüşümleri; Minimap SPK-first;
  deploy betiğine `Map\World74`+`Map\Object74` alt kümesi eklendi.
- Ek: `Source\5.Main\source\Utilities\Log\` (ErrorReport/muConsoleDebug/
  WindowsConsole) vcxproj'da listeliydi ama git'te izlenmiyordu — derleme
  bütünlüğü için commit'e alındı.

**Neden** — H-012/2e.4: istemciyi dağıtım betiğindeki yol eşlemesine bağımlı
olmadan canlı SPK paket düzeniyle çalıştırmak; run13'te keşfedilen harita yolu
uydurumsuzluğu (H-015) giriş sahnesini ölümcül diyalogla kesiyordu.

**Doğrulama** — run11→run17 (`BuildLog\2e4\{results,shots,evidence}`): run14
derlemesi `b386bd65c4fbdc1aec840adfcfb8af8a` (12.029.952 B) saf SPK-first pakette
(`Data\Local` yok) **diyalogsuz** çalıştı; pencere 0,9 s; t=5,2 s canlı sunucuya
**SynSent** (`45.87.120.29:44405`); kalıntı süreç yok. Düzenleme araçlarının
kaynakta U+FFFD'ye çevirdiği 26 EUC-KR satırı `git show HEAD` baytlarıyla geri
konuldu (`BuildLog\2e4\fix_encoding.js`), yeniden derlendi (md5
`6dc52f5fb63746cb51575ab1370f7e6c`) ve run15–17 ile doğrulandı: run17
diyalogsuz/çökmesiz, t=21,9 s `45.87.120.29:44405` **SynSent**. Kalan: içerik/şema
eşlemesi (docs/24 §8, docs/03 B-08). Rapor: docs/24.

**Commit** — `8daa52f38` (2e.4 SPK-first varlık çözümleyici + harita/nesne yol paritesi).

---

## [26.10.02 19:50] 2e.3 Sunucu test klasörü — bizim GS/CS/DS/JS çıktıları + DB restore + smoke test

**Ne yapıldı**
- `BuildLog\2e3\deploy_server_test.sh`: repo `MuServer\` ağacındaki bizim
  `Release_EX603|Win32` çıktılarını (CS `4c1f91bf…`, DS `916e9625…`, JS `91ee93d7…`,
  GS `43f086d9…`) `BuildLog\2e3\deploy\` altına canlı düzenini aynalayarak kurar
  (25 MB; MSVCP140+VCRUNTIME140 kapanışı; pdb/dmp/LOG/msvcp100 hariç); `--rebuild`
  bayrağı dört projeyi MSBuild ile yeniden derler.
- `restore_test_db.sh`: `ServerTools\MuServer_S6_2020\DB\DB_SQL_12.bak` → yerel
  SQLEXPRESS `MuOnlineS6` (ONLINE, 55 tablo; idempotent — varsa üzerine yazmaz) +
  DSN kontrolü.
- Smoke test: `Start_TestServer.bat` ile CS→DS→JS→GS başlatıldı; dört süreç ayakta,
  portlar 63000/63001udp/63002/63003, GS→DS/JS bağlantıları kuruldu (`LOG_CONNECT`);
  `Stop_TestServer.bat` ile kapatıldı, kalıntı süreç yok.

**Neden** — Faz 2e'nin sunucu ayağı: parite ancak bizim yığın test klasöründen
ayağa kalkınca doğrulanabilir; canlı ağaca hiç dokunulmadı.

**Doğrulama** — `BuildLog\2e3\results\smoke1.json` (yol+pid+port),
`GameServer_LOG_smoke1.txt` (201 satır, çökme yok),
`GameServer_LOG_CONNECT_smoke1.txt`. DB: `sys.databases` durum + tablo sayısı.
Rapor: docs/23.

**Commit** — `eea0b584a` (2e.3 sunucu test klasörü).

---

## [26.10.02 19:40] 2e.1 bağımsız doğrulama — PC hattı teyit edildi; mobil kalıntılar belgelendi

**Ne yapıldı** — 2e.1 "eksiksiz mi" sorusu üzerine üçlü doğrulama:
(1) kaynak `Source\5.Main` → Winmain/GameConfigConstants/SceneCore `45.87.120.29` + 44405,
`171.235.182.88` yok; (2) `ClientFile\Main.exe` (md5 `71b008ad…`) string taraması →
`45.87.120.29` var; `171.235.182.88`, nvapi, apicb, freeimage yok; (3) canlı
`ConnectIP.bmd` (md5 `8ac74a5b…`) 0x20/0x22 = 44405/55858; `MainLoad` bunları çalışma
anında uyguluyor. İstisna: `android_main.cpp` (3 kod) + `LauncherHelper.h` (örnek yorum)
kalıntıları **mobil** fallback değerleri (`171.235.182.88:63000`); hiçbir vcxproj'da kayıtlı
değiller, PC exe'sine girmiyor → kod değiştirilmedi; H-004 ve docs/22'ye kapsam notu işlendi.

**Neden** — H-004'teki "kaynakta 0" ifadesi tüm ağaç için fazla iddialıydı; parite kapsamının
derlenen PC hattı olduğu netleştirildi.

**Commit** — `a6b96cfc3` (2e.1 bağımsız doğrulama).

---

## [26.10.02 19:30] 2e.2 ek kabul koşusu — run10 tam varlık seti + docs/22 güncellemesi

**Ne yapıldı**
- `deploy_spk_package.sh` `--with-assets` kapsamı genişletildi: Map/Sound/Music hariç tam görsel
  set (`Interface Logo Player Monster NPC Item Skill Effect InGameShopBanner`, ~380 MB);
  eski "asgari set" notu kaldırıldı.
- Paket **10. kez** koşuldu (`run10-full-assets`): istemci penceresi `Axion Mu` (t≈1,0 s);
  **çökme yok**; tek engel t≈4,9 s diyaloğu `Data\Local\Eng\itemtooltip_Eng.bmd - File not
  exist.` — SPK paketinde bu adla karşılık yok (tooltip içeriği `Config\Text.bmd` /
  `ToolTipText.txt` / `MasterSkillTooltip.bmd`). Yol eşlemesi bitti; kalan boşluk
  **içerik/şema eşlemesi** → 2e.4.
- docs/22: §3.6 tabloya run10 satırı, run10 Frontier paragrafı, §4'e içerik/şema maddesi eklendi.
- docs/04 tutarlılık düzeltmesi: negatif-kontrol dersi (pencere+TCP tek başına içerik kanıtı
  değil) eksik kayıttı → **H-013** olarak çözülen arşive işlendi; docs/21'deki yanlış `H-012`
  atfı `H-013` yapıldı.

**Kanıt** — `BuildLog\2e2\results\run10-full-assets.json`;
`BuildLog\2e2\shots\run10-full-assets-*.png` (3); istemcinin kendi günlüğü
`BuildLog\2e2\evidence\KEN_run10.txt`. Paket md5'leri: ConnectIP `8ac74a5b…` (canlıyla aynı),
ServerData `e3617db7…` (2d.2 üretimi), SPK.ini `763ee112…`.

**Commit** — `1674fa006` (run10 ek kabul turu).

---

## [26.10.02 19:20] docs/CHANGELOG hash takibi (c90258976) + pano

**Ne yapıldı** — 2e.1+2e.2 kaydının Commit satırı `c90258976` ile kapatıldı (2d.2 raporu aynı commit).

**Commit** — bu kayıt.

---

## [26.10.02 19:10] 2e.1 + 2e.2 Gömülü IP hizalama ve derleme çıktısının SPK paketine kurulması

**Ne yapıldı**
- **2e.1:** `Winmain.cpp` gömülü `171.235.182.88` → **`45.87.120.29` + port 44405** (canlı
  `ConnectIP.bmd` ilk 12 baytı ve `GetEngine.ini IpAddressPort=44405` kanıtı); aynı değerler
  `GameConfigConstants.h` ve `SceneCore.cpp` (derlenmeyen dosya, not düşüldü). Çalışma anında
  ConnectIP 0x20/0x22 (IpAddressPort/AntiPort) SPK verisinden uygulanır.
- **2e.2 paket:** `BuildLog\2e2\deploy_spk_package.sh` — `Main.exe` + `SPK.ini` +
  `wzAudio.dll`+`ogg.dll`+`vorbisfile.dll` + `Data\SPK\{ConnectIP,ServerData}.bmd` (2d.2
  üretimi) + `Data\SPK\Config` + MUIG→SPK yol eşlemesi. Canlı kökte **olmayan**
  `APICB.dll`/`FreeImage.dll` importları kaldırıldı (`CB_ANTIHACKGGNEW=0` koşullu no-op;
  `CB_USE_FREEIMAGE=0` → GDI+). Final `Main.exe` **12.026.880 B**,
  md5 `71b008ad6d1d8e9e08c546859401dc92`; importlarında apicb/freeimage **yok**.
- **Bulunan/çözülen dağıtım hataları:** `GetGPUUse=1` nvapi.dll MessageBox'ı (GPU'suz makinede
  kilitlenme) → 0; `vorbisfile.dll`+`ogg.dll` paket eksikliği (yükleyici seviyesinde
  **csrss** sistem hatası, süreç penceresi yok) → DLL kapanışı pakete eklendi;
  `Data\Local\Mix.bmd` ölümcül kontrolü → `MixMgr` SPK-first (`Data\SPK\Config\Mix.bmd`,
  bayt formatı 14*4+134*656=87.960 B ile doğrulandı); `LoadEncDec()` SPK birleşiminden
  sonraya alındı; GS port aralığı + FPS SPK'dan.
- **Çalıştırma kanıtı:** istemci SPK paket düzeninde **kendi penceresini açtı** (`Axion Mu`,
  0,8 s; `BuildLog\2e2\shots\run9-with-player-win-0x340104.png`), kendi günlüklerini yazdı
  (`KEN.txt`, `STACK_ERROR\stack_20261002_1846.log` → çağrı zinciri `WinMain → MainLoop →
  Scene → WebzenScene → CreateTitleSceneUI → CSprite::Create`). Eksik `Data\Interface`
  bitmap'leri çökme nedeniydi (H-011).

**Neden** — Faz 2e'nin amacı: derleme çıktısının canlı pakete kurulup **çalıştığının**
kanıtlanması; parite farkları ancak böyle görünür oluyor.

**Doğrulama** — `Global Release|Win32` 3 kez derlendi (0 hata; LNK4099 dışında uyarı yok).
İmport tablosu PE tarayıcıyla (`BuildLog\2e2\pe_imports.js`) doğrulandı. DLL kapanışı
`dep_closure.js` ile çıkarıldı (paket zinciri: wzAudio→ogg→vorbisfile). Paket 9 kez fiilen
çalıştırıldı; sonuçlar `BuildLog\2e2\results\run*.json`.

**Bekleyen** — tam `Data` ağacı kopyası (disk %100 → ~1,6 GB gerekli), **SPK-first varlık
çözümleme katmanı** (2e.4; audit `BuildLog\2e2\asset_audit.txt`, 12 tablo), `CSprite::Create`
dayanıklılığı (H-011), `SPK_CRCFILE.ini SPK_MEXE`'nin Main.exe için yeniden üretimi.
Rapor: docs/22.

**Değişen dosyalar** — `Source\5.Main\source\{SPKData.h,SPKData.cpp,MainLoad.cpp,APICB.h,
APICB.cpp,Defined_Global.h,stdafx.h,CB_AutoNapGame.cpp,Winmain.cpp,MixMgr.h,MixMgr.cpp,
GameConfig\GameConfigConstants.h,Scenes\SceneCore.cpp}`; `ClientFile\Main.exe`;
`BuildLog\2e2\*`; `docs\{22,02,03,04,CHANGELOG}`.

**Commit** — `c90258976` (2d.2 raporu + 2e.1/2e.2 uygulaması **tek commit**).

---

## [26.10.02 19:05] 2d.2 Üretilen SPK verisinin canlı istemciyle kabul testi

**Ne yapıldı** — 2d.1 üretimi (`ConnectIP.bmd` `8ac74a5b…`, `ServerData.bmd` `e3617db7…`)
canlı `Engine.exe`'ye 4 koşuda verildi (kontrol, bizim üretim, bozuk negatif kontrol);
süreç ağacı + pencere + TCP + PrintWindow kanıtı toplandı, orijinal dosyalar geri konuldu
ve md5 ile doğrulandı. Rapor: [docs/21](21-2D2-KABUL-TESTI.md).

**Neden** — 2d.1'in "üretti" iddiasının canlı istemci tarafından tüketildiğinin kanıtı;
2e.2 paket testinin önkoşulu.

**Doğrulama** — run2 (bizim üretim): pencere `Axion Mu` 0,8 s'de açıldı; **t≈12 s
`45.87.120.29:44405` SynSent** (`BuildLog\2d2\results\run2-ours-accept.json`); run3
(bozuk ServerData) negatif kontrol dersi: pencere+TCP tek başına içerik kanıtı değil →
birincil kanıt bayt karşılaştırmasıdır (docs/20).

**Değişen dosyalar** — `docs\{21,02,03,04,CHANGELOG}` (yalnız doküman; kod değişmedi).

**Commit** — `c90258976` (2e kaydıyla birlikte).

---

## [26.10.02 14:05] 2d.1 SPK GetMainInfo ilk dalga (D1-D3, D6, D7) — canlı araç ölçümüyle

**Ne yapıldı**
- **Canlı üretici ölçüldü:** `Client and Tools\GetMain\GetMainInfo.exe` birebir dizin
  yapısıyla `BuildLog\2d1\sandbox\` içinde koşturuldu; sentinel ini/girdi varyantlarıyla
  **ini→çıktı bayt haritası** çıkarıldı (docs/20 §2-3). Araç tam jeneratör + deterministik.
- `Source\6.GetMainInfo\GetMainInfo\SPK\` yazıldı: `GetEngineConfig` (D1),
  `ConnectIPWriter` (D2), `ServerDataWriter` (D3: şablon + ini yaması + 0x554 CRC),
  `CrcFileReport` (D6), `SpkUtil`, `main_spk` (D7 CLI + `--check`).
  `GetMainInfo.cpp`: **SPK modu varsayılan**, eski MUIG akışı `--mode:muig` ile korunur.
  vcxproj/.filters: SPK dosyaları + `$(ProjectDir)` include.
- **Format düzeltmeleri:** ConnectIP'te **CRC YOK** — son 4 bayt `IpAddressPort + AntiPort`
  (canlı 44405/55858; docs/07 §1.3/§6-1 kapandı). ServerData `0x4F9–0x554` ini alan
  haritası kesinleşti (MENU_BUTTON 0x4F9..0x50C, ButtonShop* 0x50D..0x513+0x515,
  Ranking 0x516.., Level/MaxGameInstances 0x52B..0x52F, ReconnectTime 0x54C,
  0x554 = ClientName-dosyası CRC32'si, 0x558/0x55C float ham).
- **Anahtar tuzağı:** canlı ini `ButtonShopChaos` (Jw öneki YOK); ilk derlemede yanlış
  anahtar okunmuştu — round A karşılaştırmasında 0x510'daki tek bayt farkıyla yakalanıp düzeltildi.

**Neden** — 2a.5 kararı (tek hat = SPK GetEngine) uygulaması; 2d.2 istemci E2E'sinin önkoşulu.

**Doğrulama** — `Release|Win32` v143 LTCG **0 hata** → `GetMain\GetMainInfo.exe`
**3.723.776 B**, md5 `c480e0baf79cd1bf32c8bcfc79c1aed1`. Canlı araçla **3 senaryo**
karşılaştırması (şablon = canlı istemcinin ESKİ dosyası — en güçlü yama testi):
baseline ConnectIP `8ac74a5b…`/ServerData `e3617db7…`; round A (sayısal sentinel)
`193b21f3…`/`3cf20916…`; round B (metin sentinel) `c2da1433…`/`07e889c5…` — **hepsi
bayt-birebir**. `SPK_CRCFILE.ini` raporları zaman satırı dışında birebir; `--check`
14/14 OK. İzole kanat deneyi: kayıt tabanı 0x6B8, stride 0x154, +0x00 IIndex, +0x54 name.

**Bekleyen** — **D4/D5** (kanat/item/LEVEL katalogları → şablonsuz tam jeneratör),
RenderEffect.bmd üretimi, Engine.exe kabul testi (2d.2).

**Değişen dosyalar** — `Source\6.GetMainInfo\GetMainInfo\{SPK\*.{h,cpp}, GetMainInfo.cpp,
GetMainInfo.vcxproj, GetMainInfo.vcxproj.filters}`; `GetMain\GetMainInfo.exe`;
`docs\{20-2D1-SPK-GETMAININFO.md, 07, 08, 02, 03, CHANGELOG}`; kanıt `BuildLog\2d1\*`.

**Commit** — atılmadı.

---

## [26.10.02 13:35] 2d.0 SPK istemci format katmanı (5.Main) — Main.exe'ye SPK veri hattı

**Ne yapıldı**
- `Source\5.Main\source\SPKData.{h,cpp}` (yeni): `CSPKData` + global `gSPKData` — ConnectIP.bmd (36 B,
  XOR 0x20, 12 B IP), ServerData.bmd (1.089.576 B tam boyut şart, XOR 0x20; 4 sunucu adı +
  ClientName/CustomerName/WindowName/ScreenShotPath/ClientVersion/ClientSerial + 7 sınıf hız
  limiti + kamera/FPS), SPK.ini (MainCode/FontName/FontHeight/Resolution/Lang/BODY_X20),
  `Data\SPK` varlık kanıtı + `GetPath`/`GetConfigPath` yardımcıları.
- `MainLoad::Load()`: `LoadEncDec()` sonrası / `CheckPluginFile()` öncesi SPK bloğu — dosyalar
  yok/bozuksa MUIG (`CBGetMain.bin`) hattı aynen korunur; IP her zaman override, Version/Serial
  koşulsuz, kimlik/hız alanları doluysa yazılır.
- **Ofset düzeltmesi (canlı dosya hex dökümü):** hız bloğu `0x52C` → **`0x530`** (7×int32),
  kamera `~0x55A` → **`0x558`**, FPS **`0x55C`** (float 240.0 = 24.0×10). docs/07
  "Rev. 02.10.2026" + §4/§5 sözde kod güncellendi.
- `Main.vcxproj` + `.filters` girdileri (MU\Client\BCustomLoad filtresi).

**Neden** — docs/02 2d.0 (2a.4'te açıldı): istemci, SPK sunucuyla aynı veri hattını
konuşmadan 2d.1 üreticisi ve parite testi mümkün değil; bu katman GetMainInfo üreticisinin
(docs/08) ters yönü.

**Doğrulama** — istemci `Global Release` v143 Win32 **0 hata** (yalnız LNK4099 PDB uyarıları):
Main.exe **12.027.904 B**, md5 `39f2af32f35114241c7feb586cdd92ad` (13:30); `SPKData.obj`
disasm'ında `530h/558h/55Ch` (eski `52Ch/55Ah` **yok**); exe'de `ConnectIP.bmd`, `ServerData.bmd`,
`BODY_X20`, `MainCode` literal'leri gömülü. Standalone koşucu
(`BuildLog\5Main\spktest\build_test.bat`; gerçek `SPKData.cpp`'nin sed kopyası) canlı dosyalarla:
`Load=true`, dört sunucu adı, `45.87.120.29`, `1.03.34` / `!571Axion@Mobile`, 7×65000,
kamera 45.00 / FPS 24.00, ini alanları doğru; olmayan dizin: `Load=false`, EXIT=1.
Test kopyaları canlıyla md5-birebir (`8ac74a5b…`, `53982bf8…`, `763ee112…`).
Ek kanıt: **gerçek proje obj'si** (`BuildLog\5Main\SPKData.obj`) konsol koşucusuyla link
edilip koşuldu (`build_realobj_test.bat`; LIBPATH + `/FORCE:UNRESOLVED` — yalnız
`g_render_lock`/`g_protocol_lock` /include zorlaması ve LNK2011 PCH uyarısı) → aynı
sonuçlar (EXIT 0/1); “gerçek obj link edilemiyor” kısıtı kapandı.
**Engine.exe kabul testi 2d.2'ye bırakıldı** (ConnectIP CRC alanı hâlâ açık kalem, docs/07 §6/1).

**Değişen dosyalar** — `Source\5.Main\source\{SPKData.cpp, SPKData.h, MainLoad.cpp}`;
`Source\5.Main\{Main.vcxproj, Main.vcxproj.filters}`; `ClientFile\Main.exe`;
`docs\{19-SPK-ISTEMCI-FORMAT-KATMANI-2D0.md, 07-SPK-BMD-FORMAT.md, 02-YOL-HARITASI.md,
03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md, CHANGELOG.md}`;
`BuildLog\5Main\{spktest\*, spkdata_obj_disasm.txt}`.

**Commit** — atılmadı.

---

## [26.10.02 12:55] 2c.2-A3 SPK_CustomItemSetPro (canlı kanıttan sıfırdan) — modül dalgası ilk kalem

**Ne yapıldı** (kullanıcı talimatı: "2c.2..N — Modül modül: iskelet yaz → GS derle → config dosyasını üret")
- Yeni modül: `Source\4.GameServer\GameServer\SPK\CustomItemSetPro.{cpp,h}` (canlı obje adı), sınıf
  `CustomSetDameItem` + `gCustomSetDameItem` + `Instance()`; iki `std::vector<ConfigSetDataDamage>`
  (canlı this+0 `Item` / this+0Ch `ItemSet`, eleman 0x58 = 22 int).
- Canlı yüzey: `Load` @0x476640 (pugixml; `Item[]` 22 alan + `ItemSet[]` 21 alan — Section okunmaz),
  `Save` @0x477360 (ItemData; Item 22 / ItemSet 21 attribute), `CalcCharacter` @0x478BF0 (flag != 0 →
  erken dönüş; Item kendi Section'ıyla, ItemSet satırı kopyalanıp Section=7..11 ile 5 kez uygulanır),
  lambda `CalcSlot` @0x478C90 (wear slot 2..11 = canlı 0x1F8..0xAD4 / adım 0xFC; koşullar
  `m_Index == Section*512+Index`, `m_Level >= Lv`, `m_NewOption >= Opt` (canlı byte +9F),
  `m_SetOption == SetOption` (canlı byte +0B6); Dmg altı hasar alanına + AddShield/AddLife/AddMana/
  Defense + dokuz rate + DamageReflect).
- Kancalar: `ServerInfo::ReadCustomInfo` Load (canlı 0x569B78/0x569BAE sırası) ve
  `CObjectManager::CharacterCalcAttribute` içinde `CalcMasterSkillTreeOption` sonrası
  `CalcCharacter(lpObj, 0)` (canlı 0x5424D5 sırası). SPK GUI editörü (SPK_CustomItemSetProProc @0x4796A0)
  kapsam dışı (A1/A2 kararı).
- Config: `MuServer\4.GameServer\Sub 1\Data\SPK\CustomItemSetPro.xml` canlıdan **bayt-birebir**
  (50858 B, md5 `9b2d6ef99036b063b105ada65f2296e8`; 92 Item + 84 ItemSet).

**Neden** — 2c.1-A dalgasının üçüncü kalemi; modül canlıda hem yükleyici zincirinde hem karakter
 yeniden hesaplama zincirinde aktif.

**Doğrulama** — derleme `Release_EX603` v143 Win32 **0 hata**: exe 10.819.072 → **10.824.704 B**
(md5 `43f086d939575ac34eb373f7d527b8e9`, 12:45); bizim disasm'da `?Load@CustomSetDameItem` @0x465F00
(+ 0x525BA0 çağrısı), `?CalcSlot@CustomSetDameItem` @0x4FBDF0 (+ 0x4FBE14/0x4FBEE3 çağrıları),
`gCustomSetDameItem` başlatıcı/atexit kayıtları; exe içinde `SPK\CustomItemSetPro` yolu; config md5
kaynak = hedef. **E2E yapılmadı** (stat uygulama testi sıradaki tur).

**Değişen dosyalar** — `Source\4.GameServer\GameServer\{SPK\CustomItemSetPro.cpp, SPK\CustomItemSetPro.h,
GameServer.vcxproj, ServerInfo.cpp, ObjectManager.cpp}`; `MuServer\4.GameServer\Sub 1\Data\SPK\CustomItemSetPro.xml`;
`MuServer\4.GameServer\Sub 1\GameServer\{GameServer.exe, GameServer.pdb}`; `docs\18-SPK-CUSTOMITEMSETPRO-A3.md`;
`docs\{13,05,CHANGELOG}`; `Dashboard\data\{sonuc,sohbet}.json`; kanıt arşivi `BuildLog\2c1\A3_*.txt`.

**Commit** — atılmadı.

---

## [26.10.02 11:56] CB_ActiveInvasions E2E doğrulaması (2c.1-B3 kapanış) + 4 parite farkı

**Ne yapıldı** (kullanıcı isteği: "CB_ActiveInvasions çağrı zincirlerini bir test kurulumunda
uçtan uca doğrula: giriş push'u, F7 sub 0x02 isteği, spawn/ölüm sayaç akışı")
- Test kurulumu açıldı (§5.1 sırası), `Release_EX603` + geçici `CB_ActiveInvasions_TEST` define,
  bayrak dosyası tetikli sürücü (`..\Data\CB_ActiveInvasions_selftest.flag`), paket yakalama
  kancası `Util.cpp` `DataSend`/`DataSendAll` başında.
- Sentetik istemci slotu **sunucunun kendi yaşam döngüsüyle** kuruldu: `gObjAdd` →
  `gObjAllocData` (slota özel OBJECTSTRUCT + VpPlayer/Skill/Inventory + PShopTrade CS).
- Doğrulanan 9 kalem (hepsi ✅, paketler **bayt-birebir**): F7 sub 0x02 → `ProtocolCore` →
  `send_list_to_client(aIndex)`; giriş push'u (`CacheSendOnlogin` dalı) + gate (2. tik paket
  üretmez); `SetState(START)` → `SetMonster` → `monster_add` (class 43 için 8, 20 sınıf → 108
  obje); `MonsterDieProc` → `monster_del(true)` → `C1 10 F3 99 {43,7,0}`; `SetState(EMPTY)` →
  `ClearMonster` (108 obje temiz, 20 sınıf kaydı erase); sayaç semantiği (çift artış/azalış,
  `count==0` erase, yeniden (1,1)); negatif yol (99999 → paket yok).
- **4 parite farkı bulundu** (canlı `live_disasm.txt` + `GameServer_canli.map` çağrı sayımı):
  (1) void `send_list_to_client()` canlıda YOK — bizde 3 fazla çağrı (DSProtocol 1660,
  InvasionManager 527/615); (2) giriş push'u canlı `gObjSecondProc`'ta YOK (User.cpp 3484 fazla;
  canlı tekil listenin tek çağrı noktası ProtocolCore 0x54FA38 = pull tasarım);
  (3) `SendThongTinSauKhiVaoGame` canlıda hiç çağrılmıyor; (4) canlı
  `CObjectManager::ObjectSetStateProc` respawn yolundaki `monster_add(class,true)` (0x538687)
  **bizde eksik**.
- Sürücü kaldırıldı, temiz rebuild (10.819.072 B, md5 `d03a0a919bd2cb25f9589e044a40f2b5`),
  temiz açılış 11:55:20, ardından **yığın 11:56:22'de kapatıldı** (kural §5.1).

**Neden** — 2c.1-B3 modülünün çağrı zincirleri canlı parite iddiasıyla yazıldı; çalıştırma
kanıtı ve canlı-binary çağrı noktası denetimi istendi. Ayrıca sürücü koşularında 2 hata
bulunup düzeltildi (yalnız sürücü kaynaklı; üretim kodu etkilenmedi): `ProtocolCore`'a yanlış
`head` (0xC1 → FriendAddRequest yolu) ve `OBJECTSTRUCT_HEADER`'ın paylaşılan `CommonStruct`'ına
ham `memset` (→ `gObjClearViewport` AV, dump RVA 0x1526F6).

**Doğrulama** — `LOG\2026-10-02.txt` satır **6075-6121** (30 cb paketi, çökme yok); başarısız
koşular 5739-5806 ve 5898-5966 (dump'ları arşivde); exe'de `AITEST` stringi = 0, bayrak
dosyası temiz build'de tüketilmiyor; `tasklist` temiz. Detay: `docs\17-CB-ACTIVEINVAISIONS-E2E.md`.

**Değişen dosyalar** — test turu: `Source\4.GameServer\GameServer\{CB_ActiveInvasions.cpp,
CB_ActiveInvasions.h,User.cpp,Util.cpp,SPK\EventMainManager.cpp,stdafx.h}` (tamamı geri alındı),
`BuildLog\2c1\CB_ActiveInvasions_E2E_{driver_snapshot.cpp,hooks.patch,log.txt}`,
`BuildLog\4GS\bizim_disasm_TEST.txt`, `docs\17-CB-ACTIVEINVAISIONS-E2E.md`, `docs\13`,
`docs\CHANGELOG.md`, `Dashboard\data\{sonuc,sohbet}.json`.

**Commit** — atılmadı.

---

## [26.10.02 10:23] KURAL: test bitince sunucu yığını kapatılır (performans) + yığın kapatıldı

**Ne yapıldı** (kullanıcı isteği: "testler bitince açık olan gameserver'ları kapat …
bunu kural olarak yaz")
- `docs\00-PROJE-HARITASI.md` §5 **Kritik kurallar** bloğuna kalıcı kural eklendi:
  test/doğrulama işi biter bitmez `GameServer.exe` + `DataServer.exe` + `JoinServer.exe` +
  `ConnectServer.exe` (varsa `AntiServer.exe`) **kapatılır** — açık bırakılmaz. Gerekçe:
  sistem performansı + tur hızı (sunucu yapım aşamasında, oyuncu yok). İhtiyaç anında
  yeniden açılır; kural ajanın varsayılan davranışıdır.
- Yeni **§5.1 Sunucu yığınını açma / kapatma** bölümü: 4 `Start-Process` komutu (kendi
  working directory'leriyle) + `taskkill //IM … //F` döngüsü (kopyala-çalıştır).
- Kural bu turda uygulandı: A2 E2E sonrası açık olan yığın (GameServer PID 9152,
  DataServer 10200, JoinServer 6400, ConnectServer 3640) **10:20:29'da kapatıldı**.

**Neden** — kullanıcı talebi; üretim/tur hızı için boşta duran sunucu süreçleri kapatılır.

**Doğrulama** — `tasklist | grep -iE "GameServer|DataServer|JoinServer|ConnectServer"` → **boş**
(ilgili süreç kalmadı); sunucu logu son satırları olağan, yeni `.dmp` yok (temiz kill).

**Değişen dosyalar** — `docs\00-PROJE-HARITASI.md`, `docs\CHANGELOG.md`,
`Dashboard\data\{sonuc,sohbet}.json`.

**Commit** atılmadı (kullanıcı istemedi).

---

## [26.10.02 09:58] 2c.1-A2 — SPK_MonsterSkill canlı kanıttan uygulandı (singleton+vector, SPK\ yolu, canlı 16 satır veri) + E2E

**Ne yapıldı** (kullanıcı isteği: "2c.1-A dalgasındaki A2 MonsterSkill kalemini canlı kanıtla uygulamaya al.")
- Modül canlı `SPK_MonsterSkill.obj` şekline çekildi: dosya adı/konumu
  `SPK\SPK_MonsterSkill.{cpp,h}` (canlı obje adı), `static CCustomMonsterSkill* Instance()`
  (canlı Meyers singleton @004A4D00, guard 0213C8D4h/nesne 0213C8D8h; A1 deseniyle global
  döner), depolama `std::vector<CUSTOM_MONSTER_SKILL>` (canlı 3-pointer deseni
  0213C8D8/DC/E0, eleman 12 B, `add eax,0Ch`) + yüklemede `vector::clear()` (004A4E90-004A4E9A).
  Eski sabit `m_Monster_Skill[1000]` + `m_count` yapısı (taşma koruması yoktu, bayat kayıt
  kalıyordu) kaldırıldı — canlıda üst sınır yok.
- `Load` akışı canlı `004A4D70` ile birebir: `try/catch`, `TOKEN_END`, `GetNumber() != 0 →
  satır atla` (004A4EDC), iç döngü `strcmp("end", GetAsString())` (004A4EE8),
  `GetNumber()` + 2×`GetAsNumber()` (004A4F2D/3F/51), `push_back` (004A4F5F-004A4F85);
  hata yolu `ErrorMessageBox(GetLastError())` = `MEM_SCRIPT_ERROR_CODE4`.
- **Yol paritesi:** `Custom\CustomMonsterSkill.txt` → `SPK\CustomMonsterSkill.txt`
  (canlı yükleyici 0x569791/0x5697C7, yol literali 0x63DCB4). Yeni dosya
  `MuServer\4.GameServer\Sub 1\Data\SPK\CustomMonsterSkill.txt` canlı dosya ile
  **byte-birebir** (206 B / 16 satır / CRLF / tab); eski `Data\Custom\CustomMonsterSkill.txt`
  silindi (canlıda böyle bir dosya yok).
- **Canlı log stringi düzeltildi:** `[SPK] CustomMonsterSkill configuration saved and
  reloaded.` (exe 2344740 — nokta dahil). `Reload()` hata tespiti `m_count == 0`
  sezgisinden `m_LoadResult` alanına çevrildi.
- **Tüketiciler zaten pariteydi, kanıtla doğrulandı:** `gObjSetMonster` @0x51FD62
  (`if(lpObj->AttackType != 0)` içinde; 0051EC7D → 0051FE59 kapısı) ve `gObjMonsterAttack`
  @0x521571 (Class zincirinin sonunda 561/0x231 ile aynı hedef 005215DB →
  `PMSG_DURATION_SKILL_ATTACK_RECV`). Canlı disasm'da custom blokta görünen
  `CheckSkillRequire*` zincirinin `CSkillManager::AddSkill` (SkillManager.cpp:798) LTCG
  inline'ı olduğu tespit edildi (yalnız `OBJECT_USER` için gate'li; monsterda ölü dal) →
  iki tüketici noktasına yalnız kanıt yorumu eklendi.

**Neden** — 2c.1-A dalgasının "sıfırdan" kalemi; canlı SPK sunucusuyla hem davranış hem
veri paritesi hedefi (docs/12 §4: canlı kanıt donor'un önünde).

**Doğrulama**
- Derleme (Release_EX603/Win32/v143): 0 hata; exe 10.817.536 → **10.819.072 B** (+1.536).
- E2E (geçici sürücü, tetik `Data\SPK\MonsterSkill_selftest.flag`): `kayit=16`; loader yolu
  `..\Data\SPK\CustomMonsterSkill.txt`; `GetSkillMonster` satırları OK (750/704/706/561/754/722);
  mükerrer 719 → ilk kayıt (232); `GetSkillMonster(99999)=0`; `gObjSetMonster(704)` →
  `GetSkill(42)=1 GetSkill(264)=1`, `(706)` → `GetSkill(38)=1`, kontrol `(700)` → custom skill yok;
  `gObjMonsterAttack DURATION dal secildi: class=704 custom=1`; `Reload` canlı stringi yazdı.
  Log `LOG\2026-10-02.txt` 4523-4543 (09:48:51). Sürücü kaldırıldı; temiz derleme açılışı
  09:52:27 `[ServerInfo] Custom loaded successfully`, `MSTEST` satırı yok, yeni `.dmp` yok.
- Sürücü arşivi: `BuildLog\2c1\A2_SPK_MonsterSkill_E2E_driver_snapshot.cpp` +
  `A2_E2E_driver.patch` (158 satır) + `A2_E2E_log.txt`.
- Bizim yeni disasm: `BuildLog\envanter\bizim_disasm3.txt` — `?GetSkillMonster@CCustomMonsterSkill@@`
  @0x00465B30 (vector taraması, `2AAAAAABh` = /12, `add eax,0Ch`), `gObjSetMonster` custom bloğu
  @0x004DCE2D+ (begin/end + `add ebx,0Ch`) — canlı desenle aynı şekil.
- Exe stringleri: `SPK\CustomMonsterSkill.txt` @1498696, canlı log stringi @1477040;
  `Custom\CustomMonsterSkill.txt` artık yok.
- Veri dosyası: `cmp` canlı ile **byte-birebir**.

- Not (şeffaflık): `Monster.cpp` düzenlenirken dosyada **önceden var olan** geçersiz UTF-8
  baytları (donor yorumlarındaki tek `\xEE` / `\xE0`) editör tarafından U+FFFD'ye normalize
  edildi — 3 yorum satırı (ikisi zaten bu turda düzenlenen satırlar), davranış/derleme
  etkilenmedi.

**Rapor** — `docs\16-SPK-MONSTERSKILL-A2.md` (canlı kanıt adresleri, tuzaklar, backlog).
**Commit** atılmadı (kullanıcı istemedi).

---

## [26.10.02 08:45] EventGvG E2E doğrulama turu — 4 açılış engeli düzeltildi, 6 kalem ✅

**Ne yapıldı** (kullanıcı isteği: "EventGvG'yi test sunucusunda uçtan uca doğrula:
Switch=1, zaman tablosu, NPC dialog girişi, STAND→START geçişi, UserDieProc puanlama
ve CalcRank kazanan akışı")
- Sunucu bu turdan önce **hiç açılmıyordu** (modal Error kutuları / AV). Dört gerçek
  hata canlı kanıtla bulunup düzeltildi:
  1) `CustomJewel.cpp` `.txt` yolu: `CUSTOM_JEWEL_INFO` içindeki `std::map` tam-struct
     `memset` ile bozuluyordu → `SetInfo` değerle kopyalarken map kopya kurucusunda AV.
     Kanıt: minidump `2026-10-2_7h54m44s.dmp` → `0xC0000005 @ RVA 0x63D7E` =
     `std::_Tree::_Copy`. Düzeltme: yalnız POD alt alanlar sıfırlanıyor.
  2) `.txt` şeması canlıdan 1 alan fazlaydı (`EnableSlotRing`). Canlı kanıt: canlı
     `CUSTOM_JEWEL_INFO` = 212 B (`add eax,0D4h`), 15 eleman (`cmp ecx,0Eh`),
     `ModelName` ofset `0x54`/boyut `0x40` → canlıda ring alanı YOK. Bizim satırlarda
     12 joker okunacak; `.txt` yolunda `EnableSlotRing=-1` (kanal kapalı = canlı davranışı).
  3) `.txt` okuyucusunda **bölüm 3** eksikti → dosya sonundan sonra token zaman aşımı
     ("The file were not configured correctly"). Canlı `0047AA60 cmp eax,3` + satırı
     4 sayı okuyup `std::vector<CUSTOM_JEWEL_UPGRADE_INFO>` (16 B) global'e ekliyor.
     Düzeltme: `CUSTOM_JEWEL_UPDATE_INFO`'ya `Type` alanı + `section == 3` dalı.
  4) (B1 kalemi) SkyEvent `Monster.ini` satır şeması 5 kolon (`Stage Class X Y Dir`),
     bizim okuyucu 4 kolon okuyordu → aynı token zaman aşımı. Düzeltildi;
     doğrulama: `[EventMainManager] SkyEvent loaded (Stage:0, Win:5, Monster-groups:5)`.
- **E2E doğrulama (geçici test sürücüsüyle, `GvG_selftest.flag` / `GvG_testguilds.flag`):**
  Switch=1 ✅ · zaman tablosu geri sayımı tam `HH:MM:00`'a indi ✅ · NPC eşleşmesi
  `idx=16 class=479 map=0 x=130 y=133` + `Dialog()=1` (guildsiz kullanıcı yolu, mesaj 872) ✅ ·
  STAND→START `08:31:01 state=2 remain=60` → `08:32:01 state=3 remain=120` ✅ ·
  `UserDieProc x2 → B=2 / A=0` ✅ · `CalcRank → Winner=B(2), rank B=1 / A=2` ✅ ·
  negatif yol: STAND'da `GetGuildCount()<2` → iptal + EMPTY ✅.
- Sürücü tur sonunda kaynaktan **kaldırıldı** (parite kaynağı temiz); anlık görüntü
  `BuildLog\2c1\EventGvG_E2E_driver_snapshot.{cpp,h}` + tam fark
  `BuildLog\2c1\EventGvG_full_diff.patch`. Kalıcı tek fark: `UserDieProc` 874
  argüman sırası (canlı metin: guild adı, puan).
- Ortam (test, commit'e girmemeli): `MuOnline`+`MuOnlineJoin` DSN'leri
  `Trusted_Connection=Yes`; `DataServer.ini`/`JoinServer.ini` kullanıcı-şifre temizliği;
  `GameServerInfo - Common.ini` adresleri `127.0.0.1`.
- Yeni rapor: `docs\15-EVENTGVG-E2E-DOGRULAMA.md` (kanıt adresleri, log satırları,
  canlı struct kanıtları, backlog).
- Harness: `BuildLog\4GS\dump_oku.js` (bağımlılıksız minidump okuyucu — AV adresi/modül
  çözümü), `BuildLog\4GS\pencere_metin.ps1` (modal pencere metnini okuma).
- Not: `[BossGuild] Ko tao duoc NPC` her saniye loglanıyor (ayrı tur); canlı run
  sırasında `GHRSReset.ini` kendini güncelledi (çalışma-zamanı artefaktı).

---

## [26.10.02 06:50] SPK paket başlık taraması — F3 98/99 dışı 7 kesin fark + 4 aday (salt-okunur, kod değişikliği yok)

**Ne yapıldı** (kullanıcı isteği: "canlı ve bizim exe'de istemci paket tabanını
karşılaştır: F3 98/99 dışında taslakta yanlış başlıkla kalmış diğer SPK
paketlerini tara ve raporla")
- Canlı `GameServer.exe` (40,7 MB /disasm) ile bizim Release_EX603 exe
  (25,2 MB /disasm) paket başlığı düzeyinde karşılaştırıldı: 320 canlı + 331
  bizim fonksiyon eşleşti; 138 birebir, 67 farklı, **9'u SPK klasöründe**.
- **Kesin bulgular (ham bayt kanıtı, 7 kalem):** MocNap `D3 9A/9B → 16/17`,
  Harmony `SendListItemPoint D3 24 → 0C`, JewelBank InfoSend
  `C1 30 F3 F5 (48 B) → C2 0084 F3 F5 (132 B / 30 slot)`, BotMix
  `SendDataIsTrade D3 2E → 23`, LuckySpin `MakeItem+ActionVongQuay
  D3 8C → 21`, CastleEvent `SendKillCTCMini F3 33 → 43` (B4 girdisi),
  Buff `GC_BuffInfo F3 13/52 B → F3 26/64 B` (`F3 13` çakışma uyarısı).
- **Orta güven 4 aday:** Harmony ProcMix (`F7 04` + JewelBank yenileme
  çağrısı), SetStateInterface (`F3 13`), SendInfoItemCache (`D3 00`),
  Store OnPShopBuyItemRecv (ek `18 06` paketi).
- Yeni rapor: `docs\14-SPK-PAKET-BASLIK-TARAMASI.md` (yöntem, kanıt
  adresleri, limitler, düzeltme sırası).
- Yeni araçlar (BuildLog\envanter): `paket_triaj.js`, `paket_spk_filtre.js`,
  `paket_imm.js`, `paket_imm2.js`, `paket_imm_spk.js`,
  `paket_nearmiss.js`; çıktılar `paket_diff.txt`, `paket_diff_spk.txt`,
  `paket_imm_nearmiss_spk.txt`.

**Neden** — 2c.1-B dalgasında paket başlıkları parite kuralına (canlı kanıt >
donor) göre tek tek doğrulanıyor; F3 98/99 (B3) dışında kalan yanlış
başlıkların tespiti B4/B5 ve düzeltme turunun girdisi.

**Doğrulama**
- Her kesin bulgu, iki exe'nin ham `/disasm` satırıyla (adres + bayt) rapora
  işlendi; kod şekli birebir, fark yalnız ilgili immediate.
- Yeniden-kurma artefaktları ayıklandı (size taşması, blok birleşmesi) ve
  raporda ayrıca listelendi.
- `paket_spk_filtre.js` ile farklar canlı `.map` sınıf→`.obj` eşlemesi
  üzerinden SPK klasörüne indirgendi (9 kayıt).

**Durum** — Kod değişikliği YOK (istek: tara + raporla). Düzeltmeler ayrı iş
emri; bu turda commit yok.

---

## [26.10.01 23:40] 2c.1-B2 takip — GameServer.rc "Events" menüsüne IDM_STARTGVG görsel girdisi (Start Guild vs Guild) + derleme doğrulaması

**Ne yapıldı** (kullanıcı isteği: "GameServer.rc menüsüne IDM_STARTGVG görsel
girdisini ekle (resource.h 121 hazır bekliyor) ve derleyip doğrula")
- **Kapanan boşluk:** 2c.1-B2 turunda `IDM_STARTGVG=121` (resource.h),
  handler ([GameServer.cpp](../Source/4.GameServer/GameServer/GameServer.cpp)
  `case IDM_STARTGVG: gGvGEvent.StartGvG();`) ve `/startgvg` komutu
  bağlanmıştı ama **menüde görsel girdi yoktu** — operatör GvG'yi menüden
  başlatamıyordu (o turda "canlı rc menü girdisi sonra eklenebilir; menu
  handler kod tarafı hazır" notu düşülmüştü).
- **Donor karşılığı:** `New Source Code/Source/Source/GameServer/GameServer/
  GameServer.rc:92` → `MENUITEM "Start Guild vs Guild", IDM_STARTGVG`
  (TVT satırının hemen ardında; donor'da Quiz/King/TVT/GVG sırası aktif).
  Metin donor'dan **birebir** alındı.
- **Düzenleme:** [GameServer.rc](../Source/4.GameServer/GameServer/GameServer.rc)
  `IDR_MAINFRAME` menüsü **"Events"** bloğu — satır 94
  (`MENUITEM "Start - Loạn ChiếnPK", IDM_STARTBSV`) sonrasına **satır 95**
  olarak eklendi; hizalama (kolon 50, IDM_STARTDROP/BSV ile aynı) korundu →
  `MENUITEM "Start Guild vs Guild",          IDM_STARTGVG`. Bizim .rc'de
  Quiz/King/TVT/Castle Siege/CryWolf/Loren satırları **yorumlu** olduğundan
  yeni girdi aktif grubun son elemanı oldu (aktif Event satırları artık
  88-95).
- **UTF-16 güvenliği:** dosya UTF-16LE + BOM (FF FE) + CRLF yapısında;
  ekleme node ile UTF-16 düzeyinde yapıldı, BOM (`255 254`) ve tüm satır
  sonları korundu → 30.610 B → 30.738 B (**+128 B** = 64 UTF-16 karakter:
  62 karakterlik satır + CRLF).

**Neden** — B2'nin kapanış boşluğu: modül derlenir ve `/startgvg` ile
çalışır durumda ama menü girdisi olmadan görünmüyordu; tek doğruluk kaynağı
donor .rc (canlı SPK exe'sinde menü metni **YOK** — bu araç penceresi bize ait;
canlıdan kanıt beklenmez, 2c.1-B2 notu).

**Doğrulama**
- **rc diff birebir:** HEAD rc + çalışma ağacı rc, UTF-16LE'den UTF-8'e
  çevrilip karşılaştırıldı (CR'ler temizlenerek — ilk denemede `grep`
  filtresi CR baytlarını düşürdüğü için tüm satırlar farklı görünmüştü;
  araç artefaktı, dosya değil) → **`eski 471 / yeni 472` satır**;
  GvG satırı çıkarıldığında iki dosya **birebir eşleşiyor** (`DIFF_TEMIZ`) →
  tek-satır eklenti, başka değişiklik yok. CR bayt sayısı 471 → 472 =
  her satır CRLF (eklenen satır dahil).
- **Derleme:** MSBuild Release_EX603 / Win32 / v143 → **0 hata** (rc.exe
  GameServer.rc'yi yeniden derledi; vcxproj ResourceCompile kaydı satır 886).
- **Wide-string kanıtı (yeni exe):** `Start Guild vs Guild` UTF-16 off
  **10.666.404**; menü sırası okuması rc ile birebir:
  `Start - Drop` (10.666.328) → `Start - Loạn ChiếnPK` →
  **`Start Guild vs Guild`** → `Invasion…`.
- **Bayt-fark analizi (HEAD exe ↔ yeni exe, `cmp -l`):** farklar yalnızca
  (1) PE zaman damgası (off 280), (2) Resource veri dizini boyutu
  **83.792 → 83.840 (+48 B)**, (3) `.rsrc` VirtualSize `0x14750 → 0x14780`,
  (4) debug dizini zaman damgaları (4 × 28 B, off 1.540.116+), (5) CodeView
  PDB GUID (off 1.559.633), (6) `.rsrc` ham verisi (10.645.520-10.728.316,
  41.908 B). **`.text` kod bölümü (1.024-1.432.575) bayt-birebir aynı** →
  işlevsel kod değişmedi, tek değişiklik menü kaynağı.
- **Boyut sabit kaldı (10.816.512 B) — açıklaması:** `.rsrc`
  `SizeOfRawData 0x14800` (84.480 B) değişmedi; +48 B büyüme bölümün
  688 B'lik hizalama dolgusuna sığdı → tüm bölüm offsetleri ve dosya
  boyutu aynı kaldı (beklenen ~10.816.640 B gerçekleşmedi, sapma değil).
- **Handler bağlı (B2):** menü tıklaması `gGvGEvent.StartGvG()` çağırır;
  `GvGEvent.dat` şablonu `Switch=0` inert olduğundan event kapalı bekler
  (beklenen davranış).
- Geçici karşılaştırma dosyaları (`BuildLog/4GS/rc_*`) silindi (git dışı).

**Commit** — `fbdee8830` · **Tamamlandı** — 01.10.2026 23:40

---

## [26.10.01 23:24] 2c.1-B3 — ActiveInvasions canlı disasm paritesi: paketler C1 10 F3 99 / C2 F3 98, iki-sayaç döngüsü, F7 sub 0x02 liste isteği

**Ne yapıldı** (kullanıcı isteği: "2c.1-B dalgasında sıradaki B3
ActiveInvasions modülünü canlı kanıtla inceleyip uygulamaya al")
- **Sürpriz keşif (modül zaten vardı):** docs/13'te B3 "sıfırdan" etiketliydi
  ama bizim kaynakta `CB_ActiveInvasions.cpp/.h` **ilk commit'ten beri mevcut**
  (`CB_ActiveInvasionsD=1`, stdafx.h:165; vcxproj kayıtlı; InvasionManager ×3,
  DSProtocol giriş-push, User.cpp login-push entegrasyonları bağlı).
  Donor'da ise modül **hiç yok** (UInvasionsData/ActiveInvasions 0 vuruş —
  MUIG-özel). Yani gerçek iş "yazmak" değil, **canlıya karşı denetim** oldu.
- **Canlı kanıt:** `ActiveInvasions.obj` — `?monster_add@CActiveInvasions
  @@QAEXH_N@Z` (0x41E950), `?update_by_monster_id@@QAEXH@Z` (0x41EA80),
  `?send_list_to_client@@QBEXH@Z` (0x41EB20), `?gActiveInvasions@@
  3VCActiveInvasions@@A` (0xAA31CC) + `std::map<int,UInvasionsData>` ağaç
  kodu (hepsi aynı TU'da — tek modül). Disasm gövdeleri okundu (0x41E950-
  0x41EBFC).
- **Bulunan sapmalar (taslak → canlı çekildi):**
  1. **Paket başlıkları:** taslak `C1 10 D3 99` / `C2 D3 98` idi; canlı
     update paketi `C1 10 F3 99 {id, count, 0}` (0x41EAEC'de 99F310C1,
     0x41EABE'de spare=0) ve liste paketi `C2 [size] F3 98 {count, N × 12 B
     {id, count, max}}` (0x41EB56-0x41EB5D'de 0C2h + 98F3h). Yeni exe'de
     `C1 10 F3 99` byte kanıtı (off 185789), eski `D3` varyantları yok.
  2. **monster_del:** taslak sadece `_count--` yapıyordu; canlı iki sayacı
     birlikte azaltıyor (0x4F1C0C bloku: dec [+14] + dec [+18]) ve erase
     kriteri yalnız count==0 (SetState_EMPTY 0x4F1C1B + MonsterDieProc
     0x4F2094 yolları).
  3. **monster_add imzası:** canlı `monster_add(int,bool)` — bool 1 ise
     update_by_monster_id broadcast (0x41E9A5). Taslaktaki tek-argüman imza
     kaldırıldı; çağıranlar: SetMonster `false`, MonsterDieProc yolu
     `monster_del(...,true)` broadcast.
  4. **Eksik istek kancası EKLENDİ:** canlı ProtocolCore'da head `F7`
     sub `0x02` → `send_list_to_client(aIndex)` (0x54FA37, IG-enter sub 1
     ile aynı switch) — bizde hiç yoktu; Protocol.cpp 0xF7 bloğuna case
     0x02 eklendi + `send_list_to_client(int)` tekil-gönderim overload'u
     yazıldı.
  5. **Giriş push'u:** taslak DSProtocol giriş-push'u herkese gidiyordu;
     canlı login-push akışı tek istemciye — User.cpp login bloğuna tekil
     `send_list_to_client(lpObj->Index)` eklendi (DSProtocol push'u
     korundu).
  6. **SetMonster/ClearMonster sonundaki toplu liste-push'ları:** canlı
     karşılığı bulunamadı (istemci F3 40 sub2 ile çekiyor; spawn'da canlı
     sadece bool-broadcast kullanıyor) → korundu (fonksiyonel zarar
     vermez, ileride kanıt gelirse kaldırılır — docs/12 §4 satırında
     notlu).
- **Canlı `ObjectSetStateProc` yolu (0x538687):** boss regen'inde
  `BossInfo[class] {alive,skin,count}` güncellemesi + `monster_add
  (class,true)` — bizim ObjectManager regen yolunda BossInfo modülü yok;
  bizim spawn-path'teki monster_add zaten takip tablosunu dolduruyor (aynı
  kullanıcının gördüğü liste) → ayrı kanca gerekmedi (not düşüldü).
- **Değişen dosyalar:**
  [CB_ActiveInvasions.h](../Source/4.GameServer/GameServer/CB_ActiveInvasions.h)
  (UInvasionsData adı + PMSG_ACTIVE_INVASIONS_UPDATE_SEND + imzalar),
  [CB_ActiveInvasions.cpp](../Source/4.GameServer/GameServer/CB_ActiveInvasions.cpp)
  (paket/gövde paritesi), [InvasionManager.cpp](../Source/4.GameServer/
  GameServer/InvasionManager.cpp) (monster_add(false) + notlar),
  [Protocol.cpp](../Source/4.GameServer/GameServer/Protocol.cpp) (F7 sub
  0x02 kancası), [User.cpp](../Source/4.GameServer/GameServer/User.cpp)
  (login tekil push, include).

**Neden** — docs/13 B3: modül MUIG-özel, tek doğruluk kaynağı canlı
exe; taslak paket başlıkları istemcinin dinlemediği kanallara gidiyordu
(D3 98/99 vs F3 98/99) — parite olmadan HUD liste güncellemesi hiç
görünmezdi.

**Doğrulama**
- GS derlemesi temiz (Release_EX603, 0 hata) → **10.816.512 B (23:22)**;
  CB_ActiveInvasions.obj 156.710 B (obj içinde `??0UInvasionsData@@`
  sembol kanıtı).
- Exe byte kanıtı: `C1 10 F3 99` ×1 (off 185789); eski `C1 10 D3 99` /
  `C2 ?D3 98` desenleri 0 (temiz geçiş).
- Çağrı-zincir kanıtları canlı disasm'da: SetState_EMPTY 0x4F1C1B,
  SetMonster 0x4F1EBF/0x4F1EFC, MonsterDieProc 0x4F2094,
  ObjectSetStateProc 0x538687, ProtocolCore 0x54FA38.

**Commit** — `c7d0b464e` · **Tamamlandı** — 01.10.2026 23:24

---

## [26.10.01 21:43] 2c.1-B2 — EventGvG tam uygulama: donor 1138 satır birebir port + canlı kanıt denetimi (NPC Dialog/rank/StartGvG/9 kanca)

**Ne yapıldı** (kullanıcı isteği: "EventGvG iskeletini tam uygulamaya taşı:
donor 1138 satırı canlı kanıtla denetleyip NPC spawn/Dialog/rank/StartGvG
işlevlerini bağla")
- **Canlı kanıt denetimi (docs/12 §4 kuralı):** canlı map
  (GameServer_canli.map) yeniden tarandı → **CGvGEvent/EventGvG sınıf
  sembolleri canlıda YOK**; GvG izi yalnız 7 config anahtarı
  (ServerInfo.obj 0x64b130-0x64b19c). Donor canlısı (4.MuServer Sub-1)
  üzerinde doğrulama: `Event\\GvGEvent.dat` **deploy edilmemiş** ve
  Command.txt'te `/startgvg` satırı **yok** (/starttvt var, satır 74) →
  GvG, donor canlısında da **kapalı bekleyen modül** (donor Load'ı dosya
  yoksa ErrorMessageBox→ExitProcess'e düşer — yani donor canlısında bu
  yol asla tetiklenmemiş).
- **Donor bağımlılık eşleşmesi (bizim kaynakta birebir aynı):**
  CEventName::GlobalRemainTime + GVG_EVENT_TIME=10 (CEventName.h satır
  bile eş), CScheduleManager+CTime deseni (CastleDeep.cpp ile aynı),
  gGate.GetGate, GDSetCoinSend (DSProtocol.cpp:4826), GCNoticeSend/
  GCChatTargetSend, GetInventoryItemCount/DeleteInventoryItemCount,
  CharacterUpdateMapEffect, SearchGuild_Number, G_MASTER, LOG_EVENT
  (ServerDisplayer.h:32 = donor LOG_EVENT=6).
- **[EventGvG.cpp](../Source/4.GameServer/GameServer/EventGvG.cpp)
  (137→905 satır):** donor 1138 satır **birebir port** — ctor/Init/Clear/
  Load (MemScript 4 bölüm: süreler/GVG_TIME zaman tablosu/etkinlik alanı
  + seviye-reset sınırları/ödül coinleri)/MainProc (1 sn tick +
  GlobalRemainTime broadcast + 5 ProcState)/SetState 5 geçiş gövdesi
  (STAND/START/CLEAN süre kurulumları, CLEAN→CalcRank + kazanan guild
  coin ödülü)/CheckSync (schedule boşsa BLANK)/Dialog (7 config anahtarı
  eşleşen NPC'de GCChatTargetSend 864 + CheckEnterEnabled 16 kontrollü
  girit)/CheckReqItems (ChaosLock'lu item bedeli)/AddUser/AddGuild
  (StartGate+n slot)/GetUserRespawnLocation/UserDieProc (öldüren guild
  +1 puan, NOTICE 874)/NoticeSendToAll (varargs)/CalcRank (puan
  sıralaması → Winner + NOTICE 876)/StartGvG (şimdi+2 dk schedule push +
  `[Set GVG Start] At %02d:%02d:00` LOG_EVENT logu). **Tek sapma (donor
  yazım hatası):** `GetAsNumber` yerine `gettype` çağrısı → GetAsNumber
  (derlemeyen donor satırı); Load başına dosya-yok guard'ı eklendi
  (SetBuffer 0 → BLANK + çıkış, ExitProcess koruması — donor canlı
  davranışı korunur, boot bloklanmaz).
- **Kancalar (9 nokta, donor wiring birebir):**
  [ServerInfo.cpp](../Source/4.GameServer/GameServer/ServerInfo.cpp)
  ReadEventInfo → `gGvGEvent.Load("Event\\GvGEvent.dat")` (donor:466);
  [MonsterManager.cpp](../Source/4.GameServer/GameServer/MonsterManager.cpp)
  Init → gGvGEvent.Init (donor:441, gTvTEvent.Init sonrası);
  [NpcTalk.cpp](../Source/4.GameServer/GameServer/NpcTalk.cpp) → Dialog
  (donor:114, gTvTEvent.Dialog sonrası);
  [Attack.cpp](../Source/4.GameServer/GameServer/Attack.cpp) →
  CheckPlayerTarget×2 (CheckSelfDefense kaldırma, donor:887) +
  CheckStandTarget/CheckPlayerJoined×2/CheckSelfTeam (donor:2029-2047);
  [ObjectManager.cpp](../Source/4.GameServer/GameServer/ObjectManager.cpp)
  CharacterGetRespawnLocation → GetUserRespawnLocation (donor:1014);
  [User.cpp](../Source/4.GameServer/GameServer/User.cpp) gObjUserDie →
  UserDieProc (donor:2819);
  [GameServer.cpp](../Source/4.GameServer/GameServer/GameServer.cpp) →
  IDM_STARTGVG (donor:417, resource.h 121);
  [CommandManager.cpp](../Source/4.GameServer/GameServer/CommandManager.cpp)
  → COMMAND_STARTGVG=86 + CommandStartGvG (donor:634/4082; enum
  renumber'sız son slota — pozisyonel bağlama korundu, /addbuff dormant
  87'ye alındı);
  [resource.h](../Source/4.GameServer/GameServer/resource.h) →
  IDM_STARTGVG=121.
- **Deploy (3 hedef — 2b.1-A2 ağaç desenleri):**
  `Event\\GvGEvent.dat` **default-inert şablon** (554 B CRLF, MemScript
  formatı TvTEvent.dat deseninde: bölüm 0 süreler, bölüm 1 zaman tablosu
  **boş** → CheckSync BLANK'a kilitler = event kapalı bekler, bölüm 2
  alan satırı, bölüm 3 coinler) → MuServer/4.GameServer/{Data/Event,
  Sub 1/Data/Event} + 4.GameServer_real/Sub 1/Data/Event.

**Neden** — docs/13 B2: donor kaynak bu modül için tek donor; canlı exe
motor sembolleri içermediğinden disasm denetimi mümkün değil (denetim
bugünü: canlı sadece 7 config anahtarı bilir). Parite kararı: motor
donor'dan birebir, data tarafı donor canlı davranışı (kapalı) korunur.
Etkinleştirme: Switch=1 + bölüm 1'e zaman satırı.

**Doğrulama**
- GS derlemesi temiz (Release_EX603, MSBuild 0 hata) → **10.816.512 B
  (21:42)** (+7.168 B); EventGvG.obj 207.721 B (BuildLog\\4GS).
- Yeni exe string kanıtları: `Event\\GvGEvent.dat` ×1 (canlıda YOKTU),
  `CommandStartGvG` ×1, `Set GVG Start` ×1 (+ mevcut EventGvGSwitch).
- GvGEvent.dat ×3 dizinde; guard yolu: dosya silinirse bile GS boot
  devam eder (BLANK).

**Commit** — `c6df22bd7` · **Tamamlandı** — 01.10.2026 21:43

---

## [26.10.01 20:56] 2c.1-B1 — EventMainManager iskeleti: canlı SPK_EventMainManager kanıtlarıyla (SkyEvent + yol paritesi + taşıyıcı yapı)

**Ne yapıldı** (kullanıcı isteği: "2c.1-B ilk iş: EventMainManager event
iskeletini canlı kanıtlarla yaz (diğer event'lerin taşıyıcısı)")
- **Canlı kanıt (giriş):** `SPK_EventMainManager.obj` (canlı map 3207-3293)
  üç katman: (1) **SPK GUI editor** — DialogProc/LoadDetails/Save ailesi
  (CC/DS/Invasion/CTCMini/SkyEvent/Item380/ItemDrop/ItemMove/ItemOption/
  ItemStack/MocNap); (2) **config şema anahtarları** (exe string):
  `//WarningTime/NotifyTime//Year/DoW//Level/ExperienceTable1/2/
  MoneyTable1/2/GateNpcLife/StatueNpcLife//Enable/PKCanJoin/EnableQuest/
  KillCount//Reward/BlowUserRate/ExpRank%d/MoneyRank%d` +
  `BLOOD/DEVIL/CHAOS_CASTLE_START_TIME` emplace'leri (BC/CC/DS
  yukleyicileri bu obj'de); (3) **SkyEvent runtime event**:
  `map<int,vector<SKYEVENT_MONSTER_DATA>>` + `SKYEVENT_REWARD_DATA` +
  `vector<INVASION_START_TIME>`.
- **Config şema paritesi:** canlı .ini'ler okundu (BloodCastle/
  ChaosCastle/DevilSquare/IllusionTemple/InvasionManager) — sema bizim
  MEM-script .dat'larla BİREBİR (numaralı bölümler, aynı kolonlar); canlı
  exe yolları `.ini` ×1 / `.dat` ×0, bizim tersi → **5 canlı .ini deploy
  agacına kopyalandı** (kod yolları korunuyor — MemScript ayristiricisi
  içerik zaten aynı; yol-geçişi ayrı kalemde, çift dosya geçici durum).
- **SkyEvent şemaları canlıdan okundu:** `Event\SkyEvent\Config.xml`
  (SkyEvent/EventStage/Stage{Enabled,StageMin0..2} + EventTime/Time{...}
  + EventWin/Win{iLevel,LevelMin,LevelMax,ExtraExpStage0..2,ItemType,
  ItemIndex,ItemLevel,ItemDur,ItemLuck,ItemSkill,ItemOpt,ItemExc,WcoinC,
  WcoinP,GPoint}) + `Event\SkyEvent\Monster.ini` (bolum numarasi +
  "Class X Y Dir" satirlari); yollar canlı exe'de ×1.
- **[SPK/EventMainManager.h](../Source/4.GameServer/GameServer/SPK/
  EventMainManager.h) (YENİ):** canlı struct adlarıyla
  SKYEVENT_MONSTER_DATA/SKYEVENT_REWARD_DATA + SKYEVENT_STAGE_DATA +
  SKYEVENT_START_TIME + eEventMainSection (canlı editor string
  anahtarlarının enum etiketleri) + 5-durumlu state machine
  (BLANK/EMPTY/STAND/START/CLEAN — EventGvG deseni).
- **[SPK/EventMainManager.cpp](../Source/4.GameServer/GameServer/SPK/
  EventMainManager.cpp) (YENİ):** Init/Clear + `LoadSkyEvent()`
  (pugixml Config.xml + MemScript Monster.ini — canlı şema birebir) +
  `Load()` merkez yukleyici + MainProc state machine iskeleti
  (gObjEventRunProc kancası) + CheckSync (Year/Month/Day/DoW jokerli
  schedule eşleşmesi; '*' = -1).
- **Kancalar:** [ServerInfo.cpp](../Source/4.GameServer/GameServer/
  ServerInfo.cpp) ReadEventInfo başı → `gEventMainManager.Load()`;
  [User.cpp](../Source/4.GameServer/GameServer/User.cpp)
  gObjEventRunProc → `gEventMainManager.MainProc()` (gGvGEvent yanına);
  vcxproj ClCompile+ClInclude.
- **Kapsam dışı (belgeli):** SPK GUI editor ailesi (DialogProc/Save) —
  GameServer runtime işlevi değil; SkyEvent spawn/ödül gövdeleri ve tekil
  event yukleyicilerinin merkezileştirilmesi sonraki 2c.1-B adımları.

**Neden** — docs/13 2c.1-B'nin ilk işi: event iskeleti diğer event'
lerin taşıyıcısı; canlıda BC/CC/DS/SkyEvent yükleyicileri zaten bu
obj'nin arkasında — bizim iskelet aynı merkezileşmeyi kuruyor.

**Doğrulama**
- GS derlemesi temiz → **10.809.344 B (20:55)** (+6.656 B) — User.cpp
  include eksikliği derlemede yakalanıp düzeltildi.
- EventMainManager.obj: SKYEVENT_MONSTER_DATA ×16, SKYEVENT_REWARD_DATA
  ×13, LoadSkyEvent ×1, gEventMainManager ×2; yeni exe:
  `Event\SkyEvent\Config.xml` ×1 + `Event\SkyEvent\Monster.ini` ×1 +
  `[EventMainManager] SkyEvent loaded` ×1; PDB: EventMainManager ×28.
- Deploy: 5 canlı .ini + SkyEvent\ (Config.xml + Monster.ini) kopyalandı.

**Commit** — `9454d5d98` · **Tamamlandı** — 01.10.2026 20:56

## [26.10.01 20:32] 2c.1-A1 — SPK_Harmony modülü canlı CustomHarmony.xml şemasından sıfırdan yazıldı

**Ne yapıldı** (kullanıcı isteği: "2c.1-A dalgasını başlat: SPK_Harmony
modülünü canlı CustomHarmony.xml şemasından sıfırdan yaz, derle + commit")
- **Canlı kanıt (giriş):** canlı map SPK_Harmony.obj (1553-1563):
  `Instance/Load/GetMessageA/SetStateInterface/ProcMix/SendListItemPoint/
  SendInfoItemCache/ProcItemSend/BackItem/Save` + `SPK_HarmonyProc`
  (SPK GUI editör WndProc — kapsam dışı) + `map<int,HM_HAMORNY>` +
  `vector<OPTWEAPON/OPTSTAFF/OPTITEM>`; canlı XML okundu (Data\SPK\
  CustomHarmony.xml, 2788 B): root `Harmony{Enable=0,PriceType=1,Price=5000,
  Rate=50}` + 6 Msg + `NPC{734,0,143,140,1,"Harmony Option"}` +
  OptWeapon(10)/OptStaff(8)/OptItem(8) Option{OptIndex,Name,Level,Rate}.
- **Kritik teknik bulgu:** canlı XML OptIndex değerleri (16/32/48/64/86/
  102/121/137/153/173) bizim engine harmony-byte kodlamasının birebir
  kendisi — `(option<<4)|level` (86=0x56→opt5/lvl6, 173=0xAD→opt10/lvl13;
  JewelOfHarmonyOption.cpp:433 kodlamasıyla aynı). Canlı modül XML
  tablosundan kodlu byte'ı doğrudan item'a yazıyor.
- **Dispatcher çözümü (canlı ProtocolCore 0x551984 jump-tablosu,
  disasm 426575-426635):** `[esi+4]==0x6F` → `SetStateInterface(aIndex,0)`,
  `==0x71` → `ProcMix(aIndex)`, `ProcItemSend(aIndex, byte[esi+6])`,
  `[esi+4]==-1` koruması → `BackItem(aIndex,[esi+4])`. Bizim 0xD3 switch
  aile konvansiyonuyla aynı (SauChangeItem 0x6A/6B/6C deseni — 0x6F/111
  ThaoTac koruması bile birebir) → dallar 0xD3 altına 0x6F/0x70/0x71/0x72
  olarak eklendi (0x70=item-send, 0x72=back; komşu alt-kodlara çakışma
  kontrolü yapıldı).
- **[SPK_Harmony.h/cpp](../Source/4.GameServer/GameServer/SPK/SPK_Harmony.cpp)
  (YENİ):** canlı semantikle — Load (pugixml, şema birebir), GetMessage
  (Msg map), SetStateInterface (Enable=0 → canlı Msg0 'Disabled'; oturum
  aç; `m_HarmonyMap.size()>=50` → canlı string `[DaTaoHoa] Data qua dai
  !!`), ProcItemSend (envanter-sınırı, engine kısıt seti:
  CheckJewelOfHarmonyItemType/SetItem-switch/IsJewelOfHarmonyItem/
  IsSocketItem — AddJewelOfHarmonyOption ile aynı korumalar), BackItem
  (-1 koruması birebir), ProcMix (XML tablo seçimi + kademeli Rate
  50/40/30/20/10 + global Rate yedeği; harmony taşı 14,41 kontrolü →
  canlı Msg5; PriceType 1/2/3→WcoinC/WcoinP/GP, diğer→Zen — canlı
  `Wcoin`+`Harmony enhancement` string ailesi; GetLargeRand rate;
  `m_JewelOfHarmonyOption=OptIndex` + Convert +
  CharacterMakePreviewCharSet + GCItemModifySend),
  SendListItemPoint (`0xD3:0x24` — BCustomVIPChar 0xD3:0x24 liste
  paketiyle aynı aile; canlı C2 `0xD3:0C00C2`+[esi+14h] çözümü notu).
- **Kancalar:** [ServerInfo.cpp](../Source/4.GameServer/GameServer/ServerInfo.cpp)
  ReadCustomInfo → `gCustomHarmony.Load("SPK\\CustomHarmony.xml")`
  (canlı yolu birebir); [Protocol.cpp](../Source/4.GameServer/GameServer/
  Protocol.cpp) 0xD3 switch'e 4 dal; vcxproj ClCompile+ClInclude.
- **Deploy:** canlı CustomHarmony.xml (2788 B) Data\SPK\ altına birebir
  kopyalandı (Enable=0 — canlı durumuyla kapalı gelir; açmak için 1).

**Neden** — docs/13 2c.1-A dalgasının ilk işi (A1); harmony
zenginleştirme NPC-servisi çekirdek oynanış sınıfının ilk modülü.

**Doğrulama**
- GS derlemesi temiz → **10.802.688 B (20:30)** (+7.168 B) — 2 hata
döngüsü derlemede yakalanıp düzeltildi (PSWMSG_HEAD include +
Instance/LPITEM).
- Yeni exe: `SPK\CustomHarmony.xml` ×1, `Harmony enhancement` ×1,
  `[DaTaoHoa] Data qua dai !!` ×1 (canlı string ailesi).
- SPK_Harmony.obj: `?Load@CustomHarmony@@QAEXPAD@Z` ×1 (**canlı mangled
  sembolle birebir**), `HM_HAMORNY` ×15; PDB: `CustomHarmony` ×25 +
  `SPK_Harmony.cpp` ×3.

**Commit** — `01c7d2b16` · **Tamamlandı** — 01.10.2026 20:32

## [26.10.01 19:40] E-01 kalan parça (1/2) — BossGuild canlı log/yol paritesi: 7 kill-notice + Start Boss + Winning + Finish + CTCMini GuildWin.ini

**Ne yapıldı** (aynı tur — canlı metinler `was killed by/Start Boss/…` İngilizce,
bizde Vietnamese idi; tam SpawnBoss/HandleBossKill yeniden-yazımı 2/2'de)
- **Canlı kanıt (exe byte-bağlam, giriş verisi):** `Boss %s (Class:%d) was
  killed by %s (Guild:%s)`, `Start Boss %s`, `[BossGuild] Winning guild:
  %s`, `Winning guild member reward: %s`, `[BossGuild] Finish 1/2`,
  `Guild [%s] Score: %d / %d (KillBoss)`, `Reward Item sent to %s
  (Type:%d, Index:%d)`, `Spawn Boss next (Class:%d)...`,
  `[Set Boss Guild Start] At %2d:%2d:00`, boss adları `Wizard/Knight/
  Summoner/Rage Fighter/Magic Gladiator/Dark Lord`; kazanan-persist yolu
  byte-bağlamda **`..\Data\Event\CTCMini\GuildWin.ini`** (hemen arkasında
  GuildName/GuildWinOLD/[BossGuild] — modülün kendi kullanımı;
  `Event\BossGuild\GuildWin.ini` canlıda ×0).
- **[BossGuild.cpp](../Source/4.GameServer/GameServer/BossGuild.cpp):**
  MonsterDie'nin 7 Vietnamese kill-notice'ı → canlı `Boss %s (Class:%d)
  was killed by %s (Guild:%s)` (boss adları canlı BossGuild.xml
  yorumlarıyla eşleşen Wizard/Knight/Fairy/Summoner/Rage Fighter/Magic
  Gladiator/Dark Lord; canlı formatın %s adı + %d Class alanı birebir).
  7 `StartBoss*` Vietnamese logu → canlı `Start Boss %s` + ad.
  Kazanma bloğu: `[BossGuild] Winning guild: %s` + `Winning guild member
  reward: %s`. `Finish 1` → canlı `Finish 1/2`. 4× GuildWin.ini yolu
  `Event\BossGuild\` → `Event\CTCMini\` (canlı byte-kanıtı).
- **Korundu:** oynatıcı-yönelimli Vietnamese işlev mesajları (cống
  hiến/Chiến thắng/kết thúc ailesi) — canlı kanıt kapsamı log/notice
  formatları; bunlar işlev, bu tur kapsamı dışı.
- **Kalan (2/2, belgeli):** `Guild [%s] Score: %d / %d (KillBoss)`,
  `Reward Item sent to…`, `Spawn Boss next (Class:%d)...` stringleri
  canlı SpawnBoss(int,int,int,char*)@0x413010 / HandleBossKill@0x4132b0 /
  MonsterDie@0x4135d0 mimarisine ait — tam yeniden-yazım ayrı iş kalemi
  (canlı HandleBossKill disasm'ı henüz okunmadı). docs/09 §9.

**Neden** — E-01 stratejisinin düşük-riskli katmanı: canlı log/yol
paritesi; davranış değişikliği yok (yol farkı persist dosyası konumu).

**Doğrulama**
- GS derlemesi temiz → **10.795.520 B (19:39)**.
- Yeni exe taraması: 5 canlı string ×1 VAR (`was killed by`, `Start Boss
  %s`, `Winning guild:`, `member reward:`, `Finish 1/2`) +
  `..\Data\Event\CTCMini\GuildWin.ini` ×1 VAR; `tiêu diệt` ×0 (kalıntı
  yok). Score/Reward Item/Spawn Boss next ×0 — bilinçli (2/2 kapsamı).

**Commit** — `6c27fbcd8` · **Tamamlandı** — 01.10.2026 19:40

## [26.10.01 19:24] E-06 kalan parça (1/2) — canlı EventTime.xml mimarisi: Load + GetEventTime + ServerInfo bağlantısı + deploy

**Ne yapıldı** (araştırma turunun BÜYÜK bulgusu: canlı mimari bizden farklı —
CEventName/EventName.xml canlıda YOK; canlı CCustomEventTime kendi
Load/GetEventTime/GCReqEventTime'ına sahip, `Event\EventTime.xml` okuyor)
- **Canlı kanıt (giriş):** canlı map: `?Load@CCustomEventTime@@QAEXPAD@Z`
  0x473C80, `?GetEventTime@@QAEHE@Z` 0x4742A0, GCReqEventTime 0x474340
  (disasm satır 146450+); `MESSAGE_INFO_EVENTIME` map tipi canlı map'te;
  canlı EventTime.xml 2714 B okundu: `SPK Enable=1` + `Message/Msg
  Index/Text` + 22× `Event{Slot,Name,Map,Gate,Status}` — Event main slot
  0-7 (Blood Castle/Devil Square/Chaos Castle/CTCMini/FFA/King Mu/Divine
  War/Guild Boss), Invasion slot 8-21 (Golden Boss…Invincible War God).
- **Kritik config bulgusu:** canlı `Custom.ini:196 CustomEventTimeSwitch =
  0` → canlı GCReqEventTime `cmp ds:[0A9D39C],0 / je return` ile ERKEN
  DÖNÜYOR — event-saat penceresi canlıda KAPALI. Bizim deploy'da anahtar 1
  ama EventName.xml hiç yok → liste zaten boş. **Deploy INI 1→0** (canlı
  birebir) yapıldı.
- **[CustomEventTime.h](../Source/4.GameServer/GameServer/CustomEventTime.h):**
  `MESSAGE_INFO_EVENTTIME` (int+Text[0x100], canlı map değeri 260B) +
  `EVENT_INFO_EVENTTIME` (46B: Slot+Name[30]+Map+Gate+Status — disasm
  `imul 2Eh`) + MAX_EVENTTIME_TABLE=30 (canlı 0x1E); store: m_Enable,
  `std::map<int,MESSAGE_INFO_EVENTTIME>`, 42×46B m_EventInfo, m_SlotUsed
  (canlı +0x185C bayt dizisi), m_RemainTime[30] (canlı 0x9AE5F8 tablosu).
- **[CustomEventTime.cpp](../Source/4.GameServer/GameServer/
  CustomEventTime.cpp):** `Load()` pugixml (E-04 deseni): SPK/Enable →
  Message/Msg map → EventTime/Event (Slot 0..41 sınırı — canlı 0x29).
  `GetEventTime(BYTE)`: slot 0-7 → `gEventName.GlobalRemainTime(slot)`
  (2b.2-O'da bağlanan 11 yazıcının beslediği global sayaç deposu — canlı
  yazar adresleriyle aynı semantik: 9AE5DC←BC bölgesi 0x43DA58, 9AE5E4←CC
  bölgesi 0x45AA91); slot ≥8 → m_RemainTime[slot-8]; sınır dışı 0.
- **[ServerInfo.cpp](../Source/4.GameServer/GameServer/ServerInfo.cpp):**
  ReadEventInfo'ya `gCustomEventTime.Load("Event\\EventTime.xml")` (canlı
  ServerInfo.obj yolu birebir).
- **Deploy:** canlı EventTime.xml (2714 B) Data\Event\ altına birebir
  kopyalandı; Custom.ini 234: `CustomEventTimeSwitch = 0`.
- **Kapsam dışı bırakılan (belgeli):** GCReqEventTime paket yolu DOKUNULMADI
  (donor 132B girdi vs canlı iç 46B — canlı paket düzeni tam çözülmeden
  swap riskli; pencere iki tarafta da ölü: canlı switch=0, bizim liste
  boştu). Kalan: canlı GCReqEventTime disasm çözümü (satır 146450+),
  invasion tablo dolumu (InvasionManager paritesi) ve donor index seti ↔
  canlı event-main seti birleşimi. docs/09 §9'a işlendi.

**Neden** — E-06 stratejisinin çekirdek iddiası: veri katmanı canlı
EventTime.xml şemasına geçecek; mimari (Load+GetEventTime+global sayaçlar)
artık bizde de canlıyla aynı hatta.

**Doğrulama**
- GS derlemesi temiz → **10.795.520 B (19:23)** (+4.096 B).
- Yeni exe: `Event\EventTime.xml` yolu ×1 VAR (canlı ServerInfo.obj ile
  aynı string); CustomEventTime.obj: `?Load@CCustomEventTime@@QAEXPAD@Z` ×1
  + `GetEventTime@CCustomEventTime` ×1; PDB: MESSAGE_INFO_EVENTTIME ×36.

**Commit** — `8af2a6f3a` · **Tamamlandı** — 01.10.2026 19:24

## [26.10.01 19:05] E-02 kalan parça — BotAlchemist hata stringleri canlı formata çevrildi (5 çağrı)

**Ne yapıldı** (aynı tur — araştırma bulgusu: bizde hatalar Vietnamese,
canlıda `BotAlchemist error:` önekli İngilizce)
- **Canlı kanıt (exe byte-bağlam):** `BotAlchemist data load error %s`,
  `BotAlchemist error: BotPetIndex:%d out of range!`,
  `BotAlchemist error: BotPetIndex:%d doesnt exist`,
  `BotAlchemist error: Min Slot 0 ; Max Slot 8`,
  `[Using Class Error] Error UseClass %d` (son ikisi 2b.2-O'da zaten
  birebir yakalanmıştı).
- **[BotAlchemist.cpp](../Source/4.GameServer/GameServer/BotAlchemist.cpp):**
  5 Vietnamese hata canlı birebir ile değiştirildi — satır 80
  (`…out of range!`), 106/139/162 (`…doesnt exist` — canlıda ünlem YOK),
  114 (`Min Slot 0 ; Max Slot 8`). Oynatıcı-yönelimli Vietnamese mesajlar
  (GCNoticeSend/ChatSend: `Bạn Cần %d WcoinP…` vb.) KORUNDU — canlı kanıt
  kapsamı hata logları; bunlar işlev, string-parite dışı.
- Read alan şeması zaten canlı CongHuong.txt ile birebir doğrulanmıştı
  (araştırma turu: OnlyVip/Coin1/Zen/Coin2/OnlySameType/OnlyLowerIndex/
  AcceptAncient/MaxLevel/MaxExc ✓) — dokunulmadı.

**Neden** — E-02'nin kalan alt parçası: log/parite stringleri; canlı
konsol çıktısında Vietnamese hata görünmemeli.

**Doğrulama**
- GS derlemesi temiz → **10.791.424 B (19:05)**.
- Yeni exe taraması: `…out of range!` ×1, `…doesnt exist` ×1, `Min Slot…`
  ×1, `UseClass %d` ×1 VAR; `Cộng Hưởng Lỗi` ×0 (kalıntı yok).
- `BotAlchemist data load error %s` exe'de 0 / **obj'de ×1** — bilinen
  LTCG string artefaktı (2b.2-O'dan beri kayıtlı; kod sağlam).

**Commit** — `d75ca6f2a` · **Tamamlandı** — 01.10.2026 19:05

## [26.10.01 19:03] E-04 kalan parça — ClearMasterChangeClass INI anahtarı + koşullu adım (canlı Command.ini:270)

**Ne yapıldı** (kullanıcı isteği: "docs/09'daki kalan E-kalemlerini …
stratejilerine göre işle" — araştırma turunda bulunan alt parçaların işlenmesi)
- **Canlı kanıt:** `GameServerInfo - Command.ini:270 → ClearMasterChangeClass =
  0  //Clear master points when changing class 1 On/0 Off` (canlıda VAR);
  araştırma turunda bizim kaynakta bu anahtarın okunmadığı, ChangeClass()
  içinde `ClearMasterChangeClass()` koşulsuz çağrıldığı tespit edilmişti →
  canlıda master temizleme VARSAYILAN KAPALI, bizde daima açıktı.
- **[ServerInfo.h](../Source/4.GameServer/GameServer/ServerInfo.h):**
  `m_CommandClearMasterChangeClass` üyesi (ChangeClassTo* bloğunun başına).
- **[ServerInfo.cpp](../Source/4.GameServer/GameServer/ServerInfo.cpp):**
  `ReadCommandInfo` içinde `CommandChangeClassToDW` okumasının ardına
  `GetPrivateProfileInt(section,"ClearMasterChangeClass",0,path)` (canlı
  anahtar adı birebir; canlı bölüm düzeniyle aynı komşuluk).
- **[ChangeClass.cpp](../Source/4.GameServer/GameServer/ChangeClass.cpp):**
  `ChangeClass()` içindeki koşulsuz çağrı
  `if (gServerInfo.m_CommandClearMasterChangeClass != 0)` koşuluna bağlandı
  (default 0 → canlı davranışı: master temizleme off).
- **Deploy config:** `GameServerInfo - Command.ini` 259. satır ardına canlı
  satır birebir eklendi (`ClearMasterChangeClass = 0` + canlı yorumu).

**Neden** — E-04'ün 2b.2-O'da kapatılan temel entegrasyonundan sonra
araştırma turu (§4 strateji denetimi) bu alt parçayı çıkardı; davranış
farkı gerçekti (koşulsuz master sıfırlama vs canlı koşullu).

**Doğrulama**
- GS derlemesi temiz → **10.791.424 B (19:03)** (LTCG küçük farkı absorbe
  etti; boyut değişmedi).
- Yeni exe string taraması: `ClearMasterChangeClass` ×1 VAR (INI anahtar
  adı canlıyla aynı).
- Diğer 5 E-kalem parçası ayrı kayıtlarda: E-02 (19:xx), E-09+E-11 karar
  kaydı, E-06, E-01.

**Commit** — `0c0f57431` · **Tamamlandı** — 01.10.2026 19:03

## [26.10.01 18:49] 2b.2-R — Reload ailesinin kalanı: GameMaster + ExperienceTable + SPK alt kümesi (AddBuff/CustomMonsterSkill)

**Ne yapıldı** (kullanıcı isteği: "Reload ailesinin kalanını uygula:
GameMaster ve ExperienceTable'a (/reload gamemaster, /reload
experiencetable) + SPK alt kümesine (AddBuff, CustomMonsterSkill,
CustomShop, ResetChange — canlı 'saved and reloaded' deseni) m_Path+Reload
ekle")
- **Canlı kanıt (giriş):** canlı exe'de `saved and reloaded` tam 4 kez:
  `[SPK] CustomMonsterSkill configuration saved and reloaded`, `[SPK]
  ResetChange configuration saved and reloaded`, `[SPK] AddBuff
  configuration saved and reloaded` (4.'sü `[EffectManagerUI]` editörü —
  kapsam dışı) + `[SPK] CustomShop configuration reloaded` ("saved and"
  ve nokta YOK) + `[CGameMaster] GameMaster configuration reloaded` +
  `[CExperienceTable] ExperienceTable configuration reloaded`. Canlıda
  `gamemaster`/`experiencetable` komut-adı stringi YOK → canlıda reload'lar
  editör-sürüklü; bizim `/reload` dalları aynı davranışı operatöre açar
  (2b.2-N deseni).
- **[GameMaster.h/cpp](../Source/4.GameServer/GameServer/GameMaster.cpp):
  `m_Path[256]` + `Load` sonunda yol saklama + `Reload()` (boş-yol koruması
  → array+m_count yedeği → Load → boşsa geri yükle → canlı-format log).
- **[ExperienceTable.h/cpp](../Source/4.GameServer/GameServer/
  ExperienceTable.cpp):** aynı desen (vector yedeği, `empty()` koruması).
- **[CustomMonsterSkill.h/cpp](../Source/4.GameServer/GameServer/
  CustomMonsterSkill.cpp):** aynı desen (1000-eleman array yedeği; m_count
  public üye, geri yüklemede o da dönüyor).
- **[SPK/PC_AddBuff.h/cpp](../Source/4.GameServer/GameServer/SPK/
  PC_AddBuff.cpp):** `m_Path[256]` private + `Read` başarılı okuma sonunda
  yol saklar + `Reload()` (IsReadData yedeği; Read `SkillCount`'u sıfırladığı
  için boş-koruma `SkillCount==0` ile).
- **[CommandManager.cpp](../Source/4.GameServer/GameServer/CommandManager.cpp):**
  `/reload` ailesine 4 dal: `gamemaster`, `experiencetable`, `addbuff`,
  `custommonsterskill` (include'lar: ExperienceTable.h,
  CustomMonsterSkill.h; GameMaster.h ve PC_AddBuff.h zaten vardı).
- **2b.2-N kalıtsal bug düzeltmesi:** `CNotice::Reload` geri yüklemede
  `m_count`'u geri koymuyordu — eski veri dönse de `MainProc` notice döngüsü
  `m_count==0` ile ölürdü; `this->m_count = oldCount;` eklendi. Diğer
  2b.2-N modülleri (Gate/Move/MoveSummon/ResetTable/SkillManager)
  konteyner-tabanlı, boyut türetdiklerinden etkilenmedi.
- **⛔ CustomShop + ResetChange:** reload bağlanacak sınıf bizde de donor'da
  da YOK (canlı-özel SPK modülleri; bizde sadece `m_CustomShopMessageBox`
  INI anahtarı var) — m_Path+Reload uygulanamadı; modül portajı ayrı iş
  kalemi (docs/09 §8 tablosuna işlendi).
- **Kalan farklar (bilinçli ertelendi, docs/09 §8):** GameMaster formatı
  bizde MemScript txt ↔ canlı pugixml XML (`Util\GameMaster.xml`);
  CustomMonsterSkill yolu bizde `Custom\` ↔ canlı `SPK\` (deploy'da dosya
  `Custom\`'ta — hizalama dosya taşıması istiyor, ayrı tur).

**Neden** — 2b.2-N "GameMaster/ExperienceTable hariç" notuyla kapatılan
reload ailesinin kalan parçası; canlı 'saved and reloaded' SPK desenine
gerçek modülü olan her sınıf bağlandı.

**Doğrulama**
- GS derlemesi temiz → **10.791.424 B (18:48)** (+2.048 B, önceki
  10.789.376).
- Yeni exe string taraması: 4/4 canlı log birebir VAR (`[CGameMaster]…`,
  `[CExperienceTable]…`, `[SPK] AddBuff…saved and reloaded`, `[SPK]
  CustomMonsterSkill…saved and reloaded`) + 4/4 komut adı VAR
  (`gamemaster/experiencetable/addbuff/custommonsterskill`).
- CustomShop/ResetChange kanıtı: canlı exe'de sınıf logları VAR ama
  bizim+donor kaynak ağacında sınıf yok (grep: 0 dosya).

**Commit** — `df6a136c1` · **Tamamlandı** — 01.10.2026 18:49

## [26.10.01 14:31] 2c.1-B2 (ısınma) — EventGvG modülü: ServerInfo 7 anahtarı + CGvGEvent iskeleti

**Ne yapıldı** (kullanıcı isteği: "EventGvG modülünü canlı kanıttan yaz:
ServerInfo'da 7 EventGvG anahtarını oku, CGvGEvent iskeletini kur, derle +
CHANGELOG + commit")
- **Canlı kanıt (giriş):** GameServer.exe'de7 anahtar string'i tek tek doğrulandı
  (`EventGvGSwitch/Npc/NpcMap/NpcX/NpcY/MinUsers/MaxUsers` ×1); canlı config
  dosyalarında GvG izi YOK → varsayılan değerlerle çalışma (default 0/0/0/0/0/2/20).
- **[ServerInfo.h](../Source/4.GameServer/GameServer/ServerInfo.h):** 7 member
  (`m_GvGEventSwitch/NPC/NPCMap/NPCX/NPCY/MinUsers/MaxUsers` — donor adlandırma).
- **[ServerInfo.cpp](../Source/4.GameServer/GameServer/ServerInfo.cpp):**
  `ReadEventInfo(section,path)` içine TvT bloğunun ardına7 okuma — donor deseni
  birebir; donor koruması `(PROTECT_STATE==1 && GAMESERVER_CLIENTE_UPDATE>=14)`
  bizde daima TRUE (1 ve 17) → koşulsuz yazıldı. `ReadEventInfo()` sonuna
  `gGvGEvent.Init()` + include.
- **[EventGvG.h](../Source/4.GameServer/GameServer/EventGvG.h):** donor'dan
  birebir cp (struct GVG_EVENT_USER/GUILD/TIME + 5-durumlu state machine
  yüzeyi + `extern gGvGEvent`).
- **[EventGvG.cpp](../Source/4.GameServer/GameServer/EventGvG.cpp) (YENİ):**
  iskelet — ctor/Init (tüm alan reset)/Clear (guild+user Reset döngisi)/
  `MainProc` → state switch → 5 `ProcState_*` geçiş gövdesi + SetState/GetState.
  `Load` ve geniş metot yüzeyi (Dialog/AddUser/CalcRank/StartGvG …) gövesiz
  duruyor — tam uygulama 2c.1-B2'de donor 1138-satır kod canlı kanıtla
  denetlenerek alınacak (docs/12 §4 "donör yanlış kaynak" uyarısı korunuyor).
- **Kancalar:** [User.cpp](../Source/4.GameServer/GameServer/User.cpp)
  `gObjEventRunProc()` → `gGvGEvent.MainProc()` (gReiDoMU yanına); vcxproj'e
  ClCompile/ClInclude eklendi.

**Doğrulama** — GS derlemesi temiz → **10.789.376 B (14:31)**. Exe string
taraması: **7/7 anahtar VAR** (canlıyla aynı set); PDB: `EventGvG.cpp` kaynak
×3 + `ProcState_START` ×74 + `gGvGEvent` ×5; User.obj'te kanca referansı ✓.

**Commit** — `3d0a8e6eb` · **Tamamlandı** — 01.10.2026 14:31

---

## [26.10.01 14:05] 2c.1 — 60 modül oyun akışı etkisine göre önceliğe dizildi (iş emri docs/13)

**Ne yapıldı**
- Kullanıcı isteği: "docs/05 + docs/12'deki 60 modülü oyun akışı etkisine göre
  önceliğe diz (core gameplay > event > QoL > anticheat > kosmetik), saat
  damgalı iş emri yaz ve commit'le."
- **[docs/13-2C1-ONCELIK-IS-EMRI.md](13-2C1-ONCELIK-IS-EMRI.md) yazıldı**
  (01.10.2026 14:05): docs/05 §3'ün 59 modülü + #60 EventGvG + docs/12 §6
  açık kalemleri yeniden sınıflandırıldı:
  - **A Çekirdek Oyun Akışı = 24** (21 iş + 4 ✅ 2b.0: SkillDamage/AddBuff/
    AutoHp/JewelBank; içinde Harmony → MonsterSkill → SetPro → gelişim
    ailesi → ResetChange → MocNap+MasterReset → GuildUpgrade → Quest →
    Socket×2)
  - **B Event = 9** (EventMainManager iskeleti → EventGvG (#60, canlı 7
    anahtar) → ActiveInvasions → Castle/ThanMa/Sky/GreatPK/DameBoss → ReiDoMU)
  - **C QoL = 21** (ItemTrader/E-11 + CongHuong/E-02 fark-kapatma → shop →
    botlar → ranking/UI → MessLang/ToolKit)
  - **D Anticheat = 3** (ResetLimiter, PassLock, ChangePass)
  - **E Kosmetik = 3** (CustomNameColor, LogErrorForm, GetLicenseID)
  - **Toplam 24+9+21+3+3 = 60 ✓**
- Kapsam dışı bırakılanlar belgelendi: C grubu 19 OFF (docs/12 §6.4),
  ThuongDanhBoss parite-dışı. Dalga planı §8'de (A→B→C→D/E).
- docs/05 §3'e "2c.1 yeniden dizimi" referansı + docs/00 "Sıradaki: 2c.1-A
  (Harmony)" notu güncellendi.

**Doğrulama** — Sayım60/60; P1/P2/P3 toplamı (19+28+12) + #60 korundu;
hangi modülün hangi sınıfta olduğu tek tek gerekçelendirildi (tablolar).
Kod değişikliği YOK → derleme etkilenmez (son: 10.786.304 B, 13:47).

**Commit** — `5e490d79e` · **Tamamlandı** — 01.10.2026 14:05

---

## [26.10.01 13:50] E-kalemleri kapatıldı — E-01/E-02/E-04/E-06/E-09/E-11 docs/09 stratejilerine göre işlendi

**Ne yapıldı** (kullanıcı isteği: "Kalan E-kalemlerini docs/09 stratejilerine göre işle")
- **E-06 CustomEventTime** (önceki dalda): donor cpp + header alındı; 11 event-yazıcı
  dosya (ChaosCastle, DevilSquare, EventTvT, IllusionTemple, CustomQuiz,
  CustomEventDrop, CustomOnlineLottery, CustomArena, InvasionManager, ReiDoMU,
  BloodCastle) CEventName hattına geçti; ServerDisplayer painter'ları donor hattında.
- **E-02 BotAlchemist:** donor taban (933 satır) + canlı `WcoinC/WcoinP` kanıtıyla
  Coin1/Coin2 alanları (bizim PCPoints hattı kaldırıldı); canlı `BotAlchemist data
  load error %s` koruması 3 hata yoluna eklendi; canlı CongHuong.txt deploy'a
  kopyalandı; header donör struct'ına geçirildi + `class CItem;` ileri-bildirim.
- **E-04 ChangeClass:** canlı exe `SPK\ChangeClass.xml` VAR / `ChangeClass.ini`
  YOK → pugixml LoadXML (Enable/Coin + 3 Msg), ini hattı kaldırıldı;
  ClearMasterChangeClass adımı çıkarıldı; geçersiz ClassNum koruması (Msg Index=2);
  canlı `[ChangeClass] Config Saved & Reloaded` logu eklendi; `[CommandChangeClass]`
  logu zaten bizdeydi (CommandManager:3228, canlı formatıyla birebir).
- **E-09 OfflineMode (somut kısım):** canlıda hiç bulunmayan 7 notice yorumlandı
  (`[Helper] OfflineMode Disable` + 6× `[OfflineMode] Don't...` — kısıt return'leri
  korundu); `OnlineRewardOfflineSystems` config anahtarı eklendi (canlı exe kanıtlı,
  donor deseni; kullanım OnlineReward sistemiyle 2c). Donor taban farkları (RenderAttack
  distance6→8/flagwalk, PickUP 153 satır, Start close-mekanizması GCCloseClientSend)
  **2c'ye** — gerekçe: IsFakeOnline (12×) + InSafeZone + RenderAutoPote/CG_OFFMODE
  eklerimiz donor'da yok, donor taban bunları silerdi; davranışı canlı reverse belirler.
- **E-11 ThuMuaDoExc:** Read XML'den **canlı TXT'ye çevrildi** (donör taban + canlı
  3-bölüm şeması 0=NPC/1=Allow/2=Reward; donor'daki ChangeColorName/Body/GP
  kolonları canlıda olmadığı için atlandı; hata stringi `ThuMuaDoExc error: Index %d
  out of range!` canlı birebir); donor **Alchemy** algoritması + TradeOk bağlantısı
  (XuLyItemThuMua katmanı korundu — pasif); `GP` alanı eklendi (0 okunur);
  4 log sessizleştirildi (canlıda `[BotThuMua]`/`[ThuMuaEx]`/`[BotThuMuaDoExc]` YOK);
  canlı ThuMuaDoExc.txt (554 B) deploy'a kopyalandı.
- **E-01 BossGuild (somut kısım):** canlı exe `Event\BossGuild.xml` (duz klasor)
  → ServerInfo yolu buna çevrildi (eski `Event\BossGuild\BossGuild.xml` canlıda YOK);
  deploy dosyası canlıyla birebir doğrulandı (2739 B, CR hariç). HandleBossKill/skor/
  ödül yeniden yazımı **2c'de** (docs/09 §4 zaten 2c diyor).
- **LTCG notu:** `BotAlchemist data load error` literal'i obj'te/iobj'de/PDB'de var
  ama exe'de görünmüyor — 2b.2-N'de Move/Skill reload loglarında görülen aynı
  artımlı-LTCG string artefaktı (kod sağlam, exe-string kanıtı obj düzeyinde).

**Doğrulama** — GS derlemesi temiz (full rebuild 12:53 + artırımlar) → son exe
**10.786.304 B (13:47)**. Exe string testleri: `SPK\ChangeClass.xml` ✓,
`[ChangeClass] Config Saved & Reloaded` ✓, `ThuMuaDoExc error: Index %d out of
range!` ✓, `OnlineRewardOfflineSystems` ✓, `Event\BossGuild.xml` ✓,
`[OfflineMode] Disable in...` → 0 ✓ (sessizlik). docs/09 matris 6 satır güncellendi.

**Commit** — `acd5c642b` · **Tamamlandı** — 01.10.2026 13:50

---

## [26.10.01 12:44] Disk temizliği — C: diskinde boş alan 1.6 GB → 3.7 GB

**Ne yapıldı**
- Kullanıcı talebi ("gereksiz dosyaları sil, disk temizliği yap"). Silinenler
  (tümü takipsiz/geçici; tracked dosyalara dokunulmadı):
  - `BuildLog/` derleme objeleri + logları ≈ 419 MB (`BuildLog/envanter/` kanıt
    dosyaları korundu — 3 tracked dosya); sonraki GS derlemesi full rebuild olur.
  - `Source/Util/cryptopp/` takipsiz derleme çıktıları (`Release/` 93 MB,
    `DLL_Release/` 85 MB, `cryptest.sdf` 37 MB, `CTRelease/`, `dlltest___`)
    ≈ 250 MB; tracked manifest/dll/lib `git checkout` ile geri yüklendi.
  - `%LOCALAPPDATA%\Temp` 436→8 MB, `Windows\Temp` 35→1 MB, `/tmp` geçici
    exe kopyaları + Cline installer ≈ 140 MB.
  - `git gc` (loose objects) ≈ 40 MB (`.git` 764→724 MB).
  - **`ClientBuild_192.168.99.200` (1 GB, 17.458 tracked dosya)** — kullanıcı
    onayıyla `git rm -r` ile kaldırıldı.
- Boost takipsiz kısmı (26 MB) bilinçli olarak dokunulmadı (kaynak ağacın
  parçası); `docs/`, `Dashboard/`, WIP E-kalem değişiklikleri etkilenmedi.

**Doğrulama** — `df -h /c`: 1.6 GB → **3.7 GB boş** (%98 → %94); `git status`
da silme commit'i dışında WIP olduğu gibi duruyor; commit pathspec ile ayrık.

**Commit** — `15d1bdbe4` · **Tamamlandı** — 01.10.2026 12:44

---

## [26.10.01 09:25] 2b.2-N ek dalga — 'configuration reloaded' ailesi 6 modüle genişletildi (Gate/Move/MoveSummon/Notice/ResetTable/Skill)

**Ne yapıldı**
- Kullanıcı istği: E-05'te kurulmuş reload desenini canlı kanıtlı ailenin
  tamamına uygula. Canlı exe string ailesi (c.s): `[CGate] Gate configuration
  reloaded.`, `[CMove]`, `[CMoveSummon]`, `[CNotice]`, `[CResetTable]`,
  `[CSkillManager]`, `[CGameMaster]`, `[CExperienceTable]`, `[SPK] ...` —
  canlı log deseni **`[CSınıf] Modül configuration reloaded.`** (prefix + nokta).
- **6 modüle m_Path[256] + Reload altyapısı** (E-05 deseni: yolu Load'ta sakla;
  Reload = boş-yol koruması → yedek → Load → boşsa geri yükle → canlı-format
  başarı logu): Gate (map), Move (map), MoveSummon (vector), Notice (array +
  `m_count` sıfırlandığı için boş-koruma `GetCount()` ile), SkillManager (map),
  ResetTable (XML/pugixml — üç veri bloğu yedeği; ilk denemede txt-ReloadTxt
  `#else` dalına gitmişti, derleme C2039 ile yakalandı ve aktif
  `CB_AUTORESETINFO` dalına taşındı).
- **CommandManager `/reload` dalları:** `gate`, `movesummon`, `notice`,
  `resettable` eklendi (Gate.h include dahil). **Önemli bulgu:** `move` ve
  `skill` adında mevcut dallar zaten vardı (ServerInfo zinciri:
  ReadMoveInfo → Gate/Move/CustomMove/MoveSummon/RespawnLocation;
  ReadSkillInfo → Skill.ini/MasterSkillTree/SkillHitBox/SkillManager/
  SkillDamage). Bu dallar KORUNDU — bizim tabanın çoklu-config reload
  davranışı (CustomMove/RespawnLocation/SkillDamage dahil) kaybolmasın; zincir
  içi Load'lar artık m_Path sakladığı için Move/Skill de hot-reload edilmiş
  olur. Modüllerin Reload metotları kaynakta canlı-format loglarıyla durur
  (referanssız olduğu için linker /OPT:REF ayıklar — zararsız).
- **Canlı string birebiri:** E-05'in sade `CustomBuyVip configuration
  reloaded` logu canlı haliyle `[SPK] CustomBuyVip configuration reloaded."
  olarak yükseltildi; 6 modülün başarı logları `[CSınıf] ... .` formatına
  alındı. Not: bizim Log.h'ta LOG_BLUE/LOG_RED yok (renk sabitleri
  ServerDisplayer.h zincirinden geliyor) — include gerektirmedi.
- **Derleme yolunda yakalananlar:** ilk denemede 3 header düzenlemesinde
  newString bağlam hatası (ExportXML/GetInfo/SetInfo declare'ları yanlışlıkla
  silinmiş — C2039 ile yakalandı, geri eklendi); Notice.h m_Path eksik;
  Gate/Move/Skill/Notice/MoveSummon/ResetTable cpp'lerine Log.h include'u.

**Neden** — Canlı 'configuration reloaded' ailesi (2b.2-E'de kanıtlanmış
reload deseni) GameMaster/ExperienceTable hariç çekirdek 6 modüle uygulandı;
/resize komut ailesi canlı operatör akışının parçası.

**Doğrulama**
- GS Rebuild temiz → **10.784.256 B** (09:22; +8.704 B önceki 10.775.552'ye) —
  iki tur C2039 hata döngüsü derleme aşamasında yakalanıp düzeltildi.
- Yeni exe string taraması: `[CGate]/[CMoveSummon]/[CNotice]/[CResetTable]/
  [SPK] CustomBuyVip ... configuration reloaded.` 5/5 canlı-birebir VAR.
- `/reload move|skill` mevcut zincirleri korundu (davranış değişmedi; Load'lar
  m_Path saklar).

**Commit** — `97878b551` · **Tamamlandı** — 01.10.2026 09:25 (commit 09:28)

## [26.10.01 07:40] 2b.2-M ek dalga — MapManager zinciri donor'dan alındı, CustomPick tam donör oldu (pano önerisi)

**Ne yapıldı**
- Pano önerisi "MapManager zincirini al (CustomPick'i açar)" uygulandı — dalga 1'de
  gerekçeli korunan MapManager bağımlılığı kapatıldı.
- **MapManager.h:** donor birebir alındı — struct'a `CustomStore/CustomPick/
  PkDropItem/DeathGate/AllowTradeSafe` eklendi (`DisableCustomAttack` →
  `CustomAttack` ad değişimi), 6 getter + `CheckMap` declare. Bizim SPK
  `GetMapNonPK(index,obj,target)` overload declare'ı korundu; donor header'da
  LPOBJ bilinmediği için `#include "User.h"` SPK notuyla geri geldi (ilk
  derleme C2061 yakaladı).
- **MapManager.cpp:** donor birebir alındı — getter gövdeleri
  (GetMapCustomAttack/Store/Pick/PkDropItem/DeathGate(default 17)/
  AllowTradeSafe/PartyEnable) + CheckMap. İki SPK adaptasyonu:
  1. SPK `GetMapNonPK(index,LPOBJ,LPOBJ)` gövdesi (gPKFree PK-zona zinciri,
     Attack.cpp ×4 + GensSystem.cpp ×1 çağrıcı) HEAD'den çıkarılıp donor
     cpp'e geri eklendi + `#include "CustomPKFree.h"`.
  2. **Load canlı 16-kolon sırasına adapte edildi:** canlı
     `Sub-1\Data\MapManager.txt` başlığı `… CustAtt CustStore CustPick PkDrop
     Trade DeathGate "Name"`; donor sırası `…PkDrop DeathGate AllowTrade
     AllowTradeSafe PartyEnable` ile desync olurdu → Load: CustAtt/Store/
     Pick/PkDrop → AllowTrade → DeathGate; `AllowTradeSafe=0`, `PartyEnable=1`
     sabit (canlıda kolon yok). Canlı NonPK kolonu `*` = −1
     (MemScript.cpp:196 GetTokenNumber) → `==-1 ? gServerInfo.m_NonPK` global
     zinciri aynen çalışır.
- **CustomPick.cpp:** donor birebir alındı — `OnPickClose` refactor (pickup
  sıfırlama tekrarı tek metoda), `TradeDuel` kontrolleri,
  `GetMapCustomPick` harita gate'i. Tek SPK rename: 2× `GlobalText(36)` →
  `GetMessage(36)` (canlı exe kanıtı; 754/753/752/659 donor haliyle —
  Message.h:20 alias zararsız).
- **CustomAttack.cpp:81 + OfflineMode.cpp:71:** `GetMapDisableCustomAttack` →
  `GetMapCustomAttack` (CustAtt kolonunun canlı donor adı; ilk derlemede
  yakalandı).
- **Deploy config:** canlı `MapManager.txt` (16 kolon, 77 harita satırı,
  `end` markırlı) `MuServer\4.GameServer\Data\` ağacına kopyalandı — eski
  14 kolonlu dosya yeni Load ile desync olurdu.

**Neden** — Canlı map obj kanıtı (CustomPick.obj ServerInfo-anahtarlı, 2b.3
öncesi tarama) + canlı config kolonları (CustAtt/CustStore/CustPick/PkDrop/
Trade/DeathGate — 2a.3) zincirin canlıda VAR olduğunu gösteriyordu;
CustomPick'in donör sürümü bu zincire bağlı.

**Doğrulama**
- GS derlemesi temiz (Release_EX603|Win32, v143) → **10.775.552 B** (LTCG
  aynı boyut; 2b.2-G finaliyle eşit) — 07:38.
- PDB sembol doğrulaması: GetMapCustomPick ×2, GetMapCustomAttack ×2,
  GetMapCustomStore / GetMapPkDropItem / GetMapDeathGate /
  GetMapAllowTradeSafe ×1, OnPickClose ×2, GetMapNonPK ×8 (SPK zinciri
  derlemede canlı).

**Commit** — `4a0fb8fcb` · **Tamamlandı** — 01.10.2026 07:40 (commit 07:45)

## [26.10.01 07:00] Faz 2b.3 — 68 MUIG-özel modülün canlı envanterle çapraz kontrolü (docs/12)

**Ne yapıldı**
- Kullanıcı istği: "2b.3'ü başlat: 68 MUIG-özel modülün canlı envanterle çapraz
  kontrolünü yap, saat damgalı rapor yaz."
- **Sayım düzeltmesi:** "68" etiketinin kaynağı
  `analiz\SPK-KAYNAK-ANALIZ-20260930.md:268` (MUIG 562 ↔ SPK 564 dosya).
  Kesin sayım: donör GS kökünde **62 donor-özel dosya** (29 cpp + 33 h);
  2b.0'da alınmış 4 modülün 8 dosyası (bizde `SPK\` altında: AUTOHP,
  CustomJewelBank, PC_AddBuff, SkillDamage) düşülünce net inceleme
  **29 cpp modülü / 54 dosya**. Yeni doğrulama: donör cpp 277 = canlı PDB cpp
  277 birebir.
- **Yöntem (6 kanıt hattı):** donor vcxproj ClCompile kaydı; donor sınıf
  yüzeyi; canlı `GameServer.map` (sembol + 606 obj adı →
  `canli_obj_listesi.txt`); canlı PDB cpp yolları (277); canlı exe string dump
  (`c.s`) ↔ bizim (`b.s`); canlı `Sub-1\Data` config + `SPK\` log klasörü.
- **Ham çıktılar:** `BuildLog\envanter\{muig68_donor_only.txt,
  muig68_capraz_kontrol.csv, muig68_kanit_detay.txt, canli_obj_listesi.txt}`;
  rapor: **docs/12** (tarama 06:27–06:36, rapor 06:56).
- **A grubu — 7 modül canlıda VAR, parite tamam:** AUTOHP→`SPK_AutoHp` +
  PC_AddBuff→`SPK_AddBuff` + CustomJewelBank→`SPK\CustomJewelBank.cpp` +
  SkillDamage→kök `SkillDamage.cpp` (4'ü 2b.0 ✅); **B_MocNap**→canlı
  `SPK\B_MocNap.cpp` + `LOG_MOC_NAP` aktif (bizde paralel MocNap.cpp +
  CB_AutoNapGame.cpp/h — 2c birleştirme kalemi); **BotTrade**→canlı
  `SPK\BotTradeMix.cpp` + `LOG_OUT_TRADEBOT` (bizde BotTrader.cpp/h +
  ThuMuaDoExc.cpp/h kapsam); **SkillDamageConfig**→donor SkillDamage rate
  katmanı (bizim SPK\SkillDamage.cpp satır paraleli: 25/66/68/108/112).
- **B grubu — 3 modül canlıda işlev VAR ama donor kodu yanlış kaynak (→2c):**
  **EventGvG — YENİ KEŞİF:** canlı ServerInfo.obj'te 7 config anahtarı
  (EventGvGSwitch/Npc/NpcMap/NpcX/NpcY/MinUsers/MaxUsers) — bizim
  ServerInfo.cpp'te 0 vuruş → 05 listesine sonradan gelen 60. kalem;
  **AntiSkillDelay** — canlı işlevi `SkillManager::CheckSkillDelay` içinde
  (bizde VAR; donor dosyası çöp); **ThuongDanhBoss** — donorde derleniyor ama
  canlıda SIFIR iz → parite kapsamı dışı.
- **C grubu — 19 modül canlıda YOK (OFF/taşınmaz):** APIGameGuard,
  CGMHardwareId, CGMEarringManager, CGMFlagNatManager, CGMPetManager,
  CMixGoblinExpansion, CharacterAdvance, ChatManager, ConsoleDebug,
  CustomExchangeCoin, EventFindPath, GMHolyItem, LogToFile, MasterResetTable,
  MultiLanguage, MyTimer, SendMessage, WindowsConsole, BlackList(donör
  CBlackList). Kritik netleştirmeler: canlı `Data\BlackList.txt` kanıtı
  `CIpManager::AddBlacklist/IsBlacklisted`'e ait (bizde VAR) — donör
  CBlackList değil (formatı BlackList_Block.txt, farklı); CGMHardwareId'nin
  "HardwareId" vuruşı ServerInfo.obj'teki `CustomerHardwareId` stringi (bizde
  zaten var); canlı `DGCommandMasterResetRecv` ile donor MasterResetTable
  bağıntısı 2c'de çözülecek.

**Neden** — 2b.3, Faz 2b'nin son adımı: MUIG donor'da olup bizde olmayan
modüllerin canlıda karşılığı kanıta bağlanmalı; canlıda olmayanların pariteye
etkisi olmadığı belgelenir (OFF kararı), canlıda olanların gerçek kaynağı
belirlenir.

**Doğrulama**
- Salt-okunur tarama: canlı sistemde yazma yok; kod değişikliği yok → GS
  derlemesi etkilenmez (son: 10.776.576 B, febde56b1).
- CSV 29 satır + kanıt dökümü üretildi; sınıflandırma 7+3+19=29 tutarlı.
- Yalan-pozitifler tek tek elendi (SendMessage→user32, FindPath→CMapPath,
  Advance→CRT/SkyEvent, ExchangeCoin→LuckyCoin, HardwareId→ServerInfo).

**Commit** — `6b509e2be` · **Tamamlandı** — 01.10.2026 07:00 (commit 07:05)

## [26.10.01 01:04] Pano otomatik başlatma — Task Scheduler görevi (AxionPano)

**Ne yapıldı**
- Kullanıcı istği: makine yeniden başlasa bile pano (port 8096) ayakta kalsın.
- **Görev:** `AxionPano` — Register-ScheduledTask ile kuruldu (admin yetkisi
  mevcuttu): tetik **AtStartup** (+20 sn gecikme), hesap **SYSTEM**
  (ServiceAccount, RunLevel Highest), `RestartCount 999 / RestartInterval 1 dk`
  (çökerse kendini yeniden başlatır), `ExecutionTimeLimit 0` (süresiz),
  batarya koşullarında da çalışır. Aksiyon:
  `powershell -NoProfile -ExecutionPolicy Bypass -WindowStyle Hidden -File
  "C:\Axion Mu Source\Dashboard\server.ps1" -Published 1` (WorkDir: Dashboard).
- **Bulgu + düzeltme:** görev ilk başlatmada `/api/timeline` 500 verdi —
  SYSTEM hesabı için repo "dubious ownership" korumasına takılıyordu (repo
  Administrator'a ait). server.ps1'deki 5 git çağrısının hepsine
  `-c safe.directory=*` eklendi (hem SYSTEM hem kullanıcı oturumunda çalışır).
- Manuel sunucu süreci kapatılıp görev hemen başlatılarak kanıtlandı:
  `/`, `/api/timeline`, `/api/agent`, `/api/status` → **HTTP 200** (SYSTEM
  bağlamından, git verisi dolu).
- Dış erişim ACL'i zaten mevcuttu (http://+:8096/ → Everyone — önceki kuru­lum).

**Neden** — Panonun canlı takip aracı olarak sürekli erişilebilir olması
istemci makinesinin oturum durumundan bağımsız olmalı.

**Doğrulama**
- `Get-ScheduledTask AxionPano` → Ready; Start-ScheduledTask sonrası 4/4
  endpoint 200;
- Sunucu artık kullanıcı oturumuna değil göreve bağlı (oturum kapansa da
  çalışır); kod derlemesi gerektirmez.

**Commit** — (bu kayıtla birlikte) · **Tamamlandı** — 01.10.2026 01:04

## [26.10.01 00:56] E-05 CustomBuyVip reload — canlı SPK 'configuration reloaded' deseni entegre edildi

**Ne yapıldı**
- Kullanıcı tetiklemesi: E-05'in kalan parçası (reload) istendi. Yol geçişi
  dalga 4'te yapılmıştı (`SPK\CustomBuyVip.txt` — ServerInfo:406); bu turda:
- **Canlı kanıt:** canlı exe (6.979.072 B) string taraması →
  `CustomBuyVip configuration reloaded` canlıda VAR, bizim derlemede YOKTU;
  ayrıca canlı reload ailesi görünür oldu (Gate/Move/MoveSummon/Notice/
  ResetTable/Skill/GameMaster/CustomShop/ExperienceTable ... configuration
  reloaded) — 2c'de diğer modüllere uygulanacak desen listesi.
- **CustomBuyVip.cpp/h:** `Reload()` metodu eklendi — Load'ta saklanan yolu
  (`char m_Path[256]`, yeni alan) kullanır; parse gövdesi Load ile aynı
  (canlı şema: Index/Exp+/Drop+/Days/Coin1-3/VipName); hata durumunda mevcut
  verileri korur (Init parse'tan sonra), başarıda
  `LogAdd(LOG_BLUE,"CustomBuyVip configuration reloaded")` (canlı string;
  LOG_BLUE=ServerDisplayer.h:21). Include'lar: Path.h (gPath declare), Log.h.
- **CommandManager.cpp:** `/reload buyvip` dalı eklendi (`gCustomBuyVip.Reload()`;
  include CustomBuyVip.h). İlk derlemede C2065 yakalandı → include eklendi.

**Neden** — docs/09 E-05 stratejisi: 'txt format korunur, reload eklenir'.
Reload, canlı operatörün GS'yi durdurmadan VIP fiyatlarını güncelleyebilmesi.

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.776.576 B** (00:56,
  +1024 B — reload kodu); `CustomBuyVip configuration reloaded` + `buyvip`
  string'leri bizim derlemede artık VAR (canlı parite);
- Zincir: `/reload buyvip` → CommandManager:3446 → CCustomBuyVip::Reload
  (CustomBuyVip.cpp:113) → aynı dosyayı yeniden parse eder.

**Commit** — (bu kayıtla birlikte) · **Tamamlandı** — 01.10.2026 00:56

## [26.10.01 00:47] 2b.2-A G1 denetimi — 25/30 dosya donor-birebir teyit; MoveSummon PkMove tamamlaması

**Ne yapıldı**
- Kullanıcı panodan 2b.2-A'yı yeniden tetikledi; dalga 1-2'de tamamlanmıştı —
  yeniden yapılmadı, yerine **G1'in 30 dosyasının bugünkü durumu tek tek
  denetlendi** (cmp ile donor karşılaştırması):
  - **25 dosya donor-birebir teyit edildi** (Notice, CustomWingMix, CustomTop,
    ArcaBattle, MonsterSkillManager, Shop, MapServerManager, Kanturu×4,
    ImperialGuardian, DoubleGoer, MonsterAI, MoveSummon(→aşağıda), ComboSkill,
    CastleDeep, RaklionSelupan, QuestWorld(+Objective), Raklion, LifeStone,
    Mercenary, Crywolf).
  - **pugixml:** bilinçli (donor sürüm bizim pugixml.h ile uyumsuz — 2b.2-A kararı).
  - **Map:** bilinçli (MAX_MAP=250 + MAP_NEW4/5/BOSS_GUILD — E-01 için korunur).
  - **MossMerchant / BonusManager:** tek fark bizim CEventName.h include yorumu —
    bilinçli, donor-birebir kabulü.
  - **CustomPick:** donor cpp GetMapCustomPick istiyor (2 çağrı) — bizim
    MapManager'da bu metot yok (dalga 1 notundaki 'MapManager.h:49'da mevcut'
    ifadesi hatalıymış); tam alım MapManager zincirini (7 metot + Load
    kolonları) gerektirir. Canlı kanıt güçlü (canlı MapManager.txt'de
    CustPick/CustStore/CustAtt/PkDrop/DeathGate kolonları VAR) → MapManager
    zinciri ayrı kalem olarak işlenecek (öneri kartına eklendi).
- **Bulgu + düzeltme — MoveSummon:** dalga 1'de cpp tam alınmamış; donorün
  PkMove okuma/kontrol satırları (8 satır) bizde eksikti. Canlı kanıt:
  canlı Move\MoveSummon.txt başlığında PkMove kolonu VAR → tamamlama meşru.
  MoveSummon.cpp donor'dan yeniden alındı (h zaten birebirdi) →
  donor-birebir teyit edildi.

**Neden** — Dalga kayıtlarındaki 'alındı' iddialarının gerçekten dosyada
olup olmadığının denetimi; dalga 1'de eksik kalan 1 dosya yakalandı.

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.775.552 B** (00:46,
  boyut değişmedi — /LTCG); MoveSummon.cpp donor-birebir (cmp).

**Commit** — (bu kayıtla birlikte) · **Tamamlandı** — 01.10.2026 00:47

## [26.10.01 00:25] Dokümantasyon — saat damgası standardı: CHANGELOG + pano zaman çizelgesi

**Ne yapıldı**
- Kullanıcı talimatı: yapılan her işin **saat kaçta tamamlandığı** hem panoda hem
  CHANGELOG'da görünmeli.
- **CHANGELOG:** 29 kaydın başlığına saat damgası eklendi — format
  `## [gg.aa.yy HH:mm] Başlık`; tüm Commit satırları `hash · Tamamlandı —
tarih saat` formatına çevrildi (git commit tarihlerinden doğrulandı:
  `git log --format="%h | %ci | %s"`). Bu kayıttan itibaren yeni kayıtlar da
  bu standardı izler.
- **Pano:** server.ps1'e `/api/timeline` endpoint'i eklendi (git log canlı:
  hash|tarih saat|mesaj, son 40 commit); index.html'e **⏱️ Zaman Çizelgesi**
  paneli eklendi (saat · ne bitti · commit; 30 sn'de bir yenilenir). Sohbet
  Akışı ve Sonuç panelleri zaten `ts` damgalı.
- Pano sunucusu yeniden başlatıldı, endpoint test edildi.

**Neden** — İzlenebilirlik: hangi işin ne zaman bittiği, kaynağa bakmadan
(panodan ve CHANGELOG'dan) doğrulanabilmeli.

**Doğrulama** (kod derlemesi gerektirmez)
- `git log` saatleriyle CHANGELOG damgaları birebir eşleşiyor;
- `/api/timeline` HTTP 200 + JSON döndürüyor; pano paneli canlı.

**Commit** — `f7d8b6b7f` · **Tamamlandı** — 01.10.2026 00:40

## [26.10.01 00:00] Faz 2b.2-G — G5 bandı dalga 7: 2 dosya alındı, 17 dosya gerekçeli korundu — 2b.2 TÜM BANTLAR KAPANDI

**Ne yapıldı**
- G5'in 31 dosyasından 12'si önceki dalgalarda kararlıydı (5 kanca dosyası
  ServerInfo/ItemManager/CommandManager/ObjectManager/Attack — 2b.0 koruması;
  Util/ServerInfo dalga 3-4; MonsterSetBase dalga 2; CustomJewel/CustomBuyVip/
  CustomEventTime/BotAlchemist E-kalemleri). Kalan 19 dosya profillendi
  (SPK-imleç + alarm + bizim-tekil metot taraması).
- **Alındı (2):**
  - **JewelMix:** güçlü normalize testi (tüm whitespace silindi) semantik
    özdeşlik gösterdi — tek fark GlobalText (bizim alias'la uyumlu). Donor-birebir.
  - **380ItemType(+h):** donor ExportXML/ExportBMD zinciri TAMAMLANMADI;
    bağımlılıkları 380ItemOption.GetValue (dalga 6) + SafeGetItem (bizim
    Util.h:6) + PackFileEncrypt (dalga 3) — hepsi mevcut. ITEM_ADD_OPTION
    typedef'i donor .h'den geldi (çakışma yok — diğer .h'lerde tanım yok).
- **Gerekçeli korundu (17):**
  - SPK-imleç yoğun: CustomStore(83), Protect(42 SPK+35 alarm),
    ServerDisplayer(52), ResetTable(42), CustomWing(25), CustomMix(19),
    CustomStartItem(10), SkillManager(17), Trade(17), PersonalShop(6).
  - Modül-bağlantılı: DarkSpirit (bizim CustomArena damage-rate entegrasyonu),
    ItemOptionRate (gCustomWing.CheckCustomWingByItem çağrısı bizim-sürümden),
    Party (5 bizim-tekil metot: CGPartyListRecv2/GCPartyLifeSend2/
    GCPartyListSend2/GetLevel/SetLeader), ItemBagManager (bizim DropReward
    entegrasyonu donorde yok).
  - GetLevel zinciri: Move(2), Quest(2) — dalga-3 Gate kararıyla aynı.
  - Diğer: JewelMix dışında mantık-yapısal fark içeren tüm G5 dosyaları bizim
    tabanla kaldı (2c'de canlı-kanıtla değerlendirilecek).

**Neden** — G5 kuralı: iki farklı dal; satır satır çözümleme. SPK-özel
entegrasyonların (CustomArena, CustomWing, DropReward, ResetTable Türkçe
seviye tabloları) donorde karşılığı yok.

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.775.552 B** (23:59), pdb güncel;
- JewelMix/380ItemType donor-birebir (cmp); ITEM_ADD_OPTION çakışması yok.

**2b.2 GENEL ÖZET (7 dalga):** 213 ortak dosyanın tamamı işlendi — 66 dosya
donor'dan alındı (birçoğu adaptasyonla), 147 dosya gerekçeli bizim korundu,
0 dosya belirsiz kaldı. Tüm kararlar canlı-kanıt/bağımlılık-zincir analiziyle
verildi; her dalga derleme + CHANGELOG + commit ile kapatıldı.

**Commit** — `39a28d45e` · **Tamamlandı** — 01.10.2026 00:00

## [26.09.30 23:55] Faz 2b.2-F — G4 bandı dalga 6: 8 dosya alındı, 19 dosya gerekçeli korundu

**Ne yapıldı**
- G4'ün 40 dosyasından 13'ü önceki dalgalarda kararlıydı (8 protokol P0,
  Monster dalga-2, BotBuffer/OfflineMode/ThuMuaDoExc E-kalemleri). Kalan 27
  dosya: alarm taraması (NORMALIZE_LEVEL/GetLevel/gProtect/BlackList/
  HolyItem/vipIndex vb. donor-tekil satırlarda) + bizim-tekil metot + .h enum
  kontrolü.
- **Alındı (donor-birebir, whitespace+güvenli declare):** GameMaster(+h),
  ItemLevel(+h), Log.cpp (**Log.h bizim kaldı — LOG_ANTIFLOOD bizim ek**),
  EventHideAndSeek(+h), ShopManager(+h GetInventory declare), SetItemType(+h
  ITEM_SET_TYPE + Export*), 380ItemOption(+h ExportXML/GetValue),
  ItemBagEx(+h CreateItem declare).
- **Cascade-deny dersi (derleme ile yakalandı):** Helper donor'u donor-
  OfflineMode API'si istedi (Start(aIndex) 1-arg + Unpack) → bizim
  OfflineMode'da yok → **Helper bizim döndü**. QuestReward donor'u
  `gCharacterManager` (DefaultClassInfo ad-değişimi) + donor Quest API
  (GCQuestRewardSend 2-arg) istedi → **QuestReward bizim döndü**. İkisi de
  checkout ile geri alındı.
- **Gerekçeli korundu (19):**
  - Protokol 8: P0 kararı (docs/11).
  - SPK-imleç: ChaosBox(40), CustomAttack(51), GameServer.cpp(33), User(26),
    Viewport(8) — bizim-olan satırlarda SPK modül çağrıları yoğun.
  - NORMALIZE_LEVEL zinciri: BloodCastle(14), DevilSquare(12),
    IllusionTemple(5) alarm'lı — dalga-3 ChaosCastle/Kalima kararıyla aynı.
  - Cascade/bağımlılık: QuestReward (gCharacterManager), Helper (donor
    OfflineMode API), Fruit (vector<int> tablo refactor'u), MonsterManager
    (EventGvG/EventFindPath donorde VAR bizde YOK), Guild.h (**bizim
    BOSS_GUILD TotalScore1 alanı** — E-01 için kritik), ItemBag
    (GetItemNewOption_New bizim-tekil), MasterSkillTree
    (GetMasterLevelExpTlbInfo bizim-tekil), SetItemOption (**MAX_SET_ITEM_OPTION
    254↔100 enum kayması**), SocketItemOption (**Name[32]→[64] struct-layout**).
  - Semantik satır: MapItem — donor DropCreateItem'a `Option1=
    CheckItemSkill(index)` eklemiş (silah-skill drop davranışı) — canlı kanıt
    yok, bizim korunur.
  - Büyük fark: CastleSiege (donor +1913/−1426) alarm'sız ama revizyon
    derin — satır-satır inceleme 2c'ye.
  - E-kalemleri: OfflineMode(E-09), ThuMuaDoExc(E-11), BotBuffer(E-03) —
    dalga-4 kararları.

**Neden** — G4 kuralı: toplu alım yasak; her dosya için "SPK-özellik mi,
eskimelik mi" ayrımı yapıldı; cascade'ler derlemeyle yakalanıp geri alındı.

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.776.064 B** (23:54), pdb güncel;
- 8 cpp donor-birebir (cmp); Helper/QuestReward checkout geri dönüşü doğrulandı;
- Log.h bizim sürümde (LOG_ANTIFLOOD korunmuş).

**Commit** — `470e3ef95` · **Tamamlandı** — 30.09.2026 23:55

## [26.09.30 23:47] Faz 2b.2-D — G3 manuel birleştirme dalga 5: 4 dosya alındı, 6 dosya gerekçeli korundu

**Ne yapıldı**
- G3 havuzu (10 dosya + G2'den itilen 4) dosya dosya işlendi:
- **Alındı (donor-birebir):** CustomDeathMessage(+h, saf girinti farkı —
  normalize-diff 32, hepsi whitespace), EventKillAll(+h, tamamen
  GetMessage→GlobalText).
- **Alındı (1'er satır adaptasyon):**
  - CustomQuest(+h): donor `lpObj->GetLevel()` → bizim `lpObj->Level`
    (2b.2-C Gate adaptasyonu ile aynı gerekçe;
    `// SPK (Faz 2b.2-D)` notu). CustomQuestMonsterQtd alanı bizde VAR
    (User.h/ObjectManager/User.cpp) — donor satırları uyumlu geldi.
  - EventRunAndCatch(+h): donor yakalanan-oyuncu kapısını 1(Lorencia)→17(Devias)
    değiştirmiş — canlı kanıt yok, **bizim değer 1 korundu**
    (`// SPK (Faz 2b.2-D)` notu). Diğer tüm farklar GlobalText.
- **Gerekçeli korundu (bizim taban):**
  - CustomMonster: bizim CB_BXHDMG=1 feature bloğu (ShowBXHDmg okuma +
    gObjMonsterGetTopHitDamageUser çağrısı) donorde YOK — SPK ödül/top-damage
    sistemi korunur.
  - GameMain: bizim include setinde FakeOnline/CustomAttack (G5 modülleri) +
    PROTECT_START/FINAL makroları + Conectar bloğu var; donor 149 satır
    eklemesi bunların içinden geçiyor — alım G5 modüllerini kırar.
  - DefaultClassInfo: donor global adı `gDefaultClassInfo`→`gCharacterManager`
    yapmış; bizim tree'de 4 dosya (ChangeClass/CommandManager/Fruit/
    ObjectManager) eski adı kullanıyor — zincir alımı gerekir, kanıt yok.
  - ItemOption(+h): donor .h'de enum kayması (bizim ADD_SD=128/129 satırları
    silinmiş, sonraki değerler 2 kaymış) — kayıtlı config/istemci numaraları
    bozulur; ExportXML + typedef farkı semantik değil.
  - Warehouse: bizim GDWarehouseGuildOpenRecv/Consult + DS 0x76 hattı bizim
    DSProtocol'le çift (P0-korumalı); donor Close-tekil yapısı uyumsuz.
  - MemScript: 127 whitespace-normalize fark; `*`→-1 kuralı iki tarafta da VAR
    (CustomJewel .txt kararı etkilenmez) — donor alımı nötr ama risksiz değil
    (parser'ı 40+ modül kullanıyor); işaretlendi, G4 taramasında yeniden
    değerlendirilecek.

**Neden** — G3 kuralı: her iki tarafta gerçek gelişme var; yalnız kanıtlı
iyileştirmeler alınır, SPK davranışı korunur.

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.776.064 B** (23:46), pdb güncel;
- CustomDeathMessage/EventKillAll donor-birebir (cmp); CustomQuest/
  EventRunAndCatch donor+1'er satır SPK notlu adaptasyon;
- .h'ler birebir (git değişiklik göstermedi).

**Commit** — `6b96f9ef9` · **Tamamlandı** — 30.09.2026 23:47

## [26.09.30 23:42] Faz 2b.2-E kalemleri dalga 4 — E-05 uygulandı, 7 kalem gerekçeli ertelendi, deploy-config düzeltmesi

**Ne yapıldı**
- **E-05 CustomBuyVip ✅:** ServerInfo:405 `Custom\CustomBuyVip.txt` →
  `SPK\CustomBuyVip.txt` (canlı yolu; 135 B canlı şema:
  Index/Exp+/Drop+/Days/Coin1-3/VipName). Deploy: canlı SPK config'leri
  `MuServer\4.GameServer\Data\SPK\` altına kopyalandı (CustomBuyVip.txt,
  AddBuff.txt, ChangeClass.xml). Ek: bizim AddBuffer (2b.0) yolu
  `Custom\HuyBeo\AddBuff.txt` → `SPK\AddBuff.txt` (canlı yolu).
- **E-08 CustomRankUser ✅ (bizim taban kararlı):** docs/09 stratejisiyle —
  bizim NoticeToAll/RewardSwitch ekstraları canlıya yakın; config anahtarları
  (CustomRankUserSwitch/Type) iki tarafta da mevcut. Kod değişikliği gerekmedi.
- **E-12 ZenDrop ✅:** config-pasifizasyonunda kapatılmıştı (canlı ZenDrop.xml +
  kod aynı) — dokunulmadı.
- **Gerekçeli erteleme (2c'ye, canlı-kanıt şartıyla):**
  - E-03 BotBuffer: canlı `BotBuffer.txt` **YOK** (modül canlıda konfig'siz);
    donor MAX_BOTBUFFERSKILLS=5 vs bizim 33 — config-format uyumsuz.
  - E-04 ChangeClass: canlı XML `Enable=0` (modül kapalı); bizim .ini hattı
    Enable=1 aktif — XML okuma yazımı 2c'de (donörde kaynak yok, sıfırdan).
  - E-06 CustomEventTime: canlı EventTime.xml'i okuyan modül **canlı exe'de
    string'i taşıyor ama iki kaynakta da okuyucu yok** — 2c (canlı exe tersine
    mühendislik) gerekir. Canlı dosya deploy'a kopyalandı (aşağıda).
  - E-01 BossGuild: donorde kaynak yok; 1578 satırlık yeniden yazım. Canlı
    `BossGuild.xml` deploy'a kopyalandı (E-01 için hazır).
  - E-02 BotAlchemist / E-09 OfflineMode / E-11 ThuMuaDoExc: üçlü birleşim
    (donor+bizim+canlı şema) zor kalemler — docs/09 stratejileriyle 2c'de.
- **Deploy-config düzeltmesi:** config-pasifizasyonunda (8515c346a) EventTime.xml
  ve CustomJewel.txt yanlışlıkla **test ağacına** (`Sub 1\Data\`) kopyalanmıştı;
  asıl deploy ağacına (`MuServer\4.GameServer\Data\Event\ + Custom\`)
  eklendi. CustomJewel `Load(.txt)` çağrısı artık gerçek dosyayı buluyor.

**Neden** — E-kalemlerinde canlı kanıt yoksa SPK davranışı değişmemeli; yalnız
yollar/eksik config'ler canlıya eşlendi. Kalan kalemler 2c'de canlı-kanıtla.

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.781.184 B** (23:41, /LTCG
  özdeş boyut — ServerInfo string/yol değişikliği boyut değiştirmedi);
- Deploy: `Data\SPK\{CustomBuyVip.txt,AddBuff.txt,ChangeClass.xml}` +
  `Data\Event\{EventTime.xml,BossGuild.xml}` + `Data\Custom\CustomJewel.txt`;
- E-05 yolu canlıyla birebir (`SPK\CustomBuyVip.txt`).

**Commit** — `2995c53b0` · **Tamamlandı** — 30.09.2026 23:42

## [26.09.30 23:36] Faz 2b.2-C — G2 bandı dalga 3: 7 dosya alındı, 15 dosya gerekçeli korundu

**Ne yapıldı**
- G2'nin 24 dosyası (docs/10 listesi − Reconnect/E-10) dosya dosya diff incelendi.
- **Alındı (donor-birebir):** CrywolfStatue, CrywolfAltar (fark yalnız GlobalText
  — bizim Message.h:20 alias'ıyla uyumlu), ItemDrop(+h), ItemMove(+h), Command
  (saf whitespace). Donor-tekil ExportXML metotları ve typedef'ler değişimle geldi.
- **Alındı (2 satır adaptasyon):** Gate(+h) — donor `lpObj->GetLevel()`
  (OBJECTSTRUCT::GetLevel / m_ServerLeveAddMaster chain'i bizde yok) → bizim
  `lpObj->Level`; donor defaultunda da Level ile aynı (GetLevel: Level +
  MasterLevel*ekle-0*). `// SPK (Faz 2b.2-C)` notuyla işaretli. Gate.h donor
  MAX_GATES=1024 + GATE_ATTRIBUTE + type_map_gate typedef'i aldı (çakışma yok).
- **Util.cpp/h'ye donor util zinciri eklendi:** BuxConvert + GenerateCheckSum2 +
  PackFileEncrypt ×2 (donor Util.cpp:708/718/748/762) — Gate ExportBMD zinciri ve
  G3/G4'teki Export* metotları için ortak bağımlılık. `// SPK (Faz 2b.2-C)` notu.
- **Gerekçeli korundu (bizim taban, 15 dosya):**
  - HackCheck: bizim PROTECT_STATE anahtarları (0xF1/0x1A, 0x77) — SPK güvenlik
    kararı; donor 0x02/gProtect.m_EncMain hattı ALINMADI.
  - ChaosCastle + Kalima: donor NORMALIZE_LEVEL→gServerInfo.ConvertLevel→
    m_MaxLevelCharacter chain'i bizde yok; canlı SPK'nın düz seviye tabloları
    canlı davranış — donor seviye mantığı canlı-kanıtsız.
  - CashShop: donor m_DelayBuyXShop/BuyXShopTickCount/gShopbuyvip bağımlılıkları
    bizde yok + canlı ini kanıtı yok; bizim BuyVipDone entegrasyonu (SPK) korunur.
  - Duel: bizim Fix Dupe + m_DuelArenaAnnounceSwitch korunur; donor StartDuelBit
    kendi zinciriyle (gObjDuelStart vs) — canlı-kanıt yok.
  - EffectManager: donor GetActiveBuffCount donor Viewport zincirine bağlı
    (Viewport bizde eski); bizim GenerateEffectList Viewport'la eşleşiyor.
  - CustomCombo: bizim CheckOneSkillCombo korunur; donor sürümde yok.
  - CustomNpcQuest: donor CustomNpcQuestMonsterQtd OBJECTSTRUCT alanı istiyor
    (bizde yok).
  - MapManager: donor GetMapCustomPick vb. 7 metot bizim MapManager.h'de yok
    (donor Custom* config yapısı farkı) — G3'te tek tek.
  - Warehouse + ItemOption: diff büyük ve içiçe (guild-warehouse, typedef
    dönüşümleri) → G3 manuel birleştirme havuzuna itildi.
  - CustomMove: donor 3-arg GetInfoByName(lpObj,message,Npc) + GetLevel() chain
    istiyor; bizim Protocol.cpp (P0-korumalı) 2-arg çağırıyor.
  - MemScript: 127 whitespace-normalize fark; her iki tarafta da `*`→-1 kuralı
    VAR (bizim CustomJewel .txt kararı etkilenmez) → G3'e.
  - ItemValue + ItemValueTrade: donor 2-arg GetItemMaxStack(index,Level)
    ItemStack'te yok; donor ItemStack Level-bazlı stacking canlı-kanıtsız.

**Neden** — G2 bandı "kontrollü alım" gerektirir; 15 dosyada bağımlılık/canlı-kanıt
eksikliği tespit edildi ve her biri tek tek gerekçelendirildi. Toplu alım yapılmadı.

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.781.184 B** (23:34), pdb güncel;
- 5 dosya donor-birebir (cmp), Gate.cpp donor'a göre yalnız 2 satır fark;
- ItemValue/ItemValueTrade bizim sürüme geri döndü (checkout ile) — doğrulandı;
- Kanca dosyalarında değişiklik yok (dalga 3 dosyalarında kanca yoktu).

**Commit** — `6b6ff23a7` · **Tamamlandı** — 30.09.2026 23:36

## [26.09.30 23:15] Faz 2b.2-P0 — protokol opcode/struct diff tablosu (docs/11)

**Ne yapıldı**
- H-007 dersi gereği donor protokol alımından ÖNCE tablo çıkarıldı →
  docs/11-PROTOCOL-P0-DIFF-TABLOSU.md. Kapsam: Protocol, DSProtocol, CSProtocol,
  JSProtocol, ESProtocol, SocketManager, SocketManagerUdp, Connection, IpManager
  (+ PacketManager referans).
- Bulgular: (1) dış opcode seti Protocol(9)/DSProtocol(4)/CS(1)/JS(1)/ES(1)
  **birebir aynı**; (2) metot yüzeyi farkı 8 dosyada **sıfır** — fark tamamen
  gövdede; (3) bizim-tekil case'ler SPK handler'ları (0x1A LockWindow, 0x39
  CTCMini, 0x75 CB_NewQuest, 0x7B XULY, 0x7C AutoNapThe, 0xEE RuudToken,
  0xFF post-item; DS: 0x40 CustomRanking, 0x7A/7B AutoNapGame), donor-tekil
  case'ler donor özellikleri (0x8/0x9 HolyJOH, 0x71 CGReqtimeState, 0xFB SNS,
  DS 0x41/0x75; 0xF8 donorde zaten comment'li).
- Canlı kanıt: `Sub-1\Data\Event\CTCMini\` (CongThanhChien.xml) bizim
  ServerInfo:503 yoluyla birebir; `Data\BlackList.txt` canlıda var → donor
  SocketManager'ın BlackList/APIGameGuard bağımlılığı gerçek ama modül donor
  GS klasöründe yok → 2c'ye.
- SocketManager.h: bizim buffer sabitleri 5× büyütülmüş (MAX_MAIN_PACKET_SIZE
  40960, MAX_SIDE 81920, WORKER 16) — SPK ölçek kararı, korunur. IpManager:
  bizim sürümde FLOOD IP-ban koruması (LOG_ANTIFLOOD, Log.h:19) — korunur.

**Karar** — Donör protokol dosyası bu fazda ALINMAZ; bizim SPK tabanı korunur.
Donor-tekil struct/handler'ların canlı kanıtı yok (tek tek 2c'de). Connection/
SocketManagerUdp/CSProtocol whitespace-normalize adayı olarak G3'e bırakıldı.
Kod değişikliği YOK (salt analiz + docs) → derleme gerekmez.

**Commit** — `fe4bc2505` · **Tamamlandı** — 30.09.2026 23:15

## [26.09.30 23:04] Faz 2b.2-B dalga 2 — bağımlılık modülleri: 9 dosya + CEventName modülü alındı

**Ne yapıldı**
- 2b.2-A'da ertelenen 8 dosyanın tamamı alındı (bağımlılıklarıyla birlikte):
  - **BonusManager + MossMerchant**: donör alındı; `gEventName.GlobalRemainTime`
    çağrıları için **CEventName modülü donör'dan alındı** (CEventName.cpp 225 satır /
    CEventName.h 84 satır; BONUS_EVENT_TIME=6, MOSS_MERCH_TIME=11,
    MAX_SIZE_EVENT_TEMPLATE=50). GameServer.vcxproj'a eklendi (CrywolfUtil yanı).
    ServerInfo.cpp:471 `gEventName.OpenFile(gPath.GetFullPath("Event\\EventName.xml"))`
    (gBloodCastle.Load ile gBonusManager.Load arası — donör :452 eşdeğeri).
  - **MonsterSetBase h+cpp**: donör std::map tabanlı GetMonsterMap modeli ALINMADI;
    array-tabanlı işlev eşdeğeri yazıldı: `info.index = this->m_count` (SetInfo'da) +
    `GetMonsterMap(int)` (vector<MONSTER_SET_BASE_INFO> döner),
    `GetMonsterMap(int,int)`, `GetMonsterMapAt`, `GetMonsterMapCount`. Donör
    ImperialGuardian/Raklion/RaklionSelupan/Crywolf bu arayüzle derleniyor.
  - **Monster h+cpp**: `gObjMonsterClearExpiredDamage()` eklendi (bizim
    gObjMonsterDelHitDamageUser'ı kullanır; Monster.h:14 declare).
  - **MonsterAI.cpp**: tam donör alındı (ClearExpiredDamage çağrısı uyumlu).
  - **ImperialGuardian.cpp, RaklionSelupan.cpp, Crywolf.cpp**: tam donör alındı
    (vector GetMonsterMap + lpInfo->index kullanıyor; Crywolf G3 bandından erken alındı).
  - **Raklion.cpp**: tam donör alındı — `m_RaklionEvent != 0` guard'ı dahil (bizim
    ServerInfo.cpp:4076'da config zaten vardı: `RaklionEvent` ini anahtarı). Raklion.h
    zaten birebir idi. SPK (Faz 2b) kancası YOK (beklendiği gibi).
- **EventName.xml kararı**: dosya canlı Sub-1\Data\Event'te YOK (donör sunucuda da
  yok). CEventName::OpenFile (CEventName.cpp:31) load hatasında ErrorMessageBox +
  return yapıyor → başlangıçta popup riski. Çözüm: minimal boş `<EventList>` XML'i
  `MuServer\4.GameServer\Data\Event\EventName.xml` olarak oluşturuldu. Struct
  default'ları m_Key=-1/m_RemainTime=-1 olduğundan boş dosya = dosyasız davranış
  (BonusManager/MossMerchant -1 kontrolü atlanır) — birebir eşdeğer.

**Neden** — G1 dosyalarının donör sürümleri 3 modül bağımlılığı istiyordu
(CEventName, GetMonsterMap, ClearExpiredDamage); hepsi karşılanmadan G2'ye
geçilmemesi için dalga 2 olarak alındı.

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.781.696 B** (22:48), pdb güncel;
- CEventName.cpp donor ile birebir; Raklion.cpp donor ile birebir; EventName.xml
  hedef klasörde (`MuServer\4.GameServer\Data\Event\`);
- 8 dosyadaki `// SPK (Faz 2b)` kanca blokları korundu (dokunulan dosyalarda kanca
  yok — grep 0 eşleşme).

**Commit** — `531f984b3` · **Tamamlandı** — 30.09.2026 23:04

## [26.09.30 21:57] Faz 2b.2-A — G1 donör alımı: 19 dosya alındı, 7 ertelendi (bağımlılık)

**Ne yapıldı**
- G1 (%90+ benzer) 30 dosyanın diff'leri satır satır incelendi. Sonuç:
  - **19 dosya temiz alındı:** ArcaBattle, CustomTop, CustomWingMix, DoubleGoer,
    Kanturu×4, CastleDeep, LifeStone, MapServerManager, Mercenary, QuestWorld(+h),
    QuestWorldObjective(+h, GetCount yorumu silindi, CGQuestWorldDetailSend +h),
    Notice(+cpp), pugixml, Raklion→**hayır geri alındı** (aşağıda), RandomManager
    (**kısmi**: donor GetCount'u silmişti; bizim ItemBag.cpp kullanıyor → metot geri
    eklendi), Shop(+h, GetInventory eklendi), MoveSummon→**B'ye** (PkMove alanı
    bizim Move/MoveSummon.h'de yok değil — donor h alındı ama cpp bizim kaldı →
    düzeltme: MoveSummon cpp+h ikisi de **alındı**).
  - **7 dosya ertelendi (2b.2-B):** BonusManager + MossMerchant (donor
    `gEventName.GlobalRemainTime` kullanıyor → bizde CEventName modülü yok),
    Raklion/RaklionSelupan/ImperialGuardian (donor `MonsterSetBase::GetMonsterMap`
    + `MONSTER_SET_BASE_INFO.index` istiyor → bizim MSB'de yok), MonsterAI
    (donor `gObjMonsterClearExpiredDamage` çağırıyor → tanım donor Monster.cpp'de,
    bizim Monster.cpp G5'te), CustomPick (donor `GetMapCustomPick` + OnPickClose
    istiyor → MapManager bizimde yok).
  - **pugixml geri alındı:** donor sürüm bizim pugixml.h ile uyumsuz (data_value/
    append_attribute2 LNK hatası) → bizim kütüphane sürümü kaldı.
  - **Map.h KORUNDU:** donor MAX_MAP=200 + MAP_BOSS_GUILD silik; bizim 250 +
    MAP_NEW4/5/BOSS_GUILD aktif (BossGuild E-01 için gerekli).
  - **NOTICE_PKSYSTEM AÇILMADI:** eMessagePK bizim stdafx:115'te zaten var
    (TypeNoticeCustom); donor Notice.h'daki ikinci tanım C2365 verir. stdafx'e
    açıklama notu eklendi. Notice.cpp diff'i zaten sadece whitespace → alındı.
  - ** GetMessage→GlobalText:** donor dosyalar GlobalText kullanıyor; bizim
    Message.h:20 alias (2b.0) sayesinde uyumlu.

**Neden** — G1 bandı risksiz kabul edilmişti ama diff incelemesi 4 gizli
bağımlılık çıkardı; bunlar 2b.2-B'de bağımlılıklarıyla birlikte alınacak.

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.700.288 B**, pdb güncel;
- 15 dosya donor ile birebir; 6 dosya (ertelenenler + RandomManager kısmi) bizim
  sürümde — beklendiği gibi;
- PDB: CQuestWorld(79)/CShop(49)/CRandomManager(23).

**Commit** — `ab1708d5d` · **Tamamlandı** — 30.09.2026 21:57

## [26.09.30 21:33] Faz 2b.2 / config pasifizasyonu — 6 config eksiği kapatıldı + 2 format düzeltmesi

**Ne yapıldı**
- 09 §5 tablosundaki 6 eksik config canlı Sub-1'den salt-okunur kopyalandı:
  `SPK\ChangeClass.xml`(241), `SPK\CustomBuyVip.txt`(135),
  `Custom\CustomJewel.txt`(5379), `Custom\ZenDrop.xml`(7224, eski 6884 üstüne),
  `Event\EventTime.xml`(2714), `Custom\BotSystem\ThuMuaDoExc.txt`(554).
- **Format düzeltme 1 (E-07):** ServerInfo.cpp:411 `gCustomJewel.LoadXML(.xml)`
  → `gCustomJewel.Load(.txt)` (canlı SPK formatı); LoadXML hattı comment olarak
  korundu. Şema uyumu kanıtı: bizim MemScript parser'ı `*` karakterini -1'e
  çeviriyor (MemScript.cpp:193-202) → canlı txt'deki `*` alanları sorunsuz.
- **Format düzeltme 2 (E-11):** ServerInfo.cpp:833 ReloadBotInfo
  `ThuMuaDoExc.xml` → `.txt` (kanıt: canlı exe'de `Custom\BotSystem\ThuMuaDoExc.txt`
  string'i; donör Read zaten MemScript/txt). Bizim `.xml` kopyası bilgilendirme
  amaçlı kaldı.
- Yol farkları belgelendi (kod henüz okumuyor, entegrasyonda işlenecek):
  canlı `SPK\CustomBuyVip.txt` ↔ bizim `Custom\` (şema aynı);
  canlı `SPK\ChangeClass.xml` (SPK Enable=0) ↔ bizim `Custom\ChangeClass.ini`
  (Enable=1 — davranış farkı E-04'te birleştirilecek);
  canlı `Event\EventTime.xml` (EventMain hattı, SPK root) ↔ bizim
  `Custom\CustomEventTime.xml` (farklı şema) → E-06 kalemi.
  Canlı `Event\BossGuild.xml` 2739 B ↔ bizim `Event\BossGuild\BossGuild.xml`
  4146 B → E-01'de diff işlenecek.

**Neden** — 09 §5'te tespit edilen eksikler; modüllerin config'siz/sessiz
düşmesini önlemek (özellikle CustomJewel ne .txt ne .xml bulabiliyordu).

**Doğrulama**
- GS temiz derlendi (0 error) → GameServer.exe **10.701.824 B** (E-10 sonrası
  10.704.896 → -3072 B, küçülme normal: LoadXML hattı comment'e alındı),
  pdb 17.903.616 B; MuServer'a yazıldı.
- PDB: CCustomJewel(42)/BotThuMuaer(17)/DoiItem(101) sembolleri.
- 6 config dosyası boyut kanıtıyla yerinde (tablo §5 güncellendi).

**Commit** — `8515c346a` · **Tamamlandı** — 30.09.2026 21:33

## [26.09.30 20:48] Faz 2b.2 başlangıcı — E-10 Reconnect entegre edildi (2b.2'nin ilk kalemi)

**Ne yapıldı**
- E-10 diff analizi (bizim 152 ↔ donor 151 satır; %85 benzer): 3 fark bloğu —
  (1) ResumeParty'de parti slot temizliği: bizim Index[1..4] sabit, donor
  `for i=1..MAX_PARTY_USER` döngüsü; (2) AutoResetEnable: donörde doğrudan
  atama, bizimde `m_CommandResetAutoEnable[AccountLevel]` konfig kontrolü;
  (3) donörün ölü yorum temizliği (GCPartyListSend2).
- **Karar:** (1) donor alındı — 10 kişilik partide eski kod Index[5..9] slotlarını
  temizlemiyordu, bayat üye ID kalıyordu (real bug). (2) **bizim korundu** —
  kanıt: bizim canlı konfigte CommandResetAutoEnable_AL0..3 = 1
  (MuServer\...\GameServerInfo - Command.ini:39-42), donör paketinde = 0;
  m_CommandResetAutoEnable hattı bizim SPK ServerInfo'sunda canlı
  (ServerInfo.cpp:2715-2722, ReadCommandInfo → "GameServerInfo - Command.ini").
- `Source\4.GameServer\GameServer\Reconnect.cpp` güncellendi (E-10 işaretli);
  Reconnect.h zaten birebir.

**Neden** — 09 raporu E-10 stratejisi ("donor alımı / elle birleşim") + 10
matrisi G2 bandı; 2b.2'nin en düşük riskli ilk kalem.

**Doğrulama**
- GS `Release_EX603|Win32` **-t:Rebuild** temiz: 0 error → GameServer.exe
  **10.704.896 B** (2b.0 ile özdeş boyut — /LTCG aynı kaynak boyutu etkisi),
  pdb 18.182.144 B; `MuServer\4.GameServer\Sub 1` altına yazıldı.
- PDB: CReconnect(18)/ResumeParty/SetReconnectInfo/ResumeCommand sembolleri.
- 09 raporu E-10 ✅, 02 2b.2 🔄 başlatıldı, pano güncellendi.

**Commit** — `65f71d14b` · **Tamamlandı** — 30.09.2026 20:48

## [26.09.30 20:34] Faz 2b.1 — ortak dosya diff matrisi (docs/10)

**Ne yapıldı**
- 2a.1 envanterindeki 218 ortak dosyadan 5'i donorde yok (BossGuild,
  ChangeClass, CustomPet, FakeOnline, ZenDrop → 09 raporu E-kalemleri); kalan
  **213 gerçek ortak dosya** analiz edildi.
- Script: `BuildLog\envanter\2b1_analiz.sh` → `2b1_diff_matrisi.csv`
  (dosya; satırlar; birebir; benzer%; metot yüzeyi; ham diff).
- Rapor: `docs\10-DIFF-MATRISI.md` — G1..G5 risk grupları + 2b.2 sırası:
  **77 birebir · G1=30 (%90+, temiz donor alımı) · G2=25 (%70-89, kontrollü) ·
  G3=10 (%50-69, manuel birleşim) · G4=40 (%30-49, SPK-özel — toplu alım
  yasak) · G5=31 (<%30, iki farklı dal)**.
- Kritik bulgular: protokol katmanı benzerliği %27-45 (P0 diff tablosu olmadan
  donor alınamaz); 2b.0 kanca dosyaları G4/G5'te (ServerInfo %7, ItemManager %13)
  → alımlarda kancalar korunacak; PacketManager zaten birebir.
- 00/02 güncellendi; dashboard öneri kartları 2b.2-A/P0'a çekildi.

**Neden** — 2b.2'nin hangi dosyaya nasıl davranacağını (alım/birleştirme/
çözümleme) önceden belirlemek; toplu donor alımı riskini elemek.

**Doğrulama**
- CSV 213 satır + başlık; bant sayıları toplamı 213 ile tutarlı.
- Birebir sayısı (77) bağımsız cmp ile doğrulandı.
- Örnek saplama kontrolleri: PacketManager birebir; Reconnect %85 (09/E-10 ile uyumlu).

**Commit** — `acea272af` · **Tamamlandı** — 30.09.2026 20:34

## [26.09.30 19:57] Pano v5 — canlı sohbet akışı paneli

**Ne yapıldı**
- Kullanıcı sorusu: "bu sohbeti canlı olarak oradan görüp son komutları takip
  edebilir miyim?" → sohbetin kendisi panoya akıtmak için yeni panel.
- `Dashboard\data\sohbet.json`: ajan protokolü — **her turun sonunda en üste 1
  kayıt** (kullanıcı isteği + ajanın yaptığı + commit); mevcut oturumun tüm
  turları geriye dönük dolduruldu (14 kayıt).
- `server.ps1`: /api/agent yanıtına sohbet alanı eklendi.
- `index.html`: **💬 Sohbet Akışı** paneli — SEN (turuncu) / 🤖 AJAN (yeşil)
  timeline, son 15 kayıt, 30 sn yenileme.
- Doğrulama: /api/agent sohbet alanı döndürüyor; panelde 14 kayıt render
  ediliyor (Preview ekran görüntüsü onaylı).

**Commit** — `31ddb6a20` · **Tamamlandı** — 30.09.2026 19:57

## [26.09.30 19:46] Pano v4 — ajan köprüsü: panodan komut verme

**Ne yapıldı**
- Kullanıcı isteği: önerileri ve süreci panodan takip edip doğrudan komut vermek.
- `Dashboard\data\`: `oneriler.json` (ajanın önerileri — ajan günceller),
  `komut.json` (kullanıcı komut kuyruğu + işlenen geçmiş), `sonuc.json` (ajanın
  son mesajı), `pin.txt` (6 haneli PIN, **git dışı** tutulacak).
- `server.ps1` yeni uçlar: `GET /api/agent` (öneriler + kuyruk + geçmiş + son
  mesaj), `POST /api/cmd` (PIN doğrulamalı; yanlış PIN 403, metin ≤500 karakter).
- `index.html`: **🤖 Ajan Köprüsü** bölümü — ajan öneri kartları (tıkla → komut
  alanı dolar), PIN + komut girişi, GÖNDER; kuyruk ve geçmiş panoda görünür;
  ajanın son mesajı vurgulu kutuda.
- Akış: kullanıcı panodan komut verir → komut.json kuyruğuna düşer → kullanıcı
  sohbete **"pano"** yazar → ajan kuyruğu okur, işler, sonucu sonuc.json'a yazar
  → pano 30 sn içinde sonucu gösterir.
- Doğrulama: yanlış PIN → 403; doğru PIN → {ok:true}; test komutu kuyruktan
  işlendi, sonuc.json panoya yansıdı (Preview ekran görüntüsü onaylı).
- Uyarı: PIN'in koruması basittir (statik eşleşme); hassas iş için yine de
  sohbet onayı gerekir (request_elevation akışı panoyu bypass etmez).

**Commit** — `60dc4cddd` · **Tamamlandı** — 30.09.2026 19:46

## [26.09.30 19:19] Pano tasarım v3 — misyon kartları + katlanabilir plan

**Ne yapıldı**
- Kullanıcı isteği: daha güzel tasarım + girişte amaç/hedef anlatımı.
- `Dashboard\www\index.html` v3: (1) hero başlık + **3 misyon kartı girişte —
  🎯 Amacımız (canlı SPK'nın birebir kopyası, canlıya dokunulmaz), 🏁 Hedefimiz
  (59 modül + 12 dosya → test → Android+PC → canlı → v2), 🚦 Kuralımız
  (plandan şaşma yok, v2 parite bitmeden açılmaz)**; (2) cam efektli modern
  tasarım (radial-gradient arka plan, renk şeritli kartlar, parlayan ilerleme
  çubuğu); (3) plan grupları **katlanabilir** — tamamlanmış fazlar kapalı,
  aktif faz açık başlıyor; (4) commit listesi timeline görünümü; (5) doküman
  butonları ikonlu grid.
- Doğrulama: misyon 3 kart, 10 plan grubu, 26+2 adım, 9/28 çubuk; Preview
  ekran görüntüleriyle görsel onay (giriş + durum + son işler bölümleri).

**Commit** — `1b019bff6` · **Tamamlandı** — 30.09.2026 19:19

## [26.09.30 19:09] Pano arayüzü v2 — sadeleştirildi, plan 02'den canlı okunuyor

**Ne yapıldı**
- Kullanıcı geri bildirimi: ilk arayüz anlaşılmaz ve plan eksik görünüyordu.
- `Dashboard\www\index.html` yeniden yazıldı: sabit faz özeti yerine **docs/02
  YOL HARİTASI dosyası canlı okunup tüm adımlar (✅/🔄/⬜) tek listede
  gösteriliyor** (Faz 0/1 satırları JS ile ekleniyor). Üstte: "bu proje ne?"
  açıklaması, ilerleme çubuğu (9/28), "Son tamamlanan" ve "Sıradaki" kutuları,
  CHANGELOG'dan son 5 iş, tıklayınca açılan belge okuyucu (Türkçe başlıklarla),
  son 15 commit ve sade derleme tablosu.
- Doğrulama: /api/status + /api/doc/02 canlı; sayfa 28 adımı parse ediyor
  (9/28 tamam), Preview ekran görüntüsüyle görsel onay.

**Commit** — `ba19608be` · **Tamamlandı** — 30.09.2026 19:09

## [26.09.30 18:50] İlerleme panosu — dış IP'den canlı takip aracı (Dashboard\)

**Ne yapıldı**
- `Dashboard\server.ps1`: bağımlılıksız PowerShell 5.1 HttpListener sunucusu
  (makinede php/python/node yok). Uçlar: `/` (UI), `/api/status` (git log -15,
  çalışma ağacı durumu, derleme boyutları, disk, docs listesi), `/api/doc/<ad>.md`
  (docs klasöründen salt-okunur okuma; Türkçe dosya adları dahil), `/api/log/N`,
  `/api/ping`. Yol geçiş koruması: yalnız docs\*.md, `..` reddedilir.
- `Dashboard\www\index.html` (v1): faz panosu + derleme tablosu + commit akışı +
  doküman okuyucu; 30 sn'de bir otomatik yenileme.
- `Dashboard\start-dashboard.cmd`: çift tıkla başlatma (dış erişim modu).
- Firewall kuralı "Axion Mu Pano 8096" (TCP in 8096) + URL ACL `http://+:8096/`
  (Everyone) yönetici onayıyla eklendi.

**Neden** — kullanıcı sunucu ping'i nedeniyle makinede oturamıyor; tüm ilerlemeyi
(faz durumu, commit akışı, dokümanlar, derleme boyutları) dış IP'den tarayıcıyla
takip etmek istiyor.

**Doğrulama (v1)**
- `http://localhost:8096/api/ping` → ok; `http://45.87.120.29:8096/api/ping` → ok
  (dış IP'den erişim çalışıyor; HTTP.sys 0.0.0.0:8096 dinliyor).
- Tüm docs dosyaları (Türkçe İ̇ adlılar dahil) `/api/doc/` ile okunuyor.
- Yol geçiş testi: `/api/doc/..%2F..%2Fserver.ps1` → 404.

**Commit** — `ffd1cb607` + `f8d37a58e` · **Tamamlandı** — 30.09.2026 18:50

## [26.09.30 18:29] Faz 2b.0-E — 12 ezilen dosyanın karşılaştırma raporu (docs/09)

**Ne yapıldı**
- 05 §4 E-01..E-12 dosyaları üçlü analiz edildi: bizim kaynak ↔ MUIG donor ↔
  canlı kanıt (GameServer_canli.map sembolleri, canlı GS exe string taraması,
  gs_strings_canli/bizim.txt, canlı config boyutları).
- Yeni rapor: `docs\09-EZILEN-12-DOSYA-KARSILASTIRMA.md` — envanter matrisi,
  metot yüzeyi farkları, dosya başına strateji (Kolay 3 / Orta 5 / Zor 4),
  uygulama sırası (E-10 Reconnect → E-01 BossGuild), 6 config eksiği.
- 00/02/05 çapraz bağlandı.

**Neden** — 2b'nin "12 ezilen dosya" kaleminin girdisi: hangi dosyada donor
alımı, birleştirme veya canlıdan yeniden yazım yapılacağı karara bağlanmalı.

**Doğrulama** (kod değişikliği yok, analiz kaydı)
- Donörde olmayanlar: BossGuild, ChangeClass, ZenDrop (yalnız bizim + canlı kanıt).
- Normalize satır benzerliği %21 (BotAlchemist) - %85 (Reconnect) aralığı.
- Canlı revizyonun iki kaynağı da aştığı örnekler: BossGuild kill→skor/ödül
  (BONUS_POINT_MONSTER + HandleBossKill, map kanıtlı), BuyVip "configuration
  reloaded", Alchemist "data load error %s", OfflineMode dar log seti.
- Bizim MuServer'da 6 canlı config eksik/eski (ChangeClass.xml, CustomBuyVip.txt,
  CustomJewel.txt, ZenDrop.xml eski, EventTime.xml, ThuMuaDoExc.txt).

**Commit** — `8152b67ee` · **Tamamlandı** — 30.09.2026 18:29

## [26.09.30 18:13] Faz 2b.0 — 4 MUIG donor modülü GS'ye entegre edildi (AddBuffer, CAUTOHP, CCustomJewelBank, CSkillDamage)

**Ne yapıldı**
- Donör 8 dosya `Source\4.GameServer\GameServer\SPK\` altına alındı:
  AUTOHP.cpp/.h, CustomJewelBank.cpp/.h, SkillDamage.cpp/.h, PC_AddBuff.cpp/.h
  (AddBuffer sınıfı PC_AddBuff içinde). GameServer.vcxproj + .filters güncellendi
  (4 ClCompile + 4 ClInclude, Filter=SPK); include yolu için
  AdditionalIncludeDirectories'e `$(ProjectDir)` eklendi.
- Yardımcı eksikler: SPK paket struct'ları (PSBMSG_JEWELBANK*, JEWELFOX_GUI_REQ,
  SDHP_CUSTOM_JEWELBANK_INFO_RECV) donorde CustomJewelBank.h içinde olduğundan
  ayrıca taşınmadı. User.h'e `ItemBank[10]`/`AUTOHP`/`TradeDuel`, Message.h'e
  `GlobalText` alias, ItemManager.h/.cpp'e `CheckItemInventorySpaceCount` eklendi.
- 8 kanca dosyası: User.cpp (gAUTOHP.MainProc), Protocol.cpp (0x35 AUTOHP,
  0xF5/0xF6 JewelBank, 0x79/104 GuiNgocAll, 0xFC/104 AddBuff), ServerInfo.cpp
  (3 Load), Attack.cpp (SkillDamage hasar çarpanı), DSProtocol.cpp (0xF7/0x05
  JewelBankInfoRecv), ObjectManager.cpp (CustomJewelBankInfoSend),
  CommandManager.h/.cpp (COMMAND_ADDBUFF=86 + case).
- Config şablonları canlı Sub-1'den kopyalandı (salt okunur kaynak):
  `Data\SPK\AddBuff.txt` + `Data\Custom\BotSystem\CongHuong.txt`.

**Neden** — 05 envanteri P1'in 4 "MUIG donor" kalemi; canlı GS'de bu 4 modül
var, bizim GS'de yoktu (2a envanterinin 71 iş biriminin ilk 4'ü).

**Doğrulama**
- GS `Release_EX603|Win32` temiz derlendi → GameServer.exe **10.704.896 B**
  (önceki 10.690.560), pdb 18.182.144 B; `MuServer\4.GameServer\Sub 1`'e yazıldı.
- Derleme sırasında 2 hata çözüldü: (1) CommandManager.cpp'de Move.h include'u
  kazara silinmişti (C2065 MOVE_INFO/gMove); (2) 0x35 opcode çakışması → **H-007**:
  Protocol.cpp'de HAISLOTRING CGItemEquipRepairRecv `#if(0)` ile kapatıldı;
  kanıt: bizim SPK istemcisi 0x35 göndermiyor, AutoHP istemcide yerel
  (Protect.m_MainInfo.DelayAutoHP).
- PDB sembolleri: CAUTOHP(14)/CSkillDamage(17)/CCustomJewelBank(18)/AddBuffer(12)
  + gAUTOHP/gAddBuffer/gSkillDamage/gCustomJewelBank + GetSkillDamage/
  CommandAddBuff/JewelBankRecv/GuiNgocAll.

**Commit** — `599d568ef` · **Tamamlandı** — 30.09.2026 18:13

## [26.09.30 14:31] Faz 1 tamamlandı — Main ve GetMainInfo derlemeleri

**Ne yapıldı**
- `Source\5.Main` `"Global Release"|Win32` + v143 ile derlendi →
  `ClientFile\Main.exe` (12.023.808 B, PE32 GUI i386, linker 14.44).
- `Source\6.GetMainInfo` `Release|Win32` ile derlendi →
  `GetMain\GetMainInfo.exe` (3.693.568 B, PE32 Console i386).
- Kaynak düzeltmeleri (derlemeyi engelleyen hatalar):
  1. `Source\5.Main\source\CustomMessage.h` — windows.h `GetMessage` makrosu
     sınıf üyesini bozuyordu → sınıftan önce `#undef GetMessage`, sınıfta
     `GetMessageA` alias'ı korundu (çagrı noktaları makro genişlemesiyle uyumlu).
  2. `Source\5.Main\source\Winmain.cpp` — mesaj döngüsünde `GetMessage` →
     `GetMessageA` (CustomMessage.h makroyu kaldırdığı için).
  3. `Source\5.Main\source\stdafx.h` — (a) `#define NOMINMAX` KALDIRILDI:
     MUIG tabanı windows.h'yi NOMINMAX'sız include eder, ~100 çıplak
     `min()/max()` çağrısı makrolara dayanır; SPK katmanı ise `(std::max)`
     parantezli stilde yazıldığı için makrolarla uyumlu. (b) PC dalına
     `typedef unsigned long long Uint64;` eklendi (Android'de SDL sağlıyor).
     (c) Masaüstü dalına `#include "Platform/MobileTime.h"` eklendi.
  4. `Source\5.Main\source\Platform\MobileTime.h` — Win32 dalı eklendi:
     QPC tabanlı inline `MU_MobilePerfNow/Frequency/ToSeconds/ToMilliseconds/`
     `GetTicks/Sleep/MobileTimeInit`. Paylaşılan dosyalar (ZzzObject, ZzzCharacter,
     ZzzScene, ZzzLodTerrain) mobil perf telemetrisi bunu çağırıyor.
  5. `Source\5.Main\source\ZzzLodTerrain.cpp` — `TERRAIN_ATTRIBUTE` `inline`
     tanımdan dış bağlantılı tanıma çevrildi (donor MUIG'de header'da
     `extern inline` bildirimi vardı; çağıran TU'larda tanım yok, LNK2001).
  6. `Source\5.Main\Main.vcxproj` — `source\ScenePerfTelemetry.cpp` derleme
     listesine eklendi (LNK2001 `g_mainScenePerfSnapshot`).

**Neden** — Faz 1 çıkış kriteri: 6 bileşenin de kaynaktan derlenebilmesi.

**Doğrulama**
- Main.exe vs canlı `ClientBuild_192.168.99.200\Main.exe` (12.003.328 B):
  aynı varyant doğrulandı (CBGetMain.bin + License.json hattı); benzersiz
  string seti 6.189 vs 6.173, ~%99,7 parite; farklar revizyon sapması +
  gömülü IP (`171.235.182.88` vs `192.168.0.150`) — bkz. H-004.
- GetMainInfo: canlı pakette bu araç YOK; 369 KB referans SPK "GetEngine"
  varyantı — parite hedefi değil, Faz 2a.5'te karar (bkz. H-005).
- Analiz çıktıları: `BuildLog\main_strings_*.txt`, `BuildLog\gmi_strings_*.txt`.

**Commit** — `1339a290` · **Etiket** — `faz1-tamamlandi` · **Tamamlandı** — 30.09.2026 14:31

---

## [26.09.30 13:33] Faz 1 — GameServer derlemesi + Resource.h onarımı

**Ne yapıldı**
- `Source\4.GameServer` `Release_EX603|Win32` derlendi →
  `MuServer\4.GameServer\Sub 1\GameServer\GameServer.exe` (10.689.536 B =
  canlı PDB referans boyutu, birebir).
- `Source\4.GameServer\GameServer\Resource.h` onarıldı: UTF-16 hasarlı
  dosyadan 101 orijinal tanım kurtarıldı (`iconv -f UTF-16LE -t UTF-8`);
  eksikler eklendi: IDM_INVASION14-21=161-168, IDM_STARTBSV=169,
  IDM_EVENTS_CTCMINI=170, IDM_EVENTS_BOSSGUILD=171,
  ID_FAKEONLINE_RELOADDATA/ADDFAKEONLINE/DELFAKEONLINE=32800-32802,
  _APS_NEXT_COMMAND_VALUE=32803. (IDC_ARROW bilerek eklenmedi — winuser.h sağlar.)

**Neden** — GS derlemesi Resource.h bozuk olduğu için C2051 veriyordu.

**Doğrulama** — GS derlemesi hatasız; PE32 GUI i386.

**Commit** — `eb87f89` (Resource.h) · `c28026f` (GameServer.exe + pdb) · **Tamamlandı** — 30.09.2026 13:33

---

## [26.09.30 13:34] Faz 1 — CS / DS / JS derlemeleri

**Ne yapıldı**
- ConnectServer, DataServer, JoinServer `Release_EX603|Win32` + v143 ile
  derlendi → `MuServer\1.ConnectServer\ConnectServer.exe` (103.936 B),
  `MuServer\2.DataServer\DataServer.exe` (1.030.656 B),
  `MuServer\3.JoinServer\JoinServer.exe` (943.616 B).
- CS/DS/JS vcxproj'larına eksik `<PlatformToolset>v143` satırları eklendi.

**Neden** — Faz 1 hattı doğrulaması (ilk zafer CS ile).

**Doğrulama** — Boyutlar canlı PDB referanslarıyla birebir aynı.

**Commit** — `eb87f899`/`c28026fa8` paketi · **Tamamlandı** — 30.09.2026 13:34

---

## [26.09.30 13:20] Proje taşıma + repo kurulumu (Faz 0 bitişi)

**Ne yapıldı**
- Proje `C:\Axion Mu Mobile\New Source Code\Axion Mu Source\` →
  `C:\Axion Mu Source\` taşındı (4,1 GB, ~101.000 dosya); eski klasör silindi.
- Git repo kuruldu (main): `f530df5` ilk commit (97.114 dosya),
  `dd2fb78` ClientBuild dışlama (.gitignore).
- VS 2022 Community kuruldu: v143 + Win SDK 10.0.22621 + ATL
  (MFC gerekmez — hiçbir proje UseOfMfc=true değil).

**Neden** — Tek ve kalıcı proje kökü + sürüm takibi.

**Not** — Eski analiz raporları `C:\Axion Mu Mobile\analiz\` altındadır;
güncel dokümantasyon `docs\` klasöründedir.

---

## ŞABLON (yeni kayıt için kopyala)

```
## [YY.AA.GG] Başlık

**Ne yapıldı**
- ...

**Neden** — ...

**Doğrulama**
- ...

**Commit** — ...
```

---

## [26.09.30 16:00] Faz 2a.2-2a.4 — Canlı sistem envanteri (salt okunur tarama)

**Ne yapıldı**
- **2a.2:** Canlı `GameServer.map` (1,8 MB) `BuildLog\envanter\GameServer_canli.map`
  olarak arşivlendi; map'ten 242 sınıf/namespace çözümlendi
  (`map_siniflar.txt`). Kaynak karşılaştırması: **175 sınıf bizde var (%72),
  67 canlı-özel** → 9 CRT/obfuscation artığı ayıklandı → **58 gerçek eksik
  modül** (önceki tahmin 57 idi): 4'ü MUIG donor'da (AddBuffer, CAUTOHP,
  CCustomJewelBank, CSkillDamage), **54'ü hiçbir kaynak setinde yok (sıfırdan)**.
- **2a.3:** Canlı `Sub-1\Data\` envanteri: 450 dosya (~10 MB); `Data\SPK\`
  25 SPK modül config'i; `GameServer\SPK\` 225 dosya = SPK_ToolKitMain canlı
  logları (modülün canlıda aktif olduğunun kanıtı). Config tanınırlık tablosu:
  22 bizde / 4 MUIG'de / 24 SPK-özel (hiçbir kaynakta yok).
- **2a.4:** Gerçek SPK istemcisi tespit edildi:
  `Client and Tools\Client (eski adı 1Client)\Engine.exe` (9.201.152 B, 19.09.2026) —
  ConnectIP.bmd + ServerData.bmd + Data\SPK + SPK.ini hattı; AUTOHP istemci
  UI varlığı (`Btn_AutoHp.spk`). Bizim 5.Main kaynağında SPK istemci izi
  YOK (CBGetMain/MUIG hattı) → yeni plan adımı **2d.0** (SPK istemci format
  katmanı) açıldı.
- **Kritik keşifler:** (1) `C:\AxionMu\` farklı bir fork (57 modülden hiçbirini
  içermiyor; vcruntime140**d** = debug runtime) — parite hedefi değil.
  (2) Canlı GS v100 (VS2010) toolset'li (msvcp100/msvcr100 kanıtı).
  (3) **H-006 açıldı:** Faz 1 GS "canlıyla birebir" iddiası düzeltildi —
  eşleşme MuServer'daki eski referansla; canlı GS 6.979.072 B.

**Doküman güncellemeleri**
- Yeni: `docs\06-CANLI-SISTEM-ENVANTERI.md` (tüm bulgular + yeniden üretim komutları)
- `02-YOL-HARITASI.md`: 2a.2/2a.3/2a.4 ✅; 2d.0 eklendi
- `00-PROJE-HARITASI.md`: durum panosu güncellendi (aktif görev: 2a.1)
- `03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md`: A-01/A-02 (58 modül, kesin liste),
  A-03 alt kırılım, B-01/B-02 (SPK istemci) kalemleri
- `04-HATA-GUNLUGU.md`: H-006 kaydı

**Operasyon notu (disk)**
- Oturum ortasında C: diski %100 doldu (Temp'te 6,2 GB AI/araç çöpü);
  `cmp_axion-mu-dmncms`, `axion-spot-*`, `axion-probe-*`, Cline updater
  temizlendi → 5,7 GB boşluk açıldı. Yazma hatası nedeniyle 03 dokümanı
  boşalmıştı; yeniden yazıldı ve doğrulandı (5.540 B).

**Doğrulama**
- Ham çıktılar `BuildLog\envanter\` (gs_strings_*, map_siniflar, sinif_*,
  canlida_bizde_yok, muigden_alinabilir, sifirdan_yazilacak)
- `git fsck` temiz; tüm docs boyut kontrolü yapıldı.

**Commit** — `929d39444` · **Tamamlandı** — 30.09.2026 16:00

---

## [26.09.30 16:22] Faz 2a.1 — SPK modül envanteri (iş emri) tamamlandı

**Ne yapıldı**
- Canlı `GameServer.pdb` (27,3 MB) derin analiz edildi:
  - Gömülü linker komut satırı bulundu (`pdb_linker_cmd.txt`): VS2022
    14.44.35207 (**v143** — önceki v100 tahmini düzeltildi), /LTCG,
    SPKThemeManiFest.xml addon'u, ara yapı `D:\BuildMU\Android\ExGameServer\`.
  - Kaynak dosya yolları çıkarıldı: **367 benzersiz cpp** (277'si canlı GS
    projesi; 1'i proje dışı: `Source\Include\Math.cpp`).
  - **Canlı GS projesi 277 cpp; 218'i bizim kaynakla isim paritesinde;
    59 dosya sadece canlıda** (map'in 58 sınıf tahminini 59'a düzeltir:
    SkillDamage.cpp map deseninde yakalanamamış).
- **Canlı SPK mimarisi kesinleşti:** tüm SPK modülleri
  `GameServer\SPK\` alt klasöründe (57 cpp); 12'si bizim kaynağımızda da
  aynı adda var (canlıda bu dosyalar SPK sürümüyle EZİLİYOR: BossGuild,
  BotAlchemist, BotBuffer, ChangeClass, CustomBuyVip, CustomEventTime,
  CustomJewel, CustomRankUser, OfflineMode, Reconnect, ThuMuaDoExc, ZenDrop).
- `docs\05-SPK-MODUL-ENVANTERI.md` yazıldı: 59 modülün kategori tabloları
  (SPK_ çekirdek 20 / SPKViet sistemleri / bot / event / UI / ekonomi),
  canlı config eşlemeleri, P1/P2/P3 öncelik tanımları, MUIG transfer +
  sıfırdan yazım stratejisi, modül başına 4 adımlı doğrulama yöntemi.

**Doküman güncellemeleri**
- Yeni: `docs\05-SPK-MODUL-ENVANTERI.md`
- `02-YOL-HARITASI.md`: 2a.1 ✅ (Faz 2a'da kalan: 2a.5)
- `00-PROJE-HARITASI.md`: aktif görev 2a.5'e çekildi
- `03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md`: A-02 sayısı 59'a güncellendi

**Yeni ham veriler** (`BuildLog\envanter\`): pdb_cpp_yollari.txt (367),
pdb_obj_yollari.txt, pdb_linker_cmd.txt, canli_gs_s.txt (277),
canli_spk_s.txt (57), bizim_gs_s.txt (281), ortak_dosyalar.txt (218),
canli_ek_dosyalar.txt (59), bizim_ek_dosyalar.txt (63).

**Not** — Extractor üç denemede hazırlandı: PowerShell .NET regex'i PDB
string'inde sessiz başarısız oldu; çözüm IndexOf + geriye-yürüme (printable
run) algoritması oldu (pdb_scan.ps1).

**Commit** — `1e67ab22f` · **Tamamlandı** — 30.09.2026 16:22

---

## [26.09.30 16:37] Faz 2a.5 — GetMainInfo varyant kararı: TEK HAT = SPK GETENGINE

**Ne yapıldı (kanıt zinciri)**
1. `GetMain\GetEngine.ini` düz UTF-8 çıktı (önceki oturumda UTF-16 sanılmış;
   mojibake iconv hatalısından): IpAddress=45.87.120.29, IpAddressPort=44405,
   ClientVersion=1.03.34 (= Client\SPK.ini MainCode ile aynı), ClientName=Engine.exe,
   AntPort=58584, MaxAttackSpeed*=67000, DefaultFPS=24, Türkçe yorumlar
   (Axion'un kendi konfig aracı olduğu kesinleşti), MENUBUTTON_* Vietnamca
   SPK modül menü etiketleri, JewelBankTab, MaxLevel seti.
2. `Client\Data\SPK\ConnectIP.bmd` (36 B) formatı çözüldü:
   **XOR 0x20(IP string) + 0x20 pad + 4 B CRC** → decode = 45.87.120.29 =
   GetEngine.ini değeri. GetMainInfo bu dosyayı üretiyor.
3. `ServerData.bmd` (1.089.576 B): aynı gizleme tekniği + veri blokları
   (0xDF dolguları blok sınırlarını işaretliyor).
4. `GetMain\SPK_CRCFILE.ini`: GetMainInfo'nun bütünlük raporu ("Code by
   SuperHung": SPK_*.bmd CRC + FOUND/NOT FOUND listesi) — araç doğrulayıcı.
5. **KARAR (A varyantı):** SPK GetEngine hattı benimsendi; GetMainInfo
   GetEngine davranışına geliştirilecek (2d.1), Main'e SPK format okuma
   katmanı (2d.0) eklenecek; MUIG CBGetMain hattı bayrakla devre dışı.
   H-005 kararlandı (uygulama/kapanış 2d'de).

**Doküman güncellemeleri**
- `03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md`: "2a.5 KARARI" bölümü (kanıt zinciri + 4 maddelik uygulama kararı + B-01 ek kanıtları)
- `04-HATA-GUNLUGU.md`: H-005 → karar alındı
- `02-YOL-HARITASI.md`: 2a.5 ✅ → **Faz 2a TAMAMLANDI**

**Operasyon notu**
- `Client and Tools\Client (eski adı 1Client)\` → `Client and Tools\Client\` olarak yeniden
  adlandırılmış (dış müdahale; içerik aynı). Dokümanlardaki yollar güncel
  konumu ile yazılacak.

**Commit** — `4aedd4736` · **Tamamlandı** — 30.09.2026 16:37

---

## [26.09.30 16:57] 2d.0 önhazırlığı — ConnectIP.bmd / ServerData.bmd format çözümlemesi

**Ne yapıldı**
- Örnek dosyalar `BuildLog\envanter\bmd\` altına arşivlendi (ConnectIP 36 B,
  ServerData 1.089.576 B).
- **ServerData.bmd XOR 0x20 KANITLANDI:** decode sonrası tam okunur —
  `Axion Mu`/`Axion Mu 2..4` (4×32 B sunucu slotu, 0x200+),
  `Engine.exe`/`AxionMu`/`Axion Mu` (0x2A0+), `Photos\Screen(...).jpg`,
  `1.03.34` + `!571Axion@Mobile` (0x4E0), 7×int32=65000 hız limitleri (0x52C),
  float 45.0 (=CameraDefault) + float 240.0, kanat kataloğu (340 B stride:
  Wing200..WingCustom15, SPK\Item\*.tga yolları), item opsiyon tablosu
  (260 B stride: "[+9] Damage: +100" formatı), LEVEL tabloları (100 B stride),
  1.510 printable string. **GetEngine.ini'nin binary aynası.**
- **ConnectIP.bmd:** 12 B IP (XOR 0x20) + 20 B pad + 4 B CRC. Standart CRC32
  ve 8 basit varyant test edildi → EŞLEŞMEDİ (özel tablo/seed). Geçici çözüm
  dokümante edildi: üreticide CRC alanına 0x00 yazılabilir, 2d.0'da
  Engine.exe crc tablosundan çözülecek.
- `docs\07-SPK-BMD-FORMAT.md` yazıldı: alan tabloları + üretici/okuyucu sözde
  kodu + 5 açık kalem (CRC varyantı, opsiyon tuple semantiği, footer, port
  konumu). Arıza notu: PS byte döngüsü 5 dk timeout → openssl AES-ECB hilesi
  (yanlış sonuç, silindi) → perl tr // ile 1 saniyede doğru decode.

**Doküman güncellemeleri**
- Yeni: `docs\07-SPK-BMD-FORMAT.md`
- Ham veri: `BuildLog\envanter\bmd\` (orijinal + decode)
- `02-YOL-HARITASI.md`: 2e.0→2d.0 tutarlılığı

**Commit** — `72f71680e` · **Tamamlandı** — 30.09.2026 16:57

---

## [26.09.30 17:07] Faz 2a.5 (tamamlama) — GetMainInfo tek modül tasarımı (08 dokümanı)

**Ne yapıldı**
- **Zincirin son halkası kanıtlandı:** `GetMain\Data\CustomWing.txt` satırları
  (Wing200-202, ConquerorWing, cape_of_death→KF_Death_clka/clkb, Wing401-405,
  WingCustom1-15, ChristmasW6 ve tga yol çiftleri) ServerData.bmd kanat
  kataloğuyla (340 B kayıtlar) BİREBİR eşleşiyor. → GetMainInfo =
  GetEngine.ini + GetMain\Data\*.txt'i .bmd'ye DERLEYEN araç (kesin).
- **Varyant karşılaştırma tablosu** (08 dokümanı §1): 10 özellikte yan yana —
  girdi/çıktı/format/istemci/kanıt durumu; A (SPK GetEngine) benimsendi,
  MUIG MainInfo bayrakla devre dışı (kaynak korunur).
- **Tek GetMainInfo tasarımı** (08 dokümanı §3): tek exe, iki mod
  (--mode:spk varsayılan / --mode:muig legacy bayraklı); yeni SPK modülü
  (GetEngineConfig, CrcPatch, ConnectIPWriter, ServerDataWriter,
  CrcFileReport, main_spk); şablon-kopya stratejisi (bilinmeyen baytlar canlı
  örnekten aynen); VMP SDK import'u kaldırılacak; 5 adımlı doğrulama
  (boyut, çapraz diff, Engine.exe kabul testi, --check modu).
- **D1-D9 görev kırılımı** ile 2d.1 iş planı hazır.

**Doküman güncellemeleri**
- Yeni: `docs\08-GETMAININFO-TASARIM.md`
- `03-EKSIK-ICERIK-VE-ENTEGRE-LISTESI.md`: B-03 → tasarım tamam/2d.1
- CHANGELOG: bu kayıt

**Commit** — `cf2cb14fd` · **Tamamlandı** — 30.09.2026 17:07

---

## [26.09.30 17:14] Faz 2a.1 (yeniden yazım) — 05 iş emri temiz ve kesin hale getirildi

**Neden**
- İlk yazımda (tek seferde heredoc) tablo ID'lerinde düzeltme izleri ve
  58/59 karışık sayılar kalmıştı; kullanıcı isteği üzerine temiz tek kaynaklı
  iş emri olarak yeniden yazıldı.

**Ne yapıldı**
- `docs\05-SPK-MODUL-ENVANTERI.md` baştan yazıldı (10,4 KB):
  - Kesin sayılar: 59 yeni dosya (45 SPK\ + 14 kök) + 12 ezilen dosya =
    **71 iş birimi**; ortak 218; sadece bizde 63.
  - **59 modülün numaralı kartları** tek tabloda: P1=19, P2=28, P3=12
    (denetim: 19+28+12=59 ✓); her kartta dosya+konum, map sınıfı,
    canlı config, kaynak stratejisi (sıfırdan / MUIG donor×3).
  - **12 ezilen dosya** ayrı bölümde (E-01..E-12) canlı davranış kanıtıyla.
  - Modül başına 6 adımlı uygulama akışı, Faz 2c kabul kriterleri,
    4 açık soru (MessLang alt birimleri, EventMainManager büyüklüğü vb.).
- Sayı tutarlılığı: 58 (map) → 59 (PDB dosya) düzeltmesi korunuyor.

**Doküman güncellemeleri**
- `05-SPK-MODUL-ENVANTERI.md`: temiz yeniden yazım
- CHANGELOG: bu kayıt

**Commit** — `fae463465` · **Tamamlandı** — 30.09.2026 17:14

## [26.09.30 20:19] Pano v5.1 — ilk gerçek kullanıcı komutu işlendi

**Ne yapıldı**
- Kullanıcı panodan ilk komutunu gönderdi: "deneme yapalım çalışıyormu"
  (20:15:29, id c276aa5c). Ajan kuyruktan işledi, sonucu sonuc.json'a yazdı;
  Sohbet Akışı'na SEN/AJAN kayıtları eklendi.
- Akış doğrulandı: pano POST → kuyruk → sohbet tetikleyici ("pano") → işleme →
  panoda sonuç.

**Commit** — (veri dosyası değişimi; kod değişikliği yok, bir sonraki commit ile birlikte)
