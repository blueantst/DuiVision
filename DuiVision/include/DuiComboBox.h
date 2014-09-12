#pragma once
#include "DuiEdit.h"

// 下拉列表项信息
struct ComboListItem
{
	UINT		nResourceID;		// 图片资源ID
	CString		strImageFile;		// 图片文件
	CString		strName;			// 列表项名(显示在列表项第一行)
	CString		strDesc;			// 描述(显示在列表项第二行)
	CString		strValue;			// 列表项值
};

class CDuiComboBox : public CDuiEdit
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiComboBox, _T("combobox"))
public:
	CDuiComboBox(HWND hWnd, CDuiObject* pDuiObject);
	virtual ~CDuiComboBox(void);

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

	void SetComboValue(CString strComboValue);
	CString GetComboValue();
	int GetItemCount();
	int AddItem(CString strName, CString strDesc, CString strValue, int nResourceID, CString strImageFile = _T(""));
	void ClearItems();

	HRESULT OnAttributeHeadImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeDeleteImage(const CString& strValue, BOOL bLoading);

	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	
public:
	UINT					m_nResourceIDHeadBitmap;
	UINT					m_nResourceIDDeleteBitmap;
	CString					m_strImageHeadBitmap;
	CString					m_strImageDeleteBitmap;
	CString					m_strXmlFile;	// 定义下拉列表项的XML文件(可以不需要)
	CPopupList*				m_pPopupList;	// 保存下拉列表框对象指针
	vector<ComboListItem>	m_vecItem;		// 下拉列表项列表
	CString					m_strComboValue;// 下拉列表选择的项目的值

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("head-image"), OnAttributeHeadImage)
		DUI_CUSTOM_ATTRIBUTE(_T("del-image"), OnAttributeDeleteImage)
		DUI_TSTRING_ATTRIBUTE(_T("value"), m_strComboValue, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("xml"), m_strXmlFile, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};