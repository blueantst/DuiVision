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
	Color		clrText;			// 文字颜色
	Color		clrDesc;			// 描述文字颜色
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
	int AddItem(CString strName, CString strDesc, CString strValue, int nResourceID,
		CString strImageFile = _T(""), Color clrText = Color(255, 0, 20, 35), Color clrDesc = Color(255, 255, 255, 255));
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
	int						m_nListHeight;	// 列表高度
	CString					m_strImageScroll;	// 定义下拉列表的滚动条图片文件
	CString					m_strXmlFile;	// 定义下拉列表项的XML文件(可以不需要)
	CPopupList*				m_pPopupList;	// 保存下拉列表框对象指针
	vector<ComboListItem>	m_vecItem;		// 下拉列表项列表
	CString					m_strComboValue;// 下拉列表选择的项目的值
	Color					m_clrText;		// 文字颜色
	Color					m_clrDesc;		// 描述部分文字颜色
	Color					m_clrHover;		// 选择的列表项背景颜色(鼠标移动到行)

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("head-image"), OnAttributeHeadImage)
		DUI_CUSTOM_ATTRIBUTE(_T("del-image"), OnAttributeDeleteImage)
		DUI_TSTRING_ATTRIBUTE(_T("img-scroll"), m_strImageScroll, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("value"), m_strComboValue, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("xml"), m_strXmlFile, FALSE)
		DUI_INT_ATTRIBUTE(_T("list-height"), m_nListHeight, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crdesc"), m_clrDesc, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crhover"), m_clrHover, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};