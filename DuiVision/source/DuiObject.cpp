#include "StdAfx.h"
#include "DuiObject.h"

// DUI对象ID自动生成变量，DUI对象ID从1000开始生成（DUI控件ID自动生成的起始值和这里的对象ID是不一样的）
static int g_nDuiObjlId = 1000;

CDuiObject::CDuiObject(void)
{
	m_uID = g_nDuiObjlId++;
	m_strName = _T("");
	m_pDuiHandler = NULL;
	SetRect(CRect(0,0,0,0));
	LoadAttributesInfo();
}

CDuiObject::~CDuiObject(void)
{
}

// 判断是否此对象
BOOL CDuiObject::IsThisObject(UINT uID, CString strName)
{
	if(uID == 0 || strName == _T(""))
	{
		return FALSE;
	}

	if(uID == m_uID || strName == m_strName)
	{
		return TRUE;
	}

	return FALSE;
}

// 注册事件处理对象
BOOL CDuiObject::RegisterHandler(CDuiHandler* pDuiHandler)
{
	if(pDuiHandler == NULL)
	{
		return FALSE;
	}

	m_pDuiHandler = pDuiHandler;
	m_pDuiHandler->SetDuiObject(this);

	return TRUE;
}

// 设置对象属性的基础函数，负责ID和name属性的设置
HRESULT CDuiObject::SetAttribute(CString strAttribName, CString strValue, BOOL bLoading)
{
	HRESULT hRet = E_FAIL;
	if (_T("id") == strAttribName)
	{
		m_uID = ::StrToInt(strValue);
		hRet = S_FALSE;
	}else
	if (_T("name") == strAttribName)
	{
		m_strName = strValue;
		hRet = S_FALSE;
	}else
		return E_FAIL;

    return hRet;
}

// 加载属性信息的基础函数
BOOL CDuiObject::LoadAttributesInfo()
{
	// 添加id属性信息
	DuiObjectAttributeInfo attrInfoId;
	attrInfoId.strName = _T("id");
	m_mapDuiAttrInfo.SetAt(_T("id"), attrInfoId);

	// 添加name属性信息
	DuiObjectAttributeInfo attrInfoName;
	m_mapDuiAttrInfo.SetAt(_T("name"), attrInfoName);

	return TRUE;
}

// 加载XML节点，解析节点中的属性信息设置到当前控件的属性中
BOOL CDuiObject::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	// pos属性需要特殊处理,放在最后进行设置,否则有些属性会受到影响,不能正确的初始化
	CString strPosValue = _T("");
    for (DuiXmlAttribute pAttrib = pXmlElem.first_attribute(); pAttrib; pAttrib = pAttrib.next_attribute())
    {
		CString strName = pAttrib.name();
		if(strName == _T("pos"))
		{
			strPosValue = pAttrib.value();
		}else
		{
			SetAttribute(pAttrib.name(), pAttrib.value(), TRUE);
		}
    }

	if(!strPosValue.IsEmpty())
	{
		SetAttribute(_T("pos"), strPosValue, TRUE);
	}

	// 初始化
	OnInit();

    return TRUE;
}

// 初始化,由Load函数加载完属性之后调用
BOOL CDuiObject::OnInit()
{
	return TRUE;
}

// 解析字符串，替换其中的替换内容
void CDuiObject::ParseDuiString(CString& strString)
{
	DuiSystem::Instance()->ParseDuiString(strString);
}

ULONG CDuiObject::HexStringToULong(LPCTSTR lpszValue, int nSize)
{
	/*
	std::wstring ws = lpszValue;
    ULONG value;
    std::wistringstream iss(ws);
    iss >> std::hex >> value;
	return value;
	*/
	/*
	ULONG ret=0;
	CString strValue;
	strValue.Format(_T("0X%s"),lpszValue);
	StrToIntEx(strValue,STIF_SUPPORT_HEX,&ret);
	return ret;
	*/

	//CStringA strValueA;
	//strValueA = lpszValue;//CEncodingUtil::AnsiToUnicode(lpszValue);
	//这里不明白 我改为_T类型统一了
	CString strValueA = lpszValue;

    LPCTSTR pchValue = strValueA.GetBuffer();;
    ULONG ulValue = 0;

    while (*pchValue && nSize != 0)
    {
        ulValue <<= 4;

        if ('a' <= *pchValue && 'f' >= *pchValue)
            ulValue |= (*pchValue - 'a' + 10);
        else if ('A' <= *pchValue && 'F' >= *pchValue)
            ulValue |= (*pchValue - 'A' + 10);
        else if ('0' <= *pchValue && '9' >= *pchValue)
            ulValue |= (*pchValue - '0');
        else
            return 0;

        ++ pchValue;
        -- nSize;
    }

	strValueA.ReleaseBuffer();

    return ulValue;
}

// 16进制字符串转换为Color对象
Color CDuiObject::HexStringToColor(LPCTSTR lpszValue)
{
    return Color(
		(BYTE)HexStringToULong(lpszValue, 2), 
        (BYTE)HexStringToULong(lpszValue + 2, 2), 
        (BYTE)HexStringToULong(lpszValue + 4, 2)
        );
}

// 10进制逗号分隔字符串或16进制字符串转换为Color对象
Color CDuiObject::StringToColor(LPCTSTR lpszValue, Color clrDefault)
{
	CStringA strValue;
	strValue = lpszValue;

	// 字符串为空则返回默认值
	if(strValue.IsEmpty())
	{
		return clrDefault;
	}

	// 没有逗号,则按照10进制处理
	if(strValue.Find(",") == -1)
	{
		return HexStringToColor(lpszValue);
	}

	BYTE c1,c2,c3,c4;
	CStringA s1 = "";
	CStringA s2 = "";
	CStringA s3 = "";
	CStringA s4 = "";
	int nPos = strValue.Find(",");
	if(nPos != -1)
	{
		s1 = strValue.Left(nPos);
		strValue.Delete(0, nPos+1);
		nPos = strValue.Find(",");
		if(nPos != -1)
		{
			s2 = strValue.Left(nPos);
			strValue.Delete(0, nPos+1);
			nPos = strValue.Find(",");
			if(nPos != -1)
			{
				s3 = strValue.Left(nPos);
				strValue.Delete(0, nPos+1);
				s4 = strValue;
			}else
			{
				s3 = strValue;
			}
		}
	}

	c1 = atoi(s1);
	c2 = atoi(s2);
	c3 = atoi(s3);
	c4 = atoi(s4);

	if(s4.IsEmpty())
	{
		return Color(c1, c2, c3);
	}else
	{
		return Color(c1, c2, c3, c4);
	}
}

// 16进制字符串转RGB颜色
COLORREF CDuiObject::HexStringToRGBColor(LPCTSTR lpszValue)
{
    return RGB(
        HexStringToULong(lpszValue, 2), 
        HexStringToULong(lpszValue + 2, 2), 
        HexStringToULong(lpszValue + 4, 2)
        );
}

// 根据字符串获取键盘码
void CDuiObject::ParseKeyCode(LPCTSTR lpszValue, UINT& nChar, UINT& nFlag)
{
	CStringA strValue;
	strValue = lpszValue;
	nChar = 0;
	nFlag = 0;
	strValue.Trim();
	strValue.MakeUpper();
	CStringA strFlag = "";
	CStringA strChar = strValue;
	int nPos = strValue.Find("+");
	if(nPos != -1)
	{
		strFlag = strValue.Left(nPos);
		strValue.Delete(0, nPos+1);
		strChar = strValue;
		strFlag.Trim();
		strChar.Trim();
	}

	if(strChar.IsEmpty())
	{
		return;
	}

	if(strFlag == "CTRL")
	{
		nFlag |= VK_CONTROL;
	}else
	if(strFlag == "ALT")
	{
		nFlag |= VK_MENU;
	}else
	if(strFlag == "SHIFT")
	{
		nFlag |= VK_SHIFT;
	}

	if(strChar == "RETURN")
	{
		nChar = VK_RETURN;
	}else
	if(strChar == "ESC")
	{
		nChar = VK_ESCAPE;
	}else
	if(strChar == "BACK")
	{
		nChar = VK_BACK;
	}else
	if(strChar == "TAB")
	{
		nChar = VK_TAB;
	}else
	if(strChar == "SPACE")
	{
		nChar = VK_SPACE;
	}else
	if(strChar == "PRIOR")
	{
		nChar = VK_PRIOR;
	}else
	if(strChar == "NEXT")
	{
		nChar = VK_NEXT;
	}else
	if(strChar == "END")
	{
		nChar = VK_END;
	}else
	if(strChar == "HOME")
	{
		nChar = VK_HOME;
	}else
	if(strChar == "LEFT")
	{
		nChar = VK_LEFT;
	}else
	if(strChar == "UP")
	{
		nChar = VK_UP;
	}else
	if(strChar == "RIGHT")
	{
		nChar = VK_RIGHT;
	}else
	if(strChar == "DOWN")
	{
		nChar = VK_DOWN;
	}else
	if(strChar == "SELECT")
	{
		nChar = VK_SELECT;
	}else
	if(strChar == "PRINT")
	{
		nChar = VK_PRINT;
	}else
	if(strChar == "INSERT")
	{
		nChar = VK_INSERT;
	}else
	if(strChar == "DELETE")
	{
		nChar = VK_DELETE;
	}else
	if(strChar == "F1")
	{
		nChar = VK_F1;
	}else
	if(strChar == "F2")
	{
		nChar = VK_F2;
	}else
	if(strChar == "F3")
	{
		nChar = VK_F3;
	}else
	if(strChar == "F4")
	{
		nChar = VK_F4;
	}else
	if(strChar == "F5")
	{
		nChar = VK_F5;
	}else
	if(strChar == "F6")
	{
		nChar = VK_F6;
	}else
	if(strChar == "F7")
	{
		nChar = VK_F7;
	}else
	if(strChar == "F8")
	{
		nChar = VK_F8;
	}else
	if(strChar == "F9")
	{
		nChar = VK_F9;
	}else
	if(strChar == "F10")
	{
		nChar = VK_F10;
	}else
	if(strChar == "F11")
	{
		nChar = VK_F11;
	}else
	if(strChar == "F12")
	{
		nChar = VK_F12;
	}else
	{
		char ch = strChar[0];
		if(((ch >= '0') && (ch < '9')) || ((ch >= 'A') && (ch < 'Z')))
		{
			nChar = ch;
		}
	}
}