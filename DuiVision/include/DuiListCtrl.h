// �б�ؼ�
#pragma once

#include "Panel.h"

using namespace  std;

// �б�����Ϣ
struct ListRowInfo
{
	CRect	rcRow;			// ��λ����Ϣ
	CString	strId;			// ID
	CString strTitle;		// ����
	CString strContent;		// ����
	CString	strTime;		// ʱ��
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
	CString strLink1;		// ����1������
	CString strLinkAction1;	// ����1�Ķ���
	CRect	rcLink1;		// ����1λ����Ϣ
	CString strLink2;		// ����2������
	CString strLinkAction2;	// ����2�Ķ���
	CRect	rcLink2;		// ����2λ����Ϣ
	int		nHoverLink;		// ��ǰ�����ȵ�״̬����������
	BOOL	bNeedTitleTip;	// �Ƿ���Ҫ��ʾtitle tip(titleʵ�ʿ�ȴ�����ʾ���)
	BOOL	bNeedContentTip;// �Ƿ���Ҫ��ʾcontent tip(contentʵ�ʿ�ȴ�����ʾ���)
};

class CDuiListCtrl : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiListCtrl, _T("listctrl"))

public:
	CDuiListCtrl(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiListCtrl(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

	int InsertItem(int nItem, CString strId, CString strTitle, CString strContent, CString strTime = _T(""),
		int nImageIndex = -1, Color clrText = Color(0, 0, 0, 0), CString strImage = _T(""),
		int nRightImageIndex = -1, CString strRightImage = _T(""),
		CString strLink1 = _T(""), CString strLinkAction1 = _T(""),
		CString strLink2 = _T(""), CString strLinkAction2 = _T(""),
		int nCheck = -1, Color clrBack = Color(0, 0, 0, 0));
	int InsertItem(int nItem, CString strTitle, int nCheck = -1, Color clrText = Color(0, 0, 0, 0), int nImageIndex = -1,
		CString strLink1 = _T(""), CString strLinkAction1 = _T(""),
		CString strLink2 = _T(""), CString strLinkAction2 = _T(""), Color clrBack = Color(0, 0, 0, 0));
	int InsertItem(int nItem, ListRowInfo &rowInfo);
	BOOL DeleteItem(int nItem);
	void CalcItemsPos();
	BOOL EnsureVisible(int nRow, BOOL bPartialOK);
	int  GetItemCount() { return m_vecRowInfo.size(); }
	ListRowInfo* GetItemInfo(int nRow);
	void SetRowColor(int nRow, Color clrText);
	void SetRowBackColor(int nRow, Color clrBack);
	void SetRowCheck(int nRow, int nCheck);
	int  GetRowCheck(int nRow);
	void ClearItems();

	BOOL PtInRow(CPoint point, ListRowInfo& rowInfo);
	BOOL PtInRowCheck(CPoint point, ListRowInfo& rowInfo);
	int  PtInRowLink(CPoint point, ListRowInfo& rowInfo);

	void SetRowTooltip(int nRow, CString strTooltip);
	void ClearRowTooltip();

protected:
	vector<ListRowInfo> m_vecRowInfo;

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
	int					m_nRowHeight;		// �и߶�
	int					m_nBkTransparent;	// ����͸����
	BOOL				m_bSingleLine;		// ��ʾ��������
	BOOL				m_bTextWrap;		// �����Ƿ���

	int					m_nHoverRow;		// ��ǰ����ƶ���������
	int					m_nDownRow;			// ��ǰ�����������
	BOOL				m_bEnableDownRow;	// ������ʾ��ǰ�����

	int					m_nFirstViewRow;	// ��ǰ��ʾ���ĵ�һ�е����
	int					m_nLastViewRow;		// ��ǰ��ʾ�������һ�е����
	int					m_nVirtualTop;		// ��ǰ������λ�ö�Ӧ�������topλ��

	BOOL				m_bRowTooltip;		// �Ƿ���ʾ�е�Tooltip(title����)
	int					m_nTipRow;			// ��ǰtip��
	int					m_nTipVirtualTop;	// ��ǰtip�е�����Top

	DUI_IMAGE_ATTRIBUTE_DEFINE(Seperator);	// �����зָ���ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(CheckBox);	// �������ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("img-sep"), OnAttributeImageSeperator)
		DUI_CUSTOM_ATTRIBUTE(_T("img-check"), OnAttributeImageCheckBox)
		DUI_CUSTOM_ATTRIBUTE(_T("font-title"), OnAttributeFontTitle)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crhover"), m_clrTextHover, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crpush"), m_clrTextDown, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtitle"), m_clrTitle, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crsep"), m_clrSeperator, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crrowhover"), m_clrRowHover, FALSE)
		DUI_INT_ATTRIBUTE(_T("row-height"), m_nRowHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("wrap"), m_bTextWrap, FALSE)
		DUI_INT_ATTRIBUTE(_T("down-row"), m_bEnableDownRow, FALSE)
		DUI_INT_ATTRIBUTE(_T("bk-transparent"), m_nBkTransparent, FALSE)
		DUI_INT_ATTRIBUTE(_T("row-tip"), m_bRowTooltip, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
