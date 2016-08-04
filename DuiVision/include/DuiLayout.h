#pragma once
#include "ControlBase.h"
#include "Panel.h"

// Layoutģʽ
enum LAYOUT_TYPE
{
	LAYOUT_TYPE_HORIZONTAL = 0,			// ˮƽ����
	LAYOUT_TYPE_VERTICAL,				// ��ֱ����
};

// Layout��Ϣ�ṹ����
struct LayoutItemInfo
{
	BOOL			bVisible;	// div�Ƿ�ɼ�
	DUI_POSITION_ITEM	pos;	// λ����Ϣ
	int					nMinPos;	// λ����Сֵ
	int					nMaxPos;	// λ�����ֵ
	int					nPos;			// ʵ�ʵ�λ��ֵ
	CDuiPanel*		pControlPanel;	// Layout��Ӧ��div�����ؼ�
	CRect			rcSplit;			// �ָ�������
	CRect			rcThumb;		// ��������
};

class CDuiLayout : public CControlBaseFont
{
	DUIOBJ_DECLARE_CLASS_NAME(CDuiLayout, _T("layout"))
public:
	CDuiLayout(HWND hWnd, CDuiObject* pDuiObject);
	CDuiLayout(HWND hWnd, CDuiObject* pDuiObject, UINT uControlID, CRect rc, int nProgress = 0,
		BOOL bIsVisible = TRUE, BOOL bIsDisable = FALSE);
	virtual ~CDuiLayout(void);

	LayoutItemInfo* GetLayoutItemInfo(int nItem);
	int GetLayoutPos(LayoutItemInfo* pLayoutInfo);
	int SetSplitPos(int nSplitItem, int nSplitPos);

	int MoveSplitPos(int nSplitItem, CPoint point);
	
	virtual BOOL Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl = TRUE);
	virtual void SetControlRect(CRect rc);

	virtual void SetControlVisible(BOOL bIsVisible);

	virtual BOOL OnControlMouseMove(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonDown(UINT nFlags, CPoint point);
	virtual BOOL OnControlLButtonUp(UINT nFlags, CPoint point);

	virtual void DrawControl(CDC &dc, CRect rcUpdate);
	
public:
	LAYOUT_TYPE		m_nLayoutType;			// Layout����
	vector<LayoutItemInfo>		m_vecItemInfo;	// Layout��Ϣ�б�
	BOOL					m_bInit;				// �Ƿ��ʼ�����
	enumButtonState m_enButtonState;	// ���״̬
	BOOL			m_bHover;			// �Ƿ�����ȵ�״̬
	int					m_nHoverSplit;	// ����϶��ķָ�������
	int				m_nSplitWidth;		// �ָ��߿��
	BOOL		m_bSplitImm;			// ����ƶ��ָ����Ƿ�������Ч
	//int				m_nThumbWidth;		// ������
	//int				m_nThumbHeight;		// ����߶�
	//int				m_nThumbPos;		// ���鶥��λ��

	DUI_IMAGE_ATTRIBUTE_DEFINE(Split);	// ����SplitͼƬ
	DUI_IMAGE_ATTRIBUTE_DEFINE(Thumb);	// ����ǰ��ͼƬ
	DUI_DECLARE_ATTRIBUTES_BEGIN()
		DUI_ENUM_ATTRIBUTE(_T("layout-type"), LAYOUT_TYPE, TRUE)
            DUI_ENUM_VALUE(_T("horizontal"), LAYOUT_TYPE_HORIZONTAL)
            DUI_ENUM_VALUE(_T("vertical"), LAYOUT_TYPE_VERTICAL)
        DUI_ENUM_END(m_nLayoutType)
		DUI_CUSTOM_ATTRIBUTE(_T("img-split"), OnAttributeImageSplit)
		DUI_CUSTOM_ATTRIBUTE(_T("img-thumb"), OnAttributeImageThumb)
		DUI_INT_ATTRIBUTE(_T("split-width"), m_nSplitWidth, FALSE)
		DUI_BOOL_ATTRIBUTE(_T("split-imm"), m_bSplitImm, TRUE)
		//DUI_INT_ATTRIBUTE(_T("thumb-width"), m_nThumbWidth, FALSE)
		//DUI_INT_ATTRIBUTE(_T("thumb-height"), m_nThumbHeight, FALSE)
		//DUI_INT_ATTRIBUTE(_T("thumb-pos"), m_nThumbPos, FALSE)
	DUI_DECLARE_ATTRIBUTES_END()
};