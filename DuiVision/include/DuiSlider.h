#pragma once
#include "ControlBase.h"


class CDuiSlider : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiSlider, _T("slider"))
public:
	CDuiSlider(HWND hWnd, CDuiObject* pDuiObject);
	CDuiSlider(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress = 0,
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiSlider(void);

	int SetProgress(int nProgress);
	int GetProgress() { return m_nProgress; }
	void SetMaxProgress(int nMaxProgress) { m_nMaxProgress = nMaxProgress; }
	int GetMaxProgress() { return m_nMaxProgress; }

	int MoveThumbPos(CPoint point);
	
	virtual void SetControlRect(CRect rc);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	
public:
	enumButtonState m_enButtonState;	// ���״̬
	BOOL			m_bHover;			// �Ƿ�����ȵ�״̬
	int				m_nProgress;		// ��ǰ����(0-100)
	int				m_nMaxProgress;		// ������ֵ(Ĭ��100)
	int				m_nSliderHeight;	// �������߶�
	int				m_nThumbWidth;		// ������
	int				m_nThumbHeight;		// ����߶�
	int				m_nThumbTop;		// ���鶥��λ��
	int				m_nHeadLength;		// ������ͼƬͷ������
	CRect			m_rcThumb;			// ��������

	Color			m_clrText;			// ����������ɫ
	BOOL			m_bShowText;		// �Ƿ���ʾ��������

	DUI_IMAGE_ATTRIBUTE_DEFINE(BackGround);	// ���屳��ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(ForeGround);	// ����ǰ��ͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(Thumb);	// ���廬��ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_INT_ATTRIBUTE(_T("value"), m_nProgress, FALSE)
		DUI_INT_ATTRIBUTE(_T("max-value"), m_nMaxProgress, FALSE)
		DUI_CUSTOM_ATTRIBUTE(_T("img-back"), OnAttributeImageBackGround)
		DUI_CUSTOM_ATTRIBUTE(_T("img-fore"), OnAttributeImageForeGround)
		DUI_CUSTOM_ATTRIBUTE(_T("img-thumb"), OnAttributeImageThumb)
		DUI_INT_ATTRIBUTE(_T("slider-height"), m_nSliderHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("thumb-width"), m_nThumbWidth, FALSE)
		DUI_INT_ATTRIBUTE(_T("thumb-height"), m_nThumbHeight, FALSE)
		DUI_INT_ATTRIBUTE(_T("thumb-top"), m_nThumbTop, FALSE)
		DUI_INT_ATTRIBUTE(_T("head-len"), m_nHeadLength, FALSE)
		DUI_COLOR_ATTRIBUTE(_T("crtext"), m_clrText, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("show-text"), m_bShowText, TRUE)
	DUI_DECLARE_ATTRIBUTES_END()
};