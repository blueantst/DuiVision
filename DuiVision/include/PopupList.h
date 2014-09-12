#pragma once
#include "DlgPopup.h"

#include <vector>
using namespace std;

// 消息
#define				SELECT_ITEM					11
#define				DELETE_ITEM					12

// 列表项定义
struct EditListItem
{
	UINT		nResourceID;	// ID
	CString		strImageFile;	// 图片文件
	Image*		pImage;			// 图片指针
	CSize		sizeImage;		// 图片大小
	CString		strName;		// 显示名
	CString		strDesc;		// 描述
	CString		strValue;		// 值
	CRect		rcItem;			// 位置
};

class CPopupList :  public CDlgPopup
{
	static LPCTSTR GetClassName() { return _T("popuplist");}

public:
	CPopupList(void);
	virtual ~CPopupList(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

	void SetWidth(int nWidth) { m_nWidth = nWidth; }

	bool GetItemDesc(UINT nItem, CString &strDesc);
	bool GetItemName(UINT nItem, CString &strName);
	bool GetItemValue(UINT nItem, CString &strValue);
	bool GetItemImage(UINT nItem, UINT &nResourceID, CString &strImageFile);
	int AddItem(CString strName, CString strDesc, CString strValue, int nResourceID, CString strImageFile = _T(""));
	bool DeleteItem(UINT nItem);

	void SetCurItem(UINT nItem);
	void SetCurItem(CString strValue);

	virtual void DrawWindow(CDC &dc, CRect rcClient);
	virtual void DrawWindowEx(CDC &dc, CRect rcClient);

	virtual void InitUI(CRect rcClient);

	void SetItemPoint();

	virtual BOOL OnMouseMove(CPoint point);
	virtual BOOL OnLButtonDown(CPoint point);
	virtual BOOL OnLButtonUp(CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

public:
	CRect					m_rcClose;			// 列表项的关闭区域
	enumButtonState			m_buttonState;
	vector<EditListItem>	m_vecItem;
	int						m_nHoverItem;
	CFont					m_font;
	int						m_nWidth;			// 列表宽度

	DUI_IMAGE_ATTRIBUTE_DEFINE(Head);		// 定义列表项左侧图片
	DUI_IMAGE_ATTRIBUTE_DEFINE(Close);		// 定义列表项删除图片
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("width"), m_nWidth, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
