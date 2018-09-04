#pragma once

#include "ControlBase.h"

// 滚动条模式
enum SCROLL_TYPE
{
	SCROLL_TYPE_VERTICAL = 0,			// 垂直方向
	SCROLL_TYPE_HORIZONTAL,				// 水平方向
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

	// 消息响应
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:
	SCROLL_TYPE		m_nScrollType;			// 滚动条类型
	CBitmap			m_bitmap;
	enumButtonState m_enButtonState;		// 鼠标状态
	BOOL			m_bHover;				// 是否鼠标热点状态
	BOOL			m_bShowScroll;			// 是否显示滚动条

	CControlBase*	m_pControlUpImage;		// 上箭头控件
	CControlBase*	m_pControlDownImage;	// 下箭头控件

	int				m_nArrowLen;			// 箭头的长度

	int				m_nMaxRange;			// 最大值
	int				m_nCurrentPos;			// 当前位置
	int				m_nPageRange;			// 一页翻多少
	int				m_nRowRange;			// 一行翻多少
	CRect			m_rcBlock;				// 滚动块大小
	BOOL			m_bAutoCalcRange;		// 是否自动计算的翻页范围

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("maxrange"), OnAttributeMaxRange)
		DUI_CUSTOM_ATTRIBUTE(_T("curpos"), OnAttributeCurrentPos)
		DUI_CUSTOM_ATTRIBUTE(_T("pagerange"), OnAttributePageRange)
		DUI_CUSTOM_ATTRIBUTE(_T("rowrange"), OnAttributeRowRange)
	DUI_DECLARE_ATTRIBUTES_END()
};

// 垂直方向滚动条
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
	CControlBase*	m_pControlUpImage;		// 上箭头控件
	CControlBase*	m_pControlDownImage;	// 下箭头控件
	int				m_nDownTop;				// 鼠标点击时候的纵向坐标位置

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("upimg"), OnAttributeUpImage)
		DUI_CUSTOM_ATTRIBUTE(_T("downimg"), OnAttributeDownImage)
	DUI_DECLARE_ATTRIBUTES_END()
};

// 水平方向滚动条
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
	CControlBase*	m_pControlLeftImage;	// 左箭头控件
	CControlBase*	m_pControlRightImage;	// 右箭头控件
	int				m_nDownLeft;			// 鼠标点击时候的横向坐标位置

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("leftimg"), OnAttributeLeftImage)
		DUI_CUSTOM_ATTRIBUTE(_T("rightimg"), OnAttributeRightImage)
	DUI_DECLARE_ATTRIBUTES_END()
};