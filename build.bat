@echo off
set CompilerFlags= /MTd /Z7 /FC /wd4221 /nologo 
set LinkerFlags=-subsystem:windows
set bits=x64
set LibraryLocation=..\deps\lib\%bits%\

mkdir build > NUL 2>NUL
pushd build
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" %bits% > NUL 2>NUL
popd
cd build
ctime -begin bgfx.ctm
cl %CompilerFlags%  ..\code\main.cpp  /I..\deps\include  /link -incremental:no  /LIBPATH:%LibraryLocation% bgfx-shared-libRelease.lib SDL2.lib SDL2_ttf.lib SDL2main.lib Shell32.lib DelayImp.lib gdi32.lib psapi.lib user32.lib kernel32.lib d3d11.lib   %LinkerFlags%
REM "C:\Program Files (x86)\Windows Kits\10\Debuggers\x64\gflags.exe" /p /enable main.exe /full
ctime -end bgfx.ctm
cd ..
