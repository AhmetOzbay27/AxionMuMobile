#!/bin/bash
export MSYS2_ARG_CONV_EXCL='*'
MSB="/c/Program Files/Microsoft Visual Studio/2022/Community/MSBuild/Current/Bin/MSBuild.exe"
cd "/c/Axion Mu Source"
"$MSB" "Source/2.DataServer/DataServer/DataServer.vcxproj" /t:Rebuild /p:Configuration=Release_EX803 /p:Platform=Win32 /m /v:minimal > /tmp/ds2e9.log 2>&1
echo "MSBUILD_EXIT=$?" >> /tmp/ds2e9.log
