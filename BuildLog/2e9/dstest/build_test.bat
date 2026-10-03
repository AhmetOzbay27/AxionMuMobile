@echo off
set "PATH=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\bin\Hostx64\x86;C:\Program Files (x86)\Windows Kits\10\bin$SDKV\x86;%PATH%"
set "INCLUDE=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\include;C:\Program Files (x86)\Windows Kits\10\Include$SDKV\um;C:\Program Files (x86)\Windows Kits\10\Include$SDKV\shared;C:\Program Files (x86)\Windows Kits\10\Include$SDKV\ucrt"
set "LIB=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Tools\MSVC\14.44.35207\lib\x86;C:\Program Files (x86)\Windows Kits\10\Lib$SDKV\um\x86;C:\Program Files (x86)\Windows Kits\10\Lib$SDKV\ucrt\x86"
cd /d "C:\Axion Mu Source\BuildLog\2e9\dstest"
cl /nologo /EHsc /MD /O2 /D_CRT_SECURE_NO_WARNINGS /I "C:\Axion Mu Source\Source\2.DataServer\DataServer" ds_e2e.cpp "C:\Axion Mu Source\Source\2.DataServer\DataServer\DataStore.cpp" /Fe:ds_e2e.exe /Fo:obj\ odbc32.lib
echo CL_EXIT=%ERRORLEVEL%
