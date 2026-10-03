#!/bin/bash
export MSYS2_ARG_CONV_EXCL='*'
MSB="/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe"
cd "/c/Axion Mu Source" || exit 1

echo "===== GameServer Release_EX803 (2e.7 H-018) =====" > /tmp/build_2e7.log
"$MSB" "C:\Axion Mu Source\Source\4.GameServer\GameServer\GameServer.vcxproj" \
   /p:Configuration=Release_EX803 /p:Platform=Win32 /p:PlatformToolset=v143 \
   /m /v:minimal /nologo >> /tmp/build_2e7.log 2>&1
echo "GS_EXIT=$?" >> /tmp/build_2e7.log

echo "===== Main Global Release (2e.7 H-018) =====" >> /tmp/build_2e7.log
"$MSB" "C:\Axion Mu Source\Source\5.Main\Main.vcxproj" \
   /p:Configuration="Global Release" /p:Platform=Win32 \
   /m /v:minimal /nologo >> /tmp/build_2e7.log 2>&1
echo "MAIN_EXIT=$?" >> /tmp/build_2e7.log
echo "ALL_DONE" >> /tmp/build_2e7.log