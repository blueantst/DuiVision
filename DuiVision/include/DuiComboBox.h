#pragma once
#include "DuiEdit.h"

// �����б�����Ϣ
struct ComboListItem
{
	UINT		nResourceID;		// ͼƬ��ԴID
	CString		strImageFile;		// ͼƬ�ļ�
	CString		strName;			// �б�����(��ʾ���б����һ��)
	CString		strDesc;			// ����(��ʾ���б���ڶ���)
	CString		strValue;			// �б���ֵ
	Color		clrText;			// ������ɫ
	Color		clrDesc;			// ����������ɫ
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
	int						m_nListHeight;	// �б�߶�
	CString					m_strImageScroll;	// ���������б�Ĺ�����ͼƬ�ļ�
	CString					m_strXmlFile;	// ���������б����XML�ļ�(���Բ���Ҫ)
	CPopupList*				m_pPopupList;	// ���������б�����ָ��
	vector<ComboListItem>	m_vecItem;		// �����б����б�
	CString					m_strComboValue;// �����б�ѡ�����Ŀ��ֵ
	Color					m_clrText;		// ������ɫ
	Color					m_clrDesc;		// ��������������ɫ
	Color					m_clrHover;		// ѡ����б������ɫ(����ƶ�����)

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