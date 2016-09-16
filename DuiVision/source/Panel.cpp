#include "StdAfx.h"
#include "Panel.h"

#define	SCROLL_V	1	// ��ֱ�������ؼ�ID
#define	SCROLL_H	2	// ˮƽ�������ؼ�ID

CDuiPanel::CDuiPanel(HWND hWnd, CDuiObject* pDuiObject)
			: CControlBaseFont(hWnd, pDuiObject)
{
	m_ulRefCount = 0;
	m_bEnableScroll = TRUE;
	m_nScrollWidth = 8;

	// ��ֱ������
	CRect rcScroll = CRect(0,0,0,0);
	rcScroll.left = rcScroll.right - m_nScrollWidth;

 	m_pControScrollV = new CDuiScrollVertical(hWnd, this, SCROLL_V, rcScroll);
 	m_vecControl.push_back(m_pControScrollV);

	// ˮƽ������
	rcScroll = CRect(0,0,0,0);
	rcScroll.top = rcScroll.bottom - m_nScrollWidth;

 	m_pControScrollH = new CDuiScrollHorizontal(hWnd, this, SCROLL_H, rcScroll);
 	m_vecControl.push_back(m_pControScrollH);

	m_strXmlFile = _T("");
	m_nVirtualHeight = 0;
	m_nVirtualWidth = 0;
	m_nVirtualTop = 0;
	m_nVirtualLeft = 0;

	m_hPluginHandle = NULL;
	m_strPluginFile = _T("");
	m_pDuiPluginObject = NULL;
	m_pIDuiHostWnd = NULL;

	m_bDblClk = true;

	m_bInit = false;
}

CDuiPanel::~CDuiPanel(void)
{
	// �ͷŽ���������Ͷ�̬��
	if(m_pDuiPluginObject != NULL)
	{
		m_pDuiPluginObject->Release();
		m_pDuiPluginObject = NULL;
	}

	if(m_hPluginHandle != NULL)
	{
		FreeLibrary(m_hPluginHandle);
		m_hPluginHandle = NULL;
	}
}

// ���ݿؼ��������ؼ�ʵ��
CControlBase* CDuiPanel::_CreateControlByName(LPCTSTR lpszName)
{
	// ����Ѿ������˴��ھ��,ʹ�ô��ھ�������ؼ�
	HWND hWnd = m_hWnd;
	if(hWnd != NULL)
	{
		return DuiSystem::CreateControlByName(lpszName, hWnd, this);
	}

	// ���Ҹ��Ի���Ĵ��ھ��,ͨ�����Ի����������ؼ�
	CDuiObject* pParentObj = GetParent();
	while((pParentObj != NULL) && (!pParentObj->IsClass(_T("dlg"))) && (!pParentObj->IsClass(_T("popup"))))
	{
		pParentObj = ((CControlBase*)pParentObj)->GetParent();
	}

	if((pParentObj != NULL) && pParentObj->IsClass(_T("dlg")))
	{
		return DuiSystem::CreateControlByName(lpszName, ((CDlgBase*)pParentObj)->GetSafeHwnd(), this);
	}else
	if((pParentObj != NULL) && pParentObj->IsClass(_T("popup")))
	{
		return DuiSystem::CreateControlByName(lpszName, ((CDlgPopup*)pParentObj)->GetSafeHwnd(), this);
	}

	return NULL;
}

// ����XML�ڵ㣬�����ڵ��е�������Ϣ���õ���ǰ�ؼ���������
BOOL CDuiPanel::Load(DuiXmlNode pXmlElem, BOOL bLoadSubControl)
{
	if(!__super::Load(pXmlElem))
	{
		return FALSE;
	}

    // ʹ��XML�ڵ��ʼ��div�ؼ�
	if(pXmlElem != NULL)
	{
		InitUI(m_rc, pXmlElem);
	}

    return TRUE;
}

// ����XML�ļ�
BOOL CDuiPanel::LoadXmlFile(CString strFileName)
{
	DuiXmlDocument xmlDoc;
	DuiXmlNode pDivElem;

	if(!DuiSystem::Instance()->LoadXmlFile(xmlDoc, strFileName))
	{
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("CDuiPanel::LoadXmlFile %s failed"), strFileName);
		return FALSE;
	}

	m_strXmlFile = strFileName;
	pDivElem = xmlDoc.child(_T("div"));
	if(pDivElem == NULL)
	{
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("CDuiPanel::LoadXmlFile %s failed, not found div node"), strFileName);
		return FALSE;
	}

	// ����div�ڵ�����
	if(!Load(pDivElem))
	{
		DuiSystem::LogEvent(LOG_LEVEL_ERROR, _T("CDuiPanel::LoadXmlFile %s failed, load div node fail"), strFileName);
		return FALSE;
	}

	return TRUE;
}

// ��XML���ô�ֱ������ͼƬ��Ϣ����
HRESULT CDuiPanel::OnAttributeImageScrollV(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!m_pControScrollV->SetBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _ttoi(strSkin);
		if(!m_pControScrollV->SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!m_pControScrollV->SetBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML����ˮƽ������ͼƬ��Ϣ����
HRESULT CDuiPanel::OnAttributeImageScrollH(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	// ͨ��Skin��ȡ
	CString strSkin = _T("");
	if(strValue.Find(_T("skin:")) == 0)
	{
		strSkin = DuiSystem::Instance()->GetSkin(strValue);
		if (strSkin.IsEmpty()) return E_FAIL;
	}else
	{
		strSkin = strValue;
	}

	if(strSkin.Find(_T(".")) != -1)	// ����ͼƬ�ļ�
	{
		CString strImgFile = strSkin;
		if(strSkin.Find(_T(":")) != -1)
		{
			strImgFile = strSkin;
		}
		if(!m_pControScrollH->SetBitmap(strImgFile))
		{
			return E_FAIL;
		}
	}else	// ����ͼƬ��Դ
	{
		UINT nResourceID = _ttoi(strSkin);
		if(!m_pControScrollH->SetBitmap(nResourceID, TEXT("PNG")))
		{
			if(!m_pControScrollH->SetBitmap(nResourceID, TEXT("BMP")))
			{
				return E_FAIL;
			}
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML����XML����,����XML�ļ���������Ϊdiv�����ڵ�����
HRESULT CDuiPanel::OnAttributeXml(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	DuiXmlDocument xmlDoc;
	DuiXmlNode pDivElem;

	if(DuiSystem::Instance()->LoadXmlFile(xmlDoc, strValue))
	{
		m_strXmlFile = strValue;
		pDivElem = xmlDoc.child(_T("div"));
		if(pDivElem != NULL)
		{
			// ����div�ڵ�����
			Load(pDivElem);
		}
	}

	return bLoading?S_FALSE:S_OK;
}

// ��XML����plugin����,���ؽ������ļ���̬��
HRESULT CDuiPanel::OnAttributePlugin(const CString& strValue, BOOL bLoading)
{
	if (strValue.IsEmpty()) return E_FAIL;

	HINSTANCE hPluginHandle = NULL;
	LPVOID pPluginObj = NULL;

	//if(DuiSystem::Instance()->LoadPluginFile(strValue, CEncodingUtil::AnsiToUnicode(IID_IDuiPluginPanel), hPluginHandle, pPluginObj))
	if(DuiSystem::Instance()->LoadPluginFile(strValue, IID_IDuiPluginPanel, hPluginHandle, pPluginObj))
	{
		m_strPluginFile = strValue;
		m_hPluginHandle = hPluginHandle;
		m_pDuiPluginObject = (IDuiPluginPanel*)pPluginObj;
		// ��ʼ��������
		UINT nIDTemplate = 0;
		HWND hWnd = NULL;
		CDlgBase* pParentDlg = GetParentDialog();
		if(pParentDlg != NULL)
		{
			nIDTemplate = pParentDlg->GetIDTemplate();
			hWnd = pParentDlg->GetSafeHwnd();
		}
		//m_pDuiPluginObject->OnInit(nIDTemplate, hWnd, CEncodingUtil::UnicodeToAnsi(GetName()), m_rc);
		m_pDuiPluginObject->OnInit(nIDTemplate, hWnd, GetName(), m_rc, &m_xDuiPanel);
	}

	return bLoading?S_FALSE:S_OK;
}

// ��ʼ�����ڿؼ�
void CDuiPanel::InitUI(CRect rcClient, DuiXmlNode pNode)
{
	CRect rcTemp;
	int nStartX = 0;
	int nStartY = 0;
	CControlBase * pControlBase = NULL;

	// �������д��ڿؼ�
	if(pNode)
	{
		m_nVirtualHeight = 0;
		m_nVirtualWidth = 0;
		for (DuiXmlNode pControlElem = pNode.first_child(); pControlElem; pControlElem=pControlElem.next_sibling())
		{
			if(pControlElem != NULL)
			{
				CString strControlName = pControlElem.name();
				CControlBase* pControl = _CreateControlByName(strControlName);
				if(pControl)
				{
					pControl->Load(pControlElem);
					if(pControl->IsClass(CArea::GetClassName()))
					{
						// Area������Ӧ���,����ӵ�Area�б���
						m_vecArea.push_back(pControl);
					}else
					{
						m_vecControl.push_back(pControl);
					}

					CRect rcCtrl = pControl->GetRect();
					if(rcCtrl.bottom > m_nVirtualHeight)
					{
						m_nVirtualHeight = rcCtrl.bottom - m_rc.top;
					}
					if(rcCtrl.right > m_nVirtualWidth)
					{
						m_nVirtualWidth = rcCtrl.right - m_rc.left;
					}
				}
			}
		}

		// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ��ֱ������
		m_pControScrollV->SetVisible(m_nVirtualHeight > m_rc.Height());
		((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange(m_nVirtualHeight);

		// ��Ҫ���ܿ�ȴ�����ʾ����ȲŻ���ʾ��ֱ������
		m_pControScrollH->SetVisible(m_nVirtualWidth > m_rc.Width());
		((CDuiScrollHorizontal*)m_pControScrollH)->SetScrollMaxRange(m_nVirtualWidth);
	}

	m_bInit = true;
}

// ���ÿؼ���λ��
void CDuiPanel::SetControlRect(CRect rc)
{
	m_rc = rc;

	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->SetRect(rc);
	}

	CRect rcTemp;
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			UINT uControlID = pControlBase->GetControlID();
			if(SCROLL_V == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.left = rcTemp.right - m_nScrollWidth;
			}else
			if(SCROLL_H == uControlID)
			{
				rcTemp = m_rc;
				rcTemp.top = rcTemp.bottom - m_nScrollWidth;
			}else
			{
				rcTemp = pControlBase->GetRect();
				if(rcTemp.bottom > m_nVirtualHeight)
				{
					// ˢ��Panel������߶�
					m_nVirtualHeight = rcTemp.bottom - m_rc.top;
				}
				if(rcTemp.right > m_nVirtualWidth)
				{
					// ˢ��Panel��������
					m_nVirtualWidth = rcTemp.right - m_rc.left;
				}
				continue;
			}
			pControlBase->SetRect(rcTemp);			
		}
	}

	// ��Ҫ���ܸ߶ȴ�����ʾ���߶ȲŻ���ʾ������
	m_pControScrollV->SetVisible(m_nVirtualHeight > m_rc.Height());
	((CDuiScrollVertical*)m_pControScrollV)->SetScrollMaxRange(m_nVirtualHeight);

	// ��Ҫ���ܿ�ȴ�����ʾ����ȲŻ���ʾ��ֱ������
	m_pControScrollH->SetVisible(m_nVirtualWidth > m_rc.Width());
	((CDuiScrollHorizontal*)m_pControScrollH)->SetScrollMaxRange(m_nVirtualWidth);
}

// �������ÿؼ��ɼ��Եĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiPanel::SetControlVisible(BOOL bIsVisible)
{
	__super::SetControlVisible(bIsVisible);

	// ����ÿ���ӿؼ���ԭ��Windows�ؼ��Ŀɼ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			if(pControlBase->IsClass(_T("div")) || pControlBase->IsClass(_T("tabctrl")) || pControlBase->IsClass(_T("layout")))
			{
				// ����ӿؼ����������Ϳؼ�,������ӿؼ������ÿɼ��Ժ���
				pControlBase->SetControlVisible(bIsVisible);
			}else
			{
				// �ж��ӿؼ���ǰ�Ƿ�ɼ�,���ݿɼ��������ӿؼ���ԭ���ؼ��Ŀɼ���
				// �����edit�ؼ�,��ʱ����ʾԭ���ؼ�,����tabҳ�л�ʱ���������
				BOOL bVisible = pControlBase->GetVisible();
				if(pControlBase->IsClass(CDuiEdit::GetClassName()))
				{
					bVisible = FALSE;
				}
				pControlBase->SetControlWndVisible(bVisible);
			}
		}
	}

	// ����в��,�����ò���Ŀɼ���
	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->SetVisible(bIsVisible);
	}
}

// �������ÿؼ�����״̬�ĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiPanel::SetControlHide(BOOL bIsHide)
{
	__super::SetControlHide(bIsHide);

	// ����ÿ���ӿؼ���ԭ��Windows�ؼ��Ŀɼ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			if(pControlBase->IsClass(_T("div")) || pControlBase->IsClass(_T("tabctrl")) || pControlBase->IsClass(_T("layout")))
			{
				// ����ӿؼ����������Ϳؼ�,������ӿؼ����������غ���
				pControlBase->SetControlHide(bIsHide);
			}else
			{
				// �ж��ӿؼ���ǰ�Ƿ�ɼ�,���ݿɼ��������ӿؼ���ԭ���ؼ��Ŀɼ���
				pControlBase->SetControlWndVisible(pControlBase->GetVisible());
			}
		}
	}

	// ����в��,�����ò������ʾ״̬(����ӿ��ݲ�֧��SetHide����)
	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->SetVisible(!bIsHide);
	}
}

// �������ÿؼ��Ƿ���õĺ�������Ҫ�����ӿؼ��ĺ���
void CDuiPanel::SetControlDisable(BOOL bIsDisable)
{
	__super::SetControlDisable(bIsDisable);

	// ����ÿ���ӿؼ���ԭ��Windows�ؼ��Ŀɼ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase)
		{
			pControlBase->SetControlDisable(bIsDisable);
		}
	}

	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->SetDisable(bIsDisable);
	}
}

// �������ÿؼ�����״̬�ĺ���
BOOL CDuiPanel::SetControlFocus(BOOL bFocus)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->SetFocus(bFocus);
	}

	return __super::SetControlFocus(bFocus);
}

// ���ؼ�
void CDuiPanel::DrawControl(CDC &dc, CRect rcUpdate)
{
	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->DrawControl(dc, rcUpdate);
	}
}

// ���ӿؼ�(֧�ֹ�����ʾ)
BOOL CDuiPanel::DrawSubControls(CDC &dc, CRect rcUpdate)
{
	// �������Ҫ������,��ֱ��ʹ�ø��ؼ��Ļ�ͼ����
	if(!m_bEnableScroll || ((m_nVirtualHeight <= m_rc.Height()) && (m_nVirtualWidth <= m_rc.Width())))
	{
		return __super::DrawSubControls(dc, rcUpdate);
	}

	BOOL bShowScrollV = m_nVirtualHeight > m_rc.Height();
	BOOL bShowScrollH = m_nVirtualWidth > m_rc.Width();

	// ������ʾλ��
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	int nCurPosV = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRangeV = pScrollV->GetScrollMaxRange();
	int nVirtualTop = (nMaxRangeV > 0) ? nCurPosV*(m_nVirtualHeight-m_rc.Height())/nMaxRangeV : 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ

	CDuiScrollHorizontal* pScrollH = (CDuiScrollHorizontal*)m_pControScrollH;
	int nCurPosH = pScrollH->GetScrollCurrentPos();	// ��ǰtopλ��
	int nMaxRangeH = pScrollH->GetScrollMaxRange();
	int nVirtualLeft = (nMaxRangeH > 0) ? nCurPosH*(m_nVirtualWidth-m_rc.Width())/nMaxRangeH : 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ

	// ���ݹ�����λ�û��ӿؼ�
	// ��ʼ��������ʾdc,�����Ʊ�����������ʾdc
	CBitmap	virtualBitmap;
	CDC virtualDC;
	virtualDC.CreateCompatibleDC(&dc);
	virtualBitmap.CreateCompatibleBitmap(&dc,
				bShowScrollH ? (m_rc.left + m_nVirtualWidth) : (m_rc.right - (bShowScrollV ? m_nScrollWidth : 0)),
				bShowScrollV ? (m_rc.top + m_nVirtualHeight) : (m_rc.bottom - (bShowScrollH ? m_nScrollWidth : 0)) );
	CBitmap* pOldVirtualBitmap = virtualDC.SelectObject(&virtualBitmap);
	virtualDC.BitBlt(m_rc.left+nVirtualLeft, m_rc.top+nVirtualTop,
				m_rc.Width()-(bShowScrollV ? m_nScrollWidth : 0),
				m_rc.Height()-(bShowScrollH ? m_nScrollWidth : 0),
				&dc, m_rc.left, m_rc.top, SRCCOPY);	// �������Ƶ�������ʾdc

	// ������������ʾ�������
	rcUpdate.OffsetRect(nVirtualLeft, nVirtualTop);

	// ���ӿؼ�������dc
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && pControlBase->GetVisible()
			&& (pControlBase != m_pControScrollV) && (pControlBase != m_pControScrollH))
		{
			// �������λ�÷����˱仯,��Ҫ�ػ������ӿؼ�,��������ٶȺ���
			if(m_nVirtualTop != nVirtualTop)
			{
				pControlBase->UpdateControl(true);
			}
			pControlBase->Draw(virtualDC, rcUpdate);		
		}
	}

	// ��������,����������������Ҫ����
	m_pControScrollV->Draw(dc, m_pControScrollV->GetRect());
	m_pControScrollH->Draw(dc, m_pControScrollH->GetRect());

	m_nVirtualTop = nVirtualTop;
	m_nVirtualLeft = nVirtualLeft;

	// ������ʾdc���Ƶ�dc(���������ֲ��ø���)
	dc.BitBlt(m_rc.left, m_rc.top, m_rc.Width()-(bShowScrollV ? m_nScrollWidth : 0),
			m_rc.Height()-(bShowScrollH ? m_nScrollWidth : 0),
			&virtualDC, m_rc.left + nVirtualLeft, m_rc.top + nVirtualTop, SRCCOPY);

	// �ͷ�������ʾdc
	virtualDC.SelectObject(pOldVirtualBitmap);
	virtualDC.DeleteDC();
	virtualBitmap.DeleteObject();

	return TRUE;
}

// ���ÿؼ�ˢ�±�ʶ
void CDuiPanel::SetUpdate(BOOL bUpdate, COLORREF clr/* = 0*/)
{
	__super::SetUpdate(bUpdate, clr);

	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->SetUpdate(bUpdate, clr);
	}
}

// �������任
BOOL CDuiPanel::OnMousePointChange(CPoint& point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnMousePointChange(point);
	}

	if(m_bEnableScroll && ((m_nVirtualHeight > m_rc.Height()) || (m_nVirtualWidth > m_rc.Width())))
	{
		// ������ʾλ��
		CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
		int nCurPosV = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
		int nMaxRangeV = pScrollV->GetScrollMaxRange();
		int nVirtualTop = (nMaxRangeV > 0) ? nCurPosV*(m_nVirtualHeight-m_rc.Height())/nMaxRangeV : 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ

		CDuiScrollHorizontal* pScrollH = (CDuiScrollHorizontal*)m_pControScrollH;
		int nCurPosH = pScrollH->GetScrollCurrentPos();	// ��ǰtopλ��
		int nMaxRangeH = pScrollH->GetScrollMaxRange();
		int nVirtualLeft = (nMaxRangeH > 0) ? nCurPosH*(m_nVirtualWidth-m_rc.Width())/nMaxRangeH : 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ

		point.Offset(nVirtualLeft, nVirtualTop);

		return TRUE;
	}

	return FALSE;
}

// �ж�����Ƿ��ڿؼ�����Ӧ������
BOOL CDuiPanel::OnCheckMouseResponse(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnCheckMouseResponse(nFlags, point);
	}
	return TRUE;
}

// ����ƶ��¼�����
BOOL CDuiPanel::OnControlMouseMove(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnMouseMove(nFlags, point);
	}
	return FALSE;
}

// �����������¼�����
BOOL CDuiPanel::OnControlLButtonDown(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnLButtonDown(nFlags, point);
	}
	return FALSE;
}

// �������ſ��¼�����
BOOL CDuiPanel::OnControlLButtonUp(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnLButtonUp(nFlags, point);
	}
	return FALSE;
}

// ������˫���¼�����
BOOL CDuiPanel::OnControlLButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnLButtonDblClk(nFlags, point);
	}
	return FALSE;
}

// ����Ҽ������¼�����
BOOL CDuiPanel::OnControlRButtonDown(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnRButtonDown(nFlags, point);
	}
	return FALSE;
}

// ����Ҽ��ſ��¼�����
BOOL CDuiPanel::OnControlRButtonUp(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnRButtonUp(nFlags, point);
	}
	return FALSE;
}

// ����Ҽ�˫���¼�����
BOOL CDuiPanel::OnControlRButtonDblClk(UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnRButtonDblClk(nFlags, point);
	}
	return FALSE;
}

// �����¼�����
BOOL CDuiPanel::OnControlKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnKeyDown(nChar, nRepCnt, nFlags);
	}
	return FALSE;
}

// ��ʱ���¼�����
BOOL CDuiPanel::OnControlTimer()
{
	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnTimer(0, _T(""));
	}
	return __super::OnControlTimer();
}

// �����¼�����
BOOL CDuiPanel::OnControlScroll(BOOL bVertical, UINT nFlags, CPoint point)
{
	if(m_pDuiPluginObject)
	{
		m_pDuiPluginObject->OnScroll(bVertical, nFlags, point);
	}

	if(!m_bEnableScroll || (m_nVirtualHeight < m_rc.Height()))
	{
		return false;
	}

	// ���¹�����,��ˢ�½���
	CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
	if(pScrollV->ScrollRow((nFlags == SB_LINEDOWN) ? 1 : -1))
	{
		UpdateControl(true);
		// ˢ�¹�����
		pScrollV->UpdateControl(true);
	}

	return true;
}

// ��Ϣ��Ӧ
LRESULT CDuiPanel::OnBaseMessage(UINT uID, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return OnMessage(uID, uMsg, wParam, lParam);
}

// �ؼ�����ˢ��
LRESULT CDuiPanel::OnControlUpdate(CRect rcUpdate, BOOL bUpdate, CControlBase *pUpdateControlBase) 
{
	if(pUpdateControlBase == NULL) return 0;

	if(bUpdate)
	{
		pUpdateControlBase->SetUpdate(FALSE);
	}

	int nVirtualTop = 0;
	if(m_nVirtualHeight > m_rc.Height())
	{
		// ������ʾλ��
		CDuiScrollVertical* pScrollV = (CDuiScrollVertical*)m_pControScrollV;
		int nCurPos = pScrollV->GetScrollCurrentPos();	// ��ǰtopλ��
		int nMaxRange = pScrollV->GetScrollMaxRange();
		nVirtualTop = (nMaxRange > 0) ? nCurPos*(m_nVirtualHeight-m_rc.Height())/nMaxRange : 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ
	}

	int nVirtualLeft = 0;
	if(m_nVirtualWidth > m_rc.Width())
	{
		// ������ʾλ��
		CDuiScrollHorizontal* pScrollH = (CDuiScrollHorizontal*)m_pControScrollH;
		int nCurPosH = pScrollH->GetScrollCurrentPos();	// ��ǰtopλ��
		int nMaxRangeH = pScrollH->GetScrollMaxRange();
		nVirtualLeft = (nMaxRangeH > 0) ? nCurPosH*(m_nVirtualLeft-m_rc.Width())/nMaxRangeH : 0;	// ��ǰ��ʾ��������ͼƬ��ʲôλ�ÿ�ʼ��ͼƬ
	}

	CRect rcAllUpDate = rcUpdate;

	if(m_bInit)
	{
		BOOL bFind = false;

		for (size_t i = 0; i < m_vecControl.size(); i++)
		{
			CControlBase * pControlBase = m_vecControl.at(i);
			if (pControlBase)
			{
				if(bFind)
				{
					if (pControlBase->GetVisible() && !(pControlBase->GetRect() & rcUpdate).IsRectEmpty())
					{
						rcAllUpDate |= pControlBase->GetRect();
						pControlBase->SetUpdate(FALSE);
					}
				}
				else if(pControlBase == pUpdateControlBase)
				{
					bFind = true;
				}
			}
		}
	}

	// ���չ���λ���޸ĸ�������
	rcAllUpDate.OffsetRect(-nVirtualLeft, -nVirtualTop);
	rcAllUpDate |= m_rc;

	InvalidateRect(&rcAllUpDate);

	return 0L; 
};

// ��Ϣ����
LRESULT CDuiPanel::OnMessage(UINT uID, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	// ����ÿ���ؼ������ĸ��ؼ��Ǵ�ID��������¼��Ĵ���
	for (size_t i = 0; i < m_vecControl.size(); i++)
	{
		CControlBase * pControlBase = m_vecControl.at(i);
		if (pControlBase && (pControlBase->GetControlID() == uID))
		{
			return pControlBase->OnMessage(uID, Msg, wParam, lParam);
		}
	}

	if(m_pDuiPluginObject)
	{
		return m_pDuiPluginObject->OnMessage(uID, Msg, wParam, lParam);
	}

	// ����¼�δ����,����ø������Ϣ����,���ջ��͸����¼����������д���
	return __super::OnMessage(uID, Msg, wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
// ����������ڹ���ʵ��
INTERFACE_IMPLEMENT(DuiPanel)

// ��ȡӦ�ó�����
STDMETHODIMP_(CString)
CDuiPanel::XDuiPanel::GetAppName()
{
	return DuiSystem::Instance()->GetString(_T("APP_NAME"));
}

// ��ȡƽ̨·��
STDMETHODIMP_(CString)
CDuiPanel::XDuiPanel::GetPlatPath()
{
	return DuiSystem::GetExePath();
}

// ��ȡƽ̨�汾
STDMETHODIMP_(CString)
CDuiPanel::XDuiPanel::GetPlatVersion()
{
	return DuiSystem::Instance()->GetString(_T("APP_VER"));
}

// ��ȡ��ǰ����
STDMETHODIMP_(int)
CDuiPanel::XDuiPanel::GetCurrentLanguage()
{
	return DuiSystem::Instance()->GetCurrentLanguage();
}

// ��ȡƽ̨ע����·��
STDMETHODIMP_(CString)
CDuiPanel::XDuiPanel::GetPlatRegistry()
{
	return DuiSystem::Instance()->GetConfig(_T("APP_REGISTRY"));
}

// ��ȡƽ̨��Ȩ�ַ���
STDMETHODIMP_(CString)
CDuiPanel::XDuiPanel::GetPlatCopyRight()
{
	return DuiSystem::Instance()->GetString(_T("APP_COPYRIGHT"));
}

// ��ȡ��ҳURL
STDMETHODIMP_(CString)
CDuiPanel::XDuiPanel::GetPlatHomeURL()
{
	return DuiSystem::Instance()->GetString(_T("APP_URL_HOME"));
}

// ��ȡ����URL
STDMETHODIMP_(CString)
CDuiPanel::XDuiPanel::GetPlatDownloadURL()
{
	return DuiSystem::Instance()->GetString(_T("APP_URL_DOWNLOAD"));
}

// ������Ϣ
STDMETHODIMP_(int)
CDuiPanel::XDuiPanel::SendMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	// δʵ��
	return 0;
}

// ƽ̨����Ϣ����
STDMETHODIMP_(int)
CDuiPanel::XDuiPanel::ProcessMessage(CVciMessage* pIn, CVciMessage* ppOut)
{
	// δʵ��
	return 0;
}

// ����ƽ̨����
STDMETHODIMP_(int)
CDuiPanel::XDuiPanel::SendCommand(int nCmd, WPARAM wParam, LPARAM lParam)
{
	// δʵ��
	return 0;
}

// ����ƽ̨����
STDMETHODIMP_(BOOL)
CDuiPanel::XDuiPanel::SendCommand(int nCmd, WPARAM wParam, LPARAM lParam, LPVOID lpResult)
{
	return FALSE;
}

// ��ȡDuiVisionӦ��ID
STDMETHODIMP_(int)
CDuiPanel::XDuiPanel::GetAppID()
{
	return DuiSystem::Instance()->GetAppID();
}

// ��ȡ���ڱ�����Ϣ
STDMETHODIMP_(BOOL)
CDuiPanel::XDuiPanel::GetWindowBkInfo(int& nType, int& nIDResource, COLORREF& clr, CString& strImgFile)
{
	return DuiSystem::Instance()->GetWindowBkInfo(nType, nIDResource, clr, strImgFile);
}

// ���ô��ڱ�����Ϣ
STDMETHODIMP_(BOOL)
CDuiPanel::XDuiPanel::SetWindowBkInfo(int nType, int nIDResource, COLORREF clr, LPCTSTR lpszImgFile)
{
	return DuiSystem::Instance()->SetWindowBkInfo(nType, nIDResource, clr, lpszImgFile);
}

// ����ת��Ϊ��Ļ����
STDMETHODIMP_(void)
CDuiPanel::XDuiPanel::ClientToScreen(LPPOINT lpPoint)
{
	CDuiPanel *pObj = GET_INTERFACE_OBJECT(DuiPanel);
	pObj->ClientToScreen(lpPoint);
}

// ��ȡ�������ڵľ��
STDMETHODIMP_(HWND)
CDuiPanel::XDuiPanel::GetPaintHWnd()
{
	CDuiPanel *pObj = GET_INTERFACE_OBJECT(DuiPanel);
	return pObj->GetPaintHWnd();
}

// ����Tooltip
STDMETHODIMP_(void)
CDuiPanel::XDuiPanel::SetTooltip(int nCtrlID, LPCTSTR lpszTooltip, CRect rect, int nTipWidth)
{
	CDuiPanel *pObj = GET_INTERFACE_OBJECT(DuiPanel);
	pObj->SetTooltip(pObj, lpszTooltip, rect, FALSE, nTipWidth);
}

// ���Tooltip
STDMETHODIMP_(void)
CDuiPanel::XDuiPanel::ClearTooltip()
{
	CDuiPanel *pObj = GET_INTERFACE_OBJECT(DuiPanel);
	pObj->ClearTooltip();
}

// ���õ�ǰTooltip�ؼ�ID
STDMETHODIMP_(void)
CDuiPanel::XDuiPanel::SetTooltipCtrlID(int nTooltipCtrlID)
{
	CDuiPanel *pObj = GET_INTERFACE_OBJECT(DuiPanel);
	if(pObj)
	{
		pObj->SetTooltipCtrlID(nTooltipCtrlID);
	}
}

// ��ȡ��ǰTooltip�ؼ�ID
STDMETHODIMP_(int)
CDuiPanel::XDuiPanel::GetTooltipCtrlID()
{
	CDuiPanel *pObj = GET_INTERFACE_OBJECT(DuiPanel);
	if(pObj)
	{
		return pObj->GetTooltipCtrlID();
	}

	return 0;
}
