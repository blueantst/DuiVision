#pragma once

#include <vector>
#include "DuiVision.h"

using namespace  std;

// ��Ϣ����
#define	MSG_CLOSE	1
#define	MSG_USER	2

// ����ģʽ
enum enumBackMode
{
	enBMImage = 0,			// ����
	enBMFrame,				// �߿�
	enBMMID					// �Ź���ģʽ�ı߿�
};

// CDlgPopup

class CDlgPopup : public CWnd, public CTimer, public CDuiObject
{
public:
	static LPCTSTR GetClassName() { return _T("popup");}
	virtual BOOL IsClass(LPCTSTR lpszName)
	{
		if(_tcscmp(GetClassName(), lpszName)  == 0) return TRUE;
		return __super::IsClass(lpszName);
	}

public:
	BOOL			m_bInit;
	BOOL			m_bTracking;
	BOOL			m_bIsLButtonDown;	
	BOOL			m_bIsLButtonDblClk;
	BOOL			m_bIsRButtonDown;	
	BOOL			m_bIsRButtonDblClk;
	BOOL			m_bIsSetCapture;

protected:
	CDuiObject*		m_pParentDuiObject;			// ���ؼ�����

	Image*			m_pImage;
	CSize			m_sizeBKImage;
	CPoint			m_point;
	CSize			m_size;
	UINT			m_uMessageID;

	enumBackMode	m_enBackMode;				// ����ģʽ
	BOOL			m_bBackAlpha;				// �����Ƿ�ʹ��Alphaͨ��
	int				m_nBackTranslucent;			// ����͸����
	int				m_nFrameSize;				// �߿��С
	int				m_nFrameWLT;				// �߿����Ͻǿ��(�Ź���ģʽ)
	int				m_nFrameHLT;				// �߿����ϽǸ߶�(�Ź���ģʽ)
	int				m_nFrameWRB;				// �߿����½ǿ��(�Ź���ģʽ)
	int				m_nFrameHRB;				// �߿����½Ǹ߶�(�Ź���ģʽ)
	BOOL			m_bInitFinish;				// �Ƿ��ʼ�����
	BOOL			m_bAutoClose;				// �Ƿ��Զ��ر�
	BOOL			m_bTopMost;					// ����������ǰ��

	BOOL			m_bImageUseECM;				// �Ƿ�ʹ��ͼƬ�������ɫ������Ϣ

	BOOL			m_bNCActive;
	vector<CControlBase*>	m_vecControl;		// ���ڿؼ�
	vector<CControlBase*>	m_vecArea;			// ��(��Ӱ������¼�)
	CControlBase*	m_pControl;					// ��ǰ�ؼ�
	CControlBase*	m_pFocusControl;			// ��ǰ����ؼ�

	UINT			m_uTimerAnimation;			// ������ʱ��

	CString			m_strXmlFile;				// XML�ļ���
	CString			m_strXmlContent;			// XML����

	DECLARE_DYNAMIC(CDlgPopup)

public:
	CDlgPopup();
	virtual ~CDlgPopup();

	void SetParent(CDuiObject* pParent) { m_pParentDuiObject = pParent; }
	virtual CDuiObject* GetParent() { return m_pParentDuiObject; }

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadXmlFile(CString strFileName);
	BOOL LoadXmlContent(CString strXmlContent);
	CString GetXmlFile() { return m_strXmlFile; }

	void SetAutoClose(BOOL bAutoClose) { m_bAutoClose = bAutoClose; }
	BOOL IsAutoClose() { return m_bAutoClose; }

	BOOL UseImageECM() { return m_bImageUseECM; }

	virtual void InitUI(CRect rcClient, DuiXmlNode pNode);

	void SetBackMode(enumBackMode enBackMode);
	void SetFrameSize(int nFrameSize);
	void SetBackBitmap(UINT nResourceID);
	void SetBackBitmap(CString strImage);
	BOOL Create(CWnd *pParent, CRect rc, UINT uMessageID, UINT nResourceID, enumBackMode enBackMode = enBMFrame, int nFrameSize = 4);
	BOOL Create(CWnd *pParent, CRect rc, UINT uMessageID, CString strImage, enumBackMode enBackMode = enBMFrame, int nFrameSize = 4);
	BOOL Create(CWnd *pParent, CRect rc, UINT uMessageID);
	void CloseWindow();

	// ���ݿؼ��������ؼ�ʵ��
	CControlBase* _CreateControlByName(LPCTSTR lpszName);

	void DrawWindow();	
	void DrawWindow(CDC &dc);
	void DrawWindow(CDC *pDC);
	virtual void DrawWindow(CDC &dc, CRect rcClient){};
	virtual void DrawWindowEx(CDC &dc, CRect rcClient){};
	virtual void InitUI(CRect rcClient) {};
	virtual BOOL OnMouseMove(CPoint point){ return false;};
	virtual BOOL OnLButtonDown(CPoint point){ return false;};
	virtual BOOL OnLButtonUp(CPoint point){ return false;};
	virtual BOOL OnLButtonDblClk(CPoint point){ return false;};
	virtual BOOL OnRButtonDown(CPoint point){ return false;};
	virtual BOOL OnRButtonUp(CPoint point){ return false;};
	virtual BOOL OnRButtonDblClk(CPoint point){ return false;};
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags){ return false;};

	HRESULT OnAttributeBkMode(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeBkImage(const CString& strValue, BOOL bLoading);

	// ��ʱ����Ϣ
	virtual void OnTimer(UINT uTimerID);

	// ��Ϣ��Ӧ
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnNcCloseWindow();

	// ����֪ͨ��Ϣ
	BOOL SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam);
	BOOL PostMessage(UINT Msg, WPARAM wParam, LPARAM lParam);
	BOOL SendMessageToParent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	CControlBase *GetControl(UINT uControlID);
	CControlBase *GetControl(CString strControlName);

	vector<CControlBase*>* GetControls() { return &m_vecControl; }

	void SetFocusControl(CControlBase* pFocusControl);
	void SetFocusControlPtr(CControlBase* pFocusControl) { m_pFocusControl = pFocusControl; }	// ���ý���ؼ�ָ��
	CControlBase* GetFocusControl();
	CControlBase* GetPrevFocusableControl();
	CControlBase* GetNextFocusableControl();

	// �ƶ��ؼ�
	virtual CControlBase * SetControlRect(UINT uControlID, CRect rc);
	// �ƶ��ؼ�
	virtual CControlBase * SetControlRect(CControlBase *pControlBase, CRect rc);
	// ��ʾ�ؼ�
	virtual CControlBase * SetControlVisible(UINT uControlID, BOOL bVisible);
	// ��ʾ�ؼ�
	virtual CControlBase * SetControlVisible(CControlBase *pControlBase, BOOL bVisible);
	// ���ÿؼ�
	virtual CControlBase * SetControlDisable(UINT uControlID, BOOL bDisable);
	// ���ÿؼ�
	virtual CControlBase * SetControlDisable(CControlBase *pControlBase, BOOL bDisable);
	// ���ÿؼ�
	virtual void ResetControl();
	// ����ѡ��
	void UpdateHover();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg BOOL OnEraseBkgnd (CDC* pDC);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnClose();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("bkmode"), OnAttributeBkMode)
		DUI_CUSTOM_ATTRIBUTE(_T("bkimg"), OnAttributeBkImage)
		DUI_INT_ATTRIBUTE(_T("bkalpha"), m_bBackAlpha, TRUE)
		DUI_INT_ATTRIBUTE(_T("translucent"), m_nBackTranslucent, TRUE)
		DUI_INT_ATTRIBUTE(_T("width"), m_size.cx, FALSE)
		DUI_INT_ATTRIBUTE(_T("height"), m_size.cy, FALSE)
		DUI_INT_ATTRIBUTE(_T("framesize"), m_nFrameSize, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-lt"), m_nFrameWLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-lt"), m_nFrameHLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-rb"), m_nFrameWRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-rb"), m_nFrameHRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("img-ecm"), m_bImageUseECM, TRUE)
		DUI_INT_ATTRIBUTE(_T("autoclose"), m_bAutoClose, FALSE)
		DUI_INT_ATTRIBUTE(_T("topmost"), m_bTopMost, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};


