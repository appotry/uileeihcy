#include "stdafx.h"


//////////////////////////////////////////////////////////////////////////
//
//                          ICON
//

_ButtonIconRender::_ButtonIconRender( ButtonBase* pButton )
{
	m_hIcon = NULL;
	m_pButton = pButton;
}
_ButtonIconRender::~_ButtonIconRender()
{
	m_pButton = NULL;
	if( NULL != m_hIcon )
	{
		::UI_ReleaseBitmap(m_hIcon);
		m_hIcon = NULL;
	}
}

void _ButtonIconRender::ResetAttribute()
{
	if( NULL != m_hIcon )
	{
		UI_ReleaseBitmap(m_hIcon);
		m_hIcon = NULL;
	}
}
bool  _ButtonIconRender::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	if( 0 != mapAttrib.count(XML_BUTTON_ICON) )
	{
		String strImage = mapAttrib[XML_BUTTON_ICON];
		m_hIcon = ::UI_GetBitmap( strImage, GetGraphicsRenderType(m_pButton->GetHWND()) );

		this->m_pButton->EraseAttribute(XML_BUTTON_ICON);
	}

	return true;
}
//
//	获取图标的宽和高
//
SIZE  _ButtonIconRender::GetAutoSize(HRDC hRDC)
{	
	SIZE s = {0,0};
	if ( NULL != m_hIcon )
	{
		s.cx = UI_GetBitmapWidth(m_hIcon);
		s.cy = UI_GetBitmapHeight(m_hIcon);
	}
	return s;
}


ButtonIconStaticRender::ButtonIconStaticRender( ButtonBase* pButton) : _ButtonIconRender( pButton )
{

}
ButtonIconStaticRender::~ButtonIconStaticRender()
{

}

void ButtonIconStaticRender::DrawDisable( HRDC hRDC, RECT* prc )
{
	this->DrawNormal( hRDC, prc );
}
void ButtonIconStaticRender::DrawNormal( HRDC hRDC, RECT* prc )
{
	
	if( NULL != this->m_hIcon )
	{
		// 居中对齐
		int nImageW = UI_GetBitmapWidth(m_hIcon);
		int nImageH = UI_GetBitmapHeight(m_hIcon);

		int x = prc->left + ( Util::RectW(prc) - nImageW ) / 2;
		int y = prc->top  + ( Util::RectH(prc) - nImageH ) / 2;

		DrawBitmap( hRDC, m_hIcon, x, y );
	}
}
void ButtonIconStaticRender::DrawHover( HRDC hRDC, RECT* prc )
{
	this->DrawNormal( hRDC, prc );
}
void ButtonIconStaticRender::DrawPress( HRDC hRDC, RECT* prc )
{
	this->DrawNormal( hRDC, prc );
}

//////////////////////////////////////////////////////////////////////////

ButtonIcon8StateRender::ButtonIcon8StateRender( ButtonBase* pButton) : _ButtonIconRender( pButton )
{
	m_nIconItemWidth = m_nIconItemHeight = 0;
}
ButtonIcon8StateRender::~ButtonIcon8StateRender()
{

}

bool ButtonIcon8StateRender::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if( false == bRet )
		return false;

	if( NULL != m_hIcon )
	{
		m_nIconItemWidth =  UI_GetBitmapWidth(m_hIcon)/4;
		m_nIconItemHeight = UI_GetBitmapHeight(m_hIcon)/2;
	}
	
	return true;
}
SIZE  ButtonIcon8StateRender::GetAutoSize( HRDC hRDC )
{
	SIZE s;
	s.cx = m_nIconItemWidth;
	s.cy = m_nIconItemHeight;

	return s;
}


void ButtonIcon8StateRender::draw_item( HRDC hRDC, CRect* prc, int xIcon, int yIcon )
{
	if( NULL == m_hIcon )
		return;

	// 居中对齐
	int x = prc->left + ( Util::RectW(prc) - m_nIconItemWidth ) / 2;
	int y = prc->top  + ( Util::RectH(prc) - m_nIconItemHeight ) / 2;

	ImageList_Draw( hRDC, m_hIcon, x, y, xIcon, yIcon, m_nIconItemWidth, m_nIconItemHeight );
}
void ButtonIcon8StateRender::DrawDisable( HRDC hRDC, RECT* prc )
{
	this->draw_item( hRDC, prc, 3, m_pButton->IsChecked()?1:0 );
}
void ButtonIcon8StateRender::DrawNormal( HRDC hRDC, RECT* prc )
{
	this->draw_item( hRDC, prc, 0, m_pButton->IsChecked()?1:0 );
}
void ButtonIcon8StateRender::DrawHover( HRDC hRDC, RECT* prc )
{
	this->draw_item( hRDC, prc, 1, m_pButton->IsChecked()?1:0 );
}
void ButtonIcon8StateRender::DrawPress( HRDC hRDC, RECT* prc )
{
	this->draw_item( hRDC, prc, 2, m_pButton->IsChecked()?1:0 );
}

//////////////////////////////////////////////////////////////////////////

ButtonIconThemeRadioRender::ButtonIconThemeRadioRender( ButtonBase* pButton) : _ButtonIconRender( pButton )
{
	m_hTheme = ::OpenThemeData(NULL, L"Button");
}
ButtonIconThemeRadioRender::~ButtonIconThemeRadioRender()
{
	if( m_hTheme )
	{
		::CloseThemeData( m_hTheme );
		m_hTheme = NULL;
	}
}

void ButtonIconThemeRadioRender::OnThemeChanged()
{
	::CloseThemeData(m_hTheme);
	m_hTheme = ::OpenThemeData(NULL, L"Button");
}
SIZE  ButtonIconThemeRadioRender::GetAutoSize( HRDC hRDC )
{
	SIZE s = {12,12};

	if( NULL != m_hTheme )
	{
		HDC hDC = GetHDC(hRDC);
		GetThemePartSize(m_hTheme,hDC,BP_RADIOBUTTON,RBS_UNCHECKEDNORMAL, NULL, TS_DRAW, &s );
		ReleaseHDC(hRDC,hDC);
	}

	return s;
}
void ButtonIconThemeRadioRender::DrawDisable( HRDC hRDC, RECT* prc )
{
	CRect rc;
	m_pButton->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, m_pButton->IsChecked()?RBS_CHECKEDDISABLED:CBS_UNCHECKEDDISABLED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonIconThemeRadioRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_INACTIVE;
		if( m_pButton->IsChecked() )
			nFlag |= DFCS_CHECKED;

		DrawFrameControl(hDC, prc, DFC_BUTTON, nFlag );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonIconThemeRadioRender::DrawNormal( HRDC hRDC, RECT* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		prc->top -= 1;
		prc->bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, m_pButton->IsChecked()?RBS_CHECKEDNORMAL:RBS_UNCHECKEDNORMAL, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonIconThemeRadioRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO;
		if( m_pButton->IsChecked() )
			nFlag |= DFCS_CHECKED;;
		DrawFrameControl(hDC, prc, DFC_BUTTON, nFlag );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonIconThemeRadioRender::DrawHover( HRDC hRDC, RECT* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		prc->top -= 1;
		prc->bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, m_pButton->IsChecked()?RBS_CHECKEDHOT:RBS_UNCHECKEDHOT, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonIconThemeRadioRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_HOT;
		if( m_pButton->IsChecked() )
			nFlag |= DFCS_CHECKED;

		DrawFrameControl(hDC, prc, DFC_BUTTON, nFlag );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonIconThemeRadioRender::DrawPress( HRDC hRDC, RECT* prc )
{
	CRect rc;
	m_pButton->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		prc->top -= 1;
		prc->bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, m_pButton->IsChecked()?RBS_CHECKEDPRESSED:RBS_UNCHECKEDPRESSED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonIconThemeRadioRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_PUSHED;
		if( m_pButton->IsChecked() )
			nFlag |= DFCS_CHECKED;

		DrawFrameControl(hDC, prc, DFC_BUTTON, nFlag );
	}
	ReleaseHDC(hRDC, hDC);
}



//////////////////////////////////////////////////////////////////////////

ButtonIconThemeCheckRender::ButtonIconThemeCheckRender( ButtonBase* pButton) : _ButtonIconRender( pButton )
{
	m_hTheme = ::OpenThemeData(NULL, L"Button");
}
ButtonIconThemeCheckRender::~ButtonIconThemeCheckRender()
{
	if( m_hTheme )
	{
		::CloseThemeData( m_hTheme );
		m_hTheme = NULL;
	}
}

void ButtonIconThemeCheckRender::OnThemeChanged()
{
	::CloseThemeData(m_hTheme);
	m_hTheme = ::OpenThemeData(NULL, L"Button");
}
SIZE  ButtonIconThemeCheckRender::GetAutoSize( HRDC hRDC )
{
	SIZE s = {13,13};

	if( NULL != m_hTheme )
	{
		HDC hDC = GetHDC(hRDC);
		GetThemePartSize(m_hTheme,hDC,BP_CHECKBOX,CBS_UNCHECKEDNORMAL, NULL, TS_DRAW, &s );
		ReleaseHDC(hRDC,hDC);
	}

	return s;
}
void ButtonIconThemeCheckRender::DrawDisable( HRDC hRDC, RECT* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		prc->top -= 1;
		prc->bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX,  m_pButton->IsChecked()?CBS_CHECKEDDISABLED:CBS_UNCHECKEDDISABLED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonIconThemeRadioRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_INACTIVE;
		if( m_pButton->IsChecked() )
			nFlag |= DFCS_CHECKED;

		DrawFrameControl(hDC, prc, DFC_BUTTON, nFlag );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonIconThemeCheckRender::DrawNormal( HRDC hRDC, RECT* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		prc->top -= 1;
		prc->bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX, m_pButton->IsChecked()?CBS_CHECKEDNORMAL:CBS_UNCHECKEDNORMAL, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonIconThemeRadioRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK;
		if( m_pButton->IsChecked() )
			nFlag |= DFCS_CHECKED;

		DrawFrameControl(hDC, prc, DFC_BUTTON, nFlag );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonIconThemeCheckRender::DrawHover( HRDC hRDC, RECT* prc )
{

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		prc->top -= 1;
		prc->bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX, m_pButton->IsChecked()?CBS_CHECKEDHOT:CBS_UNCHECKEDHOT, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonIconThemeRadioRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_HOT;
		if( m_pButton->IsChecked() )
			nFlag |= DFCS_CHECKED;

		DrawFrameControl(hDC, prc, DFC_BUTTON, nFlag );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonIconThemeCheckRender::DrawPress( HRDC hRDC, RECT* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		prc->top -= 1;
		prc->bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX,  m_pButton->IsChecked()?CBS_CHECKEDPRESSED:CBS_UNCHECKEDPRESSED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonIconThemeRadioRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_PUSHED;
		if( m_pButton->IsChecked() )
			nFlag |= DFCS_CHECKED;

		DrawFrameControl(hDC, prc, DFC_BUTTON, nFlag );
	}
	ReleaseHDC(hRDC, hDC);
}

//////////////////////////////////////////////////////////////////////////
//
//                          TEXT
//

_ButtonTextRender::_ButtonTextRender( ButtonBase* pButton )
{
	m_pButton = pButton;
	m_colNormal = m_colHover = m_colPress = 0;
	m_colDisable = RGB(205,205,205);
}

_ButtonTextRender::~_ButtonTextRender()
{
	m_pButton = NULL;
}

SIZE  _ButtonTextRender::GetAutoSize( HRDC hRDC )
{
	HRFONT hRFont = (HRFONT)UISendMessage(m_pButton,WM_GETFONT);
	SIZE s = MeasureString(hRFont, this->m_text.c_str() );

	// 为了在DrawFocusRect的时候，不会去覆盖文字，在这里将所需要的大小四周都扩大1px
	s.cx +=2;
	s.cy +=2;

	return s;
}

bool _ButtonTextRender::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	if( 0 != mapAttrib.count(XML_BUTTON_TEXT) )
	{
		m_text = mapAttrib[XML_BUTTON_TEXT];
		this->m_pButton->EraseAttribute(XML_BUTTON_TEXT);
	}

	// TODO:
	::OutputDebugString(_T("_ButtonTextRender::SetAttribute, 没对添加设置颜色的属性"));
	return true;
}

ButtonTextSinglelineRender::ButtonTextSinglelineRender( ButtonBase*  pButton ) : _ButtonTextRender(pButton)
{

}
ButtonTextSinglelineRender::~ButtonTextSinglelineRender( )
{

}

void ButtonTextSinglelineRender::DrawDisable( HRDC hRDC, RECT* prc )
{
	HRFONT hRFont = (HRFONT)UISendMessage(m_pButton, WM_GETFONT);
	DrawString( hRDC, m_text.c_str(), prc, DT_VCENTER|DT_CENTER|DT_SINGLELINE, hRFont, m_colDisable );
}
void ButtonTextSinglelineRender::DrawNormal( HRDC hRDC, RECT* prc )
{
	HRFONT hRFont = (HRFONT)UISendMessage(m_pButton, WM_GETFONT);
	DrawString( hRDC, m_text.c_str(), prc, DT_VCENTER|DT_CENTER|DT_SINGLELINE, hRFont, m_colNormal );
}
void ButtonTextSinglelineRender::DrawHover( HRDC hRDC, RECT* prc )
{
	HRFONT hRFont = (HRFONT)UISendMessage(m_pButton, WM_GETFONT);
	DrawString( hRDC, m_text.c_str(), prc, DT_VCENTER|DT_CENTER|DT_SINGLELINE, hRFont, m_colHover );
}
void ButtonTextSinglelineRender::DrawPress( HRDC hRDC, RECT* prc )
{
	HRFONT hRFont = (HRFONT)UISendMessage(m_pButton, WM_GETFONT);
	DrawString( hRDC, m_text.c_str(), prc, DT_VCENTER|DT_CENTER|DT_SINGLELINE, hRFont, m_colPress );
}

ButtonTextHyperLinkRender::ButtonTextHyperLinkRender( ButtonBase*  pButton ) : ButtonTextSinglelineRender(pButton)
{
	m_colNormal = m_colHover = RGB(0,0,255);
	m_colPress = RGB(255,0,0);
	m_colDisable = RGB(192,192,192);
}
ButtonTextHyperLinkRender::~ButtonTextHyperLinkRender()
{
}


void ButtonTextHyperLinkRender::DrawDisable( HRDC hRDC, RECT* prc )
{
	__super::DrawDisable(hRDC, prc);
}
void ButtonTextHyperLinkRender::DrawNormal( HRDC hRDC, RECT* prc )
{
	__super::DrawNormal(hRDC, prc);
}
void ButtonTextHyperLinkRender::DrawHover( HRDC hRDC, RECT* prc )
{
// 	HFONT hFontUnderLine = this->CreateFontUnderLine(hRDC);
// 	HFONT hFontOld = SelectFont( hRDC, hFontUnderLine );
	__super::DrawHover(hRDC, prc);
// 	SelectFont( hRDC, hFontOld );
// 	DeleteObject(hFontUnderLine);
}
void ButtonTextHyperLinkRender::DrawPress( HRDC hRDC, RECT* prc )
{
// 	HFONT hFontUnderLine = this->CreateFontUnderLine(hRDC);
// 	HFONT hFontOld = SelectFont( hRDC, hFontUnderLine );
	__super::DrawPress(hRDC, prc);
// 	SelectFont( hRDC, hFontOld );
// 	DeleteObject(hFontUnderLine);
}

// HFONT ButtonTextHyperLinkRender::CreateFontUnderLine(HRDC hRDC)
// {
// 	HFONT hNormalFont = GetFont(hRDC);
// 	if( NULL == hNormalFont )
// 		return NULL;
// 
// 	LOGFONT lf;
// 	::GetObject(hNormalFont, sizeof(lf),&lf);
// 	lf.lfUnderline = 1;
// 	return  CreateFontIndirect(&lf);
// }

//////////////////////////////////////////////////////////////////////////
//
//                    ICON  &&  TEXT
//

ButtonIconTextRender::ButtonIconTextRender( ButtonBase*  pButton )
{
	m_pIconRender = NULL;
	m_pTextRender = NULL;
	m_nIconMarginText = 0;

	m_nFlag   = BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_LEFT;
	m_nDrawFocusType = BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT;
	m_pButton = pButton;
}
ButtonIconTextRender::~ButtonIconTextRender()
{
	if( NULL != m_pIconRender )
	{
		delete m_pIconRender;
		m_pIconRender = NULL;
	}
	if( NULL != m_pTextRender )
	{
		delete m_pTextRender ;
		m_pTextRender = NULL;
	}
}

void ButtonIconTextRender::SetTextRenderType( BUTTON_TEXT_RENDER_TYPE eType )
{
	SAFE_DELETE(m_pTextRender);

	switch(eType)
	{
	case BUTTON_TEXT_RENDER_TYPE_HYPERLINK:
		m_pTextRender = new ButtonTextHyperLinkRender(this->m_pButton);
		break;

	case BUTTON_TEXT_RENDER_TYPE_SINGLELINE:
		m_pTextRender = new ButtonTextSinglelineRender(this->m_pButton);
		break;
	}
}

void ButtonIconTextRender::ResetAttribute()
{
	m_nFlag = BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_LEFT;
	m_nDrawFocusType = BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT;
	m_nIconMarginText = 0;
}
bool ButtonIconTextRender::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	//////////////////////////////////////////////////////////////////////////
	// 有图标需要绘制

	  if( 0 != mapAttrib.count(XML_BUTTON_ICON) )                
	{
		if( 0 != mapAttrib.count(XML_BUTTON_ICON_RENDER_TYPE) )       // 绘制类型
		{
			String strType = mapAttrib[XML_BUTTON_ICON_RENDER_TYPE];
			if( XML_BUTTON_ICON_RENDER_TYPE_STATIC == strType )
			{
				m_pIconRender = new ButtonIconStaticRender(this->m_pButton);
			}
			else if( XML_BUTTON_ICON_RENDER_TYPE_8STATE == strType )
			{
				m_pIconRender = new ButtonIcon8StateRender(this->m_pButton);
			}
			else
			{
				UI_LOG_WARN( _T("ButtonIconTextRender::SetAttribute  unknown %s : %s"), XML_BUTTON_ICON_RENDER_TYPE, strType.c_str() );
			}
			this->m_pButton->EraseAttribute(XML_BUTTON_ICON_RENDER_TYPE);
		}
		else
		{
			m_pIconRender = new ButtonIconStaticRender(m_pButton);        // 默认值
		}
	}
	else
	{
		// 使用主题绘制时，不需要图标
		if( 0 != mapAttrib.count(XML_BUTTON_ICON_RENDER_TYPE) )
		{
			String strType = mapAttrib[XML_BUTTON_ICON_RENDER_TYPE];
			if( XML_BUTTON_ICON_RENDER_TYPE_THEME_RADIO == strType )
			{
				m_pIconRender = new ButtonIconThemeRadioRender(m_pButton);
				m_nIconMarginText = 4;
			}
			else if( XML_BUTTON_ICON_RENDER_TYPE_THEME_CHECK == strType )
			{
				m_pIconRender = new ButtonIconThemeCheckRender(m_pButton);
				m_nIconMarginText = 3;
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////
	// 需要绘制文本

	if ( 0 != mapAttrib.count(XML_BUTTON_TEXT) )               
	{
		if( 0 != mapAttrib.count(XML_BUTTON_TEXT_RENDER_TYPE) )       // 绘制类型
		{
			String strType = mapAttrib[XML_BUTTON_TEXT_RENDER_TYPE];
			if( XML_BUTTON_TEXT_RENDER_TYPE_SINGLELINE == strType )
			{
				m_pTextRender = new ButtonTextSinglelineRender(this->m_pButton);
			}
			else if( XML_BUTTON_TEXT_RENDER_TYPE_HYPERLINK == strType )
			{
				m_pTextRender = new ButtonTextHyperLinkRender(this->m_pButton);
			}
			else
			{
				UI_LOG_WARN( _T("ButtonIconTextRender::SetAttribute  unknown %s : %s"), XML_BUTTON_TEXT_RENDER_TYPE, strType.c_str() );
			}
			this->m_pButton->EraseAttribute(XML_BUTTON_TEXT_RENDER_TYPE);
		}
		else
		{
			if( m_pButton->GetButtonStyle() == BUTTON_STYLE_HYPERLINK )
				m_pTextRender = new ButtonTextHyperLinkRender(m_pButton);        // 默认值
			else
				m_pTextRender = new ButtonTextSinglelineRender(m_pButton);        // 默认值
		}
	}

	if( 0 != mapAttrib.count(XML_BUTTON_RENDER_DRAW_FOCUS_FLAG) )
	{
		m_nDrawFocusType = _ttoi( mapAttrib[XML_BUTTON_RENDER_DRAW_FOCUS_FLAG].c_str() );
		this->m_pButton->EraseAttribute(XML_BUTTON_RENDER_DRAW_FOCUS_FLAG);
	}

	if ( NULL != m_pIconRender )
		m_pIconRender->SetAttribute( mapAttrib,bReload );
	if( NULL != m_pTextRender )
		m_pTextRender->SetAttribute(mapAttrib,bReload);

	return true;
}

void ButtonIconTextRender::DrawDisable( HRDC hRDC  )
{
	RECT rcIcon, rcText;

	this->analyzeIconTextRect( hRDC, rcIcon, rcText );
	if ( NULL != m_pIconRender )
	{
		m_pIconRender->DrawDisable( hRDC, &rcIcon );
	}
	if( NULL != m_pTextRender )
	{
		m_pTextRender->DrawDisable( hRDC, &rcText );
	}
}
void ButtonIconTextRender::DrawNormal( HRDC hRDC )
{
	RECT rcIcon, rcText;

	this->analyzeIconTextRect( hRDC, rcIcon, rcText );
	if ( NULL != m_pIconRender )
	{
		m_pIconRender->DrawNormal( hRDC, &rcIcon );
	}
	if( NULL != m_pTextRender )
	{
		m_pTextRender->DrawNormal( hRDC, &rcText );
	}

	if( m_pButton->IsFocus() )
	{
		this->DrawFocus( hRDC, &rcIcon, &rcText );
	}
}
void ButtonIconTextRender::DrawHover( HRDC hRDC )
{
	RECT rcIcon, rcText;

	this->analyzeIconTextRect( hRDC, rcIcon, rcText );
	if ( NULL != m_pIconRender )
	{
		m_pIconRender->DrawHover( hRDC, &rcIcon );
	}
	if( NULL != m_pTextRender )
	{
		m_pTextRender->DrawHover( hRDC, &rcText );
	}

	if( m_pButton->IsFocus() )
	{
		this->DrawFocus( hRDC, &rcIcon, &rcText );
	}
}
void ButtonIconTextRender::DrawPress( HRDC hRDC )
{
	RECT rcIcon, rcText;

	this->analyzeIconTextRect( hRDC, rcIcon, rcText );
	if ( NULL != m_pIconRender )
	{
		m_pIconRender->DrawPress( hRDC, &rcIcon );
	}
	if( NULL != m_pTextRender )
	{
		m_pTextRender->DrawPress( hRDC, &rcText );
	}

	if( m_pButton->IsFocus() )
	{
		this->DrawFocus( hRDC, &rcIcon, &rcText );
	}
}


//
//	当按钮的autosize type 为 content时，通过本函数获取按钮大小
//
SIZE ButtonIconTextRender::GetAutoSize(HRDC hRDC)
{
	SIZE s = {0,0};
	SIZE sizeIcon = {0,0};
	SIZE sizeText = {0,0};

	if( NULL != m_pIconRender )
	{
		sizeIcon = m_pIconRender->GetAutoSize( hRDC );
	}
	if( NULL != m_pTextRender )
	{
		sizeText = m_pTextRender->GetAutoSize(hRDC);
	}
	
	switch(m_nFlag)
	{
	case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_LEFT:
	case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_RIGHT:
		{
			s.cx = sizeIcon.cx + sizeText.cx + m_nIconMarginText;
			s.cy = max( sizeIcon.cy, sizeText.cy );
		}
		break;
	case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_TOP:
	case BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_BOTTOM:
		{
			s.cx = max( sizeIcon.cx, sizeText.cx );
			s.cy = sizeIcon.cy + sizeText.cy + m_nIconMarginText;
		}
		break;
	}

	// 默认值
// 	if( s.cx == 0 && s.cy == 0 )
// 	{
// 		s.cx = 16;
// 		s.cy = 16;
// 	}

	return s;
}

void ButtonIconTextRender::DrawFocus( HRDC hRDC, RECT* prcIcon, RECT* prcText )
{
	switch(m_nDrawFocusType)
	{
	case BUTTON_RENDER_DRAW_FOCUS_TYPE_NULL:
		break;
	case BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT:
		{
			CRect  rc(0,0,0,0);
			switch( m_pButton->GetButtonStyle() )
			{
			case BUTTON_STYLE_RADIOBUTTON:
			case BUTTON_STYLE_CHECKBUTTON:
				{
					rc.CopyRect(prcText);
				}
				break;

			case BUTTON_STYLE_HYPERLINK:
				{
					m_pButton->GetClientRect(&rc);
				}
				break;

			//case BUTTON_STYLE_PUSHBUTTON:
			default:
				{
					m_pButton->GetClientRect(&rc);
					::InflateRect( &rc, -3, -3 );
				}
				break;
			}

			DrawFocusRect( hRDC, &rc );
		}
		break;
	}
}

//
// 根据参数的设置，计算出图标和文本的绘制区域
//
//	Remark:
//	   求出的位置没有去计算居中位置
//
void ButtonIconTextRender::analyzeIconTextRect( HRDC hRDC, RECT& rcIcon, RECT& rcText)
{
	int nWidth = m_pButton->GetWidth();
	int nHeight = m_pButton->GetHeight();

	::SetRect( &rcIcon, 
		this->m_pButton->GetPaddingL(), 
		this->m_pButton->GetPaddingT(), 
		nWidth - this->m_pButton->GetPaddingR(),
		nHeight - this->m_pButton->GetPaddingB() 
		);
	::CopyRect( &rcText, &rcIcon );

	SIZE s = {0,0};
	if( NULL != m_pIconRender )
	{
		s = m_pIconRender->GetAutoSize(hRDC);
	}

	switch(m_nFlag)
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

BackRenderBase::BackRenderBase( Object* pObj )
{
	m_pObject = pObj;
}
BackRenderBase*  BackRenderBase::CreateInstance(const String& strType, Object* pObj)
{
	if( NULL == pObj )
		return NULL;

	BackRenderBase* p = NULL;
#if 0
	if( XML_RENDER_TYPE_COLOR == strType )
	{
		p = new ColorBackRender( pObj );
	}
	else if ( XML_RENDER_TYPE_IMAGE == strType )
	{ 
		p = new ImageListRender( pObj );
	}
	else if ( XML_RENDER_TYPE_THEME == strType )
	{
		if( pObj->GetObjectName() == Button::XmlName() )
			p = new ButtonBkThemeRender( (ButtonBase*)pObj );
 		else if( pObj->GetObjectName() == Edit::XmlName() )
 			p = new EditBkThemeRender((Edit*)pObj);
		else if( pObj->GetObjectName() == GroupBox::XmlName() )
			p = new GroupBoxBkThemeRender((GroupBox*)pObj);
		else
		{
			UI_LOG_WARN( _T("%s\t invalid theme Object type=%s"), _T(__FUNCTION__),  pObj->GetObjectName().c_str() );
		}

	}
	else
	{
		UI_LOG_WARN( _T("%s\t invalid strType=%s"), _T(__FUNCTION__), strType.c_str() );
	}
#endif
	return p;
}
#if 0
ColorBackRender::ColorBackRender( Object* pObj ) : BackRenderBase( pObj )
{
	m_nFlag = XML_BACK_RENDER_COLOR_DRAW_2STATE;

	m_colNormalBrush = 0;
	m_colNormalPen   = 0;
	m_colHoverBrush  = RGB(182,189,210);
	m_colHoverPen    = RGB(10,36,106);
	m_colPressBrush  = RGB(133,146,181);
	m_colPressPen    = RGB(10,36,106);
	m_colDisableBrush= 0;
	m_colDisablePen  = 0;
}
bool ColorBackRender::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	if( false == BackRenderBase::SetAttribute(mapAttrib,bReload) )
		return false;

	if( mapAttrib.count( XML_BACK_RENDER_COLOR_BK_NORMAL_COLOR ) )
	{
		String strColor = mapAttrib[XML_BACK_RENDER_COLOR_BK_NORMAL_COLOR];
		this->m_colNormalBrush = Util::TranslateRGB( strColor );
		this->m_pObject->EraseAttribute( XML_BACK_RENDER_COLOR_BK_NORMAL_COLOR );
	}
	if( mapAttrib.count( XML_BACK_RENDER_COLOR_BK_HVOER_COLOR ) )
	{
		String strColor = mapAttrib[XML_BACK_RENDER_COLOR_BK_HVOER_COLOR];
		this->m_colHoverBrush = Util::TranslateRGB( strColor );
		this->m_pObject->EraseAttribute( XML_BACK_RENDER_COLOR_BK_HVOER_COLOR );
	}
	if( mapAttrib.count( XML_BACK_RENDER_COLOR_BK_PRESS_COLOR ) )
	{
		String strColor = mapAttrib[XML_BACK_RENDER_COLOR_BK_PRESS_COLOR];
		this->m_colPressBrush = Util::TranslateRGB( strColor );
		this->m_pObject->EraseAttribute( XML_BACK_RENDER_COLOR_BK_PRESS_COLOR );
	}
	if( mapAttrib.count( XML_BACK_RENDER_COLOR_BK_DISABLE_COLOR ) )
	{
		String strColor = mapAttrib[XML_BACK_RENDER_COLOR_BK_DISABLE_COLOR];
		this->m_colDisableBrush = Util::TranslateRGB( strColor );
		this->m_pObject->EraseAttribute( XML_BACK_RENDER_COLOR_BK_DISABLE_COLOR );
	}

	if( mapAttrib.count( XML_BACK_RENDER_COLOR_BORDER_NORMAL_COLOR ) )
	{
		String strColor = mapAttrib[XML_BACK_RENDER_COLOR_BORDER_NORMAL_COLOR];
		this->m_colNormalPen = Util::TranslateRGB( strColor );
		this->m_pObject->EraseAttribute( XML_BACK_RENDER_COLOR_BORDER_NORMAL_COLOR );
	}
	if( mapAttrib.count( XML_BACK_RENDER_COLOR_BORDER_HVOER_COLOR ) )
	{
		String strColor = mapAttrib[XML_BACK_RENDER_COLOR_BORDER_HVOER_COLOR];
		this->m_colHoverPen = Util::TranslateRGB( strColor );
		this->m_pObject->EraseAttribute( XML_BACK_RENDER_COLOR_BORDER_HVOER_COLOR );
	}
	if( mapAttrib.count( XML_BACK_RENDER_COLOR_BORDER_PRESS_COLOR ) )
	{
		String strColor = mapAttrib[XML_BACK_RENDER_COLOR_BORDER_PRESS_COLOR];
		this->m_colPressPen = Util::TranslateRGB( strColor );
		this->m_pObject->EraseAttribute( XML_BACK_RENDER_COLOR_BORDER_PRESS_COLOR );
	}
	if( mapAttrib.count( XML_BACK_RENDER_COLOR_BORDER_DISABLE_COLOR ) )
	{
		String strColor = mapAttrib[XML_BACK_RENDER_COLOR_BORDER_DISABLE_COLOR];
		this->m_colDisablePen = Util::TranslateRGB( strColor );
		this->m_pObject->EraseAttribute( XML_BACK_RENDER_COLOR_BORDER_DISABLE_COLOR );
	}

	if( mapAttrib.count( XML_BACK_RENDER_COLOR_FLAG ) )
	{
		String nFlag = mapAttrib[XML_BACK_RENDER_COLOR_FLAG];
		this->m_nFlag = _ttoi( nFlag.c_str() );
		this->m_pObject->EraseAttribute( XML_BACK_RENDER_COLOR_FLAG );
	}

	return true;
}

//
//	使用一个画刷和一个画笔来填充背景
//
void ColorBackRender::drawbk( HRDC hRDC, COLORREF colbrush, COLORREF colpen )
{
	RECT  rc;
	m_pObject->GetClientRect(&rc);
	Rectangle(hRDC, &rc,colpen,1,colbrush,false);
}

void ColorBackRender::DrawDisable( HRDC hRDC  )
{
	if( m_nFlag & XML_BACK_RENDER_COLOR_DRAW_DISALBE )
	{
		this->drawbk( hRDC, m_colDisableBrush, m_colDisablePen );
	}
}
void ColorBackRender::DrawNormal( HRDC hRDC )
{
	if( m_nFlag & XML_BACK_RENDER_COLOR_DRAW_NORMAL )
	{
		this->drawbk( hRDC, m_colNormalBrush, m_colNormalPen );
	}
}
void ColorBackRender::DrawHover( HRDC hRDC )
{
	if( m_nFlag & XML_BACK_RENDER_COLOR_DRAW_HOVER )
	{
		this->drawbk( hRDC, m_colHoverBrush, m_colHoverPen );
	}
}
void ColorBackRender::DrawPress( HRDC hRDC )
{
	if( m_nFlag & XML_BACK_RENDER_COLOR_DRAW_PRESS )
	{
		this->drawbk( hRDC, m_colPressBrush, m_colPressPen );
	}
}

//////////////////////////////////////////////////////////////////////////

ImageListRender::ImageListRender( Object* pObj ): BackRenderBase(pObj)
{
// 	m_pImage = NULL;
	m_hBitmap = NULL;
 	m_nItemWidth = m_nItemHeight = 0;
	m_nImageDirection = IMAGELIST_LAYOUT_TYPE_H;
}
ImageListRender::~ImageListRender( )
{
	if( NULL != m_hBitmap )
	{
		::UI_ReleaseBitmap(m_hBitmap);
		m_hBitmap = NULL;
	}
}
bool ImageListRender::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	if( false == BackRenderBase::SetAttribute(mapAttrib,bReload) )
		return false;

 	if( 0 != mapAttrib.count(XML_BACK_RENDER_IMAGE_BKIMAGE) )
 	{
		if( NULL != m_hBitmap )
		{
			::UI_ReleaseBitmap(m_hBitmap);
			m_hBitmap = NULL;
		}
		m_hBitmap = ::UI_GetBitmap( mapAttrib[XML_BACK_RENDER_IMAGE_BKIMAGE], GetRenderType(m_pObject->GetHWND()) );

		this->m_pObject->EraseAttribute(XML_BACK_RENDER_IMAGE_BKIMAGE);
 	}

	if( 0 != mapAttrib.count(XML_RENDER_IMAGELIST_LAYOUT) )
 	{
 		String str = mapAttrib[XML_RENDER_IMAGELIST_LAYOUT];
 		if ( str == XML_RENDER_IMAGELIST_LAYOUT_H )
 		{
 			m_nImageDirection = IMAGELIST_LAYOUT_TYPE_H;
 		}
 		else if ( str == XML_RENDER_IMAGELIST_LAYOUT_V )
 		{
 			m_nImageDirection = IMAGELIST_LAYOUT_TYPE_V;
 		}
 		else
 		{
 			UI_LOG_WARN(_T("ImageButton::SetAttribute  \"%s\" is invalid: %s"), XML_RENDER_IMAGELIST_LAYOUT, str.c_str() );
 		}
 		this->m_pObject->EraseAttribute(XML_RENDER_IMAGELIST_LAYOUT);
	}

	// 计算单张图片的大小
	if( NULL != m_hBitmap )
	{
 		if (m_nImageDirection == IMAGELIST_LAYOUT_TYPE_H)
 		{
			m_nItemWidth = UI_GetBitmapWidth(m_hBitmap)/4;
			m_nItemHeight = UI_GetBitmapHeight(m_hBitmap);
 		}
 		else
 		{
			m_nItemWidth = UI_GetBitmapWidth(m_hBitmap);
			m_nItemHeight = UI_GetBitmapHeight(m_hBitmap)/4;
 		}
	}
	return true;
}

//
//	获取背景图片的大小
//
SIZE ImageListRender::GetAutoSize( HRDC hRDC )
{
	SIZE s = {m_nItemWidth, m_nItemHeight};
	return s;
}

void ImageListRender::DrawDisable( HRDC hRDC )
{
 	if( NULL != m_hBitmap )
 	{
		if( m_nImageDirection == IMAGELIST_LAYOUT_TYPE_H )
		{
			DrawBitmap(hRDC, m_hBitmap, 0,0,m_nItemWidth,m_nItemHeight, 3*m_nItemWidth,0,m_nItemWidth,m_nItemHeight );
		}
		else
		{
			DrawBitmap(hRDC, m_hBitmap, 0,0,m_nItemWidth,m_nItemHeight, 0, 3*m_nItemHeight,m_nItemWidth,m_nItemHeight );
		}
 	}
}
void ImageListRender::DrawNormal( HRDC hRDC )
{
 	if( NULL != m_hBitmap )
 	{
		if( m_nImageDirection == IMAGELIST_LAYOUT_TYPE_H )
		{
			DrawBitmap(hRDC, m_hBitmap, 0,0,m_nItemWidth,m_nItemHeight, 0*m_nItemWidth,0,m_nItemWidth,m_nItemHeight );
		}
		else
		{
			DrawBitmap(hRDC, m_hBitmap, 0,0,m_nItemWidth,m_nItemHeight, 0, 0*m_nItemHeight,m_nItemWidth,m_nItemHeight );
		}
 	}
}
void ImageListRender::DrawHover( HRDC hRDC )
{
 	if( NULL != m_hBitmap )
 	{
		if( m_nImageDirection == IMAGELIST_LAYOUT_TYPE_H )
		{
			DrawBitmap(hRDC, m_hBitmap, 0,0,m_nItemWidth,m_nItemHeight, 1*m_nItemWidth,0,m_nItemWidth,m_nItemHeight );
		}
		else
		{
			DrawBitmap(hRDC, m_hBitmap, 0,0,m_nItemWidth,m_nItemHeight, 0, 1*m_nItemHeight,m_nItemWidth,m_nItemHeight );
		}
 	}
}
void ImageListRender::DrawPress( HRDC hRDC )
{
 	if( NULL != m_hBitmap )
 	{
		if( m_nImageDirection == IMAGELIST_LAYOUT_TYPE_H )
		{
			DrawBitmap(hRDC, m_hBitmap, 0,0,m_nItemWidth,m_nItemHeight, 2*m_nItemWidth,0,m_nItemWidth,m_nItemHeight );
		}
		else
		{
			DrawBitmap(hRDC, m_hBitmap, 0,0,m_nItemWidth,m_nItemHeight, 0, 2*m_nItemHeight,m_nItemWidth,m_nItemHeight );
		}
 	}
}

ButtonBkThemeRender::ButtonBkThemeRender( ButtonBase* pButton ) : BackRenderBase(pButton)
{
	m_hTheme = NULL;
	m_hTheme = ::OpenThemeData(NULL, L"Button");
}
ButtonBkThemeRender::~ButtonBkThemeRender( )
{
	if( m_hTheme )
	{
		::CloseThemeData( m_hTheme );
		m_hTheme = NULL;
	}
}

void ButtonBkThemeRender::OnThemeChanged()
{
	::CloseThemeData(m_hTheme);
	m_hTheme = ::OpenThemeData(NULL, L"Button");
}


void ButtonBkThemeRender::DrawDisable( HRDC hRDC )
{
	RECT rc;
	m_pObject->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, PBS_DISABLED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawFrameControl(hDC, &rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_INACTIVE );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonBkThemeRender::DrawNormal( HRDC hRDC )
{
	RECT rc;
	m_pObject->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, ((ButtonBase*)m_pObject)->IsDefault()?PBS_DEFAULTED:PBS_NORMAL, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawFrameControl(hDC, &rc, DFC_BUTTON, DFCS_BUTTONPUSH );
	}
	ReleaseHDC(hRDC, hDC);
}

void ButtonBkThemeRender::DrawHover( HRDC hRDC )
{
	RECT rc;
	m_pObject->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, PBS_HOT, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawFrameControl(hDC, &rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_HOT );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonBkThemeRender::DrawPress( HRDC hRDC )
{
	RECT rc;
	m_pObject->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, PBS_PRESSED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawFrameControl(hDC, &rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED );
	}
	ReleaseHDC(hRDC, hDC);
}


//////////////////////////////////////////////////////////////////////////


EditBkThemeRender::EditBkThemeRender( Edit* pEdit ) : BackRenderBase(pEdit)
{
	m_hTheme = NULL;
	m_hTheme = ::OpenThemeData(NULL, L"Edit");
}
EditBkThemeRender::~EditBkThemeRender( )
{
	if( m_hTheme )
	{
		::CloseThemeData( m_hTheme );
		m_hTheme = NULL;
	}
}

void EditBkThemeRender::OnThemeChanged()
{
	::CloseThemeData(m_hTheme);
	m_hTheme = ::OpenThemeData(NULL, L"Edit");
}


void EditBkThemeRender::DrawDisable( HRDC hRDC )
{
	RECT rc;
	m_pObject->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, EP_EDITBORDER_NOSCROLL, ETS_DISABLED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawDisable  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawEdge(hDC, &rc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
	}
	ReleaseHDC(hRDC, hDC);
}
void EditBkThemeRender::DrawNormal( HRDC hRDC )
{
	RECT rc;
	m_pObject->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, EP_EDITBORDER_NOSCROLL, ETS_CUEBANNER, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
		{
			DrawEdge(hDC, &rc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
		}
		else
		{
			::FillRect(hDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawEdge(hDC, &rc, EDGE_SUNKEN, BF_RECT);
		}
	}
	ReleaseHDC(hRDC, hDC);
}

void EditBkThemeRender::DrawHover( HRDC hRDC )
{
	RECT rc;
	m_pObject->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, EP_EDITBORDER_NOSCROLL, ETS_HOT, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawHover  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
		{
			DrawEdge(hDC, &rc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
		}
		else
		{
			::FillRect(hDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawEdge(hDC, &rc, EDGE_SUNKEN, BF_RECT);
		}
	}
	ReleaseHDC(hRDC, hDC);
}
void EditBkThemeRender::DrawPress( HRDC hRDC )
{
	RECT rc;
	m_pObject->GetClientRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, EP_EDITBORDER_NOSCROLL, ETS_SELECTED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawPress  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
		{
			DrawEdge(hDC, &rc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
		}
		else
		{
			::FillRect(hDC, &rc, (HBRUSH)GetStockObject(WHITE_BRUSH));
			DrawEdge(hDC, &rc, EDGE_SUNKEN, BF_RECT);
		}
	}
	ReleaseHDC(hRDC, hDC);
}



//////////////////////////////////////////////////////////////////////////


GroupBoxBkThemeRender::GroupBoxBkThemeRender( GroupBox* pGroupBox ) : BackRenderBase(pGroupBox)
{
	m_hTheme = NULL;
	m_hTheme = ::OpenThemeData(NULL, L"Button");
}
GroupBoxBkThemeRender::~GroupBoxBkThemeRender( )
{
	if( m_hTheme )
	{
		::CloseThemeData( m_hTheme );
		m_hTheme = NULL;
	}
}

void GroupBoxBkThemeRender::OnThemeChanged()
{
	::CloseThemeData(m_hTheme);
	m_hTheme = ::OpenThemeData(NULL, L"Button");
}


void GroupBoxBkThemeRender::DrawDisable( HRDC hRDC )
{
	RECT rc;
	((GroupBox*)m_pObject)->GetBorderRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_GROUPBOX, GBS_DISABLED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("GroupBoxBkThemeRender::DrawDisable  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawEdge(hDC, &rc, EDGE_ETCHED, BF_RECT);
	}
	ReleaseHDC(hRDC, hDC);
}
void GroupBoxBkThemeRender::DrawNormal( HRDC hRDC )
{
	RECT rc;
	((GroupBox*)m_pObject)->GetBorderRect(&rc);

	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_GROUPBOX, GBS_NORMAL, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("GroupBoxBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawEdge(hDC, &rc, EDGE_ETCHED, BF_RECT);
	}
	ReleaseHDC(hRDC, hDC);
}

void GroupBoxBkThemeRender::DrawHover( HRDC hRDC )
{
}
void GroupBoxBkThemeRender::DrawPress( HRDC hRDC )
{
}
#endif

//////////////////////////////////////////////////////////////////////////
//
//

ButtonRender::ButtonRender( )
{
	m_pBkRender = NULL;
	m_pIconTextRender = NULL;
	m_nAutoSizeType  = BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT;
}

ButtonRender::~ButtonRender( )
{
	if( NULL != m_pBkRender )
	{
		delete m_pBkRender;
		m_pBkRender = NULL;
	}
	if( NULL != m_pIconTextRender )
	{
		delete m_pIconTextRender;
		m_pIconTextRender = NULL;
	}
}

void ButtonRender::ResetAttribute()
{
	SAFE_DELETE(m_pIconTextRender);
	SAFE_DELETE(m_pBkRender);

	m_nAutoSizeType = BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT;

	if ( NULL != m_pBkRender )
	{
		m_pBkRender->ResetAttribute();
	}
	if( NULL != m_pIconTextRender )
	{
		m_pIconTextRender->ResetAttribute();
	}

}
bool ButtonRender::SetAttribute( map<String,String>& mapAttrib, bool bReload )
{
	if( 0 != mapAttrib.count(XML_RENDER_TYPE) )
	{
		String strType = mapAttrib[XML_RENDER_TYPE];
		m_pBkRender =  BackRenderBase::CreateInstance( strType, m_pButton );		
		this->m_pButton->EraseAttribute(XML_RENDER_TYPE);

		if ( NULL != m_pBkRender )
		{
			m_pBkRender->SetAttribute(mapAttrib,bReload);
		}
	}

	// 创建默认的icontextrender
	if( NULL == m_pIconTextRender )
	{
		m_pIconTextRender = new ButtonIconTextRender(m_pButton);
	}
// 	if( NULL == m_pBkRender && m_pButton->GetButtonStyle() == BUTTON_STYLE_PUSHBUTTON ) 
// 	{
// 		m_pBkRender = new ButtonBkThemeRender(m_pButton);
// 	}

	if( 0 != mapAttrib.count(XML_BUTTON_RENDER_AUTOSIZE_TYPE) )
	{
		String strType = mapAttrib[XML_BUTTON_RENDER_AUTOSIZE_TYPE];
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
		this->m_pButton->EraseAttribute(XML_BUTTON_RENDER_AUTOSIZE_TYPE);
	}

	if( NULL != m_pIconTextRender )
	{
		m_pIconTextRender->SetAttribute(mapAttrib,bReload);
	}

	return true;
}
SIZE ButtonRender::GetAutoSize(HRDC hRDC)
{
	SIZE s = {0,0};

	// 按钮的auto size可以按背景图标大小来算，也可以按内容的大小来算
	switch( m_nAutoSizeType )
	{
	case BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE:
		{
			if( NULL != m_pBkRender )
			{
				s = m_pBkRender->GetAutoSize( hRDC );
			}
		}
		break;

	case BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT:
		{
			s.cx = this->m_pButton->GetPaddingW();
			s.cy = this->m_pButton->GetPaddingH();

			if ( this->m_pIconTextRender != NULL )
			{
				SIZE sIconText = this->m_pIconTextRender->GetAutoSize( hRDC );
				s.cx += sIconText.cx;
				s.cy += sIconText.cy;
			}
		}
		break;

	case BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH:
		{
			SIZE sHeight = m_pBkRender->GetAutoSize( hRDC );

			SIZE sWiddth;
			sWiddth.cx = this->m_pButton->GetPaddingW();
			sWiddth.cy = this->m_pButton->GetPaddingH();

			if ( this->m_pIconTextRender != NULL )
			{
				SIZE sIconText = this->m_pIconTextRender->GetAutoSize( hRDC );
				sWiddth.cx += sIconText.cx;
				sWiddth.cy += sIconText.cy;
			}

			s.cx = sWiddth.cx;
			s.cy = sHeight.cy;
		}

	default:
		UI_LOG_WARN( _T("ButtonRender::GetAutoSize  Unknown autosize type: %d"), m_nAutoSizeType );
		break;
	}
	
	return s;
}

void ButtonRender::Draw(HRDC hRDC)
{
	if( NULL == m_pButton )
		return;

	bool  bDisable = !m_pButton->IsEnable();
	bool  bHover   = m_pButton->IsHover();
	bool  bPress   = m_pButton->IsPress();

	if( bDisable )
	{
		this->DrawDisable( hRDC );
		return;
	}

	if( bPress && bHover )
	{
		this->DrawPress( hRDC );
	}
	else if( bHover || bPress )
	{
		this->DrawHover( hRDC );
	}
	else 
	{
		this->DrawNormal( hRDC );
	}
	
}
void ButtonRender::DrawDisable( HRDC hRDC  )
{	
	if( NULL != m_pBkRender )
		m_pBkRender->DrawDisable( hRDC );

	if( NULL != m_pIconTextRender )
		m_pIconTextRender->DrawDisable(hRDC);
}
void ButtonRender::DrawNormal( HRDC hRDC )
{
	if( NULL != m_pBkRender )
		m_pBkRender->DrawNormal( hRDC );

	if( NULL != m_pIconTextRender )
		m_pIconTextRender->DrawNormal( hRDC );
}
void ButtonRender::DrawHover( HRDC hRDC )
{
	if( NULL != m_pBkRender )
		m_pBkRender->DrawHover( hRDC );

	if( NULL != m_pIconTextRender )
		m_pIconTextRender->DrawHover( hRDC );
}
void ButtonRender::DrawPress( HRDC hRDC )
{
	if( NULL != m_pBkRender )
		m_pBkRender->DrawPress( hRDC );

	if( NULL != m_pIconTextRender )
		m_pIconTextRender->DrawPress( hRDC );
}

void ButtonRender::SetTextRenderType( BUTTON_TEXT_RENDER_TYPE eType )
{
	if( NULL == m_pIconTextRender )
		m_pIconTextRender = new ButtonIconTextRender(m_pButton);

	m_pIconTextRender->SetTextRenderType(eType);
}

_ButtonTextRender* ButtonRender::GetTextRender()
{
	if( NULL == m_pIconTextRender )
		return NULL;

	return m_pIconTextRender->m_pTextRender;
}
_ButtonIconRender* ButtonRender::GetIconRender()
{
	if( NULL == m_pIconTextRender )
		return NULL;

	return m_pIconTextRender->m_pIconRender;
}
ButtonIconTextRender* ButtonRender::GetIconTextRender()
{
	return m_pIconTextRender;
}