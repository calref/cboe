
vcpkg integrate install

setlocal enabledelayedexpansion

for /f "usebackq tokens=*" %%i in (`vswhere -latest -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe`) do (
	"%%i" -clp:ForceConsoleColor -target:Build -property:Configuration=Release -property:Platform=%1 "proj/vs2017/Blades of Exile.sln"
	exit /b !errorlevel!
)
