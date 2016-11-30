// Panel控件，此控件是一个控件容器
#pragma once

#include "ControlBase.h"
#include "vcicomm.h"
#include "IDuiHostWnd.h"

using namespace  std;

interface IDuiPluginPanel;
class CDuiPanel : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiPanel, _T("div"))

public:
	CDuiPanel(HWND hWnd, CDuiObject* pDuiObject) ;
	virtual ~CDuiPanel(void);

	void	SetXmlFile(CString strXmlFile) {m_strXmlFile = strXmlFile;}

	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	BOOL LoadXmlFile(CString strFileName);
	HRESULT OnAttributeXml(const CString& strValue, BOOL bLoading);

	HRESULT OnAttributePlugin(const CString& strValue, BOOL bLoading);

	void SetVirtualHeight(int nHeight) { m_nVirtualHeight = nHeight; }
	void SetVirtualWidth(int nWidth) { m_nVirtualWidth = nWidth; }

	void SetEnableScroll(BOOL bEnableScroll) { m_bEnableScroll = bEnableScroll; }
	BOOL GetEnableScroll() { return m_bEnableScroll; }

	virtual void SetControlVisible(BOOL bIsVisible);
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlHide(BOOL bIsHide);
	virtual void SetControlDisable(BOOL bIsDisable);
	virtual BOOL SetControlFocus(BOOL bFocus);

	// 消息响应
	virtual LRESULT OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL);
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);

	// 根据控件名创建控件实例
	CControlBase* _CreateControlByName(LPCTSTR lpszName);
	void SetIDuiHostWnd(IDuiHostWnd* pIDuiHostWnd) { m_pIDuiHostWnd = pIDuiHostWnd; }
	IDuiHostWnd* GetIDuiHostWnd() { return m_pIDuiHostWnd; }

	HRESULT OnAttributeImageScrollV(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeImageScrollH(const CString& strValue, BOOL bLoading);

	virtual void SetControlRect(CRect rc);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	virtual BOOL DrawSubControls(CDC &dc, CRect rcUpdate);
	virtual void SetUpdate(BOOL bUpdate, COLORREF clr = 0);

	virtual BOOL OnMousePointChange(CPoint& point);
	virtual BOOL OnCheckMouseResponse(UINT nFlags, CPoint point);
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlRButtonDblClk(UINT nFlags, CPoint point);
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnControlKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	BOOL OnControlTimer();

	virtual void InitUI(CRect rcClient, DuiXmlNode pNode);

protected:
	// 导出的插件宿主窗口功能接口
    BEGIN_INTERFACE_PART(DuiPanel, IDuiHostWnd)
		// 平台操作
		STDMETHOD_( CString , GetAppName) ();				// 获取应用程序名
		STDMETHOD_( CString , GetPlatPath) ();				// 获取平台路径
		STDMETHOD_( CString , GetPlatVersion) ();			// 获取平台版本
		STDMETHOD_( int , GetCurrentLanguage) ();			// 获取当前语言
		STDMETHOD_( CString , GetPlatRegistry) ();			// 获取平台注册表根路径
		STDMETHOD_( CString , GetPlatCopyRight) ();			// 获取平台版权字符串
		STDMETHOD_( CString , GetPlatHomeURL) ();			// 获取主页URL
		STDMETHOD_( CString , GetPlatDownloadURL) ();		// 获取下载URL
		STDMETHOD_( int  , SendMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// 发送消息
		STDMETHOD_( int  , ProcessMessage) (CVciMessage* pIn, CVciMessage* ppOut);	// 平台的消息处理
		STDMETHOD_( int  , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam);	// 发送平台命令
		STDMETHOD_( BOOL , SendCommand) (int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult);	// 发送平台命令

		// DuiVision系统功能
		STDMETHOD_( int  , GetAppID) ();	// 获取DuiVision应用ID

		// 窗口操作
		STDMETHOD_( BOOL , GetWindowBkInfo) (int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile);	// 获取窗口背景信息
		STDMETHOD_( BOOL , SetWindowBkInfo) (int nType, int nIDResource, COLORREF clr, LPCTSTR lpszImgFile);	// 设置窗口背景信息
		STDMETHOD_( void  , ClientToScreen) (LPPOINT lpPoint);	// 坐标转换为屏幕坐标
		STDMETHOD_( HWND  , GetPaintHWnd) ();	// 获取宿主窗口的句柄

		// Tooltip操作
		STDMETHOD_( void  , SetTooltip) (int nCtrlID, LPCTSTR lpszTooltip, CRect rect, int nTipWidth);	// 设置Tooltip
		STDMETHOD_( void , ClearTooltip) ();			// 清除Tooltip
		STDMETHOD_( void , SetTooltipCtrlID) (int nTooltipCtrlID);	// 设置当前Tooltip控件ID
		STDMETHOD_( int , GetTooltipCtrlID) ();		// 获取当前Tooltip控件ID
	END_INTERFACE_PART(DuiPanel)
	EXPORT_INTERFACE_PART(DuiPanel)

public:
	ULONG			m_ulRefCount;			// 内嵌接口类使用的引用计数,vcicomm使用
	BOOL				m_bInit;					// 是否初始化完成
	CString				m_strXmlFile;				// XML文件名
	int					m_nVirtualHeight;			// Panel整体的高度
	int					m_nVirtualWidth;			// Panel整体的宽度
	CControlBaseFont*	m_pControScrollV;			// 垂直滚动条
	CControlBaseFont*	m_pControScrollH;			// 水平滚动条
	int					m_nScrollWidth;				// 滚动条宽度
	int					m_nVirtualTop;				// 当前虚拟显示的顶部位置
	int					m_nVirtualLeft;				// 当前虚拟显示的顶部位置
	BOOL				m_bEnableScroll;			// 是否允许滚动

	HINSTANCE			m_hPluginHandle;			// 保存界面插件动态库的句柄
	CString				m_strPluginFile;			// 界面插件文件名
	IDuiPluginPanel*	m_pDuiPluginObject;			// 界面插件对象
	IDuiHostWnd*	m_pIDuiHostWnd;		// 插件宿主窗口的功能接口

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_CUSTOM_ATTRIBUTE("img-scroll", OnAttributeImageScrollV)
		DUI_CUSTOM_ATTRIBUTE("img-scrollh", OnAttributeImageScrollH)
		DUI_INT_ATTRIBUTE("scroll-width", m_nScrollWidth, FALSE)
		DUI_CUSTOM_ATTRIBUTE("xml", OnAttributeXml)
#ifdef _DEBUG
		DUI_CUSTOM_ATTRIBUTE("plugin-debug", OnAttributePlugin)
#else
		DUI_CUSTOM_ATTRIBUTE("plugin", OnAttributePlugin)
#endif
		DUI_INT_ATTRIBUTE("scroll", m_bEnableScroll, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
