# 35 — Main 603 Tam Derlemesi (K2 kapatildi)

Tarih: 04.10.2026 06:30-06:45
Kapsam: K2 (Main tam derlemesi) + istemcinin 603 tel duzeni ile derlenip
`ClientFile/Main.exe` uretilmesi. Onceki tur: docs/34 (SPK 5.2 uyum turu).

## 1. K2 hatalari ve duzeltmeleri (3C.0 dosyalari)

### 1.1 SPKData.h:40 — iki #define tek satirda yapisik

Calisma agacindaki hatali satir:

    #define SPK_OFF_CAMERA_DEFAULT 0x558 // float = 45.0 (...)#define SPK_CAMERA_FPS_OFFSET  4 // 0x55C: float = 240.0

Ikinci `#define` yorumun ICINDE kaldigi icin `SPK_CAMERA_FPS_OFFSET` tanimsiz
kaliyordu -> `SPKData.cpp(203)` C2065 + ardindan C2660 (memcpy 2 arg).

Duzeltme: satir ikiye bolundu ve HEAD'deki (commit 5638aa405) orijinal bicimine
birebir getirildi (`#define SPK_CAMERA_FPS_OFFSET\t4\t\t// ...`). Yani K2'nin bu
maddesi aslinda commit'li dosyada YOKTU; bozulma 3C.0 ajaninin commit'siz
duzenlemesinde olusmustu.

### 1.2 SPKMenuBar.cpp:158 — DisplayWidth tanimsiz

`GetSlotRect` icinde `x = (DisplayWidth - barW) / 2 + ...` kullaniliyordu.
Kaynakta boyle bir global yok; kardes menu `MenuCustom.cpp:150` ayni hesabi
`DisplayWin` ile yapiyor (`CBInterface.h:21: extern int DisplayWin`).
Duzeltme: `DisplayWidth` -> `DisplayWin`.

### 1.3 SPKMenuBar.cpp:181 — g_pBCustomMenuInfo tanimsiz

`g_pBCustomMenuInfo` bir makrodur ve `NewUISystem.h:366`'da tanimlidir;
SPKMenuBar.cpp bu basligi (dogrudan/transitif) include etmiyordu. Ayni makroyu
kullanan `CBChoTroi.cpp:3` ile ayni desen uygulandi:
`#include "NewUISystem.h"` eklendi (NewUIBCustomMenu.h'nin hemen altina).

### 1.4 Duzeltme yontemi

Bu dosyalar EUC-KR Korece yorumlar tasiyor; `str_replace`/utf8 yazimi U+FFFD
uretiyor. Tum duzenlemeler latin1 oku/yaz + capa (anchor) kontrolu yapan Node
script'i ile yapildi: `BuildLog/denetim/fix_3c0_k2.js`, `fix_3c0_k2b.js`.
Sonrasinda U+FFFD=0 ve kontrol karakteri=0, CR=0 (LF korunur) dogrulandi.
SPKData.h duzeltmesinden sonra `git diff` bu satirlarda HEAD ile fark
birakmadi (yalniz 3C.0 eklemeleri gorunur).

## 2. Derleme engeli: C1033 (vc143.pdb)

Ilk build daha ilk TU'da (`StdAfx.cpp`) dustu:

    BuildLog\5Main\vc143.pdb : error C1033: program veritabani acilamiyor

Ortamda cl/MSBuild sureci yoktu; `mv` ile PDB kenara tasindi ve **basarili
oldu** -> dosya kilitli degil, BOZUKTU (03.10 kesintili build kalintisi).
`vc143.pdb.C1033-backup` olarak saklandi; yeni build PDB'yi sifirdan yazdi.
Teshis kaydi: `BuildLog/denetim/main_603_build_c1033.log`.

## 3. Derleme sonucu

Komut (kalip): MSBuild.exe Main.vcxproj /p:Configuration="Global Release"
/p:Platform=Win32 /p:PlatformToolset=v143 /m /v:minimal /nologo

- TAM yeniden derleme: 407 .cpp, **0 error**, EXIT=0
  (`BuildLog/denetim/main_603_build.log`, 455 satir).
- 41 uyari satiri: 40 x LNK4099 (detours.lib icin detours.pdb yok — kok
  kutuphanede PDB yayinlanmamis, onceden de vardi) + 1 x MSB8004 (OutDir sonda
  ters bolu yok; zararsiz).
- Link ciktisi: `Main.vcxproj -> C:\Axion Mu Source\ClientFile\Main.exe`.

Uretilen ikili (izlenen dosya guncellendi):

    ClientFile/Main.exe   12.034.048 B   04.10.2026 06:30
    md5  91fa8da9d5e4b57f863504069d0e4f61
    onceki: 12.031.488 B, md5 c49383bf62c412bf53adf1ec62c67f77 (03.10 01:00)

3C.0 kodunun gercekten baglandiginin ikili kaniti (yeni Main.exe icinde bu
ASCII dizeler bulundu): "Etkinlik Saati", "Sifre Degistirme",
"Mucevher Magazasi" (SPKMenuBar.cpp etiket tablosu).

## 4. "603 duzeni" kaniti (derleme zamaninda kilit)

1. Proje komut satirlari: `BuildLog/5Main/Main.tlog/CL.command.1.tlog`
   icindeki TUM TU'lar icin `/D GAMESERVER_UPDATE` veya `/D GAMESERVER_HAISLOTRING`
   override'i YOKTUR. Yani yalniz kaynak varsayilanlari gecerli:
   `Defined_Global.h:84 -> GAMESERVER_UPDATE 603`,
   `WSclient.h:570 -> GAMESERVER_HAISLOTRING 0`.
2. `BuildLog/denetim/verify_603_layout.cpp` projenin GERCEK bayrak setiyle
   (override'siz) derlendi -> EXIT=0 (`verify_603_layout.log`). Icerdigi
   derleme-zamani assert'leri:
   - `GAMESERVER_UPDATE == 603`, `GAMESERVER_HAISLOTRING == 0`
   - Tel govde boylari: `sizeof(PCREATE_*) - MAX_BUFF_SLOT_INDEX` =
     36 / 38 / 20 / 20 (canli SPK 5.2 sunucu PDB: PLAYER/CHANGE/MONSTER/SUMMON)
   - Canli PDB count ofsetleri: +35 / +37 / +19 / +16
   - MONSTER 603 dali: CurHp@+9, Level@+10, Life@+12

NOT (onemli, kolayca "yanlis duzeltme" yapilabilir): istemci yapilarinin
`sizeof` degerleri 68/70/52/52'dir ve bu BIR HATA DEGILDIR. Istemci sozlesmesi
" sabit govde + s_BuffCount kadar buff kuyrugu"dur; kayit ilerletmesi
`WSclient.cpp` (2525/2670/2888/2968) icinde
`sizeof(struct) - (MAX_BUFF_SLOT_INDEX - s_BuffCount)` ile yapilir
(`MAX_BUFF_SLOT_INDEX = 32`, `_define.h:665`). Sifir buff'li tel boyu bu yuzden
`sizeof - 32` = 36/38/20/20'dir. Yapilari 36'ya "indirmek" buff kuyrugunu
bozar.

## 5. Degisen / eklenen dosyalar

Benim duzenlediklerim:
- `Source/5.Main/source/SPKData.h` (define bolme)
- `Source/5.Main/source/SPKMenuBar.cpp` (DisplayWin + NewUISystem.h)
- `ClientFile/Main.exe` (yeni 603 derlemesi)

3C.0 ajanindan devralinan ve bu commit'e dahil edilen (derleme icin zorunlu):
- `Source/5.Main/source/SPKData.cpp`, `SPKMenuBar.cpp`, `SPKMenuBar.h` (yeni)
- `Source/5.Main/source/CBInterface.cpp` (`gSPKMenuBar.Draw()` kancasi)
- `Source/5.Main/Main.vcxproj`, `Main.vcxproj.filters` (SPKData/SPKMenuBar items)

Kanit:
- `BuildLog/denetim/main_603_build.log`, `main_603_build_c1033.log`
- `BuildLog/denetim/verify_603_layout.{cpp,log,obj}`, `size_probe.cpp`
- `BuildLog/denetim/fix_3c0_k2.js`, `fix_3c0_k2b.js`

## 6. Kalan isler

- H-018 runtime dogrulamasi artik MUMKUN: istemci 603 duzeniyle derleniyor.
  Sonraki adim: EX603 GameServer + yeni Main.exe ile giris/cikis sahnesi testi.
- `GetMain/GetMainInfo.exe` + `.pdb` hala commit'siz (baska ajanin/sahipsiz
  ikilisi) -> sahiplenilmedi.
- K1: EX803 ciktilari izlenmeyen "MuServe Classic 5.2 Lorencia" altina dusuyor
  (dagitim yolu karari kullanicida).
- B3 §6 (canli send_list_to_client void overload) ve gcoin kolonu kararlari
  docs/34'teki gibi acik.
