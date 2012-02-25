; Includes
!include "MUI2.nsh"
!include "Sections.nsh"
!include "WinVer.nsh"

; Pics
!define MUI_ICON install.ico
!define MUI_INICON uninstall.ico

!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP header.bmp
!define MUI_WELCOMEFINISHPAGE
!define MUI_WELCOMEFINISHPAGE_BITMAP banner.bmp

;Name and file
Name "Classic Blades of Exile v1.2.0a"
OutFile "BoE-1.2.0a-win32.exe"
RequestExecutionLevel user
ShowInstDetails show
ShowUninstDetails show
SetCompressor /SOLID lzma

!define MUI_ABORTWARNING

InstallDir "$PROGRAMFILES\Blades of Exile"
InstallDirRegKey HKCU "Software\Blades of Exile" ""

VIProductVersion "1.2.0.0"
VIAddVersionKey "ProductName" "Blades of Exile"
VIAddVersionKey "FileVersion" "1.2.0.a"
VIAddVersionKey "ProductVersion" "1.2.0.a"
VIAddVersionKey "LegalCopyright" "(c) 1997-2012"
VIAddVersionKey "FileDescription" "Blades of Exile Installer"

; Welcome page
!define MUI_WELCOMEPAGE_TITLE "Welcome to the Blades of Exile Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of CBoE (Classic Blades of Exile) v1.2.0a, the classic and now open-sourced game by Spiderweb Software.$\r$\n$\r$\nThis installer has been built using an automated nightly build system, and may have some rough patches. Use at your own risk, and be sure to report any errors that come up so that the developers can fix them.$\r$\n$\r$\n$_CLICK"

!insertmacro MUI_PAGE_WELCOME


; GPL page
!insertmacro MUI_PAGE_LICENSE inst\License.txt


; Directory
!insertmacro MUI_PAGE_DIRECTORY


; Install!
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_PAGE_INSTFILES

; Uninstall :/
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

; Files
Section
  SetOutPath "$INSTDIR"
  File inst\*.*
  
  SetOutPath "$INSTDIR\images"
  File inst\images\*.*
  
  SetOutPath "$INSTDIR\scenarios"
  File inst\scenarios\*.*
  
  WriteRegStr HKCU "Software\Blades of Exile" "" $INSTDIR
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  CreateDirectory "$SMPROGRAMS\Blades of Exile"
  CreateShortCut "$SMPROGRAMS\Blades of Exile\Blades of Exile.lnk" "$INSTDIR\Blades of Exile.exe"
  CreateShortCut "$SMPROGRAMS\Blades of Exile\Character Editor.lnk" "$INSTDIR\Blades of Exile Character Editor.exe"
  CreateShortCut "$SMPROGRAMS\Blades of Exile\Scenario Editor.lnk" "$INSTDIR\Blades of Exile Scenario Editor.exe"
  CreateShortCut "$SMPROGRAMS\Blades of Exile\Scenarios Folder.lnk" "$INSTDIR\scenarios"
  CreateShortCut "$SMPROGRAMS\Blades of Exile\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  CreateShortCut "$DESKTOP\Blades of Exile.lnk" "$INSTDIR\Blades of Exile.exe"
  CreateShortCut "$DESKTOP\Character Editor.lnk" "$INSTDIR\Blades of Exile Character Editor.exe"
  CreateShortCut "$DESKTOP\Scenario Editor.lnk" "$INSTDIR\Blades of Exile Scenario Editor.exe"
SectionEnd

; Uninstaller
Section "Uninstall"
  ;ADD YOUR OWN FILES HERE...
  Delete "$INSTDIR\Uninstall.exe"
  RMDir  /r "$INSTDIR"
  DeleteRegKey /ifempty HKCU "Software\Blades of Exile"
  
  Delete "$SMPROGRAMS\Blades of Exile\Uninstall.lnk"
  Delete "$SMPROGRAMS\Blades of Exile\Blades of Exile.lnk"
  Delete "$SMPROGRAMS\Blades of Exile\Character Editor.lnk"
  Delete "$SMPROGRAMS\Blades of Exile\Scenario Editor.lnk"
  Delete "$SMPROGRAMS\Blades of Exile\Scenarios Folder.lnk"
  RMDir  "$SMPROGRAMS\Blades of Exile"
  
  Delete "$DESKTOP\Blades of Exile.lnk"
  Delete "$DESKTOP\Character Editor.lnk"
  Delete "$DESKTOP\Scenario Editor.lnk"
SectionEnd