#pragma once
#include "ControlBase.h"

// ����������ʾģʽ
enum enumProgTextMode
{
	enProgTitleAuto = 0,			// ��ʾ����,���Զ��ж��Ƿ�ٷֱ���ʾ
	enProgTitlePercent,				// ��ʾ����,��ת��Ϊ�ٷֱ���ʾ��ֵ
	enProgTitleNumberUse,			// ��ʾ����,����ʾ������ֵ
	enProgTitleNumberAll,			// ��ʾ����,����ʾ������ֵ������
	enProgTitleOnly					// ֻ��ʾ����
};

class CDuiProgress : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiProgress, _T("progress"))
public:
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject);
	CDuiProgress(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress = 0,
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiProgress(void);

	int SetProgress(int nProgress);
	int GetProgress() { return m_nProgress; }
	void SetMaxProgress(int nMaxProgress) { m_nMaxProgress = nMaxProgress; }
	int GetMaxProgress() { return m_nMaxProgress; }
	BOOL SetRun(BOOL bRun, int nIndex = -1);
	
protected:
	virtual	BOOL OnControlTimer();
	virtual void DrawControl(CDC &dc, CRect rcUpdate);

	HRESULT OnAttributeRun(const CString& strValue, BOOL bLoading);
	
public:
	//��������
	int				m_nProgress;		// ��ǰ����(0-100)
	int				m_nMaxProgress;		// ������ֵ(Ĭ��100)
	int				m_nCount;			// ��ʱ����������
	int				m_nTimerCount;		// ��ʱ���ٴν�������һ��

	int				m_nHeadLength;		// ������ͼƬͷ������

	Color			m_clrText;			// ����������ɫ
	BOOL			m_bShowText;		// �Ƿ���ʾ��������
	enumProgTextMode m_enTextMode;		// ������ʾģʽ

	DUI_IMAGE_ATTRIBUTE_DEFINE(BackGround);	// ���屳��ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(ForeGround);	// ����ǰ��ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("value"), m_nProgress, FALSE)
		DUI_INT_ATTRIBUTE(_T("max-value"), m_nMaxProgress, FALSE)
		DUI_INT_ATTRIBUTE(_T("timer-count"), m_nTimerCount, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("run"), OnAttributeRun)
		DUI_CUSTOM_ATTRIBUTE(_T("img-back"), OnAttributeImageBackGround)
		DUI_CUSTOM_ATTRIBUTE(_T("img-fore"), OnAttributeImageForeGround)
		DUI_INT_ATTRIBUTE(_T("head-len"), m_nHeadLength, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("show-text"), m_bShowText, TRUE)
		DUI_ENUM_ATTRIBUTE(_T("text-mode"), enumProgTextMode, TRUE)
			DUI_ENUM_VALUE(_T("auto"), enProgTitleAuto)
			DUI_ENUM_VALUE(_T("percent"), enProgTitlePercent)
			DUI_ENUM_VALUE(_T("number-use"), enProgTitleNumberUse)
			DUI_ENUM_VALUE(_T("number-all"), enProgTitleNumberAll)
			DUI_ENUM_VALUE(_T("title-only"), enProgTitleOnly)
		DUI_ENUM_END(m_enTextMode)
	DUI_DECLARE_ATTRIBUTES_END()
};