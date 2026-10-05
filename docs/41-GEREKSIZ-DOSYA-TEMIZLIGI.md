# 41 - GEREKSIZ / KULLANILMAYAN DOSYA TEMIZLIGI

Tarih: 2026-10-05 · Durum: ✅ TAMAM · Kazanc: **1067 MB** (disk %100 → %98, bos 208 MB → 1286 MB)

Istek: "gereksiz ve kullanilmayan dosya temizligi yaparmisin"

---

## 1. NEDEN SIMDI (tetiği)

`df -h /c` → **60G/60G, %100 dolu, yalniz 314 MB bos**. Disk baskisi nedeniyle
temizlik sadece hijyen degil, zorunlu hâle gelmisti: bir sonraki pre-push
kancasi (docs/39: `build_all.sh` 12 hedef + 603 kapisi) tam/artimli derleme
yazacak yer bulamayabilirdi.

Kapsam disi birakilanlar (bilincli): `Source/5.Main/boost_1_80_0/` (~850 MB,
**izlenen** ucuncu taraf kutuphane, silinemez), `MuServer/7.DataBase/MuOnline.bak`
ve `ServerTools/.../DB_SQL_12.bak` (geri donusu olmayan veri yedekleri),
canli sureclerin yazdigi loglar.

---

## 2. ONCE / SONRA

| Bolge | Once | Sonra | Kazanc |
|---|---|---|---|
| `BuildLog/` | 671 MB | 147 MB | -524 MB |
| `android/` | 175 MB | 65 MB | -110 MB |
| `Source/1..4` + `Source/Util` | ~370 MB | ~11 MB | -359 MB |
| `ClientFile/` | 59 MB | 12 MB | -47 MB |
| `MuServe Classic 5.2 Lorencia/` | 30 MB | 0 | -30 MB |
| **Bos disk (C:)** | **208 MB** | **1286 MB** | **+1067 MB** |

`git status --porcelain` izlenmeyen gurultusu: **1384 → 96** girdi.

---

## 3. SILINENLER (hepsi izlenmeyen/ignore; uretimi yeniden mumkun)

### A. Derleme ara dizinleri - `docs/33 §4 H1` (444 MB)
| Yol | Boyut | Not |
|---|---|---|
| `BuildLog/5Main/` | 171 MB | Main istemci obj/pch/tlog (Global Release) |
| `BuildLog/4GS/` | 142 MB | GS obj/tlog (EX603+EX803 ortak IntDir) |
| `BuildLog/2DS/` | 36 MB | DataServer obj |
| `BuildLog/3JS/` | 33 MB | JoinServer obj |
| `BuildLog/1CS/` | 30 MB | ConnectServer obj |
| `BuildLog/Getmain/` | 32 MB | obj/tlog/pch - **`GetMainInfo.rebuilt.exe` + `.pdb` korundu** (docs/36 §3/§4 kaniti) |

### B. EX803 uretim agaclari (386 MB)
| Yol | Boyut | Not |
|---|---|---|
| `Source/4.GameServer/GameServer/Release/` | 164 MB | GS EX803 cikisi |
| `Source/Util/cryptopp/Release/` | 82 MB | **DIKKAT: link girdisi** - GS `stdafx.h` icinde `#pragma comment(lib,"..\..\Util\cryptopp\Release\cryptlib.lib")` (vcxproj taramasinda gorunmez). Objeler silindi, `cryptlib.lib` yeniden uretildi; bkz. §6.1 |
| `Source/2.DataServer/DataServer/Release/` | 41 MB | DS EX803 |
| `Source/3.JoinServer/JoinServer/Release/` | 37 MB | JS EX803 |
| `Source/1.ConnectServer/ConnectServer/Release/` | 32 MB | CS EX803 |
| `MuServe Classic 5.2 Lorencia/` | 30 MB | GS EX803 cikti dizini (docs/33 §2/§4) |

> Bu agaclar `bash BuildLog/denetim/build_all.sh` ile **tek komutta** yeniden
> uretilir; cikti md5'leri `BuildLog/denetim/build_all_summary.txt`'te kayitli.

### C. h018 sembolleri + ekran goruntuleri (95 MB)
- `BuildLog/h018/drvout/Main_h018.pdb` + `.map` (~60 MB) - **`Main_h018.exe` korundu** (surucu yeniden kosulabilir).
- `BuildLog/h018/capture/run1/` (26 MB), `run2/` (20 KB), 69 `drv_shot_*.png` + `hover_*.png` + `win_mode*.png` + `before/after_credit.png` (~9 MB).
  Tel kanitlari (`s2c.bin`, `c2s.bin`, `marks.jsonl`, `wire_summary.*`, `runtime_spec_crosscheck.*`) **izlenen** oldugu icin dokunulmadi.

### D. Istemci sembolleri + bayat ikili (58 MB)
- `ClientFile/Main.pdb` 46 MB, `Main.lib`, `Main.exp`.
- `ClientFile/Main` (uzantisiz, 12 MB): md5 `8bd4652f…` - izlenen `Main.exe` (`f79792bd…`) ile **ayni degil**, eski/bayat kopya; hicbir script referans vermiyor.

### E. Android / IDE / kucuk ara dosyalar (93 MB)
- `android/.gradle/` 14 MB, `android/captures/` 25 MB, `android/.idea/` 105 KB,
  `android/app/src/main/jniLibs/x86/` 49 MB (yalniz emulator ABI'si), 15 adet `*.so.bak`.
- `Source/Util/mapm/*.obj`, `Source/Util/detours/detours.pdb`, 8 adet `.aps`,
  `GetMainInfo.sdf`/`.suo`, `resource*.aps`, `BuildLog/h018/vinput/obj/`,
  bos `MuServer/5.Antihack/ScreenShots/`.

---

## 4. KORUNANLAR (ve gerekce)

| Korunan | Gerekce |
|---|---|
| Tum izlenen dosyalar | Kanit/ kaynak; silme yok - silme oncesi her hedefte `git ls-files` sayildi (hepsi 0; **4GS'te 8, Getmain'de 2 izlenen dosya** vardi → o dizinlerde `git clean -fdx` kullanildi, `rm -rf` degil) |
| `BuildLog/denetim/` (+`obj/` 24 MB, `pre_push/`) | docs/34 §3 kaniti (`wsclient_603.obj`, `wsclient_803.obj`), pre-push kayitlari |
| `BuildLog/{3c0,github}/`, `BuildLog/2c1,2d1,2d2,2e2,2e4,envanter` | docs/33/35 kanit loglari ve gecmis tur artefaktlari |
| `BuildLog/h018/{deploy,client,vinput,drv}` + helper scriptler | Calisan E2E yigini (CS/DS/JS/GS **halen ayakta**) ve surucu yeniden uretim seti |
| `MuServer/7.Log`, `MuServer/4.GameServer/**/LOG` | Canli sureclerin yazmakta oldugu loglar (kilit riski) |
| `ServerTools/.../DB_SQL_12.bak`, `MuServer/7.DataBase/MuOnline.bak` | Veri yedegi - geri donusu yok |
| `android/{arm64-v8a,armeabi-v7a,x86_64}/jniLibs` | Izlenen dagitim kutuphaneleri |

---

## 5. KALICI DUZELTME - `.gitignore` (docs/33 §4 H1 onerisi uygulandi)

Eklenen kurallar (bundan sonra bu tur artıklar `git status`'a dusmez):

```
*.iobj  *.ipdb  *.pch  *.tlog/  *.recipe
Source/*/Release/     MuServe Classic 5.2 Lorencia/
BuildLog/{1CS,2DS,3JS,4GS,5Main,Getmain,github}/
BuildLog/h018/{drvout,deploy}/     BuildLog/h018/vinput/obj/
```

Boylece `docs/33 §4 H1 - izlenmeyen uretim artiklari` kalemi **kapanmistir**.

---

## 6. DOGRULAMA

- Silme oncesi: 19 hedefte `git ls-files <yol>` = 0 izlenen dosya (istisna: 4GS=8, Getmain=2 → `git clean -fdx` ile korundu).
- Silme sonrasi: `git status --porcelain` izlenen tarafta yalniz ` M ClientFile/Main.exe`
  (bu turun disinda kalan, onceden var olan degisiklik) - **hicbir izlenen dosya silinmedi**.
- Calisan yigin bozulmadi: `ConnectServer/DataServer/JoinServer/GameServer` surecleri
  hâlâ `BuildLog/h018/deploy/...` altindan kosuyor.
- Silme gunlugu: `/tmp/cleanup_unused.log` (hedef bazinda boyut + rc); 1 hedef
  (`ClientFile/Main`) ilk denemede kilitli gorunup ikinci dogrulamada silinmis cikti.

---

### 6.1 ILK PUSH DENEMESI (KANCA) - IKI GERCEK SORUN VE COZUMU

Ilk kanca kosumu **7 PASS / 5 FAIL** ile dustu (`cs603, js603, ds603, gs603, gs803`):

1. **Disk yeniden doldu.** Tam yeniden derleme sirasinda bos alan 163 MB'a kadar dustu;
   MSBuild `FTK1005`, `MSB6003`, `C1085` hatalariyla durdu - hepsi "**Diskte yeterli yer yok**".
   Cozum: `%TEMP%` temizligi (`BuildLog/denetim/temp_cleanup.sh`; 1 gunden eski dosyalar)
   -> **6669 dosya / 711 MB** acildi; kalan hedefler (EX803 CS/DS/JS + layout) ayni kosumda gecti.
2. **`cryptlib.lib` sanildigi gibi "olu kopya" degildi.** GS `stdafx.h`:
   `#pragma comment(lib,"..\\..\\Util\\cryptopp\\Release\\cryptlib.lib")` (ve Debug esi).
   Referans **vcxproj'da degil**, bu yuzden ilk taramada gorunmedi; silinince GS linki
   `LNK1104: cryptlib.lib dosyasi acilamiyor` ile dustu. Cozum: lib yeniden uretildi:
   `MSBuild Source/Util/cryptopp/cryptlib.vcxproj /p:Configuration=Release /p:Platform=Win32 /p:PlatformToolset=v143`.

**Ders:** "referanssiz" karari yalniz `.vcxproj`/`.props` taramasina dayandirilmamali;
`#pragma comment(lib`, `#include` ve linker satirlari da taranmali. Bu turda
`Source/Util/{mapm,detours}` icin ayni risk yok (yalniz .obj/.pdb silindi, kaynak yerinde).

---

## 7. ETKI / SONRAKI TURA NOTLAR

1. **Pre-push kancasi (docs/39) bir sonraki push'ta TAM yeniden derleme yapar:** (bu tur
   gerceklesti - ~20 dk, bkz. §6.1)
   silinen `BuildLog/{1CS,2DS,3JS,4GS,5Main}` ve `Source/*/Release` artıkları
   yeniden olusur (artik ignore'lu). Kod/hazir degil - sure 57 sn'den dakikalara cikar;
   gerekli disk ~700 MB (bos alan 1286 MB).
2. **Android emulator (x86) kullanilacaksa** `jniLibs/x86` yeniden indirilmeli;
   fiziksel cihaz (arm64/armeabi) derlemesi etkilenmedi.
3. Geri alma gerekirse: silinen her sey `bash BuildLog/denetim/build_all.sh`
   (veya ilgili MSBuild hedefi) ile yeniden uretilir; tel/spec kanitlari zaten izlenen
   dosyalarda duruyor.
4. **Yeniden derleme izlenen ikilileri yeniledi:** `MuServer/{1.ConnectServer/ConnectServer.exe,
   2.DataServer/DataServer.exe, 3.JoinServer/JoinServer.exe, "4.GameServer/Sub 1/GameServer/GameServer.exe"}`
   + `.pdb` esleri ve `ClientFile/Main.exe` (boyutlar ayni, icerik farkli - yeniden derleme
   zaman damgasi/PDB GUID'i). Bunlar **bilincli olarak commit EDILMEDI**: hangi dagitim
   ikilisinin ne zaman yazilacagi **K1** karari (docs/33 §2 H2). H-018'de dogrulanan calisan
   GS kopyasi `BuildLog/h018/deploy/4.GameServer/Sub 1/GameServer/GameServer.exe`
   (md5 `02f695e2df973766d1592cb9bbd3cfc2`) yerinde duruyor; yeni EX603 derlemesi
   farkli md5 uretir (`cc55370c...`).
5. Ayni diski paylasan diger tuketiciler nedeniyle (60 GB disk) periyodik
   `du -sh BuildLog/*` taramasi onerilir; hizli bosaltma icin
   `bash BuildLog/denetim/temp_cleanup.sh` (>1 gunluk `%TEMP%` dosyalari).

---

## 8. KOMUT OZETI (tekrar uretilebilir)

```bash
cd '/c/Axion Mu Source'
git clean -fdxq BuildLog/5Main BuildLog/4GS BuildLog/2DS BuildLog/3JS BuildLog/1CS
git clean -fdxq -e 'GetMainInfo.rebuilt.*' BuildLog/Getmain
git clean -fdxq Source/*/*/Release 'Source/Util/cryptopp/Release' 'MuServe Classic 5.2 Lorencia'
git clean -fdxq -e 'Main_h018.exe' BuildLog/h018/drvout BuildLog/h018/capture/run1 BuildLog/h018/capture/run2
git clean -fdxq android/.gradle android/.idea android/captures android/app/src/main/jniLibs/x86
```
