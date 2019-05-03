#################################################################
# test_dui_gridctrl.tcl
# Author     : blueant
# Version    : 1.0
# Date       : 2017-01-24
# Description: test tclduivision dialog
#################################################################

# 扩展命令:dui::control
# getcontrol controlName ?-parentDialog parentDialog?
# getcontrol controlName ?-parentPopup parentPopup?
# getcontrol controlName ?-parent parentControl?
# getcontrol controlName -dialog dialogName
# getvalue controlObj
# setvalue controlObj value
# setattribute controlObj attrName attrValue
# getposstr controlObj
# setposstr controlObj pos

# 扩展命令:dui::gridctrl
# addrow controlObj row ?-image image? ?-check check? -items itemList
# deleterow controlObj row
# addcontrol controlObj row item controlClass controlName ?-dialog dialogObj?
# getrowcount controlObj
# getrowcheck controlObj row

# 扩展命令:dui::handler
# register duiObj handlerObj
# add handlerObj
# remove handlerObj

# 扩展命令:dui::dialog
# xmlTemplate ?-name dlgName? ?-modal isModule? ?-handler handlerObj? ?-control controlName? ?-init?
# show xmlTemplate dlgName isModule
# messagebox text caption ?type? ?-width width? ?-height height?
# showwindow dlgObj show|hide
# notifymsg msg –caption caption ?-delay delayTime? ?-width width? ?-height height? ?type?
# add duiDlg
# remove duiDlg
# get dlgName
# close dlgObj ?-ok|cancel|close|yes|no?

package require TclDuiVision

#-------------------------------------------------------------
# TDuiGridCtrlDialogHandler class define
#-------------------------------------------------------------
class  TDuiGridCtrlDialogHandler {
	inherit TDuiDialogHandler
	
	constructor {{xmlTemplate ""}} {};
	destructor {};
	
	### data member ###
	protected variable _progressIndex;	# 进度条的当前进度
	
	### public methods ###
	public method  OnInit {};
	public method  OnTimer {name};		# 定时器处理函数
	public method  OnButtonOK {name msg wParam lParam};
	public method  OnButtonAddLine1 {name msg wParam lParam};
	public method  OnButtonAddLine2 {name msg wParam lParam};
}

#-------------------------------------------------------------
#	单元构造函数
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::constructor {{xmlTemplate ""}} {
	chain $xmlTemplate
	SetLogLevel DEBUG
	# 定义控件DUI消息的iTcl映射方法
	MsgMethod "button.ok.gridctrl" $DUIVISION::MSG_BUTTON_UP OnButtonOK
	MsgMethod "button.addline1" $DUIVISION::MSG_BUTTON_UP OnButtonAddLine1
	MsgMethod "button.addline2" $DUIVISION::MSG_BUTTON_UP OnButtonAddLine2
	
	set _progressIndex 0
}

#-------------------------------------------------------------
#	单元析构函数
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::destructor {} {
	StopTimer "timer_animate_progress"
	chain;
}

#-------------------------------------------------------------
#	初始化
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnInit {} {
	chain;
	puts "TDuiGridCtrlDialogHandler::OnInit"
	#puts "Start Timer:timer_1 5000"
	StartTimer "timer_animate_progress" 1000
	
	# 添加gridctrl行
	set gridctrl [GetControl "gridctrl_test"]
	set rowItems [list [list "测试" "在OnInit函数中添加的项\r\nScript.NET是蓝蚂蚁工作室开发的脚本语言集成开发环境，支持Tcl、Python、Perl、Ruby、Lua、NSIS等脚本语言开发调试"] \
					[list "当前：1.0.0" "最新：2.0.0"] \
					[list "10M"] \
					[list "一键安装"]]
	dui::gridctrl addrow $gridctrl "" -check 0 -image "skins/icon/softmgr.png" -items $rowItems
}

#-------------------------------------------------------------
#	Button 添加行 点击消息
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnButtonAddLine1 {name msg wParam lParam} {
	puts "TDuiGridCtrlDialogHandler::OnButtonAddLine1"

	#set gridctrl [dui::control getcontrol "gridctrl_test" -parentDialog $_objDuiObject]
	#puts $gridctrl
	# gridctrl添加行
	set gridctrl [GetControl "gridctrl_test"]
	set rowCount [dui::gridctrl getrowcount $gridctrl]
	set rowItems [list [list "动态添加行" "点击添加行按钮添加的行" 0 -1 "128,128,0"] \
					[list "当前：2.0.0" "最新：3.0.0" 1 0] \
					[list "20M"]]
	dui::gridctrl addrow $gridctrl "" -check 0 -image "skins/icon/softmgr.png" -items $rowItems
	# 最后一列添加子控件
	set control [dui::gridctrl addcontrol $gridctrl $rowCount 3 "button" "grid_btn_resume$rowCount" "10,20,27,37"]
	dui::control setattribute $control image "skins/qq/softtastmgr_btn_resume.png"
	dui::control setattribute $control showfocus 0
	#dui::control setposstr $control "10,20,27,37"
	set control [dui::gridctrl addcontrol $gridctrl $rowCount 3 "button" "grid_btn_cancel$rowCount" "30,20,47,37"]
	dui::control setattribute $control image "skins/qq/softtastmgr_btn_cancel.png"
	dui::control setattribute $control showfocus 0
	
	# set control [dui::gridctrl addcontrol $gridctrl $rowCount 0 "edit" "edit_$rowCount" "" -dialog $_objDuiObject]
	# dui::control setattribute $control skin "skin:IDB_EDIT"
	# dui::control setattribute $control title "100"
	# dui::control setposstr $control "220, 10, -20, 40"
	
	# set control [dui::gridctrl addcontrol $gridctrl $rowCount-1 0 "text" "text_$rowCount" "120, 10, -20, 40"]
	# dui::control setattribute $control title "==========" -update
}

#-------------------------------------------------------------
#	Button 添加链接行 点击消息
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnButtonAddLine2 {name msg wParam lParam} {
	puts "TDuiGridCtrlDialogHandler::OnButtonAddLine2"

	set gridctrl [GetControl "gridctrl_test"]
	set rowItems [list [list "动态添加行" "点击添加链接行按钮添加的行" 0 -1 "128,0,0"] \
					[list "当前：3.0.0" "最新：4.0.0" 1 1] \
					[list "30M"] \
					[list "链接" "dlg:dlg_about" "-link"]]
	dui::gridctrl addrow $gridctrl "" -check 0 \
				-image "skins/icon/scriptnet.png" \
				-backColor "50,128,128,0" \
				-items $rowItems
}

#-------------------------------------------------------------
#	DUI定时器处理函数
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnTimer {name} {
	chain $name;
	#puts "TDuiGridCtrlDialogHandler::OnTimer $name"
	if {$name == "timer_animate_progress"} {
		incr _progressIndex
		if {$_progressIndex > 10} {
			set _progressIndex 0
		}
		
		# 刷新进度条
		set progress_test [GetControl "progress_test"]
		dui::control setattribute $progress_test value [expr $_progressIndex*10] -update
	}
}

#-------------------------------------------------------------
#	Button OK点击消息
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnButtonOK {name msg wParam lParam} {
	puts "TDuiGridCtrlDialogHandler::OnButtonOK"
	
	# 获取选择了第几行
	set gridctrl [GetControl "gridctrl_test"]
	set rowCount [dui::gridctrl getrowcount $gridctrl]
	for {set i 0} {$i < $rowCount} {incr i} {
		#
		set rowCheck [dui::gridctrl getrowcheck $gridctrl $i]
		if {$rowCheck == 1} {
			puts "选择了第 [expr $i+1] 行"
		}
	}
	
	# 获取输入框的值
	set edit_gridctrl_1 [GetControl "edit_gridctrl_1"]
	set editValue [dui::control getvalue $edit_gridctrl_1]
	puts "输入框的内容：$editValue"
	
	# 获取进度条进度
	#set progress_test [GetControl "progress_test"]
	#set progressValue [dui::control getvalue $progress_test]
	#puts "进度条的进度：$progressValue"
	
	# 关闭对话框
	DoOK
	
	#dui::dialog messagebox "对话框关闭" "提示"
}

# 显示指定的xml对话框定义文件
set current_path [file dirname [info script]]
set xml_file_gridctrl "$current_path/dlg_gridctrl.xml"
puts $xml_file_gridctrl
TDuiGridCtrlDialogHandler objDuiGridCtrlHandler $xml_file_gridctrl
objDuiGridCtrlHandler ShowDialog
delete object objDuiGridCtrlHandler
