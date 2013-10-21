#include "stdafx.h"
#include "button.h"
#include <atltypes.h>

#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"

namespace UI
{

ButtonBase::ButtonBase()
{
    m_pIButtonBase = NULL;
	m_nCheckState = BST_UNCHECKED;
	m_bSpaceKeyDown = false;
}

HRESULT ButtonBase::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(IButtonBase, IControl);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pIButtonBase->SetStyleEx(MAKECONTROLSUBTYPE(m_pIButtonBase->GetStyleEx(), BUTTON_STYLE_PUSHBUTTON));
    return S_OK;
}

void  ButtonBase::ResetAttribute()
{
    DO_PARENT_PROCESS(IButtonBase, IControl);
}
void  ButtonBase::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
    DO_PARENT_PROCESS(IButtonBase, IControl);
}
void  ButtonBase::OnEditorGetAttrList(IUIEditor* pEditor, IUIEditorGroupAttribute*  pRootAttr)
{
	DO_PARENT_PROCESS(IButtonBase, IControl);
}

//	按钮的类型
int  ButtonBase::GetButtonStyle()
{
	return GETCONTROLSUBTYPE(m_pIButtonBase->GetStyleEx());
}
void ButtonBase::SetButtonStyle(int n)
{
    m_pIButtonBase->SetStyleEx(MAKECONTROLSUBTYPE(m_pIButtonBase->GetStyleEx(), n));
}

void ButtonBase::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    IRenderBase* pBkgndRender = m_pIButtonBase->GetBkRender();
	if (pBkgndRender)
	{
		bool  bDisable = !m_pIButtonBase->IsEnable();
		bool  bHover   = m_pIButtonBase->IsHover();
		bool  bPress   = m_pIButtonBase->IsPress();
		bool  bForePress = m_pIButtonBase->IsForcePress();
		bool  bChecked = IsChecked();
        bool  bDefault = m_pIButtonBase->IsDefault();

		CRect rc(0,0, m_pIButtonBase->GetWidth(), m_pIButtonBase->GetHeight());

		if (bDisable)
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_DISABLE:BUTTON_BKGND_RENDER_STATE_DISABLE);
		}
		else if (bForePress || (bPress && bHover))
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_PRESS:BUTTON_BKGND_RENDER_STATE_PRESS);
		}
		else if (bHover || bPress)
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_HOVER:BUTTON_BKGND_RENDER_STATE_HOVER);
		}
        else if (bDefault)
        {
            pBkgndRender->DrawState(pRenderTarget, &rc, BUTTON_BKGND_RENDER_STATE_DEFAULT);
        }
		else 
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_NORMAL:BUTTON_BKGND_RENDER_STATE_NORMAL);
		}
	}

	if (m_pIButtonBase->IsFocus())  // 将focus放在这里是为了避免配置了padding时,在onpaint中绘制focus rect也会受到影响
	{
        LRESULT lRet = ::SendMessage(m_pIButtonBase->GetHWND(), WM_QUERYUISTATE, 0, 0);
        if (!(lRet & UISF_HIDEFOCUS))
        {
    		this->DrawFocus(pRenderTarget);
        }
	}	
}


void ButtonBase::OnStateChanged2(UINT nMask)
{
	m_pIButtonBase->UpdateObject();
}

// void ButtonBase::OnMouseMove(UINT nFlags, POINT point)
// {
// 	// 判断这个时候的特殊情况：对象处于按下状态，但鼠标在对象外面，这个时候要显示hover状态
// 	bool bOut = false;
// 	bOut = ( HTNOWHERE == this->OnHitTest( &point ) );
// 	if( bOut )
// 	{
// 		if( IsHover() )
// 		{
// 			this->SetHover( false );
// 			this->UpdateObject( );
// 		}
// 	}
// 	else
// 	{
// 		if( !IsHover() )
// 		{
// 			this->SetHover( true );
// 			this->UpdateObject( );
// 		}
// 	}
// 	
// }
// void ButtonBase::OnMouseLeave()
// {
// 	this->SetHover( false );
// 	this->SetPress( false );
// 	this->UpdateObject( );
// }
// void ButtonBase::OnLButtonDown(UINT nFlags, POINT point)
// {
// 	this->SetPress( true );
// 	this->UpdateObject( );
// }
void ButtonBase::OnLButtonDblClk(UINT nFlags, POINT point)
{
	// 为了让Hook拦截到这一次的点击事件，如双击滚动条上的按钮
	UIMSG  msg;
	msg.pMsgTo = m_pIButtonBase;
	msg.message = WM_LBUTTONDOWN;
	msg.wParam  = (WPARAM)nFlags;
	msg.lParam  = MAKELPARAM(point.x,point.y);
	::UISendMessage( &msg );  
}

void  ButtonBase::OnLButtonDown(UINT nFlags, POINT point)
{
    if (m_pIButtonBase->TestStyleEx(BUTTON_STYLE_CLICK_ONDOWN))
    {
        this->OnClicked(&point);  
    }   
}
void ButtonBase::OnLButtonUp(UINT nFlags, POINT point)
{
    if (m_pIButtonBase->TestStyleEx(BUTTON_STYLE_CLICK_ONDOWN))
        return;

    IObject* pParent = m_pIButtonBase->GetParentObject();
    if (NULL == pParent)
        return;

	POINT ptObj = {0,0};
	pParent->WindowPoint2ObjectClientPoint(&point, &ptObj);

    // UI_WM_HITTEST是判断与自己的m_rcParent的交集，因此要将窗口坐标转换成parent的client坐标
    UINT nHitTest = UISendMessage(m_pIButtonBase, UI_WM_HITTEST, ptObj.x, ptObj.y);
    if (HTNOWHERE != nHitTest)
	{
		this->OnClicked(&point);   // 备注：由于DoNotify可能导致当前press hover对象发生改变，使得本控件丢失刷新
	}
}
void  ButtonBase::OnRButtonUp(UINT nFlags, POINT point)
{
	UIMSG   msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode   = UI_BN_RCLICK;
	msg.wParam  = NULL; //(WPARAM)pt;
	msg.lParam  = NULL;
	msg.pMsgFrom = this->m_pIButtonBase;

	m_pIButtonBase->DoNotify(&msg);
}

void ButtonBase::OnClicked(POINT* pt)
{
	// 通知消息
	UIMSG   msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode   = UI_BN_CLICKED;
	msg.wParam  = NULL; //(WPARAM)pt;
	msg.lParam  = NULL;
	msg.pMsgFrom = this->m_pIButtonBase;
    
    msg.pMsgTo = m_pIButtonBase;
    BOOL bHandled = FALSE;
    UISendMessage(&msg, 0, &bHandled); // 由自己的派生类进行处理，也由派生类去通知窗口，这样一个UI_BN_CLICKED消息就能统一起来
}

void  ButtonBase::DrawFocus(IRenderTarget* p)
{
    UIMSG msg;
    msg.pMsgTo = m_pIButtonBase;
    msg.wParam = (WPARAM)p;
    msg.nCode = UI_BN_DRAWFOCUS;
    msg.message = UI_WM_NOTIFY;

    UISendMessage(&msg);
}

void ButtonBase::OnSetFocus(IObject*)
{
}
void ButtonBase::OnKillFocus(IObject*)
{
    if (Util::IsKeyDown(VK_SPACE))
    {
        bool bIsForePress = m_pIButtonBase->IsForcePress();
        m_pIButtonBase->clearStateBit(OSB_FORCEPRESS);
        if (bIsForePress)
        {
            m_pIButtonBase->UpdateObject();
        }
    }
}
void ButtonBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (VK_SPACE == nChar)
	{
        // windows控件在SPACE按下时，会去为Button窗口xxxBNSetCapture，
        // 这样所有的鼠标消息都只跑到它里面来
		m_pIButtonBase->setStateBit(OSB_FORCEPRESS);
		m_pIButtonBase->UpdateObject();

        m_pIButtonBase->SetMouseCapture(BUTTON_VK_SPACE_MOUSECAPTURE_NOTIFY_ID);
	}
}
void ButtonBase::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (VK_SPACE == nChar)
	{
		m_pIButtonBase->clearStateBit(OSB_FORCEPRESS);
        m_pIButtonBase->ReleaseMouseCapture();

		if (!Util::IsKeyDown(VK_LBUTTON))
		{
			// 用于子类扩展
            POINT pt = {0,0};
            UIMSG* pCurMsg = m_pIButtonBase->GetCurMsg();
            if (pCurMsg)
                pt = pCurMsg->pt;

			this->OnClicked(&pt);

		}
		m_pIButtonBase->UpdateObject();
	}
}

void ButtonBase::SetCheck(int nCheckState)
{
	if(nCheckState == BST_CHECKED)
		this->SetChecked();
	else
		this->SetUnChecked();
}

void ButtonBase::SetChecked()
{
	if (m_nCheckState & BST_CHECKED)
	{

	}
	else
	{
		m_nCheckState |= BST_CHECKED;
		m_pIButtonBase->UpdateObject();
	}
}

void ButtonBase::SetUnChecked()
{
	if ( m_nCheckState & BST_CHECKED )
	{
		m_nCheckState &= ~BST_CHECKED;
		m_pIButtonBase->UpdateObject();
	}
	else
	{

	}
}
int  ButtonBase::GetCheck()
{
	return m_nCheckState;
}
bool ButtonBase::IsChecked()
{
	return (m_nCheckState & BST_CHECKED) ? true:false;
}


//////////////////////////////////////////////////////////////////////////
Button::Button()
{
    m_pIButton = NULL;
}
Button::~Button()
{
}

HRESULT Button::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = ButtonBase::FinalConstruct(p);
    if (FAILED(hr))
        return hr;

    m_pIButtonBase->SetStyleEx(MAKECONTROLSUBTYPE(m_pIButtonBase->GetStyleEx(), BUTTON_STYLE_PUSHBUTTON));
    return S_OK;
}

void  Button::OnObjectLoaded()
{
    SetMsgHandled(FALSE);

    // 检测自己是否是DefaultPushButton
    IMapAttribute*  pMapAttr = NULL;
    m_pIButton->GetMapAttribute(&pMapAttr);
    
    bool bDefault = false;
    pMapAttr->GetAttr_bool(XML_BUTTON_DEFPUSHBUTTON, true, &bDefault);
    if (bDefault)
    {
        IWindowBase* pWindow = m_pIButton->GetWindowObject();
        if (pWindow)
        {
            UISendMessage(pWindow, UI_DM_SETDEFID, (WPARAM)m_pIButton);
        }
    }

    SAFE_RELEASE(pMapAttr);
}

void  Button::SetText(const TCHAR*  szText, bool bUpdate) 
{
    if (szText)
        m_strText = szText;
    else
        m_strText.clear();

    if (bUpdate)
    {
        m_pIButton->UpdateObject();
    }
}
void  Button::SetIconFromFile(const TCHAR* szIconPath)
{
    m_pIButton->SetForegndRender(NULL);
    if (NULL == szIconPath)
        return;

    IUIApplication*  pUIApplication = m_pIButton->GetUIApplication();
    if (NULL == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    IRenderBase*   pForeRender = NULL;
    pUIApplication->CreateRenderBase(RENDER_TYPE_IMAGE, m_pIButton, &pForeRender);
    IImageRender*  pImageForeRender = (IImageRender*)pForeRender->QueryInterface(UI::uiiidof(IImageRender));

    UI::IRenderBitmap*  pRenderBitmap = NULL;
    UI::UICreateRenderBitmap(GetRenderLibraryType(m_pIButton), IMAGE_ITEM_TYPE_IMAGE, &pRenderBitmap);
    pRenderBitmap->LoadFromFile(szIconPath, true);
    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);

    m_pIButton->SetForegndRender(pForeRender);
    SAFE_RELEASE(pForeRender);
}

// 该HBITMAP最后会被RenderBitmap所销毁
void  Button::SetIconFromHBITMAP(HBITMAP hBitmap)
{
    m_pIButton->SetForegndRender(NULL);
    if (NULL == hBitmap)
        return;

    IUIApplication*  pUIApplication = m_pIButton->GetUIApplication();
    if (NULL == pUIApplication)
    {
        UIASSERT(0);
        return;
    }

    IRenderBase*   pForeRender = NULL;
    pUIApplication->CreateRenderBase(RENDER_TYPE_IMAGE, m_pIButton, &pForeRender);
    IImageRender*  pImageForeRender = (IImageRender*)pForeRender->QueryInterface(UI::uiiidof(IImageRender));

    UI::IRenderBitmap*  pRenderBitmap = NULL;
    UI::UICreateRenderBitmap(UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, IMAGE_ITEM_TYPE_IMAGE, &pRenderBitmap);
    pRenderBitmap->Attach(hBitmap, true);
    pImageForeRender->SetRenderBitmap(pRenderBitmap);
    pImageForeRender->SetImageDrawType(UI::DRAW_BITMAP_CENTER);
    SAFE_RELEASE(pRenderBitmap);

    m_pIButton->SetForegndRender(pForeRender);
    SAFE_RELEASE(pForeRender);
}

const TCHAR*  Button::GetText() 
{ 
    return m_strText.c_str(); 
}

void  Button::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    // 按钮的auto size可以按背景图标大小来算，也可以按内容的大小来算
    switch (m_nAutoSizeType)
    {
    case BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE:
        {
            if (m_pIButton->GetBkRender())
            {
                *pSize = m_pIButton->GetBkRender()->GetDesiredSize( );
            }
        }
        break;

    case BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT:
        {
            pSize->cx = m_pIButton->GetNonClientW();
            pSize->cy = m_pIButton->GetNonClientH();

            SIZE sIconText = {0,0};
            {
                SIZE sizeIcon = {0,0};
                SIZE sizeText = {0,0};

                if (m_pIButton->GetForeRender())
                {
                    sizeIcon = m_pIButton->GetForeRender()->GetDesiredSize();
                }
                if (m_pIButton->GetTextRender())
                {
                    sizeText = m_pIButton->GetTextRender()->GetDesiredSize(m_strText.c_str());
                }

                switch (m_nIconAlignFlag)
                {
                case ALIGN_LEFT:
                case ALIGN_RIGHT:
                    {
                        sIconText.cx = sizeIcon.cx + sizeText.cx + m_nIconMarginText;
                        sIconText.cy = max( sizeIcon.cy, sizeText.cy );
                    }
                    break;
                case ALIGN_TOP:
                case ALIGN_BOTTOM:
                    {
                        sIconText.cx = max( sizeIcon.cx, sizeText.cx );
                        sIconText.cy = sizeIcon.cy + sizeText.cy + m_nIconMarginText;
                    }
                    break;
                }
            }
            pSize->cx += sIconText.cx;
            pSize->cy += sIconText.cy;
        }
        break;

    case BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH:
        {
            SIZE sHeight = {0,0};
            if (m_pIButton->GetBkRender())
                sHeight = m_pIButton->GetBkRender()->GetDesiredSize( );

            SIZE sWidth;
            sWidth.cx = m_pIButton->GetNonClientW();
            sWidth.cy = m_pIButton->GetNonClientH();

            SIZE sIconText = {0,0};
            {
                SIZE sizeIcon = {0,0};
                SIZE sizeText = {0,0};

                if (m_pIButton->GetForeRender())
                {
                    sizeIcon = m_pIButton->GetForeRender()->GetDesiredSize(  );
                }
                if (m_pIButton->GetTextRender())
                {
                    sizeText = m_pIButton->GetTextRender()->GetDesiredSize(m_strText.c_str());
                }

                switch (m_nIconAlignFlag)
                {
                case ALIGN_LEFT:
                case ALIGN_RIGHT:
                    {
                        sIconText.cx = sizeIcon.cx + sizeText.cx + m_nIconMarginText;
                        sIconText.cy = max( sizeIcon.cy, sizeText.cy );
                    }
                    break;
                case ALIGN_TOP:
                case ALIGN_BOTTOM:
                    {
                        sIconText.cx = max( sizeIcon.cx, sizeText.cx );
                        sIconText.cy = sizeIcon.cy + sizeText.cy + m_nIconMarginText;
                    }
                    break;
                }

                sWidth.cx += sIconText.cx;
                sWidth.cy += sIconText.cy;
            }

            pSize->cx = sWidth.cx;
            pSize->cy = sHeight.cy;
        }

    default:
        UI_LOG_WARN( _T("ButtonBase::GetAutoSize  Unknown autosize type: %d"), m_nAutoSizeType);
        break;
    }
}

UINT  Button::OnGetDlgCode(LPMSG lpMsg)
{   
    UINT nRet = DLGC_BUTTON;
    if (m_pIButton->IsDefault())
    {
        nRet |= DLGC_DEFPUSHBUTTON;
    }
    else
    {
        if (m_pIButton->CanTabstop() && !m_pIButton->TestStyleEx(BUTTON_STYLE_NOTDEFPUSH))
        {
            nRet |= DLGC_UNDEFPUSHBUTTON;  // 表示现在不是默认按钮，但自己可以成为默认按钮
        }
    }

    return nRet;
}

void Button::ResetAttribute()
{
    __super::ResetAttribute();

    m_nAutoSizeType   = BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT;
    m_nIconMarginText = 0;
    m_nIconAlignFlag  = ALIGN_LEFT;
    m_nDrawFocusType  = BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT;

    m_pIButton->ModifyStyleEx(0, BUTTON_STYLE_NOTDEFPUSH, 0);
}

void Button::SetAttribute(IMapAttribute* pMapAttrib, bool bReload )
{
    ButtonBase::SetAttribute(pMapAttrib, bReload);

    const TCHAR* pszText = pMapAttrib->GetAttr(XML_BUTTON_RENDER_AUTOSIZE_TYPE, true);
    if (pszText)
    {
        if (0 == _tcscmp(XML_BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE, pszText))
        {
            m_nAutoSizeType = BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE;
        }
        else if (0 == _tcscmp(XML_BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT, pszText))
        {
            m_nAutoSizeType = BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT;
        }
        else if (0 == _tcscmp(XML_BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH, pszText))
        {
            m_nAutoSizeType = BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH;
        }
        else
        {
            m_nAutoSizeType = BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT;
        }
    }

    pszText = pMapAttrib->GetAttr(XML_BUTTON_ICON_ALIGN, true);
    if (pszText)
    {
        if (0 == _tcscmp(XML_ALIGN_LEFT, pszText))
        {
            m_nIconAlignFlag = ALIGN_LEFT;
        }
        else if (0 == _tcscmp(XML_ALIGN_RIGHT, pszText))
        {
            m_nIconAlignFlag = ALIGN_RIGHT;
        }
        else if (0 == _tcscmp(XML_ALIGN_TOP, pszText))
        {
            m_nIconAlignFlag = ALIGN_TOP;
        }
        else if (0 == _tcscmp(XML_ALIGN_BOTTOM, pszText))
        {
            m_nIconAlignFlag = ALIGN_BOTTOM;
        }
        else if (0 == _tcscmp(XML_ALIGN_CENTER, pszText))
        {
            m_nIconAlignFlag = ALIGN_CENTER;
        }
    }

    if (false == bReload)
    {
        pszText = pMapAttrib->GetAttr(XML_BUTTON_TEXT, true);
        if (pszText)
            m_strText = pszText;
        else
            m_strText.clear();
    }

    int nButtonStyle = GetButtonStyle();
    if (nButtonStyle >= BUTTON_STYLE_HAVE_TEXT_FIRST && nButtonStyle <= BUTTON_STYLE_HAVE_TEXT_LAST)
    {
        ITextRenderBase* pTextRender = NULL;
        pMapAttrib->GetAttr_TextRenderBase(XML_TEXTRENDER_TYPE, true, m_pIButton->GetUIApplication(), static_cast<IObject*>(m_pIButton), &pTextRender);
        if (pTextRender)
        {
            m_pIButton->SetTextRender(pTextRender);
            pTextRender->Release();
        }

        if (NULL == m_pIButton->GetTextRender())
        {
            m_pIButton->GetUIApplication()->CreateTextRenderBase(TEXTRENDER_TYPE_NORMAL, this->m_pIButton, &pTextRender);
            if (pTextRender)
            {
                pTextRender->SetAttribute(pMapAttrib, NULL, true);
                this->m_pIButton->SetTextRender(pTextRender);
                pTextRender->Release();
            }
        }
    }

    if (NULL == m_pIButton->GetBkRender() && this->GetButtonStyle() == BUTTON_STYLE_PUSHBUTTON)
    {
        IRenderBase* p = NULL;
        m_pIButton->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_BUTTON_BKGND, this->m_pIButton, &p);
        if (p)
        {
            p->SetAttribute(pMapAttrib, XML_BACKGND_RENDER_PREFIX, true);
            m_pIButton->SetBkgndRender(p);
            p->Release();
            m_pIButton->ModifyStyle(OBJECT_STYLE_TRANSPARENT,0, false);
        }
    }

    pMapAttrib->GetAttr_int(XML_BUTTON_ICON_TEXT_MARGIN, true, &m_nIconMarginText);
    pMapAttrib->GetAttr_int(XML_BUTTON_RENDER_DRAW_FOCUS_FLAG, true, &m_nDrawFocusType);

    bool bNotDefpushbutton = 0;
    pMapAttrib->GetAttr_bool(XML_BUTTON_NOTDEFPUSHBUTTON, true, &bNotDefpushbutton);
    if (bNotDefpushbutton)
    {
        m_pIButton->ModifyStyleEx(BUTTON_STYLE_NOTDEFPUSH, 0, 0);
    }
}

void  Button::OnEditorGetAttrList(IUIEditor* pEditor, IUIEditorGroupAttribute*  pRootAttr)
{
	__super::OnEditorGetAttrList(pEditor, pRootAttr);

	IUIEditorGroupAttribute*  pObjGroup = pEditor->CreateGroupAttribute(pRootAttr, Button::GetXmlName(), NULL);
	pEditor->CreateTextAttribute(pObjGroup, XML_BUTTON_RENDER_AUTOSIZE_TYPE, NULL);
	pEditor->CreateTextAttribute(pObjGroup, XML_BUTTON_ICON_ALIGN, NULL);
	pEditor->CreateTextAttribute(pObjGroup, XML_BUTTON_TEXT, NULL);
	pEditor->CreateTextAttribute(pObjGroup, XML_TEXTRENDER_TYPE, NULL);
	pEditor->CreateTextAttribute(pObjGroup, XML_BUTTON_ICON_TEXT_MARGIN, NULL);
	pEditor->CreateTextAttribute(pObjGroup, XML_BUTTON_RENDER_DRAW_FOCUS_FLAG, NULL);
	pEditor->CreateTextAttribute(pObjGroup, XML_BUTTON_NOTDEFPUSHBUTTON, NULL);
}
void Button::SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType)
{
    m_nDrawFocusType = eType;
}
void Button::SetAutoSizeType(BUTTON_RENDER_AUTO_SIZE_TYPE eType)
{
    m_nAutoSizeType = eType;
}

//
// 根据参数的设置，计算出图标和文本的绘制区域
//
//	Remark:
//	   求出的位置没有去计算居中位置
//
void Button::analyzeIconTextRect(IRenderTarget* pRenderTarget, CRect& rcIcon, CRect& rcText)
{
    m_pIButton->GetClientRectAsWin32(&rcIcon);
    rcText.CopyRect(rcIcon);

    SIZE s = {0,0};
    if (m_pIButton->GetForeRender())
    {
        s = m_pIButton->GetForeRender()->GetDesiredSize();
    }

    switch (m_nIconAlignFlag)
    {
    case ALIGN_LEFT:
        {
            rcIcon.right = rcIcon.left + s.cx;
            rcText.left  = rcIcon.right + m_nIconMarginText;
        }
        break;
    case ALIGN_TOP:
        {
            rcIcon.bottom = rcIcon.top + s.cy;
            rcText.top = rcIcon.bottom + m_nIconMarginText;
        }
        break;
    case ALIGN_RIGHT:
        {
            rcIcon.left = rcIcon.right - s.cx;
            rcText.right = rcIcon.left - m_nIconMarginText;
        }
        break;
    case ALIGN_BOTTOM:
        {
            rcIcon.top = rcIcon.bottom - s.cy;
            rcText.bottom = rcIcon.top - m_nIconMarginText;
        }
        break;

    case ALIGN_CENTER:
        {
            rcIcon.left = (rcIcon.Width() - s.cx)/2;
            rcIcon.right = rcIcon.left + s.cx;
            rcIcon.top = (rcIcon.Height() - s.cy)/2;
            rcIcon.bottom = rcIcon.top + s.cy;
        }
        break;
    }
}

void Button::OnPaint(IRenderTarget* pRenderTarget)
{
    CRect rcIcon, rcText;

    this->analyzeIconTextRect(pRenderTarget, rcIcon, rcText);

    bool  bDisable = !m_pIButton->IsEnable();
    bool  bHover   = m_pIButton->IsHover();
    bool  bPress   = m_pIButton->IsPress();
    bool  bChecked = IsChecked() || m_pIButton->IsSelected();
    bool  bForePress = m_pIButton->IsForcePress();

    int  nState = 0;
    if (bDisable)
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:BUTTON_ICON_RENDER_STATE_DISABLE;
    }
    else if (bForePress || (bPress && bHover))
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:BUTTON_ICON_RENDER_STATE_PRESS;
    }
    else if (bHover || bPress)
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:BUTTON_ICON_RENDER_STATE_HOVER;
    }
    else 
    {
        nState = bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:BUTTON_ICON_RENDER_STATE_NORMAL;
    }

    if (m_pIButton->GetForeRender())
        m_pIButton->GetForeRender()->DrawState(pRenderTarget, &rcIcon, nState);
    if (m_pIButton->GetTextRender())
        m_pIButton->GetTextRender()->DrawState(pRenderTarget, &rcText, nState, m_strText.c_str()/*, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS*/);

    return;
}

LRESULT  Button::DrawFocus(WPARAM w, LPARAM)
{
    IRenderTarget* pRenderTarget = (IRenderTarget*)w;
    switch(m_nDrawFocusType)
    {
    case BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE:
        break;
    case BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT:
        {
            CRect  rc;
            switch (this->GetButtonStyle())
            {
            case BUTTON_STYLE_RADIOBUTTON:
            case BUTTON_STYLE_CHECKBUTTON:
                {
                    CRect rcIcon, rcText;
                    this->analyzeIconTextRect(pRenderTarget, rcIcon, rcText );  // 返回的坐标是基于client的

                    this->m_pIButton->ClientRect2ObjectRect(&rcText, &rc);   
                }
                break;

            case BUTTON_STYLE_HYPERLINK:
                {
                    this->m_pIButton->GetClientRect(&rc);
                }
                break;

                //case BUTTON_STYLE_PUSHBUTTON:
            default:
                {
                    rc.SetRect(0,0, m_pIButton->GetWidth(), m_pIButton->GetHeight());
                    ::InflateRect( &rc, -3, -3 );
                }
                break;
            }

            pRenderTarget->DrawFocusRect(&rc);
        }
        break;
    }

    return 0;
}

void  Button::OnClicked()
{
    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode   = UI_BN_CLICKED;
    msg.wParam  = NULL; //(WPARAM)&pt;
    msg.lParam  = NULL;
    msg.pMsgFrom = m_pIButton;
    LRESULT lRet = m_pIButton->DoNotify(&msg);
}

}


