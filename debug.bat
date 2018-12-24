@echo off
pushd build
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x64 > NUL 2> NUL 
popd
cd build
devenv /debugexe main.exe
cd ..
