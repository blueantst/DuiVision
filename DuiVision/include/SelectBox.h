// 表格控件
#pragma once
#include "ControlBase.h"
#include <vector>

using namespace  std;

// 选择项的信息
struct SelectInfo
{
	int nType;
	Color clr;
	UINT uIndex;
};

class CSelectBox : public CControlBase
{
	DUIOBJ_DECLARE_CLASS_NAME(CSelectBox, _T("selectbox"))
public:
	CSelectBox(HWND hWnd, CDuiObject* pDuiObject);
	CSelectBox(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nXCount, int nYCount, BOOL bImage,  
		Color clrFrame = Color(254, 0, 0, 0), Color clrHover = Color(128, 0, 0, 0), Color clrSelect = Color(254, 255, 255, 255), BOOL bIsVisible = TRUE);
	virtual ~CSelectBox(void);

	BOOL SetBitmap(UINT nResourceID, int nIndex = -1, CString strType= TEXT("PNG"));
	BOOL SetBitmap(CString strImage, int nIndex = -1);
	BOOL SetColor(Color clr, int nIndex = -1);
	BOOL SetColor(Color clr[], int nColorCount);

	int  GetSelectIndex() { return m_nSelectIndex; }
	Color GetSelectColor() { return m_clrSelect; }

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);

protected:
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);

public:
	BOOL			m_bImage;		// 是否图片类型
	vector<Image*>	m_vecpImage;	// 图片列表
	vector<CSize>	m_vecsizeImage;	// 图片大小列表
	vector<Color>	m_vecclr;		// 颜色列表
	int				m_nXCount;		// 行数
	int				m_nYCount;		// 列数
	Color			m_clrFrame;		// 边框颜色
	Color			m_clrHover;		// 热点颜色
	Color			m_clrSelect;	// 选择项颜色

	int				m_nXHover;		// 热点列
	int				m_nYHover;		// 热点行
	int				m_nXSelect;		// 选择列
	int				m_nYSelect;		// 选择行
	int				m_nSelectIndex;	// 当前选择的项的索引
	Color			m_clrCurSelect;	// 当前选择的颜色

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE("image", m_bImage, FALSE)
		DUI_COLOR_ATTRIBUTE("crframe", m_clrFrame, FALSE)
		DUI_COLOR_ATTRIBUTE("crhover", m_clrHover, FALSE)
		DUI_COLOR_ATTRIBUTE("crselect", m_clrSelect, FALSE)
		DUI_INT_ATTRIBUTE("col", m_nXCount, FALSE)
		DUI_INT_ATTRIBUTE("row", m_nYCount, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
