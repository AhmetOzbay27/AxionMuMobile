#!/bin/bash
export MSYS2_ARG_CONV_EXCL='*'
cd "/c/Axion Mu Source/BuildLog/2e7" || exit 1
cmd /c "C:\Axion Mu Source\BuildLog\2e7\build_pdbtype.bat" > /tmp/pdbtype_build.log 2>&1
echo "EXIT=$?" >> /tmp/pdbtype_build.log