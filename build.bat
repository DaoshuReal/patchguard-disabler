@echo off
setlocal
set CFG=Release
if not "%~1"=="" set CFG=%~1
for /f "delims=" %%V in ('"C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe" -latest -property installationPath') do set "VSROOT=%%V"
for /d %%D in ("%VSROOT%\VC\Tools\MSVC\*") do if exist "%%D\bin\Hostx64\x64\ml64.exe" set "ML64DIR=%%D\bin\Hostx64\x64\"
if defined ML64DIR set "PATH=%ML64DIR%;%PATH%"
cmake -S . -B intermediate -G Ninja -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=%CFG%
cmake --build intermediate --config %CFG%
endlocal
