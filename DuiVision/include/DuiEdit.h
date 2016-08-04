#pragma once
#include "ControlBase.h"

// edit�ؼ��е��ӿؼ�����
#define	CONTROL_BUTTON		1	// �༭���еİ�ť����
#define	CONTROL_EDIT		2	// �༭���зǰ�ť����

class CDuiEdit : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiEdit, _T("edit"))
public:
	CDuiEdit(HWND hWnd, CDuiObject* pDuiObject);
	CDuiEdit(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle= TEXT(""), BOOL bPassWord = FALSE, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE, BOOL bIsPressDown = FALSE);
	virtual ~CDuiEdit(void);

	// �������СͼƬ
	bool SetLeftBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	bool SetLeftBitmap(CString strImage);
	HRESULT OnAttributeLeftImage(const CString& strValue, BOOL bLoading);

	// �����Ҳ�СͼƬ
	bool SetSmallBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	bool SetSmallBitmap(CString strImage);
	HRESULT OnAttributeSmallImage(const CString& strValue, BOOL bLoading);

	void SetBackColor(Color clrBack);
	HRESULT OnAttributeBackColor(const CString& strValue, BOOL bLoading);

	virtual void SetControlRect(CRect rc);
	virtual BOOL SetControlFocus(BOOL bFocus);
	virtual BOOL IsDraw(CPoint point) ;
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlTitle(CString strTitle);
	BOOL GetLButtonDown();
	BOOL GetLButtonUp();
	CString GetEditText();

	BOOL IsReadOnly() { return m_bReadOnly; }
	void SetReadOnly(BOOL bReadOnly) { m_bReadOnly = bReadOnly; }

	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual	void SetControlDisable(BOOL bIsDisable);

	void CreateEditFont();
	void ShowEdit();
	void HideEdit();
	void DeleteEdit();
	
public:
	Image*			m_pLeftImage;		// ���СͼƬ
	CSize			m_sizeLeftImage;	// ���СͼƬ��С
	int				m_nLeftImageCount;	// ���СͼƬ�ĸ���

	Image*			m_pSmallImage;		// �Ҳ�СͼƬ
	CSize			m_sizeSmallImage;	// �Ҳ�СͼƬ��С
	BOOL			m_bIsSmallButton;	// �Ҳ�СͼƬ�Ƿ���Ϊ��ť������
	int				m_nSmallImageCount;	// �Ҳ�СͼƬ�ĸ���

	BOOL			m_bPassWord;		// ��������
	BOOL			m_bMultiLine;		// ����
	BOOL			m_bWantReturn;		// �Ƿ�����س�����
	BOOL			m_bAutoHScroll;		// ����ˮƽ����
	BOOL			m_bAutoVScroll;		// ����ֱ����
	BOOL			m_bShowHScroll;		// ��ʾˮƽ������
	BOOL			m_bShowVScroll;		// ��ʾ��ֱ������
	BOOL			m_bNumber;			// ֻ����������
	BOOL			m_bReadOnly;		// ֻ��
	int				m_nMaxChar;			// ����ַ���

	bool			m_bDown;
	bool			m_bDownTemp;
	enumButtonState m_buttonState;
	enumButtonState m_EditState;
	CRect			m_rcText;			// ���ֲ���λ��

	bool				m_bBack;			// �Ƿ���䱳��
	Color			m_clrBack;			// ����ɫ
	Color			m_clrText;			// ������ɫ
	Color			m_clrTooltip;		// ��ʾ��Ϣ��ɫ
	CFont			m_fontTemp;			// ����������
	CEdit*			m_pEdit;			// Windowsԭ������ؼ�

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_BOOL_ATTRIBUTE(_T("password"), m_bPassWord, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("multiline"), m_bMultiLine, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("wantreturn"), m_bWantReturn, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("autohscroll"), m_bAutoHScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("autovscroll"), m_bAutoVScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("hscroll"), m_bShowHScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("vscroll"), m_bShowVScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("number"), m_bNumber, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("readonly"), m_bReadOnly, FALSE)
		DUI_INT_ATTRIBUTE(_T("maxchar"), m_nMaxChar, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("left-image"), OnAttributeLeftImage)
		DUI_CUSTOM_ATTRIBUTE(_T("small-image"), OnAttributeSmallImage)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("crback"), OnAttributeBackColor)
		DUI_COLOR_ATTRIBUTE(_T("crtip"), m_clrTooltip, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};