; 该脚本使用 HM VNISEdit 脚本编辑器向导产生

!include "LogicLib.nsh"
!include "WordFunc.nsh"

; 安装程序初始定义常量
!define PRODUCT_NAME "DuiVisionWizard"
!define PRODUCT_DISPLAY_NAME "DuiVision界面库VC向导安装程序"
!define PRODUCT_VERSION "1.9.0.1"
!define PRODUCT_PUBLISHER "蓝蚂蚁工作室"

SetCompressor lzma

; ------ MUI 现代界面定义 (1.67 版本以上兼容) ------
!include "MUI.nsh"

!define MUI_CUSTOMFUNCTION_GUIINIT "onGUIInit"

; MUI 预定义常量
;用户要关闭安装程序时, 显示一个警告消息框
!define MUI_ABORTWARNING
;安装和卸载图标设置
!define MUI_ICON "Icons\orange-install.ico"

;页眉图像设置
!define MUI_HEADERIMAGE
!define MUI_HEADERIMAGE_BITMAP "Icons\header-l.bmp"
;欢迎页面图片设置
!define MUI_WELCOMEFINISHPAGE_BITMAP "Icons\wizard.bmp"

; 欢迎页面
!insertmacro MUI_PAGE_WELCOME

; 组件选择页面
!insertmacro MUI_PAGE_COMPONENTS

; 安装目录选择页面
;!insertmacro MUI_PAGE_DIRECTORY

; 安装过程页面
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

; 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "SimpChinese"

; ------ MUI 现代界面定义结束 ------

# 替换文件内容用的宏
!macro ReplaceConfig FileName StringOld StringNew
    Push `${StringOld}`     #text to be replaced
    Push `${StringNew}`     #replace with
    Push all                #replace all occurrences
    Push all                #replace all occurrences
    Push `${FileName}`      #file to replace in
    Call AdvReplaceInFile
!macroend

;Name and file
 Name "${PRODUCT_DISPLAY_NAME} ${PRODUCT_VERSION}"
 OutFile "DuiVisionWizardSetup.exe"

;Default installation folder
 ;InstallDir "$PROGRAMFILES\Microsoft Visual Studio 10.0\VC\vcprojects"
 ;InstallDir "$PROGRAMFILES\Microsoft Visual Studio 9.0\VC\vcprojects"

;Define issue directory, export from svn repos
!define ISSUEDIR "Issue"

ShowInstDetails hide

BrandingText ${PRODUCT_PUBLISHER}

;基本运行库
;Section
;  SectionIn RO
  
;  IfFileExists $INSTDIR\emptyproj.vsz step_install
;    MessageBox MB_ICONINFORMATION|MB_OK "未找到VC2008的安装路径，无法安装此向导！"
;    Abort
  
; step_install:
;  SetOverwrite ifnewer
  
;  SetOutPath "$INSTDIR\"
;  File /nonfatal /r "${ISSUEDIR}\*.*"
  
  ;替换vsz文件中的VC路径信息和引擎版本(引擎版本对应表：10.0-VS2010,9.0-VS2008,8.0-VS2005,7.0-VC2003)
  ;!insertmacro ReplaceConfig "$INSTDIR\DuiVisionWizard.vsz" "$$$$ENGINE_VER$$$$" "10"
  ;!insertmacro ReplaceConfig "$INSTDIR\DuiVisionWizard.vsz" "$$$$INST_DIR$$$$" $INSTDIR
  
;SectionEnd

Section "VC2008" SEC_VC2008
  IfFileExists "$PROGRAMFILES\Microsoft Visual Studio 9.0\VC\vcprojects\emptyproj.vsz" step_install step_continue
    MessageBox MB_ICONINFORMATION|MB_OK "未找到VC2008的安装路径，无法安装此向导！"
 step_install:
  SetOverwrite ifnewer
  SetOutPath "$PROGRAMFILES\Microsoft Visual Studio 9.0\VC\vcprojects"
  File /nonfatal /r "${ISSUEDIR}\*.*"
 step_continue:
SectionEnd

Section "VC2010" SEC_VC2010
  IfFileExists "$PROGRAMFILES\Microsoft Visual Studio 10.0\VC\vcprojects\emptyproj.vsz" step_install step_continue
    MessageBox MB_ICONINFORMATION|MB_OK "未找到VC2010的安装路径，无法安装此向导！"
 step_install:
  SetOverwrite ifnewer
  SetOutPath "$PROGRAMFILES\Microsoft Visual Studio 10.0\VC\vcprojects"
  File /nonfatal /r "${ISSUEDIR}\*.*"
 step_continue:
SectionEnd

#-- 根据 NSIS 脚本编辑规则，所有 Function 区段必须放置在 Section 区段之后编写，以避免安装程序出现未可预知的问题。--#

; 区段组件描述
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT "VC2008" "安装向导到VC2008"
  !insertmacro MUI_DESCRIPTION_TEXT "VC2010" "安装向导到VC2010"
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Function .onInit
FunctionEnd

Function onGUIInit
FunctionEnd

;替换文件中的字符串
Function AdvReplaceInFile
    Exch $0  ;file to replace in
    Exch
    Exch $1  ;number to replace after
    Exch
    Exch 2
    Exch $2  ;replace and onwards
    Exch 2
    Exch 3
    Exch $3  ;replace with
    Exch 3
    Exch 4
    Exch $4  ;to replace
    Exch 4
    Push $5  ;minus count
    Push $6  ;universal
    Push $7  ;end string
    Push $8  ;left string
    Push $9  ;right string
    Push $R0 ;file1
    Push $R1 ;file2
    Push $R2 ;read
    Push $R3 ;universal
    Push $R4 ;count (onwards)
    Push $R5 ;count (after)
    Push $R6 ;temp file name
    GetTempFileName $R6
    FileOpen $R1 $0 r   ;file to search in
    FileOpen $R0 $R6 w  ;temp file
    StrLen $R3 $4
    StrCpy $R4 -1
    StrCpy $R5 -1
    loop_read:
    ClearErrors
    FileRead $R1 $R2 ;read line
    IfErrors exit
    StrCpy $5 0
    StrCpy $7 $R2
    loop_filter:
    IntOp $5 $5 - 1
    StrCpy $6 $7 $R3 $5 ;search
    StrCmp $6 "" file_write2
    StrCmp $6 $4 0 loop_filter
    StrCpy $8 $7 $5     ;left part
    IntOp $6 $5 + $R3
    IntCmp $6 0 is0 not0
    is0:
    StrCpy $9 ""
    Goto done
    not0:
    StrCpy $9 $7 "" $6  ;right part
    done:
    StrCpy $7 $8$3$9    ;re-join
    IntOp $R4 $R4 + 1
    StrCmp $2 all file_write1
    StrCmp $R4 $2 0 file_write2
    IntOp $R4 $R4 - 1
    IntOp $R5 $R5 + 1
    StrCmp $1 all file_write1
    StrCmp $R5 $1 0 file_write1
    IntOp $R5 $R5 - 1
    Goto file_write2
    file_write1:
    FileWrite $R0 $7  ;write modified line
    Goto loop_read
    file_write2:
    FileWrite $R0 $R2 ;write unmodified line
    Goto loop_read
    exit:
    FileClose $R0
    FileClose $R1
    SetDetailsPrint none
    Delete $0
    Rename $R6 $0
    Delete $R6
    SetDetailsPrint both
    Pop $R6
    Pop $R5
    Pop $R4
    Pop $R3
    Pop $R2
    Pop $R1
    Pop $R0
    Pop $9
    Pop $8
    Pop $7
    Pop $6
    Pop $5
    Pop $0
    Pop $1
    Pop $2
    Pop $3
    Pop $4
FunctionEnd
