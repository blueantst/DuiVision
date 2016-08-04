

#ifndef __DLG_BASE_H__
#define __DLG_BASE_H__
#include <vector>
#include "DuiVision.h"
#include "WndShadow.h"

using namespace  std;

class  CDlgBase : public CDialog, public CTimer, public CDuiObject
{
public:
	static LPCTSTR GetClassName() { return _T("dlg");}
	virtual BOOL IsClass(LPCTSTR lpszName)
	{
		if(_tcscmp(GetClassName(), lpszName)  == 0) return TRUE;
		return __super::IsClass(lpszName);
	}

	DECLARE_DYNAMIC(CDlgBase)

	// �Ի���ؼ�Ԥ���ýṹ
	struct CONTROL_VALUE
	{
		CString strName;	// �ؼ�������
		CString strType;	// �ؼ�����
		CString strValue;	// ���õ�ֵ
	};

public:
	UINT			m_nIDTemplate;				// �Ի�����ԴID
	BOOL			m_bInit;					// �Ƿ��ʼ�����
	CString			m_strXmlFile;				// ���ڵ�XML�ļ���
	CString			m_strXmlContent;			// ���ڵ�XML����

protected:
	CDuiObject*		m_pParentDuiObject;			// ���ؼ�����

	DWORD			m_nMainThreadId;			// ���߳�ID

	UINT			m_uTimerAnimation;			// ������ʱ��
	UINT			m_uTimerAutoClose;			// ���ڴ����Զ��رյĶ�ʱ��ID

	BOOL			m_bChangeSize;				// �ı䴰�ڴ�С
	CSize			m_MinSize;					// �����޶���С��С
	CRgn			m_Rgn;						// �����򴰿�����
	BOOL			m_bTopMost;					// ����������ǰ��

	CString			m_strTitle;					// ���ڱ���
	CFont			m_TitleFont;				// ���Ʊ�����������
	HICON			m_hIcon;					// ����ͼ����
	BOOL			m_bAppWin;					// �Ƿ�������(������������ʾ���˴���)

	CDlgPopup		*m_pWndPopup;				// ����ĵ�����ָ��

	COLORREF		m_clrBK;					// ������ɫ,�ɱ���ͼƬ�������ƽ��ɫ,������ͼƬ������ʱ���Դﵽ����ɫЧ��
	CBitmap			m_BKImage;					// ��ܱ���ͼƬ����
	CSize			m_sizeBKImage;				// ����ͼƬ��С
	CString			m_strFramePicture;			// ����ͼƬ
	CDC				m_MemBKDC;					// ����dc(�ɱ���ͼƬ�򱳾���ɫ����)
	CBitmap			*m_pOldMemBK;
	CBitmap			m_MemBK;
	BOOL			m_bDrawImage;				// ͼƬ��ɫ����
	int				m_nFrameSize;				// �߿��С
	int				m_nFrameWLT;				// �߿����Ͻǿ��(�Ź���ģʽ)
	int				m_nFrameHLT;				// �߿����ϽǸ߶�(�Ź���ģʽ)
	int				m_nFrameWRB;				// �߿����½ǿ��(�Ź���ģʽ)
	int				m_nFrameHRB;				// �߿����½Ǹ߶�(�Ź���ģʽ)

	CString			m_strBkImg;					// ����ͼƬ
	COLORREF		m_crlBack;					// ������ɫ
	COLORREF		m_crlBackTransParent;		// ����͸����ɫ
	int				m_nBackTranslucent;			// ����͸����

	int				m_nFrameTopBottomSpace;		// ����϶��ı䴰�ڴ�С��������
	int				m_nFrameLeftRightSpace;		// ����϶��ı䴰�ڴ�С������߶�

	int				m_nOverRegioX;				// ���ȵĴ�С
	int				m_nOverRegioY;				// ���ȵĴ�С
	BOOL			m_bNCActive;

	BOOL			m_bTracking;
	BOOL			m_bIsLButtonDown;	
	BOOL			m_bIsLButtonDblClk;
	BOOL			m_bIsRButtonDown;
	BOOL			m_bIsRButtonDblClk;
	BOOL			m_bIsSetCapture;

	BOOL			m_bAutoClose;				// �����Զ��رձ�־
	BOOL			m_bAutoHide;				// �����Զ����ر�־
	UINT			m_uAutoCloseDelayTime;		// �����Զ��رյ��ӳ�ʱ��

	BOOL			m_bImageUseECM;				// �Ƿ�ʹ��ͼƬ�������ɫ������Ϣ

	CToolTipCtrl	m_wndToolTip;				// Tooltip
	int				m_nTooltipCtrlID;			// ��ǰTooltip��ʾ�Ŀؼ�ID

	CDuiHandler*	m_pTrayHandler;				// ����ͼ��Ͳ˵����¼��������
	CString			m_strTrayMenuXml;			// ���̲˵���XML�����ļ�

	vector<CControlBase*>	m_vecControl;		// �û���ӵĴ��ڿؼ�
	vector<CControlBase*>	m_vecArea;			// �û���ӵ�����(��Ӱ������¼�)
	CControlBase*	m_pControl;				// ��ǰ��ؼ�����
	CControlBase*	m_pFocusControl;			// ��ǰ����Ŀؼ�����

	vector<CONTROL_VALUE>	m_vecControlValue;	// �ؼ�Ԥ������Ϣ

	// ������Ӱ
	CWndShadow		m_Shadow;					// ��Ӱ����
	int				m_nShadowWLT;				// ��ӰͼƬ���Ͻǿ��(�Ź���ģʽ)
	int				m_nShadowHLT;				// ��ӰͼƬ���ϽǸ߶�(�Ź���ģʽ)
	int				m_nShadowWRB;				// ��ӰͼƬ���½ǿ��(�Ź���ģʽ)
	int				m_nShadowHRB;				// ��ӰͼƬ���½Ǹ߶�(�Ź���ģʽ)
	int				m_nShadowSize;				// ��Ӱ���(�㷨��Ӱ)

private:
	vector<CControlBase*>	m_vecBaseControl;	// ���������õ���һЩĬ�Ͽؼ�
	vector<CControlBase*>	m_vecBaseArea;		// ����Ĭ������

public:
	CDlgBase(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CDlgBase();

	// ��ȡ����ģ��ID
	UINT GetIDTemplate() { return m_nIDTemplate; }

	void SetMinSize(int iWidth, int iHeight);	// ������С���ڴ�С
	CSize GetMinSize();

	void SetParent(CDuiObject* pParent) { m_pParentDuiObject = pParent; }
	virtual CDuiObject* GetParent() { return m_pParentDuiObject; }

	void TestMainThread();	// �����Ƿ������߳�

	// �Ƿ�ʹ���û��Զ���ECM
	BOOL UseImageECM() { return m_bImageUseECM; }

	// ���ô��ڵ�xml�ļ�
	void SetXmlFile(CString strXmlFile) {m_strXmlFile = strXmlFile;}
	// ���ô��ڵ�xml����
	void SetXmlContent(CString strXmlContent) {m_strXmlContent = strXmlContent;}

	// �������̲˵����¼��������
	void SetTrayHandler(CDuiHandler* pDuiHandler) { m_pTrayHandler = pDuiHandler; }
	// �������̲˵�xml�����ļ�
	void SetTrayMenuXml(CString strMenuXml) { m_strTrayMenuXml = strMenuXml; }

	// ��ȡ�ؼ�
	CControlBase* GetControl(UINT uControlID);
	CControlBase* GetControl(CString strControlName);
	CControlBase* GetBaseControl(UINT uControlID);
	CControlBase* GetBaseControl(CString strControlName);
	vector<CControlBase*>* GetControls() { return &m_vecControl; }

	// ����ؼ���غ���
	void SetFocusControl(CControlBase* pFocusControl);
	void SetFocusControlPtr(CControlBase* pFocusControl) { m_pFocusControl = pFocusControl; }	// ���ý���ؼ�ָ��
	CControlBase* GetFocusControl();
	CControlBase* GetPrevFocusableControl();
	CControlBase* GetNextFocusableControl();

	// ȷ�Ϻͷ���
	void DoOK() { PostMessage(WM_USER_CLOSEWND, IDOK, 0); }
	void DoCancel() { PostMessage(WM_USER_CLOSEWND, IDCANCEL, 0); }
	void DoClose() { PostMessage(WM_USER_CLOSEWND, IDCANCEL, 0); }
	void DoYes() { PostMessage(WM_USER_CLOSEWND, IDYES, 0); }
	void DoNo() { PostMessage(WM_USER_CLOSEWND, IDNO, 0); }

	// �ƶ��ؼ�
	virtual CControlBase* SetControlRect(UINT uControlID, CRect rc);
	virtual CControlBase* SetControlRect(CControlBase *pControlBase, CRect rc);
	// ��ʾ�ؼ�
	virtual CControlBase* SetControlVisible(UINT uControlID, BOOL bVisible);
	virtual CControlBase* SetControlVisible(CControlBase *pControlBase, BOOL bVisible);
	// ���ÿؼ�
	virtual CControlBase* SetControlDisable(UINT uControlID, BOOL bDisable);
	virtual CControlBase* SetControlDisable(CControlBase *pControlBase, BOOL bDisable);

	// ����resize����
	HRESULT OnAttributeResize(const CString& strValue, BOOL bLoading);

	// �򿪵����Ի���
	void OpenDlgPopup(CDlgPopup *pWndPopup, CRect rc, UINT uMessageID, BOOL bShow=TRUE);
	// �رյ����Ի���
	void CloseDlgPopup();

	// ���ݿؼ��������ؼ�ʵ��
	CControlBase* _CreateControlByName(LPCTSTR lpszName);

	// ���ò�����������
	void SetupRegion(int border_offset[], int nSize);
	// �����ڱ����Ϳؼ�
	void DrawBackgroundAndControls(CDC &dc, const CRect &rcClient, const CRect &rcUpdate);
	
	// ��ʼ�����ڱ���Ƥ��(���ص������ڴ�dc)
	void InitWindowBkSkin();
	// ���ش��ڱ���ͼƬ
	void LoadBackgroundImage(UINT nIDResource, CString strType = TEXT("PNG"));
	void LoadBackgroundImage(CString strFileName);
	// ��ʼ�����ڿؼ���Ĭ��ֵ
	void InitUIState();
	// ���ô��ڱ���͸����
	void SetupBackTranslucent();
	// ���ò�����������
	void SetupRegion(int nSize);
	// ������ͼƬ
	void DrawBackground(CBitmap &bitBackground);
	// ��������ɫ
	void DrawBackground(COLORREF clr);
	// ���ؼ�
	virtual void DrawControl(CDC &dc, const CRect &rcClient);	
	// ���ÿؼ�
	virtual void ResetControl();
	// ���������������
	void UpdateHover();

	// ����Tooltip
	void SetTooltip(CControlBase* pControl, CString strTooltip, CRect rect, BOOL bControlWidth = FALSE, int nTipWidth = 0);
	// ���Tooltip
	void ClearTooltip();
	// ���õ�ǰtooltip�ؼ�ID
	void SetTooltipCtrlID(int nTooltipCtrlID) { m_nTooltipCtrlID = nTooltipCtrlID; }
	// ��ȡ��ǰtooltip�ؼ�ID
	int  GetTooltipCtrlID() { return m_nTooltipCtrlID; }

	void InitBaseUI(CRect rcClient, DuiXmlNode pNode);
	virtual void InitUI(CRect rcClient, DuiXmlNode pNode);
	virtual void OnSize(CRect rcClient);

	// ���ÿؼ���Ԥ��ֵ
	void SetControlValue(CString strName, CString strType, CString strValue);
	void InitDialogValue();
	void InitControlValue();

	// ���ô��ڵ��Զ��رն�ʱ��
	void SetAutoCloseTimer(UINT uDelayTime, BOOL bHideWnd = FALSE);

	// ��ʱ����Ϣ
	virtual void OnTimer(UINT uTimerID);
	virtual void OnTimer(UINT uTimerID, CString strTimerName);

	// ��Ϣ��Ӧ
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual void OnClose();
	virtual void OnMinimize();
	virtual BOOL OnMaximize();
	virtual void OnSkin();

	virtual BOOL OnInitDialog();
	virtual void PostNcDestroy();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd (CDC* pDC);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
	afx_msg void OnNcPaint();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS* lpncsp);
	afx_msg BOOL OnNcActivate(BOOL bActive);
	afx_msg void OnWindowPosChanging(WINDOWPOS* lpwndpos);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);

	afx_msg LRESULT OnUserCloseWindow(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageSkin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMessageUITask(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnSystemTrayIcon(WPARAM wParam, LPARAM lParam);

	// �����֪ͨ��Ϣ
	LRESULT OnCheckItsMe(WPARAM wParam, LPARAM lParam);

	LRESULT OnMessageButtonMin(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtonMax(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtonClose(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT OnMessageButtonSkin(UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
	virtual void PreSubclassWindow();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseLeave(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMouseHover(WPARAM wParam, LPARAM lParam);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();

	DUI_IMAGE_ATTRIBUTE_DEFINE(Shadow);			// ������ӰͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("appwin"), m_bAppWin, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("title"), m_strTitle, FALSE)
		DUI_INT_ATTRIBUTE(_T("width"), m_MinSize.cx, FALSE)
		DUI_INT_ATTRIBUTE(_T("height"), m_MinSize.cy, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("resize"), OnAttributeResize)
		DUI_TSTRING_ATTRIBUTE(_T("frame"), m_strFramePicture, FALSE)
		DUI_INT_ATTRIBUTE(_T("framesize"), m_nFrameSize, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-lt"), m_nFrameWLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-lt"), m_nFrameHLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-rb"), m_nFrameWRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-rb"), m_nFrameHRB, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("bkimg"), m_strBkImg, FALSE)
		DUI_RGBCOLOR_ATTRIBUTE(_T("crbk"), m_crlBack, FALSE)
		DUI_RGBCOLOR_ATTRIBUTE(_T("crtransparent"), m_crlBackTransParent, FALSE)
		DUI_INT_ATTRIBUTE(_T("img-ecm"), m_bImageUseECM, TRUE)
		DUI_INT_ATTRIBUTE(_T("translucent"), m_nBackTranslucent, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("img-shadow"), OnAttributeImageShadow)
		DUI_INT_ATTRIBUTE(_T("shadow-wlt"), m_nShadowWLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("shadow-hlt"), m_nShadowHLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("shadow-wrb"), m_nShadowWRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("shadow-hrb"), m_nShadowHRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("shadow-size"), m_nShadowSize, FALSE)
		DUI_INT_ATTRIBUTE(_T("topmost"), m_bTopMost, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};

#endif __DLG_BASE_H__

