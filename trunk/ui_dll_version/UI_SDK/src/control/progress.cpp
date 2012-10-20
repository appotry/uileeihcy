#include "stdafx.h"

ProgressCtrlBase::ProgressCtrlBase()
{
	m_nMin  = 0;
	m_nMax  = 100;
	m_nCur  = 75;
	m_nPage = 10;
	m_nLine = 1;
	m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
}

bool ProgressCtrlBase::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	if (false == Control::SetAttribute(mapAttrib,bReload))
		return false;

	ATTRMAP::iterator iter = mapAttrib.find(XML_PROGRESSCTRL_DIRECTION);
	if (mapAttrib.end() != iter )
	{
		String& str = iter->second;
		if ( str == XML_PROGRESSCTRL_DIRECTION_H )
		{
			m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
		}
		else if ( str == XML_PROGRESSCTRL_DIRECTION_V )
		{
			m_eDirectionType = PROGRESS_SCROLL_BOTTOM_2_TOP;
		}
		else
		{
			UI_LOG_WARN(_T("%s  \"%s\" is invalid: %s"), FUNC_NAME, XML_PROGRESSCTRL_DIRECTION, str.c_str() );
		}
		this->m_mapAttribute.erase(XML_PROGRESSCTRL_DIRECTION);
	}
	CRegion4 rBorder(1,1,1,1);
	this->SetBorderRegion(&rBorder);

	return true;
}

SIZE  ProgressCtrlBase::GetAutoSize( HRDC hRDC )
{
	if(NULL != m_pBkgndRender)
		return m_pBkgndRender->GetDesiredSize();
	if(NULL != m_pForegndRender)
		return m_pForegndRender->GetDesiredSize();

	SIZE s = {0,0};
	return s;
}
int ProgressCtrlBase::SetPos(int nPos, bool bUpdate)
{
	int oldPos = m_nCur;

	if (nPos >= m_nMax - m_nPage)
		nPos = m_nMax - m_nPage;

	if (nPos <= m_nMin)
		nPos = m_nMin;

	m_nCur = nPos;

	if (m_nCur != oldPos && bUpdate)
		this->UpdateObject();

	return oldPos;
}

int ProgressCtrlBase::SetPage(int nPage, bool bUpdate)
{
	int nOldPage = m_nPage;
	m_nPage = nPage;

	int nRange = GetRange();

	if (m_nPage < 0)
		m_nPage = 0;
	if (m_nPage > nRange)
		m_nPage = nRange;

	if (nOldPage != m_nPage && bUpdate)
		this->UpdateObject();

	return nOldPage;
}

void ProgressCtrlBase::SetRange(int nLower, int nUpper, bool bUpdate)
{
	m_nMax = nUpper;
	m_nMin = nLower;

	if( m_nCur > m_nMax-m_nPage )
		m_nCur = m_nMax-m_nPage;
	if (m_nCur < m_nMin)
		m_nCur = m_nMin;

	if (bUpdate)
		this->UpdateObject();
}

void ProgressCtrlBase::SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate)
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
		this->UpdateObject();
}

// 进度条控件的类型
int  ProgressCtrlBase::GetProgressStyle()
{
	return m_nStyle & PROGRESS_STYLE_MASK;
}
void ProgressCtrlBase::SetProgressStyle(int n)
{
	if(n > PROGRESS_STYLE_MASK)
		return;

	m_nStyle &= ~PROGRESS_STYLE_MASK;
	m_nStyle |= n;
}


bool ProgressCtrl::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	if (NULL == m_pBkgndRender)
	{
		m_pBkgndRender = RenderFactory::GetRender(RENDER_TYPE_THEME_PROGRESS_BKGND, this);
	}
	if (NULL == m_pForegndRender)
	{
		m_pForegndRender = RenderFactory::GetRender(RENDER_TYPE_THEME_PROGRESS_FOREGND, this);
	}
 
	return true;
}

void ProgressCtrl::OnPaint(IRenderDC* pDC)
{
	if (m_nMax == m_nMin)
		return ;

	if (NULL != m_pForegndRender)
	{
		CRect rcClient;
		this->GetClientRectAsWin32(&rcClient);

		int nX = rcClient.Width() * m_nCur / (m_nMax-m_nMin);
		rcClient.right = nX;

		m_pForegndRender->DrawState(pDC, &rcClient, 0);
	}
}