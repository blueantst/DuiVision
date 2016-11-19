#include "StdAfx.h"
#include "DuiEdit.h"

// 支持修改背景色和文字颜色的编辑控件
class CColorEdit : public CEdit
{
private:
	COLORREF m_clrBack;	// 背景色
	COLORREF m_clrText;	// 文字颜色
	CBrush	m_brBack;	// 背景色刷
	BOOL	m_bTransparent;	// 背景是否透明
	
protected:
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	DECLARE_MESSAGE_MAP()

public:
	CColorEdit(COLORREF backColor = RGB(255, 255, 255), COLORREF textColor = RGB(255, 255, 255), BOOL bTransparent = FALSE);
	virtual ~CColorEdit();
	void SetBackColor(COLORREF color);
	COLORREF GetBackColor();
	void SetTransparent(BOOL bTransparent);
	void SetTextColor(COLORREF color);
	COLORREF GetTextColor();
};

CColorEdit::CColorEdit(COLORREF backColor, COLORREF textColor, BOOL bTransparent)
{
	m_clrBack = backColor;
	m_brBack.CreateSolidBrush(m_clrBack);
	m_clrText = textColor;
	m_bTransparent = bTransparent;
	EnableToolTips(TRUE);
}

CColorEdit::~CColorEdit()
{
}

BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

HBRUSH CColorEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	if(m_bTransparent)
	{
		pDC-> SetBkMode(TRANSPARENT); //设置字体背景为透明
	}else
	{
		pDC->SetBkColor(m_clrBack);	// 设置背景色,不支持设置透明度
	}
	// 设置文字颜色
	pDC->SetTextColor(m_clrText);
	return m_brBack;
}

void CColorEdit::SetBackColor(COLORREF color)
{
	m_clrBack = color;
	m_brBack.CreateSolidBrush(color);
}

COLORREF CColorEdit::GetBackColor()
{
	return m_clrBack;
}

void CColorEdit::SetTransparent(BOOL bTransparent)
{
	m_bTransparent = bTransparent;
}

void CColorEdit::SetTextColor(COLORREF color)
{
	m_clrText = color;
}

COLORREF CColorEdit::GetTextColor()
{
	return m_clrText;
}

// CDuiEdit
CDuiEdit::CDuiEdit(HWND hWnd, CDuiObject* pDuiObject)
		: CControlBaseFont(hWnd, pDuiObject)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_bDown = false;
	m_bDownTemp = false;
	m_buttonState = enBSNormal;
	m_EditState = enBSNormal;
	m_pLeftImage = NULL;
	m_pSmallImage = NULL;
	m_pEdit = NULL;
	m_strTitle = _T("");

	SetRect(CRect(0,0,0,0));
	SetBitmapCount(4);

	m_sizeLeftImage.SetSize(0,0);
	m_sizeSmallImage.SetSize(0,0);

	m_bBack = false;
	m_clrBack = Color(255, 255, 255);
	m_clrText = Color(255, 0, 0, 0);

	m_bPassWord = false;
	m_bMultiLine = false;
	m_bWantReturn = true;
	m_bAutoHScroll = false;
	m_bAutoVScroll = false;
	m_bShowHScroll = false;
	m_bShowVScroll = false;
	m_bNumber = false;
	m_bReadOnly = false;
	m_nMaxChar = -1;

	m_bIsSmallButton = FALSE;
}

CDuiEdit::CDuiEdit(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, CString strTitle/* = ""*/,
			BOOL bPassWord/* = FALSE*/, BOOL bIsVisible/* = TRUE*/, BOOL bIsDisable/* = FALSE*/ ,BOOL bIsPressDown/* = FALSE*/)
		: CControlBaseFont(hWnd, pDuiObject, uControlID, rc, strTitle, bIsVisible, bIsDisable)
{
	m_bTabStop = TRUE;	// 可以响应tab键
	m_bDown = false;
	m_bDownTemp = false;
	m_buttonState = enBSNormal;
	m_EditState = enBSNormal;
	m_pLeftImage = NULL;
	m_pSmallImage = NULL;
	m_pEdit = NULL;
	m_bPassWord = bPassWord;
	m_strTitle = strTitle;

	m_sizeLeftImage.SetSize(0,0);
	m_sizeSmallImage.SetSize(0,0);

	m_bBack = false;
	m_clrBack = Color(255, 255, 255);
	m_clrText = Color(255, 0, 0, 0);

	SetRect(rc);
	SetBitmapCount(4);

	m_bMultiLine = false;
	m_bAutoHScroll = false;
	m_bAutoVScroll = false;
	m_bShowHScroll = false;
	m_bShowVScroll = false;
	m_bNumber = false;
	m_bReadOnly = false;
	m_nMaxChar = -1;

	m_bIsSmallButton = FALSE;
}

CDuiEdit::~CDuiEdit(void)
{
	// 删除Windows控件
	DeleteEdit();

	if (m_fontTemp.m_hObject)
	{
		m_fontTemp.DeleteObject();
	}

	if(m_pLeftImage != NULL)
	{
		delete m_pLeftImage;
		m_pLeftImage = NULL;
	}

	if(m_pSmallImage != NULL)
	{
		delete m_pSmallImage;
		m_pSmallImage = NULL;
	}
}

bool CDuiEdit::SetLeftBitmap(UINT nResourceID, CString strType)
{
	if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, m_pLeftImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight()) >= 3)
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetWidth() / 4, m_pLeftImage->GetHeight());
			m_nLeftImageCount = 4;
		}else
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetHeight(), m_pLeftImage->GetHeight());
			m_nLeftImageCount = m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight();
		}
		return true;
	}
	return false;
}

bool CDuiEdit::SetLeftBitmap(CString strImage)
{
	if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, m_pLeftImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight()) >= 3)
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetWidth() / 4, m_pLeftImage->GetHeight());
			m_nLeftImageCount = 4;
		}else
		{
			m_sizeLeftImage.SetSize(m_pLeftImage->GetHeight(), m_pLeftImage->GetHeight());
			m_nLeftImageCount = m_pLeftImage->GetWidth() / m_pLeftImage->GetHeight();
		}
		return true;
	}
	return false;
}

// 从XML设置图片信息属性
HRESULT CDuiEdit::OnAttributeLeftImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// 加载图片文件
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!SetLeftBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = _ttoi(strSkin);
		if(!SetLeftBitmap(nResourceID, TEXT("PNG")))
		{
			if(!SetLeftBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

bool CDuiEdit::SetSmallBitmap(UINT nResourceID, CString strType)
{
	if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, m_pSmallImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight()) >= 3)
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetWidth() / 4, m_pSmallImage->GetHeight());
			m_nSmallImageCount = 4;
			m_bIsSmallButton = TRUE;
		}else
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetHeight(), m_pSmallImage->GetHeight());
			m_nSmallImageCount = m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight();
			m_bIsSmallButton = FALSE;
		}
		return true;
	}
	return false;
}

bool CDuiEdit::SetSmallBitmap(CString strImage)
{
	if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, m_pSmallImage))
	{
		// 如果宽高比大于或等于3,则按照4张图片,否则按照宽度和高度相同来设置宽度和高度
		// 如果是4张图片的情况,才认为小图片是一个按钮
		if((m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight()) >= 3)
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetWidth() / 4, m_pSmallImage->GetHeight());
			m_nSmallImageCount = 4;
			m_bIsSmallButton = TRUE;
		}else
		{
			m_sizeSmallImage.SetSize(m_pSmallImage->GetHeight(), m_pSmallImage->GetHeight());
			m_nSmallImageCount = m_pSmallImage->GetWidth() / m_pSmallImage->GetHeight();
			m_bIsSmallButton = FALSE;
		}
		return true;
	}
	return false;
}

// 从XML设置图片信息属性
HRESULT CDuiEdit::OnAttributeSmallImage(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// 通过Skin读取
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// 加载图片文件
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!SetSmallBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// 加载图片资源
	{
		UINT nResourceID = _ttoi(strSkin);
		if(!SetSmallBitmap(nResourceID, TEXT("PNG")))
		{
			if(!SetSmallBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// 设置背景色
void CDuiEdit::SetBackColor(Color clrBack)
{
	m_clrBack = clrBack;
	if(!m_bBack)
	{
		// 如果没有设置过背景色,则删除已创建的edit控件,等下一次显示时候自动刷新edit控件的颜色
		DeleteEdit();
	}else
	{
		// 如果设置过背景色,则只要调用CBkColorEdit的设置背景色的函数刷新edit控件的背景色
		static_cast<CColorEdit*>(m_pEdit)->SetBackColor(m_clrBack.ToCOLORREF());
	}
	m_bBack = true;
}

// 从XML设置背景颜色属性
HRESULT CDuiEdit::OnAttributeBackColor(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	m_clrBack = CDuiObject::StringToColor(strValue);
	m_bBack = true;

	return bLoading?S_FALSE:S_OK;
}

void  CDuiEdit::SetControlRect(CRect rc) 
{
	BOOL bRefresh = ((m_rc.top != rc.top) || (m_rc.left != rc.left) || (m_rc.bottom != rc.bottom) || (m_rc.right != rc.right));
	m_rc = rc;
	m_rcText = m_rc;
	m_rcText.top += 4;
	m_rcText.left += (6 + m_sizeLeftImage.cx);
	m_rcText.bottom -= 4;
	m_rcText.right -= (3 + m_sizeSmallImage.cx);
	// 如果edit位置有变化,删除编辑控件,这样当编辑控件重新创建时候就可以调整大小
	if(bRefresh)
	{
		DeleteEdit();
	}
}

BOOL CDuiEdit::IsDraw(CPoint point)
{
 	if(m_buttonState == enBSDown)
 	{
 		return false;
 	}
	CRect  rc;
	rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
	rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
	rc.right = rc.left + m_sizeSmallImage.cx;
	rc.bottom = rc.top + m_sizeSmallImage.cy;

	if(rc.PtInRect(point))
	{
		if(m_buttonState == enBSNormal)
			return true;
	}
	else
	{
		if(m_buttonState != enBSNormal)
			return true;
	}

	return false;
}

// 设置控件中的Windows原生控件是否可见的状态
void CDuiEdit::SetControlWndVisible(BOOL bIsVisible)
{
	if(bIsVisible)
	{
		ShowEdit();
	}else
	{
		HideEdit();
	}
}

// 设置控件的禁用状态
void CDuiEdit::SetControlDisable(BOOL bIsDisable)
{
	if(m_bIsDisable != bIsDisable)
	{
		m_bIsDisable = bIsDisable;
		if(bIsDisable)
		{
			m_EditState = enBSDisable;
			m_buttonState = enBSDisable;
			DeleteEdit();
		}
		else
		{
			if(m_bDown)
			{
				m_EditState = enBSDown;
				m_buttonState = enBSDown;
			}
			else
			{
				m_EditState = enBSNormal;
				m_buttonState = enBSNormal;
			}
		}
		::InvalidateRect(m_hWnd, &m_rc, true);
	}
}

// 设置控件title
void CDuiEdit::SetControlTitle(CString strTitle)
{
	__super::SetControlTitle(strTitle);

	// 如果edit控件已经激活,则需要设置edit控件的内容
	if(m_pEdit)
	{
		m_pEdit->SetWindowText(m_strTitle);	
		m_pEdit->SetSel(m_strTitle.GetLength(), -1);
	}
}

// 设置控件的焦点
BOOL CDuiEdit::SetControlFocus(BOOL bFocus)
{
	__super::SetControlFocus(bFocus);

	enumButtonState buttonState = m_buttonState;
	enumButtonState editState = m_EditState;

	if(!bFocus)
	{
		m_bDown = false;
		m_buttonState = enBSNormal;
		m_EditState = enBSNormal;
		HideEdit();
	}else
	{
		m_bDown = true;
		m_buttonState = enBSDown;
		m_EditState = enBSDown;
		ShowEdit();
	}

	bool bIsDraw = buttonState != m_buttonState || editState != m_EditState;
	if(bIsDraw)
	{
		::InvalidateRect(m_hWnd, &m_rc, true);
	}

	return bIsDraw;
}

BOOL CDuiEdit::OnControlMouseMove(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_buttonState;
	enumButtonState editState = m_EditState;
	if(!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_EditState = enBSHover;
			if(m_buttonState != enBSDown)
			{
				CRect  rc;
				rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
				rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
				rc.right = rc.left + m_sizeSmallImage.cx;
				rc.bottom = rc.top + m_sizeSmallImage.cy;

				if(rc.PtInRect(point))
				{
					m_buttonState = enBSHover;
				}
				else
				{
					//ShowEdit();
					m_buttonState = enBSNormal;
				}
			}
		}
		else
		{
			if(m_buttonState != enBSDown)
			{
				m_buttonState = enBSNormal;
				m_EditState = enBSNormal;
			}
		}
	}
	
	if(buttonState != m_buttonState || editState != m_EditState)
	{
		UpdateControl();
	}

	return buttonState != m_buttonState || editState != m_EditState;
}

BOOL CDuiEdit::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_buttonState;	
	enumButtonState editState = m_EditState;
	if(!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			CRect  rc;
			rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
			rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
			rc.right = rc.left + m_sizeSmallImage.cx;
			rc.bottom = rc.top + m_sizeSmallImage.cy;

			if(rc.PtInRect(point))	// 点击的是编辑框右部的小按钮图片
			{
				if(m_bIsSmallButton)	// 小图片作为按钮才进行处理
				{
					m_bDown = !m_bDown;
					m_bDownTemp = true;
					if(m_bDown)
					{
						m_buttonState = enBSDown;
					}
					else
					{				
						m_buttonState = enBSHover;
					}				
					SendMessage(MSG_CONTROL_BUTTON, CONTROL_BUTTON, MSG_BUTTON_DOWN);
					HideEdit();
				}
			}
			else
			{
				if(m_bDown)
				{
					m_bDown = false;
					m_buttonState = enBSHover;
				}
				ShowEdit();
				
				SendMessage(MSG_CONTROL_BUTTON, CONTROL_EDIT, MSG_BUTTON_DOWN);
			}		
		}
		else
		{
			m_buttonState = enBSNormal;
			m_EditState = enBSNormal;
		}
	}

	if(buttonState != m_buttonState || editState != m_EditState)
	{
		UpdateControl();
	}
	
	return buttonState != m_buttonState || editState != m_EditState;
}

BOOL CDuiEdit::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	enumButtonState buttonState = m_buttonState;
	enumButtonState editState = m_EditState;
	if(!m_bIsDisable)
	{
		if(m_rc.PtInRect(point))
		{
			m_EditState = enBSHover;

			CRect  rc;
			rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
			rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
			rc.right = rc.left + m_sizeSmallImage.cx;
			rc.bottom = rc.top + m_sizeSmallImage.cy;

			if(rc.PtInRect(point))
			{
				if(m_bIsSmallButton)	// 小图片作为按钮才进行处理
				{
					if(m_bDown)
					{
						m_buttonState = enBSDown;
					}
					else
					{
						m_buttonState = enBSHover;
					}	
					SendMessage(MSG_CONTROL_BUTTON, CONTROL_BUTTON, MSG_BUTTON_UP);
				}
			}
			else
			{
				if(m_bDown)
				{
					m_buttonState = enBSDown;
				}
				else
				{
					m_buttonState = enBSNormal;
				}	
				SendMessage(MSG_CONTROL_BUTTON, CONTROL_EDIT, MSG_BUTTON_UP);
			}			
		}
		else
		{
			if(!m_bDown)
			{
				m_buttonState = enBSNormal;
				m_EditState = enBSNormal;
			}
		}
	}
	
	m_bDownTemp = false;

	if(buttonState != m_buttonState || editState != m_EditState)
	{
		UpdateControl();
	}

	return buttonState != m_buttonState || editState != m_EditState;
}

// 键盘事件处理
BOOL CDuiEdit::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// 如果是回车键,则转换为字符事件传递给原生控件
	if((nChar == VK_RETURN) && m_pEdit && ::IsWindow(m_pEdit->GetSafeHwnd()))
	{
		m_pEdit->SendMessage(WM_CHAR, VK_RETURN, nFlags);
		// 返回值为是否允许回车换行,如果不允许,则即使焦点在此输入控件,也可以按回车键关闭窗口
		return m_bWantReturn;
	}

	return false;
}

BOOL CDuiEdit::GetLButtonDown()
{
	return m_buttonState == enBSDown;
}

BOOL CDuiEdit::GetLButtonUp()
{
	return FALSE;
}

// 获取编辑框文字
CString CDuiEdit::GetEditText()
{
	if(m_pEdit && ::IsWindow(m_pEdit->GetSafeHwnd()))
	{
		m_pEdit->GetWindowText(m_strTitle);
	}
	return m_strTitle;
}

// 创建字体对象
void CDuiEdit::CreateEditFont()
{
	if (m_fontTemp.m_hObject)
	{
		return;
	}

	// 计算字体的高度,转换方法是设置的宽度值*18/12,如果宽度值大于12,则转换结果减2,这个转换方法可能不准确
	int nFontHeight = m_nFontWidth * 18 / 12;
	if(m_nFontWidth > 12)
	{
		nFontHeight-= 2;
	}

	VERIFY(m_fontTemp.CreateFont(
		nFontHeight,			// 字体的高度  
		0,							// 字体的宽度  
		0,							// 字体显示的角度
		0,							// 字体的角度
		(((m_fontStyle & FontStyleBold) != 0) ? FW_BOLD : FW_NORMAL),	// 字体的磅数
		((m_fontStyle & FontStyleItalic) != 0),	// 斜体字体
		((m_fontStyle & FontStyleUnderline) != 0),		// 带下划线的字体
		((m_fontStyle & FontStyleStrikeout) != 0),		// 带删除线的字体
		GB2312_CHARSET,				// 所需的字符集
		OUT_DEFAULT_PRECIS,			// 输出的精度
		CLIP_DEFAULT_PRECIS,		// 裁减的精度
		DEFAULT_QUALITY,			// 逻辑字体与输出设备的实际
		DEFAULT_PITCH | FF_SWISS,	// 字体间距和字体集
		m_strFont));					// 字体名称
}

// 显示编辑控件
void CDuiEdit::ShowEdit()
{
	// 如果是只读属性，则不用创建edit控件
	if(m_bReadOnly || m_bIsDisable)
	{
		return;
	}

 	if(NULL == m_pEdit)
 	{
		TestMainThread();	// 测试是否在主线程

		CRect rc;
		rc = m_rcText;
		//rc.left--;
		if(!m_bMultiLine)
		{
			// 仅针对单行编辑框调整位置
			rc.top += (m_rc.Height() - 18 - 6) / 2;	// 调整windows控件激活时候的显示位置,和非激活状态下更接近一些
			if(m_nFontWidth > 12)	// 如果修改了默认字体,则做一些微调
			{
				rc.top--;
			}
		}else
		{
			rc.top += 2;
		}

		if(m_bBack || (m_clrText.GetValue() != Color(255,0,0,0).GetValue()))
		{
			// 如果设置了背景色或文字色,则创建颜色可以更改的编辑控件
			m_pEdit = new CColorEdit(m_clrBack.ToCOLORREF(), m_clrText.ToCOLORREF(), (m_clrBack.GetAlpha() == 0));
		}else
		{
			// 否则创建普通的编辑控件
  			m_pEdit = new CEdit;
		}

		DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_TABSTOP;
		if(m_bMultiLine)
		{
			dwStyle |= ES_MULTILINE;
		}
		if(m_bWantReturn)
		{
			dwStyle |= ES_WANTRETURN;
		}
		if(m_bAutoHScroll)
		{
			dwStyle |= ES_AUTOHSCROLL;
		}
		if(m_bAutoVScroll)
		{
			dwStyle |= ES_AUTOVSCROLL;
		}
		if(m_bNumber)
		{
			dwStyle |= ES_NUMBER;
		}
		if(m_bReadOnly)
		{
			dwStyle |= ES_READONLY;
		}

		// 计算父窗口的偏移量
		CPoint pt(rc.left, rc.top);
		if (m_pParentDuiObject)
		{
			CControlBase* pParentDuiObject = dynamic_cast<CControlBase*>(m_pParentDuiObject);
			if (pParentDuiObject && pParentDuiObject->OnMousePointChange(pt))
			{
				int xOffset = pt.x - rc.left;
				int yOffset = pt.y - rc.top;
				rc.left = rc.left - xOffset;
				rc.right = rc.right - xOffset;
				rc.top = rc.top - yOffset;
				rc.bottom = rc.bottom - yOffset;
			}			
		}	

  		m_pEdit->Create(dwStyle, rc, CWnd::FromHandle(m_hWnd), GetID());
		// 创建并设置字体对象
		CreateEditFont();
  		m_pEdit->SetFont(&m_fontTemp);
		m_pEdit->SetWindowText(m_strTitle);
		if(m_bPassWord)
		{
			m_pEdit->SetPasswordChar('*');
		}
		if(m_nMaxChar > 0)
		{
			m_pEdit->LimitText(m_nMaxChar);
		}
		m_pEdit->SetSel(m_strTitle.GetLength(), -1);
 	}else
	{
		// 如果原生控件已创建,则显示出来
		m_pEdit->ShowWindow(SW_NORMAL);
	}

	// 设置滚动条是否显示,只读和禁用属性等
	m_pEdit->ShowScrollBar(SB_VERT, m_bShowVScroll);
	m_pEdit->ShowScrollBar(SB_HORZ, m_bShowHScroll);
	m_pEdit->EnableWindow(!m_bIsDisable);
	m_pEdit->SetReadOnly(m_bReadOnly);
	m_pEdit->SetFocus();
}

// 隐藏编辑控件
void CDuiEdit::HideEdit()
{
	if(m_pEdit)
	{
		TestMainThread();	// 测试是否在主线程

		// 调用Edit的Windows控件必须先判断窗口是否有效
		if(::IsWindow(m_pEdit->GetSafeHwnd()))
		{
			// 获取编辑框的内容保存在控件的变量中
			m_pEdit->GetWindowText(m_strTitle);
		}

		// 隐藏输入控件
		m_pEdit->ShowWindow(SW_HIDE);
	}
}

// 删除编辑控件
void CDuiEdit::DeleteEdit()
{
	if(m_pEdit)
	{
		TestMainThread();	// 测试是否在主线程

		// 调用Edit的Windows控件必须先判断窗口是否有效
		if(::IsWindow(m_pEdit->GetSafeHwnd()))
		{
			// 获取编辑框的内容保存在控件的变量中
			m_pEdit->GetWindowText(m_strTitle);
		}
		delete m_pEdit;
		m_pEdit = NULL;
	}
}

// 消息处理
LRESULT CDuiEdit::OnMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return __super::OnMessage(uID, uMsg, wParam, lParam);
}

void CDuiEdit::DrawControl(CDC &dc, CRect rcUpdate)
{
	Graphics graphics(dc);

	DrawImageFrame(graphics, m_pImage, m_rc, m_EditState * m_sizeImage.cx, 0, m_sizeImage.cx, m_sizeImage.cy, 4);

	// 画背景色
	if(m_bBack)
	{
		SolidBrush brush(m_clrBack);
		graphics.FillRectangle(&brush, m_rc.left + 2, m_rc.top + 2, m_rc.Width() - 4, m_rc.Height() - 4);
	}

	if(m_pLeftImage)
	{
		CRect  rc;
		rc.left = m_rc.left + 2;
		rc.top = m_rc.top + (m_rc.Height() - m_sizeLeftImage.cy) / 2;
		rc.right = rc.left + m_sizeLeftImage.cx;
		rc.bottom = rc.top + m_sizeLeftImage.cy;
		
		if(m_nLeftImageCount > m_buttonState)
		{
			graphics.DrawImage(m_pLeftImage, RectF((Gdiplus::REAL)rc.left , (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height()),
				(Gdiplus::REAL)(m_buttonState * m_sizeLeftImage.cx), 0, (Gdiplus::REAL)m_sizeLeftImage.cx, (Gdiplus::REAL)m_sizeLeftImage.cy, UnitPixel);
		}else
		{
			graphics.DrawImage(m_pLeftImage, RectF((Gdiplus::REAL)rc.left , (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height()),
				0, 0, (Gdiplus::REAL)m_sizeLeftImage.cx, (Gdiplus::REAL)m_sizeLeftImage.cy, UnitPixel);
		}
	}

	if(m_pSmallImage)
	{
		CRect  rc;
		rc.left = m_rc.right - m_sizeSmallImage.cx - 2;
		rc.top = m_rc.top + (m_rc.Height() - m_sizeSmallImage.cy) / 2;
		rc.right = rc.left + m_sizeSmallImage.cx;
		rc.bottom = rc.top + m_sizeSmallImage.cy;
		
		if(m_nSmallImageCount > m_buttonState)
		{
			graphics.DrawImage(m_pSmallImage, RectF((Gdiplus::REAL)rc.left , (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height()),
				(Gdiplus::REAL)(m_buttonState * m_sizeSmallImage.cx), 0, (Gdiplus::REAL)m_sizeSmallImage.cx, (Gdiplus::REAL)m_sizeSmallImage.cy, UnitPixel);
		}else
		{
			graphics.DrawImage(m_pSmallImage, RectF((Gdiplus::REAL)rc.left , (Gdiplus::REAL)rc.top, (Gdiplus::REAL)rc.Width(), (Gdiplus::REAL)rc.Height()),
				0, 0, (Gdiplus::REAL)m_sizeSmallImage.cx, (Gdiplus::REAL)m_sizeSmallImage.cy, UnitPixel);
		}
	}

	BSTR bsFont = m_strFont.AllocSysString();
	FontFamily fontFamily(bsFont);
	Font font(&fontFamily, (REAL)m_nFontWidth, m_fontStyle, UnitPixel);
	SolidBrush solidBrush(m_clrText);
	SolidBrush solidBrushTip(m_clrTooltip);
	graphics.SetTextRenderingHint( TextRenderingHintClearTypeGridFit );
	::SysFreeString(bsFont);
	StringFormat strFormat;
	strFormat.SetAlignment(StringAlignmentNear);		// 水平方向左对齐
	if(!m_bMultiLine)
	{
		// 单行文字
		strFormat.SetLineAlignment(StringAlignmentCenter);	// 垂直方向中间对齐
		strFormat.SetFormatFlags( StringFormatFlagsNoWrap | StringFormatFlagsMeasureTrailingSpaces);
	}else
	{
		strFormat.SetLineAlignment(StringAlignmentNear);	// 垂直方向上对齐
	}

	RectF rect((Gdiplus::REAL)m_rcText.left, (Gdiplus::REAL)(m_rcText.top+2), (Gdiplus::REAL)m_rcText.Width(), (Gdiplus::REAL)(m_rcText.Height()-2));

	if(!m_strTitle.IsEmpty())
	{
		// 文字非空
		CString strTitle = m_strTitle;
		if(m_bPassWord)
		{
			int nlen = strTitle.GetLength();
			strTitle = "";
			for(int i = 0; i < nlen; i++)
			{
				strTitle += '*';
			}
		}

		BSTR bsTitle = strTitle.AllocSysString();
		graphics.DrawString(bsTitle, (INT)wcslen(bsTitle), &font, rect, &strFormat, &solidBrush);
		::SysFreeString(bsTitle);
	}else
	if(!m_strTooltip.IsEmpty())
	{
		// 如果没有文字,但设置了tooltip,则显示tooltip
		BSTR bsTooltip = m_strTooltip.AllocSysString();
		graphics.DrawString(bsTooltip, (INT)wcslen(bsTooltip), &font, rect, &strFormat, &solidBrushTip);
		::SysFreeString(bsTooltip);
	}
}