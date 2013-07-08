!define MYPRODUCT "BayCom MXPTiny"
!define MYVERSION "0.9"
!define VER_MAJOR 0
!define VER_MINOR 9

;OutFile "MXPTiny-${VER_MAJOR}${VER_MINOR}.exe"
OutFile "MXPTinyinstall.exe"
Icon res/MXPTiny.ico

;--------------------------------

;Page license
Page components
Page directory
Page instfiles

UninstPage uninstConfirm
UninstPage instfiles

; First is default
LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Dutch.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\French.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\German.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Korean.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Russian.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Spanish.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Swedish.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\TradChinese.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\SimpChinese.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\Slovak.nlf"

;LicenseData "license.txt"


Name "${MYPRODUCT} ${MYVERSION}"
Function .onInit

	;Language selection dialog

	Push ""
	Push ${LANG_ENGLISH}
	Push English
	Push ${LANG_DUTCH}
	Push Dutch
	Push ${LANG_FRENCH}
	Push French
	Push ${LANG_GERMAN}
	Push German
	Push ${LANG_KOREAN}
	Push Korean
	Push ${LANG_RUSSIAN}
	Push Russian
	Push ${LANG_SPANISH}
	Push Spanish
	Push ${LANG_SWEDISH}
	Push Swedish
	Push ${LANG_TRADCHINESE}
	Push "Traditional Chinese"
	Push ${LANG_SIMPCHINESE}
	Push "Simplified Chinese"
	Push ${LANG_SLOVAK}
	Push Slovak
	Push A ; A means auto count languages
	       ; for the auto count to work the first empty push (Push "") must remain
	LangDLL::LangDialog "Installer Language" "Please select the language of the installer"

	Pop $LANGUAGE
	StrCmp $LANGUAGE "cancel" 0 +2
		Abort
FunctionEnd

InstallDir "$PROGRAMFILES\BayCom\MXPTiny"
InstallDirRegKey HKEY_LOCAL_MACHINE "SOFTWARE\BayCom\MXPTiny" ""

Section "Executables and DLLs (Required)" default
SectionIn RO

retry:
FindProcDLL::FindProc "MXPTiny.exe"
IntCmp $R0 1 0 notRunning
    MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "MXPTiny is running. Please close it first" /SD IDCANCEL IDRETRY retry IDCANCEL cancel
cancel:
    Abort
notRunning:

SetOutPath "$INSTDIR"

; add files / whatever that need to be installed here.
WriteRegStr HKEY_LOCAL_MACHINE "SOFTWARE\BayCom\MXPTiny" "" "$INSTDIR"
WriteRegStr HKEY_LOCAL_MACHINE "Software\BayCom\MXPTiny" "Installer Language" "$LANGUAGE"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\MXPTiny" "DisplayName" "MXPTiny (remove only)"
WriteRegStr HKEY_LOCAL_MACHINE "Software\Microsoft\Windows\CurrentVersion\Uninstall\MXPTiny" "UninstallString" '"$INSTDIR\uninst.exe"'


; write out uninstaller
WriteUninstaller "$INSTDIR\uninst.exe"

File "release\MXPTiny.exe" 
File "Preview.xspf"

File "install\vcredist_x86.exe"

; Check if VC10 Redist package is installed
ClearErrors
ReadRegDWORD $0 HKLM SOFTWARE\Microsoft\VisualStudio\10.0\VC\VCRedist\x86\ "Installed"
StrCmp $0 "1" keyexist

ExecWait '"$INSTDIR\vcredist_x86.exe" /passive /norestart'  
keyexist:

Delete "$INSTDIR\vcredist_x86.exe"

SetOutPath "$INSTDIR"

;Create Shortcuts
SetShellVarContext all
CreateDirectory "$SMPROGRAMS\MXPTiny"
CreateShortCut "$SMPROGRAMS\MXPTiny\Uninstall.lnk" "$INSTDIR\uninst.exe" "" "$INSTDIR\uninst.exe" 0
CreateShortCut "$SMPROGRAMS\MXPTiny\MXPTiny.lnk" "$INSTDIR\MXPTiny.exe" "" "$INSTDIR\MXPTiny.exe" 0
CreateShortCut "$SMPROGRAMS\MXPTiny\Preview.lnk" "$INSTDIR\Preview.xspf" "" "$INSTDIR\Preview.xspf" 0
SectionEnd ; end of default section

Section "Desktop Menu Shortcut" desktop
CreateShortCut "$DESKTOP\MXPTiny.lnk" "$INSTDIR\MXPTiny.exe" "" "$INSTDIR\MXPTiny.exe" 0
CreateShortCut "$DESKTOP\Preview.lnk" "$INSTDIR\Preview.xspf" "" "$INSTDIR\Preview.xspf" 0
SectionEnd

Section "Quick Launch Menu Shortcut" quicklaunch
SetShellVarContext current
CreateShortCut "$QUICKLAUNCH\MXPTiny.lnk" "$INSTDIR\MXPTiny.exe" "" "$INSTDIR\MXPTiny.exe" 0
CreateShortCut "$QUICKLAUNCH\Preview.lnk" "$INSTDIR\Preview.xspf" "" "$INSTDIR\Preview.xspf" 0
SectionEnd
 
Section "-post"
SetShellVarContext all

ExecShell open '$SMPROGRAMS\MXPTiny'

Sleep 500
BringToFront

SectionEnd

Function un.onInit

;Get language from registry
ReadRegStr $LANGUAGE HKEY_LOCAL_MACHINE "Software\BayCom\MXPTiny" "Installer Language"
  
FunctionEnd

Section Uninstall

retry:
FindProcDLL::FindProc "MXPTiny.exe"
IntCmp $R0 1 0 notRunning
    MessageBox MB_RETRYCANCEL|MB_ICONEXCLAMATION "MXPTiny is running. Please close it first" /SD IDCANCEL IDRETRY retry IDCANCEL cancel
cancel:
    Abort
notRunning:

;Remove all files
Delete $INSTDIR\iconengines\*
Delete $INSTDIR\*
RMDir "$INSTDIR\iconengines"
RMDir "$INSTDIR"

; add delete commands to delete whatever files/registry keys/etc you installed here.
Delete "$INSTDIR\uninst.exe"
DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\BayCom\MXPTiny"
DeleteRegKey HKEY_LOCAL_MACHINE "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\MXPTiny"

;Remove Shortcuts from Menu
SetShellVarContext all
Delete "$SMPROGRAMS\MXPTiny\*.*"
Delete "$DESKTOP\MXPTiny.lnk"
Delete "$DESKTOP\Preview.lnk"
RMDir "$SMPROGRAMS\MXPTiny"

SetShellVarContext current
Delete "$QUICKLAUNCH\MXPTiny.lnk"
Delete "$QUICKLAUNCH\Preview.lnk"

SectionEnd ; end of uninstall section

; eof
