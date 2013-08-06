#include "stdafx.h"
#include "sliderctrl.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"

SliderCtrl::SliderCtrl()
{
    m_pISliderCtrl = NULL;
	m_pButton = NULL;  // 该对象由具体的子类创建(new)，并加为子对象(AddChild)，由object系统自动释放(delete)
	m_nDiffFix = 0;

    m_nMin  = 0;
    m_nMax  = 100;
    m_nCur  = 0;
    m_nPage = 10;
    m_nLine = 1;
    m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;

	m_nBegin = m_nEnd = m_nTrack = 0;
	m_pTickRender = NULL;
	m_nFreq = 10;
}

SliderCtrl::~SliderCtrl()
{
	SAFE_RELEASE(m_pTickRender);
	for (int i = 0; i < (int)m_vTickInfo.size(); i++)
	{
		delete m_vTickInfo[i];
	}
	m_vTickInfo.clear();
}

void SliderCtrl::ResetAttribute()
{
	DO_PARENT_PROCESS(ISliderCtrl, IControl);

    m_eDirectionType = PROGRESS_SCROLL_LEFT_2_RIGHT;
	if (m_pButton)
	{
        UISendMessage(m_pButton, UI_WM_RESETATTRIBUTE);
		m_pButton->SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);  // 因为默认按钮是CONTENT autosize的，这里的滑块按钮没有内容，需要按背景进行auto size
		m_pButton->SetObjectPos(0,0,0,0,SWP_NOMOVE|SWP_NOREDRAW); // 强制换肤后，刷新按钮的大小 TODO: 优化整个逻辑
	}

	m_nFreq = 10;
	SAFE_RELEASE(m_pTickRender);
	m_pISliderCtrl->ModifyStyle(0, SLIDER_STYLE_POINT_LEFT|SLIDER_STYLE_POINT_TOP, false);  // 默认无指向
}	
void SliderCtrl::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
	DO_PARENT_PROCESS(ISliderCtrl, IControl);

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

	szText = pMapAttr->GetAttr(XML_SLIDERCTRL_THUMB_POINT_DIRECTION, true);
	if (szText)
	{
		if (0 == _tcscmp(XML_SLIDERCTRL_THUMB_POINT_DIRECTION_LEFT, szText))
		{
			m_pISliderCtrl->ModifyStyle(SLIDER_STYLE_POINT_LEFT, SLIDER_STYLE_POINT_RIGHT, false);
		}
		else if (0 == _tcscmp(XML_SLIDERCTRL_THUMB_POINT_DIRECTION_RIGHT, szText))
		{
			m_pISliderCtrl->ModifyStyle(SLIDER_STYLE_POINT_RIGHT, SLIDER_STYLE_POINT_LEFT, false);
		}
		else if (0 == _tcscmp(XML_SLIDERCTRL_THUMB_POINT_DIRECTION_TOP, szText))
		{
			m_pISliderCtrl->ModifyStyle(SLIDER_STYLE_POINT_TOP, SLIDER_STYLE_POINT_BOTTOM, false);
		}
		else if (0 == _tcscmp(XML_SLIDERCTRL_THUMB_POINT_DIRECTION_BOTTOM, szText))
		{
			m_pISliderCtrl->ModifyStyle(SLIDER_STYLE_POINT_BOTTOM, SLIDER_STYLE_POINT_TOP, false);
		}
		else if (0 == _tcscmp(XML_SLIDERCTRL_THUMB_POINT_DIRECTION_BOTH, szText))
		{
			m_pISliderCtrl->ModifyStyle(0, SLIDER_STYLE_POINT_LEFT|SLIDER_STYLE_POINT_RIGHT, false);
		}
		else
		{
			UI_LOG_WARN(_T("%s unknown %s : %s"), FUNC_NAME, XML_SLIDERCTRL_THUMB_POINT_DIRECTION, szText);
		}
	}
    if (NULL == m_pISliderCtrl->GetBkRender())
    {
        IRenderBase* p = NULL;
        m_pISliderCtrl->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_SLIDERCTRL_BKGND, m_pISliderCtrl, &p);
        if (p)
        {
            p->SetAttribute(pMapAttr, XML_BACKGND_RENDER_PREFIX, true);
            m_pISliderCtrl->SetBkgndRender(p);
            SAFE_RELEASE(p);
            m_pISliderCtrl->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, false);
        }
    }

	if (m_pButton)
    {
	    m_pButton->SetAttributeByPrefix(XML_SLIDERCTRL_BUTTON_ATTR_PRIFIX, pMapAttr, bReload, true);
	    if (NULL == m_pButton->GetBkRender())
	    {
            IRenderBase* p = NULL;
            m_pISliderCtrl->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_SLIDER_TRACK_BUTTON, m_pButton, &p);
		    if (p)
            {
		        m_pButton->SetBkgndRender(p);
		        SAFE_RELEASE(p);
		        m_pButton->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, false);
            }
	    }
    }
}


int SliderCtrl::SetPos(int nPos, bool bUpdate)
{
	int oldPos = m_nCur;

	if (nPos >= m_nMax)
		nPos = m_nMax;

	if (nPos <= m_nMin)
		nPos = m_nMin;

	m_nCur = nPos;

	if (m_nCur != oldPos)
	{
		this->UpdateUIData(false, true);
		if (bUpdate)
			m_pISliderCtrl->UpdateObject();
	}

	return oldPos;
}
void SliderCtrl::SetRange(int nLower, int nUpper, bool bUpdate)
{
	if (nLower == m_nMin && nUpper == m_nMax)
		return;

	m_nMax = nUpper;
	m_nMin = nLower;

	if (m_nCur > m_nMax)
		m_nCur = m_nMax;
	if (m_nCur < m_nMin)
		m_nCur = m_nMin;

	this->UpdateUIData(false, true);
	this->UpdateTicksData();

	if (bUpdate)
		m_pISliderCtrl->UpdateObject();
}
int SliderCtrl::SetPage(int nPage, bool bUpdate)
{
	int nOldPage = m_nPage;
	m_nPage = nPage;

	if (m_nPage < 0)
		m_nPage = 0;

	if (nOldPage != m_nPage)
	{
		this->UpdateUIData(false, true);

		if (bUpdate)
			m_pISliderCtrl->UpdateObject();
	}

	return nOldPage;
}

void SliderCtrl::SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate)
{
	if (NULL == lpsi || 0 == lpsi->fMask)
		return;

	if (lpsi->fMask & SIF_RANGE)
		SetRange(lpsi->nMin, lpsi->nMax, false);

	if (lpsi->fMask & SIF_POS)
		SetPos(lpsi->nPos, false);

	if (lpsi->fMask & SIF_PAGE)
		SetPage(lpsi->nPage, false);

	this->UpdateUIData(false,true);

	if (bUpdate)
		m_pISliderCtrl->UpdateObject();
}

//
//	从当前的逻辑位置计算出，滑动按钮左侧/下侧应该处于的位置
//
// int SliderCtrl::CurPosToWindowPos()
// {
// 	int nWH = 0;
// 	int nRange = GetRange();
// 	if (nRange == 0)
// 		nRange = 1;
// 
// 	if (m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP)
// 	{
// 		nWH = this->GetHeight() - m_pButton->GetHeight();
// 		return this->GetHeight() - (int)((double)nWH / (double)nRange * (double)(m_nCur-m_nMin));
// 	}
// 	else if(m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT)
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
//	是鼠标点击、移动按钮时，将当前鼠标所有的窗口坐标转换为UI Track Pos
//
int SliderCtrl::WindowPoint2UITrackPos(const POINT* ptWindow)
{
	int nTrackPos = 0;

	POINT ptObj = {0,0};
	m_pISliderCtrl->WindowPoint2ObjectPoint(ptWindow, &ptObj); 

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
//	用鼠标点击进行定位，计算出m_nCur
//
//	nTrackPos为距离m_nBegin的距离
//
void SliderCtrl::CalcCurPosByUITrackPos( int nTrackPos)
{
	int nRange = GetRange();
	int nLong = abs(m_nEnd - m_nBegin);

	if (nTrackPos <= 0)
	{
		m_nCur = m_nMin;
		return;
	}
	if (nTrackPos >= nLong)
	{
		m_nCur = m_nMax;
		return;
	}

	switch (m_eDirectionType)
	{
	case PROGRESS_SCROLL_LEFT_2_RIGHT:
	case PROGRESS_SCROLL_TOP_2_BOTTOM:
		{
			// (m_nTrack-m_nBegin)/(m_nEnd-m_nBegin) = (m_nCur-m_nMin)/(m_nMax-m_nMin)
			if (0 == nLong || 0 == nRange)
				m_nCur = m_nMin;
            else
			    m_nCur = (int)( (double)(nTrackPos)/(double)nLong * (double)nRange + 0.5) + m_nMin;
		}
		break;

	case PROGRESS_SCROLL_RIGHT_2_LEFT:
	case PROGRESS_SCROLL_BOTTOM_2_TOP:
		{
			// (m_nTrack-m_nBegin)/(m_nEnd-m_nBegin) = (m_nCur-m_nMin)/(m_nMax-m_nMin)
            if (0 == nLong || 0 == nRange)
                m_nCur = m_nMin;
            else
			    m_nCur = (int)((double)(nTrackPos)/(double)nLong * (double)nRange + 0.5) + m_nMin;
		}
		break;
	}

}

//
//	在这里对Trackbar内部控件的位置重新整理，为OnMove是从 Trackbar_Base::UpdateRect中调用的
//
void SliderCtrl::OnSize(UINT nType, int cx, int cy)
{
	this->UpdateUIData(true,true);
	this->UpdateTicksData();
}


//
//	计算出m_nCur之后，就可以更新 m_nBegin, m_nEnd, m_nTrack
//
//	Remark:
//		在这里需要遵循m_nBegin, m_nEnd, m_nTrack必须由m_nCur计算得出，
//		而不是在鼠标点击时就计算m_nTrack。
//
//		整个过程是：
//		在鼠标点击时计算出m_nCur，再由m_nCur计算出m_nTrack，再根据m_nTrack更新界面
//
void SliderCtrl::UpdateUIData(bool bCalBeginEnd, bool bUpdateButtonRect)
{
	SIZE s = {0,0};
	if (m_pButton)
	{
		s.cx = m_pButton->GetWidth();
		s.cy = m_pButton->GetHeight();

		if(s.cx == 0 || s.cy == 0)
		{
			s = m_pButton->GetDesiredSize();
			s.cx -= m_pButton->GetMarginW();
			s.cy -= m_pButton->GetMarginH();
		}
	}

	CRect rcClient;
	m_pISliderCtrl->GetClientRect(&rcClient);
	
	if (m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT)
	{
		if (bCalBeginEnd)
		{
			m_nBegin = rcClient.left;
			m_nEnd = rcClient.right;

			m_nBegin += s.cx/2;
			m_nEnd   -= s.cx/2;
		}

		// (m_nTrack-m_nBegin)/(m_nEnd-m_nBegin) = (m_nCur-m_nMin)/(m_nMax-m_nMin)
		int nRange = GetRange();
		if (nRange == 0 || (m_nEnd-m_nBegin)==0)
            m_nTrack = m_nBegin;
        else
		    m_nTrack = (int)((double)(m_nCur-m_nMin)/(double)nRange * (double)(m_nEnd-m_nBegin))+ m_nBegin;

		if (bUpdateButtonRect && NULL != m_pButton)
		{
			int nLeft = m_nTrack-s.cx/2;
			if (nLeft < rcClient.left)
			{
				nLeft = rcClient.left;
			}
			int nRight = nLeft + s.cx;
			if (nRight > rcClient.right) 
			{
				nRight = rcClient.right;
				nLeft = rcClient.right - s.cx;
			}

			// 上下居中
			int yCenter = (m_pISliderCtrl->GetHeight()-s.cy) / 2;
			CRect rc(nLeft, yCenter, nRight, yCenter+s.cy);
			m_pButton->SetObjectPos(&rc, SWP_NOREDRAW);
		}
	}
	else if (PROGRESS_SCROLL_BOTTOM_2_TOP == m_eDirectionType)
	{
		if (bCalBeginEnd)
		{
			// y 方向的sliderctrl，起点为底部，终点为顶部
			m_nBegin = rcClient.bottom;
			m_nEnd   = rcClient.top;

			if (m_pButton != NULL)
			{
				m_nBegin -= s.cy/2;
				m_nEnd   += s.cy/2;
			}
		}

		int nRange = GetRange();
		if (nRange == 0 || (m_nEnd-m_nBegin)==0)
			m_nTrack = m_nBegin;
        else
		    m_nTrack = (int)((double)(m_nCur-m_nMin)/(double)nRange*(double)(m_nEnd-m_nBegin))+ m_nBegin;

		if (bUpdateButtonRect && NULL != m_pButton)
		{
			int nBottom = m_nTrack+s.cy/2;
			if(nBottom > rcClient.bottom)
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
			m_pButton->SetObjectPos(&rc,SWP_NOREDRAW);
		}
	}
	else if (m_eDirectionType == PROGRESS_SCROLL_TOP_2_BOTTOM)
	{
		if(bCalBeginEnd)
		{
			// y 方向的sliderctrl，起点为底部，终点为顶部
			m_nBegin = rcClient.top;
			m_nEnd   = rcClient.bottom;

			if (m_pButton != NULL)
			{
				m_nBegin += s.cy/2;
				m_nEnd   -= s.cy/2;
			}
		}

		int nRange = GetRange();
		if (nRange == 0 || (m_nEnd-m_nBegin)==0)
			m_nTrack = m_nBegin;
        else
		    m_nTrack = (int)((double)(m_nCur-m_nMin)/(double)nRange*(double)(m_nEnd-m_nBegin))+ m_nBegin;

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
	else if (m_eDirectionType == PROGRESS_SCROLL_RIGHT_2_LEFT)
	{
		if (bCalBeginEnd)
		{
			m_nBegin = rcClient.right;
			m_nEnd = rcClient.left;

			m_nBegin -= s.cx/2;
			m_nEnd   += s.cx/2;
		}

		// (m_nTrack-m_nBegin)/(m_nEnd-m_nBegin) = (m_nCur-m_nMin)/(m_nMax-m_nMin)
		int nRange = GetRange();
        if (nRange == 0 || (m_nEnd-m_nBegin)==0)
            m_nTrack = m_nBegin;
        else
		m_nTrack = (int)((double)(m_nCur-m_nMin)/(double)nRange * (double)(m_nEnd-m_nBegin))+ m_nBegin;

		if (bUpdateButtonRect && NULL != m_pButton)
		{
			int nLeft = m_nTrack-s.cx/2;
			if (nLeft < rcClient.left)
			{
				nLeft = rcClient.left;
			}
			int nRight = nLeft + s.cx;
			if (nRight > rcClient.right) 
			{
				nRight = rcClient.right;
				nLeft = rcClient.right - s.cx;
			}

			// 上下居中
			int yCenter = (m_pISliderCtrl->GetHeight()-s.cy) / 2;
			CRect rc(nLeft, yCenter, nRight, yCenter+s.cy);
			m_pButton->SetObjectPos(&rc, SWP_NOREDRAW);
		}
	}
	else
	{
		UIASSERT(0);
	}
}

void SliderCtrl::OnLButtonDown(UINT nFlags, POINT point)
{
	int nOldPos = m_nCur;
	int nTrackPos = this->WindowPoint2UITrackPos(&point);

	this->CalcCurPosByUITrackPos(nTrackPos);
	this->UpdateUIData(false, true);
	m_pISliderCtrl->UpdateObject();

	if (m_nCur != nOldPos)
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
			break;
		default:
			UIASSERT(0);
			break;
		}
		// 发送当前位置改变通知
		UIMSG   msg;
		msg.message = UI_WM_NOTIFY;
		msg.nCode    = UI_TRBN_POSCHANGED;
		msg.wParam  = (WPARAM)m_nCur;
		msg.lParam  = SB_type;
		msg.pMsgFrom = m_pISliderCtrl;
		m_pISliderCtrl->DoNotify( &msg);

		// 直接发送结束标志
		msg.lParam = SB_ENDSCROLL;
		m_pISliderCtrl->DoNotify( &msg);
	}
}

//
//	支持键盘定位
//
void SliderCtrl::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	int nOldPos = m_nCur;
	int nScrollLineType = -1;

	if (VK_NEXT == nChar)
	{
		if (m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP)
		{
			nChar = VK_DOWN;
			UIASSERT(0); // TODO
		}
		else if (m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT)
		{
			m_nCur += m_nPage;
			m_nCur = FixCurValue(m_nCur);
			nScrollLineType = SB_PAGERIGHT;
		}
		else
			UIASSERT(0);
	}
	else if (VK_PRIOR == nChar)
	{
		if (m_eDirectionType & PROGRESS_SCROLL_BOTTOM_2_TOP)
		{
			nChar = VK_UP;
			UIASSERT(0); // TODO
		}
		else if (m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT)
		{
			m_nCur -= m_nPage;
			m_nCur = FixCurValue(m_nCur);
			nScrollLineType = SB_PAGELEFT;
		}
		else
			UIASSERT(0);
	}

	switch (nChar)
	{
	case VK_LEFT:
		if (m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP)
		{}
		else if (m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT)
		{
			m_nCur -= m_nLine;
			m_nCur = FixCurValue(m_nCur);
			nScrollLineType = SB_LINELEFT;
		}
		else
		{
			UIASSERT(0);
		}
		break;
	case VK_RIGHT:
		if (m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP)
		{

		}
		else if (m_eDirectionType == PROGRESS_SCROLL_LEFT_2_RIGHT)
		{
			m_nCur += m_nLine;
			m_nCur = FixCurValue(m_nCur);
			nScrollLineType = SB_LINERIGHT;
		}
		else
			UIASSERT(0);
		break;
	case VK_UP:
		if (m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP)
		{
			m_nCur += m_nLine;
            m_nCur = FixCurValue(m_nCur);
			nScrollLineType = SB_LINEUP;
		}
		else
		{
			UIASSERT(0);
		}
		break;
	case VK_DOWN:
		if (m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP)
		{
			m_nCur -= m_nLine;

			if (m_nCur < m_nMin)
				m_nCur = m_nMin;

			nScrollLineType = SB_LINEDOWN;
		}
		else
		{
			UIASSERT(0);
		}
		break;
	}

	if (m_nCur != nOldPos)
	{
		this->UpdateUIData(false,true);
		m_pISliderCtrl->UpdateObject();

	}

	// 发送当前位置改变通知
	if (-1 != nScrollLineType)
	{
		UIMSG   msg;
		msg.message = UI_WM_NOTIFY;
		msg.nCode    = UI_TRBN_POSCHANGED;
		msg.wParam  = (WPARAM)m_nCur;
		msg.lParam  = nScrollLineType;
		msg.pMsgFrom = m_pISliderCtrl;
		m_pISliderCtrl->DoNotify(&msg);
	}

	UI_LOG_DEBUG(_T("SliderCtrl::OnKeyDown, nchat=%d, m_nCur＝%d"), nChar, m_nCur);
}

void SliderCtrl::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bNeedNotify = false;
	switch (nChar)
	{
	case VK_LEFT:
	case VK_RIGHT:
		{
			if (m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP){}
			else
				bNeedNotify = true;
		}
		break;
	case VK_UP:
	case VK_DOWN:
		{
			if (m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP)
				bNeedNotify = true;
		}
		break;
	case VK_PRIOR:
	case VK_NEXT:
		{
			bNeedNotify = true;
		}
	}

	if (bNeedNotify)
	{
		// 发送当前位置改变通知
		UIMSG   msg;
		msg.message = UI_WM_NOTIFY;
		msg.nCode    = UI_TRBN_POSCHANGED;
		msg.wParam  = (WPARAM)m_nCur;
		msg.lParam  = SB_ENDSCROLL;
		msg.pMsgFrom = m_pISliderCtrl;
		m_pISliderCtrl->DoNotify( &msg);
	}
}

//
//	鼠标点击在滑动按钮上面(point为window坐标)
//
void SliderCtrl::OnBtnLButtonDown(UINT nFlags, POINT point)
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
	m_pButton->SetForcePress(true);  // 防止鼠标拖拽时移出了按钮的范围按钮变成hover状态
}

//
//	鼠标在滑动按钮上拖动
//
//  注：必须先计算出m_nCur的位置，才能决定滑块的位置！不能先由point来计算滑块的本次位置，在range很小的时候体现的更为明显
//
void SliderCtrl::OnBtnMouseMove(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if(! (nFlags & MK_LBUTTON))
		return;

	int nOldPos = m_nCur;
	int nTrackPos = this->WindowPoint2UITrackPos(&point);
	this->CalcCurPosByUITrackPos(nTrackPos);

	if (nOldPos != m_nCur)
	{
		this->UpdateUIData(false,true);
		m_pISliderCtrl->UpdateObject();

		// 发送当前位置改变通知
		UIMSG   msg;
		msg.message = UI_WM_NOTIFY;
		msg.nCode    = UI_TRBN_POSCHANGED;
		msg.wParam  = (WPARAM)m_nCur;
		msg.lParam  = SB_THUMBTRACK;
		msg.pMsgFrom = m_pISliderCtrl;
		m_pISliderCtrl->DoNotify( &msg);
	}
}

//
//  鼠标从滑动按钮上释放
//	
void SliderCtrl::OnBtnLButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	m_nDiffFix = 0;

	// 发送当前位置改变通知
	UIMSG   msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode    = UI_TRBN_POSCHANGED;
	msg.wParam  = (WPARAM)m_nCur;
	msg.lParam  = SB_ENDSCROLL;
	msg.pMsgFrom = m_pISliderCtrl;
	m_pISliderCtrl->DoNotify( &msg);

	m_pButton->SetForcePress(false);
}


void SliderCtrl::OnStateChanged(UINT nMask)
{
	// TODO: 绘制焦点focus rect。还需要继续拦截button的focus变化
// 	if (nOld&CSB_FOCUS && !(nNew&CSB_FOCUS) ||
// 		nNew&CSB_FOCUS && !(nOld&CSB_FOCUS))
// 	{
// 		this->UpdateObject();
// 	}
// 	else
	if (m_pButton)
	{
		if (nMask&OSB_DISABLE)
		{
			m_pButton->SetEnable(m_pISliderCtrl->IsEnable());
		}
	}
}

void SliderCtrl::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	SetMsgHandled(FALSE);

	bool bOldAutoTicks = (lpStyleStruct->styleOld & SLIDER_STYLE_AUTOTICKS)?true:false;
	bool bOldShowTicks = (lpStyleStruct->styleOld & SLIDER_STYLE_SHOWTICKS)?true:false;
	bool bNewAutoTicks = (lpStyleStruct->styleNew & SLIDER_STYLE_AUTOTICKS)?true:false;
	bool bNewShowTicks = (lpStyleStruct->styleNew & SLIDER_STYLE_SHOWTICKS)?true:false;

	if (bNewAutoTicks && !bOldAutoTicks)  // 自动显示刻度
	{
		this->UpdateTicksData();
	}
// 	else if (bNewShowTicks && (bOldAutoTicks&&!bNewAutoTicks))  // 去除了自动显示刻度，使用自定义刻度
// 	{
// 
// 	}
// 	else if (bNewShowTicks && !bOldShowTicks)  // 显示自定义刻度
// 	{
// 
// 	}
// 	else   // 不显示刻度
// 	else
// 	{
// 	}

}


int SliderCtrl::SetTickFreq(int nFreq)
{
	int nOldFreq = m_nFreq;
	m_nFreq = nFreq;
	
	this->UpdateTicksData();
	return nOldFreq;
}

// 重新计算每个刻度的位置
void SliderCtrl::UpdateTicksData()
{
    int nStyle = m_pISliderCtrl->GetStyle();
	if (!(nStyle & SLIDER_STYLE_AUTOTICKS))
	{
		return ;
	}

	for (int i = 0; i < (int)m_vTickInfo.size(); i++)
	{
		delete m_vTickInfo[i];
	}
	m_vTickInfo.clear();

	if (nStyle & SLIDER_STYLE_AUTOTICKS)
	{
		for (int i = m_nMin; i<= m_nMax; i+= m_nFreq)
		{
			TickInfo* pt = new TickInfo;
			pt->nPos = i;
			
			// TODO: 区分方向
			int nRange = GetRange();
			if (nRange == 0)
				nRange = 1;
			int nx = (int)( (double)(i-m_nMin)/(double)nRange * (double)(m_nEnd-m_nBegin))+ m_nBegin;
			pt->rcRect.left = nx;
			pt->rcRect.top = 26;
			pt->rcRect.right = nx+1;
			pt->rcRect.bottom = 28;

			m_vTickInfo.push_back(pt);
		}
	}
}

void SliderCtrl::OnPaint(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return;

	// 绘制进度条
    IRenderBase* pForegndRender = m_pISliderCtrl->GetForeRender();
	if (pForegndRender)
	{
		if (PROGRESS_SCROLL_LEFT_2_RIGHT == m_eDirectionType)
		{
			// 将前景拉伸绘制在背景上面，但只在按钮左侧显示出来，因此在这做一个clip
			CRect rcBtnWindow(0,0,0,0);
			CRect rcThisWindow(0,0,0,0);

			if (m_pButton)
				this->m_pButton->GetWindowRect(&rcBtnWindow);

			m_pISliderCtrl->GetWindowRect(&rcThisWindow);
			HRGN hRgnClip = ::CreateRectRgn(rcThisWindow.left,rcThisWindow.top,rcBtnWindow.left,rcThisWindow.bottom);

			HRGN hRgnOld = pRenderTarget->GetClipRgn();
			pRenderTarget->SelectClipRgn(hRgnClip);

			CRect rc(0,0,0,0);
			SIZE s = pForegndRender->GetDesiredSize();
			if (s.cx == 0 || s.cy == 0)
			{
				rc.right = m_pISliderCtrl->GetWidth();
				rc.bottom = m_pISliderCtrl->GetHeight();
			}
			else
			{
				int yCenter = (m_pISliderCtrl->GetHeight() - s.cy) / 2;

				rc.top = yCenter;
				rc.bottom = rc.top + s.cy;
				rc.right = m_pISliderCtrl->GetWidth();
			}
			pForegndRender->DrawState(pRenderTarget, &rc, 0);

			::DeleteObject(hRgnClip);
			pRenderTarget->SelectClipRgn(hRgnOld);
			if (hRgnOld)
			{
				::DeleteObject(hRgnOld);
				hRgnOld = NULL;
			}
		}
		else if (m_eDirectionType == PROGRESS_SCROLL_BOTTOM_2_TOP)
		{
			// 将前景拉伸绘制在背景上面，但只在按钮下侧显示出来，因此在这做一个clip
			CRect rcBtnWindow(0,0,0,0);
			CRect rcThisWindow(0,0,0,0);
			this->m_pButton->GetWindowRect(&rcBtnWindow);
			m_pISliderCtrl->GetWindowRect(&rcThisWindow);
			HRGN hRgnClip = ::CreateRectRgn(rcThisWindow.left,rcBtnWindow.top,rcThisWindow.right,rcThisWindow.bottom);

			HRGN hRgnOld = pRenderTarget->GetClipRgn();
			pRenderTarget->SelectClipRgn(hRgnClip);

			CRect rc(0,0,0,0);
			SIZE s = pForegndRender->GetDesiredSize();
			if (s.cx == 0 || s.cy == 0)
			{
				rc.right = m_pISliderCtrl->GetWidth();
				rc.bottom = m_pISliderCtrl->GetHeight();
			}
			else
			{
				int xCenter = (m_pISliderCtrl->GetWidth() - s.cx) / 2;

				rc.left = xCenter;
				rc.right = rc.left + s.cx;
				rc.bottom = m_pISliderCtrl->GetHeight();
			}
			pForegndRender->DrawState(pRenderTarget, &rc, 0);

			::DeleteObject(hRgnClip);
			pRenderTarget->SelectClipRgn(hRgnOld);
			if (hRgnOld)
			{
				::DeleteObject(hRgnOld);
				hRgnOld = NULL;
			}
		}
		else if (PROGRESS_SCROLL_RIGHT_2_LEFT == m_eDirectionType)
		{
			// 将前景拉伸绘制在背景上面，但只在按钮左侧显示出来，因此在这做一个clip
// 			CRect rcBtnWindow(0,0,0,0);
// 			CRect rcThisWindow(0,0,0,0);
// 
// 			if (m_pButton)
// 				this->m_pButton->GetWindowRect(&rcBtnWindow);
// 
// 			this->GetWindowRect(&rcThisWindow);
// 			HRGN hRgnClip = ::CreateRectRgn(rcThisWindow.left,rcThisWindow.top,rcBtnWindow.left,rcThisWindow.bottom);
// 
// 			HRGN hRgnOld = GetClipRgn(pRenderTarget);
// 			SelectClipRgn(pRenderTarget, hRgnClip);
// 
// 			CRect rc(0,0,0,0);
// 			SIZE s = m_pForegndRender->GetDesiredSize();
// 			if (s.cx == 0 || s.cy == 0)
// 			{
// 				rc.right = GetWidth();
// 				rc.bottom = GetHeight();
// 			}
// 			else
// 			{
// 				int yCenter = (this->GetHeight() - s.cy) / 2;
// 
// 				rc.top = yCenter;
// 				rc.bottom = rc.top + s.cy;
// 				rc.right = this->GetWidth();
// 			}
// 			this->m_pForegndRender->DrawState(pRenderTarget, &rc, 0);
// 
// 			::DeleteObject(hRgnClip);
// 			::SelectClipRgn(pRenderTarget,hRgnOld);
// 			if (NULL != hRgnOld)
// 			{
// 				::DeleteObject(hRgnOld);
// 				hRgnOld = NULL;
// 			}
		}
		else if (m_eDirectionType == PROGRESS_SCROLL_TOP_2_BOTTOM)
		{
			// 将前景拉伸绘制在背景上面，但只在按钮下侧显示出来，因此在这做一个clip
// 			CRect rcBtnWindow(0,0,0,0);
// 			CRect rcThisWindow(0,0,0,0);
// 			this->m_pButton->GetWindowRect(&rcBtnWindow);
// 			this->GetWindowRect(&rcThisWindow);
// 			HRGN hRgnClip = ::CreateRectRgn(rcThisWindow.left,rcBtnWindow.top,rcThisWindow.right,rcThisWindow.bottom);
// 
// 			HRGN hRgnOld = GetClipRgn(pRenderTarget);
// 			SelectClipRgn(pRenderTarget, hRgnClip);
// 
// 			CRect rc(0,0,0,0);
// 			SIZE s = m_pForegndRender->GetDesiredSize();
// 			if (s.cx == 0 || s.cy == 0)
// 			{
// 				rc.right = GetWidth();
// 				rc.bottom = GetHeight();
// 			}
// 			else
// 			{
// 				int xCenter = (this->GetWidth() - s.cx) / 2;
// 
// 				rc.left = xCenter;
// 				rc.right = rc.left + s.cx;
// 				rc.bottom = GetHeight();
// 			}
// 			this->m_pForegndRender->DrawState(pRenderTarget, &rc, 0);
// 
// 			::DeleteObject(hRgnClip);
// 			::SelectClipRgn(pRenderTarget,hRgnOld);
// 			if (NULL != hRgnOld)
// 			{
// 				::DeleteObject(hRgnOld);
// 				hRgnOld = NULL;
// 			}
		}
		else
		{
			UIASSERT(0);
		}
	}

	// 绘制刻度 TODO:
//	if (m_pTickRender)
	{
        int nStyle = m_pISliderCtrl->GetStyle();
		if (nStyle & SLIDER_STYLE_AUTOTICKS || nStyle & SLIDER_STYLE_SHOWTICKS)
		{
			for (int i = 0; i < (int)m_vTickInfo.size(); i++)
			{
                Color c(128,128,128,255);
				pRenderTarget->FillRect(&(m_vTickInfo[i]->rcRect), &c);
			}
		}		
	}

	// 焦点. TODO 拦截button的focus change msg
// 	if (this->IsFocus() || (m_pButton!=NULL && m_pButton->IsFocus()))
// 	{
// 		CRect rc;
// 		this->GetClientRectAsWin32(&rc);
// 		DrawFocusRect( pRenderTarget, &rc);
// 	}
}

HRESULT  SliderCtrl::FinalConstruct(IUIApplication* p)
{
	DO_PARENT_PROCESS(ISliderCtrl, IControl);

    IButton::CreateInstance(m_pISliderCtrl->GetUIApplication(), &m_pButton);
	m_pISliderCtrl->AddChild(m_pButton);
    m_pButton->SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);  // 因为默认按钮是CONTENT autosize的，这里的滑块按钮没有内容，需要按背景进行auto size
	m_pButton->SetButtonStyle(BUTOTN_STYLE_SLIDERTRACKBUTTON);

	// 对按钮的消息进行拦截，统一在父类中处理 
	m_pButton->AddHook(m_pISliderCtrl, 0, 1);

	return S_OK;
}

// 因为有可能min=256, max=0，这个时候判断当前值的范围就得另外判断
int  SliderCtrl::FixCurValue(int nCur)
{
    int nRet = nCur;
    if (m_nMin < m_nMax)
    {
        if (nCur < m_nMin)
            nRet = m_nMin;
        else if (m_nCur > m_nMax)
            nRet = m_nMax;
    }
    else
    {
        if (nCur < m_nMax)
            nRet = m_nMax;
        else if (m_nCur > m_nMin)
            nRet = m_nMin;
    }

    return nRet;
}