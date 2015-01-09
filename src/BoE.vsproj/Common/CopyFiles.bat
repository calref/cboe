
@echo off
echo Copying files...
set SolutionDir=%1
set Config=%2
set VCDir=%~f3

set ResourceDir=%SolutionDir%%Config%\..\..\..\rsrc
set TargetDir=%SolutionDir%%Config%
set ScenEdDir=%TargetDir%\"Scenario Editor"
set DllSrcDir=%VCDir%bin
@echo on

@echo Copying graphics...
xcopy %ResourceDir%\graphics.exd %ScenEdDir%\graphics.exd /s /y /i /d

@echo Copying sound effects...
xcopy %ResourceDir%\sounds.exa %ScenEdDir%\sounds.exa /s /y /i /d

@echo Copying dialog definitions...
xcopy %ResourceDir%\dialogs\*.xml %TargetDir%\data\dialogs /s /y /i /d

@echo Copying fonts...
xcopy %ResourceDir%\fonts %TargetDir%\data\fonts /s /y /i /d

@echo Copying string lists...
xcopy %ResourceDir%\strings %TargetDir%\data\strings /s /y /i /d

@echo Copying shaders...
xcopy %1\..\..\tools\mask.* %TargetDir%\data\shaders /s /y /i /d

@echo Copying base scenarios...
xcopy %ResourceDir%\"Blades of Exile Bases" %ScenEdDir%\"Blades of Exile Base" /s /y /i /d

@echo Copying scenario files...
xcopy %ResourceDir%\"Blades of Exile Scenarios"\*.exs %TargetDir%\"Blades of Exile Scenarios" /s /y /i /d
xcopy %ResourceDir%\"Blades of Exile Scenarios"\*.bmp %TargetDir%\"Blades of Exile Scenarios" /s /y /i /d

@echo Copying required DLLs...
if %Config% == "Debug" (
	xcopy "%DllSrcDir%\sfml-audio-d-2.dll" %TargetDir% /s /y /i /d
	xcopy "%DllSrcDir%\sfml-graphics-d-2.dll" %TargetDir% /s /y /i /d
	xcopy "%DllSrcDir%\sfml-system-d-2.dll" %TargetDir% /s /y /i /d
	xcopy "%DllSrcDir%\sfml-window-d-2.dll" %TargetDir% /s /y /i /d
) else (
	xcopy "%DllSrcDir%\sfml-audio-2.dll" %TargetDir% /s /y /i /d
	xcopy "%DllSrcDir%\sfml-graphics-2.dll" %TargetDir% /s /y /i /d
	xcopy "%DllSrcDir%\sfml-system-2.dll" %TargetDir% /s /y /i /d
	xcopy "%DllSrcDir%\sfml-window-2.dll" %TargetDir% /s /y /i /d
)

xcopy "%DllSrcDir%\zlib1.dll" %TargetDir% /s /y /i /d
xcopy "%DllSrcDir%\libsndfile-1.dll" %TargetDir% /s /y /i /d