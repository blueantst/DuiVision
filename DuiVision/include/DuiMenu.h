#pragma once
#include "DlgPopup.h"

#include <vector>
using namespace std;

// �˵���Ԥ������Ϣ
struct MenuItemValue
{
	//UINT		uID;			// �˵���ID
	CString		strName;		// �˵�������
	CString		strTitle;		// �˵������
	BOOL		bVisible;		// �Ƿ�ɼ�
	BOOL		bDisable;		// �Ƿ����
	int			nCheck;			// �Ƿ�ѡ��
};

class CMenuItem;

class CDuiMenu : public CDlgPopup
{
public:
	static LPCTSTR GetClassName() { return _T("menu");}
	virtual BOOL IsClass(LPCTSTR lpszName)
	{
		if(_tcscmp(GetClassName(), lpszName)  == 0) return TRUE;
		return __super::IsClass(lpszName);
	}

public:
	CDuiMenu(CString strFont = DuiSystem::GetDefaultFont(), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular);
	virtual ~CDuiMenu(void);

	// �����˵�����
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID, UINT nResourceID, int nFrameSize = 4, int nMinWidth = 113, enumBackMode enBackMode = enBMFrame);
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID, CString strImage, int nFrameSize = 4, int nMinWidth = 113, enumBackMode enBackMode = enBMFrame);
	BOOL Create(CWnd *pParent, CPoint point, UINT uMessageID);

	// ��Ӳ˵���ͷָ���
	int AddMenu(CString strText, UINT uMenuID, int nResourceID = -1, BOOL bSelect = false, int nIndex = -1);
	int AddMenu(CString strText, UINT uMenuID, CString strImage, BOOL bSelect = false, int nIndex = -1);
	int AddSeparator(int nIndex = -1);

	// Ԥ��ֵ�˵��������
	void SetItemTitle(CString strName, CString strTitle);
	void SetItemVisible(CString strName, BOOL bVisible);
	void SetItemDisable(CString strName, BOOL bDisable);
	void SetItemCheck(CString strName, int nCheck);

	// ���ò˵���λ��
	void SetMenuPoint();

	// ��ȡ���˵�����
	CDuiMenu* GetParentMenu();
	// ��ȡ��ǰ����˵������
	CMenuItem* GetHoverMenuItem();
	// ��ȡ��ǰ���ڻ״̬���Ӳ˵���(�������λ�ò���)
	CMenuItem* GetMenuItemWithPoint(CPoint point);

	// ���ز˵�
	BOOL LoadSubMenu(DuiXmlNode pXmlElem, CString strSubItemName);
	BOOL LoadXmlNode(DuiXmlNode pXmlElem, CString strXmlFile = _T(""));
	BOOL LoadXmlFile(CString strFileName, CString strSubItemName = _T(""));
	BOOL LoadXmlFile(CString strFileName, CWnd *pParent, CPoint point, UINT uMessageID, CString strSubItemName = _T(""));

	virtual void DrawWindowEx(CDC &dc, CRect rcClient);
	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	virtual void InitUI(CRect rcClient);

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);

	virtual BOOL OnNcCloseWindow();

public:
	CString		m_strTitle;				// ����
	CString		m_strFont;				// �˵�����
	int			m_nFontWidth;			// ������
	FontStyle	m_fontStyle;			// ������
	UINT		m_uAlignment;			// ˮƽ���뷽ʽ
	UINT		m_uVAlignment;			// ��ֱ���뷽ʽ

	int			m_nLeft;				// ��߼��
	int			m_nHeight;				// �˵���Ĭ�ϸ߶�
	int			m_nWidth;				// �˵��������
	int			m_nFrameWidth;			// �˵������߿�Ŀ��
	int			m_nTopHeight;			// �˵��������߿�ĸ߶�
	int			m_nBottomHeight;		// �˵���ײ�����߿�ĸ߶�
	int			m_nSeparatorHeight;		// �ָ��߸߶�
	CPoint		m_point;				// �˵�λ��
	CWnd*		m_pParent;				// ������

	Color		m_clrRowHover;			// �б�����ɫ(����ƶ����˵���)

	vector<MenuItemValue> m_vecMenuItemValue;	// �˵���Ԥ������Ϣ

	DUI_IMAGE_ATTRIBUTE_DEFINE(RowHover);	// �����б���ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(PopupArrow);	// ���嵯���˵���ͷͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_TSTRING_ATTRIBUTE(_T("title"), m_strTitle, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("font"), m_strFont, FALSE)
		DUI_INT_ATTRIBUTE(_T("fontwidth"), m_nFontWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("item-height"), m_nHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("frame-width"), m_nFrameWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("top-height"), m_nTopHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("bottom-height"), m_nBottomHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("left"), m_nLeft, FALSE)
		DUI_INT_ATTRIBUTE(_T("sep-height"), m_nSeparatorHeight, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crrowhover"), m_clrRowHover, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("img-rowhover"), OnAttributeImageRowHover)
		DUI_CUSTOM_ATTRIBUTE(_T("img-popuparrow"), OnAttributeImagePopupArrow)
	DUI_DECLARE_ATTRIBUTES_END()
};
