@echo off
rem H-018 dogrulama: Main projesinin tam derlemesi baska bir ajanin ayni anda
rem duzenledigi SPKData.cpp / SPKMenuBar.cpp dosyalarindaki 4 hatada duruyor
rem (ClCompile varsayilani ErrorAndStop). Bu betik WSclient.cpp'yi tek basina
rem derleyerek H-018 degisikliginin derlendigini kanitlar.
rem PCH kullanilmaz (/Yu yok); stdafx.h kaynak olarak derlenir.
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86 >nul 2>&1
set "SD=C:\Axion Mu Source\Source\5.Main"
set "ID=C:\Axion Mu Source\BuildLog\2e7\main_int"
cd /d "%SD%"
cl /nologo /c /W0 /Z7 /std:c++17 /EHsc /MT ^
   /D WIN32 /D NDEBUG /D _WINDOWS /D _FOREIGN_NDEBUG /D _LANGUAGE_FOREIGN ^
   /D _LANGUAGE_ENG /D LDS_PATCH_GLOBAL_100520 ^
   /I"%SD%\source" /I"%SD%\dependencies\include" /I"%SD%\dependencies\include\asio" ^
   /I"%SD%\boost_1_80_0" ^
   /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.22621.0\ucrt" ^
   "%SD%\source\WSclient.cpp" /Fo"%ID%\wsclient_zs.obj"
echo WSCLIENT_BUILD_EXIT=%ERRORLEVEL%