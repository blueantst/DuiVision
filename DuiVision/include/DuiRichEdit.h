#ifndef __DUIRICHEDIT_H__
#define __DUIRICHEDIT_H__

class CTxtWinHost;

class CDuiRichEdit : public CDuiPanel
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiRichEdit, _T("richedit"))
public:
    CDuiRichEdit(HWND hWnd, CDuiObject* pDuiObject);
    ~CDuiRichEdit();

	static BOOL InitTextService();
	static void ReleaseTextService();

	// 设置左侧小图片
	bool SetLeftBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	bool SetLeftBitmap(CString strImage);
	HRESULT OnAttributeLeftImage(const CString& strValue, BOOL bLoading);

	// 设置右侧小图片
	bool SetSmallBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	bool SetSmallBitmap(CString strImage);
	HRESULT OnAttributeSmallImage(const CString& strValue, BOOL bLoading);

    BOOL IsWantReturn();
    void SetWantReturn(BOOL bWantReturn = true);
    BOOL IsWantCtrlReturn();
    void SetWantCtrlReturn(BOOL bWantCtrlReturn = true);
    BOOL IsRich();
    void SetRich(BOOL bRich = true);
    BOOL IsReadOnly();
    void SetReadOnly(BOOL bReadOnly = true);
    BOOL GetWordWrap();
    void SetWordWrap(BOOL bWordWrap = true);
	HFONT GetDefaultFont();
	void SetFont();
    void SetFont(LPCTSTR pStrFontName, int nSize, bool bBold, bool bUnderline, bool bItalic);
    LONG GetWinStyle();
    void SetWinStyle(LONG lStyle);
    Color GetTextColor();
    void SetTextColor(Color clrText);
    int GetLimitText();
    void SetLimitText(int iChars);
    long GetTextLength(DWORD dwFlags = GTL_DEFAULT) const;
    CString GetText() const;
    void SetText(LPCTSTR pstrText);
	bool SetFile(LPCTSTR pstrFile);
    bool GetModify() const;
    void SetModify(bool bModified = true) const;
    void GetSel(CHARRANGE &cr) const;
    void GetSel(long& nStartChar, long& nEndChar) const;
    int SetSel(CHARRANGE &cr);
    int SetSel(long nStartChar, long nEndChar);
    void ReplaceSel(LPCTSTR lpszNewText, bool bCanUndo);
    void ReplaceSelW(LPCWSTR lpszNewText, bool bCanUndo = false);
    CString GetSelText() const;
    int SetSelAll();
    int SetSelNone();
    WORD GetSelectionType() const;
    bool GetZoom(int& nNum, int& nDen) const;
    bool SetZoom(int nNum, int nDen);
    bool SetZoomOff();
    bool GetAutoURLDetect() const;
    bool SetAutoURLDetect(bool bAutoDetect = true);
    DWORD GetEventMask() const;
    DWORD SetEventMask(DWORD dwEventMask);
    CString GetTextRange(long nStartChar, long nEndChar) const;
    void HideSelection(bool bHide = true, bool bChangeStyle = false);
    void ScrollCaret();
    int InsertText(long nInsertAfterChar, LPCTSTR lpstrText, bool bCanUndo = false);
    int AppendText(LPCTSTR lpstrText, bool bCanUndo = false);
    DWORD GetDefaultCharFormat(CHARFORMAT2 &cf) const;
    bool SetDefaultCharFormat(CHARFORMAT2 &cf);
    DWORD GetSelectionCharFormat(CHARFORMAT2 &cf) const;
    bool SetSelectionCharFormat(CHARFORMAT2 &cf);
    bool SetWordCharFormat(CHARFORMAT2 &cf);
    DWORD GetParaFormat(PARAFORMAT2 &pf) const;
    bool SetParaFormat(PARAFORMAT2 &pf);
    bool Redo();
    bool Undo();
    void Clear();
    void Copy();
    void Cut();
    void Paste();
    int GetLineCount() const;
    CString GetLine(int nIndex, int nMaxLength) const;
    int LineIndex(int nLine = -1) const;
    int LineLength(int nLine = -1) const;
    bool LineScroll(int nLines, int nChars = 0);
	CPoint GetCharPos(long lChar) const;
    long LineFromChar(long nIndex) const;
    CPoint PosFromChar(UINT nChar) const;
    int CharFromPos(CPoint pt) const;
    void EmptyUndoBuffer();
    UINT SetUndoLimit(UINT nLimit);
    long StreamIn(int nFormat, EDITSTREAM &es);
    long StreamOut(int nFormat, EDITSTREAM &es);
	void SetAccumulateDBCMode(bool bDBCMode);
	bool IsAccumulateDBCMode();

    void DoInit();
    // 注意：TxSendMessage和SendMessage是有区别的，TxSendMessage没有multibyte和unicode自动转换的功能，
    // 而richedit2.0内部是以unicode实现的，在multibyte程序中，必须自己处理unicode到multibyte的转换
	bool SetDropAcceptFile(bool bAccept);
    virtual HRESULT TxSendMessage(UINT msg, WPARAM wparam, LPARAM lparam, LRESULT *plresult) const; 
    IDropTarget* GetTxDropTarget();
    virtual bool OnTxViewChanged();
    virtual void OnTxNotify(DWORD iNotify, void *pv);

    void SetScrollPos(SIZE szPos);
    void LineUp();
    void LineDown();
    void PageUp();
    void PageDown();
    void HomeUp();
    void EndDown();
    void LineLeft();
    void LineRight();
    void PageLeft();
    void PageRight();
    void HomeLeft();
    void EndRight();

    //SIZE EstimateSize(SIZE szAvailable);

    LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	virtual BOOL OnInit();
	virtual void SetControlRect(CRect rc);
	virtual BOOL SetControlFocus(BOOL bFocus);
	virtual BOOL IsDraw(CPoint point) ;
	virtual void SetControlWndVisible(BOOL bIsVisible);
	virtual void SetControlTitle(CString strTitle);

protected:
	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual	void SetControlDisable(BOOL bIsDisable);
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

protected:
	Image*			m_pLeftImage;		// 左侧小图片
	CSize			m_sizeLeftImage;	// 左侧小图片大小
	int				m_nLeftImageCount;	// 左侧小图片的个数

	Image*			m_pSmallImage;		// 右侧小图片
	CSize			m_sizeSmallImage;	// 右侧小图片大小
	BOOL			m_bIsSmallButton;	// 右侧小图片是否作为按钮来处理
	int				m_nSmallImageCount;	// 右侧小图片的个数

	bool			m_bDown;
	bool			m_bDownTemp;
	enumButtonState m_buttonState;
	enumButtonState m_EditState;
	CRect			m_rcText;			// 文字部分位置
	CString			m_strFile;			// 加载的文件名

    CTxtWinHost*	m_pTxtWinHost;		// richedit控件
    bool			m_bVScrollBarFixing;
	BOOL			m_bPassWord;		// 密码输入
	BOOL			m_bMultiLine;		// 多行
    BOOL			m_bWantReturn;		// 是否允许回车键换行
	BOOL			m_bWantCtrlReturn;	// 是否允许Ctrl+回车
	BOOL			m_bHScrollBar;		// 水平滚动条
	BOOL			m_bVScrollBar;		// 垂直滚动条
	BOOL			m_bAutoHScroll;		// 自动显示水平滚动条
	BOOL			m_bAutoVScroll;		// 自动显示垂直滚动条
	BOOL			m_bReadOnly;		// 只读
	BOOL			m_bNumber;			// 只能输入数字
    BOOL			m_bRich;			// 是否支持rtf格式字符串
    BOOL			m_bWordWrap;		// 是否单词自动换行
	Color			m_clrText;			// 文字颜色
	HFONT			m_hFont;			// 字体句柄
    int				m_iLimitText;		// 最大字符数
	int				m_nStartChar;		// 选择块的开始位置
	int				m_nEndChar;			// 选择块的结束位置
    LONG			m_lTwhStyle;		// richedit控件的窗口风格
	bool			m_bInited;			// richedit控件是否已经初始化
	bool			m_fAccumulateDBC;	// TRUE - need to cumulate ytes from 2 WM_CHAR msgs
	// we are in this mode when we receive VK_PROCESSKEY
	UINT			m_chLeadByte; // use when we are in _fAccumulateDBC mode

	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_BOOL_ATTRIBUTE(_T("password"), m_bPassWord, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("multiline"), m_bMultiLine, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("wantreturn"), m_bWantReturn, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("wantctrlreturn"), m_bWantCtrlReturn, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("hscrollbar"), m_bHScrollBar, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("vscrollbar"), m_bVScrollBar, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("autohscroll"), m_bAutoHScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("autovscroll"), m_bAutoVScroll, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("readonly"), m_bReadOnly, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("number"), m_bNumber, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("rich"), m_bRich, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("wordwrap"), m_bWordWrap, FALSE)
		DUI_TSTRING_ATTRIBUTE(_T("file"), m_strFile, FALSE)
		DUI_INT_ATTRIBUTE(_T("maxchar"), m_iLimitText, FALSE)
		DUI_INT_ATTRIBUTE(_T("startchar"), m_nStartChar, FALSE)
		DUI_INT_ATTRIBUTE(_T("endchar"), m_nEndChar, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("left-image"), OnAttributeLeftImage)
		DUI_CUSTOM_ATTRIBUTE(_T("small-image"), OnAttributeSmallImage)
	DUI_DECLARE_ATTRIBUTES_END()
};


#endif // __DUIRICHEDIT_H__
