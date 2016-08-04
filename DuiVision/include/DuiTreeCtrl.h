// ���ؼ�
#pragma once

#include "Panel.h"

using namespace  std;

// ����Ϣ
struct TreeColumnInfo
{
	CRect	rcHeader;		// ��ͷλ����Ϣ
	CString strTitle;		// ����
	Color	clrText;		// ������ɫ
	int		nWidth;			// �п��
};

// ��Ԫ����Ϣ
struct TreeItemInfo
{
	CRect	rcItem;			// λ����Ϣ
	CString strTitle;		// ����
	CString strContent;		// ����
	int		nImageIndex;	// ͼƬ����
	Image * pImage;			// ͼƬ����
	CSize	sizeImage;		// ͼƬ��С
	int		nImageCount;	// ͼƬ����
	Color	clrText;		// ������ɫ
	CString strLink;		// ���ӵ�����
	CString strLinkAction;	// ���ӵĶ���
	BOOL	bNeedTitleTip;	// �Ƿ���Ҫ��ʾtitle tip(titleʵ�ʿ�ȴ�����ʾ���)
	BOOL	bNeedContentTip;// �Ƿ���Ҫ��ʾcontent tip(contentʵ�ʿ�ȴ�����ʾ���)
	BOOL	bUseTitleFont;	// �Ƿ�ʹ�ñ���������ʾ����
	BOOL	bShowCollapse;	// �Ƿ��ڴ˵�Ԫ����ʾ�ڵ�����ͼƬ
	vector<CControlBase *>	vecControl;// �ؼ��б�
};

#define HTREEITEM	int

// ���ڵ���Ϣ
struct TreeNodeInfo
{
	HTREEITEM hParentNode;	// ���ڵ���
	HTREEITEM hNode;		// �ڵ���
	CRect	rcRow;			// ��λ����Ϣ
	CString	strId;			// ID
	int		nCheck;			// ����״̬(-1��ʾ����ʾ)
	CRect	rcCheck;		// ����λ����Ϣ
	int		nImageIndex;	// ͼƬ����
	Image * pImage;			// ͼƬ����
	CSize	sizeImage;		// ͼƬ��С
	int		nRightImageIndex;// �ұ�ͼƬ����
	Image * pRightImage;	// �ұ�ͼƬ����
	CSize	sizeRightImage;	// �ұ�ͼƬ��С
	BOOL	bRowColor;		// ʹ���ж����������ɫ
	Color	clrText;		// ��������ɫ
	BOOL	bRowBackColor;		// ʹ���ж���ı�����ɫ
	Color	clrBack;		// �б�����ɫ
	int		nHoverItem;		// ��ǰ�ȵ���
	BOOL	bCollapse;		// �Ƿ��۵�
	BOOL	bHide;			// �Ƿ�����
	vector<TreeItemInfo> vecItemInfo;
};

class CDuiTreeCtrl : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiTreeCtrl, _T("treectrl"))

public:
	CDuiTreeCtrl(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiTreeCtrl(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadNode(HTREEITEM hParentNode, DuiXmlNode pXmlElem);

	BOOL InsertColumn(int nColumn, CString strTitle, int nWidth = -1, Color clrText = Color(0, 0, 0, 0));
	HTREEITEM InsertNode(HTREEITEM hParentNode, CString strId, CString strTitle, BOOL bCollapse = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""),
		int nRightImageIndex = -1, CString strRightImage = _T(""),
		int nCheck = -1, Color clrBack = Color(0, 0, 0, 0));
	HTREEITEM InsertNode(HTREEITEM hParentNode, TreeNodeInfo &nodeInfo);
	BOOL SetSubItem(HTREEITEM hNode, int nItem, CString strTitle, CString strContent = _T(""), BOOL bUseTitleFont = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemLink(HTREEITEM hNode, int nItem, CString strLink, CString strLinkAction = _T(""),
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemCollapse(HTREEITEM hNode, int nItem, CString strImage = _T(""), int nImageCount = 0);
	BOOL AddSubItemControl(HTREEITEM hNode, int nItem, CControlBase* pControl);
	BOOL DeleteSubItemControl(CControlBase* pControl);
	BOOL DeleteSubItemControl(CString strControlName, UINT uControlID = ID_NULL);
	BOOL DeleteNode(HTREEITEM hNode);
	int  GetNodeCount() { return m_vecRowInfo.size(); }
	int  GetNodeRow(HTREEITEM hNode);
	int	 GetNodeLastChildRow(HTREEITEM hNode);
	BOOL HaveChildNode(HTREEITEM hNode);
	HTREEITEM GetParentNode(HTREEITEM hNode);
	HTREEITEM GetChildNode(HTREEITEM hNode);
	HTREEITEM GetNextSiblingNode(HTREEITEM hNode);
	HTREEITEM GetPrevSiblingNode(HTREEITEM hNode);
	int  GetChildNodeCount(HTREEITEM hNode);
	int  GetNodeLevel(HTREEITEM hNode);
	HTREEITEM GetNodeById(CString strId);
	TreeNodeInfo* GetNodeInfo(HTREEITEM hNode);
	TreeItemInfo* GetItemInfo(HTREEITEM hNode, int nItem);
	void SetItemInfo(HTREEITEM hNode, int nItem, TreeItemInfo* pItemInfo);
	void SetNodeColor(HTREEITEM hNode, Color clrText);
	void SetNodeBackColor(HTREEITEM hNode, Color clrBack);
	void ToggleNode(HTREEITEM hNode);
	void ExpandNode(HTREEITEM hNode, BOOL bExpand);
	void SetNodeCheck(HTREEITEM hNode, int nCheck);
	int  GetNodeCheck(HTREEITEM hNode);
	void ClearNodes();
	void HideChildNodes(HTREEITEM hItem);
	void RefreshNodeRows();
	BOOL EnsureVisible(HTREEITEM hNode, BOOL bPartialOK);

	BOOL PtInRow(CPoint point, TreeNodeInfo& rowInfo);
	BOOL PtInRowCheck(CPoint point, TreeNodeInfo& rowInfo);
	BOOL PtInRowCollapse(CPoint point, TreeNodeInfo& rowInfo);
	int  PtInRowItem(CPoint point, TreeNodeInfo& rowInfo);

	void SetGridTooltip(HTREEITEM hNode, int nItem, CString strTooltip);
	void ClearGridTooltip();

protected:
	vector<TreeColumnInfo> m_vecColumnInfo;
	vector<TreeNodeInfo> m_vecRowInfo;

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);

	HRESULT OnAttributeFontTitle(const CString& strValue, BOOL bLoading);

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	CString				m_strFontTitle;		// ��������
	int					m_nFontTitleWidth;	// ����������
	FontStyle			m_fontTitleStyle;	// ��������Style
	Color				m_clrText;			// ������ɫ
	Color				m_clrTextHover;		// ������ɫ(����ƶ�)
	Color				m_clrTextDown;		// ������ɫ(��갴��)
	Color				m_clrTitle;			// ������ɫ
	Color				m_clrSeperator;		// �ָ�����ɫ
	Color				m_clrRowHover;		// �б�����ɫ(����ƶ�����)
	int					m_nLeftPos;			// �����ʼλ��
	int					m_nRowHeight;		// �и߶�
	int					m_nBkTransparent;	// ����͸����
	BOOL				m_bSingleLine;		// ��ʾ��������
	BOOL				m_bTextWrap;		// �����Ƿ���

	int					m_nHoverRow;		// ��ǰ����ƶ���������
	int					m_nDownRow;			// ��ǰ�����������
	BOOL				m_bEnableDownRow;	// ������ʾ��ǰ�����

	int					m_nFirstViewRow;	// ��ǰ��ʾ���ĵ�һ�е����
	int					m_nVirtualTop;		// ��ǰ������λ�ö�Ӧ�������topλ��
	int					m_nVisibleRowCount;	// ��ǰ����ʾ������(���۵���)

	BOOL				m_bGridTooltip;		// �Ƿ���ʾ��Ԫ���Tooltip
	int					m_nTipNode;			// ��ǰtip�ڵ�
	int					m_nTipItem;			// ��ǰtip��
	int					m_nTipVirtualTop;	// ��ǰtip�е�����Top

	HTREEITEM			m_nNodeIndex;		// �����������

	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);	// �����зָ���ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(CheckBox);	// �������ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(Collapse);	// ����������ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(Toggle);		// �������ڵ�����ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("img-sep"), OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-check"), OnAttributeImageCheckBox)
		DUI_CUSTOM_ATTRIBUTE(_T("img-collapse"), OnAttributeImageCollapse)
		DUI_CUSTOM_ATTRIBUTE(_T("img-toggle"), OnAttributeImageToggle)
		DUI_CUSTOM_ATTRIBUTE(_T("font-title"), OnAttributeFontTitle)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crhover"), m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crpush"), m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtitle"), m_clrTitle, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crsep"), m_clrSeperator, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crrowhover"), m_clrRowHover, FALSE)
		DUI_INT_ATTRIBUTE(_T("row-height"), m_nRowHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("left-pos"), m_nLeftPos, FALSE)
		DUI_INT_ATTRIBUTE(_T("wrap"), m_bTextWrap, FALSE)
		DUI_INT_ATTRIBUTE(_T("down-row"), m_bEnableDownRow, FALSE)
		DUI_INT_ATTRIBUTE(_T("bk-transparent"), m_nBkTransparent, FALSE)
		DUI_INT_ATTRIBUTE(_T("grid-tip"), m_bGridTooltip, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
