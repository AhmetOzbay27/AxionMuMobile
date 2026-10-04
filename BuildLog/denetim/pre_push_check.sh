#!/bin/bash
# pre_push_check.sh - git pre-push kancasinin cekirdegi (docs/39):
#   1) build_all.sh (9 hedef + tel duzeni + GS makrolari) --logdir ile izlenmez
#      dizine yazilir; 2) 603 kapisi canli makro derlemesi (GECMELI);
#   3) 603 kapisi yanlis makro derlemesi (C1189 ile KIRILMALI, docs/37);
#   4) kontrollerin izlenen dosyalari degistirmedigini dogrular (degistirdiyse
#      push bloklanir: once commit'leyip tekrar push edilir).
# Kullanim: .githooks/pre-push -> otomatik. Elle test: bash BuildLog/denetim/pre_push_check.sh </dev/null
# Atlatma (acil durum): git push --no-verify
# Loglar: BuildLog/denetim/pre_push/ (izlenmez). Cikis: 0 = push serbest, 1 = blok.
set -u
export MSYS2_ARG_CONV_EXCL='*'
ROOT='/c/Axion Mu Source'
cd "$ROOT" || exit 1
D="$ROOT/BuildLog/denetim"
LD="$D/pre_push"
mkdir -p "$LD"
LOG="$LD/pre_push.log"
: > "$LOG"
T0=$SECONDS
say() { echo "$1" | tee -a "$LOG"; }
fill() { echo "$1" >> "$LOG"; }
fail() {
  say "PRE_PUSH_SONUC=FAIL ($1)"
  say "Ayrinti: BuildLog/denetim/pre_push/pre_push.log"
  say "Acil durumda atlatma: git push --no-verify"
  exit 1
}
MSB='/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe'
CL='/c/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x86/cl.exe'
[ -x "$MSB" ] || fail "MSBuild bulunamadi: $MSB (kanca fail-closed)"
[ -f "$CL" ] || fail "cl.exe bulunamadi: $CL (kanca fail-closed)"
if [ ! -t 0 ]; then REFS="$(cat)"; else REFS='(elle kosum: stdin tty)'; fi
git status --porcelain -uno > "$LD/status_before.txt"
DIRTY_N="$(wc -l < "$LD/status_before.txt" | tr -d ' ')"
say "pre-push dogrulama basladi | HEAD=$(git rev-parse --short HEAD) izlenen-kirli=$DIRTY_N"
fill "push refs:"; printf '%s\n' "$REFS" >> "$LOG"
[ "$DIRTY_N" -eq 0 ] || say "UYARI: calisma agaci zaten kirli ($DIRTY_N izlenen dosya); dogrulama HEAD'i degil calisma agacini olcer."
say "[1/3] build_all.sh (9 hedef, artimli; --logdir=pre_push/build_all_son) ..."
T=$SECONDS
RUNDIR="$LD/build_all_son"
bash "$D/build_all.sh" --logdir "$RUNDIR" > "$LD/build_all_console.log" 2>&1
BA=$?
DT=$((SECONDS - T))
if [ "$BA" -ne 0 ]; then
  [ -f "$RUNDIR/build_all_summary.txt" ] && grep '^FAIL' "$RUNDIR/build_all_summary.txt" >> "$LOG"
  fail "build_all.sh BASARISIZ (exit=$BA, ${DT} sn) - ozet: BuildLog/denetim/pre_push/build_all_son/build_all_summary.txt"
fi
tail -2 "$RUNDIR/build_all_summary.txt" >> "$LOG"
say "[1/3] build_all.sh OK (${DT} sn, 12/12)"
say "[2/3] 603 kapisi: canli makro (603+0) derlemesi, GEC bekleniyor ..."
T=$SECONDS
bash "$D/gate_compile.sh" pre_push_live /DGAMESERVER_UPDATE=603 /DGAMESERVER_HAISLOTRING=0 > "$LD/gate_live.log" 2>&1
GL=$?
DT=$((SECONDS - T))
if [ "$GL" -ne 0 ]; then
  tail -5 "$LD/gate_live.log" >> "$LOG"
  fail "603 kapisi: canli makroyla WSclient.cpp derlenemedi (exit=$GL, ${DT} sn) - BuildLog/denetim/pre_push/gate_live.log"
fi
say "[2/3] 603 kapisi canli OK (${DT} sn, GATE_EXIT=0)"
say "[3/3] 603 kapisi: yanlis makro (HAISLOTRING=1) derlemesi, KIRILMA bekleniyor ..."
T=$SECONDS
bash "$D/gate_compile.sh" pre_push_bad /DGAMESERVER_UPDATE=603 /DGAMESERVER_HAISLOTRING=1 > "$LD/gate_bad.log" 2>&1
GB=$?
DT=$((SECONDS - T))
if [ "$GB" -eq 0 ]; then
  fail "603 kapisi ETKISIZ: HAISLOTRING=1 ile derleme kirilmadi (docs/37 kapisi kaldirilmis olabilir)"
fi
if ! grep -q 'C1189' "$LD/gate_bad.log"; then
  tail -5 "$LD/gate_bad.log" >> "$LOG"
  fail "603 kapisi: yanlis makroda beklenen C1189 yok (exit=$GB) - BuildLog/denetim/pre_push/gate_bad.log"
fi
say "[3/3] 603 kapisi negatif OK (${DT} sn, C1189, exit=$GB)"
git status --porcelain -uno > "$LD/status_after.txt"
comm -13 <(sort "$LD/status_before.txt") <(sort "$LD/status_after.txt") > "$LD/yeni_kirli.txt"
NEW_N="$(grep -c . "$LD/yeni_kirli.txt" | tr -d ' ')"
if [ "$NEW_N" -gt 0 ]; then
  say "Kanca kosusu izlenen dosyalari degistirdi:"
  sed 's/^/    /' "$LD/yeni_kirli.txt" | tee -a "$LOG"
  fail "derleme izlenen dosyalari degistirdi ($NEW_N dosya): commit'leyip tekrar push edin"
fi
TOT=$((SECONDS - T0))
say "PRE_PUSH_SONUC=PASS | build_all 12/12 + 603 canli/negatif + yeni-kirli temiz | ${TOT} sn"
exit 0
