#include "stdafx.h"

ProgressCtrlBase::ProgressCtrlBase()
{
	m_nMin  = 0;
	m_nMax  = 100;
	m_nCur  = 0;
	m_nScrollPage = 0;
	m_nLine = 1;
	m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
}

bool ProgressCtrlBase::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	if( false == Control::SetAttribute(mapAttrib,bReload) )
		return false;

	if( 0 != mapAttrib.count(XML_PROGRESSCTRL_DIRECTION) )
	{
		String str = mapAttrib[XML_PROGRESSCTRL_DIRECTION];
		if ( str == XML_PROGRESSCTRL_DIRECTION_X )
		{
			m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
		}
		else if ( str == XML_PROGRESSCTRL_DIRECTION_Y )
		{
			m_eDirectionType = PROGRESS_SCROLL_BOTTOM_2_TOP;
		}
		else
		{
			UI_LOG_WARN(_T("ProgressCtrlBase::SetAttribute  \"%s\" is invalid: %s"), XML_PROGRESSCTRL_DIRECTION, str.c_str() );
		}
		this->m_mapAttribute.erase(XML_PROGRESSCTRL_DIRECTION);
	}

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

	if (nPos >= m_nMax - m_nScrollPage)
		nPos = m_nMax - m_nScrollPage;

	if (nPos <= m_nMin)
		nPos = m_nMin;

	m_nCur = nPos;

	if (m_nCur != oldPos && bUpdate)
		this->UpdateObject();

	return oldPos;
}

int ProgressCtrlBase::SetPage(int nPage, bool bUpdate)
{
	int nOldPage = m_nScrollPage;
	m_nScrollPage = nPage;

	int nRange = GetRange();

	if (m_nScrollPage < 0)
		m_nScrollPage = 0;
	if (m_nScrollPage > nRange)
		m_nScrollPage = nRange;

	if (nOldPage != m_nScrollPage && bUpdate)
		this->UpdateObject();

	return nOldPage;
}

void ProgressCtrlBase::SetRange(int nLower, int nUpper, bool bUpdate)
{
	m_nMax = nUpper;
	m_nMin = nLower;

	if( m_nCur > m_nMax-m_nScrollPage )
		m_nCur = m_nMax-m_nScrollPage;
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

// �������ؼ�������
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

SliderCtrlBase::SliderCtrlBase()
{
	this->SetProgressStyle(PROGRESS_STYLE_SLIDER);
	m_pButton = NULL;  // �ö����ɾ�������ഴ��(new)������Ϊ�Ӷ���(AddChild)����objectϵͳ�Զ��ͷ�(delete)
	m_nDiffFix = 0;

	m_nBegin = m_nEnd = m_nTrack = 0;
}

void SliderCtrlBase::ResetAttribute()
{
	ProgressCtrlBase::ResetAttribute();

	if( NULL != m_pButton )
	{
		m_pButton->ResetAttribute();
	}
}	
bool SliderCtrlBase::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = ProgressCtrlBase::SetAttribute(mapAttrib,bReload);
	if( false == bRet )
		return bRet;

	if(  m_pButton == NULL )
		return true;


	this->SetChildObjectAttribute( m_pButton, XML_SLIDERCTRL_BUTTON_ATTR_PRIFIX, mapAttrib, bReload );

	// �԰�ť����Ϣ�������أ�ͳһ�ڸ����д��� 
	m_pButton->AddHook( this, 0, 1 );

	return true;
}


int SliderCtrlBase::SetPos(int nPos, bool bUpdate)
{
	int oldPos = m_nCur;

	if (nPos >= m_nMax - m_nScrollPage)
		nPos = m_nMax - m_nScrollPage;

	if( nPos <= m_nMin )
		nPos = m_nMin;

	m_nCur = nPos;

	if (m_nCur != oldPos )
	{
		this->UpdateUIData(false, true);
		if (bUpdate)
			this->UpdateObject();
	}

	return oldPos;
}
void SliderCtrlBase::SetRange(int nLower, int nUpper, bool bUpdate)
{
	if( nLower == m_nMin && nUpper == m_nMax )
		return;

	m_nMax = nUpper;
	m_nMin = nLower;

	if( m_nCur > m_nMax-m_nScrollPage )
		m_nCur = m_nMax-m_nScrollPage;
	if( m_nCur < m_nMin )
		m_nCur = m_nMin;

	if (this->GetProgressStyle() & PROGRESS_STYLE_SCROLLBAR)
		this->UpdateUIData(true, true);
	else
		this->UpdateUIData(false, true);

	if (bUpdate)
		this->UpdateObject();
}
int SliderCtrlBase::SetPage(int nPage, bool bUpdate)
{
	int nOldPage = m_nScrollPage;
	m_nScrollPage = nPage;

/*	int nRange = GetRange();*/

	if (m_nScrollPage < 0)
		m_nScrollPage = 0;
// 	if (m_nScrollPage > nRange)
// 		m_nScrollPage = nRange;

	if (nOldPage != m_nScrollPage)
	{
		if (this->GetProgressStyle() & PROGRESS_STYLE_SCROLLBAR)
			this->UpdateUIData(true, true);
		else
			this->UpdateUIData(false, true);
		if (bUpdate)
			this->UpdateObject();
	}

	return nOldPage;
}

void SliderCtrlBase::SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate)
{
	if (NULL == lpsi || 0 == lpsi->fMask)
		return;

	if (lpsi->fMask & SIF_RANGE)
		__super::SetRange(lpsi->nMin, lpsi->nMax, false);

	if (lpsi->fMask & SIF_POS)
		__super::SetPos(lpsi->nPos, false);

	if (lpsi->fMask & SIF_PAGE)
		__super::SetPage(lpsi->nPage, false);

	this->UpdateUIData(false,true);
	
	if (bUpdate)
		this->UpdateObject();
}

//
//	�ӵ�ǰ���߼�λ�ü������������ť���/�²�Ӧ�ô��ڵ�λ��
//
// int SliderCtrlBase::CurPosToWindowPos( )
// {
// 	int nWH = 0;
// 	int nRange = GetRange();
// 	if( nRange == 0 )
// 		nRange = 1;
// 
// 	if( m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP )
// 	{
// 		nWH = this->GetHeight() - m_pButton->GetHeight();
// 		return this->GetHeight() - (int)((double)nWH / (double)nRange * (double)(m_nCur-m_nMin));
// 	}
// 	else if(m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT )
// 	{
// 		nWH = this->GetWidth() - m_pButton->GetWidth();
// 		return (int)((double)nWH / (double)nRange * (double)(m_nCur-m_nMin));
// 	}
// 	else
// 	{
// 		UIASSERT(0);
// 		return 0;
// 	}
// }

//
//	����������ƶ���ťʱ������ǰ������еĴ�������ת��ΪUI Track Pos
//
int SliderCtrlBase::WindowPoint2UITrackPos(const POINT* ptWindow)
{
	int nTrackPos = 0;

	POINT ptObj = {0,0};
	this->WindowPoint2ObjectPoint(ptWindow, &ptObj); 

	switch(m_eDirectionType)
	{
	case PROGRESS_SCROLL_BOTTOM_2_TOP:
		nTrackPos = m_nBegin - ptObj.y + m_nDiffFix;
		break;
	case PROGRESS_SCROLL_LEFT_2_RIGHT:
		nTrackPos = ptObj.x - m_nDiffFix - m_nBegin;
		break;
	case PROGRESS_SCROLL_TOP_2_BOTTOM:
		nTrackPos = ptObj.y - m_nDiffFix - m_nBegin ;
		break;
	case PROGRESS_SCROLL_RIGHT_2_LEFT:
		nTrackPos = m_nBegin - ptObj.x + m_nDiffFix;
		break;
	default:
		UIASSERT(0);
		break;
	}
	return nTrackPos;
}
// 
//	����������ж�λ�������m_nCur
//
//	nTrackPosΪ����m_nBegin�ľ���
//
void SliderCtrlBase::CalcCurPosByUITrackPos( int nTrackPos )
{
	int nRange = GetRange();
	int nLong = abs(m_nEnd - m_nBegin);

	if(nTrackPos <= 0)
	{
		m_nCur = m_nMin;
		return;
	}
	if( nTrackPos >= nLong )
	{
		m_nCur = m_nMax-m_nScrollPage;
		return;
	}

	switch(m_eDirectionType)
	{
	case PROGRESS_SCROLL_LEFT_2_RIGHT:
	case PROGRESS_SCROLL_TOP_2_BOTTOM:
		{
			// (m_nTrack-m_nBegin)/(m_nEnd-m_nBegin) = (m_nCur-m_nMin)/(m_nMax-m_nMin)
			if( 0 == nLong )
				nLong = 1;

			m_nCur = (int)( (double)(nTrackPos)/(double)nLong * (double)nRange + 0.5 ) + m_nMin;
		}
		break;

	case PROGRESS_SCROLL_RIGHT_2_LEFT:
	case PROGRESS_SCROLL_BOTTOM_2_TOP:
		{
			// (m_nTrack-m_nBegin)/(m_nEnd-m_nBegin) = (m_nCur-m_nMin)/(m_nMax-m_nMin)
			if( 0 == nLong )
				nLong = 1;

			m_nCur = (int)( (double)(nTrackPos)/(double)nLong * (double)nRange + 0.5 ) + m_nMin;
		}
		break;
	}
	
}

//
//	�������Trackbar�ڲ��ؼ���λ����������ΪOnMove�Ǵ� Trackbar_Base::UpdateRect�е��õ�
//
void SliderCtrlBase::OnSize( UINT nType, int cx, int cy )
{
	UpdateUIData(true,true);
}


//
//	�����m_nCur֮�󣬾Ϳ��Ը��� m_nBegin, m_nEnd, m_nTrack
//
//	Remark:
//		��������Ҫ��ѭm_nBegin, m_nEnd, m_nTrack������m_nCur����ó���
//		�������������ʱ�ͼ���m_nTrack��
//
//		���������ǣ�
//		�������ʱ�����m_nCur������m_nCur�����m_nTrack���ٸ���m_nTrack���½���
//
void SliderCtrlBase::UpdateUIData(bool bCalBeginEnd, bool bUpdateButtonRect)
{
	SIZE s = {0,0};
	if(NULL != m_pButton)
	{
		if (this->GetProgressStyle() & PROGRESS_STYLE_SCROLLBAR)
		{
			switch(m_eDirectionType)
			{
			case PROGRESS_SCROLL_BOTTOM_2_TOP:
			case PROGRESS_SCROLL_TOP_2_BOTTOM:
				{
					int nRange = m_nMax - m_nMin;
					if(nRange == 0) nRange = 1;

					int nBtnLength = 0;
					if (m_nScrollPage < nRange)
					{
						nBtnLength = this->GetHeight() * m_nScrollPage / nRange;
						if (nBtnLength < 10)
							nBtnLength = 10; // ��С��С
					}

					if (nBtnLength<0)
						nBtnLength = 0;

					s.cx = this->GetWidth();
					s.cy = nBtnLength;
				}
				break;

			case PROGRESS_SCROLL_LEFT_2_RIGHT:
			case PROGRESS_SCROLL_RIGHT_2_LEFT:
				{
					int nRange = GetRange();
					if(nRange == 0) nRange = 1;

					int nBtnLength = 0;
					if (m_nScrollPage < nRange)
					{
						nBtnLength = this->GetWidth() * m_nScrollPage / nRange;
						if (nBtnLength < 10)
							nBtnLength = 10; // ��С��С
					}

					if (nBtnLength<0)
						nBtnLength = 0;

					s.cy = this->GetHeight();
					s.cx = nBtnLength;
				}
				break;
			}
		}
		else
		{
			s.cx = m_pButton->GetWidth();
			s.cy = m_pButton->GetHeight();

			if(s.cx == 0 || s.cy == 0)
			{
				s = m_pButton->GetDesiredSize(NULL);
			}
		}
		
	}

	CRect rcClient;
	this->GetClientRect(&rcClient);

	if(PROGRESS_SCROLL_BOTTOM_2_TOP == m_eDirectionType)
	{
		if( bCalBeginEnd )
		{
			// y �����sliderctrl�����Ϊ�ײ����յ�Ϊ����
			m_nBegin = rcClient.bottom;
			m_nEnd   = rcClient.top;

			if( m_pButton!=NULL )
			{
				m_nBegin -= s.cy/2;
				m_nEnd   += s.cy/2;
			}
		}

		//(m_nBegin - m_nTrack)/(m_nBegin - m_nEnd) = (m_nCur-m_nMin)/(m_nMax-m_nMin)
		//m_nTrack = m_nBegin - (int)( (double)(m_nCur-m_nMin)/(double)(m_nMax-m_nMin) * (double)(m_nBegin - m_nEnd) );
		// (m_nTrack-m_nBegin)/(m_nEnd-m_nBegin) = (m_nCur-m_nMin)/(m_nMax-m_nMin)

		int nRange = GetRange();
		if( nRange == 0 )
			nRange = 1;
		m_nTrack = (int)( (double)(m_nCur-m_nMin)/(double)nRange*(double)(m_nEnd-m_nBegin) )+ m_nBegin;

		if (bUpdateButtonRect && NULL != m_pButton )
		{
			SIZE s = m_pButton->GetAutoSize(NULL);
			int nBottom = m_nTrack+s.cy/2;
			if( nBottom > rcClient.bottom )
			{
				nBottom = rcClient.bottom;
			}
			int nTop = nBottom-s.cy; 
			if( nTop < rcClient.top )   
			{
				nTop = rcClient.top;
				nBottom = nTop + s.cy;
			}

			CRect rc(0, nTop, s.cx, nBottom);
			m_pButton->SetObjectPos(&rc,SWP_NOREDRAW);
		}
	}
	else if( m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT )
	{
		if( bCalBeginEnd )
		{
			m_nBegin = rcClient.left;
			m_nEnd = rcClient.right;

			m_nBegin += s.cx/2;
			m_nEnd   -= s.cx/2;
		}

		// (m_nTrack-m_nBegin)/(m_nEnd-m_nBegin) = (m_nCur-m_nMin)/(m_nMax-m_nMin)
		int nRange = GetRange();
		if( nRange == 0 )
			nRange = 1;
		m_nTrack = (int)( (double)(m_nCur-m_nMin)/(double)nRange * (double)(m_nEnd-m_nBegin) )+ m_nBegin;

		if (bUpdateButtonRect && NULL != m_pButton )
		{
			int nLeft = m_nTrack-s.cx/2;
			if( nLeft < rcClient.left )
			{
				nLeft = rcClient.left;
			}
			int nRight = nLeft + s.cx;
			if( nRight > rcClient.right ) 
			{
				nRight = rcClient.right;
				nLeft = rcClient.right - s.cx;
			}

			// ���¾���
			int yCenter = ( this->GetHeight()-s.cy ) / 2;
			CRect rc(nLeft, yCenter, nRight, yCenter+s.cy);
			m_pButton->SetObjectPos(&rc, SWP_NOREDRAW);
		}
	}
	else if(m_eDirectionType == PROGRESS_SCROLL_TOP_2_BOTTOM)
	{
		if(bCalBeginEnd)
		{
			// y �����sliderctrl�����Ϊ�ײ����յ�Ϊ����
			m_nBegin = rcClient.top;
			m_nEnd   = rcClient.bottom;

			if( m_pButton!=NULL )
			{
				m_nBegin += s.cy/2;
				m_nEnd   -= s.cy/2;
			}
		}

		int nRange = GetRange();
		if( nRange == 0 )
			nRange = 1;
		m_nTrack = (int)( (double)(m_nCur-m_nMin)/(double)nRange*(double)(m_nEnd-m_nBegin)  )+ m_nBegin;

		if (bUpdateButtonRect && NULL != m_pButton)
		{
				int nBottom = m_nTrack+s.cy/2;
				if (nBottom > rcClient.bottom)
				{
					nBottom = rcClient.bottom;
				}
				int nTop = nBottom-s.cy; 
				if (nTop < rcClient.top)   
				{
					nTop = rcClient.top;
					nBottom = nTop + s.cy;
				}

				CRect rc(0, nTop, s.cx, nBottom);
				m_pButton->SetObjectPos(&rc, SWP_NOREDRAW);
		}
	}
	else
	{
		UIASSERT(0);
	}
}

void SliderCtrlBase::OnLButtonDown(UINT nFlags, POINT point)
{
	int nOldPos = m_nCur;
	int nTrackPos = this->WindowPoint2UITrackPos(&point);

	this->CalcCurPosByUITrackPos(nTrackPos);
	this->UpdateUIData(false, true);
	this->UpdateObject();

	if( m_nCur != nOldPos )
	{
		int SB_type = SB_PAGELEFT;
		switch(m_eDirectionType)
		{
		case PROGRESS_SCROLL_BOTTOM_2_TOP:
			SB_type = m_nCur>nOldPos? SB_PAGEUP:SB_PAGEDOWN;
			break;
		case PROGRESS_SCROLL_LEFT_2_RIGHT:
			SB_type = m_nCur>nOldPos? SB_PAGERIGHT:SB_PAGELEFT;
			break;
		case PROGRESS_SCROLL_TOP_2_BOTTOM:
			SB_type = m_nCur>nOldPos? SB_PAGEDOWN:SB_PAGEUP;
			break;
		case PROGRESS_SCROLL_RIGHT_2_LEFT:
			SB_type = m_nCur>nOldPos? SB_PAGELEFT:SB_PAGERIGHT;
		default:
			UIASSERT(0);
			break;
		}
		// ���͵�ǰλ�øı�֪ͨ
		UIMSG   msg;
		msg.message = UI_WM_NOTIFY;
		msg.code    = UI_TRBN_POSCHANGED;
		msg.wParam  = (WPARAM)m_nCur;
		msg.lParam  = SB_type;
		msg.pObjMsgFrom = this;
		this->DoNotify( &msg );

		// ֱ�ӷ��ͽ�����־
		msg.lParam = SB_ENDSCROLL;
		this->DoNotify( &msg );
	}
}

//
//	֧�ּ��̶�λ
//
void SliderCtrlBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	int nOldPos = m_nCur;
	int nScrollLineType = -1;

	if( VK_NEXT == nChar )
	{
		if( m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP )
			nChar = VK_DOWN;
		else if(m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT )
			nChar = VK_RIGHT;
		else
			UIASSERT(0);
	}
	else if( VK_PRIOR == nChar )
	{
		if( m_eDirectionType & PROGRESS_SCROLL_BOTTOM_2_TOP )
			nChar = VK_UP;
		else if( m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT )
			nChar = VK_LEFT;
		else
			UIASSERT(0);
	}

	switch( nChar )
	{
	case VK_LEFT:
		if( m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP )
		{}
		else if( m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT )
		{
			m_nCur -= m_nLine;

			if( m_nCur < m_nMin )
				m_nCur = m_nMin;

			nScrollLineType = SB_LINELEFT;
		}
		else
		{
			UIASSERT(0);
		}
		break;
	case VK_RIGHT:
		if( m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP )
		{

		}
		else if( m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT )
		{
			m_nCur += m_nLine;
		
			if( m_nCur > m_nMax-m_nScrollPage )
				m_nCur = m_nMax-m_nScrollPage;

			nScrollLineType = SB_LINERIGHT;
		}
		else
			UIASSERT(0);
		break;
	case VK_UP:
		if( m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP )
		{
			m_nCur += m_nLine;
			
			if( m_nCur > m_nMax-m_nScrollPage )
				m_nCur = m_nMax-m_nScrollPage;

			nScrollLineType = SB_LINEUP;
		}
		else
		{
			UIASSERT(0);
		}
		break;
	case VK_DOWN:
		if( m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP )
		{
			m_nCur -= m_nLine;

			if( m_nCur < m_nMin )
				m_nCur = m_nMin;

			nScrollLineType = SB_LINEDOWN;
		}
		else
		{
			UIASSERT(0);
		}
		break;
	}

	if( m_nCur != nOldPos )
	{
		this->UpdateUIData(false,true);
		this->UpdateObject();

	}

	// ���͵�ǰλ�øı�֪ͨ
	if( -1 != nScrollLineType )
	{
		UIMSG   msg;
		msg.message = UI_WM_NOTIFY;
		msg.code    = UI_TRBN_POSCHANGED;
		msg.wParam  = (WPARAM)m_nCur;
		msg.lParam  = nScrollLineType;
		msg.pObjMsgFrom = this;
		this->DoNotify( &msg );
	}
	
	UI_LOG_DEBUG( _T("SliderCtrlBase::OnKeyDown, nchat=%d, m_nCur��%d"), nChar, m_nCur );
}

void SliderCtrlBase::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bNeedNotify = false;
	switch( nChar )
	{
	case VK_LEFT:
	case VK_RIGHT:
		{
			if( m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP ){}
			else
				bNeedNotify = true;
		}
		break;
	case VK_UP:
	case VK_DOWN:
		{
			if( m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP )
				bNeedNotify = true;
		}
		break;
	case VK_PRIOR:
	case VK_NEXT:
		{
			bNeedNotify = true;
		}
	}

	if( bNeedNotify )
	{
		// ���͵�ǰλ�øı�֪ͨ
		UIMSG   msg;
		msg.message = UI_WM_NOTIFY;
		msg.code    = UI_TRBN_POSCHANGED;
		msg.wParam  = (WPARAM)m_nCur;
		msg.lParam  = SB_ENDSCROLL;
		msg.pObjMsgFrom = this;
		this->DoNotify( &msg );
	}
}

//
//	������ڻ�����ť����(pointΪwindow����)
//
void SliderCtrlBase::OnBtnLButtonDown(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	m_nDiffFix = 0;

	switch(m_eDirectionType)
	{
	case PROGRESS_SCROLL_BOTTOM_2_TOP:
	case PROGRESS_SCROLL_TOP_2_BOTTOM:
		{
			CRect rcBtnWindow(0,0,0,0);
			this->m_pButton->GetWindowRect(&rcBtnWindow);
			int yBtnCenter = (rcBtnWindow.top + rcBtnWindow.bottom)/2;
			m_nDiffFix = point.y - yBtnCenter;
		}
		break;
	case PROGRESS_SCROLL_LEFT_2_RIGHT:
	case PROGRESS_SCROLL_RIGHT_2_LEFT:
		{
			CRect rcBtnWindow(0,0,0,0);
			this->m_pButton->GetWindowRect(&rcBtnWindow);
			int xBtnCenter = (rcBtnWindow.right + rcBtnWindow.left)/2;
			m_nDiffFix = point.x - xBtnCenter;
		}
		break;
	default:
		UIASSERT(0);
		break;
	}
	m_pButton->SetForcePress(true);  // ��ֹ�����קʱ�Ƴ��˰�ť�ķ�Χ��ť���hover״̬
}

//
//	����ڻ�����ť���϶�
//
//  ע�������ȼ����m_nCur��λ�ã����ܾ��������λ�ã���������point�����㻬��ı���λ�ã���range��С��ʱ�����ֵĸ�Ϊ����
//
void SliderCtrlBase::OnBtnMouseMove(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if(! (nFlags & MK_LBUTTON) )
		return;

	int nOldPos = m_nCur;
	int nTrackPos = this->WindowPoint2UITrackPos(&point);
	this->CalcCurPosByUITrackPos(nTrackPos);

	if( nOldPos != m_nCur )
	{
		this->UpdateUIData(false,true);
		this->UpdateObject();

		// ���͵�ǰλ�øı�֪ͨ
		UIMSG   msg;
		msg.message = UI_WM_NOTIFY;
		msg.code    = UI_TRBN_POSCHANGED;
		msg.wParam  = (WPARAM)m_nCur;
		msg.lParam  = SB_THUMBTRACK;
		msg.pObjMsgFrom = this;
		this->DoNotify( &msg );
	}
}

//
//  ���ӻ�����ť���ͷ�
//	
void SliderCtrlBase::OnBtnLButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	m_nDiffFix = 0;

	// ���͵�ǰλ�øı�֪ͨ
	UIMSG   msg;
	msg.message = UI_WM_NOTIFY;
	msg.code    = UI_TRBN_POSCHANGED;
	msg.wParam  = (WPARAM)m_nCur;
	msg.lParam  = SB_ENDSCROLL;
	msg.pObjMsgFrom = this;
	this->DoNotify( &msg );

	m_pButton->SetForcePress(false);
}

//
//	���ù������ͻ�����ť�������ʽ
//
// BOOL SliderCtrlBase::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message )
// {
// 	::SetCursor(::LoadCursor(NULL,IDC_HAND));
// 	return TRUE;
// }

void SliderCtrlBase::OnPaint( HRDC hRDC )
{
	if ( NULL != m_pForegndRender )
	{
		if( m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP )
		{
			// ��ǰ����������ڱ������棬��ֻ�ڰ�ť�²���ʾ���������������һ��clip
			CRect rcBtnWindow(0,0,0,0);
			CRect rcThisWindow(0,0,0,0);
			this->m_pButton->GetWindowRect(&rcBtnWindow);
			this->GetWindowRect(&rcThisWindow);
			HRGN hRgnClip = ::CreateRectRgn(rcThisWindow.left,rcBtnWindow.top,rcThisWindow.right,rcThisWindow.bottom);

			HRGN hRgnOld = GetClipRgn(hRDC);
			SelectClipRgn(hRDC, hRgnClip );

			CRect rc(0,0,0,0);
			SIZE s = m_pForegndRender->GetDesiredSize();
			if( s.cx == 0 || s.cy == 0 )
			{
				rc.right = GetWidth();
				rc.bottom = GetHeight();
			}
			else
			{
				int xCenter = (this->GetWidth() - s.cx ) / 2;

				rc.left = xCenter;
				rc.right = rc.left + s.cx;
				rc.bottom = GetHeight();
			}
			this->m_pForegndRender->DrawState(hRDC, &rc, 0);

			::DeleteObject(hRgnClip);
			::SelectClipRgn(hRDC,hRgnOld);
			if( NULL != hRgnOld )
			{
				::DeleteObject(hRgnOld);
				hRgnOld = NULL;
			}
		}
		else if( PROGRESS_SCROLL_LEFT_2_RIGHT == m_eDirectionType )
		{
			// ��ǰ����������ڱ������棬��ֻ�ڰ�ť�����ʾ���������������һ��clip
			CRect rcBtnWindow(0,0,0,0);
			CRect rcThisWindow(0,0,0,0);
			this->m_pButton->GetWindowRect(&rcBtnWindow);
			this->GetWindowRect(&rcThisWindow);
			HRGN hRgnClip = ::CreateRectRgn(rcThisWindow.left,rcThisWindow.top,rcBtnWindow.left,rcThisWindow.bottom);

			HRGN hRgnOld = GetClipRgn(hRDC);
			SelectClipRgn(hRDC, hRgnClip );

			CRect rc(0,0,0,0);
			SIZE s = m_pForegndRender->GetDesiredSize();
			if( s.cx == 0 || s.cy == 0 )
			{
				rc.right = GetWidth();
				rc.bottom = GetHeight();
			}
			else
			{
				int yCenter = (this->GetHeight() - s.cy) / 2;

				rc.top = yCenter;
				rc.bottom = rc.top + s.cy;
				rc.right = this->GetWidth();
			}
			this->m_pForegndRender->DrawState(hRDC, &rc, 0);

			::DeleteObject(hRgnClip);
			::SelectClipRgn(hRDC,hRgnOld);
			if( NULL != hRgnOld )
			{
				::DeleteObject(hRgnOld);
				hRgnOld = NULL;
			}
		}
		else{
			UIASSERT(0);
		}
	}
}

SliderCtrl::SliderCtrl()
{
	m_pButton = new Button;
	this->AddChild( m_pButton );
}

SliderCtrl::~SliderCtrl()
{
}

