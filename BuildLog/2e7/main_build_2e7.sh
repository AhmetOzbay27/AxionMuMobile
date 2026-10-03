#!/bin/bash
export MSYS2_ARG_CONV_EXCL='*'
MSB="/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe"
"$MSB" "C:\Axion Mu Source\Source\5.Main\Main.vcxproj" \
  /p:Configuration="Global Release" /p:Platform=Win32 \
  /p:IntDir="C:\Axion Mu Source\BuildLog\2e7\main_int\\" \
  /m /v:minimal /nologo > /tmp/main_2e7.log 2>&1
echo "MAIN_EXIT=$?" >> /tmp/main_2e7.log
echo MAIN_ALL_DONE >> /tmp/main_2e7.log
