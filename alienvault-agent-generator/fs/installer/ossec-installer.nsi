;--------------------------------
;Include Modern UI

!include "MUI.nsh"

;--------------------------------
;General

!define VERSION "2.8.3"
!define NAME "Ossec HIDS"
!define /date CDATE "%b %d %Y at %H:%M:%S"


Name "${NAME} Windows Agent v${VERSION}"
BrandingText "Copyright (C) 2003 - 2014 Trend Micro Inc."
OutFile "ossec-agent-alienvault-installer.exe"


InstallDir $PROGRAMFILES\ossec-agent
InstallDirRegKey HKLM "ossec" "Install_Dir"


;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING

;--------------------------------
;Pages
  !define MUI_ICON favicon.ico
  !define MUI_UNICON ossec-uninstall.ico
  ;!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the install of ${Name}.\r\n\r\nClick next to continue."

  ; Page for choosing components.
  ;!define MUI_COMPONENTSPAGE_TEXT_TOP "Select the options you want to be executed. Click next to continue."

  ;!define MUI_COMPONENTSPAGE_TEXT_COMPLIST "text complist"

  ;!define MUI_COMPONENTSPAGE_TEXT_INSTTYPE "Select components to install:"

  ;!define MUI_COMPONENTSPAGE_TEXT_DESCRIPTION_TITLE "text abac"

  ;!define MUI_COMPONENTSPAGE_TEXT_DESCRIPTION_INFO "text info oi"
  
  ;!define MUI_COMPONENTSPAGE_NODESC 

  ;!insertmacro MUI_PAGE_WELCOME
  ;!insertmacro MUI_PAGE_LICENSE "LICENSE.txt"
  ;!insertmacro MUI_PAGE_COMPONENTS
  ;!insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES
  ;!insertmacro MUI_PAGE_FINISH

  ;!insertmacro MUI_UNPAGE_WELCOME
  ;!insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  ;!insertmacro MUI_UNPAGE_FINISH

;--------------------------------
;Languages

  !insertmacro MUI_LANGUAGE "English"

;--------------------------------

Function .onInit
    SetOutPath $INSTDIR
    IfFileExists $INSTDIR\ossec.conf 0 +3
;    MessageBox MB_OKCANCEL "${NAME} is already installed. We will stop it before continuing." IDOK NoAbort
;    Abort
;    NoAbort:
    
    ;; Stopping ossec service.
    ExecWait '"net" "stop" "OssecSvc"'  
FunctionEnd
            

Section "OSSEC Agent (required)" MainSec

;Required section.
SectionIn RO
SetOutPath $INSTDIR

ClearErrors

File ossec-agent.exe ossec.conf default-ossec.conf ossec-lua.exe ossec-luac.exe manage_agents.exe os_win32ui.exe win32ui.exe ossec-rootcheck.exe internal_options.conf local_internal_options.conf setup-windows.exe setup-syscheck.exe setup-iis.exe service-start.exe service-stop.exe doc.html rootkit_trojans.txt rootkit_files.txt add-localfile.exe LICENSE.txt rootcheck\rootcheck.conf rootcheck\db\win_applications_rcl.txt rootcheck\db\win_malware_rcl.txt rootcheck\db\win_audit_rcl.txt help.txt vista_sec.csv route-null.cmd restart-ossec.cmd client.keys
WriteRegStr HKLM SOFTWARE\ossec "Install_Dir" "$INSTDIR"

WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ossec" "DisplayName" "OSSEC Hids Agent"
WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ossec" "UninstallString" '"$INSTDIR\uninstall.exe"'
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ossec" "NoModify" 1
WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ossec" "NoRepair" 1
WriteUninstaller "uninstall.exe"


; Writing version and install information
FileOpen $0 $INSTDIR\VERSION.txt w
IfErrors done
FileWrite $0 "${NAME} v${VERSION} - "
FileWrite $0 "Installed on ${CDATE}"
FileClose $0
done:


CreateDirectory "$INSTDIR\rids"
CreateDirectory "$INSTDIR\syscheck"
CreateDirectory "$INSTDIR\shared"
CreateDirectory "$INSTDIR\active-response"
CreateDirectory "$INSTDIR\active-response\bin"
Delete "$INSTDIR\active-response\bin\route-null.cmd"
Delete "$INSTDIR\active-response\bin\restart-ossec.cmd"
Rename "$INSTDIR\rootkit_trojans.txt" "$INSTDIR\shared\rootkit_trojans.txt"
Rename "$INSTDIR\rootkit_files.txt" "$INSTDIR\shared\rootkit_files.txt"
Rename "$INSTDIR\win_malware_rcl.txt" "$INSTDIR\shared\win_malware_rcl.txt"
Rename "$INSTDIR\win_audit_rcl.txt" "$INSTDIR\shared\win_audit_rcl.txt"
Rename "$INSTDIR\win_applications_rcl.txt" "$INSTDIR\shared\win_applications_rcl.txt"
Rename "$INSTDIR\route-null.cmd" "$INSTDIR\active-response\bin\route-null.cmd"
Rename "$INSTDIR\restart-ossec.cmd" "$INSTDIR\active-response\bin\restart-ossec.cmd"
Delete "$SMPROGRAMS\ossec\Edit.lnk"
Delete "$SMPROGRAMS\ossec\Uninstall.lnk"
Delete "$SMPROGRAMS\ossec\Documentation.lnk"
Delete "$SMPROGRAMS\ossec\Edit Config.lnk"
Delete "$SMPROGRAMS\ossec\*.*"

; Remove directories used
RMDir "$SMPROGRAMS\ossec"

; Creating SMS directory
CreateDirectory "$SMPROGRAMS\ossec"
      
CreateShortCut "$SMPROGRAMS\ossec\Manage Agent.lnk" "$INSTDIR\win32ui.exe" "" "$INSTDIR\win32ui.exe" 0
CreateShortCut "$SMPROGRAMS\ossec\Documentation.lnk" "$INSTDIR\doc.html" "" "$INSTDIR\doc.html" 0
CreateShortCut "$SMPROGRAMS\ossec\Edit Config.lnk" "$INSTDIR\ossec.conf" "" "$INSTDIR\ossec.conf" 0
CreateShortCut "$SMPROGRAMS\ossec\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0


; Install in the services 
ExecWait '"$INSTDIR\ossec-agent.exe" install-service'
ExecWait '"$INSTDIR\setup-windows.exe" "$INSTDIR"' 
ExecWait '"$INSTDIR\setup-syscheck.exe" "$INSTDIR" "enable"'
ExecWait '"$INSTDIR\setup-iis.exe" "$INSTDIR"'
ExecWait '"net" "stop" "OssecSvc"'
ExecWait '"net" "start" "OssecSvc"'
;Exec '"$INSTDIR\os_win32ui.exe" "$INSTDIR"' 
Quit
SectionEnd

Section "Uninstall"
  
  ; Stop ossec
  ExecWait '"net" "stop" "OssecSvc"'
  
  ; Uninstall from the services
  Exec '"$INSTDIR\ossec-agent.exe" uninstall-service'

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\ossec"
  DeleteRegKey HKLM SOFTWARE\ossec

  ; Remove files and uninstaller
  Delete "$INSTDIR\ossec-agent.exe"
  Delete "$INSTDIR\manage_agents.exe"
  Delete "$INSTDIR\ossec.conf"
  Delete "$INSTDIR\uninstall.exe"
  Delete "$INSTDIR\*"
  Delete "$INSTDIR\rids\*"
  Delete "$INSTDIR\syscheck\*"
  Delete "$INSTDIR\shared\*"
  Delete "$INSTDIR\active-response\bin\*"
  Delete "$INSTDIR\active-response\*"
  Delete "$INSTDIR"

  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\ossec\*.*"
  Delete "$SMPROGRAMS\ossec\*"

  ; Remove directories used
  RMDir "$SMPROGRAMS\ossec"
  RMDir "$INSTDIR\shared"
  RMDir "$INSTDIR\syscheck"
  RMDir "$INSTDIR\rids"
  RMDir "$INSTDIR\active-response\bin"
  RMDir "$INSTDIR\active-response"
  RMDir "$INSTDIR"

SectionEnd
