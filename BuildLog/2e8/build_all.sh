#!/bin/bash
export MSYS2_ARG_CONV_EXCL='*'
MSB="/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe"
LOG=/tmp/build_2e8.log
: > "$LOG"

run() {
  echo "===== $1 =====" >> "$LOG"
  "$MSB" "$2" /p:Configuration="$3" /p:Platform=Win32 /p:PlatformToolset=v143 \
     /m /v:minimal /nologo >> "$LOG" 2>&1
  echo "EXIT_$1=$?" >> "$LOG"
}

run GameServer    'C:\Axion Mu Source\Source\4.GameServer\GameServer\GameServer.vcxproj'    Release_EX803
run DataServer    'C:\Axion Mu Source\Source\2.DataServer\DataServer\DataServer.vcxproj'    Release_EX803
run JoinServer    'C:\Axion Mu Source\Source\3.JoinServer\JoinServer\JoinServer.vcxproj'    Release_EX803
run ConnectServer 'C:\Axion Mu Source\Source\1.ConnectServer\ConnectServer\ConnectServer.vcxproj' Release_EX803
echo ALL_DONE >> "$LOG"