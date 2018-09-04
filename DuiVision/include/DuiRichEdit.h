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

	// �������СͼƬ
	bool SetLeftBitmap(UINT nResourceID, CString strType = TEXT("PNG"));
	bool SetLeftBitmap(CString strImage);
	HRESULT OnAttributeLeftImage(const CString& strValue, BOOL bLoading);

	// �����Ҳ�СͼƬ
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
    // ע�⣺TxSendMessage��SendMessage��������ģ�TxSendMessageû��multibyte��unicode�Զ�ת���Ĺ��ܣ�
    // ��richedit2.0�ڲ�����unicodeʵ�ֵģ���multibyte�����У������Լ�����unicode��multibyte��ת��
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
	Image*			m_pLeftImage;		// ���СͼƬ
	CSize			m_sizeLeftImage;	// ���СͼƬ��С
	int				m_nLeftImageCount;	// ���СͼƬ�ĸ���

	Image*			m_pSmallImage;		// �Ҳ�СͼƬ
	CSize			m_sizeSmallImage;	// �Ҳ�СͼƬ��С
	BOOL			m_bIsSmallButton;	// �Ҳ�СͼƬ�Ƿ���Ϊ��ť������
	int				m_nSmallImageCount;	// �Ҳ�СͼƬ�ĸ���

	bool			m_bDown;
	bool			m_bDownTemp;
	enumButtonState m_buttonState;
	enumButtonState m_EditState;
	CRect			m_rcText;			// ���ֲ���λ��
	CString			m_strFile;			// ���ص��ļ���

    CTxtWinHost*	m_pTxtWinHost;		// richedit�ؼ�
    bool			m_bVScrollBarFixing;
	BOOL			m_bPassWord;		// ��������
	BOOL			m_bMultiLine;		// ����
    BOOL			m_bWantReturn;		// �Ƿ�����س�������
	BOOL			m_bWantCtrlReturn;	// �Ƿ�����Ctrl+�س�
	BOOL			m_bHScrollBar;		// ˮƽ������
	BOOL			m_bVScrollBar;		// ��ֱ������
	BOOL			m_bAutoHScroll;		// �Զ���ʾˮƽ������
	BOOL			m_bAutoVScroll;		// �Զ���ʾ��ֱ������
	BOOL			m_bReadOnly;		// ֻ��
	BOOL			m_bNumber;			// ֻ����������
    BOOL			m_bRich;			// �Ƿ�֧��rtf��ʽ�ַ���
    BOOL			m_bWordWrap;		// �Ƿ񵥴��Զ�����
	Color			m_clrText;			// ������ɫ
	HFONT			m_hFont;			// ������
    int				m_iLimitText;		// ����ַ���
	int				m_nStartChar;		// ѡ���Ŀ�ʼλ��
	int				m_nEndChar;			// ѡ���Ľ���λ��
    LONG			m_lTwhStyle;		// richedit�ؼ��Ĵ��ڷ��
	bool			m_bInited;			// richedit�ؼ��Ƿ��Ѿ���ʼ��
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
