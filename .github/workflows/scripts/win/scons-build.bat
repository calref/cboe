
setlocal enabledelayedexpansion

for /f "usebackq tokens=*" %%i in (`vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -find **/Auxiliary/Build/vcvarsall.bat`) do (
  %%i x86_amd64 8.1 && scons
)