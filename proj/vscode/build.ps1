
Set-Location proj\vscode

$vswhere_path = "vswhere.exe"

if(-not(Test-Path -path $vswhere_path)) {
	(New-Object Net.WebClient).DownloadFile('https://github.com/microsoft/vswhere/releases/latest/download/vswhere.exe', $vswhere_path)
	if(-not(Test-Path -path $vswhere_path)) {
		Write-Output 'Failed to download vswhere.exe'
		exit 1
	}
}

# $msbuild_path = .\proj\vscode\vswhere -latest -products * -requires Microsoft.Component.MSBuild -find MSBuild\**\Bin\MSBuild.exe | select-object -first 1
$vars_path = .\vswhere -latest -products * -requires Microsoft.Component.MSBuild -find **\VC\Auxiliary\Build\vcvars64.bat
if($vars_path) {
	.\Invoke-CmdScript $vars_path
	# Write-Output $msbuild_path @args
	# & "$msbuild_path\..\..\..\..\VC\Auxiliary\Build\vcvars64.bat"
	# Write-Output $env:PATH
	MSBuild @args
} else {
	Write-Output 'Could not find MSBuild.exe'
	exit 1
}
