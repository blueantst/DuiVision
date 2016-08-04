#pragma once

#include "ControlBase.h"

// ������ģʽ
enum SCROLL_TYPE
{
	SCROLL_TYPE_VERTICAL = 0,			// ��ֱ����
	SCROLL_TYPE_HORIZONTAL,				// ˮƽ����
};

class CDuiScroll : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiScroll, _T("scroll"))
public:
	CDuiScroll(HWND hWnd, CDuiObject* pDuiObject);
	CDuiScroll(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible = TRUE);
	virtual ~CDuiScroll(void);

	BOOL SetScrollCurrentPos(int nCurrentPos);
	int GetScrollCurrentPos() { return m_nCurrentPos; }
	BOOL SetScrollMaxRange(int nMaxRange);
	int GetScrollMaxRange() { return m_nMaxRange; }
	BOOL SetScrollSize(int nPageRange, int nRowRange = 0);
	BOOL SetScrollInfo(int nMaxRange, int nCurrentPos, int nPageRange = 0, int nRowRange = 0);
	BOOL ScrollRow(int nRow);

	HRESULT OnAttributeMaxRange(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeCurrentPos(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributePageRange(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeRowRange(const CString& strValue, BOOL bLoading);

	virtual void SetControlRect(CRect rc);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);

protected:	
	virtual int  MoveRange(int nMove);
	virtual int  SetRange();

	// ��Ϣ��Ӧ
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	SCROLL_TYPE		m_nScrollType;			// ����������
	CBitmap			m_bitmap;
	enumButtonState m_enButtonState;		// ���״̬
	BOOL			m_bHover;				// �Ƿ�����ȵ�״̬
	BOOL			m_bShowScroll;			// �Ƿ���ʾ������

	CControlBase*	m_pControlUpImage;		// �ϼ�ͷ�ؼ�
	CControlBase*	m_pControlDownImage;	// �¼�ͷ�ؼ�

	int				m_nArrowLen;			// ��ͷ�ĳ���

	int				m_nMaxRange;			// ���ֵ
	int				m_nCurrentPos;			// ��ǰλ��
	int				m_nPageRange;			// һҳ������
	int				m_nRowRange;			// һ�з�����
	CRect			m_rcBlock;				// �������С
	BOOL			m_bAutoCalcRange;		// �Ƿ��Զ�����ķ�ҳ��Χ

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("maxrange"), OnAttributeMaxRange)
		DUI_CUSTOM_ATTRIBUTE(_T("curpos"), OnAttributeCurrentPos)
		DUI_CUSTOM_ATTRIBUTE(_T("pagerange"), OnAttributePageRange)
		DUI_CUSTOM_ATTRIBUTE(_T("rowrange"), OnAttributeRowRange)
	DUI_DECLARE_ATTRIBUTES_END()
};

// ��ֱ���������
class CDuiScrollVertical : public CDuiScroll
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiScrollVertical, _T("scrollv"))
public:
	CDuiScrollVertical(HWND hWnd, CDuiObject* pDuiObject);
	CDuiScrollVertical(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible = TRUE);
	virtual ~CDuiScrollVertical(void);

	virtual BOOL SetBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	virtual BOOL SetBitmap(CString strImage);

	HRESULT OnAttributeUpImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeDownImage(const CString& strValue, BOOL bLoading);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);

protected:	
	void DrawRange(CDC &dc, CRect rcUpdate, int nXPos);
	virtual int  MoveRange(int nMove);
	virtual int  SetRange();

public:
	CControlBase*	m_pControlUpImage;		// �ϼ�ͷ�ؼ�
	CControlBase*	m_pControlDownImage;	// �¼�ͷ�ؼ�
	int				m_nDownTop;				// �����ʱ�����������λ��

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("upimg"), OnAttributeUpImage)
		DUI_CUSTOM_ATTRIBUTE(_T("downimg"), OnAttributeDownImage)
	DUI_DECLARE_ATTRIBUTES_END()
};

// ˮƽ���������
class CDuiScrollHorizontal : public CDuiScroll
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiScrollHorizontal, _T("scrollh"))
public:
	CDuiScrollHorizontal(HWND hWnd, CDuiObject* pDuiObject);
	CDuiScrollHorizontal(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible = TRUE);
	virtual ~CDuiScrollHorizontal(void);

	virtual BOOL SetBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	virtual BOOL SetBitmap(CString strImage);

	HRESULT OnAttributeLeftImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeRightImage(const CString& strValue, BOOL bLoading);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);

protected:	
	void DrawRange(CDC &dc, CRect rcUpdate, int nYPos);
	virtual int  MoveRange(int nMove);
	virtual int  SetRange();

public:
	CControlBase*	m_pControlLeftImage;	// ���ͷ�ؼ�
	CControlBase*	m_pControlRightImage;	// �Ҽ�ͷ�ؼ�
	int				m_nDownLeft;			// �����ʱ��ĺ�������λ��

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("leftimg"), OnAttributeLeftImage)
		DUI_CUSTOM_ATTRIBUTE(_T("rightimg"), OnAttributeRightImage)
	DUI_DECLARE_ATTRIBUTES_END()
};