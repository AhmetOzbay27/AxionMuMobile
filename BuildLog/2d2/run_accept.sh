#!/bin/bash
# run_accept.sh <label> <serverdata_kaynak> <connectip_kaynak> [observe]
# Canli istemciye kaynak dosyalari koyar, Engine.exe SPKLaunch ile kosar,
# sonunda orijinal yedekleri geri yukler ve md5 dogrular.
set -u
CLIENT="/c/Axion Mu Mobile/Client and Tools/Client"
BACKUP="/c/Axion Mu Source/BuildLog/2d2/backup"
LABEL="$1"; SRC_SD="$2"; SRC_CI="$3"
OBS="${4:-22}"

echo "== [$LABEL] deploy =="
if ! cp -f "$SRC_CI" "$CLIENT/Data/SPK/ConnectIP.bmd"; then echo "DEPLOY FAIL (ConnectIP): $SRC_CI"; exit 1; fi
if ! cp -f "$SRC_SD" "$CLIENT/Data/SPK/ServerData.bmd"; then echo "DEPLOY FAIL (ServerData): $SRC_SD"; exit 1; fi
md5sum "$CLIENT/Data/SPK/ConnectIP.bmd" "$CLIENT/Data/SPK/ServerData.bmd"

echo "== [$LABEL] run =="
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "C:\\Axion Mu Source\\BuildLog\\2d2\\run_client_test.ps1" -Label "$LABEL" -Arguments SPKLaunch -ObserveSeconds "$OBS"

echo "== [$LABEL] restore =="
cp -f "$BACKUP/ConnectIP.bmd" "$CLIENT/Data/SPK/ConnectIP.bmd"
cp -f "$BACKUP/ServerData.bmd" "$CLIENT/Data/SPK/ServerData.bmd"
md5sum "$CLIENT/Data/SPK/ConnectIP.bmd" "$CLIENT/Data/SPK/ServerData.bmd"
RESTORED=$(md5sum "$CLIENT/Data/SPK/ConnectIP.bmd" "$CLIENT/Data/SPK/ServerData.bmd")
echo "$RESTORED" | grep -q "8ac74a5b79244943ab5607d408049353" && echo "RESTORE OK (ConnectIP)" || echo "RESTORE FAIL (ConnectIP)"
echo "$RESTORED" | grep -q "53982bf8423f731cef2df6de714e78ff" && echo "RESTORE OK (ServerData)" || echo "RESTORE FAIL (ServerData)"
echo "== [$LABEL] done =="
