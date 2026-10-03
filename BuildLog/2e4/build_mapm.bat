@echo off
rem mapm.lib yeniden derleme (v100 -> gecerli toolset)
rem Gerekce: repodaki mapm.lib 2015'te VC6/v100 ile uretilmis. mapmutl1.obj
rem icindeki mapmutil.c, fprintf/stderr kullaniyor; v100'de bunlar _fprintf ve
rem ___iob_func olarak adlandirilir, modern UCRT'de o adlar yok -> LNK2001.
rem Orijinal betik: Source/Util/mapm/MKALLMSC.BAT ("cl /c /O2 /W3 /Zl map*.c")
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars32.bat" >nul
cd /d "C:\Axion Mu Source\Source\Util\mapm"
del map*.obj 2>nul
cl /c /O2 /Zl map*.c
if errorlevel 1 exit /b 1
lib /OUT:mapm.lib map*.obj
if errorlevel 1 exit /b 1
echo MAPM_BUILD_OK
exit /b 0
