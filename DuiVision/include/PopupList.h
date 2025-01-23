#pragma once
#include "DlgPopup.h"

#include <vector>
using namespace std;

// ������ʾģʽ
enum enumPopupTextMode
{
	enPopupTextAuto = 0,			// �Զ���ʾ
	enPopupTextHoverEnlarge,		// ����ȵ��еĸ߶ȷŴ�
	enPopupTextFixedHeight,			// ÿһ�еĸ߶ȹ̶���ֻ��ʾ����
	enPopupTextLeftRight			// ÿһ�еĸ߶ȹ̶��������ʾ���֣��Ҳ���ʾ����
};

// �б����
struct EditListItem
{
	UINT		nResourceID;	// ID
	CString		strImageFile;	// ͼƬ�ļ�
	Image*		pImage;			// ͼƬָ��
	CSize		sizeImage;		// ͼƬ��С
	CString		strName;		// ��ʾ��
	CString		strDesc;		// ����
	CString		strValue;		// ֵ
	CRect		rcItem;			// λ��
	Color		clrText;		// ������ɫ
	Color		clrDesc;		// ����������ɫ
};

class CPopupList :  public CDlgPopup
{
	static LPCTSTR GetClassName() { return _T("popuplist");}

public:
	CPopupList(void);
	virtual ~CPopupList(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

	void SetWidth(int nWidth) { m_nWidth = nWidth; }
	void SetHeight(int nHeight) { m_nHeight = nHeight; }
	void SetFont(CString strFont, int nFontWidth, FontStyle fontStyle);
	void SetTitleFont(CString strFont, int nFontWidth, FontStyle fontStyle);
	void SetHoverColor(Color clrHover);
	void SetRowHeight(int nRowHeight) { m_nRowHeight = nRowHeight; }
	void SetTextMode(enumPopupTextMode enTextMode) { m_enTextMode = enTextMode; }

	bool GetItemDesc(UINT nItem, CString &strDesc);
	bool GetItemName(UINT nItem, CString &strName);
	bool GetItemValue(UINT nItem, CString &strValue);
	bool GetItemImage(UINT nItem, UINT &nResourceID, CString &strImageFile);
	int AddItem(CString strName, CString strDesc, CString strValue, int nResourceID,
		CString strImageFile = _T(""), Color clrText = Color(255, 0, 20, 35), Color clrDesc = Color(255, 255, 255, 255));
	bool DeleteItem(UINT nItem);

	void SetCurItem(UINT nItem);
	void SetCurItem(CString strValue);

	virtual void InitUI(CRect rcClient);

	void SetItemPoint();

	HRESULT OnAttributeImageScrollV(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeFontTitle(const CString& strValue, BOOL bLoading);

	virtual BOOL OnMouseMove(CPoint point);
	virtual BOOL OnLButtonDown(CPoint point);
	virtual BOOL OnLButtonUp(CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	virtual void DrawWindow(CDC &dc, CRect rcClient);
	virtual void DrawWindowEx(CDC &dc, CRect rcClient);

public:
	CRect					m_rcClose;			// �б���Ĺر�����
	enumButtonState			m_buttonState;
	vector<EditListItem>	m_vecItem;			// �������б�
	int						m_nHoverItem;		// ��ǰѡ����б�������
	int						m_nWidth;			// �б���
	int						m_nHeight;			// �б�߶�

	CString					m_strFont;			// ��������
	int						m_nFontWidth;		// ����������
	FontStyle				m_fontStyle;		// ��������Style
	CString					m_strFontTitle;		// ��������
	int						m_nFontTitleWidth;	// ����������
	FontStyle				m_fontTitleStyle;	// ��������Style

	Color					m_clrHover;			// ѡ����б������ɫ(����ƶ�����)
	BOOL					m_bSingleLine;		// �Ƿ��е��б�
	enumPopupTextMode		m_enTextMode;		// ������ʾģʽ
	int						m_nVirtualHeight;	// ����ĸ߶�
	CControlBaseFont*		m_pControScrollV;	// ��ֱ������
	int						m_nScrollWidth;		// ���������
	int						m_nRowHeight;		// �и߶�
	int						m_nVirtualTop;		// ��ǰ������ʾ�Ķ���λ��

	DUI_IMAGE_ATTRIBUTE_DEFINE(Head);			// �����б������ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE_DPI(Close);		// �����б���ɾ��ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("width"), m_nWidth, FALSE)
		DUI_INT_ATTRIBUTE_DPI(_T("height"), m_nHeight, FALSE)
		DUI_CUSTOM_ATTRIBUTE("img-scroll", OnAttributeImageScrollV)
		DUI_INT_ATTRIBUTE("scroll-width", m_nScrollWidth, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("font-title"), OnAttributeFontTitle)
		DUI_INT_ATTRIBUTE_DPI(_T("row-height"), m_nRowHeight, FALSE)
		DUI_ENUM_ATTRIBUTE(_T("text-mode"), enumPopupTextMode, TRUE)
			DUI_ENUM_VALUE(_T("auto"), enPopupTextAuto)
			DUI_ENUM_VALUE(_T("hover-enlarge"), enPopupTextHoverEnlarge)
			DUI_ENUM_VALUE(_T("fixed-height"), enPopupTextFixedHeight)
			DUI_ENUM_VALUE(_T("left-right"), enPopupTextLeftRight)
		DUI_ENUM_END(m_enTextMode)
    DUI_DECLARE_ATTRIBUTES_END()
	
protected:
	int GetItemsOffset();//�б��������ʾ��������ƫ����
	DECLARE_DYNAMIC(CPopupList)
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
