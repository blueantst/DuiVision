// Tabҳ�ؼ�
#pragma once
#include "ControlBase.h"

// ������ģʽ
enum TAB_TYPE
{
	TAB_TYPE_HORIZONTAL = 0,			// ˮƽ����
	TAB_TYPE_VERTICAL,				// ��ֱ����
};

// TabҳǩͼƬ��ʾģʽ
enum enumTabImageMode
{
	enTIMNormal = 0,			// ����
	enTIMExtrude,				// ����
	enTIMMID					// �Ź���߿�
};

// Tabҳ��Ϣ�ṹ����
struct TabItemInfo
{
	BOOL			bVisible;	// tabҳǩ�Ƿ�ɼ�
	Image*			pImage;		// tabҳǩͼƬ
	CSize			sizeImage;	// tabҳǩͼƬ��С
	int				nImageCount;// tabҳǩͼƬ����(������1��3,�����1,��hoverͼƬʹ��tabctrl���õ�)
	int				nImageIndex;// tabҳǩͼƬ����,���pImageΪ��,��ʹ�ô�������tabctrl�в���ͼƬ
	CRect			rc;			// tabҳǩ���ֵ�λ��
	CString			strText;	// tabҳǩ����
	CString			strAction;	// ��������
	UINT			nItemID;	// ID
	CString			strName;	// ����
	BOOL			bOutLink;	// �Ƿ��ⲿ����ҳ��
	CRect			rcButton;	// tabҳǩ�İ�ť����
	enumButtonState	buttonState;// Tabҳǩ��ť��״̬
	BOOL			bNeedTextTip;// �Ƿ���Ҫ��ʾ���ֵ�tip(����ʵ�ʿ�ȴ�����ʾ���)
	CControlBase*	pControl;	// Tabҳ��Ӧ����ҳ�������ؼ�
};


class CDuiTabCtrl : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiTabCtrl, _T("tabctrl"))
public:
	CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject);
	CDuiTabCtrl(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle= TEXT(""), BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual~CDuiTabCtrl(void);

	BOOL InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, UINT nResourceID, CControlBase* pControl, int nImageCount = -1, BOOL bOutLink = FALSE, int nItemWidth = 0, CString strType= TEXT("PNG"));
	BOOL InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, CString strImage, CControlBase* pControl, int nImageCount = -1, BOOL bOutLink = FALSE, int nItemWidth = 0);
	BOOL InsertItem(int nItem, UINT nItemID, CString strName, CString strItemText, CString strAction, int nImageIndex, CControlBase* pControl, BOOL bOutLink = FALSE, int nItemWidth = 0);

	int  GetItemCount();
	int  GetItemIndex(CString strTabName);
	TabItemInfo* GetItemInfo(int nItem);
	void SetItemWidth(int nTabItemMaxWidth, int nTabItemMinWidth, BOOL bRefresh = TRUE);
	void SetItemHeight(int nTabItemMaxHeight, int nTabItemMinHeight, BOOL bRefresh = TRUE);
	void RefreshItems();
	void DeleteItem(int nItem);
	void DeleteItem(CString strTabName);
	int  SetSelectItem(int nItem);
	int  GetSelectItem();
	void SetItemVisible(int nItem, BOOL bVisible);
	void SetItemVisible(CString strTabName, BOOL bVisible);
	BOOL GetItemVisible(int nItem);
	BOOL GetItemVisible(CString strTabName);

	virtual void SetControlRect(CRect rc);
	virtual void SetControlVisible(BOOL bIsVisible);
	virtual void SetControlHide(BOOL bIsHide);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadTabXml(CString strFileName);

	BOOL PtInTabButton(CPoint point, TabItemInfo& itemInfo);

	HRESULT OnAttributeTabBtnPosChange(const CString& strValue, BOOL bLoading);

	void SetTabTooltip(int nItem, CString strTooltip);
	void ClearTabTooltip();
	
protected:
	// ���ݿؼ��������ؼ�ʵ��
	CControlBase* _CreateControlByName(LPCTSTR lpszName);

	virtual BOOL OnCheckMouseResponse(UINT nFlags, CPoint point);
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	virtual	BOOL OnControlTimer();
	virtual BOOL DrawSubControls(CDC &dc, CRect rcUpdate);

	void DrawControlHorizontal(CDC &dc, CRect rcUpdate);
	void DrawControlVertical(CDC &dc, CRect rcUpdate);

	BOOL InsertItem(int nItem, TabItemInfo &itemInfo);
	
public:
	TAB_TYPE			m_nTabType;			// Tabҳ����
	vector<TabItemInfo>		m_vecItemInfo;			// Tabҳ��Ϣ�б�
	vector<CRect>			m_vecRcSeperator;		// tabλ���б�

	Color					m_clrText;				// ������ɫ
	Color					m_clrTextHover;			// ������ɫ(����ƶ�)
	Color					m_clrTextDown;			// ������ɫ(��갴��)

	// ˮƽ����tab�ؼ�����
	int						m_nTabItemWidth;		// ÿ��Tabҳ�ĵ�ǰ���
	int						m_nTabItemMaxWidth;		// ÿ��Tabҳ�������(Ĭ�Ͽ��)
	int						m_nTabItemMinWidth;		// ÿ��Tabҳ����С���(���������ֵ,��ʵ�ʿ�Ȼ��Զ�����)
	int						m_nTabCtrlHeight;		// TabCtrl���ֵĸ߶�

	// ��ֱ����tab�ؼ�����
	int						m_nTabItemHeight;		// ÿ��Tabҳ�ĵ�ǰ�߶�
	int						m_nTabItemMaxHeight;		// ÿ��Tabҳ�����߶�(Ĭ�ϸ߶�)
	int						m_nTabItemMinHeight;		// ÿ��Tabҳ����С�߶�(���������ֵ,��ʵ�ʸ߶Ȼ��Զ�����)
	int						m_nTabCtrlWidth;		// TabCtrl���ֵĿ��

	BOOL					m_bInit;				// �Ƿ��ʼ�����

	int						m_nHoverItem;			// ��ǰ��ʾ��ҳ������
	int						m_nDownItem;			// ��ǰ�����ҳ������
	BOOL					m_bAnimateChangeTab;	// �Ƿ�֧���л�tabҳ�Ķ���
	int						m_nOldItem;				// �л�ǰ��ҳ������
	int						m_nAnimateCount;		// �л�������֡��
	int						m_nCurXPos;				// �л������е�ǰ�ĺ�����λ��
	int						m_nCurYPos;				// �л������е�ǰ��������λ��
	int						m_nTabLeftPading;		// Tabҳǩ���Ŀհ׿��
	int						m_nTabRightPading;		// Tabҳǩ�Ҳ�Ŀհ׿��
	int						m_nTabTopPading;		// Tabҳǩ�ϱߵĿհ׿��
	int						m_nTabBottomPading;		// Tabҳǩ�±ߵĿհ׿��
	DUI_POSITION			m_posTabBtn;			// Tabҳǩ���ڲ���ťλ����Ϣ

	enumTabImageMode		m_enTabImageMode;		// TabҳǩͼƬ����ʾģʽ(��ͨ�����졢�Ź���)
	int						m_nWLT;					// ���Ͻǿ��(TabҳǩͼƬ�ľŹ���ģʽ)
	int						m_nHLT;					// ���ϽǸ߶�(TabҳǩͼƬ�ľŹ���ģʽ)
	int						m_nWRB;					// ���½ǿ��(TabҳǩͼƬ�ľŹ���ģʽ)
	int						m_nHRB;					// ���½Ǹ߶�(TabҳǩͼƬ�ľŹ���ģʽ)

	BOOL					m_bTabTooltip;			// �Ƿ���ʾTabҳǩ��Tooltip
	int						m_nTipItem;				// ��ǰtipҳ������

	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);			// ����ָ�ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(Hover);				// �����ȵ�ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(TabBtn);				// ����tabҳǩ��ťͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_ENUM_ATTRIBUTE(_T("tab-type"), TAB_TYPE, TRUE)
            DUI_ENUM_VALUE(_T("horizontal"), TAB_TYPE_HORIZONTAL)
            DUI_ENUM_VALUE(_T("vertical"), TAB_TYPE_VERTICAL)
        DUI_ENUM_END(m_nTabType)
		DUI_CUSTOM_ATTRIBUTE(_T("img-sep"), OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-hover"), OnAttributeImageHover)
		DUI_CUSTOM_ATTRIBUTE(_T("img-tabbtn"), OnAttributeImageTabBtn)
		DUI_CUSTOM_ATTRIBUTE(_T("tabbtnpos"), OnAttributeTabBtnPosChange)
		DUI_INT_ATTRIBUTE(_T("item-width"), m_nTabItemMaxWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("item-width-min"), m_nTabItemMinWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("item-height"), m_nTabItemMaxHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("item-height-min"), m_nTabItemMinHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-height"), m_nTabCtrlHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-width"), m_nTabCtrlWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-left-pading"), m_nTabLeftPading, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-right-pading"), m_nTabRightPading, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-top-pading"), m_nTabTopPading, FALSE)
		DUI_INT_ATTRIBUTE(_T("tab-bottom-pading"), m_nTabBottomPading, FALSE)
		DUI_INT_ATTRIBUTE(_T("animate"), m_bAnimateChangeTab, FALSE)
		DUI_INT_ATTRIBUTE(_T("animate-count"), m_nAnimateCount, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crhover"), m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crpush"), m_clrTextDown, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-lt"), m_nWLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-lt"), m_nHLT, FALSE)
		DUI_INT_ATTRIBUTE(_T("width-rb"), m_nWRB, FALSE)
		DUI_INT_ATTRIBUTE(_T("height-rb"), m_nHRB, FALSE)
		DUI_ENUM_ATTRIBUTE(_T("tabimg-mode"), enumTabImageMode, TRUE)
            DUI_ENUM_VALUE(_T("normal"), enTIMNormal)
            DUI_ENUM_VALUE(_T("extrude"), enTIMExtrude)
			DUI_ENUM_VALUE(_T("mid"), enTIMMID)
        DUI_ENUM_END(m_enTabImageMode)
		DUI_INT_ATTRIBUTE(_T("tab-tip"), m_bTabTooltip, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};