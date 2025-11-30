; NSIS Installer Script for simple-sftpd
; Modern UI with license acceptance

!include "MUI2.nsh"
!include "FileFunc.nsh"

;--------------------------------
; General

Name "simple-sftpd"
OutFile "simple-sftpd-Setup-${VERSION}.exe"
Unicode True
InstallDir "$PROGRAMFILES\SimpleDaemons\simple-sftpd"
RequestExecutionLevel admin

; Version Information
VIProductVersion "${VERSION}.0"
VIAddVersionKey "ProductName" "simple-sftpd"
VIAddVersionKey "FileDescription" "simple-sftpd Installer"
VIAddVersionKey "FileVersion" "${VERSION}"
VIAddVersionKey "ProductVersion" "${VERSION}"
VIAddVersionKey "CompanyName" "SimpleDaemons"
VIAddVersionKey "LegalCopyright" "Copyright (C) 2024 SimpleDaemons"

;--------------------------------
; Interface Settings

!define MUI_ICON "..\assets\icons\simple-sftpd.ico"
!define MUI_UNICON "..\assets\icons\simple-sftpd.ico"
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "..\assets\icons\header.bmp"
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\assets\icons\wizard.bmp"

;--------------------------------
; Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "..\licenses\LICENSE.txt"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
; Languages

!insertmacro MUI_LANGUAGE "English"

;--------------------------------
; Installer Sections

Section "Core Files" SecCore
    SectionIn RO
    
    SetOutPath "$INSTDIR"
    File /r "simple-sftpd\*"
    
    ; Create uninstaller
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    
    ; Registry entries
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\simple-sftpd" \
        "DisplayName" "simple-sftpd"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\simple-sftpd" \
        "UninstallString" "$INSTDIR\Uninstall.exe"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\simple-sftpd" \
        "Publisher" "SimpleDaemons"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\simple-sftpd" \
        "DisplayVersion" "${VERSION}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\simple-sftpd" \
        "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\simple-sftpd" \
        "NoRepair" 1
SectionEnd

Section "Start Menu Shortcuts" SecShortcuts
    CreateDirectory "$SMPROGRAMS\SimpleDaemons"
    CreateShortcut "$SMPROGRAMS\SimpleDaemons\simple-sftpd.lnk" "$INSTDIR\simple-sftpd.exe"
    CreateShortcut "$SMPROGRAMS\SimpleDaemons\Uninstall simple-sftpd.lnk" "$INSTDIR\Uninstall.exe"
SectionEnd

Section "Windows Service" SecService
    ; Install as Windows service
    ExecWait '"$INSTDIR\simple-sftpd.exe" install'
SectionEnd

;--------------------------------
; Descriptions

LangString DESC_SecCore ${LANG_ENGLISH} "Core application files (required)"
LangString DESC_SecShortcuts ${LANG_ENGLISH} "Create Start Menu shortcuts"
LangString DESC_SecService ${LANG_ENGLISH} "Install as Windows Service"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecCore} $(DESC_SecCore)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecShortcuts} $(DESC_SecShortcuts)
    !insertmacro MUI_DESCRIPTION_TEXT ${SecService} $(DESC_SecService)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

;--------------------------------
; Uninstaller

Section "Uninstall"
    ; Remove service
    ExecWait '"$INSTDIR\simple-sftpd.exe" uninstall'
    
    ; Remove files
    RMDir /r "$INSTDIR"
    
    ; Remove shortcuts
    Delete "$SMPROGRAMS\SimpleDaemons\simple-sftpd.lnk"
    Delete "$SMPROGRAMS\SimpleDaemons\Uninstall simple-sftpd.lnk"
    RMDir "$SMPROGRAMS\SimpleDaemons"
    
    ; Remove registry entries
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\simple-sftpd"
SectionEnd

