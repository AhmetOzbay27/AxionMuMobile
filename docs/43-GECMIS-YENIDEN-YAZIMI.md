# 43 - GECMIS YENIDEN YAZIMI (BOOST + OLU AGACLAR)

Tarih: 2026-10-05 · Durum: ✅ TAMAM · `.git`: **776 MB -> 178 MB** (-598 MB, -%77)

Istek (docs/42 devami): "depo ve push boyutunu olculebilir sekilde kucult" -
docs/42 izlemeden cikarmayi yapti ama gecmis blob'lari `.git`'te kaldigi icin
klon boyutu degismemisti. Kullanici onayiyla ("Boost + olu agaclari temizle")
gecmis yeniden yazildi.

---

## 1. KAPSAM

Tum tarihten (141 commit) tamamen cikarilan yollar:

| Yol | Paketteki payi | Durum |
|---|---|---|
| `Source/5.Main/boost_1_80_0/` | 134,7 MB | docs/42'de izlemeden cikarildi, simdi gecmisten de silindi |
| `ClientBuild_192.168.99.200/` | 440,2 MB | HEAD'de zaten yoktu (eski olu agac), gecmisten silindi |

Baska olu ust duzey agac yok (denetim: paket tablosu vs `HEAD` dosya listesi);
olculen diger tum onekler HEAD'de yasiyor.

## 2. YONTEM (arac: `BuildLog/denetim/rewrite_filter.js`, YENI)

`git fast-export --no-data` -> satir filtresi (yalnizca `M`/`D` satirlari) ->
`git fast-import`:

- `--no-data` sayesinde blob'lar **yeniden yazilmaz**, SHA ile referans verilir
  (disk sismez; yalniz yeni agac/commit nesneleri uretilir).
- `data <n>` bloklari (commit mesajlari) bayt bazinda dokunulmadan gecirilir.
- Yollar .git C-tirnaklama bicimi (`"..."`, octal escape) cozulerek karsilastirilir.

Filtre raporu: **171.866 satir atildi** (boost + ClientBuild ekleme/silme satirlari),
**korunan M satiri 12.923**, `data` blogu 141; hedef oneklere ait kalinti M/D: **0**.

## 3. OLCUM (ONCE / SONRA)

| Olcum | Once | Sonra | Fark |
|---|---|---|---|
| `.git` disk boyutu | 776 MB | **178 MB** | **-598 MB (-%77)** |
| `size-pack` | 766,96 MiB | **176,27 MiB** | -590,7 MiB |
| pack nesne sayisi | 95.668 | **9.942** | -85.726 |
| commit sayisi | 141 | 141 (tum SHA'lar degisti) | 0 |
| izlenen dosya | 11.840 | 11.840 | 0 |
| tip agaci SHA | `a26d2bf7...` | `a26d2bf7...` | **birebir ayni** |
| bos disk (C:) | ~911 MB | **~1,9 GB** | +~1 GB |

## 4. DOGRULAMA

1. **Tip agaci SHA'si birebir ayni** (`a26d2bf75aa5de389bb234c06acfb4a63012db23`):
   HEAD'deki dosya kumesi/icerigi hic degismedi -> calisma agaci, derleme
   girdileri ve ciktilari etkilenmedi.
2. **Cift bazli denklik denetimi:** 141 eski↔yeni commit ciftinin TAMAMI
   `git diff --no-renames --name-status` ile karsilastirildi; her fark yalnizca
   iki hedef onekte **silme (D)** satirlari. `uyumsuz_cift=0`.
3. `git log --all -- <hedef yollar>` = **0** (boost ve ClientBuild artik hicbir
   commit'te yok); `git cat-file -e 0b9d9e96d` -> nesne budanmis.
4. `git fsck --connectivity-only` -> exit 0 (sorun yok).
5. Pre-push kancasi (iki kez) **PASS**: build_all 12/12 + 603 kapisi canli/negatif
   + "yeni-kirli temiz". Calisma agaci durumu degismedi (9 eski derleme ikilisi).
6. `HEAD == origin/main == c592b5e5...` (uzak `main` force-push ile guncellendi).

## 5. REF / SHA ETKISI

- `refs/heads/main`: `0b9d9e96d` -> **`c592b5e5`**
- yerel dal `cline/33893` (main'in atasi, benzersiz commit yok): `f78fad878` -> `78e61cc3`
- hafif tag `faz1-tamamlandi`: `1339a290` -> `eba271b9`
- Uzakta yalniz `refs/heads/main` vardi; `--force-with-lease` ile guncellendi.
- **141 satirlik eski->yeni SHA haritasi:** `BuildLog/denetim/gecmis_yeniden_yazim_haritasi.txt`

## 6. SINIRLAR / RISKLER

1. **Eski commit SHA'lari artik yerelde yok.** Uzakta (GitHub) eski nesneler kendi
   sunucu GC'sine kadar erisebilir kalabilir; bu garanti degildir. Acil geri donus
   gerekirse eski tip `0b9d9e96d` uzaktan kurtarilabilir (GitHub destek sureci).
2. **Diger klonlar** eski gecmise merge edemez: `git fetch --all` +
   `git reset --hard origin/main` (veya yeniden klon) gerekir.
3. `git reflog` kayitlari temizlendi (`--expire=now --all`); yerel "geri alma"
   gecmisi yok. `git gc --prune=now` ile nesneler budandi.
4. GitHub'daki **depo boyutu gostergesi** sunucu tarafinda gecikmeli guncellenir;
   bu rapordaki olcumler yerel `.git` ve klon paketine aittir.
5. docs/41-42'de gecen eski commit kisa hash'leri (or. `c49e8ed72`) eski gecmise
   aittir; cevirisi icin §5'teki harita kullanilir.

**Kanit:** `/tmp/pair_check.txt` (bos = uyumsuz cift yok), `/tmp/fsck_after.txt`,
`/tmp/rewrite_old_revs.txt` + `/tmp/rewrite_new_revs.txt`,
`BuildLog/denetim/gecmis_yeniden_yazim_haritasi.txt`, `BuildLog/denetim/rewrite_filter.js`.
**Ilgili:** docs/42 (boost'un izlemeden cikarilmasi), docs/41 (temizlik), docs/39 (kanca).
