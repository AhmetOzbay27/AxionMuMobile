#!/bin/bash
# build_all.sh - tek komutla dogrulanmis derleme + log (docs/38).
# Hedefler (varsayilan): EX603 istemci (Main "Global Release") + EX603 ve EX803
# sunucu yiginlari (ConnectServer, JoinServer, DataServer, GameServer).
# Kullanim:  bash BuildLog/denetim/build_all.sh [--no-client] [--help]
# Cikti:     BuildLog/denetim/build_all.log           (tam bolumlu log)
#            BuildLog/denetim/build_all/targets/*.log (hedef bazli loglar)
#            BuildLog/denetim/build_all_summary.txt    (ozet + dogrulama)
# Dogrulama: hedef bazinda exit kodu + ': error ' taramasi + cikti ikilisi
#            (boyut/md5) + tel duzeni (viewport_layout.js) + GS config makrolari.
# Cikis:     tum hedefler gecerse 0, aksi halde 1.
# Not: Main istemcisi 603+0 derleme kapisina tabidir (docs/37); yanlis makroda C1189.
set -u
export MSYS2_ARG_CONV_EXCL='*'
ROOT='/c/Axion Mu Source'
cd "$ROOT" || exit 1
D="$ROOT/BuildLog/denetim"
OUT="$D/build_all"; TLOGS="$OUT/targets"
mkdir -p "$TLOGS"
MASTER="$D/build_all.log"; SUM="$D/build_all_summary.txt"
: > "$MASTER"; : > "$SUM"
MSB="${MSBUILD:-/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe}"
[ -x "$MSB" ] || { echo "HATA: MSBuild bulunamadi: $MSB"; exit 1; }
NOCLIENT=0
for a in "$@"; do
  case "$a" in
    --no-client) NOCLIENT=1 ;;
    --help|-h) sed -n '2,12p' "$0"; exit 0 ;;
    *) echo "Bilinmeyen arguman: $a"; exit 1 ;;
  esac
done
FAIL=0; PASS=0; TOTAL=0
STAMP="$(date '+%Y-%m-%d %H:%M:%S')"
HASH="$(git rev-parse --short HEAD 2>/dev/null || echo -)"
DIRTY="$(git status --porcelain 2>/dev/null | wc -l | tr -d ' ')"
record() { echo "$1" >> "$SUM"; echo "$1"; }
record "# build_all - $STAMP | repo $HASH (izlenen degisiklik=$DIRTY)"

build_target() { # ad proje_win config cikti_msys
  local name="$1" proj="$2" cfg="$3" art="$4"
  local tlog="$TLOGS/$name.log" t0=$SECONDS rc errs sz md5 st dt
  echo "===== $name ($cfg) =====" >> "$MASTER"
  "$MSB" "$proj" /p:Configuration="$cfg" /p:Platform=Win32 /p:PlatformToolset=v143 /m /nologo /v:minimal > "$tlog" 2>&1
  rc=$?
  dt=$((SECONDS - t0))
  cat "$tlog" >> "$MASTER"
  echo "EXIT_$name=$rc ($dt sn)" >> "$MASTER"
  errs=$(grep -c ': error ' "$tlog" || true)
  st="YOK"; sz="-"; md5="-"
  if [ -f "$art" ]; then st="VAR"; sz=$(stat -c %s "$art"); md5=$(md5sum "$art" | cut -d' ' -f1); fi
  TOTAL=$((TOTAL + 1))
  if [ "$rc" -eq 0 ] && [ "$errs" -eq 0 ] && [ "$st" = "VAR" ]; then
    PASS=$((PASS + 1))
    record "PASS  $name  cfg=$cfg  exit=0  hata=0  boyut=$sz  md5=$md5"
  else
    FAIL=$((FAIL + 1))
    record "FAIL  $name  cfg=$cfg  exit=$rc  hata=$errs  cikti=$st"
  fi
}

verify_layout() { # update hais
  local u="$1" h="$2"
  local log="$OUT/layout_${u}_${h}.log" rc
  node "BuildLog/2e7/viewport_layout.js" "$u" "$h" > "$log" 2>&1
  rc=$?
  { echo "===== LAYOUT $u/$h ====="; cat "$log"; echo "EXIT_layout_${u}_${h}=$rc"; } >> "$MASTER"
  TOTAL=$((TOTAL + 1))
  if [ "$rc" -eq 0 ] && grep -q "TUM PAKETLER HIZALI" "$log"; then
    PASS=$((PASS + 1)); record "PASS  layout $u/$h  exit=0  TUM PAKETLER HIZALI"
  else
    FAIL=$((FAIL + 1)); record "FAIL  layout $u/$h  exit=$rc"
  fi
}

verify_gs_macros() {
  local vcx="$ROOT/Source/4.GameServer/GameServer/GameServer.vcxproj"
  TOTAL=$((TOTAL + 1))
  if grep -q 'GAMESERVER_UPDATE=603;HAISLOTRING=0' "$vcx" && grep -q 'GAMESERVER_UPDATE=803' "$vcx"; then
    PASS=$((PASS + 1)); record "PASS  GS config makrolari  EX603=603+HAISLOTRING=0  EX803=803"
  else
    FAIL=$((FAIL + 1)); record "FAIL  GS config makrolari eksik"
  fi
}

P_CS='C:\Axion Mu Source\Source\1.ConnectServer\ConnectServer\ConnectServer.vcxproj'
P_DS='C:\Axion Mu Source\Source\2.DataServer\DataServer\DataServer.vcxproj'
P_JS='C:\Axion Mu Source\Source\3.JoinServer\JoinServer\JoinServer.vcxproj'
P_GS='C:\Axion Mu Source\Source\4.GameServer\GameServer\GameServer.vcxproj'
P_MAIN='C:\Axion Mu Source\Source\5.Main\Main.vcxproj'
A_MAIN="$ROOT/ClientFile/Main.exe"
A_CS603="$ROOT/MuServer/1.ConnectServer/ConnectServer.exe"
A_JS603="$ROOT/MuServer/3.JoinServer/JoinServer.exe"
A_DS603="$ROOT/MuServer/2.DataServer/DataServer.exe"
A_GS603="$ROOT/MuServer/4.GameServer/Sub 1/GameServer/GameServer.exe"
A_CS803="$ROOT/Source/1.ConnectServer/ConnectServer/Release/ConnectServer_EX803/ConnectServer.exe"
A_JS803="$ROOT/Source/3.JoinServer/JoinServer/Release/JoinServer_EX803/JoinServer.exe"
A_DS803="$ROOT/Source/2.DataServer/DataServer/Release/DataServer.exe"
A_GS803="$ROOT/MuServe Classic 5.2 Lorencia/GameServer/GameServer.exe"

record "# --- EX603 (canli): istemci + sunucu yigini ---"
if [ "$NOCLIENT" -eq 0 ]; then build_target client "$P_MAIN" "Global Release" "$A_MAIN"; fi
build_target cs603 "$P_CS" Release_EX603 "$A_CS603"
build_target js603 "$P_JS" Release_EX603 "$A_JS603"
build_target ds603 "$P_DS" Release_EX603 "$A_DS603"
build_target gs603 "$P_GS" Release_EX603 "$A_GS603"
verify_layout 603 0
verify_gs_macros
record "# --- EX803 (test cifti): sunucu yigini ---"
build_target cs803 "$P_CS" Release_EX803 "$A_CS803"
build_target js803 "$P_JS" Release_EX803 "$A_JS803"
build_target ds803 "$P_DS" Release_EX803 "$A_DS803"
build_target gs803 "$P_GS" Release_EX803 "$A_GS803"
verify_layout 803 1
record "# --- ozet ---"
record "TOPLAM: $PASS PASS / $FAIL FAIL (hedef+dogrulama=$TOTAL)"
if [ "$FAIL" -eq 0 ]; then
  record "SONUC: TUM HEDEFLER GECTI ($STAMP)"
  echo "ALL_DONE" >> "$MASTER"
  exit 0
else
  record "SONUC: BASARISIZ ($STAMP)"
  exit 1
fi
