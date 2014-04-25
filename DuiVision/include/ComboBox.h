#pragma once
#include "EditEx.h"

struct ComboListItem
{
	UINT nResourceID;
	CString strImageFile;
	CString strName;
	CString strDesc;
	CString strValue;
};

class CDuiComboBox : public CEditEx
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiComboBox, "combobox")
public:
	CDuiComboBox(HWND hWnd, CDuiObject* pDuiObject);
	virtual ~CDuiComboBox(void);

	virtual BOOL Load(TiXmlElement* pXmlElem, BOOL bLoadSubControl = TRUE);

	void SetComboValue(CString strComboValue);
	CString GetComboValue();
	int GetItemCount();
	int AddItem(CString strName, CString strDesc, CString strValue, int nResourceID, CString strImageFile = _T(""));
	void ClearItems();

	HRESULT OnAttributeHeadImage(const CStringA& strValue, BOOL bLoading);
	HRESULT OnAttributeDeleteImage(const CStringA& strValue, BOOL bLoading);

	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	UINT		m_nResourceIDHeadBitmap;
	UINT		m_nResourceIDDeleteBitmap;
	CString		m_strImageHeadBitmap;
	CString		m_strImageDeleteBitmap;
	CString		m_strXmlFile;
	CPopupList*	m_pPopupList;	// 保存下拉列表框对象指针
	vector<ComboListItem> m_vecItem;
	CString		m_strComboValue;

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("head-image", OnAttributeHeadImage)
		DUI_CUSTOM_ATTRIBUTE("del-image", OnAttributeDeleteImage)
		DUI_TSTRING_ATTRIBUTE("value", m_strComboValue, FALSE)
		DUI_TSTRING_ATTRIBUTE("xml", m_strXmlFile, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};