@echo off
cd build
mkdir shader > NUL 2>NUL
cd ..
REM set VS_FLAGS=--platform windows -p vs_3_0 -O 3
REM set FS_FLAGS=--platform windows -p ps_3_0 -O 3
set VS_FLAGS=--platform windows -p vs_4_0 -O 3
set FS_FLAGS=--platform windows -p ps_4_0 -O 3
VERIFY OTHER 2>nul
SETLOCAL ENABLEDELAYEDEXPANSION
SETLOCAL ENABLEEXTENSIONS
IF ERRORLEVEL 1 echo Unable to enable extensions
set file=notset
set LIST=
for /D %%i in (shader\*) do set LIST=!LIST! %%i
set LIST=%LIST:shader\=%
for %%i in (%LIST%) do (
tools\shaderc.exe -f shader\%%i\fs_%%i.sc -o build\shader\fs_%%i.bin --type f %FS_FLAGS% --disasm --debug --varyingdef shader\%%i\varying.def.sc -i shader\
tools\shaderc.exe -f shader\%%i\vs_%%i.sc -o build\shader\vs_%%i.bin --type v %VS_FLAGS% --disasm --debug --varyingdef shader\%%i\varying.def.sc -i shader\
)
ENDLOCAL
echo Shaders Compiled
