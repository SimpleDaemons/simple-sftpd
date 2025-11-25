; Simple-Secure FTP Daemon Windows Installer
; NSIS Installer Script

!define PRODUCT_NAME "Simple-Secure FTP Daemon"
!define PRODUCT_VERSION "0.1.0"
!define PRODUCT_PUBLISHER "simple-sftpd Team"
!define PRODUCT_WEB_SITE "https://github.com/simple-sftpd/simple-sftpd"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\simple-sftpd.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

SetCompressor lzma

; MUI 1.67 compatible ------
!include "MUI.nsh"

; MUI Settings
!define MUI_ABORTWARNING
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; License page
!insertmacro MUI_PAGE_LICENSE "LICENSE"
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; MUI end ------

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "simple-sftpd-${PRODUCT_VERSION}-windows-x64.exe"
InstallDir "$PROGRAMFILES\simple-sftpd"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainApplication" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  ; Main executable
  File "simple-sftpd.exe"
  File "simple-sftpd-site.exe"
  File "simple-sftpd-module.exe"
  
  ; Libraries
  File "*.dll"
  
  ; Configuration files
  SetOutPath "$INSTDIR\etc\deployment"
  File /r "deployment\*"
  
  ; Service files
  SetOutPath "$INSTDIR\etc"
  File /r "etc\windows\*"
  
  ; Documentation
  SetOutPath "$INSTDIR\docs"
  File /r "docs\*"
  
  ; Create directories
  CreateDirectory "$INSTDIR\logs"
  CreateDirectory "$INSTDIR\var"
  CreateDirectory "$INSTDIR\ssl"
  
  ; Create shortcuts
  CreateDirectory "$SMPROGRAMS\simple-sftpd"
  CreateShortCut "$SMPROGRAMS\simple-sftpd\simple-sftpd.lnk" "$INSTDIR\simple-sftpd.exe"
  CreateShortCut "$SMPROGRAMS\simple-sftpd\Uninstall.lnk" "$INSTDIR\uninst.exe"
  CreateShortCut "$DESKTOP\simple-sftpd.lnk" "$INSTDIR\simple-sftpd.exe"
SectionEnd

Section "ServiceInstallation" SEC02
  ; Install Windows service using NSSM
  SetOutPath "$INSTDIR"
  File "nssm.exe"
  
  ; Check if running as Administrator
  UserInfo::GetAccountType
  Pop $0
  StrCmp $0 1 admin_ok
  MessageBox MB_OK|MB_ICONSTOP "This installer must be run as Administrator to install the Windows service."
  Goto admin_fail
  
  admin_ok:
    ; Install service
    ExecWait '"$INSTDIR\nssm.exe" install simple-sftpd "$INSTDIR\simple-sftpd.exe" --config "$INSTDIR\etc\deployment\simple-sftpd.conf"'
    ExecWait '"$INSTDIR\nssm.exe" set simple-sftpd DisplayName "Simple-Secure FTP Daemon"'
    ExecWait '"$INSTDIR\nssm.exe" set simple-sftpd Description "Secure, configurable FTP server with modular architecture"'
    ExecWait '"$INSTDIR\nssm.exe" set simple-sftpd AppDirectory "$INSTDIR"'
    ExecWait '"$INSTDIR\nssm.exe" set simple-sftpd AppStdout "$INSTDIR\logs\simple-sftpd-service.log"'
    ExecWait '"$INSTDIR\nssm.exe" set simple-sftpd AppStderr "$INSTDIR\logs\simple-sftpd-service-error.log"'
    ExecWait '"$INSTDIR\nssm.exe" set simple-sftpd Start SERVICE_AUTO_START'
    
    ; Set environment variables
    ExecWait '"$INSTDIR\nssm.exe" set simple-sftpd AppEnvironmentExtra SIMPLE_SFTPD_HOME="$INSTDIR" SIMPLE_SFTPD_CONFIG="$INSTDIR\etc\deployment" SIMPLE_SFTPD_LOGS="$INSTDIR\logs"'
    
    ; Start service
    ExecWait 'net start simple-sftpd'
    
    Goto admin_done
  
  admin_fail:
    MessageBox MB_OK|MB_ICONINFORMATION "Service installation skipped. You can install it later by running the installer as Administrator."
  
  admin_done:
SectionEnd

Section -AdditionalIcons
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"
  CreateShortCut "$SMPROGRAMS\simple-sftpd\Website.lnk" "$INSTDIR\${PRODUCT_NAME}.url"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\simple-sftpd.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\simple-sftpd.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
SectionEnd

; Uninstaller
Function un.onUninstSuccess
  HideWindow
  MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

Function un.onInit
  MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
  Abort
FunctionEnd

Section Uninstall
  ; Stop and remove service
  ExecWait 'net stop simple-sftpd'
  ExecWait '"$INSTDIR\nssm.exe" remove simple-sftpd confirm'
  
  ; Remove shortcuts
  Delete "$SMPROGRAMS\simple-sftpd\Uninstall.lnk"
  Delete "$SMPROGRAMS\simple-sftpd\Website.lnk"
  Delete "$SMPROGRAMS\simple-sftpd\simple-sftpd.lnk"
  Delete "$DESKTOP\simple-sftpd.lnk"
  
  ; Remove directories
  RMDir "$SMPROGRAMS\simple-sftpd"
  RMDir /r "$INSTDIR\etc"
  RMDir /r "$INSTDIR\docs"
  RMDir /r "$INSTDIR\logs"
  RMDir /r "$INSTDIR\var"
  RMDir /r "$INSTDIR\ssl"
  
  ; Remove files
  Delete "$INSTDIR\simple-sftpd.exe"
  Delete "$INSTDIR\simple-sftpd-site.exe"
  Delete "$INSTDIR\simple-sftpd-module.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\nssm.exe"
  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  
  ; Remove installation directory
  RMDir "$INSTDIR"
  
  ; Remove registry keys
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
SectionEnd
