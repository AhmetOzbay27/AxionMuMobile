# 39 - Push Oncesi Kanca: 603 Kapisi + build_all.sh Dogrulamalari

> Tarih: 2026-10-04 20:02
> Istek (kullanici): "603 derleme kapisini ve build_all.sh dogrulamalarini her push
> oncesi otomatik kosan bir kancaya bagla."
> Durum: kanca yazildi + kuruldu; yesil (push gecti) ve kirmizi (push reddedildi)
> testleriyle dogrulandi.

## Kullanim

    bash BuildLog/denetim/install_hooks.sh   # klon basina bir kez: core.hooksPath=.githooks
    git push ...                             # kanca otomatik kosar
    git push --no-verify                     # acil durum atlatmasi (git yerlesik)

- Kanca dosyasi: `.githooks/pre-push` (izlenir; ince kabuk, cekirdege devreder).
- Cekirdek: `BuildLog/denetim/pre_push_check.sh`.
- Her kosunun loglari: `BuildLog/denetim/pre_push/` (izlenmez).

## Kanca ne kosar (toplam ~25-35 sn; hepsi artimli)

1. **build_all.sh** `--logdir BuildLog/denetim/pre_push/build_all_son`: 9 derleme
   hedefi + tel duzeni 603/0 ve 803/1 + GS config makrolari = 12 kontrol. Kanca
   ciktilari izlenmez dizine yazar; kanonik loglar (`build_all.log` vb.) kirletilmez.
2. **603 kapisi canli**: `gate_compile.sh ... /DGAMESERVER_UPDATE=603
   /DGAMESERVER_HAISLOTRING=0` -> derleme GECMELI (GATE_EXIT=0).
3. **603 kapisi negatif**: ayni derleme `HAISLOTRING=1` ile KIRILMALI (C1189);
   derleme gecerse kapi etkisiz sayilir ve push bloklanir (kapinin sessizce
   kaldirilmasina karsi bekci).
4. **Yeni-kirli kontrolu**: kanca kosusu izlenen bir dosyayi degistirdiyse (or.
   kaynak degisince yeniden linklenen ikili) push bloklanir; once commit'leyip
   tekrar push edilir.

Fail-closed: MSBuild veya cl.exe bulunamazsa push bloklanir. Calisma agaci zaten
kirliyse uyari basilir (dogrulama HEAD'i degil agaci olcer).

## Kurulum

- Bu klonda uygulandi: `git config --local core.hooksPath .githooks` (yerel ayar;
  `install_hooks.sh` yeniden uygular). `.git/hooks` (zaten bos) devre disi kalir.
- Yeni klon icin tek komut: `bash BuildLog/denetim/install_hooks.sh`.

## Testler (04.10.2026 ~19:55-20:00)

### Yesil
- Elle: EXIT=0, 34 sn -> build_all 12/12 (20 sn) + kapi canli (7 sn) + negatif (4 sn).
- Gercek push (yerel bare dummy remote): EXIT=0; kanca PASS (27 sn), `main -> main`
  aktarildi. Kanit: `BuildLog/denetim/pre_push_green_push.txt`, `pre_push_green.log`.
- Kanca sonrasi izlenen dosya listesi degismedi (yeni-kirli temiz).

### Kirmizi
- Tel duzeni denetleyicisi gecici olarak devre disi birakildi: elle kosu EXIT=1,
  FAIL build_all adiminda.
- Ayni durumda gercek `git push`: EXIT=1; kanca ciktisi + `error: failed to push
  some refs`; dummy remote'a hicbir ref gitmedi (ls-remote bos).
- Kanit: `BuildLog/denetim/pre_push_red.log`, `pre_push_red_push.txt`.

## build_all.sh degisikligi (--logdir)

- Yeni `--logdir DIR`: `build_all.log`, `build_all_summary.txt`, `build_all/*.log`
  bu dizine yazilir. Argumansiz cagri davranisi birebir aynidir.
- Arg regresyonu (yeni parser): `--help` EXIT=0 ve log dosyalarina dokunmuyor;
  `--bogus` EXIT=1; `--logdir` degerini vermezseniz EXIT=1 (HATA satiri).
- Varsayilan tam kosu (04.10 19:53): 12/12 PASS, EXIT=0, 14 sn; kanonik loglar
  yenilendi; ikili md5 leri degismedi (client `91fa8da9...`, gs603 `02f695e2...`).

## Notlar

- Gecici dummy remote test sonrasi sokuldu (remote kaydi + bare depo silindi).
- `git push --no-verify` kapiyi bilincli olarak atlar; kanca cikti her zaman
  atlatma yolunu hatirlatir.
- Kapi sabit MSVC yoluna baglidir (v143 14.44.35207 + VS2022 Community); farkli
  makinede fail-closed davranir (push --no-verify ile bilincli atlanabilir).

**Ilgili:** docs/37 (603 kapisi), docs/38 (build_all.sh), docs/00 (harita).
