@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x86 >nul 2>&1
cd /d "C:\Axion Mu Source\BuildLog\2e7"
if not exist obj mkdir obj
cl /nologo /EHsc /W3 /O1 /MD /Fo:obj\ /Fe:pdbtype.exe ^
   /I "C:\Program Files\Microsoft Visual Studio\2022\Community\DIA SDK\include" ^
   pdbtype.cpp ^
   /link /LIBPATH:"C:\Program Files\Microsoft Visual Studio\2022\Community\DIA SDK\lib" diaguids.lib ole32.lib
echo COMPILE_EXIT=%ERRORLEVEL%
copy /y "C:\Program Files\Microsoft Visual Studio\2022\Community\DIA SDK\bin\msdia140.dll" . >nul
echo DONE