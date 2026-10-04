# 37 - 603 Tel Duzeni Derleme-Zamani Kapisi (Main projesi)

> Tarih: 2026-10-04 13:33
> Istek (kullanici): "603 tel duzeni dogrulamasini (verify_603_layout) Main projesine
> derleme-zamani kapisi olarak ekle; yanlis makroyla derleme kirilsin"
> Karar (kullanici): Main yalnizca canli SPK 5.2 cifti ile derlenir:
> **GAMESERVER_UPDATE=603 + GAMESERVER_HAISLOTRING=0**; diger tum kombinasyonlar #error.

## Ne yapildi

1. `Source/5.Main/source/WSclient.cpp` icine derleme-zamani kapisi eklendi
   (ilk include blogundan hemen sonra, ~satir 87):
   - `#error` ile makro kilidi: canli cift degilse derleme DURUR (C1189),
   - 11 `static_assert`: tel govde 36/38/20/20 (sifir buff), count ofsetleri
     +35/+37/+19/+16, MONSTER CurHp@+9 / Level@+10 / Life@+12.
   Sayilar `BuildLog/denetim/verify_603_layout.cpp` (bagimsiz denetim) ve canli
   GameServer.pdb kanitiyla birebir aynidir (BuildLog/2e7/live_pdb_viewport_layout.txt).
2. Bayat EX803 yorumlari guncellendi: `WSclient.h` (satir 567, 592, 626) ve
   `Defined_Global.h` (satir 81-82). Artik "farkli ciftler Main projesinde
   DERLENEMEZ (docs/37)" yaziyor; eski "/DGAMESERVER_UPDATE=803 verin" notlari kaldirildi.

## Kapsam (kullanici karariyla)

- **Kabul:** 603+0 (canli: sunucu Release_EX603 + HAISLOTRING=0).
- **Red:** 603+1, 803+1, 803+0 ve digerleri -> C1189 ile derleme kirilir.
- Bilincli sonuc: EX803/603+1 istemci derlemesi Main projesinde KAPALIDIR; gerektiginde
  kapi bilincli olarak degistirilir (yorumda docs/37'ye baglidir). 803 kod dallarinin
  surekli derlenmez kalma riski bu kararla kabul edilmistir.
- Harici denetim dosyasi `BuildLog/denetim/verify_603_layout.cpp` aynen korundu
  (bagimsiz dogrulama; ayni sayilar).

## Kapi metni (ozet; tamami `BuildLog/denetim/wsclient_gate.block.txt`)

```cpp
#if (GAMESERVER_UPDATE != 603) || (GAMESERVER_HAISLOTRING != 0)
#error Main yalniz canli SPK 5.2 ile derlenir: GAMESERVER_UPDATE=603 + GAMESERVER_HAISLOTRING=0 (bkz. docs/37)
#endif
static_assert(sizeof(PCREATE_CHARACTER) - MAX_BUFF_SLOT_INDEX == 36, "PLAYER tel boyu 36 olmali");
static_assert(sizeof(PCREATE_TRANSFORM) - MAX_BUFF_SLOT_INDEX == 38, "CHANGE tel boyu 38 olmali");
static_assert(sizeof(PCREATE_SUMMON)  - MAX_BUFF_SLOT_INDEX == 20, "SUMMON tel boyu 20 olmali");
static_assert(sizeof(PCREATE_MONSTER) - MAX_BUFF_SLOT_INDEX == 20, "MONSTER tel boyu 20 olmali");
// + count ofsetleri +35/+37/+19/+16 ve MONSTER +9/+10/+12 (11 assert)
```

## Dogrulama (kanit)

| # | Test | Sonuc | Kanit |
|---|------|-------|-------|
| 1 | Canli makro, tek dosya derleme (proje bayraklari) | EXIT=0 | `BuildLog/denetim/gate_live603.log` |
| 2 | `/DGAMESERVER_HAISLOTRING=1` | C1189 + EXIT=2 | `BuildLog/denetim/gate_bad_hais1.log` |
| 3 | `/DGAMESERVER_UPDATE=803` | C1189 + EXIT=2 | `BuildLog/denetim/gate_bad_803.log` |
| 4 | MSBuild Main "Global Release" Win32 v143 (incremental) | 0 error, EXIT=0 | `BuildLog/denetim/main_gate_build.log` |
| 5 | Yeni Main.exe vs HEAD | kod ozdes; 13 bayt meta | `BuildLog/denetim/main_gate_exe_cmp.txt` |
| 6 | Harici denetim `verify_603_layout.cpp` (tekrar) | EXIT=0 | `BuildLog/denetim/verify_603_layout_rerun.log` |

- Test 4: WSclient.cpp yeniden derlendi (obj 13:29) ve Main.exe linklendi; hata yok.
- Test 5 ayrinti: iki exe ayni boyutta (12.034.048 B); fark yalniz HEADER 3B
  (COFF `TimeDateStamp`) + `.rdata` 10B (3x debug-directory ts + RSDS age).
  Kod bolumleri birebir oldugundan `ClientFile/Main.exe` HEAD'e geri alindi
  (docs/36 kurali; md5 `91fa8da9...`). Kapi kod uretmez; ikiliye yansimaz.
- Test 2/3 hata satiri: `WSclient.cpp(101): fatal error C1189: #error:  Main yalniz
  canli SPK 5.2 ile derlenir...`

## Isletme

1. Kapi yanarsa once makroya bak: `GAMESERVER_UPDATE` / `GAMESERVER_HAISLOTRING`
   (varsayilanlar: `Defined_Global.h` ve `WSclient.h`).
2. Tel duzeni bilerek degistiyse: `node BuildLog/2e7/viewport_layout.js [UPDATE] [HAISLOTRING]`
   ile hizalamayi dogrula, canli referansi `BuildLog/2e7/live_pdb_viewport_layout.txt`
   ile karsilastir; sonra kapiyi ve `verify_603_layout.cpp`'yi birlikte guncelle.
3. Canli disi (or. EX803) derleme gerekiyorsa kapiyi bilincli degistir; degisiklik
   docs/37'ye baglanmalidir.

**Ilgili:** docs/27 (H-018 tel duzeni), docs/34 (SPK 5.2 uyum), docs/35 (Main 603
derlemesi), docs/36 (ikili karari). Harici denetim kanitlari: `BuildLog/2e7/`,
`BuildLog/denetim/`.
