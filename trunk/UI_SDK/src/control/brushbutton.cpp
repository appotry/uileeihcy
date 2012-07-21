#include "stdafx.h"
#define  BRUSHBUTTON_ICONTEXT_SPAN   4   // 按钮图标于文字之间的距离

BrushButton::BrushButton()
{
	pImgIcon = NULL;

	::SetRect( &__super::rcPadding, 4,2,2,4 );
}

BrushButton::~BrushButton()
{
	if( NULL != pImgIcon )
	{
		pImgIcon->Release();
		pImgIcon = NULL;
	}
}

bool BrushButton::SetAttribute( map<String,String>& mapAttrib )
{
	bool bRet = Object::SetAttribute( mapAttrib );
	if( false == bRet )	return bRet;

	if( mapAttrib.count( XML_BRUSHBUTTON_TEXT ) )
	{
		this->text = mapAttrib[XML_BRUSHBUTTON_TEXT];
		__super::attribute.erase( XML_BRUSHBUTTON_TEXT );
	}
	if( mapAttrib.count( XML_BRUSHBUTTON_ICON ) )
	{
		UI_GetImage( mapAttrib[XML_BRUSHBUTTON_ICON], &this->pImgIcon );
		__super::attribute.erase(XML_BRUSHBUTTON_ICON );
	}
	
	if( mapAttrib.count( XML_BRUSHBUTTON_BKHOVERCOL ))
	{
		String colorName = mapAttrib[XML_BRUSHBUTTON_BKHOVERCOL];
		this->bkBrushHoverCol = UI_GetColor( colorName );
		__super::attribute.erase(XML_BRUSHBUTTON_BKHOVERCOL);
	}
	if( mapAttrib.count( XML_BRUSHBUTTON_BKPRESSCOL ))
	{
		String colorName = mapAttrib[XML_BRUSHBUTTON_BKPRESSCOL];
		this->bkBrushPressCol = UI_GetColor( colorName );
		__super::attribute.erase(XML_BRUSHBUTTON_BKPRESSCOL);
	}
	if( mapAttrib.count( XML_BRUSHBUTTON_BORDERHOVERCOL ))
	{
		String colorName = mapAttrib[XML_BRUSHBUTTON_BORDERHOVERCOL];
		this->borderPenHoverCol = UI_GetColor( colorName );
		__super::attribute.erase(XML_BRUSHBUTTON_BORDERHOVERCOL);
	}

	if( mapAttrib.count( XML_BRUSHBUTTON_BORDERPRESSCOL ))
	{
		String colorName = mapAttrib[XML_BRUSHBUTTON_BORDERPRESSCOL];
		this->borderPenPressCol = UI_GetColor( colorName );
		__super::attribute.erase(XML_BRUSHBUTTON_BORDERPRESSCOL);
	}
	if( mapAttrib.count( XML_BRUSHBUTTON_TEXTNORMALCOL ))
	{
		String colorName = mapAttrib[XML_BRUSHBUTTON_TEXTNORMALCOL];
		this->textNormalCol = UI_GetColor( colorName );
		__super::attribute.erase(XML_BRUSHBUTTON_TEXTNORMALCOL);
	}
	if( mapAttrib.count( XML_BRUSHBUTTON_TEXTHOVERCOL ))
	{
		String colorName = mapAttrib[XML_BRUSHBUTTON_TEXTHOVERCOL];
		this->textHoverCol = UI_GetColor( colorName );
		__super::attribute.erase(XML_BRUSHBUTTON_TEXTHOVERCOL);
	}
	if( mapAttrib.count( XML_BRUSHBUTTON_TEXTPRESSCOL ))
	{
		String colorName = mapAttrib[XML_BRUSHBUTTON_TEXTPRESSCOL];
		this->textPressCol = UI_GetColor( colorName );
		__super::attribute.erase(XML_BRUSHBUTTON_TEXTPRESSCOL);
	}
	if( mapAttrib.count( XML_BRUSHBUTTON_TEXTDISABLECOL ))
	{
		String colorName = mapAttrib[XML_BRUSHBUTTON_TEXTDISABLECOL];
		this->textDisableCol = UI_GetColor( colorName );
		__super::attribute.erase(XML_BRUSHBUTTON_TEXTDISABLECOL);
	}

	return bRet;
}

void BrushButton::DrawDisable( HDC hDC )
{

}

void BrushButton::DrawNormal( HDC hDC ) 
{
	COLORREF oldCol  = ::SetTextColor( hDC, this->textNormalCol );

	int nImgWidth = 0;
	int nImgHeight= 0;
	if( NULL != pImgIcon )
	{
		nImgWidth = pImgIcon->GetWidth();
		nImgHeight= pImgIcon->GetHeight();
		pImgIcon->Draw( hDC, rcPadding.left, (Util::RectH(&rectW)+ rcPadding.top - rcPadding.bottom - nImgHeight )/2 );
	}

	if( this->text != _T("") )
	{
		RECT  rcText;
		rcText.left = rcPadding.left + nImgWidth + BRUSHBUTTON_ICONTEXT_SPAN;
		rcText.top = rcPadding.top;
		rcText.right = Util::RectW( &this->rectP)- rcPadding.right;
		rcText.bottom = Util::RectH( &this->rectP ) - rcPadding.bottom;

		::DrawText( hDC, this->text.c_str(), (int)this->text.length(), &rcText, DT_SINGLELINE|DT_VCENTER );
	}

	::SetTextColor( hDC, oldCol );
}

void BrushButton::DrawHover( HDC hDC ) 
{
	HPEN     oldPen  = NULL;
	HBRUSH   oldBrush= NULL;

	HPEN     hPen   = ::CreatePen( PS_SOLID,1,this->borderPenHoverCol );
	HBRUSH   hBrush = ::CreateSolidBrush( this->bkBrushHoverCol );

	COLORREF oldCol  = ::SetTextColor( hDC, this->textHoverCol );

	oldPen   = (HPEN)::SelectObject( hDC, hPen );
	oldBrush = (HBRUSH)::SelectObject( hDC, hBrush );
	
	::Rectangle( hDC, 0,0,Util::RectW( &this->rectP),Util::RectH( &this->rectP ));

	int nImgWidth = 0;
	int nImgHeight= 0;
	if( NULL != pImgIcon )
	{
		nImgWidth = pImgIcon->GetWidth();
		nImgHeight= pImgIcon->GetHeight();
		pImgIcon->Draw( hDC, rcPadding.left, (Util::RectH(&rectW)+ rcPadding.top - rcPadding.bottom - nImgHeight )/2 );
	}

	if( this->text != _T("") )
	{
		RECT  rcText;
		rcText.left = rcPadding.left + nImgWidth + BRUSHBUTTON_ICONTEXT_SPAN;
		rcText.top = rcPadding.top;
		rcText.right = Util::RectW( &this->rectP)- rcPadding.right;
		rcText.bottom = Util::RectH( &this->rectP ) - rcPadding.bottom;

		::DrawText( hDC, this->text.c_str(), (int)this->text.length(), &rcText, DT_SINGLELINE|DT_VCENTER );
	}

	::SetTextColor( hDC, oldCol );
	::SelectObject( hDC, oldPen );
	::SelectObject( hDC, oldBrush );
	::DeleteObject( hPen );
	::DeleteObject( hBrush );
}
void BrushButton::DrawPress( HDC hDC ) 
{
	HPEN     oldPen  = NULL;
	HBRUSH   oldBrush= NULL;
	COLORREF oldCol;

	HPEN     hPen   = ::CreatePen( PS_SOLID,1,this->borderPenPressCol );
	HBRUSH   hBrush = ::CreateSolidBrush( this->bkBrushPressCol );

	oldCol   = ::SetTextColor( hDC, this->textPressCol );
	oldPen   = (HPEN)::SelectObject( hDC, hPen );
	oldBrush = (HBRUSH)::SelectObject( hDC, hBrush );

	::Rectangle( hDC, 0,0,Util::RectW( &this->rectP),Util::RectH( &this->rectP ));

	int nImgWidth = 0;
	int nImgHeight= 0;
	if( NULL != pImgIcon )
	{
		nImgWidth = pImgIcon->GetWidth();
		nImgHeight= pImgIcon->GetHeight();
		pImgIcon->Draw( hDC, rcPadding.left, (Util::RectH(&rectW)+ rcPadding.top - rcPadding.bottom - nImgHeight )/2 );
	}

	if( this->text != _T("") )
	{
		RECT  rcText;
		rcText.left = rcPadding.left + nImgWidth + BRUSHBUTTON_ICONTEXT_SPAN;
		rcText.top = rcPadding.top;
		rcText.right = Util::RectW( &this->rectP)- rcPadding.right;
		rcText.bottom = Util::RectH( &this->rectP ) - rcPadding.bottom;

		::DrawText( hDC, this->text.c_str(), (int)this->text.length(), &rcText, DT_SINGLELINE|DT_VCENTER );
	}

	::SetTextColor( hDC, oldCol );
	::SelectObject( hDC, oldPen );
	::SelectObject( hDC, oldBrush );
	::DeleteObject( hPen );
	::DeleteObject( hBrush );
}
void BrushButton::DrawFocus( HDC hDC ) 
{
	
}


SIZE BrushButton::GetAutoSize( HDC hDC )
{
	SIZE  size;

	::GetTextExtentPoint32( hDC, this->text.c_str(), this->text.length(), &size );
	size.cx += BRUSHBUTTON_ICONTEXT_SPAN + pImgIcon->GetWidth() 
		+ rcPadding.left + rcPadding.right
		+ rcMargin.left  + rcMargin.right;

	if( size.cy < pImgIcon->GetHeight() )
		size.cy = pImgIcon->GetHeight();
	size.cy += rcPadding.top + rcPadding.bottom
		+ rcMargin.top + rcMargin.bottom;

	return size;
}