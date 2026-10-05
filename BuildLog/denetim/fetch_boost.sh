#!/bin/bash
# fetch_boost.sh - Boost 1.80.0 yerel bagimliligini indirip cikarir (docs/42).
# Boost artik surum kontrolunde DEGILDIR (.gitignore: Source/5.Main/boost_1_80_0/);
# temiz bir klonda istemci derlemesi icin bu betik bir kez calistirilir.
# Bu dizine bagimli yerler:
#   - Source/5.Main/Main.vcxproj  "Global Release|Win32"  ($(SolutionDir)boost_1_80_0\)
#   - BuildLog/denetim/gate_compile.sh        (603 kapisi INCLUDE yolu)
#   - BuildLog/2e7/check_wsclient_syntax.bat  (/I yolu)
#   - Source/5.Main/source/ExternalObject/Leaf/constants.h:
#       boost/math/tools/precision.hpp  (tek proje-kodu include'u; header-only,
#       linker'a boost kutuphanesi GIRMEZ)
# Kullanim: bash BuildLog/denetim/fetch_boost.sh [--force]
#   --force          var olan dizini silip yeniden cikarir
#   BOOST_DEST       hedef dizini degistirir (test icin)
# Cikis: 0 hazir | 2 sha256 uyusmaz | 3 disk yetersiz | 4 indirme hatasi | 5 cikarma/dogrulama
set -u
export MSYS2_ARG_CONV_EXCL='*'
ROOT='/c/Axion Mu Source'
DEST="${BOOST_DEST:-$ROOT/Source/5.Main/boost_1_80_0}"
URL='https://archives.boost.io/release/1.80.0/source/boost_1_80_0.tar.gz'
SHA256='4b2136f98bdd1f5857f1c3dea9ac2018effe65286cf251534b6ae20cc45e1847'
MARKER="$DEST/boost/math/tools/precision.hpp"
FORCE=0
[ "${1:-}" = '--force' ] && FORCE=1
TGZ="${TMPDIR:-/tmp}/boost_1_80_0.tar.gz"
# MSYS2_ARG_CONV_EXCL='*' yollar otomatik cevrilmez; curl yerel Windows ikilisidir,
# bu yuzden -o argumanina Windows bicimli yol verilir.
TGZ_W="$(cygpath -w "$TGZ" 2>/dev/null || printf '%s' "$TGZ")"
ham() { printf 'HATA: %s\n' "$1" >&2; }
var() { printf 'OK: %s\n' "$1"; }

# 1) Zaten hazir mi?
if [ -f "$MARKER" ] && [ "$FORCE" -eq 0 ]; then
  var "boost zaten hazir: $DEST ($(du -sh "$DEST" | cut -f1))"
  exit 0
fi

# 2) Disk on kontrolu: indirme ~137 MB + cikarma ~870 MB
PARENT="$(dirname "$DEST")"
mkdir -p "$PARENT" || { ham "hedef dizin olusturulamadi: $PARENT"; exit 5; }
HAVE_KB=$(df -k "$PARENT" | awk 'NR==2{print $4}')
if [ -z "$HAVE_KB" ] || [ "$HAVE_KB" -lt 1228800 ]; then
  ham "disk yetersiz: $(( ${HAVE_KB:-0} / 1024 )) MB bos, ~1200 MB gerekli"
  exit 3
fi

# 3) Indir (onbellekten de olabilir) + sha256
if [ -f "$TGZ" ] && echo "$SHA256  $TGZ" | sha256sum -c - >/dev/null 2>&1; then
  var "onbellek arsivi kullaniliyor: $TGZ"
else
  rm -f "$TGZ"
  var "indiriliyor: $URL"
  curl -fL --retry 3 --retry-delay 2 -o "$TGZ_W" "$URL" || { ham "indirme basarisiz: $URL"; exit 4; }
fi
if ! echo "$SHA256  $TGZ" | sha256sum -c - >/dev/null 2>&1; then
  ham "sha256 uyusmuyor: $TGZ"
  rm -f "$TGZ"
  exit 2
fi
var "sha256 dogrulandi ($(stat -c %s "$TGZ") bayt)"

# 4) Cikar (arsiv kok dizini: boost_1_80_0/)
rm -rf "$DEST"
if command -v cygpath >/dev/null 2>&1 && [ -x '/c/Windows/System32/tar.exe' ]; then
  var "cikariliyor (Windows bsdtar): $PARENT"
  '/c/Windows/System32/tar.exe' -xzf "$(cygpath -w "$TGZ")" -C "$(cygpath -w "$PARENT")" || { ham "cikarma hatasi"; exit 5; }
else
  var "cikariliyor (MSYS tar): $PARENT"
  tar -xzf "$TGZ" -C "$PARENT" || { ham "cikarma hatasi"; exit 5; }
fi
rm -f "$TGZ"

# 5) Dogrulama
[ -f "$MARKER" ] || { ham "cikarma sonrasi marker yok: $MARKER"; exit 5; }
grep -q 'BOOST_LIB_VERSION "1_80"' "$DEST/boost/version.hpp" || { ham "boost/version.hpp surumu 1_80 demiyor"; exit 5; }
N=$(find "$DEST" -type f | wc -l | tr -d ' ')
[ "$N" -gt 60000 ] || { ham "dosya sayisi beklenenden az: $N"; exit 5; }
var "boost hazir: $DEST ($N dosya, $(du -sh "$DEST" | cut -f1))"
printf 'SONUC: FETCH_BOOST_OK\n'
exit 0
