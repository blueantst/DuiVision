////////////////////////////////////////////////////////////////////////////
//	File:		DuiVisionApp.h
//	Version:	1.0.0.0
//	Created:	2017-02-12
//
//	Author:		blueant
//	E-mail:		script.net@gmail.com
//
//	header of Platform DuiVisionApp interface object.
////////////////////////////////////////////////////////////////////////////

#if !defined(__DUIVISIONAPP_H_)
#define __DUIVISIONAPP_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DuiVisionApp.h : header file
//

#include "vcicomm.h"
#include "IDuiVisionApp.h"

/////////////////////////////////////////////////////////////////////////////
// CDuiVisionApp command target

class CDuiVisionApp : public CVisualComponent
{
public:
	CDuiVisionApp();           // protected constructor used by dynamic creation
	virtual ~CDuiVisionApp();

// Attributes
public:

// Interface define
public:
	BEGIN_INTERFACE_PART(VciControl, IVciControl)
		CONTROL_INTERFACE_DEFINE
	END_INTERFACE_PART(VciControl)
	EXPORT_CONTROL_INTERFACE_PART(VciControl)

    BEGIN_INTERFACE_PART(DuiVisionApp, IDuiVisionApp)

		// ƽ̨����
		STDMETHOD_( CString , GetAppName) ();				// ��ȡӦ�ó�����
		STDMETHOD_( CString , GetPlatPath) ();				// ��ȡƽ̨·��
		STDMETHOD_( CString , GetPlatVersion) ();			// ��ȡƽ̨�汾
		STDMETHOD_( int , GetCurrentLanguage) ();			// ��ȡ��ǰ����
		STDMETHOD_( CString , GetPlatRegistry) ();			// ��ȡƽ̨ע����·��
		STDMETHOD_( CString , GetPlatCopyRight) ();			// ��ȡƽ̨��Ȩ�ַ���
		STDMETHOD_( CString , GetPlatHomeURL) ();			// ��ȡ��ҳURL
		STDMETHOD_( CString , GetPlatDownloadURL) ();		// ��ȡ����URL
		STDMETHOD_( int  , SendMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// ������Ϣ
		STDMETHOD_( int  , ProcessMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// ƽ̨����Ϣ����
		STDMETHOD_( int  , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam);	// ����ƽ̨����
		STDMETHOD_( BOOL , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult);	// ����ƽ̨����

		// ��־����
		STDMETHOD_( void , LogEvent) (int nLevel, CString strInfo);	// ��¼��־
		STDMETHOD_( void , LogEventModule) (int nLevel, CString strModule, CString strInfo);	// ��¼��־(��ģ����Ϣ)

		// DuiVision����
		STDMETHOD_( BOOL , IsEnableDuiVision) ();	// �Ƿ�֧��DuiVision��ز���
		STDMETHOD_( CString , GetDuiResourcePath) () ;	// ��ȡDuiVision��resourceĿ¼
		STDMETHOD_( BOOL , DuiLoadResourceXml) (CString strResFile, CString strStyle);	// ����XML��Դ�ļ�(����ָ�����صķ��)
		STDMETHOD_( CString , DuiGetConfig) (CString strName) ;	// ��ȡϵͳ������Ϣ
		STDMETHOD_( CString , DuiGetXmlFile) (CString strName);	// ��ȡXML�ļ�
		STDMETHOD_( CString , DuiGetSkin) (CString strName);	// ��ȡSkin
		STDMETHOD_( CString , DuiGetString) (CString strName);	// ��ȡ�ַ���ֵ
		STDMETHOD_( void , DuiSetString) (CString strName, CString strValue);	// �����ַ���ֵ
		STDMETHOD_( void , ParseDuiString) (CString& strString);	// �����ַ������滻���е��滻����
		STDMETHOD_( void  , RegisterDuiControl) (CDuiObjectInfo* pDuiObjectInfo);	// ע��ؼ�
		STDMETHOD_( BOOL , UnRegisterDuiControl) (LPCTSTR lpszName);	// ж�ؿؼ�
		STDMETHOD_( CControlBase*  , DuiCreateControlByName) (LPCTSTR lpszName, HWND hWnd, CDuiObject* pParentObject);	// ���ݿؼ����������ؼ�ʵ��
		STDMETHOD_( CControlBase* , DuiGetControl) (CDuiObject* pDuiObject, CString strControlName);	// ��ȡ�ӿؼ�
		STDMETHOD_( CControlBase* , DuiGetDialogControl) (CString strDialogName, CString strControlName);
		STDMETHOD_( BOOL , RegisterDuiHandler) (CDuiObject* pDuiObject, CDuiHandler* pDuiHandler, CString strControlName, BOOL bInit);	// ��ָ����DUI����ע���¼��������
		STDMETHOD_( void , AddDuiHandler) (CDuiHandler* pDuiHandler);	// ���DUI�¼��������
		STDMETHOD_( void , RemoveDuiHandler) (CDuiHandler* pDuiHandler);	// ɾ��DUI�¼��������
		STDMETHOD_( CDlgBase* , CreateDuiDialog) (LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule, BOOL bAdd);	// ����DUI�Ի���
		STDMETHOD_( int , ShowDuiDialog) (LPCTSTR lpszXmlTemplate, CDuiObject* pParentObject, CString strName, BOOL bModule);	// ��ʾDUI�Ի���
		STDMETHOD_( int , DuiMessageBox) (CDuiObject* pParent, LPCTSTR lpszText, LPCTSTR lpszCaption, UINT uType, int nWidth, int nHeight);	// ��ʾDUI MessageBox
		STDMETHOD_( void , AddDuiDialog) (CDlgBase* pDuiDialog);	// ���DUI�Ի���
		STDMETHOD_( void , RemoveDuiDialog) (CDlgBase* pDuiDialog);	// ɾ��DUI�Ի���
		STDMETHOD_( CDlgBase* , GetDuiDialog) (CString strName);	// ��ȡDUI�Ի���
		STDMETHOD_( void , AddDuiNotifyMsgTask) (CString strMsg, UINT uNotifyType, CString strCaption, int nDelayTime, int nWidth, int nHeight);	// �����ʾ��������
		STDMETHOD_( UINT , AddDuiTimer) (UINT uTimerResolution, CString strTimerName);	//  ���DUI��ʱ��
		STDMETHOD_( void , RemoveDuiTimer) (CString strTimerName);	//  ɾ��DUI��ʱ��

	END_INTERFACE_PART(DuiVisionApp)
	EXPORT_INTERFACE_PART(DuiVisionApp)

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__DUIVISIONAPP_H_)
