#include "stdafx.h"
#include "ledctrl.h"

namespace UI
{

LEDCtrl::LEDCtrl()
{
    m_pILEDCtrl = NULL;
	m_pImagelist = NULL;
}

void  LEDCtrl::ResetAttribute()
{
	DO_PARENT_PROCESS(ILEDCtrl, IControl);

	m_pILEDCtrl->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, false);
	m_pImagelist = NULL;
}
void  LEDCtrl::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
	DO_PARENT_PROCESS(ILEDCtrl, IControl);

    IRenderBase* pForegndRender = m_pILEDCtrl->GetForeRender();
	if (NULL == m_pImagelist && NULL != pForegndRender)
    {
        m_pImagelist = (IImageListRender*)UISendMessage(pForegndRender, UI_WM_QUERYINTERFACE, (WPARAM)&uiiidof(IImageListRender));
    }

	const TCHAR* szText = pMapAttr->GetAttr(XML_LEDCTRL_INDEXMAP, true);
	if (szText)
		this->SetIndexMap(szText);

	if (false == bReload)
	{
		szText = pMapAttr->GetAttr(XML_TEXT, true);
		if (szText)
			this->SetText(szText, false);
        else
            this->SetText(_T(""), false);
	}
}

void  LEDCtrl::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = m_pILEDCtrl->GetNonClientW();
    pSize->cy = m_pILEDCtrl->GetNonClientH();

	if (NULL == m_pImagelist)
		return;

    pSize->cx += m_pImagelist->GetItemWidth() * (int)m_vecIndex.size();
	pSize->cy += m_pImagelist->GetItemHeight();
}


void LEDCtrl::OnLButtonUp(UINT nFlags, POINT point)
{
    IObject* pParent = m_pILEDCtrl->GetParentObject();
    if (NULL == pParent)
        return;

    POINT ptObj = {0,0};
    pParent->WindowPoint2ObjectClientPoint(&point, &ptObj);

    // UI_WM_HITTEST���ж����Լ���m_rcParent�Ľ��������Ҫ����������ת����parent��client����
    UINT nHitTest = UISendMessage(m_pILEDCtrl, UI_WM_HITTEST, ptObj.x, ptObj.y);
    if (HTNOWHERE != nHitTest)
    {
        this->OnClicked(&point);   // ��ע������DoNotify���ܵ��µ�ǰpress hover�������ı䣬ʹ�ñ��ؼ���ʧˢ��
    }
}


void LEDCtrl::OnClicked(POINT* pt)
{
    // ֪ͨ��Ϣ
    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode   = UI_BN_CLICKED;
    msg.wParam  = NULL; //(WPARAM)pt;
    msg.lParam  = NULL;
    msg.pMsgFrom = this->m_pILEDCtrl;

    msg.pMsgTo = NULL;
    m_pILEDCtrl->DoNotify(&msg);
}

// ����ͼƬ�е�ÿһ���Ӧ���ַ�
void  LEDCtrl::SetIndexMap(const TCHAR* szText)
{
	m_mapIndex.clear();
    if (NULL == szText)
        return;

	int nSize = _tcslen(szText);
	for (int i = 0; i < nSize; i++)
	{
		m_mapIndex[szText[i]] = i;
	}
}

// ����Ҫ��ʾ���ı�
void LEDCtrl::SetText(const TCHAR* szText, bool bUpdate)
{
	m_vecIndex.clear();
    m_strText.clear();
    if (szText)
    	m_strText = szText;

	int nLength = m_strText.length();
	for (int i = 0; i < nLength; i++)
	{
		if (m_mapIndex.count(m_strText[i]))
		{
			m_vecIndex.push_back(m_mapIndex[m_strText[i]]);
		}
		else
		{
			m_vecIndex.push_back(-1);   //  ��Ч�ַ�
		}
	}	
	if (bUpdate)
		m_pILEDCtrl->UpdateObject();
}

void LEDCtrl::OnPaint(IRenderTarget* pRenderTarget)
{
    IRenderBase* pForegndRender = m_pILEDCtrl->GetForeRender();
    if (!pForegndRender || !m_pImagelist)
        return;

	int nSize = (int)m_vecIndex.size();
	CRect  rcClient;
	m_pILEDCtrl->GetClientRectAsWin32(&rcClient);
	
	CRect  rcItem = rcClient;
	rcItem.right = rcItem.left;
	rcItem.left = rcItem.right - m_pImagelist->GetItemWidth(); 

	for (int i = 0; i < nSize; i++)
	{
		rcItem.OffsetRect(m_pImagelist->GetItemWidth(), 0);
		int nIndex = m_vecIndex[i];
		if (-1 == nIndex)
			continue;

		pForegndRender->DrawState(pRenderTarget, &rcItem, nIndex);
	}	
}

}