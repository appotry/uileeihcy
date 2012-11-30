#include "stdafx.h"

ButtonBase::ButtonBase()
{
	m_nCheckState = BST_UNCHECKED;
	this->ModifyStyle(BUTTON_STYLE_PUSHBUTTON);
	this->SetTabstop( true ); 
	m_bSpaceKeyDown = false;
}

HRESULT ButtonBase::FinalConstruct()
{
	this->ResetAttribute();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               消息映射                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

void ButtonBase::ResetAttribute()
{
	Control::ResetAttribute();

	m_nAutoSizeType   = BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT;
	m_nIconMarginText = 0;
	m_nIconTextFlag   = BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_LEFT;
	m_nDrawFocusType  = BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT;

//	::SetRect( &m_rcPadding, 1,1,1,1 );
}
bool ButtonBase::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	if( false == Control::SetAttribute(mapAttrib, bReload) )
		return false;

	ATTRMAP::iterator iter = m_mapAttribute.find(XML_BUTTON_RENDER_AUTOSIZE_TYPE);
	if (m_mapAttribute.end() != iter)
	{
		String& strType = iter->second;
		if( strType == XML_BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE )
		{
			m_nAutoSizeType = BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE;
		}
		else if( strType == XML_BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT )
		{
			m_nAutoSizeType = BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT;
		}
		else if( strType == XML_BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH )
		{
			m_nAutoSizeType = BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH;
		}
		else
		{
			m_nAutoSizeType = BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT;
		}
		m_mapAttribute.erase(iter);
	}

	int nButtonStyle = GetButtonStyle();
	if (nButtonStyle >= BUTTON_STYLE_HAVE_TEXT_FIRST && nButtonStyle <= BUTTON_STYLE_HAVE_TEXT_LAST)
	{
		iter = m_mapAttribute.find(XML_TEXTRENDER_TYPE);
		if (m_mapAttribute.end() != iter)
		{
			SAFE_DELETE(m_pTextRender);
			const String& strTextRenderType = iter->second;
			m_pTextRender = TextRenderFactory::GetTextRender(strTextRenderType, this);
			m_pTextRender->SetAttribute(_T(""),m_mapAttribute);

			this->m_mapAttribute.erase(iter);
		}
		else if( NULL == m_pTextRender )
		{
			m_pTextRender = TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_NORMAL, this);
			m_pTextRender->SetAttribute(_T(""),m_mapAttribute);
		}
	}

	if (NULL == m_pBkgndRender && this->GetButtonStyle() == BUTTON_STYLE_PUSHBUTTON)
	{
		m_pBkgndRender = RenderFactory::GetRender( RENDER_TYPE_THEME, this);
		this->ModifyStyle(OBJECT_STYLE_TRANSPARENT,0);
	}

	iter = m_mapAttribute.find(XML_BUTTON_TEXT);
	if (false == bReload && m_mapAttribute.end() != iter)
	{
		m_strText = iter->second;
		m_mapAttribute.erase(iter);
	}
	iter = m_mapAttribute.find(XML_BUTTON_RENDER_DRAW_FOCUS_FLAG);
	if (m_mapAttribute.end() != iter)
	{
		m_nDrawFocusType = _ttoi( iter->second.c_str() );
		m_mapAttribute.erase(iter);
	}

	return true;
}
SIZE ButtonBase::GetAutoSize()
{
	SIZE s = {0,0};

	// 按钮的auto size可以按背景图标大小来算，也可以按内容的大小来算
	switch( m_nAutoSizeType )
	{
	case BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE:
		{
			if( NULL != m_pBkgndRender )
			{
				s = m_pBkgndRender->GetDesiredSize( );
			}
		}
		break;

	case BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT:
		{
			s.cx = this->GetPaddingW();
			s.cy = this->GetPaddingH();

			SIZE sIconText = {0,0};
			{
				SIZE sizeIcon = {0,0};
				SIZE sizeText = {0,0};

				if (NULL != m_pForegndRender)
				{
					sizeIcon = m_pForegndRender->GetDesiredSize();
				}
				if (NULL != m_pTextRender)
				{
					sizeText = m_pTextRender->GetDesiredSize(m_strText);
				}

				switch (m_nIconTextFlag)
				{
				case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_LEFT:
				case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_RIGHT:
					{
						sIconText.cx = sizeIcon.cx + sizeText.cx + m_nIconMarginText;
						sIconText.cy = max( sizeIcon.cy, sizeText.cy );
					}
					break;
				case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_TOP:
				case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_BOTTOM:
					{
						sIconText.cx = max( sizeIcon.cx, sizeText.cx );
						sIconText.cy = sizeIcon.cy + sizeText.cy + m_nIconMarginText;
					}
					break;
				}
			}
			s.cx += sIconText.cx;
			s.cy += sIconText.cy;
		}
		break;

	case BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH:
		{
			SIZE sHeight = {0,0};
			if( NULL != m_pBkgndRender )
				sHeight = m_pBkgndRender->GetDesiredSize( );

			SIZE sWidth;
			sWidth.cx = this->GetPaddingW();
			sWidth.cy = this->GetPaddingH();

			SIZE sIconText = {0,0};
			{
				SIZE sizeIcon = {0,0};
				SIZE sizeText = {0,0};

				if( NULL != m_pForegndRender )
				{
					sizeIcon = m_pForegndRender->GetDesiredSize(  );
				}
				if( NULL != m_pTextRender )
				{
					sizeText = m_pTextRender->GetDesiredSize( m_strText );
				}

				switch(m_nIconTextFlag)
				{
				case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_LEFT:
				case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_RIGHT:
					{
						sIconText.cx = sizeIcon.cx + sizeText.cx + m_nIconMarginText;
						sIconText.cy = max( sizeIcon.cy, sizeText.cy );
					}
					break;
				case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_TOP:
				case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_BOTTOM:
					{
						sIconText.cx = max( sizeIcon.cx, sizeText.cx );
						sIconText.cy = sizeIcon.cy + sizeText.cy + m_nIconMarginText;
					}
					break;
				}

				sWidth.cx += sIconText.cx;
				sWidth.cy += sIconText.cy;
			}

			s.cx = sWidth.cx;
			s.cy = sHeight.cy;
		}

	default:
		UI_LOG_WARN( _T("ButtonBase::GetAutoSize  Unknown autosize type: %d"), m_nAutoSizeType );
		break;
	}

	return s;
}

//	按钮的类型
int  ButtonBase::GetButtonStyle()
{
	return m_nStyle & BUTTON_STYLE_MASK;
}
void ButtonBase::SetButtonStyle(int n)
{
	if(n > BUTTON_STYLE_MASK)
		return;

	m_nStyle &= ~BUTTON_STYLE_MASK;
	m_nStyle |= n;
}

void ButtonBase::SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType)
{
	m_nDrawFocusType = eType;
}
void ButtonBase::SetAutoSizeType(BUTTON_RENDER_AUTO_SIZE_TYPE eType)
{
	m_nAutoSizeType = eType;
}

void ButtonBase::OnEraseBkgnd(HRDC hRDC)
{
	if( NULL != m_pBkgndRender )
	{
		bool  bDisable = !IsEnable();
		bool  bHover   = IsHover();
		bool  bPress   = IsPress();
		bool  bForePress = IsForePress();
		bool  bChecked = IsChecked();

		CRect rc(0,0, GetWidth(), GetHeight());

		if( bDisable )
		{
			this->m_pBkgndRender->DrawState(hRDC, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_DISABLE:BUTTON_BKGND_RENDER_STATE_DISABLE);
		}
		else if( bForePress || (bPress && bHover) )
		{
			this->m_pBkgndRender->DrawState(hRDC, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_PRESS:BUTTON_BKGND_RENDER_STATE_PRESS);
		}
		else if( bHover || bPress )
		{
			this->m_pBkgndRender->DrawState(hRDC, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_HOVER:BUTTON_BKGND_RENDER_STATE_HOVER);
		}
		else 
		{
			this->m_pBkgndRender->DrawState(hRDC, &rc, bChecked?BUTTON_BKGND_RENDER_STATE_SELECTED_NORMAL:BUTTON_BKGND_RENDER_STATE_NORMAL);
		}
	}
}



//
// 根据参数的设置，计算出图标和文本的绘制区域
//
//	Remark:
//	   求出的位置没有去计算居中位置
//
void ButtonBase::analyzeIconTextRect( HRDC hRDC, CRect& rcIcon, CRect& rcText)
{
	int nWidth = GetWidth();
	int nHeight = GetHeight();

	GetClientRect( &rcIcon );
	rcText.CopyRect(rcIcon);

	SIZE s = {0,0};
	if( NULL != m_pForegndRender )
	{
		s = m_pForegndRender->GetDesiredSize();
	}

	switch(m_nIconTextFlag)
	{
	case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_LEFT:
		{
			rcIcon.right = rcIcon.left + s.cx;
			rcText.left  = rcIcon.right + m_nIconMarginText;
		}
		break;
	case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_TOP:
		{
			rcIcon.bottom = rcIcon.top + s.cy;
			rcText.top = rcIcon.bottom + m_nIconMarginText;
		}
		break;
	case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_RIGHT:
		{
			rcIcon.left = rcIcon.right - s.cx;
			rcText.right = rcIcon.left - m_nIconMarginText;
		}
		break;
	case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_BOTTOM:
		{
			rcIcon.top = rcIcon.bottom - s.cy;
			rcText.bottom = rcIcon.top - m_nIconMarginText;
		}
		break;
	}
}
void ButtonBase::OnPaint(HRDC hRDC)
{
	CRect rcIcon, rcText;

	this->analyzeIconTextRect( hRDC, rcIcon, rcText );

	bool  bDisable = !IsEnable();
	bool  bHover   = IsHover();
	bool  bPress   = IsPress();
	bool  bChecked = IsChecked();
	bool  bForePress = IsForePress();

	if (bDisable)
	{
		if (NULL != m_pForegndRender)
		{
			m_pForegndRender->DrawState(hRDC, &rcIcon, bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:BUTTON_ICON_RENDER_STATE_DISABLE );
		}
		if (NULL != m_pTextRender)
		{
			m_pTextRender->DrawState(hRDC, &rcText, BUTTON_BKGND_RENDER_STATE_DISABLE, m_strText, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS );
		}
	}
	else if (bForePress || (bPress && bHover))
	{
		if (NULL != m_pForegndRender)
		{
			m_pForegndRender->DrawState(hRDC, &rcIcon, bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:BUTTON_ICON_RENDER_STATE_PRESS);
		}
		if (NULL != m_pTextRender)
		{
			m_pTextRender->DrawState(hRDC, &rcText, BUTTON_BKGND_RENDER_STATE_PRESS, m_strText, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		}
	}
	else if (bHover || bPress)
	{
		if (NULL != m_pForegndRender)
		{
			m_pForegndRender->DrawState(hRDC, &rcIcon, bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:BUTTON_ICON_RENDER_STATE_HOVER );
		}
		if (NULL != m_pTextRender)
		{
			m_pTextRender->DrawState(hRDC, &rcText, BUTTON_BKGND_RENDER_STATE_HOVER, m_strText, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS );
		}
	}
	else 
	{
		if (NULL != m_pForegndRender)
		{
			m_pForegndRender->DrawState(hRDC, &rcIcon, bChecked?BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:BUTTON_ICON_RENDER_STATE_NORMAL);
		}
		if (NULL != m_pTextRender)
		{
			m_pTextRender->DrawState(hRDC, &rcText, BUTTON_BKGND_RENDER_STATE_NORMAL, m_strText, DT_VCENTER|DT_CENTER|DT_SINGLELINE|DT_END_ELLIPSIS);
		}
	}

	if (IsFocus())
	{
		this->DrawFocus(hRDC, &rcIcon, &rcText);
	}	
	return;
}


void ButtonBase::DrawFocus( HRDC hRDC, CRect* prcIcon, CRect* prcText )
{
	switch(m_nDrawFocusType)
	{
	case BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE:
		break;
	case BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT:
		{
			CRect  rc;
			switch( this->GetButtonStyle() )
			{
			case BUTTON_STYLE_RADIOBUTTON:
			case BUTTON_STYLE_CHECKBUTTON:
				{
					CopyRect(&rc, prcText);
				}
				break;

			case BUTTON_STYLE_HYPERLINK:
				{
					this->GetClientRect(&rc);
				}
				break;

				//case BUTTON_STYLE_PUSHBUTTON:
			default:
				{
					this->GetClientRect(&rc);
					::InflateRect( &rc, -3, -3 );
				}
				break;
			}

			DrawFocusRect( hRDC, &rc );
		}
		break;
	}
}

void ButtonBase::OnStateChanged(int nOld, int nNew)
{
	this->UpdateObject();
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
// void ButtonBase::OnLButtonDblClk(UINT nFlags, POINT point)
// {
// 	this->OnLButtonDown(nFlags,point);
// }
void ButtonBase::OnLButtonUp(UINT nFlags, POINT point)
{
	UIASSERT(NULL != this->GetParentObject());

	POINT ptObj = {0,0};
	this->GetParentObject()->WindowPoint2ObjectPoint(&point, &ptObj);
	if (HTNOWHERE != this->OnHitTest(&ptObj))
	{
		// 用于子类扩展
		this->OnClicked();   // 备注：由于DoNotify可能导致当前press hover对象发生改变，使得本控件丢失刷新，因此将OnClicked提前处理

		// 通知消息
		UIMSG   msg;
		msg.message = UI_WM_NOTIFY;
		msg.code    = UI_BN_CLICKED;
		msg.wParam  = (WPARAM)&point;
		msg.lParam  = NULL;
		msg.pObjMsgFrom = this;
		this->DoNotify( &msg );
	}

// 	this->SetPress( false );
// 	this->UpdateObject( );
}
void ButtonBase::OnSetFocus( Object* )
{
}
void ButtonBase::OnKillFocus( Object* )
{
	this->clearStateBit( CSB_FORCEPRESS );
}
void ButtonBase::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if( VK_SPACE == nChar )
	{
		this->setStateBit(CSB_FORCEPRESS);
		this->UpdateObject();
	}
}
void ButtonBase::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if( VK_SPACE == nChar )
	{
		this->clearStateBit(CSB_FORCEPRESS);
		if( !Util::IsKeyDown(VK_LBUTTON) )
		{
			// 通知消息
			UIMSG   msg;
			msg.message = UI_WM_NOTIFY;
			msg.code    = UI_BN_CLICKED;
			msg.wParam  = (WPARAM)NULL;
			msg.lParam  = NULL;
			msg.pObjMsgFrom = this;
			this->DoNotify( &msg );

			// 用于子类扩展
			this->OnClicked();
		}
		this->UpdateObject();
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
	if ( m_nCheckState & BST_CHECKED )
	{

	}
	else
	{
		m_nCheckState |= BST_CHECKED;
		this->UpdateObject();
	}
}

void ButtonBase::SetUnChecked()
{
	if ( m_nCheckState & BST_CHECKED )
	{
		m_nCheckState &= ~BST_CHECKED;
		this->UpdateObject();
	}
	else
	{

	}
}
int ButtonBase::GetCheck()
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

}
Button::~Button()
{

}

RadioButtonBase::RadioButtonBase()
{
	this->ModifyStyle(BUTTON_STYLE_RADIOBUTTON);
}


//
//	TODO:
//		在这里注释掉了tab stop的变化，因为当前还没有实现使用方向键来切换同一个组内的
//		其它对象。同时也觉得使用方向键来切换同一个组内的对象没有使用TAB键切换所有对象
//		方便。
//
void RadioButtonBase::OnClicked()
{
	if( m_nCheckState & BST_CHECKED )
	{
	}
	else
	{
		this->SetCheck(BST_CHECKED);
//		this->SetTabstop(true);

		// 取消其它radio按钮的选择状态

		bool bFind = false;
		if( !this->IsGroup() ) // 不是第一个选项.
		{
			Object* pPrevRadioButton = this->GetPrevObject();  // 往前遍历
			while (1)
			{
				if( NULL == pPrevRadioButton || (Object*)this == pPrevRadioButton )
					break;

				if( pPrevRadioButton->GetObjectName() != RadioButton::XmlName() )  // TODO: 有没有别的办法判断是否是radio button控件类型？
					break;

				if( ((ButtonBase*)pPrevRadioButton)->IsChecked() )
				{
					bFind = true;
					((ButtonBase*)pPrevRadioButton)->SetCheck(BST_UNCHECKED);
//					((ButtonBase*)pPrevRadioButton)->SetTabstop(false);
					break;
				}

				if( pPrevRadioButton->IsGroup() )
					break;

				pPrevRadioButton = pPrevRadioButton->GetPrevObject();
			}

		}

		if( !bFind )
		{
			Object* pNextRadioButton = this->GetNextObject();      // 往后遍历
			while(1)
			{
				if( NULL == pNextRadioButton || (Object*)this == pNextRadioButton )
					break;

				if( pNextRadioButton->IsGroup() )
					break;

				if( pNextRadioButton->GetObjectName() != RadioButton::XmlName() )  // TODO: 有没有别的办法判断是否是radio button控件类型？
					break;

				if( ((ButtonBase*)pNextRadioButton)->IsChecked() )
				{
					bFind = true;
					((ButtonBase*)pNextRadioButton)->SetCheck(BST_UNCHECKED);
//					((ButtonBase*)pPrevRadioButton)->SetTabstop(false);
					break;
				}

				pNextRadioButton = pNextRadioButton->GetNextObject();
			}
		}
	}

	
}

CheckButtonBase::CheckButtonBase()
{
	this->ModifyStyle(BUTTON_STYLE_CHECKBUTTON);
}

void CheckButtonBase::OnClicked()
{
	if( m_nCheckState & BST_CHECKED )
	{
		m_nCheckState &= ~BST_CHECKED;
	}
	else
	{
		m_nCheckState |= BST_CHECKED;
	}

//	this->UpdateObject();
}

CheckButton::CheckButton()
{
}

CheckButton::~CheckButton()
{
}

bool RadioButtonBase::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = ButtonBase::SetAttribute(mapAttrib, bReload);
	if( false == bRet )
		return false;

	if( NULL == m_pBkgndRender )
	{
		this->ModifyStyle(OBJECT_STYLE_TRANSPARENT);  // Radio默认是透明的

		if( NULL == m_pForegndRender )
		{
			m_pForegndRender = RenderFactory::GetRender( RENDER_TYPE_THEME, this );
			m_nIconMarginText = 3;
		}
	}
	return true;
}
bool CheckButtonBase::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = ButtonBase::SetAttribute(mapAttrib, bReload);
	if( false == bRet )
		return false;

	if( NULL == m_pBkgndRender )
	{
		this->ModifyStyle(OBJECT_STYLE_TRANSPARENT);  // check默认是透明的

		if( NULL == m_pForegndRender )
		{
			m_pForegndRender = RenderFactory::GetRender( RENDER_TYPE_THEME, this );
			m_nIconMarginText = 4;
		}
	}

	return true;
}

HyperLink::HyperLink()
{
	this->ModifyStyle(BUTTON_STYLE_HYPERLINK);
	::SetRect( &m_rcPadding, 0,0,0,0 );
}
bool HyperLink::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	if (0 == mapAttrib.count( XML_TEXTRENDER_TYPE))
	{
		if (NULL == m_pTextRender)
		{
			HRFONT hRFont = this->GetFont();
			m_pTextRender = TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_FONTCOLORLIST, this);
			if (NULL != m_pTextRender)
			{
				FontColorListTextRender* p = (FontColorListTextRender*)m_pTextRender;
				p->SetCount(4);
				p->SetColor(0, RGB(0,0,255));
				p->SetColor(1, RGB(255,0,0));
				p->SetColor(2, RGB(255,0,0));
				p->SetColor(3, RGB(192,192,192));

				HRFONT hUnderLineFont = UI_GetFontEx(hRFont, FONTITEM_FLAG_UNDERLINE, 0, GetGraphicsRenderType(GetHWND()) );
				p->SetFont(0, hRFont);
				p->SetFont(1, hUnderLineFont);
				p->SetFont(2, hUnderLineFont);
				p->SetFont(3, hRFont);
				UI_ReleaseFont(hUnderLineFont);
			}
		}
	}

	bool bRet = ButtonBase::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	this->ModifyStyle(OBJECT_STYLE_TRANSPARENT);   // Hyperlink默认是透明的
	return true;
}

BOOL HyperLink::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message )
{
	SetCursor( ::LoadCursor(NULL, IDC_HAND) );
	return TRUE;
}
void HyperLink::OnClicked()
{
	String strHref;
	if( this->GetAttribute(XML_HYPERLINK_HREF, strHref) )
	{
		::ShellExecute(NULL,_T("open"), strHref.c_str(), NULL, NULL, SW_SHOW );
	}
}