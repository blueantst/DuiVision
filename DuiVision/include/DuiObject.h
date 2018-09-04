#pragma once

#include  <afxtempl.h>
#include "DuiXml.h"

//////////////////////////////////////////////////////////////////////////////////
// DuiVision Class Name Declaration
#define DUIOBJ_DECLARE_CLASS_NAME(theclass, classname)   \
public:                                                 \
    static CControlBase* CheckAndNew(LPCTSTR lpszName, HWND hWnd, CDuiObject* pDuiObject)       \
    {                                                   \
        if (_tcscmp(GetClassName(), lpszName)  == 0)     \
		{\
			CControlBase* pControl = new theclass(hWnd, pDuiObject);							\
			return pControl;                        \
		}\
        else                                            \
            return NULL;                                \
    }                                                   \
                                                        \
    static LPCTSTR GetClassName()                        \
    {                                                   \
        return classname;                               \
    }                                                   \
                                                        \
    virtual LPCTSTR GetObjectClass()                     \
    {                                                   \
        return classname;                               \
    }                                                   \
                                                        \
    virtual BOOL IsClass(LPCTSTR lpszName)               \
    {                                                   \
        if(_tcscmp(GetClassName(), lpszName)  == 0) return TRUE;  \
		return __super::IsClass(lpszName);				\
    }                                                   \
														\
	virtual LPCTSTR BaseObjectClassName()				\
	{													\
		return __super::GetObjectClass();				\
	}													\


//////////////////////////////////////////////////////////////////////////
// Xml Attributes Declaration Map

// Attribute Declaration
#define DUI_DECLARE_ATTRIBUTES_BEGIN()                            \
public:                                                             \
    virtual HRESULT SetAttribute(                                   \
        CString strAttribName,                                     \
        CString strValue,                                          \
        BOOL     bLoading)                                          \
    {                                                               \
        HRESULT hRet = __super::SetAttribute(                       \
            strAttribName,                                          \
            strValue,                                               \
            bLoading                                                \
            );                                                      \
        if (SUCCEEDED(hRet))                                        \
            return hRet;                                            \

#define DUI_DECLARE_ATTRIBUTES_END()                              \
            return E_FAIL;                                          \
                                                                    \
        return hRet;                                                \
    }                                                               \

#define DUI_CHAIN_ATTRIBUTE(varname, allredraw)                   \
        if (SUCCEEDED(hRet = varname.SetAttribute(strAttribName, strValue, bLoading)))   \
        {                                                           \
            return hRet;                                            \
        }                                                           \
        else                                                        \

// Custom Attribute
// HRESULT OnAttributeChange(CStringA& strValue, BOOL bLoading)
#define DUI_CUSTOM_ATTRIBUTE(attribname, func)                    \
        if (attribname == strAttribName)                            \
        {                                                           \
            hRet = func(strValue, bLoading);                        \
        }                                                           \
        else                                                        \

// Int = %d StringA
#define DUI_INT_ATTRIBUTE(attribname, varname, allredraw)         \
        if (attribname == strAttribName)                            \
        {                                                           \
			    varname = ::StrToInt(strValue);                    \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// bool = %d StringA
#define DUI_BOOL_ATTRIBUTE(attribname, varname, allredraw)         \
        if (attribname == strAttribName)                            \
        {                                                           \
		    varname = ((::StrToInt(strValue) > 0) || (strValue == _T("true"))) ? true : false;     \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// UInt = %u StringA
#define DUI_UINT_ATTRIBUTE(attribname, varname, allredraw)        \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = (UINT)::StrToInt(strValue);                  \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// DWORD = %u StringA
#define DUI_DWORD_ATTRIBUTE(attribname, varname, allredraw)       \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = (DWORD)::StrToInt(strValue);                 \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// StringA = StringA
#define DUI_STRING_ATTRIBUTE(attribname, varname, allredraw)      \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = strValue;                                     \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// StringT = StringA
#define DUI_TSTRING_ATTRIBUTE(attribname, varname, allredraw)     \
        if (attribname == strAttribName)                            \
        {                                                           \
			ParseDuiString(strValue);	\
            varname = strValue;                      \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// DWORD = %X StringA
#define DUI_HEX_ATTRIBUTE(attribname, varname, allredraw)         \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = CDuiObject::HexStringToULong(strValue);        \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// Color = %08X StringA
#define DUI_COLOR_ATTRIBUTE(attribname, varname, allredraw)       \
        if (attribname == strAttribName)                            \
        {                                                           \
			if(strValue.Find(_T(",")) == -1)	\
			{	\
				varname = CDuiObject::HexStringToColor(strValue);        \
			}else	\
			{	\
				varname = CDuiObject::StringToColor(strValue);        \
			}	\
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// COLORREF = %08X StringA
#define DUI_RGBCOLOR_ATTRIBUTE(attribname, varname, allredraw)       \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = CDuiObject::HexStringToRGBColor(strValue);        \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// HFONT = %04X StringA
#define DUI_FONT_ATTRIBUTE(attribname, varname, allredraw)        \
        if (attribname == strAttribName)                            \
        {                                                           \
            DWORD dwValue = CDuiObject::HexStringToULong(strValue);  \
            varname = DuiSystem::GetFont(LOWORD(dwValue));         \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// Value In {String1 : Value1, String2 : Value2 ...}
#define DUI_ENUM_ATTRIBUTE(attribname, vartype, allredraw)        \
        if (attribname == strAttribName)                            \
        {                                                           \
            vartype varTemp;                                        \
                                                                    \
            hRet = allredraw ? S_OK : S_FALSE;                      \

#define DUI_ENUM_VALUE(enumstring, enumvalue)                     \
            if (strValue == enumstring)                             \
                varTemp = enumvalue;                                \
            else                                                    \

#define DUI_ENUM_END(varname)                                     \
                return E_FAIL;                                      \
                                                                    \
            varname = varTemp;                                      \
        }                                                           \
        else                                                        \

// BkStyle From StringA Key
#define DUI_STYLE_ATTRIBUTE(attribname, varname, allredraw)       \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = DuiSystem::GetStyle(strValue);                  \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else                                                        \

// BkSkin From StringA Key
#define DUI_SKIN_ATTRIBUTE(attribname, varname, allredraw)        \
        if (attribname == strAttribName)                            \
        {                                                           \
            varname = DuiSystem::GetSkin(strValue);                    \
            hRet = allredraw ? S_OK : S_FALSE;                      \
        }                                                           \
        else   


//////////////////////////////////////////////////////////////////////////
// ͼƬ���ԵĶ����ʵ�ִ����,���ڼ�ͼƬ���Ե����ƴ���ʵ��
// ͷ�ļ��еĶ���,����ͼƬ�����ͼƬ��С����,����������ͼƬ��ͼƬ����ӳ�亯��
#define DUI_IMAGE_ATTRIBUTE_DEFINE(imgName)	\
	Image*	m_pImage##imgName;	\
	CSize	m_size##imgName;	\
	BOOL Set##imgName##Bitmap(UINT nResourceID = 0, CString strType= TEXT("PNG"));	\
	BOOL Set##imgName##Bitmap(CString strImage = TEXT(""));	\
	HRESULT OnAttributeImage##imgName(const CString& strValue, BOOL bLoading);	\

// CPP�����е�����ͼƬ��ͼƬ����ӳ�亯��ʵ��,ͼƬ�����ͼƬ��С����ĳ�ʼ����������Ҫ�Լ�д����
#define DUI_IMAGE_ATTRIBUTE_IMPLEMENT(theclass, imgName, imgCount)	\
	BOOL theclass::Set##imgName##Bitmap(UINT nResourceID, CString strType)	\
	{	\
		if(m_pImage##imgName != NULL)	\
		{	\
			delete m_pImage##imgName;	\
			m_pImage##imgName = NULL;	\
		}	\
		\
		if(LoadImageFromIDResource(nResourceID, strType, m_bImageUseECM, m_pImage##imgName))	\
		{	\
			m_size##imgName.SetSize(m_pImage##imgName->GetWidth() / imgCount, m_pImage##imgName->GetHeight());	\
			return true;	\
		}	\
		return false;	\
	}	\
	BOOL theclass::Set##imgName##Bitmap(CString strImage)	\
	{	\
		if(m_pImage##imgName != NULL)	\
		{	\
			delete m_pImage##imgName;	\
			m_pImage##imgName = NULL;	\
		}	\
		\
		if(DuiSystem::Instance()->LoadImageFile(strImage, m_bImageUseECM, m_pImage##imgName))	\
		{	\
			m_size##imgName.SetSize(m_pImage##imgName->GetWidth() / imgCount, m_pImage##imgName->GetHeight());	\
			return true;	\
		}	\
		return false;	\
	}	\
	HRESULT theclass::OnAttributeImage##imgName(const CString& strValue, BOOL bLoading)	\
	{	\
		if (strValue.IsEmpty()) return E_FAIL;	\
		\
		CString strSkin = _T("");	\
		if(strValue.Find(_T("skin:")) == 0)	\
		{	\
			strSkin = DuiSystem::Instance()->GetSkin(strValue);	\
			if (strSkin.IsEmpty()) return E_FAIL;	\
		}else	\
		{	\
			strSkin = strValue;	\
		}	\
		\
		if(strSkin.Find(_T(".")) != -1)	\
		{	\
			CString strImgFile = strSkin;	\
			if(strSkin.Find(_T(":")) != -1)	\
			{	\
				strImgFile = strSkin;	\
			}	\
			if(!Set##imgName##Bitmap(strImgFile))	\
			{	\
				return E_FAIL;	\
			}	\
		}else	\
		{	\
			UINT nResourceID = _ttoi(strSkin);	\
			if(!Set##imgName##Bitmap(nResourceID, TEXT("PNG")))	\
			{	\
				if(!Set##imgName##Bitmap(nResourceID, TEXT("BMP")))	\
				{	\
					return E_FAIL;	\
				}	\
			}	\
		}	\
		\
		return bLoading?S_FALSE:S_OK;	\
	}	\


//////////////////////////////////////////////////////////////////////////
// ���뷽ʽ���ú�
// ����ˮƽ���뷽ʽ�ʹ�ֱ���뷽ʽ
#define DUI_STRING_ALIGN_DEFINE()	\
	StringFormat strFormat;	\
	if(m_uAlignment == Align_Left)	\
	{	\
		strFormat.SetAlignment(StringAlignmentNear);	\
	}else	\
	if(m_uAlignment == Align_Center)	\
	{	\
		strFormat.SetAlignment(StringAlignmentCenter);	\
	}else	\
	if(m_uAlignment == Align_Right)	\
	{	\
		strFormat.SetAlignment(StringAlignmentFar);	\
	}	\
	\
	if(m_uVAlignment == VAlign_Top)	\
	{	\
		strFormat.SetLineAlignment(StringAlignmentNear);	\
	}else	\
	if(m_uVAlignment == VAlign_Middle)	\
	{	\
		strFormat.SetLineAlignment(StringAlignmentCenter);	\
	}else	\
	if(m_uVAlignment == VAlign_Bottom)	\
	{	\
		strFormat.SetLineAlignment(StringAlignmentFar);	\
	}	\


// ���뷽ʽ���ú�,֧������format������,ˮƽ���뷽ʽ������,��ֱ���뷽ʽ������
#define DUI_STRING_ALIGN_DEFINENAME(formatName, formatVarNameH, formatVarNameV)	\
	StringFormat strFormat##formatName;	\
	if(formatVarNameH == Align_Left)	\
	{	\
		strFormat##formatName.SetAlignment(StringAlignmentNear);	\
	}else	\
	if(formatVarNameH == Align_Center)	\
	{	\
		strFormat##formatName.SetAlignment(StringAlignmentCenter);	\
	}else	\
	if(formatVarNameH == Align_Right)	\
	{	\
		strFormat##formatName.SetAlignment(StringAlignmentFar);	\
	}	\
	\
	if(formatVarNameV == VAlign_Top)	\
	{	\
		strFormat##formatName.SetLineAlignment(StringAlignmentNear);	\
	}else	\
	if(formatVarNameV == VAlign_Middle)	\
	{	\
		strFormat##formatName.SetLineAlignment(StringAlignmentCenter);	\
	}else	\
	if(formatVarNameV == VAlign_Bottom)	\
	{	\
		strFormat##formatName.SetLineAlignment(StringAlignmentFar);	\
	}	\


// ��������
enum enumAttrType
{
	enAttrInt = 0,				// int
	enAttrBool,				// bool
	enAttrUInt					// uint
};

// DUI�����������Ϣ�ṹ
struct DuiObjectAttributeInfo
{
	CString strName;	// ������
	enumAttrType attrType;		// ��������
	LPVOID	lpVar;	// ���Թ����ı���
	LPVOID	lpVarSetFunc;	// ���Թ����ı��������ú���
	LPVOID	lpVarGetFunc;// ���Թ����ı����Ļ�ȡ����
};

class CControlBase;
class CDuiHandler;

// DUI����
class CDuiObject
{
public:
	CDuiObject(void);
	virtual ~CDuiObject(void);

	virtual BOOL IsClass(LPCTSTR lpszName) { return FALSE; }	// �Ƿ����
	virtual LPCTSTR GetObjectClass() { return NULL; }		// ��ȡ����
	virtual LPCTSTR BaseObjectClassName() { return NULL; }	// ��ȡ������

	UINT GetID() {return m_uID;};							// ��ȡDUI����ID
	void SetName(CString strName) { m_strName = strName; }	// ����DUI��������
	CString GetName() {return m_strName;};					// ��ȡDUI��������
	BOOL IsThisObject(UINT uID, CString strName);			// �ж��Ƿ�˶���

	BOOL RegisterHandler(CDuiHandler* pDuiHandler);			// ע���¼��������
	void SetDuiHandler(CDuiHandler* pDuiHandler) { m_pDuiHandler = pDuiHandler;	}	// �������¼��������
	CDuiHandler* GetDuiHandler() { return m_pDuiHandler; }	// ��ȡ�¼��������

	virtual LRESULT OnBaseMessage(UINT	uID, UINT Msg, WPARAM wParam, LPARAM lParam) { return 0L; };
	virtual LRESULT OnControlUpdate(CRect rcUpdate, BOOL bUpdate = false, CControlBase *pControlBase = NULL) { return 0L; };
	
	virtual HRESULT SetAttribute(CString strAttribName, CString strValue, BOOL bLoading);
	virtual BOOL LoadAttributesInfo();
	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	virtual BOOL OnInit();

	virtual void SetRect(CRect rc) { m_rc = rc;};
	virtual CRect GetRect() { return m_rc;};

	static void ParseDuiString(CString& strString);
	static ULONG HexStringToULong(LPCTSTR lpszValue, int nSize = -1);
    static Color HexStringToColor(LPCTSTR lpszValue);
	static Color StringToColor(LPCTSTR lpszValue, Color clrDefault = Color(0, 0, 0, 0));
	static COLORREF HexStringToRGBColor(LPCTSTR lpszValue);
	static void ParseKeyCode(LPCTSTR lpszValue, UINT& nChar, UINT& nFlag);

protected:
	UINT	m_uID;					// DUI����ID
	CString	m_strName;				// DUI��������
	CRect	m_rc;					// ����
	CMap<CString,LPCTSTR,DuiObjectAttributeInfo,DuiObjectAttributeInfo&> m_mapDuiAttrInfo;	// ������Ϣ�б�
	CDuiHandler* m_pDuiHandler;		// �¼��������
};

///////////////////////////////////////////////////////////////////////////
// define DUI object static function
typedef LPCTSTR (*DUIFunc_GetClassName)();
typedef LPVOID (*DUIFunc_CheckAndNew)(LPCTSTR lpszName, HWND hWnd, CDuiObject* pDuiObject);
typedef VOID (*DUIFunc_Shutdown)();

// DUI�����Ϣ��
class CDuiObjectInfo
{
public:
	DUIFunc_GetClassName		m_pfGetClassName;	// ��ȡ�ؼ����ĺ���ָ��
	DUIFunc_CheckAndNew	m_pfCheckAndNew;	// ���ʹ����ؼ�����ĺ���ָ��
	DUIFunc_Shutdown			m_pfShutdown;			// �ؼ��������ͷŵĺ���ָ��
};