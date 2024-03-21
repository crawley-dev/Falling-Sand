@echo off

pushd ..
Premake\premake5.exe --file=Build.lua vs2022
popd
pause