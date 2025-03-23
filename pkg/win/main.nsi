; Script generated with the Venis Install Wizard

; Define your application name
!define APPNAME "Blades of Exile"
!define APPNAMEANDVERSION "Blades of Exile 1.0 beta"

; This specifies the build output dir to copy files from
; It uses /ifndef so it can be overridden from the commandline
; The default is the scons output directory
!ifndef RELEASE_DIR
	!define RELEASE_DIR "..\..\build\Blades of Exile"
!endif

; Main Install settings
Name "${APPNAMEANDVERSION}"
InstallDir "$PROGRAMFILES\Blades of Exile"
InstallDirRegKey HKLM "Software\${APPNAME}" ""
OutFile "Install-OBoE.exe"

; File association helpers
!include "fileassoc.nsh"

; Modern interface settings
!include "MUI.nsh"

!define MUI_ABORTWARNING
!define MUI_FINISHPAGE_RUN "$INSTDIR\Blades of Exile.exe"

; These ensure that you don't have to "agree" to the license to continue
!define MUI_LICENSEPAGE_TEXT_BOTTOM "Press the Install button to Continue"
!define MUI_LICENSEPAGE_BUTTON "&Install"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\..\LICENSE.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Set languages (first is default language)
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_RESERVEFILE_LANGDLL

Section "Blades of Exile" Section1

	; Set Section properties
	SectionIn RO
	SetOverwrite ifnewer

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	; Install Visual Studio Redistributables
	!ifdef MSVC
		File "${RELEASE_DIR}\VCRedistInstall.exe"
		MessageBox MB_OK "Blades of Exile Installer will now launch the Microsoft Visual C++ \
			Redistributable installer, which is required to complete the installation."
		ExecWait '$INSTDIR\VCRedistInstall.exe /passive'
		Delete "$INSTDIR\VCRedistInstall.exe"
	!endif
	; The executable file itself
	File "${RELEASE_DIR}\Blades of Exile.exe"
	; Required DLLs
	File "${RELEASE_DIR}\libsndfile-1.dll"
	File "${RELEASE_DIR}\openal32.dll"
	File "${RELEASE_DIR}\sfml-audio-2.dll"
	File "${RELEASE_DIR}\sfml-graphics-2.dll"
	File "${RELEASE_DIR}\sfml-system-2.dll"
	File "${RELEASE_DIR}\sfml-window-2.dll"
	File "${RELEASE_DIR}\zlib1.dll"
	; Scenarios
	SetOutPath "$INSTDIR\Blades of Exile Scenarios"
	File "${RELEASE_DIR}\Blades of Exile Scenarios\busywork.boes"
	File "${RELEASE_DIR}\Blades of Exile Scenarios\stealth.boes"
	File "${RELEASE_DIR}\Blades of Exile Scenarios\valleydy.boes"
	File "${RELEASE_DIR}\Blades of Exile Scenarios\zakhazi.boes"
	File "${RELEASE_DIR}\Blades of Exile Scenarios\tutorial.boes"
	!include data.nsi
	SetShellVarContext all
	CreateShortCut "$DESKTOP\Blades of Exile.lnk" "$INSTDIR\Blades of Exile.exe"
	CreateDirectory "$SMPROGRAMS\Blades of Exile"
	CreateShortCut "$SMPROGRAMS\Blades of Exile\Blades of Exile.lnk" "$INSTDIR\Blades of Exile.exe"
	CreateShortCut "$SMPROGRAMS\Blades of Exile\Uninstall.lnk" "$INSTDIR\uninstall.exe"
SectionEnd

Section "Character Editor" Section2

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "${RELEASE_DIR}\BoE Character Editor.exe"
	SetShellVarContext all
	CreateShortCut "$SMPROGRAMS\Blades of Exile\Character Editor.lnk" "$INSTDIR\BoE Character Editor.exe"

SectionEnd

Section "Scenario Editor" Section3

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\"
	File "${RELEASE_DIR}\BoE Scenario Editor.exe"
	SetOutPath "$INSTDIR\Blades of Exile Base\"
	File "${RELEASE_DIR}\Blades of Exile Base\bladbase.boes"
	File "${RELEASE_DIR}\Blades of Exile Base\cavebase.boes"
	SetShellVarContext all
	CreateShortCut "$SMPROGRAMS\Blades of Exile\Scenario Editor.lnk" "$INSTDIR\BoE Scenario Editor.exe"

SectionEnd

Section /o "Documentation" Section4

	; Set Section properties
	SetOverwrite on

	; Set Section Files and Shortcuts
	SetOutPath "$INSTDIR\doc\game"
	File "..\..\doc\game\Contents.html"
	File "..\..\doc\game\About.html"
	File "..\..\doc\game\Tips.html"
	File "..\..\doc\game\Intro.html"
	File "..\..\doc\game\Screen.html"
	File "..\..\doc\game\Menus.html"
	File "..\..\doc\game\Town.html"
	File "..\..\doc\game\Outdoors.html"
	File "..\..\doc\game\Combat.html"
	File "..\..\doc\game\Misc.html"
	File "..\..\doc\game\Mage.html"
	File "..\..\doc\game\Priest.html"
	File "..\..\doc\game\Hints.html"
	File "..\..\doc\game\Editor.html"
	File "..\..\doc\game\Credits.html"
	File "..\..\doc\game\Licensing.html"
	File "..\..\doc\game\nav.js"
	File "..\..\doc\game\style.css"
	SetOutPath "$INSTDIR\doc\editor"
	File "..\..\doc\editor\Contents.html"
	File "..\..\doc\editor\About.html"
	File "..\..\doc\editor\Building.html"
	File "..\..\doc\editor\Editing.html"
	File "..\..\doc\editor\Outdoors.html"
	File "..\..\doc\editor\Towns.html"
	File "..\..\doc\editor\Terrain.html"
	File "..\..\doc\editor\Monsters.html"
	File "..\..\doc\editor\Items.html"
	File "..\..\doc\editor\Advanced.html"
	File "..\..\doc\editor\Specials.html"
	File "..\..\doc\editor\Dialogue.html"
	File "..\..\doc\editor\Graphics.html"
	File "..\..\doc\editor\Testing.html"
	File "..\..\doc\editor\nav.js"
	File "..\..\doc\editor\style.css"
	SetOutPath "$INSTDIR\doc\editor\appendix"
	File "..\..\doc\editor\appendix\Specials.html"
	File "..\..\doc\editor\appendix\Items.html"
	File "..\..\doc\editor\appendix\Monsters.html"
	File "..\..\doc\editor\appendix\Terrain.html"
	File "..\..\doc\editor\appendix\Sounds.html"
	File "..\..\doc\editor\appendix\Messages.html"
	File "..\..\doc\editor\appendix\Magic.html"
	File "..\..\doc\editor\appendix\Examples.html"
	SetOutPath "$INSTDIR\doc\img"
	File "..\..\doc\img\background.gif"
	File "..\..\doc\img\boe.gif"
	File "..\..\doc\img\editormainmenu.png"
	File "..\..\doc\img\editorsymbols.gif"
	File "..\..\doc\img\edoutbtns.png"
	File "..\..\doc\img\edtownbtns.png"
	File "..\..\doc\img\invenbtn.gif"
	File "..\..\doc\img\pcbtn.gif"
	File "..\..\doc\img\terscr.gif"

SectionEnd

Section -FinishSection

	WriteRegStr HKLM "Software\${APPNAME}" "" "$INSTDIR"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$INSTDIR\uninstall.exe"
	WriteUninstaller "$INSTDIR\uninstall.exe"
	
	!insertmacro APP_ASSOCIATE "exg" "BladesofExile.SaveGame" "Blades of Exile saved game" "$INSTDIR\Blades of Exile.exe,1" "" '"$INSTDIR\Blades of Exile.exe" "%1"'
	!insertmacro APP_ASSOCIATE_ADDVERB "BladesOfExile.SaveGame" "edit" "" '"$INSTDIR\Char Editor.exe" "%1"'
	!insertmacro APP_ASSOCIATE "boes" "BladesofExile.Scenario" "Blades of Exile scenario" "$INSTDIR\Blades of Exile.exe,2" "" '"$INSTDIR\Scen Editor.exe" "%1"'
	!insertmacro APP_ASSOCIATE "exs" "BladesofExile.OldScenario" "Blades of Exile scenario" "$INSTDIR\Blades of Exile.exe,2" "" '"$INSTDIR\Scen Editor.exe" "%1"'
	!insertmacro UPDATEFILEASSOC
	
;	WriteRegStr HKCR ".exg" "" "BladesOfExile.SaveGame"
;	WriteRegStr HKCR ".boes" "" "BladesOfExile.Scenario"
	
;	WriteRegStr HKCR "MPC.avi" "" "AVI File"
;	WriteRegStr HKCR "MPC.avi\shell" "" "Open"
;	WriteRegStr HKCR "MPC.avi\shell\open\command" "" '"$INSTDIR\Blades of Exile.exe" "%1"'
;	WriteRegStr HKCR "MPC.avi\DefaultIcon" "" "$INSTDIR\Blades of Exile.exe,1" 
	

SectionEnd

; Modern install component descriptions
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
	!insertmacro MUI_DESCRIPTION_TEXT ${Section1} "Install the Blades of Exile game and the four scenarios created by Jeff Vogel, the game's original creator."
	!insertmacro MUI_DESCRIPTION_TEXT ${Section2} "Allows you to edit saved games. You can use this to cheat, or to build a stronger party to enter a harder scenario."
	!insertmacro MUI_DESCRIPTION_TEXT ${Section3} "Allows you to create your own adventures!"
	!insertmacro MUI_DESCRIPTION_TEXT ${Section4} "Install an offline copy of the Blades of Exile documentation."
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;Uninstall section
Section Uninstall

	FindWindow $0 "SFML_Window" "Blades of Exile"
	IsWindow $0 0 +3
		MessageBox MB_ABORTRETRYIGNORE|MB_ICONSTOP "You can't uninstall now because Blades of Exile is currently running. Quit Blades of Exile, and then try again." IDRETRY -2 IDIGNORE +2
		Abort
	FindWindow $0 "SFML_Window" "Blades of Exile Character Editor"
	IsWindow $0 0 +3
		MessageBox MB_ABORTRETRYIGNORE|MB_ICONSTOP "You can't uninstall now because the Blades of Exile Character Editor is currently running. Quit the Character Editor, and then try again." IDRETRY -2 IDIGNORE +2
		Abort
	FindWindow $0 "SFML_Window" "Blades of Exile Scenario Editor"
	IsWindow $0 0 +3
		MessageBox MB_ABORTRETRYIGNORE|MB_ICONSTOP "You can't uninstall now because the Blades of Exile Scenario Editor is currently running. Quit the Scenario Editor, and then try again." IDRETRY -2 IDIGNORE +2
		Abort

	;Remove from registry...
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
	DeleteRegKey HKLM "SOFTWARE\${APPNAME}"
	
	!insertmacro APP_UNASSOCIATE "exg" "BladesOfExile.SaveGame"
	!insertmacro APP_UNASSOCIATE "boes" "BladesOfExile.Scenario"
	!insertmacro APP_UNASSOCIATE "exs" "BladesOfExile.OldScenario"
	!insertmacro UPDATEFILEASSOC

	; Delete self
	Delete "$INSTDIR\uninstall.exe"

	; Clean up Blades of Exile
	Delete "$INSTDIR\Blades of Exile.exe"
	Delete "$INSTDIR\Blades of Exile Scenarios\busywork.boes"
	Delete "$INSTDIR\Blades of Exile Scenarios\stealth.boes"
	Delete "$INSTDIR\Blades of Exile Scenarios\valleydy.boes"
	Delete "$INSTDIR\Blades of Exile Scenarios\zakhazi.boes"
	Delete "$INSTDIR\Blades of Exile Scenarios\tutorial.boes"
	Delete "$INSTDIR\VCRedistInstall.exe"
	Delete "$INSTDIR\libsndfile-1.dll"
	Delete "$INSTDIR\sfml-audio-2.dll"
	Delete "$INSTDIR\sfml-graphics-2.dll"
	Delete "$INSTDIR\sfml-system-2.dll"
	Delete "$INSTDIR\sfml-window-2.dll"
	Delete "$INSTDIR\zlib1.dll"
	Delete "$INSTDIR\openal32.dll"
	RMDir /r "$INSTDIR\doc"
	RMDir /r "$INSTDIR\data"

	; Clean up Character Editor
	Delete "$INSTDIR\Char Editor.exe"

	; Clean up Scenario Editor
	Delete "$INSTDIR\Scen Editor.exe"
	Delete "$INSTDIR\Blades of Exile Base\bladbase.boes"
	Delete "$INSTDIR\Blades of Exile Base\cavebase.boes"

	; Remove remaining directories
	RMDir "$INSTDIR\Blades of Exile Scenarios\"
	RMDir "$INSTDIR\Blades of Exile Base\"
	RMDir "$INSTDIR\"

	; Delete Shortcuts
	SetShellVarContext all
	Delete "$DESKTOP\Blades of Exile.lnk"
	Delete "$SMPROGRAMS\Blades of Exile\Blades of Exile.lnk"
	Delete "$SMPROGRAMS\Blades of Exile\Uninstall.lnk"
	Delete "$SMPROGRAMS\Blades of Exile\Character Editor.lnk"
	Delete "$SMPROGRAMS\Blades of Exile\Scenario Editor.lnk"
	RMDir "$SMPROGRAMS\Blades of Exile\"

SectionEnd

; eof