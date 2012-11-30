#include "stdafx.h"

ProgressCtrlBase::ProgressCtrlBase()
{
	m_nMin  = 0;
	m_nMax  = 100;
	m_nCur  = 0;
	m_nPage = 10;
	m_nLine = 1;
	m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
}

bool ProgressCtrlBase::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	if (false == Control::SetAttribute(mapAttrib,bReload))
		return false;

	ATTRMAP::iterator iter = m_mapAttribute.find(XML_PROGRESSCTRL_DIRECTION);
	if (m_mapAttribute.end() != iter )
	{
		String& str = iter->second;
		if (XML_PROGRESSCTRL_DIRECTION_LEFTRIGHT == str)
		{
			m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
		}
		else if (XML_PROGRESSCTRL_DIRECTION_BOTTOMTOP == str)
		{
			m_eDirectionType = PROGRESS_SCROLL_BOTTOM_2_TOP;
		}
		else if (XML_PROGRESSCTRL_DIRECTION_RIGHTLEFT == str)
		{
			m_eDirectionType = PROGRESS_SCROLL_RIGHT_2_LEFT;
		}
		else if (XML_PROGRESSCTRL_DIRECTION_TOPBOTTOM == str)
		{
			m_eDirectionType = PROGRESS_SCROLL_TOP_2_BOTTOM;
		}
		else
		{
			UI_LOG_WARN(_T("%s  \"%s\" is invalid: %s"), FUNC_NAME, XML_PROGRESSCTRL_DIRECTION, str.c_str() );
		}
		m_mapAttribute.erase(iter);
	}
	return true;
}

SIZE  ProgressCtrlBase::GetAutoSize()
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

	if (nPos >= m_nMax)
		nPos = m_nMax;

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

	if( m_nCur > m_nMax )
		m_nCur = m_nMax;
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
	m_nStyle |= n&PROGRESS_STYLE_MASK;
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

void ProgressCtrl::OnPaint(IRenderTarget* pDC)
{
	if (m_nMax == m_nMin)
		return ;

	if (NULL != m_pForegndRender)
	{
		CRect rc;
		this->GetClientRectAsWin32(&rc);

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

		m_pForegndRender->DrawState(pDC, &rc, 0);
	}
}