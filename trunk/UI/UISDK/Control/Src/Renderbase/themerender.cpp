#include "stdafx.h"
#include "themerender.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"
#include "UISDK\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Control\Inc\Interface\iprogressctrl.h"
#include "UISDK\Control\Inc\Interface\isliderctrl.h"
#include "UISDK\Control\Inc\Interface\imenu.h"

#pragma comment(lib, "uxtheme.lib")

namespace UI
{
//////////////////////////////////////////////////////////////////////////

void  ButtonBkThemeRender::OnDrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = this->GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, BP_PUSHBUTTON, GetThemeStateId(pDrawStruct->nState), &pDrawStruct->rc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
    }
    else
    {
        DrawFrameControl(hDC, (RECT*)&pDrawStruct->rc, DFC_BUTTON, GetNoThemeStateId(pDrawStruct->nState));
    }
    pRenderTarget->ReleaseHDC(hDC);
}


int  ButtonBkThemeRender::GetThemeStateId(int nState)
{
    switch (nState)
    {
    case BUTTON_BKGND_RENDER_STATE_NORMAL:
        return GetObject()->IsDefault() ? PBS_DEFAULTED:PBS_NORMAL;

    case BUTTON_BKGND_RENDER_STATE_PRESS:
        return PBS_PRESSED;

    case BUTTON_BKGND_RENDER_STATE_HOVER:
        return PBS_HOT;

    case BUTTON_BKGND_RENDER_STATE_DISABLE:
        return PBS_DISABLED;

    case BUTTON_BKGND_RENDER_STATE_DEFAULT:
        return PBS_DEFAULTED;
    }
    return PBS_NORMAL;
}
UINT  ButtonBkThemeRender::GetNoThemeStateId(int nState)
{
    switch (nState)
    {
    case BUTTON_BKGND_RENDER_STATE_NORMAL:
        return DFCS_BUTTONPUSH;

    case BUTTON_BKGND_RENDER_STATE_HOVER:
        return DFCS_BUTTONPUSH|DFCS_HOT;

    case BUTTON_BKGND_RENDER_STATE_PRESS:
        return DFCS_BUTTONPUSH|DFCS_PUSHED;

    case BUTTON_BKGND_RENDER_STATE_DISABLE:
        return DFCS_BUTTONPUSH|DFCS_INACTIVE;
    }

    return DFCS_BUTTONPUSH;
}

//////////////////////////////////////////////////////////////////////////
  
void  RadioButtonThemeRender::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = 12;
    pSize->cy = 12;

	if (GetTHEME())
	{
		HDC hDC = GetObject()->GetUIApplication()->GetCacheDC();
		GetThemePartSize(GetTHEME(), hDC, BP_RADIOBUTTON,RBS_UNCHECKEDNORMAL, NULL, TS_DRAW, pSize);
		GetObject()->GetUIApplication()->ReleaseCacheDC(hDC);
	}
}
void  RadioButtonThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    HDC hDC = pDrawStruct->pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        RECT rc ;
        CopyRect(&rc, &pDrawStruct->rc);

        rc.top -= 1;
        rc.bottom += 1;

        HRESULT hr = DrawThemeBackground(hTheme, hDC, BP_RADIOBUTTON, GetThemeStateId(pDrawStruct->nState), &rc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
    }
    else
    {
        DrawFrameControl(hDC, (RECT*)&pDrawStruct->rc, DFC_BUTTON, GetNoThemeStateId(pDrawStruct->nState));
    }
    pDrawStruct->pRenderTarget->ReleaseHDC(hDC);
}

int  RadioButtonThemeRender::GetThemeStateId(int nState)
{
	switch(nState)
	{
	case BUTTON_ICON_RENDER_STATE_NORMAL:
        return RBS_UNCHECKEDNORMAL;

    case BUTTON_ICON_RENDER_STATE_HOVER:
		return RBS_UNCHECKEDHOT;

	case BUTTON_ICON_RENDER_STATE_PRESS:
        return RBS_UNCHECKEDPRESSED;        

	case BUTTON_ICON_RENDER_STATE_DISABLE:
        return CBS_UNCHECKEDDISABLED;

    case BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:
        return RBS_CHECKEDNORMAL;

    case BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:
		return RBS_CHECKEDHOT;

	case BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:
		return RBS_CHECKEDPRESSED;

	case BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:
        return RBS_CHECKEDDISABLED;
	}

    return RBS_UNCHECKEDNORMAL;
}

UINT  RadioButtonThemeRender::GetNoThemeStateId(int nState)
{
    switch(nState)
    {
    case BUTTON_ICON_RENDER_STATE_NORMAL:
        return DFCS_BUTTONRADIO;

    case BUTTON_ICON_RENDER_STATE_HOVER:
        return DFCS_BUTTONRADIO|DFCS_HOT;

    case BUTTON_ICON_RENDER_STATE_PRESS:
        return DFCS_BUTTONRADIO|DFCS_PUSHED;

    case BUTTON_ICON_RENDER_STATE_DISABLE:
        return DFCS_BUTTONRADIO|DFCS_INACTIVE;

    case BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:
        return DFCS_BUTTONRADIO|DFCS_CHECKED;

    case BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:
        return DFCS_BUTTONRADIO|DFCS_HOT|DFCS_CHECKED;

    case BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:
        return DFCS_BUTTONRADIO|DFCS_PUSHED|DFCS_CHECKED;

    case BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:
        return DFCS_BUTTONRADIO|DFCS_INACTIVE|DFCS_CHECKED;
    }
    return DFCS_BUTTONRADIO;
}

//////////////////////////////////////////////////////////////////////////

void  CheckButtonThemeRender::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 13;

	if (GetTHEME())
	{
		HDC hDC = GetObject()->GetUIApplication()->GetCacheDC();
		GetThemePartSize(GetTHEME(), hDC, BP_CHECKBOX,CBS_UNCHECKEDNORMAL, NULL, TS_DRAW, pSize );
		GetObject()->GetUIApplication()->ReleaseCacheDC(hDC);
	}
}

void  CheckButtonThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (NULL == pDrawStruct->pRenderTarget)
        return;

    HDC hDC = pDrawStruct->pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        RECT rc;
        ::CopyRect(&rc, &pDrawStruct->rc);
        rc.top -= 1;
        rc.bottom += 1;
        HRESULT hr = DrawThemeBackground(hTheme, hDC, BP_CHECKBOX,  GetThemeStateId(pDrawStruct->nState), &rc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
    }
    else
    {
        UINT nFlag = DFCS_BUTTONCHECK|DFCS_INACTIVE;
        DrawFrameControl(hDC, (RECT*)&pDrawStruct->rc, DFC_BUTTON, GetNoThemeStateId(pDrawStruct->nState));
    }
    pDrawStruct->pRenderTarget->ReleaseHDC(hDC);
}

int  CheckButtonThemeRender::GetThemeStateId(int nState)
{
    switch(nState)
    {
    case BUTTON_ICON_RENDER_STATE_NORMAL:
        return CBS_UNCHECKEDNORMAL;

    case BUTTON_ICON_RENDER_STATE_HOVER:
        return CBS_UNCHECKEDHOT;

    case BUTTON_ICON_RENDER_STATE_PRESS:
        return CBS_UNCHECKEDPRESSED;

    case BUTTON_ICON_RENDER_STATE_DISABLE:
        return CBS_UNCHECKEDDISABLED;

    case BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:
        return CBS_CHECKEDNORMAL;

    case BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:
        return CBS_CHECKEDHOT;

    case BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:
        return CBS_CHECKEDPRESSED;

    case BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:
        return CBS_CHECKEDDISABLED;
    }

    return CBS_UNCHECKEDNORMAL;
}
UINT  CheckButtonThemeRender::GetNoThemeStateId(int nState)
{
    switch(nState)
    {
    case BUTTON_ICON_RENDER_STATE_NORMAL:
        return DFCS_BUTTONCHECK;

    case BUTTON_ICON_RENDER_STATE_HOVER:
        return DFCS_BUTTONCHECK|DFCS_HOT;

    case BUTTON_ICON_RENDER_STATE_PRESS:
        return DFCS_BUTTONCHECK|DFCS_PUSHED;

    case BUTTON_ICON_RENDER_STATE_DISABLE:
        return DFCS_BUTTONCHECK|DFCS_INACTIVE;

    case BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:
        return DFCS_BUTTONCHECK|DFCS_CHECKED;

    case BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:
        return DFCS_BUTTONCHECK|DFCS_HOT|DFCS_CHECKED;

    case BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:
        return DFCS_BUTTONCHECK|DFCS_PUSHED|DFCS_CHECKED;

    case BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:
        return DFCS_BUTTONCHECK|DFCS_INACTIVE|DFCS_CHECKED;
    }

    return DFCS_BUTTONCHECK;
}

//////////////////////////////////////////////////////////////////////////

void  EditBkThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (NULL == pDrawStruct->pRenderTarget)
        return;

    HDC hDC = pDrawStruct->pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, EP_EDITBORDER_NOSCROLL, GetThemeStateId(pDrawStruct->nState), (RECT*)&pDrawStruct->rc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("EditBkThemeRender::DrawDisable  DrawThemeBackground failed."));
    }
    else
    {
        if (pDrawStruct->nState == EDIT_BKGND_RENDER_STATE_DISABLE)
        {
            DrawEdge(hDC, (RECT*)&pDrawStruct->rc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
        }
        else
        {
            if (GetObject()->IsReadonly())
            {
                DrawEdge(hDC, (RECT*)&pDrawStruct->rc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
            }
            else
            {
                ::FillRect(hDC, (RECT*)&pDrawStruct->rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
                DrawEdge(hDC, (RECT*)&pDrawStruct->rc, EDGE_SUNKEN, BF_RECT);
            }
        }
    }
    pDrawStruct->pRenderTarget->ReleaseHDC(hDC);
}

int  EditBkThemeRender::GetThemeStateId(int nState)
{
    switch(nState)
    {
    case EDIT_BKGND_RENDER_STATE_NORMAL:
        return ETS_CUEBANNER;

    case EDIT_BKGND_RENDER_STATE_DISABLE:
        return ETS_DISABLED;

    case EDIT_BKGND_RENDER_STATE_PRESS:
        return ETS_SELECTED;

    case EDIT_BKGND_RENDER_STATE_HOVER:
        return ETS_HOT;
    }

    return ETS_CUEBANNER;
}


//////////////////////////////////////////////////////////////////////////

void GroupBoxBkThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
//     RECT rc;
//     ((IGroupBox*)m_pObject)->GetBorderRect(&rc);

    HDC hDC = pDrawStruct->pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        int iStateId = GBS_NORMAL;
        if (pDrawStruct->nState == GROUPBOX_BKGND_RENDER_STATE_DISABLE)
            iStateId = GBS_DISABLED;

        HRESULT hr = DrawThemeBackground(hTheme, hDC, BP_GROUPBOX, iStateId, &pDrawStruct->rc, 0);
        if ( S_OK != hr )
        {
            UI_LOG_WARN(_T("GroupBoxBkThemeRender::DrawDisable  DrawThemeBackground failed."));
        }
    }
    else
    {
        if (pDrawStruct->rc.Height()<=2)
            DrawEdge(hDC, &pDrawStruct->rc, EDGE_ETCHED, BF_TOP);   // 分隔线类型
        else
            DrawEdge(hDC, &pDrawStruct->rc, EDGE_ETCHED, BF_RECT);
    }
    pDrawStruct->pRenderTarget->ReleaseHDC(hDC);
}


//////////////////////////////////////////////////////////////////////////

void  ProgressCtrlBkgndThemeRender::OnInit()
{
    SetMsgHandled(FALSE);
    if (GetObject())
    {
        m_pIProgressCtrl = (IProgressCtrl*)GetObject()->QueryInterface(uiiidof(IProgressCtrl));
    }
}

void  ProgressCtrlBkgndThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    CRect* prc = &pDrawStruct->rc;
    int& nState = pDrawStruct->nState;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        int iPartId = PP_BAR;
        if (m_pIProgressCtrl)
        {
            PROGRESS_SCROLL_DIRECTION_TYPE eType = m_pIProgressCtrl->GetDirectionType();
            if (PROGRESS_SCROLL_BOTTOM_2_TOP == eType || PROGRESS_SCROLL_TOP_2_BOTTOM == eType)
            {
                iPartId = PP_BARVERT;
            }
        }
        HRESULT hr = DrawThemeBackground(hTheme, hDC, iPartId, PBFS_NORMAL, (RECT*)prc, 0);
        if ( S_OK != hr )
        {
            UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {
        // TODO: 有没有更直接的方法来绘制这种背景边缘？
        DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_SOFT|BF_RIGHT|BF_BOTTOM|BF_MIDDLE);
        CRect rc(prc);
        rc.right--;
        rc.bottom--;
        DrawEdge(hDC, (RECT*)&rc, EDGE_SUNKEN, BF_FLAT|BF_LEFT|BF_TOP);
    }
    pRenderTarget->ReleaseHDC(hDC);
}


void  ProgressCtrlForegndThemeRender::OnInit()
{
    SetMsgHandled(FALSE);
    if (GetObject())
    {
        m_pIProgressCtrl = (IProgressCtrl*)GetObject()->QueryInterface(uiiidof(IProgressCtrl));
    }
}
void  ProgressCtrlForegndThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    int& nState = pDrawStruct->nState;
    CRect& rc = pDrawStruct->rc;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        if (GetObject()->GetUIApplication()->IsUnderXpOS())
        {
            rc.DeflateRect(3,3,3,3);
        }
        else
        {
            //rc.DeflateRect(1,1,1,1);
        }

        // 判断该进度条是否是纵向的
        bool bVert = false;
        if (m_pIProgressCtrl)
        {
            PROGRESS_SCROLL_DIRECTION_TYPE eType = m_pIProgressCtrl->GetDirectionType();
            if (PROGRESS_SCROLL_BOTTOM_2_TOP == eType || PROGRESS_SCROLL_TOP_2_BOTTOM == eType)
            {
                bVert = true;
            }
        }

        HRESULT hr = DrawThemeBackground(hTheme, hDC, bVert?PP_FILLVERT:PP_FILL, PBFS_NORMAL, (RECT*)&rc, 0);
        if ( S_OK != hr )
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
        //hr = DrawThemeBackground(m_hTheme, hDC, PP_MOVEOVERLAY, PBFS_NORMAL, (RECT*)&rc, 0);
    }
    else
    {
        HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_HIGHLIGHT));
        rc.DeflateRect(2,2,2,2);

        PROGRESS_SCROLL_DIRECTION_TYPE eType = PROGRESS_SCROLL_LEFT_2_RIGHT;
        if (m_pIProgressCtrl)
        {
            eType = m_pIProgressCtrl->GetDirectionType();
        }

        switch (eType)
        {
        case PROGRESS_SCROLL_RIGHT_2_LEFT:
            for (int i = rc.right; i > rc.left; )
            {
                int j = i-12;
                if (j < rc.left)
                    j = rc.left;

                CRect rcItem(j,rc.top,i,rc.bottom);
                ::FillRect(hDC, &rcItem, hBrush);

                i -= 14;
            }
            break;

        case PROGRESS_SCROLL_BOTTOM_2_TOP:
            for (int i = rc.bottom; i > rc.top; )
            {
                int j = i-12;
                if (j < rc.left)
                    j = rc.left;

                CRect rcItem(rc.left,j,rc.right, i);
                ::FillRect(hDC, &rcItem, hBrush);

                i -= 14;
            }
            break;

        case PROGRESS_SCROLL_TOP_2_BOTTOM:
            for (int i = rc.top; i < rc.bottom; )
            {
                int j = i+12;
                if (j > rc.bottom)
                    j = rc.bottom;

                CRect rcItem(rc.left, i, rc.right, j);
                ::FillRect(hDC, &rcItem, hBrush);

                i += 14;
            }
            break;

        default:
            for (int i = rc.left; i < rc.right; )
            {
                int j = i+12;
                if (j > rc.right)
                    j = rc.right;

                CRect rcItem(i,rc.top,j,rc.bottom);
                ::FillRect(hDC, &rcItem, hBrush);

                i += 14;
            }
            break;
        }
        ::DeleteObject(hBrush);
    }
    pRenderTarget->ReleaseHDC(hDC);
}



void SliderCtrlBkgndThemeRender::OnInit()
{
    SetMsgHandled(FALSE);
    if (GetObject())
    {
        m_pISliderCtrl = (ISliderCtrl*)GetObject()->QueryInterface(uiiidof(ISliderCtrl));
    }
}
void SliderCtrlBkgndThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    CRect& rc = pDrawStruct->rc;
    
    PROGRESS_SCROLL_DIRECTION_TYPE eType = PROGRESS_SCROLL_LEFT_2_RIGHT;
    if (m_pISliderCtrl)
        eType = m_pISliderCtrl->GetDirectionType();

    switch(eType)
    {
    case PROGRESS_SCROLL_TOP_2_BOTTOM:
    case PROGRESS_SCROLL_BOTTOM_2_TOP:
        {
            rc.left += (rc.Width()-4)/2;
            rc.right = rc.left + 4;
        }
        break;

    default:
        {
            rc.top += (rc.Height()-4)/2;
            rc.bottom = rc.top + 4;
        }
        break;
    }

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, TKP_TRACK, TKS_NORMAL, (RECT*)&rc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
    }
    else
    {
        DrawEdge(hDC, (RECT*)&rc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

void SliderTrackButtonThemeRender::OnInit()
{
    SetMsgHandled(FALSE);
    if (NULL == GetObject())
        return;

    IObject* pParent = GetObject()->GetParentObject();
    if (pParent)
    {
        m_pISliderCtrl = (ISliderCtrl*)GetObject()->QueryInterface(uiiidof(ISliderCtrl));
    }
}

void  SliderTrackButtonThemeRender::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = 11;
    pSize->cy = 22;

    PROGRESS_SCROLL_DIRECTION_TYPE eType = PROGRESS_SCROLL_LEFT_2_RIGHT;
    if (m_pISliderCtrl)
        eType = m_pISliderCtrl->GetDirectionType();

    switch(eType)
    {
    case PROGRESS_SCROLL_TOP_2_BOTTOM:
    case PROGRESS_SCROLL_BOTTOM_2_TOP:
        {
            pSize->cx = 22;
            pSize->cy = 11;
            return;
        }

    default:
        {
            pSize->cx = 11;
            pSize->cy = 22;
            return;
        }
    }
}

void SliderTrackButtonThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;

    if (GetTHEME())
    {
        this->DrawThemeState(pRenderTarget, &pDrawStruct->rc, pDrawStruct->nState);
        return;
    }
    else
    {
        HDC hDC = pRenderTarget->GetHDC();
        this->DrawNoThemeState(hDC, &pDrawStruct->rc, pDrawStruct->nState);
        pRenderTarget->ReleaseHDC(hDC);
    }
}

int  SliderTrackButtonThemeRender::GetDrawThemeStateID(int iPartID, int nDrawState)
{
    switch(iPartID)
    {
    case TKP_THUMB:
        {
            switch(nDrawState)
            {
            case BUTTON_BKGND_RENDER_STATE_DISABLE:
                return TUS_DISABLED;
            case BUTTON_BKGND_RENDER_STATE_PRESS:
                return TUS_PRESSED;
            case BUTTON_BKGND_RENDER_STATE_HOVER:
                return TUS_HOT;
            default:
                return TUS_NORMAL;
            }
        }
        break;

    case TKP_THUMBBOTTOM:
        {
            switch(nDrawState)
            {
            case BUTTON_BKGND_RENDER_STATE_DISABLE:
                return TUBS_DISABLED;
            case BUTTON_BKGND_RENDER_STATE_PRESS:
                return TUBS_PRESSED;
            case BUTTON_BKGND_RENDER_STATE_HOVER:
                return TUBS_HOT;
            default:
                return TUBS_NORMAL;
            }
        }
        break;

    case TKP_THUMBTOP:
        {
            switch(nDrawState)
            {
            case BUTTON_BKGND_RENDER_STATE_DISABLE:
                return TUTS_DISABLED;
            case BUTTON_BKGND_RENDER_STATE_PRESS:
                return TUTS_PRESSED;
            case BUTTON_BKGND_RENDER_STATE_HOVER:
                return TUTS_HOT;
            default:
                return TUTS_NORMAL;
            }
        }
        break;

    case TKP_THUMBVERT:
        {
            switch(nDrawState)
            {
            case BUTTON_BKGND_RENDER_STATE_DISABLE:
                return TUVS_DISABLED;
            case BUTTON_BKGND_RENDER_STATE_PRESS:
                return TUVS_PRESSED;
            case BUTTON_BKGND_RENDER_STATE_HOVER:
                return TUVS_HOT;
            default:
                return TUVS_NORMAL;
            }
        }
        break;

    case TKP_THUMBLEFT:
        {
            switch(nDrawState)
            {
            case BUTTON_BKGND_RENDER_STATE_DISABLE:
                return TUVLS_DISABLED;
            case BUTTON_BKGND_RENDER_STATE_PRESS:
                return TUVLS_PRESSED;
            case BUTTON_BKGND_RENDER_STATE_HOVER:
                return TUVLS_HOT;
            default:
                return TUVLS_NORMAL;
            }
        }
        break;

    case TKP_THUMBRIGHT:
        {
            switch(nDrawState)
            {
            case BUTTON_BKGND_RENDER_STATE_DISABLE:
                return TUVRS_DISABLED;
            case BUTTON_BKGND_RENDER_STATE_PRESS:
                return TUVRS_PRESSED;
            case BUTTON_BKGND_RENDER_STATE_HOVER:
                return TUVRS_HOT;
            default:
                return TUVRS_NORMAL;
            }
        }
        break;
    }
    return 0;
}

void SliderTrackButtonThemeRender::DrawThemeState(IRenderTarget* pRenderTarget, const CRect* prc, int nDrawState)
{
    CRect rc(prc);
    HDC hDC = pRenderTarget->GetHDC();

    int iPart = TKP_THUMB;
    int iState = TKS_NORMAL;
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        PROGRESS_SCROLL_DIRECTION_TYPE eType = PROGRESS_SCROLL_LEFT_2_RIGHT;
        bool bPointLeftTop = false;
        bool bPointRightBottom = false;
        if (m_pISliderCtrl)
        {
            eType = m_pISliderCtrl->GetDirectionType();
            bPointLeftTop = m_pISliderCtrl->TestStyle(SLIDER_STYLE_POINT_LEFT);
            bPointRightBottom = m_pISliderCtrl->TestStyle(SLIDER_STYLE_POINT_RIGHT);
        }

        switch(eType)
        {
        case PROGRESS_SCROLL_TOP_2_BOTTOM:
        case PROGRESS_SCROLL_BOTTOM_2_TOP:
            {
                if (bPointLeftTop)
                {
                    iPart = TKP_THUMBLEFT;
                }
                else if (bPointRightBottom)
                {
                    iPart = TKP_THUMBRIGHT;
                }
                else
                    iPart = TKP_THUMBVERT;
            }
            break;

        default:
            {

                if (bPointLeftTop)
                {
                    iPart = TKP_THUMBTOP;
                }
                else if (bPointRightBottom)
                {
                    iPart = TKP_THUMBBOTTOM;
                }
                else
                    iPart = TKP_THUMB;
            }
            break;
        }
        int nStateID = this->GetDrawThemeStateID(iPart, nDrawState);
        HRESULT hr = DrawThemeBackground(hTheme, hDC, iPart, nStateID, (RECT*)&rc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
    }
    pRenderTarget->ReleaseHDC(hDC);
}
void SliderTrackButtonThemeRender::DrawNoThemeState(HDC hDC, const CRect* prc, int nDrawState)
{
    // TODO: 又没有直接的方法来绘制。。。 NND，随便画吧
    CRect rc(prc);

    HBRUSH hBrush = ::CreateSolidBrush(RGB(212,208,200));
    ::FillRect(hDC, &rc, hBrush);
    ::DeleteObject(hBrush);

    rc.bottom -= 7;  // TODO: 这个会导致箭头不透明
    
    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    ::MoveToEx(hDC,rc.left, rc.bottom,NULL);
    ::LineTo(hDC, rc.left, rc.top);
    ::LineTo(hDC, rc.right, rc.top);
    ::SelectObject(hDC, hOldPen);
    DeleteObject(hPen);

    rc.right--;
    hPen = CreatePen(PS_SOLID, 1, RGB(64,64,64));
    hOldPen = (HPEN)::SelectObject(hDC, hPen);
    ::MoveToEx(hDC,rc.right, rc.top,NULL);
    ::LineTo(hDC, rc.right, rc.bottom);
    ::SelectObject(hDC, hOldPen);
    DeleteObject(hPen);

    rc.right--;
    hPen = CreatePen(PS_SOLID, 1, RGB(128,128,128));
    hOldPen = (HPEN)::SelectObject(hDC, hPen);
    ::MoveToEx(hDC,rc.right, rc.top+1,NULL);
    ::LineTo(hDC, rc.right, rc.bottom);
    ::LineTo(hDC, rc.right+2, rc.bottom);

    rc.bottom++;
    rc.left++;

    ::SetPixel(hDC, rc.left++,  rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.left,    rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.right--, rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.right,   rc.bottom++, RGB(128,128,128));

    ::SetPixel(hDC, rc.left++,  rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.left,    rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.right--, rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.right,   rc.bottom++, RGB(128,128,128));

    ::SetPixel(hDC, rc.left++,  rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.left,    rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.right--, rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.right,   rc.bottom++, RGB(128,128,128));

    ::SetPixel(hDC, rc.left++,  rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.left,    rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.right--, rc.bottom, RGB(128,128,128));
    ::SetPixel(hDC, rc.right,   rc.bottom++, RGB(128,128,128));

    ::SetPixel(hDC, rc.right, rc.bottom, RGB(0,0,0));

    ::SelectObject(hDC, hOldPen);
    DeleteObject(hPen);
}

//////////////////////////////////////////////////////////////////////////

void HeaderCtrlItemBkgndThemeRender::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
    if (NULL == GetTHEME())
    {
        pSize->cy = 20;
    }
    else
    {
        pSize->cy = 24;
    }
}
void HeaderCtrlItemBkgndThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    CRect* prc = &pDrawStruct->rc;
    int& nState = pDrawStruct->nState;

    HEADERITEMSTATES eState = HIS_NORMAL;
    int  nNothemeState = DFCS_BUTTONPUSH;

    if (nState & RENDER_STATE_PRESS)
    {
        eState = HIS_PRESSED;
        nNothemeState = DFCS_BUTTONPUSH|DFCS_FLAT/*DFCS_PUSHED*/;
    }
    else if (nState & RENDER_STATE_HOVER)
    {
        eState = HIS_HOT;
        nNothemeState = DFCS_BUTTONPUSH|DFCS_HOT;
    }
    else if (nState & RENDER_STATE_DISABLE)
    {
        nNothemeState = DFCS_BUTTONPUSH|DFCS_INACTIVE;
    }

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, HP_HEADERITEM, eState, (RECT*)prc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
    }
    else
    {
        DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nNothemeState);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

void HeaderCtrlRightBlankThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    CRect* prc = &pDrawStruct->rc;
    int& nState = pDrawStruct->nState;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        HEADERITEMRIGHTSTATES eState = HIRS_NORMAL;
        if (nState & RENDER_STATE_PRESS)
        {
            eState = HIRS_PRESSED;
        }
        else if (nState & RENDER_STATE_HOVER)
        {
            eState = HIRS_HOT;
        }

        HRESULT hr = DrawThemeBackground(hTheme, hDC, HP_HEADERITEMRIGHT, eState, (RECT*)prc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
    }
    else
    {
        DrawEdge(hDC, (LPRECT)prc, EDGE_RAISED, BF_BOTTOM|BF_LEFT|BF_TOP|BF_MIDDLE);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

void  ScrollBarBkgndThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    HDC hDC = pDrawStruct->pRenderTarget->GetHDC();		
    CRect* prc = &pDrawStruct->rc;

    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, GetThemeType(), SCRBS_NORMAL, prc, 0);
        if (S_OK != hr)
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {
        // TODO: 真有没有直接绘制滚动条背景的API吗
        HBRUSH hBkgndBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE)); /*RGB(212,208,200)*/
        ::FillRect(hDC, prc, hBkgndBrush);
        ::DeleteObject(hBkgndBrush);

        LOGBRUSH    LogBrush;
        LogBrush.lbStyle = BS_SOLID;
        LogBrush.lbColor = RGB(255, 255, 255);
        HPEN hPen = ExtCreatePen(PS_GEOMETRIC | PS_DOT | PS_ENDCAP_FLAT, 1, &LogBrush, 0,0);  // 貌似用普通的createpen创建的width 1 不准确

        HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);

        for (int i=prc->top, j=0; i < prc->bottom; i++,j++)
        {
            int xFrom = j%2==0?prc->left:prc->left+1;
            int xTo   = prc->right;

            ::MoveToEx(hDC, xFrom, i, NULL);
            ::LineTo  (hDC, xTo,   i);
        }
        SelectObject(hDC, hOldPen);
        DeleteObject(hPen);
    }
    pDrawStruct->pRenderTarget->ReleaseHDC(hDC);	
}
void  ScrollBarBkgndThemeRender::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = GetSystemMetrics(SM_CXVSCROLL);
    pSize->cy = GetSystemMetrics(SM_CYVSCROLL); 
}

//////////////////////////////////////////////////////////////////////////


void  ScrollLineButtonBkThemeRender::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = GetSystemMetrics(SM_CXVSCROLL);
    pSize->cy = GetSystemMetrics(SM_CYVSCROLL); 
}

void ScrollLineButtonBkThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    CRect* prc = &pDrawStruct->rc;
    int& nState = pDrawStruct->nState;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        int iStateId = 0;

        HRESULT hr = DrawThemeBackground(hTheme, hDC, SBP_ARROWBTN, GetThemeStateId(nState), prc, 0);
        if ( S_OK != hr )
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {
        DrawFrameControl(hDC, (RECT*)prc, DFC_SCROLL, GetNoThemeStateId(nState));
    }
    pRenderTarget->ReleaseHDC(hDC);
}

int  ScrollLineButtonBkThemeRender::GetThemeStateId(int nState)
{
    switch (nState)
    {
    case BUTTON_BKGND_RENDER_STATE_NORMAL:
        return GetThemeStateNormal();

    case BUTTON_BKGND_RENDER_STATE_HOVER:
        return GetThemeStateHover();

    case BUTTON_BKGND_RENDER_STATE_PRESS:
        return GetThemeStatePress();

    case BUTTON_BKGND_RENDER_STATE_DISABLE:
        return GetThemeStateDisable();
    }

    return 0;
}
int  ScrollLineButtonBkThemeRender::GetNoThemeStateId(int nState)
{
    switch (nState)
    {
    case BUTTON_BKGND_RENDER_STATE_NORMAL:
        return GetNoThemeState();

    case BUTTON_BKGND_RENDER_STATE_HOVER:
        return GetNoThemeState();

    case BUTTON_BKGND_RENDER_STATE_PRESS:
        return DFCS_FLAT|DFCS_PUSHED|GetNoThemeState();

    case BUTTON_BKGND_RENDER_STATE_DISABLE:
        return GetNoThemeState()|DFCS_INACTIVE;
    }

    return 0;
}

void  ScrollThumbButtonThemeRender::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = GetSystemMetrics(SM_CXVSCROLL);
    pSize->cy = GetSystemMetrics(SM_CYVSCROLL);
}


void ScrollThumbButtonThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    CRect* prc = &pDrawStruct->rc;
    int& nState = pDrawStruct->nState;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        int iStateId = GetThemeStateId(nState);
        HRESULT hr = DrawThemeBackground(hTheme, hDC, GetThumbBtmType(), iStateId, prc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
        
        hr = DrawThemeBackground(hTheme, hDC, GetGripperType(), iStateId, prc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
    }
    else
    {
        DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, GetNoThemeStateId(nState));
    }
    pRenderTarget->ReleaseHDC(hDC);
}

int  ScrollThumbButtonThemeRender::GetThemeStateId(int nState)
{
    switch(nState)
    {
    case BUTTON_BKGND_RENDER_STATE_NORMAL:
        return SCRBS_NORMAL;

    case BUTTON_BKGND_RENDER_STATE_HOVER:
        return SCRBS_HOT;

    case BUTTON_BKGND_RENDER_STATE_PRESS:
        return SCRBS_PRESSED;

    case BUTTON_BKGND_RENDER_STATE_DISABLE:
        return SCRBS_DISABLED;   
    }

    return 0;
}
int  ScrollThumbButtonThemeRender::GetNoThemeStateId(int nState)
{
    switch(nState)
    {
    case BUTTON_BKGND_RENDER_STATE_NORMAL:
        return DFCS_BUTTONPUSH;

    case BUTTON_BKGND_RENDER_STATE_HOVER:
        return DFCS_BUTTONPUSH|DFCS_HOT;

    case BUTTON_BKGND_RENDER_STATE_PRESS:
        return DFCS_BUTTONPUSH;

    case BUTTON_BKGND_RENDER_STATE_DISABLE:
        return DFCS_BUTTONPUSH|DFCS_INACTIVE;   
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////

void ListboxBkThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    CRect* prc = &pDrawStruct->rc;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, /*LBCP_BORDER_HVSCROLL*/LBCP_BORDER_NOSCROLL, GetThemeStateId(pDrawStruct->nState), (RECT*)prc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
    }
    else
    {
        if (LISTBOX_BKGND_RENDER_STATE_DISABLE == pDrawStruct->nState)
        {
            DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
        }
        else if (GetObject()->IsReadonly())
        {
            DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
        }
        else
        {
            ::FillRect(hDC, (RECT*)prc, (HBRUSH)GetStockObject(WHITE_BRUSH));
            DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT);
        }
    }
    pRenderTarget->ReleaseHDC(hDC);
}
int  ListboxBkThemeRender::GetThemeStateId(int nState)
{
    switch (nState)
    {
    case LISTBOX_BKGND_RENDER_STATE_NORMAL:
        return LBPSH_NORMAL;

    case LISTBOX_BKGND_RENDER_STATE_HOVER:
        return LBPSH_HOT;

    case LISTBOX_BKGND_RENDER_STATE_PRESS:
        return LBPSH_FOCUSED;

    case LISTBOX_BKGND_RENDER_STATE_DISABLE:
        return LBPSH_DISABLED;
    }

    return 0;
}

//////////////////////////////////////////////////////////////////////////

void  ComboboxButtonBkThemeRender::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = GetTHEME()?17:18;
    pSize->cy = 0;

	//  TODO: 1. 这里一直返回0
	//        2. 有xp中有些主题的button在绘制时，又不用去deflatrect(1,1,1,1)，这个怎么去判断？例如那个仿苹果主题
	// 	if( NULL != m_hTheme )
	// 	{
	// 		HDC hDC = Image::GetCacheDC();
	// 		RECT rect = {0,0,100,50};
	// 		GetThemePartSize(m_hTheme,hDC,CP_DROPDOWNBUTTON,CBXS_NORMAL, &rect, TS_DRAW, &s );
	// 		Image::ReleaseCacheDC(hDC);
	// 	}
}

void ComboboxButtonBkThemeRender::DrawState(RENDERBASE_DRAWSTATE*  pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    //, const CRect* prc, int nState
// 	switch(nState)
// 	{
// 	case BUTTON_BKGND_RENDER_STATE_DISABLE:
// 		this->DrawDisable(pRenderTarget, (CRect*)prc);
// 		break;
// 
// 	case BUTTON_BKGND_RENDER_STATE_PRESS:
// 		this->DrawPress(pRenderTarget, (CRect*)prc);
// 		break;
// 
// 	case BUTTON_BKGND_RENDER_STATE_HOVER:
// 		this->DrawHover(pRenderTarget, (CRect*)prc);
// 		break;;
// 
// 	default:
// 		this->DrawNormal(pRenderTarget, (CRect*)prc);
// 		break;
// 	}

    HDC hDC = pRenderTarget->GetHDC();
    CRect* prc = &pDrawStruct->rc;
    HTHEME hTheme = GetTHEME();

    if (hTheme)
    {
        //rc.DeflateRect(1,1,1,1);
        HRESULT hr = DrawThemeBackground(hTheme, hDC, CP_DROPDOWNBUTTONRIGHT, GetThemeStateId(pDrawStruct->nState), prc, 0);
        if (S_OK != hr)
            UI_LOG_WARN(_T("ComboboxButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
    }
    else
    {
        CRect rc(prc);
        rc.DeflateRect(0,2,2,2);
        //DrawFrameControl(hDC, (RECT*)&rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_INACTIVE );
        //DrawGlyph(hDC, &rc);
        ::DrawFrameControl(hDC, (RECT*)&rc, DFC_SCROLL, GetNoThemeStateId(pDrawStruct->nState));
    }
    pRenderTarget->ReleaseHDC(hDC);
}

int  ComboboxButtonBkThemeRender::GetThemeStateId(int nState)
{
    switch (nState)
    {
    case BUTTON_BKGND_RENDER_STATE_NORMAL:
        return CBXSR_NORMAL;

    case BUTTON_BKGND_RENDER_STATE_HOVER:
        return CBXSR_HOT;

    case BUTTON_BKGND_RENDER_STATE_PRESS:
        return CBXSR_PRESSED;

    case BUTTON_BKGND_RENDER_STATE_DISABLE:
        return CBXSR_DISABLED;
    }
    return 0;
}
int  ComboboxButtonBkThemeRender::GetNoThemeStateId(int nState)
{
    switch (nState)
    {
    case BUTTON_BKGND_RENDER_STATE_NORMAL:
        return DFCS_SCROLLCOMBOBOX;

    case BUTTON_BKGND_RENDER_STATE_HOVER:
        return DFCS_SCROLLCOMBOBOX|DFCS_HOT;

    case BUTTON_BKGND_RENDER_STATE_PRESS:
        return DFCS_SCROLLCOMBOBOX|DFCS_PUSHED|DFCS_FLAT;

    case BUTTON_BKGND_RENDER_STATE_DISABLE:
        return DFCS_SCROLLCOMBOBOX|DFCS_INACTIVE;
    }
    return 0;
}

// 绘制按钮上的箭头 -- 可直接使用DFC_SCROLL + DFCS_SCROLLCOMBOBOX实现COMBOBOX的按钮绘制
// void ComboboxButtonBkThemeRender::DrawGlyph( HDC hDC, const CRect* prc, bool bPressDown )
// {
// 	POINT pt[4] = 
// 	{
// 		{-3,-2}, {3,-2}, {0,1}, {-3,-2}
// 	};
// 
// 	int x = 0; int y = 0;
// 	x = (prc->left + prc->right)/2;
// 	y = (prc->top + prc->bottom)/2;
// 
// 	for (int i = 0; i < 4; i++)
// 	{
// 		pt[i].x += x;
// 		pt[i].y += y;
// 
// 		if(bPressDown)
// 		{
// 		//	pt[i].x += 1;
// 			pt[i].y += 1;
// 		}
// 	}
// 
// 	HBRUSH hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
// 	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBlackBrush);
// 	::Polygon(hDC, pt, 4);
// 	::SelectObject(hDC,hOldBrush);
// }

//////////////////////////////////////////////////////////////////////////


void ComboboxBkThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    const CRect* prc = &pDrawStruct->rc;

	switch(pDrawStruct->nState)
	{
	case COMBOBOX_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;

	case COMBOBOX_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(pRenderTarget, prc);
		break;

	case COMBOBOX_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;;

	case COMBOBOX_BKGND_RENDER_STATE_READONLY_NORMAL:
		this->DrawReadonlyNormal(pRenderTarget, prc);
		break;

	case COMBOBOX_BKGND_RENDER_STATE_READONLY_HOVER:
		this->DrawReadonlyHover(pRenderTarget, prc);
		break;

	case COMBOBOX_BKGND_RENDER_STATE_READONLY_PRESS:
		this->DrawReadonlyPress(pRenderTarget, prc);
		break;

	case COMBOBOX_BKGND_RENDER_STATE_READONLY_DISABLE:
		this->DrawReadonlyDisable(pRenderTarget, prc);
		break;

	default:
		this->DrawNormal(pRenderTarget, prc);
		break;
	}
}

void ComboboxBkThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if (GetTHEME())
	{
		HRESULT hr = DrawThemeBackground(GetTHEME(), hDC, CP_BORDER, CBXS_DISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawDisable  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void ComboboxBkThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if (GetTHEME())
	{
		HRESULT hr = DrawThemeBackground(GetTHEME(), hDC, CP_BORDER, CBXS_NORMAL, (RECT*)prc, 0);
		if (S_OK != hr)
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawNormal  DrawThemeBackground failed."));
	}
	else
	{
		if (GetObject()->IsReadonly())
		{
			DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
		}
		else
		{
			::FillRect(hDC, (RECT*)prc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT);
		}
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void ComboboxBkThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if (GetTHEME())
	{
		HRESULT hr = DrawThemeBackground(GetTHEME(), hDC, CP_BORDER, CBXS_HOT, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawHover  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( GetObject()->IsReadonly() )
		{
			DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
		}
		else
		{
			::FillRect(hDC, (RECT*)prc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT);
		}
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void ComboboxBkThemeRender::DrawPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if (GetTHEME())
	{
		HRESULT hr = DrawThemeBackground(GetTHEME(), hDC, CP_BORDER, CBXS_PRESSED, (RECT*)prc, 0);
		if (S_OK != hr)
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawPress  DrawThemeBackground failed."));
		}
	}
	else
	{
		if (GetObject()->IsReadonly())
		{
			DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
		}
		else
		{
			::FillRect(hDC, (RECT*)prc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT);
		}
	}
	pRenderTarget->ReleaseHDC(hDC);
}


void ComboboxBkThemeRender::DrawReadonlyDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if (GetTHEME())
	{
		HRESULT hr = DrawThemeBackground(GetTHEME(), hDC, CP_READONLY, CBXS_DISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawDisable  DrawThemeBackground failed."));
		}
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(GetTHEME(), hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_DISABLED, (RECT*)&rc, 0);
			if ( S_OK != hr )
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);
			}
		}
	}
	else
	{
		DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);

		CRect rc(prc);
		rc.DeflateRect(0,2,2,2);
		rc.left = rc.right - 16;
		::DrawFrameControl(hDC, (RECT*)&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX|DFCS_INACTIVE);
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void ComboboxBkThemeRender::DrawReadonlyNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if (GetTHEME())
	{
		HRESULT hr = DrawThemeBackground(GetTHEME(), hDC, CP_READONLY, CBXS_NORMAL, (RECT*)prc, 0);
		if (S_OK != hr)
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);

		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(GetTHEME(), hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_NORMAL, (RECT*)&rc, 0);
			if ( S_OK != hr )
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);
			}
		}
	}
	else
	{
		::FillRect(hDC, (RECT*)prc, (HBRUSH)GetStockObject(WHITE_BRUSH));
		DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT);

		CRect rc(prc);
		rc.DeflateRect(0,2,2,2);
		rc.left = rc.right - 16;
		::DrawFrameControl(hDC, (RECT*)&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX);
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void ComboboxBkThemeRender::DrawReadonlyHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if (GetTHEME())
	{
		HRESULT hr = DrawThemeBackground(GetTHEME(), hDC, CP_READONLY, CBXS_HOT, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}

		if (GetObject()->GetUIApplication()->IsUnderXpOS())
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(GetTHEME(), hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_HOT, (RECT*)&rc, 0);
			if ( S_OK != hr )
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);
			}
		}
		else
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(GetTHEME(), hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_NORMAL, (RECT*)&rc, 0);
			if ( S_OK != hr )
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);
			}
		}
	}
	else
	{
		::FillRect(hDC, (RECT*)prc, (HBRUSH)GetStockObject(WHITE_BRUSH));
		DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT);

		CRect rc(prc);
		rc.DeflateRect(0,2,2,2);
		rc.left = rc.right - 16;
		::DrawFrameControl(hDC, (RECT*)&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX|DFCS_HOT);
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void ComboboxBkThemeRender::DrawReadonlyPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if (GetTHEME())
	{
		HRESULT hr = DrawThemeBackground(GetTHEME(), hDC, CP_READONLY, CBXS_PRESSED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawPress  DrawThemeBackground failed."));
		}

		if (GetObject()->GetUIApplication()->IsUnderXpOS())
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(GetTHEME(), hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_PRESSED, (RECT*)&rc, 0);
			if ( S_OK != hr )
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);
			}
		}
		else
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(GetTHEME(), hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_NORMAL, (RECT*)&rc, 0);
			if ( S_OK != hr )
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);
			}
		}
	}
	else
	{
		::FillRect(hDC, (RECT*)prc, (HBRUSH)GetStockObject(WHITE_BRUSH));
		DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT);

		CRect rc(prc);
		rc.DeflateRect(0,2,2,2);
		rc.left = rc.right - 16;
		::DrawFrameControl(hDC, (RECT*)&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX|DFCS_PUSHED|DFCS_FLAT);
	}
	pRenderTarget->ReleaseHDC(hDC);
}


MenuBkThemeRender::MenuBkThemeRender()
{
    m_pIMenu = NULL; 
}

void  MenuBkThemeRender::OnInitialize()
{
    SetMsgHandled(FALSE);
    if (GetObject())
    {
        m_pIMenu = (IMenu*)GetObject()->QueryInterface(uiiidof(IMenu));
    }
}
void MenuBkThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    const CRect* prc = &pDrawStruct->rc;

    HDC  hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, MENU_POPUPBACKGROUND, 1, (RECT*)prc, 0);
        if (SUCCEEDED(hr))
        {
            hr = DrawThemeBackground(hTheme, hDC, MENU_POPUPBORDERS, 1, (RECT*)prc, 0);
            if (S_OK != hr)
            {
                UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
            }
        }

        if (m_pIMenu)
        {
            CRect rcIconGutter(*prc);
            CRegion4 rcBorder;
            m_pIMenu->GetBorderRegion(&rcBorder);

            Util::DeflatRect(&rcIconGutter, &rcBorder);
            rcIconGutter.right = rcIconGutter.left + m_pIMenu->GetIconGutterWidth();

            if (FAILED(DrawThemeBackground(hTheme, hDC, MENU_POPUPGUTTER, 1, (RECT*)&rcIconGutter, 0)))
                UI_LOG_WARN(_T("%s DrawThemeBackground MENU_POPUPGUTTER failed."), FUNC_NAME);
        }
    }
    else
    {
        COLORREF col = GetSysColor(COLOR_BTNFACE);
        HBRUSH hBrush = CreateSolidBrush(col);
        ::FillRect(hDC, (RECT*)prc, hBrush);
        SAFE_DELETE_GDIOBJECT(hBrush);

        DrawEdge(hDC, (RECT*)prc, EDGE_RAISED, BF_RECT);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

//////////////////////////////////////////////////////////////////////////

void MenuSeperatorThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget*  pRenderTarget = pDrawStruct->pRenderTarget;
    const CRect*  prc = &pDrawStruct->rc;

    HDC  hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();

    if (hTheme)
    {
        CRect rc(prc);
        int yCenter = rc.top + rc.Height()/2;
        rc.top = yCenter-3;
        rc.bottom = yCenter+3;

        // 如果小于6个高度, MENU_POPUPSEPARATOR将不显示，大于6个高度，又将被拉伸
        if (FAILED(DrawThemeBackground(hTheme, hDC, MENU_POPUPSEPARATOR, 1, (RECT*)&rc/*prc*/, 0)))
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
    }
    else
    {
        COLORREF col1 = RGB(128,128,128);
        COLORREF col2 = RGB(255,255,255);
        HPEN hPen = CreatePen(PS_SOLID, 1, col1);
        HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);

        int y = (prc->top+prc->bottom)/2;
        MoveToEx(hDC, prc->left, y, NULL);
        LineTo(hDC, prc->right, y);

        ::SelectObject(hDC, hOldPen);
        SAFE_DELETE_GDIOBJECT(hPen);
        hPen = CreatePen(PS_SOLID,1,col2);
        ::SelectObject(hDC, hPen);

        y++;
        MoveToEx(hDC, prc->left, y, NULL);
        LineTo(hDC, prc->right, y);

        SelectObject(hDC, hOldPen);
        SAFE_DELETE_GDIOBJECT(hPen);
    }
    pRenderTarget->ReleaseHDC(hDC);
}


//////////////////////////////////////////////////////////////////////////

void  MenuStringItemRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    switch(pDrawStruct->nState)
    {
    case MENU_STRING_ITEM_RENDER_STATE_DISABLE:
        this->DrawDisable(pDrawStruct->pRenderTarget, &pDrawStruct->rc);
        break;

    case MENU_STRING_ITEM_RENDER_STATE_PRESS:
        this->DrawPress(pDrawStruct->pRenderTarget, &pDrawStruct->rc);
        break;

    case MENU_STRING_ITEM_RENDER_STATE_HOVER:
        this->DrawHover(pDrawStruct->pRenderTarget, &pDrawStruct->rc);
        break;;

    default:
        this->DrawNormal(pDrawStruct->pRenderTarget, &pDrawStruct->rc);
        break;
    }
}

void MenuStringItemRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
    HDC  hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        if (FAILED(DrawThemeBackground(hTheme, hDC, MENU_POPUPITEM, MPI_DISABLED, (RECT*)prc, 0)))
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    pRenderTarget->ReleaseHDC(hDC);
}
void MenuStringItemRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{}
void MenuStringItemRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
    HDC  hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, MENU_POPUPITEM, MPI_HOT, (RECT*)prc, 0);
        if (S_OK != hr)
        {
            UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {
        COLORREF col = RGB(10,36,106);
        HBRUSH hBrush = CreateSolidBrush(col);
        ::FillRect(hDC, prc, hBrush);
        SAFE_DELETE_GDIOBJECT(hBrush);
    }
    pRenderTarget->ReleaseHDC(hDC);
}
void MenuStringItemRender::DrawPress( IRenderTarget* pRenderTarget, const CRect* prc )
{}



void MenuPopupTriangleRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget*  pRenderTarget = pDrawStruct->pRenderTarget;
    const CRect*  prc = &pDrawStruct->rc;

    switch (pDrawStruct->nState)
    {
    default:
        this->DrawNormal(pRenderTarget, prc);
        break;
    case MENU_POPUPTRIANGLE_RENDER_STATE_HOVER:
        this->DrawHover(pRenderTarget, prc);
        break;
    case MENU_POPUPTRIANGLE_RENDER_STATE_DISABLE:
        this->DrawDisable(pRenderTarget, prc);
        break;
    }
}

void MenuPopupTriangleRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
    HDC  hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, MENU_POPUPSUBMENU, MSM_DISABLED, (RECT*)prc, 0);
        if ( S_OK != hr )
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {
        this->DrawTriangle(hDC, prc, false);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

void MenuPopupTriangleRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
    HDC hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, MENU_POPUPSUBMENU, MSM_NORMAL, (RECT*)prc, 0);
        if ( S_OK != hr )
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {
        this->DrawTriangle(hDC, prc, false);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

void MenuPopupTriangleRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
    HDC hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, MENU_POPUPSUBMENU, MSM_NORMAL, (RECT*)prc, 0);
        if ( S_OK != hr )
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {
        this->DrawTriangle(hDC, prc, true);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

// 绘制箭头
void MenuPopupTriangleRender::DrawTriangle( HDC hDC, const CRect* prc, int nState )
{
    POINT pt[4] = 
    {
        {-3,4}, {1,0}, {-3,-4}, {-3,4}
    };

    int x = 0; int y = 0;
    x = (prc->left + prc->right)/2;
    y = (prc->top + prc->bottom)/2;

    for (int i = 0; i < 4; i++)
    {
        pt[i].x += x;
        pt[i].y += y;
    }

    HBRUSH hBrush = NULL;
    HPEN   hPen = NULL;
    switch(nState)
    {
    case MENU_POPUPTRIANGLE_RENDER_STATE_HOVER:
        hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
        hPen = CreatePen(PS_SOLID, 1, RGB(255,255,255));
        break;

    case MENU_POPUPTRIANGLE_RENDER_STATE_DISABLE:
        hBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
        hPen = CreatePen(PS_SOLID, 1, RGB(192,192,192));
        break;

    default:
        hBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
        hPen = CreatePen(PS_SOLID, 1, RGB(0,0,0));
        break;
    }

    HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);
    HPEN   hOldPen   = (HPEN)::SelectObject(hDC, hPen);
    ::Polygon(hDC, pt, 4);
    ::SelectObject(hDC,hOldBrush);
    ::SelectObject(hDC, hOldPen);
    SAFE_DELETE_GDIOBJECT(hBrush);
    SAFE_DELETE_GDIOBJECT(hPen);
}

//////////////////////////////////////////////////////////////////////////

void MenuCheckedIconThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    const CRect* prc = &pDrawStruct->rc;

    HDC  hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        int nStateId = MC_CHECKMARKNORMAL;
        if (pDrawStruct->nState & RENDER_STATE_DISABLE)
            nStateId = MC_CHECKMARKDISABLED;

        SIZE s = {22,22};
        RECT rcIcon;
        rcIcon.left = prc->left;
        rcIcon.top = prc->top + (prc->Height() - s.cy)/2;
        rcIcon.right = rcIcon.left + s.cx;
        rcIcon.bottom = rcIcon.top + s.cy;

        HRESULT hr = DrawThemeBackground(hTheme, hDC, MENU_POPUPCHECK, nStateId, (RECT*)&rcIcon, 0);
        if (S_OK != hr)
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {
        // 注：直接使用DrawFrameControl得到的图片是白底黑字（√）的，因此需要进行二次处理
        //     将白色背景去掉。见MSDN
        SIZE s = {15, 15};
        HDC hMemDC = GetObject()->GetUIApplication()->GetCacheDC();
        HBITMAP hBitmap = ::CreateCompatibleBitmap(hMemDC, s.cx, s.cy);
        HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);

        CRect rcDraw;
        rcDraw.left = prc->left + (prc->Width()-s.cx)/2;
        rcDraw.top  = prc->top  + (prc->Height()-s.cy)/2;
        rcDraw.right = rcDraw.left + s.cx;
        rcDraw.bottom = rcDraw.top + s.cy;

        CRect rcMem(0,0, s.cx, s.cy);
        ::DrawFrameControl(hMemDC, (LPRECT)&rcMem, DFC_MENU, DFCS_MENUCHECK);

        int nROP = SRCAND;
        if (pDrawStruct->nState & RENDER_STATE_HOVER)
            nROP = MERGEPAINT;

        ::BitBlt(hDC, rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(), hMemDC, 0,0, nROP);

        SelectObject(hMemDC, hOldBmp);
        SAFE_DELETE_GDIOBJECT(hBitmap);
        GetObject()->GetUIApplication()->ReleaseCacheDC(hMemDC);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

//////////////////////////////////////////////////////////////////////////

void MenuRadioIconThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    const CRect* prc = &pDrawStruct->rc;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        int nStateId = MC_BULLETNORMAL;
        if (pDrawStruct->nState & RENDER_STATE_DISABLE)
            nStateId = MC_BULLETDISABLED;

        SIZE s = {22,22};
        RECT rcIcon;
        rcIcon.left = prc->left;
        rcIcon.top = prc->top + (prc->Height() - s.cy)/2;
        rcIcon.right = rcIcon.left + s.cx;
        rcIcon.bottom = rcIcon.top + s.cy;

        HRESULT hr = DrawThemeBackground(hTheme, hDC, MENU_POPUPCHECK, nStateId, (RECT*)&rcIcon, 0);
        if (S_OK != hr)
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {
        SIZE s = {15, 15};
        HDC hMemDC = GetObject()->GetUIApplication()->GetCacheDC();
        HBITMAP hBitmap = ::CreateCompatibleBitmap(hMemDC, s.cx, s.cy);
        HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);

        CRect rcDraw;
        rcDraw.left = prc->left + (prc->Width()-s.cx)/2;
        rcDraw.top  = prc->top  + (prc->Height()-s.cy)/2;
        rcDraw.right = rcDraw.left + s.cx;
        rcDraw.bottom = rcDraw.top + s.cy;

        CRect rcMem(0,0, s.cx, s.cy);
        ::DrawFrameControl(hMemDC, (LPRECT)&rcMem, DFC_MENU, DFCS_MENUBULLET);

        int nROP = SRCAND;
        if (pDrawStruct->nState & RENDER_STATE_HOVER)
            nROP = MERGEPAINT;

        ::BitBlt(hDC, rcMem.left, rcMem.top, rcMem.Width(), rcMem.Height(), hMemDC, 0,0, nROP);

        SelectObject(hMemDC, hOldBmp);
        SAFE_DELETE_GDIOBJECT(hBitmap);
        GetObject()->GetUIApplication()->ReleaseCacheDC(hMemDC);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

//////////////////////////////////////////////////////////////////////////

void MenuRadioCheckIconBkThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget*  pRenderTarget = pDrawStruct->pRenderTarget;
    const CRect*  prc = &pDrawStruct->rc;

    HDC hDC = pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        SIZE s = {22,22};
       // GetThemePartSize(hTheme, hDC, MENU_POPUPCHECKBACKGROUND, MCB_NORMAL, NULL, TS_DRAW, &s);

        RECT rcIcon;
        //rcIcon.left = prc->left + (prc->Width() - s.cx)/2;;
        rcIcon.left = prc->left;
        rcIcon.top = prc->top + (prc->Height() - s.cy)/2;
        rcIcon.right = rcIcon.left + s.cx;
        rcIcon.bottom = rcIcon.top + s.cy;

        int nStateId = MCB_NORMAL;
        if (pDrawStruct->nState & RENDER_STATE_DISABLE)
            nStateId = MCB_DISABLED;

        HRESULT hr = DrawThemeBackground(hTheme, hDC, MENU_POPUPCHECKBACKGROUND, nStateId, &rcIcon, 0);
        if (S_OK != hr)
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    pRenderTarget->ReleaseHDC(hDC);
}

MenuTextThemeRender::MenuTextThemeRender()
{
    m_pRenderFont = NULL;
}
MenuTextThemeRender::~MenuTextThemeRender()
{
    SAFE_RELEASE(m_pRenderFont);
}

void MenuTextThemeRender::SetRenderFont(IRenderFont* pRenderFont)
{ 
    SAFE_RELEASE(m_pRenderFont);
    m_pRenderFont = pRenderFont;
    if (pRenderFont)
        pRenderFont->AddRef();
}

// 如果字体指针为空，则取object对象的配置字体。
void  MenuTextThemeRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    IThemeTextRenderBase::xProcessMessage(GetCurMsg(), 0, 0);

    IUIApplication* pUIApp = GetObject()->GetUIApplication();
    if (NULL == pUIApp)
        return;

    IFontRes* pFontRes = pUIApp->GetActiveSkinFontRes();

    String strAttr;
    if (szPrefix)
        strAttr = szPrefix;
    strAttr.append(XML_TEXTRENDER_FONT);

    const TCHAR* szText = pMapAttrib->GetAttr(strAttr.c_str(), pData->bErase);
    if (szText)
    {
        pFontRes->GetFont((BSTR)szText, ::GetRenderLibraryType(GetObject()), &m_pRenderFont);
    }
    if (NULL == m_pRenderFont)
    {
        IRenderFont* pFont = GetObject()->GetRenderFont();
        if (pFont)
        {
            m_pRenderFont = GetObject()->GetRenderFont();
            m_pRenderFont->AddRef();
        }
        else
        {
            // 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
            pFontRes->GetDefaultFont(GetRenderLibraryType(GetObject()), &m_pRenderFont);
        }
    }
}
void  MenuTextThemeRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (NULL == m_pRenderFont || _tcslen(pDrawStruct->szText)==0)
        return;

    int&  nState = pDrawStruct->ds_renderbase.nState;
    COLORREF  col = 0;

    if (NULL == GetTHEME())
    {
        if (nState & RENDER_STATE_DISABLE)
            col = RGB(109, 109, 109);
        if (pDrawStruct->ds_renderbase.nState & RENDER_STATE_HOVER)
            col = RGB(255,255,255);
    }
    else
    {
        if (nState & RENDER_STATE_DISABLE)
            col = RGB(109, 109, 109);
        else
            col = RGB(0,0,0);
    }

    DRAWTEXTPARAM  param;
    param.nFormatFlag = pDrawStruct->nDrawTextFlag;
    if (-1 == param.nFormatFlag)
        param.nFormatFlag = DT_LEFT|DT_SINGLELINE|DT_VCENTER;
    param.szText = pDrawStruct->szText;
    param.prc = &pDrawStruct->ds_renderbase.rc;
    param.color.ReplaceRGB(col);
    pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_pRenderFont, &param);
}


TabCtrlButtonBkThemeRender::TabCtrlButtonBkThemeRender()
{
    m_nDir = 0;
}
void  TabCtrlButtonBkThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    HDC  hDC = pDrawStruct->pRenderTarget->GetHDC();
    HTHEME  hTheme = GetTHEME();
    if (hTheme)
    {
        HRESULT hr = DrawThemeBackground(hTheme, hDC, 
            //m_nDir?TABP_TABITEMBOTHEDGE:TABP_TABITEM, 
            TABP_TOPTABITEMBOTHEDGE,
            GetStateId(pDrawStruct->nState), &pDrawStruct->rc, 0);
        if (S_OK != hr)
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
    }
    else
    {

    }
    pDrawStruct->pRenderTarget->ReleaseHDC(hDC);
}
int  TabCtrlButtonBkThemeRender::GetStateId(int nState)
{
    if (m_nDir)
    {
        switch (nState)
        {
        case TABCTRLBUTTON_BKGND_RENDER_STATE_NORMAL:
            return TIBES_NORMAL;

        case TABCTRLBUTTON_BKGND_RENDER_STATE_HOVER:
            return TIBES_HOT;

        case TABCTRLBUTTON_BKGND_RENDER_STATE_SELECTED:
            return TIBES_SELECTED;

        case TABCTRLBUTTON_BKGND_RENDER_STATE_DISABLE:
            return TIBES_DISABLED;
        }
    }
    else
    {
        switch (nState)
        {
        case TABCTRLBUTTON_BKGND_RENDER_STATE_NORMAL:
            return TIS_NORMAL;

        case TABCTRLBUTTON_BKGND_RENDER_STATE_HOVER:
            return TIS_HOT;

        case TABCTRLBUTTON_BKGND_RENDER_STATE_SELECTED:
            return TIS_SELECTED;

        case TABCTRLBUTTON_BKGND_RENDER_STATE_DISABLE:
            return TIS_DISABLED;
        }
    }
    return TIS_NORMAL;
}
void*  TabCtrlButtonBkThemeRender::QueryInterface(const IID* pIID)
{
    if (::IsEqualIID(*pIID, IID_UI_TabCtrlButtonBkThemeRender))
    {
        return (void*)this;
    }
    SetMsgHandled(FALSE);
    return NULL;
}
void  TabCtrlButtonBkThemeRender::SetDirection(long n)
{
    m_nDir = n;
}

ListViewThemeTextRender::ListViewThemeTextRender()
{
    m_pRenderFont = NULL;
    m_bUseExplorerTheme = false;
}

ListViewThemeTextRender::~ListViewThemeTextRender()
{
    SAFE_RELEASE(m_pRenderFont);
}

void ListViewThemeTextRender::SetRenderFont(IRenderFont* pRenderFont)
{ 
    SAFE_RELEASE(m_pRenderFont);
    m_pRenderFont = pRenderFont;
    if (pRenderFont)
        pRenderFont->AddRef();
}

void  ListViewThemeTextRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    IThemeTextRenderBase::xProcessMessage(GetCurMsg(), 0, 0);

    IUIApplication* pUIApp = GetObject()->GetUIApplication();
    if (NULL == pUIApp)
        return;

    IFontRes* pFontRes = pUIApp->GetActiveSkinFontRes();

    String strAttr;
    if (szPrefix)
        strAttr = szPrefix;
    strAttr.append(XML_TEXTRENDER_FONT);

    const TCHAR* szText = pMapAttrib->GetAttr(strAttr.c_str(), pData->bErase);
    if (szText)
    {
        pFontRes->GetFont((BSTR)szText, ::GetRenderLibraryType(GetObject()), &m_pRenderFont);
    }
    if (NULL == m_pRenderFont)
    {
        IRenderFont* pFont = GetObject()->GetRenderFont();
        if (pFont)
        {
            m_pRenderFont = GetObject()->GetRenderFont();
            m_pRenderFont->AddRef();
        }
        else
        {
            // 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
            pFontRes->GetDefaultFont(GetRenderLibraryType(GetObject()), &m_pRenderFont);
        }
    }
}

void ListViewThemeTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (NULL == m_pRenderFont ||  _tcslen(pDrawStruct->szText)==0)
        return;

    COLORREF col = 0;
    int&  nState = pDrawStruct->ds_renderbase.nState;

    if (NULL == GetTHEME())
    {
        if(nState & RENDER_STATE_SELECTED)
        {
            col = GetSysColor(COLOR_HIGHLIGHTTEXT);
        }
    }
    else
    {
        int nPartid = -1;
        int nStateid = -1;
        switch (nState)
        {
        case LISTCTRLITEM_FOREGND_RENDER_STATE_NORMAL:
            nPartid = LVP_LISTITEM;
            nStateid = LISS_NORMAL;
            break;

        case LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER:
        case LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS:
            nPartid = LVP_LISTITEM;
            nStateid = LISS_HOT;
            break;

        case LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL:
        case LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER:
        case LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS:
            nPartid = LVP_LISTITEM;
            nStateid = LISS_SELECTED;
            break;

        case LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE:
        case LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE:
            nPartid = LVP_LISTITEM;
            nStateid = LISS_DISABLED;
            break;

        case LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NOT_FOCUS:
            nPartid = LVP_LISTITEM;
            nStateid = LISS_SELECTEDNOTFOCUS;
            break;
        }

        GetThemeColor(GetTHEME(), nPartid, nStateid, 0/*TMT_BODYTEXTCOLOR*/, &col);
    }

    DRAWTEXTPARAM  param;
    param.nFormatFlag = pDrawStruct->nDrawTextFlag;
    if (-1 == param.nFormatFlag)
        param.nFormatFlag = DT_LEFT|DT_SINGLELINE|DT_VCENTER;
    param.szText = pDrawStruct->szText;
    param.prc = &pDrawStruct->ds_renderbase.rc;
    param.color.ReplaceRGB(col);
    pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_pRenderFont, &param);
}


// 需要SetWindowTheme(m_hWnd, L"explorer", NULL);的支持，见WindowBase::CreateUI
void ListViewCtrlForeThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    const CRect* prc = &pDrawStruct->rc;
    int nState = pDrawStruct->nState;

    if (NULL == GetTHEME())
    {
        if (nState & RENDER_STATE_SELECTED)
        {
            HDC hDC = pRenderTarget->GetHDC();
            HBRUSH hSysBrush = GetSysColorBrush(COLOR_HIGHLIGHT);
            ::FillRect(hDC, prc, hSysBrush);
            hSysBrush = NULL;
            pRenderTarget->ReleaseHDC(hDC);
        }
        return;
    }

    int nPartid = -1;
    int nStateid = -1;
    switch (nState)
    {
    case LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER:
    case LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS:
        nPartid = LVP_LISTITEM;
        nStateid = LISS_HOT;
        break;

    case LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL:
    case LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER:
    case LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS:
        nPartid = LVP_LISTITEM;
        nStateid = LISS_SELECTED;
        break;

    case LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NOT_FOCUS:
        nPartid = LVP_LISTITEM;
        nStateid = LISS_SELECTEDNOTFOCUS;
        break;
    }

    if (nPartid != -1 && nStateid != -1)
    {
        HDC hDC = pRenderTarget->GetHDC();
        HRESULT hr = DrawThemeBackground(GetTHEME(), hDC, nPartid, nStateid, (RECT*)prc, 0);
        if ( S_OK != hr )
        {
            UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
        }
        pRenderTarget->ReleaseHDC(hDC);
    }
}

TreeViewCtrlExpandCollapseIconThemeRender::TreeViewCtrlExpandCollapseIconThemeRender()
{
    m_bUseExplorerTheme = false;
}

void TreeViewCtrlExpandCollapseIconThemeRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (NULL == GetTHEME())
        return DrawNoTheme(pDrawStruct->pRenderTarget, &pDrawStruct->rc, pDrawStruct->nState);
    else
        return DrawTheme(pDrawStruct->pRenderTarget, &pDrawStruct->rc, pDrawStruct->nState);
}
void TreeViewCtrlExpandCollapseIconThemeRender::DrawTheme(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
    HDC hDC = pRenderTarget->GetHDC();

    TREEVIEWPARTS ePart = TVP_GLYPH;
    GLYPHSTATES   eState = GLPS_OPENED;
    if (nState & RENDER_STATE_SELECTED)
        eState = GLPS_CLOSED;

    if (nState & RENDER_STATE_HOVER || nState & RENDER_STATE_PRESS)
    {
        TCHAR*  szThemeName = NULL;
        OnGetThemeName(&szThemeName);
        if (0 != _tcscmp(VSCLASS_TREEVIEW, szThemeName))
            ePart = TVP_HOTGLYPH;
    }


    HTHEME hTheme = GetTHEME();
    SIZE s = {0,0};
    GetThemePartSize(hTheme, hDC, ePart, eState, NULL, TS_DRAW, &s);
    CRect rcDraw(prc);
    rcDraw.left = rcDraw.left + (int)((rcDraw.Width()-s.cx)/2.0+0.5);  // +0.5 使得树控件虚线位于图标中间
    rcDraw.top  = rcDraw.top  + (int)((rcDraw.Height()-s.cy)/2.0+0.5);
    rcDraw.right = rcDraw.left + s.cx;
    rcDraw.bottom = rcDraw.top + s.cy;

    HRESULT hr = DrawThemeBackground(hTheme, hDC, ePart, eState, (RECT*)&rcDraw, 0);
    if (S_OK != hr)
    {
        UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
    }
    pRenderTarget->ReleaseHDC(hDC);
}

void TreeViewCtrlExpandCollapseIconThemeRender::DrawNoTheme(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
    HDC hDC = pRenderTarget->GetHDC();

    CRect rcDraw;
    int nWidth = 9, nHeight = 9;
    rcDraw.left = prc->left + (prc->Width()-nWidth)/2;
    rcDraw.top  = prc->top + (prc->Height()-nHeight)/2;
    rcDraw.right = rcDraw.left + nWidth;
    rcDraw.bottom = rcDraw.top + nHeight;

    HPEN hPen = CreatePen(PS_SOLID, 1, RGB(128,128,128));
    HBRUSH hBrush = (HBRUSH)GetStockObject(WHITE_BRUSH);
    HPEN hOldPen = (HPEN)::SelectObject(hDC, hPen);
    HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBrush);

    // 边框
    Rectangle(hDC, rcDraw.left, rcDraw.top, rcDraw.right, rcDraw.bottom);

    // 内部 + -

    HPEN hPen2 = CreatePen(PS_SOLID, 1, RGB(0,0,0));
    SelectObject(hDC, hPen2);

    POINT pt1 = { rcDraw.left+2, rcDraw.top+4};
    POINT pt2 = { rcDraw.right-2,pt1.y};

    ::MoveToEx(hDC, pt1.x, pt1.y, NULL);
    ::LineTo(hDC, pt2.x, pt2.y);

    if (nState & RENDER_STATE_SELECTED)  // +
    {
        POINT pt3 = { rcDraw.left+4, rcDraw.top+2};
        POINT pt4 = { pt3.x,  rcDraw.bottom-2};

        ::MoveToEx(hDC, pt3.x, pt3.y, NULL);
        ::LineTo(hDC, pt4.x, pt4.y);
    }

    ::SelectObject(hDC, hOldPen);
    ::SelectObject(hDC, hOldBrush);
    ::DeleteObject(hPen);
    ::DeleteObject(hPen2);
    ::DeleteObject(hBrush);

    pRenderTarget->ReleaseHDC(hDC);
}

void TreeViewCtrlExpandCollapseIconThemeRender::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = 20;
    pSize->cy = 20;
}

// http://fc01.deviantart.net/fs26/f/2008/154/0/6/Vista_Visual_Style_Classes_by_UkIntel.html
void  TreeViewCtrlExpandCollapseIconThemeRender::OnGetThemeName(TCHAR** ppszName) 
{ 
    IUIApplication* pUIApp = NULL;
    if (GetObject())
        pUIApp = GetObject()->GetUIApplication();

    if (m_bUseExplorerTheme)
    {
        if (pUIApp && pUIApp->IsUnderXpOS())
        {
            *ppszName = VSCLASS_TREEVIEW;
        }
        else
        {
            *ppszName = _T("Explorer::TreeView"); 
        }
    }
    else
    {
        *ppszName = VSCLASS_TREEVIEW;
    }
}

#if 0
//////////////////////////////////////////////////////////////////////////

SIZE ScrollBarSizeBoxThemeRender::GetDesiredSize()
{
	SIZE s = {GetSystemMetrics(SM_CXVSCROLL),GetSystemMetrics(SM_CYVSCROLL)}; 
	return s;
}

void ScrollBarSizeBoxThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	HDC hDC = pRenderTarget->GetHDC();		
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, SBP_SIZEBOX, SCRBS_NORMAL, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		::DrawFrameControl(hDC, (RECT*)prc, DFC_SCROLL, DFCS_SCROLLSIZEGRIP);
	}

	pRenderTarget->ReleaseHDC(hDC);	
}
//////////////////////////////////////////////////////////////////////////

void TooltipBkgndThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	if (NULL == pRenderTarget)
		return ;

	if (m_hTheme)
	{
		// 		COLORREF colBkgnd = ::GetSysColor(COLOR_INFOBK);
		// 		COLORREF colBorder = RGB(0,0,0);
		// 		Rectangle(pRenderTarget, prc, colBorder, colBkgnd );

		// 		CRect r(*prc);
		// 		r.top += 3;
		// 		r.left += 3;
		HDC hDC = pRenderTarget->GetHDC();
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, TTP_STANDARD, TTSS_NORMAL, (RECT*)prc, 0);
		if (S_OK != hr)
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
		pRenderTarget->ReleaseHDC(hDC);
	}
	else
	{
		COLORREF colBkgnd = ::GetSysColor(COLOR_INFOBK);
		COLORREF colBorder = RGB(0,0,0);
		pRenderTarget->Rectangle(prc, colBorder, colBkgnd, 1, false);
	}

}
SIZE TooltipBkgndThemeRender::GetDesiredSize()
{
	SIZE s = {0,0};
	return s;
}

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

// void ListViewCtrlBkgndThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
// {
// 	if (NULL == pRenderTarget)
// 		return ;
// 	
// 	HEADERITEMRIGHTSTATES eState = HIRS_NORMAL;
// 	if (nState & RENDER_STATE_PRESS)
// 	{
// 		eState = HIRS_PRESSED;
// 	}
// 	else if (nState & RENDER_STATE_HOVER)
// 	{
// 		eState = HIRS_HOT;
// 	}
// 
// 	HDC hDC = pRenderTarget->GetHDC();
// 	if (m_hTheme)
// 	{
// 		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, HP_HEADERITEMRIGHT, eState, (RECT*)prc, 0);
// 		if (S_OK != hr)
// 		{
// 			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
// 		}
// 	}
// 	else
// 	{
// 
// 	}
// 	pRenderTarget->ReleaseHDC(hDC);
// }


//////////////////////////////////////////////////////////////////////////

#endif

}