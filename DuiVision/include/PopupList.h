#pragma once
#include "DlgPopup.h"

#include <vector>
using namespace std;

// ÏûÏ¢
#define				SELECT_ITEM					11
#define				DELETE_ITEM					12

struct EditListItem
{
	UINT nResourceID;
	CString strImageFile;
	Image *pImage;
	CSize sizeImage;
	CString strName;
	CString strDesc;
	CString strValue;
	CRect rcItem;
};

class CPopupList :  public CDlgPopup
{
	static LPCSTR GetClassName() { return "popuplist";}

public:
	CPopupList(void);
	virtual ~CPopupList(void);

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);

	void SetWidth(int nWidth) { m_nWidth = nWidth; }
	void SetHeadBitmap(UINT nResourceID);
	void SetHeadBitmap(CString strImage);
	void SetDeleteBitmap(UINT nResourceID);
	void SetDeleteBitmap(CString strImage);

	bool GetItemDesc(UINT nItem, CString &strDesc);
	bool GetItemName(UINT nItem, CString &strName);
	bool GetItemValue(UINT nItem, CString &strValue);
	bool GetItemImage(UINT nItem, UINT &nResourceID, CString &strImageFile);
	bool DeleteItem(UINT nItem);

	void SetCurItem(UINT nItem);
	void SetCurItem(CString strValue);

	virtual void DrawWindow(CDC &dc, CRect rcClient);
	virtual void DrawWindowEx(CDC &dc, CRect rcClient);

	virtual void InitUI(CRect rcClient);
	int AddItem(CString strName, CString strDesc, CString strValue, int nResourceID, CString strImageFile = _T(""));

	virtual BOOL OnMouseMove(CPoint point);
	virtual BOOL OnLButtonDown(CPoint point);
	virtual BOOL OnLButtonUp(CPoint point);

private:
	void SetItemPoint();

protected:
	Image*			m_pHeadImage;
	Image*			m_pCloseImage;
	CSize			m_sizeHeadImage;
	CSize			m_sizeCloseImage;
	CRect			m_rcClose;
	enumButtonState   m_buttonState;
	vector<EditListItem> m_vecItem;
	int				m_nHoverItem;
	CFont			m_font;
	int				m_nWidth;
};
