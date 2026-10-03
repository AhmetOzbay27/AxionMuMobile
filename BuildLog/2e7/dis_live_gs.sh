#!/bin/bash
export MSYS2_ARG_CONV_EXCL='*'
DB="/c/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.44.35207/bin/Hostx64/x64/dumpbin.exe"
cd "/c/Axion Mu Mobile/4.MuServer/Sub-1/GameServer" || exit 1
"$DB" /disasm GameServer.exe > /tmp/live_gs.dis 2>&1
echo "EXIT=$?" >> /tmp/live_gs.dis