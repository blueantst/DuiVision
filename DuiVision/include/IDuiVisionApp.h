////////////////////////////////////////////////////////////////////////////
//	File:		IDuiVisionApp.h
//	Version:	1.0.0.0
//	Created:	2017-02-12
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	Interface define of DuiVisionApp.
////////////////////////////////////////////////////////////////////////////
#ifndef __IDUIVISIONAPP_H_
#define __IDUIVISIONAPP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// �ӿ�ʹ�õ��ඨ��
class CVciMessage;
// ����DuiVision�����
class CDuiObject;
class CDuiObjectInfo;
class CControlBase;
class CDuiHandler;
class CDlgBase;

interface IDuiVisionApp : public IUnknown
{
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

	// ��־����
	virtual void __stdcall LogEvent(int nLevel, CString strInfo) = 0; // ��¼��־
	virtual void __stdcall LogEventModule(int nLevel, CString strModule, CString strInfo) = 0; // ��¼��־(��ģ����Ϣ)

	// DuiVision����
	virtual BOOL __stdcall IsEnableDuiVision() = 0;	// �Ƿ�֧��DuiVision��ز���
	virtual CString __stdcall GetDuiResourcePath() = 0;	// ��ȡDuiVision��resourceĿ¼
	virtual BOOL __stdcall DuiLoadResourceXml(CString strResFile, CString strStyle) = 0;	// ����XML��Դ�ļ�(����ָ�����صķ��)
	virtual CString __stdcall DuiGetConfig(CString strName) = 0;	// ��ȡϵͳ������Ϣ
	virtual CString __stdcall DuiGetXmlFile(CString strName) = 0;	// ��ȡXML�ļ�
	virtual CString __stdcall DuiGetSkin(CString strName) = 0;	// ��ȡSkin
	virtual CString __stdcall DuiGetString(CString strName) = 0;	// ��ȡ�ַ���ֵ
	virtual void __stdcall DuiSetString(CString strName, CString strValue) = 0;	// �����ַ���ֵ
	virtual void __stdcall ParseDuiString(CString& strString) = 0;	// �����ַ������滻���е��滻����
	virtual void __stdcall RegisterDuiControl(CDuiObjectInfo* pDuiObjectInfo) = 0;	// ע��ؼ�
	virtual BOOL __stdcall UnRegisterDuiControl(LPCTSTR lpszName) = 0;	// ж�ؿؼ�
	virtual CControlBase* __stdcall DuiCreateControlByName(LPCTSTR lpszName, HWND hWnd, CDuiObject* pParentObject) = 0;	// ���ݿؼ����������ؼ�ʵ��
	virtual CControlBase* __stdcall DuiGetControl(CDuiObject* pDuiObject, CString strControlName) = 0;	// ��ȡ�ӿؼ�
	virtual CControlBase* __stdcall DuiGetDialogControl(CString strDialogName, CString strControlName) = 0;	// ��ȡ�Ի����ӿؼ�
	virtual BOOL __stdcall RegisterDuiHandler(CDuiObject* pDuiObject, CDuiHandler* pDuiHandler, CString strControlName, BOOL bInit) = 0;	// ��ָ����DUI����ע���¼��������
	virtual void __stdcall AddDuiHandler(CDuiHandler* pDuiHandler) = 0;	// ���DUI�¼��������
	virtual void __stdcall RemoveDuiHandler(CDuiHandler* pDuiHandler) = 0;	// ɾ��DUI�¼��������
	virtual CDlgBase* __stdcall CreateDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule, BOOL bAdd) = 0;	// ����DUI�Ի���
	virtual int __stdcall ShowDuiDialog(LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule) = 0;	// ��ʾDUI�Ի���
	virtual int __stdcall DuiMessageBox(CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption, UINT uType, int nWidth, int nHeight) = 0;	// ��ʾDUI MessageBox
	virtual void __stdcall AddDuiDialog(CDlgBase* pDuiDialog) = 0;	// ���DUI�Ի���
	virtual void __stdcall RemoveDuiDialog(CDlgBase* pDuiDialog) = 0;	// ɾ��DUI�Ի���
	virtual CDlgBase* __stdcall GetDuiDialog(CString strName) = 0;	// ��ȡDUI�Ի���
	virtual void __stdcall AddDuiNotifyMsgTask(CString strMsg, UINT uNotifyType, CString strCaption, int nDelayTime, int nWidth, int nHeight) = 0;	// �����ʾ��������
	virtual UINT __stdcall AddDuiTimer(UINT uTimerResolution, CString strTimerName) = 0;	// ���DUI��ʱ��
	virtual void __stdcall RemoveDuiTimer(CString strTimerName) = 0;	// ɾ��DUI��ʱ��
};


#endif // !defined(__IDUIVISIONAPP_H_)