// ���ؼ�
#pragma once

#include "Panel.h"

using namespace  std;

// ����Ϣ
struct GridColumnInfo
{
	CRect	rcHeader;		// ��ͷλ����Ϣ
	CString strTitle;		// ����
	Color	clrText;		// ������ɫ
	int		nWidth;			// �п��
	UINT	uAlignment;		// ˮƽ���뷽ʽ
	UINT	uVAlignment;	// ��ֱ���뷽ʽ
};

// ��Ԫ����Ϣ
struct GridItemInfo
{
	CRect	rcItem;			// λ����Ϣ
	CString strTitle;		// ����
	CString strContent;		// ����
	int		nImageIndex;	// ͼƬ����
	Image * pImage;			// ͼƬ����
	CSize	sizeImage;		// ͼƬ��С
	Color	clrText;		// ������ɫ
	CString strLink;		// ���ӵ�����
	CString strLinkAction;	// ���ӵĶ���
	BOOL	bNeedTitleTip;	// �Ƿ���Ҫ��ʾtitle tip(titleʵ�ʿ�ȴ�����ʾ���)
	BOOL	bNeedContentTip;// �Ƿ���Ҫ��ʾcontent tip(contentʵ�ʿ�ȴ�����ʾ���)
	BOOL	bUseTitleFont;	// �Ƿ�ʹ�ñ���������ʾ����
	vector<CControlBase *>	vecControl;// �ؼ��б�
};

// ����Ϣ
struct GridRowInfo
{
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
	vector<GridItemInfo> vecItemInfo;
};

class CDuiGridCtrl : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiGridCtrl, _T("gridctrl"))

public:
	CDuiGridCtrl(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiGridCtrl(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

	BOOL InsertColumn(int nColumn, CString strTitle, int nWidth = -1, Color clrText = Color(0, 0, 0, 0),
		UINT uAlignment = 0xFFFFUL, UINT uVAlignment = 0xFFFFUL);
	int SetColumnWidth(int nColumn, int nWidth, int nWidthNextColumn = -1);
	void MoveColumnSplit(int nColumn, int nPos);
	int GetTotalColumnWidth();
	int InsertRow(int nRow, CString strId,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""),
		int nRightImageIndex = -1, CString strRightImage = _T(""),
		int nCheck = -1, Color clrBack = Color(0, 0, 0, 0));
	int InsertRow(int nRow, GridRowInfo &rowInfo);
	BOOL SetSubItem(int nRow, int nItem, CString strTitle, CString strContent = _T(""), BOOL bUseTitleFont = FALSE,
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL SetSubItemLink(int nRow, int nItem, CString strLink, CString strLinkAction = _T(""),
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""));
	BOOL AddSubItemControl(int nRow, int nItem, CControlBase* pControl);
	BOOL DeleteSubItemControl(CControlBase* pControl);
	BOOL DeleteSubItemControl(CString strControlName, UINT uControlID = ID_NULL);
	BOOL DeleteRow(int nRow);
	void CalcRowsPos();
	void CalcColumnsPos();
	BOOL EnsureVisible(int nRow, BOOL bPartialOK);
	int  GetRowCount() { return m_vecRowInfo.size(); }
	GridRowInfo* GetRowInfo(int nRow);
	GridItemInfo* GetItemInfo(int nRow, int nItem);
	void SetRowColor(int nRow, Color clrText);
	void SetRowBackColor(int nRow, Color clrBack);
	void SetRowCheck(int nRow, int nCheck);
	int  GetRowCheck(int nRow);
	void ClearItems();

	BOOL PtInRow(CPoint point, GridRowInfo& rowInfo);
	BOOL PtInRowCheck(CPoint point, GridRowInfo& rowInfo);
	int  PtInRowItem(CPoint point, GridRowInfo& rowInfo);

	void SetGridTooltip(int nRow, int nItem, CString strTooltip);
	void ClearGridTooltip();

protected:
	vector<GridColumnInfo> m_vecColumnInfo;
	vector<GridRowInfo> m_vecRowInfo;

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonUp(UINT nFlags, CPoint point);

	HRESULT OnAttributeFontTitle(const CString& strValue, BOOL bLoading);

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	CString				m_strFontTitle;		// ��������
	int					m_nFontTitleWidth;	// ����������
	FontStyle			m_fontTitleStyle;	// ��������Style
	UINT					m_uAlignmentHeader;		// ��������ˮƽ���뷽ʽ
	UINT					m_uVAlignmentHeader;		// �������ִ�ֱ���뷽ʽ
	Color				m_clrHeader;			// ������������ɫ
	Color				m_clrText;			// ������ɫ
	Color				m_clrTextHover;		// ������ɫ(����ƶ�)
	Color				m_clrTextDown;		// ������ɫ(��갴��)
	Color				m_clrTitle;			// ������ɫ
	Color				m_clrSeperator;		// �ָ�����ɫ
	Color				m_clrRowHover;		// �б�����ɫ(����ƶ�����)
	int					m_nLeftPos;			// �����ʼλ��
	int					m_nRowHeight;		// �и߶�
	int					m_nHeaderHeight;	// �����и߶�
	int					m_nBkTransparent;	// ����͸����
	BOOL				m_bSingleLine;		// ��ʾ��������
	BOOL				m_bTextWrap;		// �����Ƿ���
	BOOL			m_bShowColumnSeperator;	// �Ƿ���ʾ���ݲ��ֵ��зָ���

	int					m_nHoverRow;		// ��ǰ����ƶ���������
	int					m_nDownRow;			// ��ǰ�����������
	BOOL				m_bEnableDownRow;	// ������ʾ��ǰ�����

	int					m_nFirstViewRow;	// ��ǰ��ʾ���ĵ�һ�е����
	int					m_nLastViewRow;		// ��ǰ��ʾ�������һ�е����
	int					m_nVirtualTop;		// ��ǰ������λ�ö�Ӧ�������topλ��
	int					m_nVirtualLeft;		// ��ǰ������λ�ö�Ӧ�������leftλ��

	BOOL				m_bGridTooltip;		// �Ƿ���ʾ��Ԫ���Tooltip
	int					m_nTipRow;			// ��ǰtip��
	int					m_nTipItem;			// ��ǰtip��
	int					m_nTipVirtualTop;	// ��ǰtip�е�����Top

	BOOL			m_bEnableModifyColumn;	// �Ƿ������޸��п��
	enumButtonState m_enButtonState;	// ���״̬
	BOOL			m_bHoverSplitColumn;	// �Ƿ�����ȵ�״̬(�зָ���)
	int					m_nHoverSplitColumn;	// ����϶����зָ�������

	DUI_IMAGE_ATTRIBUTE_DEFINE(Header);	// ��������б���ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(ColumnSeperator);	// �����зָ���ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);	// �����зָ���ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(CheckBox);	// �������ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_ENUM_ATTRIBUTE(_T("valign-header"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("top"), VAlign_Top)
            DUI_ENUM_VALUE(_T("middle"), VAlign_Middle)
            DUI_ENUM_VALUE(_T("bottom"), VAlign_Bottom)
        DUI_ENUM_END(m_uVAlignmentHeader)
        DUI_ENUM_ATTRIBUTE(_T("align-header"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("left"), Align_Left)
            DUI_ENUM_VALUE(_T("center"), Align_Center)
            DUI_ENUM_VALUE(_T("right"), Align_Right)
        DUI_ENUM_END(m_uAlignmentHeader)
		DUI_CUSTOM_ATTRIBUTE(_T("img-header"), OnAttributeImageHeader)
		DUI_CUSTOM_ATTRIBUTE(_T("img-colsep"), OnAttributeImageColumnSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-sep"), OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-check"), OnAttributeImageCheckBox)
		DUI_CUSTOM_ATTRIBUTE(_T("font-title"), OnAttributeFontTitle)
		DUI_COLOR_ATTRIBUTE(_T("crheader"), m_clrHeader, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crhover"), m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crpush"), m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtitle"), m_clrTitle, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crsep"), m_clrSeperator, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crrowhover"), m_clrRowHover, FALSE)
		DUI_INT_ATTRIBUTE(_T("row-height"), m_nRowHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("header-height"), m_nHeaderHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("left-pos"), m_nLeftPos, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("wrap"), m_bTextWrap, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("down-row"), m_bEnableDownRow, FALSE)
		DUI_INT_ATTRIBUTE(_T("bk-transparent"), m_nBkTransparent, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("grid-tip"), m_bGridTooltip, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("column-sep"), m_bShowColumnSeperator, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("modify-column-width"), m_bEnableModifyColumn, TRUE)
    DUI_DECLARE_ATTRIBUTES_END()
};
