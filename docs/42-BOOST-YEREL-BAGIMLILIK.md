# 42 - BOOST YEREL BAGIMLILIK (SURUM KONTROLU DISI)

Tarih: 2026-10-05 · Durum: ✅ TAMAM · Izlenen dosya: **80.312 -> 11.837** (-68.475, %85)

Istek: "Boost kutuphanesini surum kontrolunden cikarip indirilebilir/yerel bir
bagimliliga donustur; depo ve push boyutunu olculebilir sekilde kucult."

---

## 1. ONCEKI DURUM

`Source/5.Main/boost_1_80_0/` depodaki en buyuk izlenen agacti:

| Olcum | Deger |
|---|---|
| Izlenen dosya (boost) | **68.475** (tum deponun %85'i) |
| Diskte | 847 MB |
| HEAD'deki ham icerik | 651,6 MB |
| Paket (pack) icindeki payi | **134,7 MB** (72.283 nesne) |
| Izlenen ikili (.lib/.dll/.exe) | 0 - header-only kullanim |

Kullanim (tumuyle header-only, linker girdisi YOK):

- `Source/5.Main/source/ExternalObject/Leaf/constants.h` →
  `#include <boost/math/tools/precision.hpp>` (proje kodundaki **tek** boost include'u)
- `Source/5.Main/dependencies/include/asio/` icinde 4 baslik `<boost/...>` include eder
- `Main.vcxproj` "Global Release|Win32" `AdditionalIncludeDirectories`:
  `$(SolutionDir)boost_1_80_0\` (istemciyi yoneten `build_all.sh` MSBuild'i
  dogrudan .vcxproj uzerinde kostugu icin bu yol `Source/5.Main/boost_1_80_0`'ya cozulur)
- `BuildLog/denetim/gate_compile.sh` (603 kapisi) ve
  `BuildLog/2e7/check_wsclient_syntax.bat` de ayni dizini `/I` ile kullanir

`Main.vcxproj` icindeki `C:\Libraries\boost_1_75_0` satirlari (L51/52/63/64/175)
baska konfigurasyonlara ait harici yollardir; bu turda **degistirilmedi**.

## 2. YAPILANLAR

1. `git rm -r --cached Source/5.Main/boost_1_80_0` - **68.475 dosya izlemeden
   cikarildi; diskteki kopya korundu** (derleme bozulmaz, izlenmez hale gelir).
2. `.gitignore` kurali (`Source/5.Main/boost_1_80_0/`) docs/41 turunda zaten
   eklenmisti; bu turda yanina geri yukleme ipucu eklendi.
3. `BuildLog/denetim/fetch_boost.sh` **yeni**: resmi arsivden indirir,
   sha256 dogrular, cikarir, dogrular (idempotent; `--force`, `BOOST_DEST`).
   - URL: `https://archives.boost.io/release/1.80.0/source/boost_1_80_0.tar.gz`
   - Boyut: 136.670.223 bayt · sha256:
     `4b2136f98bdd1f5857f1c3dea9ac2018effe65286cf251534b6ae20cc45e1847`
   - Cikti dizini: `Source/5.Main/boost_1_80_0/` (arsiv kok adi birebir ayni)
4. `BuildLog/denetim/build_all.sh` **on kosul kapisi**: istemci hedefi
   (`--no-client` degilse) baslamadan once boost include'u aranir; yoksa
   hedefler hic baslamadan `SONUC: BASARISIZ (eksik bagimlilik)` + `fetch_boost.sh`
   yonlendirmesi ile exit 1.

## 3. OLCUM (ONCE / SONRA)

| Olcum | Once (HEAD c49e8ed72) | Sonra | Fark |
|---|---|---|---|
| Izlenen dosya (toplam) | 80.312 | **11.837** | **-68.475 (%85)** |
| Izlenen ham bayt | 1.246,2 MB | ~594,6 MB | -651,6 MB |
| `git ls-files Source/5.Main/boost_1_80_0` | 68.475 | **0** | -68.475 |
| Diskte boost kopyasi | 847 MB | 847 MB (korundu) | 0 |
| `.git` boyutu | 785 MB | 785 MB (degismedi) | 0 |

**Neden `.git` degismedi:** boost blob'lari gecmiste (140 commit) durmaya devam
eder; `git rm --cached` yalnizca *bundan sonraki* agaclardan cikarir. Gercek
`.git`/klon kuculmesi icin gecmisin yeniden yazilmasi (filter-repo/filter-branch)
gerekir - bu **geri donusu olmayan bir karardir** (tum commit hash'leri degisir,
`push --force` gerekir) ve kullaniciya birakildi. Mevcut pakette boost payi
**134,7 MB**; ayrica HEAD'de olmayan eski agaclar da pakette duruyor
(`ClientBuild_192.168.99.200/` ~440 MB). Rewrite secilirse ikisi birden
temizlenebilir.

Kazanclar (rewrite olmadan, bu turda kesinlesen):

- Yeni klonun **calisma agaci** 847 MB daha kucuk iner (boost artik checkout edilmez).
- Boost'a dokunan degisiklikler artik diff/push yukunu buyutmez (izlenmez + ignore).
- Depoyu tarayicilar icin izlenen dosya sayisi 68.475 azaldi.

## 4. DOGRULAMA

1. **fetch_boost.sh gercek kosum** (`BOOST_DEST=/tmp/boxtest/boost_1_80_0`,
   temiz hedefe):
   - indirme 136.670.223 bayt · **sha256 dogrulandi** ✅
   - cikarma: **72.069 dosya / 847 MB**, `boost/version.hpp = 1_80` ✅
   - exit 0, sure 278 sn (`SONUC: FETCH_BOOST_OK`)
   - Yol boyunca iki gercek hata bulunup duzeltildi: hedef ust dizini yokken disk
     kontrolu ve `MSYS2_ARG_CONV_EXCL='*'` altinda yerel `curl`e POSIX yol verilmesi
     (bkz. `TGZ_W`); script bu nedenle yalnizca "yazildi" degil "kosuldu" diye
     dogrulanmis sayilir.
2. **build_all on kosulu - negatif test:** marker gecici olarak gizlendi ->
   `GATE_EXIT=1`, ozet `ONKOSUL HATA ... COZUM: fetch_boost.sh`, derleme hedefi
   **0 adet** basladi; marker geri konuldu ✅
3. **Arsiv <-> yerel kopya karsilastirmasi** (resmi arsiv yeniden indirilip gecici
   dizine acildi; dosya kumesi + git blob hash'i):
   - Arsiv **72.069** dosya · yerel disk kopyasi **72.067** · HEAD'de izlenen **68.475**.
   - 68.474 ortak yolun **68.235'i birebir ayni**; 83'u yalnizca satir sonu farki
     (`core.autocrlf=true` commit aninda CRLF -> LF normalize etmis; ornek dosya
     blob'u, CRLF'siz hâliyle birebir eslesiyor).
   - **156 dosya upstream 1.80.0'dan gercekten farkli; 155'i dokuman/test**
     (`doc/html/**`, `libs/*/doc/**`). Tek baslik: `boost/dynamic_bitset/dynamic_bitset.hpp`
     (yerel kopyada `at()` overload'lari eklenmis, ~83 bayt). Bu baslik proje
     kodunda KULLANILMIYOR (`Source/5.Main/source`, `dependencies/include`,
     asio: referans yok); boost icinde yalnizca graph/random basliklari dahil
     ediyor -> derleme etkisi yok.
   - Arsivde olup hic izlenmeyen 3.595 dosya var: agirlikli `libs/log` (933),
     `tools/build` (855), `boost/log` (258), `libs/hana` (379) + test/dokuman.
     Bunlar bugune kadar hep izlenmemisti; fetch sonrasi calisma agacinda
     bulunurlar (fazlalik = zararsiz, derlemeyi bozmaz).
4. **Pre-push kancasi** (build_all 12/12 + 603 kapisi canli/negatif) bu commit
   ile kostu ve gecti (bkz. CHANGELOG kaydi).

> Not: `constants.h` disindaki boost'lar asio basliklari uzerinden gelir; derleme
> dogrulamasi bu yuzden gecerlidir - boost dizini yoksa istemci derlemesi kirilir,
> kapida yakalanir.

## 5. TEMIZ KLONDA KURULUM (tek adim)

```bash
cd /c/Axion\ Mu\ Source
bash BuildLog/denetim/fetch_boost.sh          # ~5 dk (indirme ~95 sn + cikarma)
bash BuildLog/denetim/build_all.sh            # on kosul + 12 hedef + dogrulamalar
```

`fetch_boost.sh` zaten dolu dizinde **yeniden indirmez** (marker kontrolu);
zorlamak icin `--force`, baska hedefe acmak icin `BOOST_DEST=...`.

## 6. SINIRLAR / KALAN ISLER

1. `.git` kuculmesi bu turda **saglanmadi** - gecmis yeniden yazimi (ayri karar).
   Rewrite yapilirsa docs/42'deki bu bolum guncellenmelidir.
2. `Main.vcxproj` L51/52/63/64/175 harici `C:\Libraries\boost_1_75_0` yollari hala
   duruyor (baska konfigurasyonlar). Temizlik ayri bir is.
3. Boost'u izlemeyen baska makinelerin yerel kopyasi en az bir kez
   `fetch_boost.sh` ile eslesmelidir (sha256 ile ayni icerik garanti).
4. Yerel kopyadaki tek upstream-disi baslik `dynamic_bitset.hpp` yamasidir
   (`at()` overload'lari); upstream arsivde yok. Proje kodu bugun kullanmiyor;
   ileride gerekirse fetch sonrasi elle yamalanmalidir.

**Kanit:** `/tmp/fetch_boost_test.log` (gercek kosum), `/tmp/boost_verify.log`
(arsiv karsilastirmasi), `/tmp/boost_measure_before.txt` (oncesi olcumler),
`BuildLog/denetim/pre_push/pre_push.log` (kanca kosumu).
**Ilgili:** docs/41 (temizlik), docs/39 (kanci), docs/38 (build_all), docs/37 (603 kapisi).
