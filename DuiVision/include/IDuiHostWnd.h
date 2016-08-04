////////////////////////////////////////////////////////////////////////////
//	File:		IDuiHostWnd.h
//	Version:	1.0.0.0
/*
  Copyright (c) 2014-2016 by blueant <script.net@gmail.net>
  This file is part of the DuiVision library. https://github.com/blueantst/DuiVision
*/
#if !defined(__INTERFACE_IDUIHOSTWND_H_INCLUDED__)
#define __INTERFACE_IDUIHOSTWND_H_INCLUDED__

// �ӿڶ���ID����
#define IID_IDuiHostWnd _T("IDuiHostWnd")	// ����������ڹ��ܽӿ�

class CVciMessage;

// ����������ڹ��ܽӿ�
interface IDuiHostWnd : public IUnknown
{
	//{{VCI_INTERFACE_BEGIN
	// ƽ̨����
	virtual CString __stdcall GetAppName() = 0;				// ��ȡӦ�ó�����
	virtual CString __stdcall GetPlatPath() = 0;			// ��ȡƽ̨·��
	virtual CString __stdcall GetPlatVersion() = 0;			// ��ȡƽ̨�汾
	virtual int __stdcall GetCurrentLanguage() = 0;			// ��ȡ��ǰ����
	virtual CString __stdcall GetPlatRegistry() = 0;		// ��ȡƽ̨ע����·��
	virtual CString __stdcall GetPlatCopyRight() = 0;		// ��ȡƽ̨��Ȩ�ַ���
	virtual CString __stdcall GetPlatHomeURL() = 0;			// ��ȡ��ҳURL
	virtual CString __stdcall GetPlatDownloadURL() = 0;		// ��ȡ����URL
	virtual int  __stdcall SendMessage(CVciMessage* pIn, CVciMessage* ppOut) = 0;	// ������Ϣ
	virtual int  __stdcall ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut) = 0;	// ƽ̨����Ϣ����
	virtual int  __stdcall SendCommand(int nCmd, WPARAM wParam, LPARAM lParam) = 0;	// ����ƽ̨����
	virtual BOOL __stdcall SendCommand(int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult) = 0;	// ����ƽ̨����

	// DuiVisionϵͳ����
	virtual int __stdcall GetAppID() = 0;	// ��ȡDuiVisionӦ��ID

	// ���ڲ���
	virtual BOOL __stdcall GetWindowBkInfo(int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile) = 0;	// ��ȡ���ڱ�����Ϣ
	virtual BOOL __stdcall SetWindowBkInfo(int nType, int nIDResource, COLORREF clr, LPCTSTR lpszImgFile) = 0;	// ���ô��ڱ�����Ϣ
	virtual void __stdcall ClientToScreen(LPPOINT lpPoint) = 0;		// ����ת��Ϊ��Ļ����
	virtual HWND __stdcall GetPaintHWnd() = 0;		// ��ȡ�������ڵľ��

	// Tooltip����
	virtual void __stdcall SetTooltip(int nCtrlID, LPCTSTR lpszTooltip, CRect rect, int nTipWidth) = 0;	// ����Tooltip
	virtual void __stdcall ClearTooltip() = 0;	// ���Tooltip
	virtual void __stdcall SetTooltipCtrlID(int nTooltipCtrlID) = 0;	// ���õ�ǰTooltip�ؼ�ID
	virtual int  __stdcall GetTooltipCtrlID() = 0;	// ��ȡ��ǰTooltip�ؼ�ID

	// ���̲���

	// ��־

	//}}VCI_INTERFACE_END
};


#endif // !defined(__INTERFACE_IDUIHOSTWND_H_INCLUDED__)
