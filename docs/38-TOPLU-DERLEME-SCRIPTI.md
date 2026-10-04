# 38 - Tek Komutlu Dogrulanmis Derleme: build_all.sh

> Tarih: 2026-10-04 14:15 (ilk kosu 13:59; --help duzeltmesi sonrasi kanit loglari
> 14:11 kosusuyla yeniden uretildi)
> Istek (kullanici): "Sunucu + istemci derlemelerini tek komutla, dogrulamali ve
> log'lu calistiran bir build script'i yaz (EX603 istemci + EX603/EX803 sunucular)"
> Durum: script yazildi, 3 test kosusuyla dogrulandi; son kosu **12/12 PASS, EXIT=0**.

## Kullanim

    bash BuildLog/denetim/build_all.sh              # tam: istemci + EX603 + EX803
    bash BuildLog/denetim/build_all.sh --no-client  # yalniz sunucular
    bash BuildLog/denetim/build_all.sh --help

Onceki basit `build_all.sh` (yalniz EX803 yigini + Main, dogrulamasiz) bu surumle
degistirildi; davranis ust kume + dogrulama + ozet.

## Hedefler (9 derleme)

| Hedef | Proje | Konfig | Cikti ikili |
|-------|-------|--------|-------------|
| client | 5.Main/Main.vcxproj | Global Release | ClientFile/Main.exe (603+0 kapi, docs/37) |
| cs603  | 1.ConnectServer | Release_EX603 | MuServer/1.ConnectServer/ConnectServer.exe |
| js603  | 3.JoinServer | Release_EX603 | MuServer/3.JoinServer/JoinServer.exe |
| ds603  | 2.DataServer | Release_EX603 | MuServer/2.DataServer/DataServer.exe |
| gs603  | 4.GameServer | Release_EX603 | MuServer/4.GameServer/Sub 1/GameServer/GameServer.exe |
| cs803  | 1.ConnectServer | Release_EX803 | Source/1.ConnectServer/.../Release/ConnectServer_EX803/ConnectServer.exe |
| js803  | 3.JoinServer | Release_EX803 | Source/3.JoinServer/.../Release/JoinServer_EX803/JoinServer.exe |
| ds803  | 2.DataServer | Release_EX803 | Source/2.DataServer/DataServer/Release/DataServer.exe |
| gs803  | 4.GameServer | Release_EX803 | MuServe Classic 5.2 Lorencia/GameServer/GameServer.exe |

Not: Main istemcisi yalnizca canli 603+0 ile derlenir (docs/37 kapisi); yanlis makro
C1189 verir ve script bunu FAIL sayar. EX803 istemcisi bilincli olarak kapalidir.

## Dogrulama (her kosuda)

1. MSBuild exit kodu + hedef logunda `: error ` taramasi.
2. Cikti ikilisi var mi; boyut + md5 ozete yazilir.
3. Tel duzeni: `node BuildLog/2e7/viewport_layout.js 603 0` ve `803 1` ->
   "TUM PAKETLER HIZALI" beklenir.
4. GameServer config makrolari (vcxproj): EX603=`GAMESERVER_UPDATE=603;HAISLOTRING=0`,
   EX803=`GAMESERVER_UPDATE=803`.
5. Ozet: `BuildLog/denetim/build_all_summary.txt`; script 12 kontrolun tamami gecerse
   exit 0, aksi halde exit 1 (FAIL satirlari ozette).

## Cikti dosyalari

- `BuildLog/denetim/build_all.log` - tum MSBuild ciktilari, hedef bazli bolumler halinde.
- `BuildLog/denetim/build_all/targets/<hedef>.log` - hedef bazli ciplak loglar.
- `BuildLog/denetim/build_all/layout_<update>_<hais>.log` - tel duzeni denetim ciktilari.
- `BuildLog/denetim/build_all_summary.txt` - PASS/FAIL ozeti (boyut + md5 ile).
- `BuildLog/denetim/build_all_console.log` - kosu konsol kaydi (ozetle ayni satirlar).

## Test kosusu (04.10 14:11; ilk kosu 13:59) - 12/12 PASS, SCRIPT_EXIT=0

| Hedef | Sonuc | Boyut (B) | md5 |
|-------|-------|-----------|-----|
| client | PASS | 12.034.048 | 91fa8da9d5e4b57f863504069d0e4f61 |
| cs603  | PASS | 103.936 | 516d6a496b236a38d4b69cd89c8c2351 |
| js603  | PASS | 943.616 | 4698cff71701f38909298742cad0d934 |
| ds603  | PASS | 1.034.240 | 4fe2cda2994b24c5784dfdacdbb94336 |
| gs603  | PASS | 10.656.256 | 02f695e2df973766d1592cb9bbd3cfc2 |
| cs803  | PASS | 103.936 | 7e5775f8f783a7bb18a7ac43551f7218 |
| js803  | PASS | 943.616 | 504a19080f369b0953cd64079d205bc2 |
| ds803  | PASS | 1.062.912 | dfddd76d6e672e9519b2849863d96ee1 |
| gs803  | PASS | 11.294.720 | d1cd380b5b6bab37397b8b1b28cddf08 |
| layout 603/0 | PASS | - | TUM PAKETLER HIZALI |
| layout 803/1 | PASS | - | TUM PAKETLER HIZALI |
| GS config makrolari | PASS | - | EX603=603+HAISLOTRING=0, EX803=803 |

Not: client 13:59 kosusunda tam yeniden derlenip `a851551d...` uretti (yalniz 13 bayt
meta); docs/36 geregi HEAD ikilisi (`91fa8da9...`) korundu. 14:11 kosusu MSBuild
linkini atladi ve ozet depodaki ikiliyi dogruladi.

## Kosuda cikan bulgular

1. **cs603/js603/ds603 izlenen ikilileri bayatti (30.09)** ve yeniden derleme KOD
   farki getirdi: CS `.text` 23 B, JS `.text` 31 B, DS +3.584 B (`.text` 101.106 B
   fark). Bu ikililer docs/34 UUID duzeltmesini icermiyordu; yeni derlemeler bu
   turda commit edildi. Kanit: `BuildLog/denetim/build_all_binary_diff.txt`.
2. **Main.exe kod ozdes**: yalniz 13 bayt meta (COFF ts 3B + 3x debug-dir ts +
   RSDS age) -> docs/36 kurali geregi HEAD'e geri alindi (md5 `91fa8da9...`).
3. gs603 zaten gunceldi (04.10 03:07 derlemesi); yeniden link yapilmadi, degismedi.

## Script testinde yakalanan 3 hata (duzeltildi)

1. `local u="$1" h="$2" log="$OUT/layout_$u_$h.log"` -> bash `$u_` diye degisken
   arar; `set -u` ile "unbound variable". `${u}_${h}` + ayri satira bolundu.
2. `node "$ROOT/BuildLog/..."` -> node Windows'ta `/c/...` yolunu `C:\c\...`
   cozumler; script repo kokune `cd` ettigi icin goreli yol kullanildi.
3. `--help` ve bilinmeyen arguman, arg ayristirmadan ONCE log dosyalarini
   sifirliyordu; 14:07 cagrisi kanit loglarini bosaltti (0 bayt). Arg ayristirma
   log kirpmasindan onceye alindi. Dogrulama: `--help` oncesi/sonrasi log md5 leri
   birebir ayni (d18c958c / 710b355c / 154a02ab), HELP_EXIT=0; `--bogus` ise
   BOGUS_EXIT=1 ve loglara yine dokunmuyor. Kanit loglari 14:11 kosusuyla
   yeniden uretildi.

## Notlar

- MSBuild yolu `MSBUILD` ortam degiskeniyle degistirilebilir; toolset v143 sabittir.
- Script build'leri sirali kosar; her hedefin logu ayridir, konsol ozet satirlari basilir.
- Iki hedef arasinda dosya kilidi sorunu olmamasi icin sunucu surecleri kapali olmalidir
  (script bunu kontrol etmez; docs/00 §5.1 kurali: test bitince yigin kapatilir).

**Ilgili:** docs/00 (harita), docs/26 (DS EX803 veri yolu), docs/33 (denetim),
docs/34 (SPK 5.2 uyum), docs/37 (istemci 603 kapisi).
