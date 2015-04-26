#pragma once

#include "ControlBase.h"

class CScrollV : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CScrollV, _T("scroll"))
public:
	CScrollV(HWND hWnd, CDuiObject* pDuiObject);
	CScrollV(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible = TRUE);
	virtual ~CScrollV(void);

	virtual BOOL SetBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	virtual BOOL SetBitmap(CString strImage);

	BOOL SetScrollCurrentPos(int nCurrentPos);
	int GetScrollCurrentPos() { return m_nCurrentPos; }
	BOOL SetScrollMaxRange(int nMaxRange);
	int GetScrollMaxRange() { return m_nMaxRange; }
	BOOL SetScrollSize(int nPageRange, int nRowRange = 0);
	BOOL SetScrollInfo(int nMaxRange, int nCurrentPos, int nPageRange = 0, int nRowRange = 0);
	BOOL ScrollRow(int nRow);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void SetControlRect(CRect rc);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	HRESULT OnAttributeUpImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeDownImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeMaxRange(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeCurrentPos(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributePageRange(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeRowRange(const CString& strValue, BOOL bLoading);

protected:	
	void DrawRange(CDC &dc, CRect rcUpdate, int nXPos);
	int MoveRange(int nMove);
	int SetRange();

	// 消息响应
	virtual LRESULT OnMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam);

public:	
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
	int				m_nDownTop;				// 鼠标点击时候的纵向坐标位置

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE(_T("maxrange"), OnAttributeMaxRange)
		DUI_CUSTOM_ATTRIBUTE(_T("curpos"), OnAttributeCurrentPos)
		DUI_CUSTOM_ATTRIBUTE(_T("pagerange"), OnAttributePageRange)
		DUI_CUSTOM_ATTRIBUTE(_T("rowrange"), OnAttributeRowRange)
		DUI_CUSTOM_ATTRIBUTE(_T("upimg"), OnAttributeUpImage)
		DUI_CUSTOM_ATTRIBUTE(_T("downimg"), OnAttributeDownImage)
	DUI_DECLARE_ATTRIBUTES_END()
};
