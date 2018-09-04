#################################################################
# test_dui_gridctrl.tcl
# Author     : blueant
# Version    : 1.0
# Date       : 2017-01-24
# Description: test tclduivision dialog
#################################################################

# ��չ����:dui::control
# getcontrol controlName ?-parentDialog parentDialog?
# getcontrol controlName ?-parentPopup parentPopup?
# getcontrol controlName ?-parent parentControl?
# getcontrol controlName -dialog dialogName
# getvalue controlObj
# setvalue controlObj value
# setattribute controlObj attrName attrValue
# getposstr controlObj
# setposstr controlObj pos

# ��չ����:dui::gridctrl
# addrow controlObj row ?-image image? ?-check check? -items itemList
# deleterow controlObj row
# addcontrol controlObj row item controlClass controlName ?-dialog dialogObj?
# getrowcount controlObj
# getrowcheck controlObj row

# ��չ����:dui::handler
# register duiObj handlerObj
# add handlerObj
# remove handlerObj

# ��չ����:dui::dialog
# xmlTemplate ?-name dlgName? ?-modal isModule? ?-handler handlerObj? ?-control controlName? ?-init?
# show xmlTemplate dlgName isModule
# messagebox text caption ?type? ?-width width? ?-height height?
# showwindow dlgObj show|hide
# notifymsg msg �Ccaption caption ?-delay delayTime? ?-width width? ?-height height? ?type?
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
	protected variable _progressIndex;	# �������ĵ�ǰ����
	
	### public methods ###
	public method  OnInit {};
	public method  OnTimer {name};		# ��ʱ��������
	public method  OnButtonOK {name msg wParam lParam};
	public method  OnButtonAddLine1 {name msg wParam lParam};
	public method  OnButtonAddLine2 {name msg wParam lParam};
}

#-------------------------------------------------------------
#	��Ԫ���캯��
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::constructor {{xmlTemplate ""}} {
	chain $xmlTemplate
	SetLogLevel DEBUG
	# ����ؼ�DUI��Ϣ��iTclӳ�䷽��
	MsgMethod "button.ok.gridctrl" $DUIVISION::MSG_BUTTON_UP OnButtonOK
	MsgMethod "button.addline1" $DUIVISION::MSG_BUTTON_UP OnButtonAddLine1
	MsgMethod "button.addline2" $DUIVISION::MSG_BUTTON_UP OnButtonAddLine2
	
	set _progressIndex 0
}

#-------------------------------------------------------------
#	��Ԫ��������
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::destructor {} {
	StopTimer "timer_animate_progress"
	chain;
}

#-------------------------------------------------------------
#	��ʼ��
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnInit {} {
	chain;
	puts "TDuiGridCtrlDialogHandler::OnInit"
	#puts "Start Timer:timer_1 5000"
	StartTimer "timer_animate_progress" 1000
	
	# ���gridctrl��
	set gridctrl [GetControl "gridctrl_test"]
	set rowItems [list [list "����" "��OnInit��������ӵ���"] \
					[list "��ǰ��1.0.0" "���£�2.0.0"] \
					[list "10M"] \
					[list "һ����װ"]]
	dui::gridctrl addrow $gridctrl "" -check 0 -image "skins/icon/softmgr.png" -items $rowItems
}

#-------------------------------------------------------------
#	Button ����� �����Ϣ
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnButtonAddLine1 {name msg wParam lParam} {
	puts "TDuiGridCtrlDialogHandler::OnButtonAddLine1"

	#set gridctrl [dui::control getcontrol "gridctrl_test" -parentDialog $_objDuiObject]
	#puts $gridctrl
	# gridctrl�����
	set gridctrl [GetControl "gridctrl_test"]
	set rowCount [dui::gridctrl getrowcount $gridctrl]
	set rowItems [list [list "��̬�����" "�������а�ť��ӵ���" 0 -1 "128,128,0"] \
					[list "��ǰ��2.0.0" "���£�3.0.0" 1 0] \
					[list "20M"]]
	dui::gridctrl addrow $gridctrl "" -check 0 -image "skins/icon/softmgr.png" -items $rowItems
	# ���һ������ӿؼ�
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
#	Button ��������� �����Ϣ
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnButtonAddLine2 {name msg wParam lParam} {
	puts "TDuiGridCtrlDialogHandler::OnButtonAddLine2"

	set gridctrl [GetControl "gridctrl_test"]
	set rowItems [list [list "��̬�����" "�����������а�ť��ӵ���" 0 -1 "128,0,0"] \
					[list "��ǰ��3.0.0" "���£�4.0.0" 1 1] \
					[list "30M"] \
					[list "����" "dlg:dlg_about" "-link"]]
	dui::gridctrl addrow $gridctrl "" -check 0 \
				-image "skins/icon/scriptnet.png" \
				-backColor "50,128,128,0" \
				-items $rowItems
}

#-------------------------------------------------------------
#	DUI��ʱ��������
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnTimer {name} {
	chain $name;
	#puts "TDuiGridCtrlDialogHandler::OnTimer $name"
	if {$name == "timer_animate_progress"} {
		incr _progressIndex
		if {$_progressIndex > 10} {
			set _progressIndex 0
		}
		
		# ˢ�½�����
		set progress_test [GetControl "progress_test"]
		dui::control setattribute $progress_test value [expr $_progressIndex*10] -update
	}
}

#-------------------------------------------------------------
#	Button OK�����Ϣ
#-------------------------------------------------------------
body TDuiGridCtrlDialogHandler::OnButtonOK {name msg wParam lParam} {
	puts "TDuiGridCtrlDialogHandler::OnButtonOK"
	
	# ��ȡѡ���˵ڼ���
	set gridctrl [GetControl "gridctrl_test"]
	set rowCount [dui::gridctrl getrowcount $gridctrl]
	for {set i 0} {$i < $rowCount} {incr i} {
		#
		set rowCheck [dui::gridctrl getrowcheck $gridctrl $i]
		if {$rowCheck == 1} {
			puts "ѡ���˵� [expr $i+1] ��"
		}
	}
	
	# ��ȡ������ֵ
	set edit_gridctrl_1 [GetControl "edit_gridctrl_1"]
	set editValue [dui::control getvalue $edit_gridctrl_1]
	puts "���������ݣ�$editValue"
	
	# ��ȡ����������
	#set progress_test [GetControl "progress_test"]
	#set progressValue [dui::control getvalue $progress_test]
	#puts "�������Ľ��ȣ�$progressValue"
	
	# �رնԻ���
	DoOK
	
	#dui::dialog messagebox "�Ի���ر�" "��ʾ"
}

# ��ʾָ����xml�Ի������ļ�
set current_path [file dirname [info script]]
set xml_file_gridctrl "$current_path/dlg_gridctrl.xml"
puts $xml_file_gridctrl
TDuiGridCtrlDialogHandler objDuiGridCtrlHandler $xml_file_gridctrl
objDuiGridCtrlHandler ShowDialog
delete object objDuiGridCtrlHandler
