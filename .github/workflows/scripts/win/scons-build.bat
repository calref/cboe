
setlocal enabledelayedexpansion

REM This for loop takes a long time to find vcvarsall.bat,
REM so I hard-coded the path in. When Github Runner versions change,
REM the for loop might be needed again to discover the right path.

REM for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find **/Auxiliary/Build/vcvarsall.bat`) do (
REM  @echo "%%i"
REM  call "%%i" x86_amd64
REM )

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x86_amd64

scons bits=64 %*
