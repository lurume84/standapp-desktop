;NSIS Modern User Interface
;Basic Example Script
;Written by Joost Verburg

;--------------------------------
;Include StandApp Desktop

!include "MUI2.nsh"
!include "WinVer.nsh"
!include "LogicLib.nsh"

!define MUI_ICON "..\..\src\DesktopUI\img\logo2.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "..\..\src\DesktopUI\img\logo.bmp"
!define MUI_HEADERIMAGE_RIGHT
!define PRODUCT_NAME "StandApp Desktop"
!define VERSION "0.0.0.0"
!define PRODUCT_PUBLISHER "Luis Ruiz"
!define PRODUCT_WEB_SITE "https://github.com/lurume84/standapp-desktop"

VIProductVersion "${VERSION}"
VIAddVersionKey "ProductName" "StandApp Desktop"
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "LegalCopyright" "Copyright (c) 2019 Luis Ruiz"
VIAddVersionKey "FileDescription" "StandApp Desktop"
;--------------------------------
;General

  !system 'if not exist "../../bin/Release/DesktopInstaller/" md "../../bin/Release/DesktopInstaller/"'

  ;Name and file
  Name "StandApp Desktop"
  OutFile "../../bin/Release/DesktopInstaller/StandAppSetup.exe"

  ;Default installation folder
  InstallDir "$LOCALAPPDATA\StandApp Desktop"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\StandApp Desktop" ""

  ;Request application privileges for Windows Vista
  RequestExecutionLevel user

;--------------------------------
;Interface Settings

  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_LICENSE "..\..\LICENSE"
  !insertmacro MUI_PAGE_COMPONENTS
  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES

  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Desktop" SecDummy

	nsExec::ExecToStack "cmd.exe /C tasklist /nh /fi $\"IMAGENAME eq StandApp.exe$\" | find /i $\"StandApp.exe$\""
    Pop $0
    Pop $1
    
    ${If} $1 != ""
      MessageBox MB_ICONEXCLAMATION|MB_OK "StandApp is running, close it before proceeding" /SD IDOK
    ${EndIf}

  SetOutPath "$INSTDIR"
  File /x "*.pdb" /x "*.ipdb" /x "*.iobj" /x "*.lib" "..\..\bin\Release\DesktopApp\*.*"

  SetOutPath "$INSTDIR\Html"
  File /r "..\..\bin\Release\DesktopApp\Html\loading"

  SetOutPath "$INSTDIR\locales"
  File /r "..\..\bin\Release\DesktopApp\locales\*.*"

  SetOutPath "$INSTDIR\swiftshader"
  File /r "..\..\bin\Release\DesktopApp\swiftshader\*.dll"

  ; Create application shortcut (first in installation dir to have the correct "start in" target)
  SetOutPath "$INSTDIR"
  
  CreateShortCut "$INSTDIR\${PRODUCT_NAME}.lnk" "$INSTDIR\StandApp.exe" 
  
  ; Start menu entries
  SetOutPath "$SMPROGRAMS\${PRODUCT_NAME}\"
  CopyFiles "$INSTDIR\${PRODUCT_NAME}.lnk" "$SMPROGRAMS\${PRODUCT_NAME}\"
  CopyFiles "$INSTDIR\${PRODUCT_NAME}.lnk" "$desktop"
  Delete "$INSTDIR\${PRODUCT_NAME}.lnk"

  ;Store installation folder
  WriteRegStr HKCU "Software\StandApp Desktop" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecDummy ${LANG_ENGLISH} "Desktop section."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\Uninstall.exe"

  RMDir /r "$INSTDIR"

  DeleteRegKey /ifempty HKCU "Software\StandApp Desktop"

SectionEnd