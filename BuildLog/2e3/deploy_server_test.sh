#!/usr/bin/env bash
# 2e.3 — Sunucu test klasörü kurulumu (canlıya DOKUNMADAN)
#
# Kaynak: repo içi MuServer/ ağacındaki BİZİM Release_EX603|Win32 derlemelerimiz
#   (vcxproj OutDir eşlemesi: CS→MuServer\1.ConnectServer, DS→MuServer\2.DataServer,
#    JS→MuServer\3.JoinServer, GS→MuServer\4.GameServer\Sub 1\GameServer)
# Hedef: BuildLog/2e3/deploy/  (canlı C:\Axion Mu Mobile\... ağacına dokunulmaz)
#
# Kullanım:
#   bash BuildLog/2e3/deploy_server_test.sh              # mevcut çıktıları kur
#   bash BuildLog/2e3/deploy_server_test.sh --rebuild    # önce Release_EX603 derle, sonra kur
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$ROOT"
SRC="MuServer"
OUT="BuildLog/2e3/deploy"

if [ "${1:-}" = "--rebuild" ]; then
  echo "== 0) Release_EX603|Win32 yeniden derleme (MSBuild v143) =="
  MSB="/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe"
  export MSYS2_ARG_CONV_EXCL='*'
  for proj in "Source/1.ConnectServer/ConnectServer/ConnectServer.vcxproj" \
              "Source/2.DataServer/DataServer/DataServer.vcxproj" \
              "Source/3.JoinServer/JoinServer/JoinServer.vcxproj" \
              "Source/4.GameServer/GameServer/GameServer.vcxproj"; do
    echo "   -> $(basename "$proj")"
    "$MSB" "$proj" -p:Configuration="Release_EX603" -p:Platform=Win32 -m -v:minimal -nologo
  done
fi

echo "== 1) hedef temizleniyor: $OUT"
rm -rf "$OUT"
mkdir -p "$OUT/1.ConnectServer" "$OUT/2.DataServer" "$OUT/3.JoinServer" \
         "$OUT/4.GameServer/Sub 1/GameServer" "$OUT/4.GameServer/Sub 1/Data"

echo "== 2) ConnectServer (bizim)"
cp -f "$SRC/1.ConnectServer/ConnectServer.exe" \
      "$SRC/1.ConnectServer/ConnectServer.ini" \
      "$SRC/1.ConnectServer/ServerList.ini" "$OUT/1.ConnectServer/"

echo "== 3) DataServer (bizim)"
cp -f "$SRC/2.DataServer/DataServer.exe" \
      "$SRC/2.DataServer/DataServer.ini" "$OUT/2.DataServer/"
for f in AllowableIpList.txt BadSyntax.txt ConfigAutoNap.ini; do
  [ -f "$SRC/2.DataServer/$f" ] && cp -f "$SRC/2.DataServer/$f" "$OUT/2.DataServer/"
done

echo "== 4) JoinServer (bizim)"
cp -f "$SRC/3.JoinServer/JoinServer.exe" \
      "$SRC/3.JoinServer/JoinServer.ini" "$OUT/3.JoinServer/"
[ -f "$SRC/3.JoinServer/AllowableIpList.txt" ] && \
  cp -f "$SRC/3.JoinServer/AllowableIpList.txt" "$OUT/3.JoinServer/"

echo "== 5) GameServer (bizim, Sub 1)"
GS="$SRC/4.GameServer/Sub 1/GameServer"
GO="$OUT/4.GameServer/Sub 1/GameServer"
cp -rf "$SRC/4.GameServer/Sub 1/Data/." "$OUT/4.GameServer/Sub 1/Data/"
for f in GameServer.exe AutoTrain.xml GHRSReset.ini; do
  [ -f "$GS/$f" ] && cp -f "$GS/$f" "$GO/$f"
done
if [ -d "$GS/Data" ]; then mkdir -p "$GO/Data"; cp -rf "$GS/Data/." "$GO/Data/"; fi
mkdir -p "$GO/LOG"
# Atlananlar (bilinçli): *.dmp (crash dökümü), *.pdb (18 MB), LOG içeriği,
# msvcp100/msvcr100 (import kapanışında yok; v143 için MSVCP140+VCRUNTIME140 gerekir)

echo "== 6) VC++ runtime (import kapanışı: MSVCP140 + VCRUNTIME140)"
for d in "1.ConnectServer" "2.DataServer" "3.JoinServer" "4.GameServer/Sub 1/GameServer"; do
  cp -f "$SRC/1.ConnectServer/msvcp140.dll" "$SRC/1.ConnectServer/vcruntime140.dll" "$OUT/$d/"
done

echo "== 7) baslat/durdur betikleri + README"
w() { printf '%s\r\n' "$1"; }   # .bat icin CRLF

{
  w '@echo off'
  w 'title 2e.3 Test MuServer (bizim GS/CS/DS/JS derlemeleri)'
  w 'setlocal'
  w 'cd /d "%~dp0"'
  w 'echo [1/4] ConnectServer...'
  w 'start "ConnectServer" /D "%~dp01.ConnectServer" "%~dp01.ConnectServer\ConnectServer.exe"'
  w 'timeout /t 3 /nobreak >nul'
  w 'echo [2/4] DataServer...'
  w 'start "DataServer" /D "%~dp02.DataServer" "%~dp02.DataServer\DataServer.exe"'
  w 'timeout /t 3 /nobreak >nul'
  w 'echo [3/4] JoinServer...'
  w 'start "JoinServer" /D "%~dp03.JoinServer" "%~dp03.JoinServer\JoinServer.exe"'
  w 'timeout /t 3 /nobreak >nul'
  w 'echo [4/4] GameServer...'
  w 'start "GameServer" /D "%~dp04.GameServer\Sub 1\GameServer" "%~dp04.GameServer\Sub 1\GameServer\GameServer.exe"'
  w 'echo.'
  w 'echo Baslatildi. Durdurmak icin: Stop_TestServer.bat'
  w 'timeout /t 3 /nobreak >nul'
} > "$OUT/Start_TestServer.bat"

{
  w '@echo off'
  w 'title Stop 2e.3 Test MuServer'
  w 'echo Test sunucu surecleri kapatiliyor...'
  w 'taskkill /F /IM ConnectServer.exe >nul 2>nul'
  w 'taskkill /F /IM DataServer.exe >nul 2>nul'
  w 'taskkill /F /IM JoinServer.exe >nul 2>nul'
  w 'taskkill /F /IM GameServer.exe >nul 2>nul'
  w 'echo Done.'
} > "$OUT/Stop_TestServer.bat"

MD5CS=$(md5sum "$OUT/1.ConnectServer/ConnectServer.exe" | cut -d' ' -f1)
MD5DS=$(md5sum "$OUT/2.DataServer/DataServer.exe" | cut -d' ' -f1)
MD5JS=$(md5sum "$OUT/3.JoinServer/JoinServer.exe" | cut -d' ' -f1)
MD5GS=$(md5sum "$GO/GameServer.exe" | cut -d' ' -f1)

{
  echo "2e.3 — Sunucu test klasoru (canliya dokunulmadan kuruldu)"
  echo "Kaynak: repo MuServer/ agaci — BIZIM Release_EX603|Win32 derlemeleri"
  echo ""
  echo "Calistirma : Start_TestServer.bat  (sira: CS - DS - JS - GS)"
  echo "Durdurma   : Stop_TestServer.bat"
  echo ""
  echo "Bizim ciktilarimiz (md5):"
  echo "  ConnectServer.exe $MD5CS"
  echo "  DataServer.exe    $MD5DS"
  echo "  JoinServer.exe    $MD5JS"
  echo "  GameServer.exe    $MD5GS"
  echo ""
  echo "Notlar:"
  echo "  - Bu klasor canli sunucudan (C:\\Axion Mu Mobile\\...) bagimsizdir; oraya dokunulmaz."
  echo "  - DB: DataServer/JoinServer ODBC DSN kullanir (MuOnline / MuOnlineJoin -> yerel"
  echo "    SQLEXPRESS). DB yoksa: BuildLog/2e3/restore_test_db.sh (DB_SQL_12.bak -> MuOnlineS6)."
  echo "  - ServerList.ini icindeki 192.168.99.200 adresleri parite geregi korunmustur;"
  echo "    istemci baglantisi icin testin yapildigi makinenin adresi yazilmalidir."
  echo "  - Anti-hack (XShield) 2e.3 kapsami disidir; pakete konmadi."
} > "$OUT/README.txt"

echo
echo "== OZET =="
du -sh "$OUT" 2>/dev/null
echo "  CS md5: $MD5CS"
echo "  DS md5: $MD5DS"
echo "  JS md5: $MD5JS"
echo "  GS md5: $MD5GS"
echo "Kurulum tamam: $OUT"
