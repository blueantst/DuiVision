#pragma once

#include "DuiObject.h"

#include <vector>

using namespace  std;

class CDlgBase;
class CDlgPopup;
interface IDuiHostWnd;

// ��ť״̬
enum enumButtonState
{
	enBSNormal = 0,
	enBSHover,
	enBSDown,
	enBSDisable,
	enBSHoverDown,	
	enBSDisableDown
};

// λ�õļ��㷽ʽ,��ͨ��������Ϊ0���㡢���հٷֱȼ���
enum PIT{PIT_NORMAL=0,PIT_CENTER,PIT_PERCENT};

// λ����Ϣ��ÿ�������Ϣ
struct DUI_POSITION_ITEM
{
	PIT pit;			// λ�ü��㷽ʽ	
    BOOL bMinus;		// �Ƿ�ֵ
    float  nPos;		// λ����Ϣ
};

// λ����Ϣ�ṹ
struct DUI_POSITION
{
    int nCount;
    union
    {
        struct
        {
            DUI_POSITION_ITEM Left;
            DUI_POSITION_ITEM Top;
            DUI_POSITION_ITEM Right;
            DUI_POSITION_ITEM Bottom;
        };
        DUI_POSITION_ITEM Item[4];
    };
};

// �˵�ID����
#define	WM_DUI_MENU		(WM_USER + 20)	

class CControlBase : public CDuiObject
{
public:
	CControlBase(HWND hWnd, CDuiObject* pDuiObject) ;
	CControlBase(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE , BOOL bRresponse = TRUE) ;
	virtual ~CControlBase(void);

	// ����;��
	virtual CDuiObject* GetParent() { return m_pParentDuiObject; }
	void SetParent(CDuiObject* pParent) { m_pParentDuiObject = pParent; }
	HWND GetHWND() { return m_hWnd; }
	HWND GetNativeHWnd() { return m_hwndHost; }
	HWND GetPaintHWnd();

	void TestMainThread();
	void Draw(CDC &dc, CRect rcUpdate);
	virtual void DrawControl(CDC &dc, CRect rcUpdate) = 0;
	virtual BOOL DrawSubControls(CDC &dc, CRect rcUpdate);
	virtual BOOL IsDraw(CPoint point) { return FALSE; }
	virtual void SetUpdate(BOOL bUpdate, COLORREF clr = 0);
	BOOL GetUpdate() { return m_bUpdate; };
	void UpdateMemDC(CDC &dc, int nWidth, int nHeight);
	void UpdateAnimateDC(CDC &dc, int nWidth, int nHeight);
	void ClientToScreen(LPPOINT lpPoint);

	// λ����Ϣ
	virtual void SetRect(CRect rc);
	virtual void SetControlRect(CRect rc) { m_rc = rc; };
	virtual CRect GetRect() { return m_rc;};
	virtual void OnPositionChange();	// λ��ˢ��
	virtual void InvalidateRect(LPCRECT lpRect, BOOL bErase = TRUE);

	void SetPosStr(CString strPos) { m_strPos = strPos; };
	CString GetPosStr() { return m_strPos;};
	int PositionItem2Value( const DUI_POSITION_ITEM &pos ,int nMin, int nMax);
	LPCTSTR ParsePosition(LPCTSTR pszPos,DUI_POSITION_ITEM &pos);
	BOOL SetPositionWithParent(CRect rectParent);
	HRESULT OnAttributePosChange(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeWidth(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeHeight(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeMenuPosChange(const CString& strValue, BOOL bLoading);

	HRESULT OnAttributeShortcut(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeDisable(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeCursor(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeSendDuiMsg(const CString& strValue, BOOL bLoading);
	
	// ����
	void SetVisible(BOOL bIsVisible);
	virtual void SetControlVisible(BOOL bIsVisible) { m_bIsVisible = bIsVisible; }
	BOOL GetVisible() { return IsControlVisible(); }
	virtual BOOL IsControlVisible();
	void SetHide(BOOL bIsHide);
	virtual void SetControlHide(BOOL bIsHide) { m_bIsHide = bIsHide; }
	virtual void SetControlWndVisible(BOOL bIsVisible) { };	// ���ÿؼ��е�Windowsԭ���ؼ��Ƿ�ɼ���״̬
	void SetDisable(BOOL bIsDisable);
	virtual	void SetControlDisable(BOOL bIsDisable) { m_bIsDisable = bIsDisable; }
	BOOL GetDisable() { return m_bIsDisable; }
	void SetRresponse(BOOL bRresponse) { m_bRresponse = bRresponse; }
	BOOL GetRresponse() { return m_bRresponse; }
	void SetTabStop(BOOL bTabStop) { m_bTabStop = bTabStop; }
	BOOL IsTabStop() { return m_bTabStop; }
	void SetTooltip(CString strTooltip) { m_strTooltip = strTooltip; }
	CString GetTooltip() { return m_strTooltip; }
	void SetTipWidth(int nTipWidth) { m_nTipWidth = nTipWidth; }
	void SetAction(CString strAction) { m_strAction = strAction; }
	CString GetAction() { return m_strAction; }
	BOOL UseImageECM() { return m_bImageUseECM; }
	void SetDragEnable(BOOL bDragEnable) { m_bDragEnable = bDragEnable; }
	BOOL GetDragEnable() { return m_bDragEnable; }
	void SetDropFileEnable(BOOL bDropFileEnable) { m_bDropFileEnable = bDropFileEnable; }
	BOOL GetDropFileEnable() { return m_bDropFileEnable; }

	virtual	BOOL PtInRect(CPoint point);	// �ж������Ƿ��ڿؼ���Χ��
	UINT GetControlID() { return m_uID; }	// �ؼ�ID����DUI�����ID
	void UpdateControl(BOOL bUpdate = false, BOOL bVisible = false);
	void UpdateControl(CRect rc, BOOL bVisible = false, BOOL bUpdate = false);
	BOOL GetDblClk() { return m_bDblClk; }

	// ����
	virtual	BOOL SetWindowFocus();				// ���ô��ڽ���
	BOOL OnFocus(BOOL bFocus);
	virtual	BOOL SetControlFocus(BOOL bFocus);		// ���ÿؼ��Ľ���
	BOOL IsFocusControl();
	CControlBase* GetFocusControl(CControlBase* pFocusControl);
	CControlBase* GetPrevFocusableControl(CControlBase* pFocusControl);
	CControlBase* GetNextFocusableControl(CControlBase* pFocusControl);

	// Tooltip
	void SetTooltip(CControlBase* pControl, CString strTooltip, CRect rect, BOOL bControlWidth, int nTipWidth = 0);
	void ClearTooltip();
	void SetTooltipCtrlID(int nTooltipCtrlID);
	int GetTooltipCtrlID();

	// ��������Ϣ
	BOOL OnMouseMove(UINT nFlags, CPoint point);
	BOOL OnLButtonDown(UINT nFlags, CPoint point);
	BOOL OnLButtonUp(UINT nFlags, CPoint point);
	BOOL OnLButtonDblClk(UINT nFlags, CPoint point);
	BOOL OnScroll(BOOL bVertical, UINT nFlags, CPoint point);
	BOOL OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	BOOL OnRButtonDown(UINT nFlags, CPoint point);
	BOOL OnRButtonUp(UINT nFlags, CPoint point);
	BOOL OnRButtonDblClk(UINT nFlags, CPoint point);

	BOOL OnTimer();

	virtual BOOL OnMousePointChange(CPoint& point) { return FALSE; }
	virtual BOOL OnCheckMouseResponse(UINT nFlags, CPoint point) { return TRUE; }	// �ж�����Ƿ��ڿؼ�����Ӧ������
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlSetCursor(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlLButtonDblClk(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL OnControlSetDuiMsg(LPCTSTR lpszDuiMsg);
	virtual BOOL OnControlRButtonDown(UINT nFlags, CPoint point){ return FALSE; };
	virtual BOOL OnControlRButtonUp(UINT nFlags, CPoint point){ return FALSE; };
	virtual BOOL OnControlRButtonDblClk(UINT nFlags, CPoint point){ return FALSE; }
	virtual BOOL OnControlDropFile(CPoint point, CString strFileName);

	virtual	BOOL OnControlTimer() { return FALSE; }

	// �ؼ����ӿؼ�
	CControlBase *GetControl(UINT uControlID);
	CControlBase *GetControl(CString strControlName);
	void AddControl(CControlBase* pControl);
	BOOL RemoveControl(CControlBase* pControl);
	BOOL RemoveControl(CString strControlName, UINT uControlID);
	void RemoveControls(CString strClassName);
	void RemoveControls();
	CDlgBase* GetParentDialog(BOOL bEnablePopup = TRUE);
	IDuiHostWnd* GetParentIDuiHostWnd();
	vector<CControlBase*>* GetControls() { return &m_vecControl; }

	// ��Ϣ����
	virtual LRESULT OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT CallDuiHandler(UINT uMsg, WPARAM wParam, LPARAM lParam);

	// ����֪ͨ��Ϣ
	LRESULT SendMessage(UINT Msg, WPARAM wParam, LPARAM lParam);

	// �򿪵����Ի���
	void OpenDlgPopup(CDlgPopup *pWndPopup, CRect rc, UINT uMessageID, BOOL bShow=TRUE);
	// �رյ����Ի���
	void CloseDlgPopup();

protected:
	CDuiObject*				m_pParentDuiObject;	// ���ؼ�����
	HWND					m_hWnd;				// �����ڵ�Windows���ھ��(һ���ǶԻ���Ĵ��ھ��)
	HWND					m_hwndHost;			// ԭ�����ھ��(���ڱ���ԭ���ؼ�����������)
	
	CString					m_strPos;			// λ��(XML�����е��ַ���)
	BOOL					m_bIsVisible;		// �Ƿ�ɼ�
	BOOL					m_bIsHide;			// �Ƿ�������״̬(tabҳ�л�ʱ����Զ������ӿؼ�)
	BOOL					m_bIsDisable;		// �Ƿ����
	BOOL					m_bRresponse;		// �Ƿ������Ӧ����¼�
	BOOL					m_bTabStop;			// �Ƿ������ӦTab��
	BOOL					m_bIsSelect;		// �Ƿ�����ѡ��
	BOOL					m_bDblClk;			// �Ƿ������Ӧ˫���¼�
	BOOL					m_bMouseDown;		// �Ƿ���갴��
	BOOL					m_bDragEnable;		// �Ƿ���������϶��ؼ�
	BOOL					m_bDropFileEnable;	// �Ƿ����������ק�ļ����˿ؼ�

	UINT					m_nShortcutKey;		// ��ݼ�����
	UINT					m_nShortcutFlag;	// ��ݼ�ɨ����

	BOOL					m_bUpdate;			// ������ͼ

	BOOL					m_bIsRun;			// �Ƿ���ִ�й���
	BOOL					m_bRunTime;			// �Ƿ�����ִ�ж�ʱ��

	BOOL					m_bImageUseECM;		// �Ƿ�ʹ��ͼƬ�������ɫ������Ϣ

	vector<CControlBase *>	m_vecControl;		// �ؼ��б�
	vector<CControlBase *>	m_vecArea;			// �����б�(��Ӱ������¼�)
	CControlBase*			m_pControl;			// ��ǰ�ؼ�

	// �ڴ�DC
	int						m_nDCWidth;			// �ڴ�DC���
	int						m_nDCHeight;		// �ڴ�DC�߶�
	CBitmap					m_bitmap;			// �ڴ�DC��λͼ����
	CDC						m_memDC;			// �ڴ�DC
	CBitmap*				m_pOldBitmap;		// �����ڴ�DC�ɵ�λͼ����

	// ����DC
	int						m_nAniDCWidth;		// ����DC���
	int						m_nAniDCHeight;		// ����DC�߶�
	CBitmap					m_aniBitmap;		// ����DC��λͼ����
	CDC						m_animateDC;		// ����DC
	CBitmap*				m_pOldAniBitmap;	// ���涯��DC�ɵ�λͼ����

	int						m_nWidth;			// �ؼ����
	int						m_nHeight;			// �ؼ��߶�

	DUI_POSITION			m_posMenu;			// �����˵���λ����Ϣ

	CString					m_strTooltip;		// �ؼ���Tooltip
	int						m_nTipWidth;		// Tooltip�Ŀ��
	CString					m_strAction;		// �ؼ�ִ�еĶ���
	BOOL					m_bTaskMsg;			// �����¼���Ӧ����ʱ���������ʽ

	CDlgPopup*				m_pWndPopup;		// ����ĵ�����ָ��

	HCURSOR					m_hCursor;			// �ؼ��������
	
	CPoint					m_ptLastMousePosition;	// �ؼ���һ�ε�����ƶ�λ��

	BOOL					m_bDuiMsgMouseMove;	// �Ƿ�������ƶ�DUI��Ϣ
	BOOL					m_bDuiMsgMouseLDown;// �Ƿ�������������DUI��Ϣ
	BOOL					m_bDuiMsgMouseLUp;	// �Ƿ����������ſ�DUI��Ϣ
	BOOL					m_bDuiMsgMouseLDblClk;// �Ƿ���������˫��DUI��Ϣ
	BOOL					m_bDuiMsgMouseRDown;// �Ƿ�������Ҽ�����DUI��Ϣ
	BOOL					m_bDuiMsgMouseRUp;	// �Ƿ�������Ҽ��ſ�DUI��Ϣ
	BOOL					m_bDuiMsgMouseRDblClk;// �Ƿ�������Ҽ�˫��DUI��Ϣ
	BOOL					m_bDuiMsgKeyDown;	// �Ƿ��ͼ��̰���DUI��Ϣ
	BOOL					m_bMouseLeave;		// ����Ƿ��Ѿ��뿪�ؼ�

	DUI_DECLARE_ATTRIBUTES_BEGIN()
        //DUI_STYLE_ATTRIBUTE("class", m_style, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("show"), m_bIsVisible, TRUE)
		DUI_CUSTOM_ATTRIBUTE(_T("disable"), OnAttributeDisable)
		DUI_BOOL_ATTRIBUTE(_T("response"), m_bRresponse, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("tabstop"), m_bTabStop, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("drag"), m_bDragEnable, TRUE)
		DUI_BOOL_ATTRIBUTE(_T("dropfile"), m_bDropFileEnable, TRUE)
		DUI_CUSTOM_ATTRIBUTE(_T("pos"), OnAttributePosChange)
		DUI_CUSTOM_ATTRIBUTE(_T("width"), OnAttributeWidth)
		DUI_CUSTOM_ATTRIBUTE(_T("height"), OnAttributeHeight)
		DUI_CUSTOM_ATTRIBUTE(_T("menupos"), OnAttributeMenuPosChange)
		DUI_TSTRING_ATTRIBUTE(_T("action"), m_strAction, FALSE)
		DUI_INT_ATTRIBUTE(_T("taskmsg"), m_bTaskMsg, TRUE)
		DUI_INT_ATTRIBUTE(_T("img-ecm"), m_bImageUseECM, TRUE)
		DUI_TSTRING_ATTRIBUTE(_T("tip"), m_strTooltip, FALSE)
		DUI_INT_ATTRIBUTE(_T("tip-width"), m_nTipWidth, TRUE)
		DUI_CUSTOM_ATTRIBUTE(_T("shortcut"), OnAttributeShortcut)
		DUI_CUSTOM_ATTRIBUTE(_T("cursor"), OnAttributeCursor)
		DUI_CUSTOM_ATTRIBUTE(_T("duimsg"), OnAttributeSendDuiMsg)
    DUI_DECLARE_ATTRIBUTES_END()
};

// DUI������Ϣ�ṹ
struct DuiFontInfo
{
	CString strName;	// ���������
	CString strFont;	// ������
	int nFontWidth;		// �����С
	FontStyle fontStyle;// ������
	CString strOS;		// ����ϵͳ
};

// λ�úͶ��뷽ʽ
enum {
    // Specify by "width" attribute
    SizeX_Mask          = 0x0007UL, 
    SizeX_Specify       = 0x0001UL, // width > 0
    SizeX_FitContent    = 0x0002UL, // width <= 0
    SizeX_FitParent     = 0x0004UL, // width = "full" default

    // Specify by "height" attribute
    SizeY_Mask          = 0x0070UL, 
    SizeY_Specify       = 0x0010UL, // height > 0
    SizeY_FitContent    = 0x0020UL, // height <= 0 default
    SizeY_FitParent     = 0x0040UL, // height = "full" default
    // Specify by "float" attribute

    Position_Mask       = 0x0300UL, 
    Position_Relative   = 0x0100UL, // float = 0 default
    Position_Float      = 0x0200UL, // float = 1

    // Specify by "valign" and "align" attribute, only using at float = 1 or panel control (Vert-Align)
    Align_Mask          = 0xF000UL, 
    VAlign_Top          = 0x0000UL, // valign = top
    VAlign_Middle       = 0x1000UL, // valign = middle
    VAlign_Bottom       = 0x2000UL, // valign = bottom
    Align_Left          = 0x0000UL, // align = left
    Align_Center        = 0x4000UL, // align = center
    Align_Right         = 0x8000UL, // align = right
};

// �������ֵĿؼ�����
class CControlBaseFont : public CControlBase
{
public:
	CControlBaseFont(HWND hWnd, CDuiObject* pDuiObject);
	CControlBaseFont(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle, BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE , BOOL bRresponse = TRUE,
		CString strFont = _T(""), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular) ;
	virtual ~CControlBaseFont(void);

	void SetTitle(CString strTitle);
	virtual void SetControlTitle(CString strTitle) { m_strTitle = strTitle; }
	virtual CString GetTitle() { return m_strTitle; }

	// ���뷽ʽ
	virtual void SetAlignment(UINT uAlignment);
	virtual void SetVAlignment(UINT uVAlignment);
	virtual void SetAlignment(UINT uAlignment, UINT uVAlignment);

	UINT GetGDIAlignment(UINT uAlignment);
	UINT GetGDIVAlignment(UINT uVAlignment);

	// �����ͼƬ����
	virtual void SetFont(CString strFont = _T(""), int nFontWidth = 12, FontStyle fontStyle = FontStyleRegular);
	virtual BOOL SetBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	virtual BOOL SetBitmap(CString strImage);
	void SetBitmapCount(int nCount);
	BOOL SetImage(CString strImageA);
	HRESULT OnAttributeFont(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeImage(const CString& strValue, BOOL bLoading);
	HRESULT OnAttributeSkin(const CString& strValue, BOOL bLoading);

protected:
	CString					m_strTitle;			// ����
	CString					m_strFont;			// ����
	int						m_nFontWidth;		// ������
	FontStyle				m_fontStyle;		// ����Style
	UINT					m_uAlignment;		// ˮƽ���뷽ʽ
	UINT					m_uVAlignment;		// ��ֱ���뷽ʽ

	Image*					m_pImage;			// ͼƬ
	CSize					m_sizeImage;		// ͼƬ��С
	int						m_nImagePicCount;	// Image��Ĭ�ϰ�����ͼƬ����

	DUI_DECLARE_ATTRIBUTES_BEGIN()
        DUI_TSTRING_ATTRIBUTE(_T("title"), m_strTitle, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("font"), OnAttributeFont)
		DUI_TSTRING_ATTRIBUTE(_T("fontname"), m_strFont, FALSE)
		DUI_INT_ATTRIBUTE(_T("fontwidth"), m_nFontWidth, FALSE)
		DUI_ENUM_ATTRIBUTE(_T("valign"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("top"), VAlign_Top)
            DUI_ENUM_VALUE(_T("middle"), VAlign_Middle)
            DUI_ENUM_VALUE(_T("bottom"), VAlign_Bottom)
        DUI_ENUM_END(m_uVAlignment)
        DUI_ENUM_ATTRIBUTE(_T("align"), UINT, TRUE)
            DUI_ENUM_VALUE(_T("left"), Align_Left)
            DUI_ENUM_VALUE(_T("center"), Align_Center)
            DUI_ENUM_VALUE(_T("right"), Align_Right)
        DUI_ENUM_END(m_uAlignment)
		DUI_CUSTOM_ATTRIBUTE(_T("image"), OnAttributeImage)
		DUI_CUSTOM_ATTRIBUTE(_T("skin"), OnAttributeSkin)
		DUI_INT_ATTRIBUTE(_T("img-count"), m_nImagePicCount, FALSE)
    DUI_DECLARE_ATTRIBUTES_END()
};
