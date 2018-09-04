#pragma once
#include "DlgPopup.h"

#include <vector>
using namespace std;

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
	void SetHoverColor(Color clrHover);

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
	CString					m_strFont;			// ����
	int						m_nFontWidth;		// ������
	FontStyle				m_fontStyle;		// ����Style
	Color					m_clrHover;			// ѡ����б������ɫ(����ƶ�����)
	BOOL					m_bSingleLine;		// �Ƿ��е��б�
	int						m_nVirtualHeight;	// ����ĸ߶�
	CControlBaseFont*		m_pControScrollV;	// ��ֱ������
	int						m_nScrollWidth;		// ���������
	int						m_nVirtualTop;		// ��ǰ������ʾ�Ķ���λ��

	DUI_IMAGE_ATTRIBUTE_DEFINE(Head);			// �����б������ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(Close);			// �����б���ɾ��ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("width"), m_nWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("height"), m_nHeight, FALSE)
		DUI_CUSTOM_ATTRIBUTE("img-scroll", OnAttributeImageScrollV)
		DUI_INT_ATTRIBUTE("scroll-width", m_nScrollWidth, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()

	
protected:
	int GetItemsOffset();//�б��������ʾ��������ƫ����
	DECLARE_DYNAMIC(CPopupList)
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};
