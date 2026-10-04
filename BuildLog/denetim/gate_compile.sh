#!/bin/bash
# 04.10.2026 (docs/37): WSclient.cpp'yi Main proje bayraklariyla tek basina
# derler (vcvarsall yok; INCLUDE elle). Kullanim: gate_compile.sh <etiket> [ek /D ...]
set -u
export MSYS2_ARG_CONV_EXCL='*'
MSVC='C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207'
SDK='C:\Program Files (x86)\Windows Kits\10'
SD='C:\Axion Mu Source\Source\5.Main'
OUT='C:\Axion Mu Source\BuildLog\denetim\obj'
export INCLUDE="$MSVC\include;$SDK\Include\10.0.22621.0\ucrt;$SDK\Include\10.0.22621.0\shared;$SDK\Include\10.0.22621.0\um;$SD\source;$SD\dependencies\include;$SD\dependencies\include\asio;$SD\boost_1_80_0"
LABEL="$1"; shift
"$MSVC\bin\Hostx64\x86\cl.exe" /nologo /c /W0 /Z7 /std:c++17 /EHsc /MT \
  /D WIN32 /D NDEBUG /D _WINDOWS /D _FOREIGN_NDEBUG /D _LANGUAGE_FOREIGN \
  /D _LANGUAGE_ENG /D LDS_PATCH_GLOBAL_100520 "$@" \
  "$SD\source\WSclient.cpp" /Fo"$OUT\wsclient_gate_$LABEL.obj"
rc=$?
echo "GATE_EXIT_$LABEL=$rc"
exit $rc
