#include "stdafx.h"
#include "progressctrl.h"

namespace UI
{

ProgressCtrl::ProgressCtrl()
{
    m_pIProgressCtrl = NULL;

	m_nMin  = 0;
	m_nMax  = 100;
	m_nCur  = 0;
	m_nPage = 10;
	m_nLine = 1;
	m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
}

HRESULT  ProgressCtrl::FinalConstruct(IUIApplication* )
{
    DO_PARENT_PROCESS(IProgressCtrl, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    if (m_pIProgressCtrl->GetUIApplication()->IsDesignMode())
    {
        SetPos(50, false);
    }
    return S_OK;
}
void  ProgressCtrl::ResetAttribute()
{
    DO_PARENT_PROCESS(IProgressCtrl, IControl);
    m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
}

void ProgressCtrl::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
	DO_PARENT_PROCESS(IProgressCtrl, IControl);

	const TCHAR* szText = pMapAttr->GetAttr(XML_PROGRESSCTRL_DIRECTION, true);
	if (szText)
	{
		if (0 == _tcscmp(szText, XML_PROGRESSCTRL_DIRECTION_LEFTRIGHT))
		{
			m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
		}
		else if (0 == _tcscmp(szText, XML_PROGRESSCTRL_DIRECTION_BOTTOMTOP))
		{
			m_eDirectionType = PROGRESS_SCROLL_BOTTOM_2_TOP;
		}
		else if (0 == _tcscmp(szText, XML_PROGRESSCTRL_DIRECTION_RIGHTLEFT))
		{
			m_eDirectionType = PROGRESS_SCROLL_RIGHT_2_LEFT;
		}
		else if (0 == _tcscmp(szText, XML_PROGRESSCTRL_DIRECTION_TOPBOTTOM))
		{
			m_eDirectionType = PROGRESS_SCROLL_TOP_2_BOTTOM;
		}
		else
		{
			UI_LOG_WARN(_T("%s  \"%s\" is invalid: %s"), FUNC_NAME, XML_PROGRESSCTRL_DIRECTION, szText);
		}
	}

    if (NULL == m_pIProgressCtrl->GetBkRender())
    {
        IRenderBase* p = NULL;
        m_pIProgressCtrl->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_PROGRESS_BKGND, m_pIProgressCtrl, &p);
        if (p)
        {
            p->SetAttribute(pMapAttr, XML_ALIGN_BOTTOM, true);
            m_pIProgressCtrl->SetBkgndRender(p);
            SAFE_RELEASE(p);
        }
    }
    if (NULL == m_pIProgressCtrl->GetForeRender())
    {
        IRenderBase* p = NULL;
        m_pIProgressCtrl->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_PROGRESS_FOREGND, m_pIProgressCtrl, &p);
        if (p)
        {
            p->SetAttribute(pMapAttr, XML_ALIGN_BOTTOM, true);
            m_pIProgressCtrl->SetForegndRender(p);
            SAFE_RELEASE(p);
        }
    }
}

void  ProgressCtrl::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
    IRenderBase* p = m_pIProgressCtrl->GetBkRender();
	if(p)
    {
		*pSize = p->GetDesiredSize();
    }

    p = m_pIProgressCtrl->GetForeRender();
	if(p)
    {
		*pSize = p->GetDesiredSize();
    }
}

int ProgressCtrl::SetPos(int nPos, bool bUpdate)
{
	int oldPos = m_nCur;

	if (nPos >= m_nMax)
		nPos = m_nMax;

	if (nPos <= m_nMin)
		nPos = m_nMin;

	m_nCur = nPos;

	if (m_nCur != oldPos && bUpdate)
		m_pIProgressCtrl->UpdateObject();

	return oldPos;
}

int ProgressCtrl::SetPage(int nPage, bool bUpdate)
{
	int nOldPage = m_nPage;
	m_nPage = nPage;

	int nRange = GetRange();

	if (m_nPage < 0)
		m_nPage = 0;
	if (m_nPage > nRange)
		m_nPage = nRange;

	if (nOldPage != m_nPage && bUpdate)
		m_pIProgressCtrl->UpdateObject();

	return nOldPage;
}

void ProgressCtrl::SetRange(int nLower, int nUpper, bool bUpdate)
{
	m_nMax = nUpper;
	m_nMin = nLower;

	if( m_nCur > m_nMax )
		m_nCur = m_nMax;
	if (m_nCur < m_nMin)
		m_nCur = m_nMin;

	if (bUpdate)
		m_pIProgressCtrl->UpdateObject();
}

void ProgressCtrl::SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate)
{
	if (NULL == lpsi || 0 == lpsi->fMask)
		return;

	if (lpsi->fMask & SIF_RANGE)
		this->SetRange(lpsi->nMin, lpsi->nMax, false);

	if (lpsi->fMask & SIF_POS)
		this->SetPos(lpsi->nPos, false);

	if (lpsi->fMask & SIF_PAGE)
		this->SetPage(lpsi->nPage, false);

	if (bUpdate)
		m_pIProgressCtrl->UpdateObject();
}

// 进度条控件的类型
int  ProgressCtrl::GetProgressStyle()
{
    return GETCONTROLSUBTYPE(m_pIProgressCtrl->GetStyleEx());
}
void ProgressCtrl::SetProgressStyle(int n)
{
    m_pIProgressCtrl->SetStyleEx(MAKECONTROLSUBTYPE(m_pIProgressCtrl->GetStyleEx(), n));
}


void ProgressCtrl::OnPaint(IRenderTarget* pDC)
{
	if (m_nMax == m_nMin)
		return ;

    IRenderBase* pForegndRender = m_pIProgressCtrl->GetForeRender();
	if (pForegndRender)
	{
		CRect rc;
		m_pIProgressCtrl->GetClientRectAsWin32(&rc);

		switch(m_eDirectionType)
		{
		case PROGRESS_SCROLL_LEFT_2_RIGHT:
			{
				int nX = rc.Width() * m_nCur / (m_nMax-m_nMin);
				rc.right = nX;
			}
			break;

		case PROGRESS_SCROLL_RIGHT_2_LEFT:
			{
				int nX = rc.Width() * m_nCur / (m_nMax-m_nMin);
				rc.left = rc.right - nX;
			}
			break;

		case PROGRESS_SCROLL_TOP_2_BOTTOM:
			{
				int nY = rc.Height() * m_nCur / (m_nMax-m_nMin);
				rc.bottom = nY;
			}
			break;

		case PROGRESS_SCROLL_BOTTOM_2_TOP:
			{
				int nY = rc.Height() * m_nCur / (m_nMax-m_nMin);
				rc.top = rc.bottom - nY;
			}
			break;
		}

		pForegndRender->DrawState(pDC, &rc, 0);
	}
}

}