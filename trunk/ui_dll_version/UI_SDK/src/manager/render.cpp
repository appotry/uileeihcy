#include "stdafx.h"


RenderBase::RenderBase()
{
	m_pObject = NULL;
	m_nRenderType = RENDER_TYPE_NULL;
}


//
//	从工厂方法中创建出一个render
//
//	parameter
//		strType
//			[in]	render类型 
//		
//		pObj
//			[in]	窗口指针
//
//	return
//		失败返回NULL，失败时可参见日志
//
//
RenderBase* RenderFactory::GetRender( const String& strType, Object* pObj )
{
	RENDER_TYPE eType = RENDER_TYPE_THEME;

	if (XML_RENDER_TYPE_THEME == strType)
		eType = RENDER_TYPE_THEME;

	else if (XML_RENDER_TYPE_IMAGE == strType)
		eType = RENDER_TYPE_IMAGE;

	else if (XML_RENDER_TYPE_IMAGELIST == strType)
		eType = RENDER_TYPE_IMAGELIST;

	else if (XML_RENDER_TYPE_COLOR == strType)
		eType = RENDER_TYPE_COLOR;

	else if (XML_RENDER_TYPE_IMAGELISTITEM == strType)
		eType = RENDER_TYPE_IMAGELISTITEM;

	else if (XML_RENDER_TYPE_COLORLIST == strType)
		eType = RENDER_TYPE_COLORLIST;

	else if (XML_RENDER_TYPE_GRADIENT_H == strType)
		eType = RENDER_TYPE_GRADIENTH;

	else if (XML_RENDER_TYPE_GRADIENT_V == strType)
		eType = RENDER_TYPE_GRADIENTV;

	else if (XML_RENDER_TYPE_NOTHEME == strType)
		eType = RENDER_TYPE_NOTHEME;

	else if (XML_RENDER_TYPE_NULL == strType)
		eType = RENDER_TYPE_NULL;

	else
		return NULL;

	return GetRender(eType, pObj);
}

RenderBase* RenderFactory::GetRender( RENDER_TYPE eType, Object* pObj )
{
	RenderBase*  pRender = NULL;

	if (RENDER_TYPE_THEME == eType)
	{
		TCHAR* szObjName = pObj->GetObjectName();
		if (0 == _tcscmp(szObjName, _T("Button")))
		{
			Button* pButton = dynamic_cast<Button*>(pObj);
			if (NULL == pButton)
				return NULL;

			int nButtonStyle = pButton->GetButtonStyle();
			if (BUTTON_STYLE_COMBOBOX == nButtonStyle)
			{
				pRender = new ComboboxButtonBkThemeRender();
			}
			else if (BUTTON_STYLE_SCROLLLINEUP == nButtonStyle)
			{
				pRender = new ScrollLineUpButtonBkThemeRender();
			}
			else if (BUTTON_STYLE_SCROLLLINEDOWN == nButtonStyle)
			{
				pRender = new ScrollLineDownButtonBkThemeRender();
			}
			else if (BUTTON_STYLE_SCROLLLINELEFT == nButtonStyle)
			{
				pRender = new ScrollLineLeftButtonBkThemeRender();
			}
			else if (BUTTON_STYLE_SCROLLLINERIGHT == nButtonStyle)
			{
				pRender = new ScrollLineRightButtonBkThemeRender();
			}
			else if (BUTTON_STYLE_VSCROLLTHUMB == nButtonStyle)
			{
				pRender = new VScrollThumbButtonThemeRender();
			}
			else if (BUTTON_STYLE_HSCROLLTHUMB == nButtonStyle)
			{
				pRender = new HScrollThumbButtonThemeRender();
			}
			else if (BUTOTN_STYLE_SLIDERTRACKBUTTON == nButtonStyle)
			{
				pRender = new SliderTrackButtonThemeRender();
			}
			else
			{
				pRender = new ButtonBkThemeRender();
			}
		}
		else if (0 == _tcscmp(szObjName, _T("RadioButton")))
		{
			pRender = new RadioButtonThemeRender();
		}
		else if (0 == _tcscmp(szObjName, _T("CheckButton")))
		{
			pRender = new CheckButtonThemeRender();
		}
		else if (0 == _tcscmp(szObjName, _T("Edit")))
		{
			pRender = new EditBkThemeRender();
		}
		else if (0 == _tcscmp(szObjName, _T("GroupBox")))
		{
			pRender = new GroupBoxBkThemeRender();
		}
		else if (0 == _tcscmp(szObjName, _T("Combobox")))
		{
			pRender = new ComboboxBkThemeRender();
		}
		else if (0 == _tcscmp(szObjName, _T("ListBox")))
		{
			ListBox* pListBox = dynamic_cast<ListBox*>(pObj);
			if (NULL != pListBox)
			{
				if (LISTBOX_STYLE_COMBOBOX == pListBox->GetListBoxStyle())
				{
					ColorRender* pTemp = new ColorRender();
					pTemp->SetBkColor(RGB(255,255,255));
					pTemp->SetBorderColor(RGB(0,0,0));
					pRender = static_cast<RenderBase*>(pTemp);

					int nBorder = 1;
					CRegion4 r(nBorder, nBorder, nBorder, nBorder);
					pObj->SetBorderRegion(&r);
				}
				else
				{
					pRender = new ListboxBkThemeRender();
				}
			}
		}
		else if (0 == _tcscmp(szObjName, _T("Menu")))
		{
			pRender = new MenuBkThemeRender();
			CRegion4 r(2,2,2,2);
			pObj->SetBorderRegion(&r);
		}
		else if (0 == _tcscmp(szObjName, _T("ThemeTooltip")))
		{
			pRender = new TooltipBkgndThemeRender;
		}
	}
	else if( RENDER_TYPE_IMAGE == eType )
	{
		pRender = new ImageRender();
	}
	else if( RENDER_TYPE_COLORLIST == eType )
	{
		pRender = new ColorListRender();
	}
	else if( RENDER_TYPE_IMAGELIST == eType )
	{
		pRender = new ImageListRender();
	}
	else if (RENDER_TYPE_IMAGELISTITEM == eType)
	{
		pRender = new ImageListItemRender();
	}
	else if (RENDER_TYPE_NOTHEME == eType)
	{
		if( _T("GroupBox") == pObj->GetObjectName() )
		{
			pRender = new GroupBoxBkNoThemeRender();
		}
	}
	else if( RENDER_TYPE_COLOR == eType )
	{
		pRender = new ColorRender();
	}

	else if( RENDER_TYPE_GRADIENTH == eType )
	{
		pRender = new GradientRender();
	}

	else if( RENDER_TYPE_GRADIENTV == eType )
	{
		pRender = new GradientRender();
	}
	else if(RENDER_TYPE_THEME_MENUSTRINGITEM == eType)
	{
		pRender = new MenuStringItemRender();

		// 注：由于不同的主题下面，字体render也不一样。为了解决在使用主题样式下面字体颜色的问题，
		//     当菜单使用了主题前景时，自动修改它的字体颜色样式
		TextRenderBase* pTextRender = TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_COLORLIST, pObj);
		if( NULL != pTextRender )
		{
			ColorListTextRender* p = dynamic_cast<ColorListTextRender*>(pTextRender);

			HRFONT hRFont = pObj->GetFont();
			p->SetHRFont(hRFont);
			p->SetCount(4);
			p->SetColor(0, RGB(0,0,0));
			p->SetColor(1, RGB(255,255,255));
			p->SetColor(2, RGB(0,0,0));
			p->SetColor(3, RGB(192,192,192));

			OSVERSIONINFOEX os;
			::ZeroMemory(&os,sizeof(os));
			os.dwOSVersionInfoSize=sizeof(os);
			::GetVersionEx(reinterpret_cast<LPOSVERSIONINFO>(&os));

			// 在WIN7开户主题的情况下，将菜单项的hover颜色改成黑色
			if(os.dwPlatformId==VER_PLATFORM_WIN32_NT && os.dwMajorVersion > 5)  // visita or higher
			{
				HTHEME h = OpenThemeData(NULL, _T("MENU"));
				if (NULL != h)
				{
					p->SetColor(1, RGB(0,0,0));
					CloseThemeData(h);
				}
			}
			pObj->SetTextRender(p);
		}
		
	}
	else if(RENDER_TYPE_THEME_MENUSEPERATOR == eType)
	{
		pRender = new MenuSeperatorThemeRender();
	}
	else if (RENDER_TYPE_THEME_MENUPOPUPTRIANGLE == eType)
	{
		pRender = new MenuPopupTriangleRender();
	}
	else if (RENDER_TYPE_THEME_MENUCHECKICON == eType)
	{
		pRender = new MenuCheckedIconThemeRender();
	}
	else if (RENDER_TYPE_THEME_MENURADIOICON == eType)
	{
		pRender = new MenuRadioIconThemeRender();
	}
	else if (RENDER_TYPE_THEME_MENURADIOCHECKICONBK == eType)
	{
		pRender = new MenuRadioCheckIconBkThemeRender();
	}
	else if (RENDER_TYPE_THEME_VSCROLLBARBACKGND == eType)
	{
		pRender = new VScrollBarBkgndThemeRender();
	}
	else if (RENDER_TYPE_THEME_HSCROLLBARBACKGND == eType)
	{
		pRender = new HScrollBarBkgndThemeRender();
	}
	else if (RENDER_TYPE_THEME_SCROLLBAR_SIZEBOX == eType)
	{
		pRender = new ScrollBarSizeBoxThemeRender();
	}
	else if (RENDER_TYPE_THEME_PROGRESS_FOREGND == eType)
	{
		pRender = new ProgressCtrlForegndThemeRender();
	}
	else if (RENDER_TYPE_THEME_PROGRESS_BKGND == eType)
	{
		pRender = new ProgressCtrlBkgndThemeRender();
	}
	else if (RENDER_TYPE_THEME_TRACKBAR_BKGND == eType)
	{
		pRender = new SliderCtrlBkgndThemeRender();
	}
	else if (RENDER_TYPE_NULL == eType)
	{
		pRender = new NullRenderBase();
	}
	else
	{
		UI_LOG_WARN(_T("%s invalid render type %d"), FUNC_NAME,  eType );
	}

	if( NULL != pRender )
	{
		pRender->SetRenderType(eType);
		pRender->SetObject(pObj);
		pRender->Init();
	}

	return pRender;
}


//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               ColorRender                                  //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

ColorRender::ColorRender()
{
	m_pBkColor = NULL;
	m_pBorderColor = NULL;
}
ColorRender::~ColorRender()
{
	SAFE_RELEASE(m_pBkColor);
	SAFE_RELEASE(m_pBorderColor);
}

void ColorRender::SetBkColor( COLORREF col )
{
	SAFE_RELEASE(m_pBkColor);
	UIColor::CreateInstance(col, &m_pBkColor);
	m_pBkColor->AddRef();
}
void ColorRender::SetBorderColor( COLORREF col )
{
	SAFE_RELEASE(m_pBorderColor);
	UIColor::CreateInstance(col, &m_pBorderColor);
	m_pBorderColor->AddRef();
}

bool ColorRender::SetAttribute( const String& strPrefix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrefix + XML_RENDER_COLOR;
	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		const String& strColID = iter->second;
		::UI_GetColor( strColID, &m_pBkColor );
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_RENDER_BORDERCOLOR;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		const String& strColID = iter->second;
		::UI_GetColor( strColID, &m_pBorderColor );
		mapAttrib.erase(iter);
	}

	return true;
} 

void ColorRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	if( NULL != m_pBkColor )
	{
		FillRect(pRenderTarget, prc, m_pBkColor->GetColor());
	}
	
	// 绘制边框，由于直接调用Rectangle创建指定宽度的PEN后进行绘制，会导致PEN的
	// 一半区域位于控件剪裁区域外面。因此在这里用FillRect来实现边框的绘制。
	// 同时也支持了当各个边框大小不一致时的绘制
	if (NULL == m_pBorderColor)
		return;
	
	CRegion4 rBorder;
	m_pObject->GetBorderRegion(&rBorder);
	if (0 != rBorder.left)
	{
		CRect rcLeft = *prc;
		rcLeft.right = rBorder.left;
		FillRect(pRenderTarget, &rcLeft, m_pBorderColor->GetColor());
	}
	if (0 != rBorder.top)
	{
		CRect rcTop = *prc;
		rcTop.bottom = rBorder.top;
		FillRect(pRenderTarget, &rcTop, m_pBorderColor->GetColor());
	}
	if (0 != rBorder.right)
	{
		CRect rcRight = *prc;
		rcRight.left = rcRight.right-rBorder.right;
		FillRect(pRenderTarget, &rcRight, m_pBorderColor->GetColor());
	}
	if (0 != rBorder.bottom)
	{
		CRect rcBottom = *prc;
		rcBottom.top = rcBottom.bottom - rBorder.bottom;
		FillRect(pRenderTarget, &rcBottom, m_pBorderColor->GetColor());
	}

}



//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               GradientRender                                  //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


GradientRender::GradientRender()
{
	m_pColorFrom = NULL;
	m_pColorTo = NULL;
	m_pBorderColor = NULL;
}
GradientRender::~GradientRender()
{
	SAFE_RELEASE(m_pColorFrom);
	SAFE_RELEASE(m_pColorTo);
	SAFE_RELEASE(m_pBorderColor);
}

bool GradientRender::SetAttribute( const String& strPrefix, ATTRMAP& mapAttrib )
{
	String strAttrib = strPrefix + XML_RENDER_COLORFROM;

	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		const String& strColID = iter->second;
		::UI_GetColor( strColID, &m_pColorFrom );
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_RENDER_COLORTO;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		const String& strColID = iter->second;
		::UI_GetColor( strColID, &m_pColorTo );
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_RENDER_BORDERCOLOR;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		const String& strColID = iter->second;
		::UI_GetColor( strColID, &m_pBorderColor );
		mapAttrib.erase(iter);
	}
	return true;
} 

void GradientRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	if( NULL == m_pColorFrom || NULL == m_pColorTo)
		return;

	CRect rc(0,0, this->m_pObject->GetWidth(), this->m_pObject->GetHeight());
	if( m_nRenderType == RENDER_TYPE_GRADIENTH )
	{
		GradientFillH( pRenderTarget, &rc, m_pColorFrom->GetColor(), m_pColorTo->GetColor() );
	}
	else if( m_nRenderType == RENDER_TYPE_GRADIENTV )
	{
		GradientFillV( pRenderTarget, &rc, m_pColorFrom->GetColor(), m_pColorTo->GetColor() );
	}

	if( NULL != m_pBorderColor )
	{
		Rectangle( pRenderTarget, prc, m_pBorderColor->GetColor(), 0,1,true );
	}

}


 
//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               SimpleImageRender                            //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


ImageRender::ImageRender()
{
	m_pBitmap = NULL;
	m_pColorBk = NULL;
	m_nImageDrawType = DRAW_BITMAP_BITBLT;
	m_pRegion = NULL;
}
ImageRender::~ImageRender()
{
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pColorBk);
	SAFE_DELETE(m_pRegion);
}

bool ImageRender::SetAttribute( const String& strPrefix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrefix + XML_RENDER_IMAGE;
	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		const String& strImageID = iter->second;
		m_pBitmap = ::UI_GetBitmap( strImageID, ::GetGraphicsRenderType(m_pObject) );
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_RENDER_COLOR;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		const String& strColID = iter->second;
		::UI_GetColor( strColID, &m_pColorBk );
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_RENDER_IMAGE9REGION;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		SAFE_DELETE(m_pRegion);
		m_pRegion = new Image9Region;

		const String& str9Region = iter->second;
		Util::TranslateImage9Region( str9Region, (void*)m_pRegion );
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_RENDER_IMAGE_DRAWTYPE;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		const String& strType = iter->second;
		if (XML_RENDER_IMAGE_DRAWTYPE_SIMPLE == strType)
			this->SetImageDrawType(DRAW_BITMAP_BITBLT);
		else if (XML_RENDER_IMAGE_DRAWTYPE_TILE == strType)
			this->SetImageDrawType(DRAW_BITMAP_TILE);
		else if (XML_RENDER_IMAGE_DRAWTYPE_STRETCH == strType)
			this->SetImageDrawType(DRAW_BITMAP_STRETCH);
		else if (XML_RENDER_IMAGE_DRAWTYPE_ADAPT == strType)
			this->SetImageDrawType(DRAW_BITMAP_ADAPT);
		else if (XML_RENDER_IMAGE_DRAWTYPE_CENTER == strType)
			this->SetImageDrawType(DRAW_BITMAP_CENTER);

		mapAttrib.erase(iter);
	}
	return true;
} 

void ImageRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	if (NULL != m_pColorBk)
	{
		pRenderTarget->FillRect(prc, m_pColorBk->GetColor() );
	}

	POINT ptSrc = this->GetBitmapSrcDrawPos();
	SIZE  sizeSrc = this->GetDesiredSize();

	if (NULL != m_pBitmap)
	{
		DRAWBITMAPPARAM param;
		param.nFlag = m_nImageDrawType;
		param.xDest = prc->left;
		param.yDest = prc->top;
		param.wDest = prc->Width();
		param.hDest = prc->Height();
		param.xSrc = ptSrc.x;
		param.ySrc = ptSrc.y;
		param.wSrc = sizeSrc.cx;
		param.hSrc = sizeSrc.cy;
		param.pRegion = m_pRegion;

		if (nState & RENDER_STATE_DISABLE)
		{
			param.nFlag |= DRAW_BITMAP_DISABLE;
		}
		pRenderTarget->DrawBitmap(m_pBitmap, &param);

// 		switch (m_eImageDrawType)
// 		{
// 		case IMAGE_DRAW_TYPE_SIMPLE:
// 			pRDC->DrawBitmap(m_pBitmap,prc->left,prc->top, sizeSrc.cx,sizeSrc.cy, ptSrc.x, ptSrc.y);
// 			break;
// 
// 		case IMAGE_DRAW_TYPE_STRETCH:
// 			pRDC->DrawBitmap(m_pBitmap, prc->left,prc->top,prc->Width(), prc->Height(), 
// 						ptSrc.x, ptSrc.y, sizeSrc.cx,sizeSrc.cy, m_pRegion);
// 			break;
// 
// 		case IMAGE_DRAW_TYPE_TILE:
// 			pRDC->TileRect(prc, m_pBitmap);
// 			break;
// 
// 		case IMAGE_DRAW_TYPE_CENTER:
// 			{
// 				int x = prc->left + (prc->Width() - sizeSrc.cx)/2;
// 				int y = prc->top  + (prc->Height() - sizeSrc.cy)/2;
// 
// 				pRDC->DrawBitmap(m_pBitmap, x, y, sizeSrc.cx,sizeSrc.cy, ptSrc.x, ptSrc.y);
// 			}
// 			break;
// 
// 		case IMAGE_DRAW_TYPE_ADAPT:
// 			{
// 				if (sizeSrc.cy == 0 || sizeSrc.cy == 0)
// 					break;
// 
// 				if (prc->Width() == 0 || prc->Height() == 0)
// 					break;
// 
// 				double tan_x_y_image  = 0;
// 				double tan_x_y_window = 0;
// 
// 				bool bNeedToStretch = false;
// 				int  wImage = sizeSrc.cx;
// 				int  hImage = sizeSrc.cy;
// 
// 				if (prc->Width() < sizeSrc.cx || prc->Height() < sizeSrc.cy)
// 				{
// 					bNeedToStretch = true;
// 
// 					tan_x_y_image = (double)sizeSrc.cx / (double)sizeSrc.cy;
// 					tan_x_y_window = (double)prc->Width() / (double)prc->Height();
// 
// 					if( tan_x_y_image > tan_x_y_window ) // 横向占满
// 					{
// 						wImage = prc->Width();
// 						hImage = (int)((double)wImage/tan_x_y_image);
// 					}
// 					else   // 纵向占满
// 					{
// 						hImage = prc->Height();
// 						wImage = (int)(hImage*tan_x_y_image);
// 					}
// 				}
// 
// 				// 计算图片显示位置
// 				int xDisplayPos = prc->left + (prc->Width()-wImage)/2;
// 				int yDisplayPos = prc->top + (prc->Height()-hImage)/2;
// 
// 				if( bNeedToStretch )
// 				{
// 					pRDC->DrawBitmap(m_pBitmap, xDisplayPos, yDisplayPos, wImage, hImage, ptSrc.x, ptSrc.y, sizeSrc.cx,sizeSrc.cy );
// 				}
// 				else
// 				{
// 					pRDC->DrawBitmap(m_pBitmap, xDisplayPos, yDisplayPos, sizeSrc.cx,sizeSrc.cy, ptSrc.x, ptSrc.y);
// 				}
// 			}
// 			break;
// 		}
	}
}
SIZE ImageRender::GetDesiredSize()
{
	SIZE s = {0,0};
	if( NULL == m_pBitmap )
		return s;

	s.cx = m_pBitmap->GetWidth();
	s.cy = m_pBitmap->GetHeight();
	return s;
}

//////////////////////////////////////////////////////////////////////////

ImageListItemRender::ImageListItemRender()
{
	m_nImagelistIndex = 0;
	m_pImageList = NULL;
}
ImageListItemRender::~ImageListItemRender()
{
	m_nImagelistIndex = 0;
	m_pImageList = NULL;
}

bool ImageListItemRender::SetAttribute( const String& strPrefix, ATTRMAP& mapAttrib ) 
{
	bool bRet = __super::SetAttribute( strPrefix, mapAttrib );
	if (false == bRet )
		return false;

	if (NULL != m_pBitmap)
	{
		m_pImageList = dynamic_cast<IImageListRenderBitmap*>(m_pBitmap);
	}

	String str = strPrefix + XML_RENDER_IMAGELISTITEM_INDEX;
	ATTRMAP::iterator iter = mapAttrib.find(str);
	if (mapAttrib.end() != iter)
	{
		m_nImagelistIndex = _ttoi(iter->second.c_str());
		mapAttrib.erase(iter);
	}

	return true;
}

SIZE ImageListItemRender::GetDesiredSize()
{
	SIZE s = {0,0};
	if( NULL == m_pImageList )
		return s;

	s.cx = m_pImageList->GetItemWidth();
	s.cy = m_pImageList->GetItemHeight();
	return s;
}

void ImageListItemRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	if (DRAW_BITMAP_TILE == m_nImageDrawType)
	{
		UI_LOG_WARN(_T("%s image list item donot support tile draw"),FUNC_NAME);
		UIASSERT(0 && _T("TODO:"));
		return;
	}

	__super::DrawState(pRenderTarget, prc, nState);
}

POINT ImageListItemRender::GetBitmapSrcDrawPos()
{
	POINT pt = {0,0};
	if (NULL != m_pImageList)
	{
		m_pImageList->GetIndexPos(m_nImagelistIndex, &pt);
	}

	return pt;
}
//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               ColorListRender                                        //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

ColorListRender::ColorListRender( )
{
	m_nCount = 0;
}

ColorListRender::~ColorListRender()
{
	this->Clear();
}
void  ColorListRender::Clear()
{
	for (int i = 0; i < m_nCount; i++)
	{
		SAFE_RELEASE(m_vBkColor[i]);
		SAFE_RELEASE(m_vBorderColor[i]);
	}
	m_vBkColor.clear();
	m_vBorderColor.clear();
}

void ColorListRender::SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder)
{
	nState = LOWORD(nState);
	if(m_nCount > nState)
	{}
	else
	{
		// 需要先填充空内容
		for(int i = m_nCount-1; i <= nState; i++ )
		{
			m_vBkColor.push_back((UIColor*)NULL);
			m_vBorderColor.push_back((UIColor*)NULL);
		}
		m_nCount = nState+1;
	}

	if (bSetBk)
	{
		UIColor::CreateInstance(colorBk, &m_vBkColor[nState]);
		m_vBkColor[nState]->AddRef();
	}
	if (bSetBorder)
	{
		UIColor::CreateInstance(colBorder, &m_vBorderColor[nState]);
		m_vBorderColor[nState]->AddRef();
	}

}
bool ColorListRender::SetAttribute( const String& strPrefix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrefix + XML_RENDER_COLORLIST_COUNT;
	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		this->Clear();

		m_nCount = _ttoi(iter->second.c_str());

		for(int i = 0; i < m_nCount; i++ )
		{
			m_vBkColor.push_back((UIColor*)NULL);
			m_vBorderColor.push_back((UIColor*)NULL);
		}

		mapAttrib.erase(iter);
	}

	if ( 0 == m_nCount )
		return true;

	strAttrib = strPrefix + XML_RENDER_COLOR;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		String& strColors = iter->second;

		vector<String> vColors;
		UI_Split(strColors, XML_MULTI_SEPARATOR, vColors);
		int nCount = (int)vColors.size();

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vColors[i].empty() )
			{
				::UI_GetColor(vColors[i], &m_vBkColor[i]);
			}
		}

		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_RENDER_BORDERCOLOR;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		String& strColors = iter->second;

		vector<String> vColors;
		UI_Split(strColors, XML_MULTI_SEPARATOR, vColors);
		int nCount = (int)vColors.size();

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vColors[i].empty() )
			{
				::UI_GetColor(vColors[i], &m_vBorderColor[i]);
			}
		}

		mapAttrib.erase(iter);
	}

	return true;
}
void ColorListRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	nState = LOWORD(nState);
	int nRealState = nState;
	if( nState >= m_nCount )
	{
		nRealState = 0;
	}

	if( nRealState >= m_nCount )
		return;

	if( NULL == m_vBorderColor[nRealState] )   // 不绘制边框
	{
		if( NULL != m_vBkColor[nRealState] )
		{
			FillRect(pRenderTarget, prc, m_vBkColor[nRealState]->GetColor());
		}
	}
	else                           // 绘制边框
	{
		if( NULL != m_vBkColor[nRealState] )
			Rectangle( pRenderTarget, prc, m_vBorderColor[nRealState]->GetColor(), m_vBkColor[nRealState]->GetColor() );
		else
			Rectangle( pRenderTarget, prc, m_vBorderColor[nRealState]->GetColor(), 0,1,true );
	}
}

//////////////////////////////////////////////////////////////////////////

bool ListRenderBase::SetAttribute(const String& strPrefix, ATTRMAP& mapAttrib)
{
	String strAttrib = strPrefix + XML_RENDER_LIST_STATEMAPINDEX;
	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		m_mapState2Index.clear();
		String& strMap = iter->second;

		vector<String>  vecSplit;
		UI_Split(strMap, XML_SEPARATOR, vecSplit);
		int nCount = vecSplit.size();
		for (int i = 0; i < nCount; i++)
		{
			String& strStateIndex = vecSplit[i];
			vector<String>  vecStateIndex;
			UI_Split(strStateIndex, _T(':'), vecStateIndex);
			if (2 != vecStateIndex.size())
			{
				UI_LOG_WARN(_T("%s invalid state index: %s"), FUNC_NAME, strStateIndex.c_str());
				continue;
			}
			int nState = _ttoi(vecStateIndex[0].c_str());
			int nIndex = _ttoi(vecStateIndex[1].c_str());
			m_mapState2Index[nState] = nIndex;
		}

		mapAttrib.erase(iter);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               ImageListRender                                        //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

ImageListRender::ImageListRender( )
{
	m_eImageLayout = IMAGELIST_LAYOUT_TYPE_H;
	m_hBitmap = NULL;
	m_nItemWidth = m_nItemHeight = 0;
	m_nCount = 0;
	m_p9Region = NULL;
	m_nImageDrawType = DRAW_BITMAP_BITBLT;
}
ImageListRender::~ImageListRender( )
{
	if( NULL != m_hBitmap )
	{
		::UI_ReleaseBitmap(m_hBitmap);
		m_hBitmap = NULL;
	}
	SAFE_DELETE(m_p9Region);
}

bool ImageListRender::SetAttribute( const String& strPrefix, map<String,String>& mapAttrib )
{
	bool bRet = __super::SetAttribute(strPrefix, mapAttrib);
	if (false == bRet)
		return false;

	String strAttrib = strPrefix + XML_RENDER_IMAGELIST_COUNT;
	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		m_nCount = _ttoi(iter->second.c_str());
		mapAttrib.erase(iter);
	}

	if ( 0 == m_nCount )
		return true;

	strAttrib = strPrefix + XML_RENDER_IMAGE;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		if( NULL != m_hBitmap )
		{
			::UI_ReleaseBitmap(m_hBitmap);
			m_hBitmap = NULL;
		}
		m_hBitmap = ::UI_GetBitmap( iter->second, ::GetGraphicsRenderType(m_pObject->GetHWND()) );

		mapAttrib.erase(iter);
	}
	if( NULL == m_hBitmap )
		return true;

	strAttrib = strPrefix + XML_RENDER_IMAGELIST_LAYOUT;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		String& str = iter->second;
		if ( str == XML_RENDER_IMAGELIST_LAYOUT_H )
		{
			m_eImageLayout = IMAGELIST_LAYOUT_TYPE_H;
		}
		else if ( str == XML_RENDER_IMAGELIST_LAYOUT_V )
		{
			m_eImageLayout = IMAGELIST_LAYOUT_TYPE_V;
		}
		else
		{
			UI_LOG_WARN(_T("%s  \"%s\" is invalid: %s"), _T(__FUNCTION__), strAttrib.c_str(), str.c_str() );
		}
		mapAttrib.erase(iter);
	}

	// 计算单张图片的大小
	if( NULL != m_hBitmap && 0 != m_nCount )
	{
		if (m_eImageLayout == IMAGELIST_LAYOUT_TYPE_H)
		{
			m_nItemWidth = UI_GetBitmapWidth(m_hBitmap)/m_nCount;
			m_nItemHeight = UI_GetBitmapHeight(m_hBitmap);
		}
		else
		{
			m_nItemWidth = UI_GetBitmapWidth(m_hBitmap);
			m_nItemHeight = UI_GetBitmapHeight(m_hBitmap)/m_nCount;
		}
	}

	strAttrib = strPrefix + XML_RENDER_IMAGE9REGION;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		SAFE_DELETE(m_p9Region);
		m_p9Region = new Image9Region;

		const String& str9Region = iter->second;
		Util::TranslateImage9Region( str9Region, (void*)m_p9Region );
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_RENDER_IMAGE_DRAWTYPE;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		const String& strType = iter->second;
		if (XML_RENDER_IMAGE_DRAWTYPE_SIMPLE == strType)
			this->SetImageDrawType(DRAW_BITMAP_BITBLT);
		else if (XML_RENDER_IMAGE_DRAWTYPE_TILE == strType)
			this->SetImageDrawType(DRAW_BITMAP_TILE);
		else if (XML_RENDER_IMAGE_DRAWTYPE_STRETCH == strType)
			this->SetImageDrawType(DRAW_BITMAP_STRETCH);
		else if (XML_RENDER_IMAGE_DRAWTYPE_ADAPT == strType)
			this->SetImageDrawType(DRAW_BITMAP_ADAPT);
		else if (XML_RENDER_IMAGE_DRAWTYPE_CENTER == strType)
			this->SetImageDrawType(DRAW_BITMAP_CENTER);

		mapAttrib.erase(iter);
	}

	return true;
}
void ImageListRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	nState = LOWORD(nState);
	int nRealState = nState;
	if( nState >= m_nCount )
	{
		nRealState = 0;
	}

	if( nRealState >= m_nCount )
		return;

	if( NULL != m_hBitmap )
	{
		DRAWBITMAPPARAM param;
		param.nFlag = m_nImageDrawType;
		param.xDest = prc->left;
		param.yDest = prc->top;
		param.wDest = prc->Width();
		param.hDest = prc->Height();
		param.wSrc = m_nItemWidth;
		param.hSrc = m_nItemHeight;
		param.pRegion = m_p9Region;

		if( m_eImageLayout == IMAGELIST_LAYOUT_TYPE_H )
		{
			param.xSrc = nRealState*m_nItemWidth;
			param.ySrc = 0;
		}
		else
		{
			param.xSrc = 0;
			param.ySrc = nRealState*m_nItemHeight;
		}
		pRenderTarget->DrawBitmap(m_hBitmap, &param);

// 		switch (m_eImageDrawType)
// 		{
// 		case IMAGE_DRAW_TYPE_SIMPLE:
// 			{
// 				if( m_eImageLayout == IMAGELIST_LAYOUT_TYPE_H )
// 				{
// 					DrawBitmap(pRenderTarget, m_hBitmap, prc->left,prc->top,m_nItemWidth,m_nItemHeight, nRealState*m_nItemWidth,0,m_nItemWidth,m_nItemHeight );
// 				}
// 				else
// 				{
// 					DrawBitmap(pRenderTarget, m_hBitmap, prc->left,prc->top,m_nItemWidth,m_nItemHeight, 0, nRealState*m_nItemHeight,m_nItemWidth,m_nItemHeight );
// 				}
// 			}
// 			break;
// 
// 		case IMAGE_DRAW_TYPE_STRETCH:
// 			{
// 				if( m_eImageLayout == IMAGELIST_LAYOUT_TYPE_H )
// 				{
// 					DrawBitmap(pRenderTarget, m_hBitmap, prc->left,prc->top,prc->Width(),prc->Height(), 
// 						nRealState*m_nItemWidth,0,m_nItemWidth,m_nItemHeight,
// 						m_p9Region);
// 				}
// 				else
// 				{
// 					DrawBitmap(pRenderTarget, m_hBitmap, prc->left,prc->top,prc->Width(),prc->Height(), 
// 						0, nRealState*m_nItemHeight,m_nItemWidth,m_nItemHeight,
// 						m_p9Region);
// 				}
// 			}
// 			break;;
// 		}
// 		
	}
}
SIZE ImageListRender::GetDesiredSize()
{
	SIZE s = {m_nItemWidth, m_nItemHeight};
	return s;
}

//
//	获取指定状态对应的图片项
//
int ImageListRender::GetStateIndex(int nState)
{
	if (m_nCount <= 0)
		return -1;

	map<int, int>::iterator iter = m_mapState2Index.find(nState);
	if (m_mapState2Index.end() == iter)
	{
		// 没有配置该状态或者没有配置
		if (nState < m_nCount)  // 如果图片数量大于当前状态，直接取当前状态值对应的图片
		{
			return nState;
		}
		else                    // 状态值大于图片数量，取默认值0
		{
			return 0;
		}
	}
	else
	{
		int& nRet = iter->second;
		if (nRet < 0 || nRet >= m_nCount)
			return -1;

		return nRet;
	}
	return -1;
}


//////////////////////////////////////////////////////////////////////////

ThemeRenderBase::ThemeRenderBase()
{
	m_hTheme = NULL;
}
ThemeRenderBase::~ThemeRenderBase()
{
	if( m_hTheme )
	{
		::CloseThemeData( m_hTheme );
		m_hTheme = NULL;
	}
}
void ThemeRenderBase::Init()
{
	this->CreateTheme();
}
void ThemeRenderBase::OnThemeChanged()
{
	this->CreateTheme();
}
void ThemeRenderBase::CreateTheme()
{
	if (NULL != m_hTheme)
	{
		::CloseThemeData(m_hTheme);
		m_hTheme = NULL;
	}
	m_hTheme = ::OpenThemeData(NULL, this->GetThemeName());
}

void ButtonBkThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(pRenderTarget, prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;;

	default:
		this->DrawNormal(pRenderTarget, prc);
		break;
	}
}

void ButtonBkThemeRender::DrawDisable(IRenderTarget* pRenderTarget, const CRect* prc  )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, PBS_DISABLED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_INACTIVE );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void ButtonBkThemeRender::DrawNormal(IRenderTarget* pRenderTarget, const CRect* prc  )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, ((ButtonBase*)m_pObject)->IsDefault()?PBS_DEFAULTED:PBS_NORMAL, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH );
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void ButtonBkThemeRender::DrawHover(IRenderTarget* pRenderTarget, const CRect* prc  )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, PBS_HOT, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_HOT );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void ButtonBkThemeRender::DrawPress(IRenderTarget* pRenderTarget, const CRect* prc  )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, PBS_PRESSED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED );
	}
	pRenderTarget->ReleaseHDC(hDC);
}


//////////////////////////////////////////////////////////////////////////

SIZE RadioButtonThemeRender::GetDesiredSize( )
{
	SIZE s = {12,12};

	if( NULL != m_hTheme )
	{
		HDC hDC = UI_GetCacheDC();
		GetThemePartSize(m_hTheme,hDC,BP_RADIOBUTTON,RBS_UNCHECKEDNORMAL, NULL, TS_DRAW, &s );
		UI_ReleaseCacheDC(hDC);
	}

	return s;
}
void RadioButtonThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_ICON_RENDER_STATE_NORMAL:
		this->DrawNormal(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_PRESS:
		this->DrawPress(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:
		this->DrawCheckNormal(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:
		this->DrawCheckHover(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:
		this->DrawCheckPress(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:
		this->DrawCheckDisable(pRenderTarget, prc);
		break;
	}
}

void RadioButtonThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc ;
		CopyRect(&rc, prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, CBS_UNCHECKEDDISABLED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_INACTIVE;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void RadioButtonThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc ;
		CopyRect(&rc, prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, RBS_UNCHECKEDNORMAL, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void RadioButtonThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc ;
		CopyRect(&rc, prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, RBS_UNCHECKEDHOT, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_HOT;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void RadioButtonThemeRender::DrawPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc ;
		CopyRect(&rc, prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, RBS_UNCHECKEDPRESSED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_PUSHED;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void RadioButtonThemeRender::DrawCheckDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		CRect rc ;
		rc.CopyRect(prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, RBS_CHECKEDDISABLED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_INACTIVE|DFCS_CHECKED;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void RadioButtonThemeRender::DrawCheckNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc ;
		CopyRect(&rc, prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON,RBS_CHECKEDNORMAL, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_CHECKED;;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void RadioButtonThemeRender::DrawCheckHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc ;
		CopyRect(&rc, prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, RBS_CHECKEDHOT, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_HOT|DFCS_CHECKED;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void RadioButtonThemeRender::DrawCheckPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc ;
		CopyRect(&rc, prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_RADIOBUTTON, RBS_CHECKEDPRESSED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONRADIO|DFCS_PUSHED|DFCS_CHECKED;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}

//////////////////////////////////////////////////////////////////////////

SIZE  CheckButtonThemeRender::GetDesiredSize( )
{
	SIZE s = {13,13};

	if( NULL != m_hTheme )
	{
		HDC hDC = UI_GetCacheDC();
		GetThemePartSize(m_hTheme,hDC,BP_CHECKBOX,CBS_UNCHECKEDNORMAL, NULL, TS_DRAW, &s );
		UI_ReleaseCacheDC(hDC);
	}

	return s;
}

void CheckButtonThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_ICON_RENDER_STATE_NORMAL:
		this->DrawNormal(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_PRESS:
		this->DrawPress(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:
		this->DrawCheckNormal(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:
		this->DrawCheckHover(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:
		this->DrawCheckPress(pRenderTarget, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:
		this->DrawCheckDisable(pRenderTarget, prc);
		break;
	}
}

void CheckButtonThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc;
		::CopyRect(&rc,prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX,  CBS_UNCHECKEDDISABLED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_INACTIVE;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void CheckButtonThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc;
		::CopyRect(&rc,prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX,CBS_UNCHECKEDNORMAL, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void CheckButtonThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc;
		::CopyRect(&rc,prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX, CBS_UNCHECKEDHOT, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_HOT;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void CheckButtonThemeRender::DrawPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc;
		::CopyRect(&rc,prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX, CBS_UNCHECKEDPRESSED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_PUSHED;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}



void CheckButtonThemeRender::DrawCheckDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc;
		::CopyRect(&rc,prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX, CBS_CHECKEDDISABLED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_INACTIVE|DFCS_CHECKED;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void CheckButtonThemeRender::DrawCheckNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc;
		::CopyRect(&rc,prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX,CBS_CHECKEDNORMAL, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_CHECKED;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void CheckButtonThemeRender::DrawCheckHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc;
		::CopyRect(&rc,prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX, CBS_CHECKEDHOT, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_HOT|DFCS_CHECKED;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void CheckButtonThemeRender::DrawCheckPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		RECT rc;
		::CopyRect(&rc,prc);
		rc.top -= 1;
		rc.bottom += 1;
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_CHECKBOX, CBS_CHECKEDPRESSED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("RadioButtonThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		UINT nFlag = DFCS_BUTTONCHECK|DFCS_PUSHED|DFCS_CHECKED;
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, nFlag );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
//////////////////////////////////////////////////////////////////////////

void EditBkThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case EDIT_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;

	case EDIT_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(pRenderTarget, prc);
		break;

	case EDIT_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;;

	default:
		this->DrawNormal(pRenderTarget, prc);
		break;
	}
}

void EditBkThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, EP_EDITBORDER_NOSCROLL, ETS_DISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawDisable  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void EditBkThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, EP_EDITBORDER_NOSCROLL, ETS_CUEBANNER, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
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

void EditBkThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, EP_EDITBORDER_NOSCROLL, ETS_HOT, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawHover  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
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
void EditBkThemeRender::DrawPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, EP_EDITBORDER_NOSCROLL, ETS_SELECTED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawPress  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
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



//////////////////////////////////////////////////////////////////////////

void GroupBoxBkThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case GROUPBOX_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;
	default:
		this->DrawNormal(pRenderTarget, prc);
		break;
	}
}

void GroupBoxBkThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	RECT rc;
	((GroupBox*)m_pObject)->GetBorderRect(&rc);

	HDC hDC = pRenderTarget->GetHDC();
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
		if (prc->Height()<=2)
			DrawEdge(hDC, &rc, EDGE_ETCHED, BF_TOP);   // 分隔线类型
		else
			DrawEdge(hDC, &rc, EDGE_ETCHED, BF_RECT);
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void GroupBoxBkThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	RECT rc;
	((GroupBox*)m_pObject)->GetBorderRect(&rc);

	HDC hDC = pRenderTarget->GetHDC();
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
		if (prc->Height()<=2)
			DrawEdge(hDC, &rc, EDGE_ETCHED, BF_TOP);   // 分隔线类型
		else
			DrawEdge(hDC, &rc, EDGE_ETCHED, BF_RECT);
	}
	pRenderTarget->ReleaseHDC(hDC);
}


void GroupBoxBkNoThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	RECT rc;
	((GroupBox*)m_pObject)->GetBorderRect(&rc);

	HDC hDC = pRenderTarget->GetHDC();
	if (prc->Height()<=2)
		DrawEdge(hDC, &rc, EDGE_ETCHED, BF_TOP);   // 分隔线类型
	else
		DrawEdge(hDC, &rc, EDGE_ETCHED, BF_RECT);
	pRenderTarget->ReleaseHDC(hDC);
}


SIZE ComboboxButtonBkThemeRender::GetDesiredSize()
{
	SIZE s = {m_hTheme?17:18,0};

//  TODO: 1. 这里一直返回0
//        2. 有xp中有些主题的button在绘制时，又不用去deflatrect(1,1,1,1)，这个怎么去判断？例如那个仿苹果主题
// 	if( NULL != m_hTheme )
// 	{
// 		HDC hDC = UI_GetCacheDC();
// 		RECT rect = {0,0,100,50};
// 		GetThemePartSize(m_hTheme,hDC,CP_DROPDOWNBUTTON,CBXS_NORMAL, &rect, TS_DRAW, &s );
// 		UI_ReleaseCacheDC(hDC);
// 	}

	return s;
}

void ComboboxButtonBkThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_BKGND_RENDER_STATE_DISABLE:
			this->DrawDisable(pRenderTarget, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_PRESS:
			this->DrawPress(pRenderTarget, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_HOVER:
			this->DrawHover(pRenderTarget, (CRect*)prc);
		break;;

	default:
			this->DrawNormal(pRenderTarget, (CRect*)prc);
		break;
	}
}

void ComboboxButtonBkThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc  )
{
	HDC hDC = pRenderTarget->GetHDC();
	CRect rc = *prc;
	if( m_hTheme )
	{
		//rc.DeflateRect(1,1,1,1);
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTONRIGHT, CBXSR_DISABLED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		rc.DeflateRect(0,2,2,2);
		//DrawFrameControl(hDC, (RECT*)&rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_INACTIVE );
		//DrawGlyph(hDC, &rc);
		::DrawFrameControl(hDC, (RECT*)&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX|DFCS_INACTIVE);
	}
	pRenderTarget->ReleaseHDC(hDC);
}
 

void ComboboxButtonBkThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc  )
{
	HDC hDC = pRenderTarget->GetHDC();
	CRect rc = *prc;
	if( m_hTheme )
	{
		//rc.DeflateRect(1,1,1,1);
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, /*CP_DROPDOWNBUTTON*/CP_DROPDOWNBUTTONRIGHT, CBXSR_NORMAL, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
 		rc.DeflateRect(0,2,2,2);
// 		DrawFrameControl(hDC, (RECT*)&rc, DFC_BUTTON, DFCS_BUTTONPUSH );
// 		DrawGlyph(hDC, &rc);
		::DrawFrameControl(hDC, (RECT*)&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX);
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void ComboboxButtonBkThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc  )
{
	HDC hDC = pRenderTarget->GetHDC();

	CRect rc = *prc;
	if( m_hTheme )
	{
		//rc.DeflateRect(1,1,1,1);
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, /*CP_DROPDOWNBUTTON*/CP_DROPDOWNBUTTONRIGHT, CBXSR_HOT, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		rc.DeflateRect(0,2,2,2);
		//DrawFrameControl(hDC, (RECT*)&rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_HOT );
		//DrawGlyph(hDC, &rc);
		::DrawFrameControl(hDC, (RECT*)&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX|DFCS_HOT);
	}
	pRenderTarget->ReleaseHDC(hDC);
}
 
void ComboboxButtonBkThemeRender::DrawPress( IRenderTarget* pRenderTarget, const CRect* prc  )
{
	HDC hDC = pRenderTarget->GetHDC();
	CRect rc = *prc;
	if( m_hTheme )
	{
		//rc.DeflateRect(1,1,1,1);
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, /*CP_DROPDOWNBUTTON*/CP_DROPDOWNBUTTONRIGHT, CBXSR_PRESSED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		rc.DeflateRect(0,2,2,2);
		//DrawFrameControl(hDC, (RECT*)&rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED|DFCS_FLAT );
		//DrawGlyph(hDC, &rc, true);
		::DrawFrameControl(hDC, (RECT*)&rc, DFC_SCROLL, DFCS_SCROLLCOMBOBOX|DFCS_PUSHED|DFCS_FLAT);
	}
	pRenderTarget->ReleaseHDC(hDC);
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


void ComboboxBkThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_BORDER, CBXS_DISABLED, (RECT*)prc, 0);
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_BORDER, CBXS_NORMAL, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_BORDER, CBXS_HOT, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawHover  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_BORDER, CBXS_PRESSED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawPress  DrawThemeBackground failed."));
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_READONLY, CBXS_DISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawDisable  DrawThemeBackground failed."));
		}
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_DISABLED, (RECT*)&rc, 0);
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_READONLY, CBXS_NORMAL, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);
		}

		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_NORMAL, (RECT*)&rc, 0);
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_READONLY, CBXS_HOT, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}

		if (UI_IsUnderXpOS())
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_HOT, (RECT*)&rc, 0);
			if ( S_OK != hr )
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);
			}
		}
		else
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_NORMAL, (RECT*)&rc, 0);
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_READONLY, CBXS_PRESSED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxBkThemeRender::DrawPress  DrawThemeBackground failed."));
		}

		if (UI_IsUnderXpOS())
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_PRESSED, (RECT*)&rc, 0);
			if ( S_OK != hr )
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground failed."),FUNC_NAME);
			}
		}
		else
		{
			CRect rc(prc);
			rc.left = rc.right-18;
			hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTONRIGHT, CBXS_NORMAL, (RECT*)&rc, 0);
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

//////////////////////////////////////////////////////////////////////////

void ScrollLineButtonBkThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(pRenderTarget, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, (CRect*)prc);
		break;;

	default:
		this->DrawNormal(pRenderTarget, (CRect*)prc);
		break;
	}
}

SIZE ScrollLineButtonBkThemeRender::GetDesiredSize() 
{
	SIZE s = {GetSystemMetrics(SM_CXVSCROLL),GetSystemMetrics(SM_CYVSCROLL)}; 
	return s;
}

void ScrollLineButtonBkThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, SBP_ARROWBTN, GetThemeStateDisable(), prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_SCROLL, GetNoThemeState()|DFCS_INACTIVE );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void ScrollLineButtonBkThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, SBP_ARROWBTN, GetThemeStateNormal(), prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_SCROLL, GetNoThemeState() );
	}
	pRenderTarget->ReleaseHDC(hDC);	
}
void ScrollLineButtonBkThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, SBP_ARROWBTN, GetThemeStateHover(), prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_SCROLL, GetNoThemeState() );
	}
	pRenderTarget->ReleaseHDC(hDC);	
}
void ScrollLineButtonBkThemeRender::DrawPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, SBP_ARROWBTN, GetThemeStatePress(), prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_SCROLL, DFCS_FLAT|DFCS_PUSHED|GetNoThemeState() );
	}
	pRenderTarget->ReleaseHDC(hDC);	
}


//////////////////////////////////////////////////////////////////////////

void ScrollThumbButtonThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(pRenderTarget, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, (CRect*)prc);
		break;;

	default:
		this->DrawNormal(pRenderTarget, (CRect*)prc);
		break;
	}
}

SIZE ScrollThumbButtonThemeRender::GetDesiredSize() 
{
	SIZE s = {GetSystemMetrics(SM_CXVSCROLL),GetSystemMetrics(SM_CYVSCROLL)}; 
	return s;
}

void ScrollThumbButtonThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, GetThumbBtmType(), SCRBS_DISABLED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
		hr = DrawThemeBackground(m_hTheme, hDC, GetGripperType(), SCRBS_DISABLED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_INACTIVE );
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void ScrollThumbButtonThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, GetThumbBtmType(), SCRBS_NORMAL, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
		hr = DrawThemeBackground(m_hTheme, hDC, GetGripperType(), SCRBS_NORMAL, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH);
	}
	pRenderTarget->ReleaseHDC(hDC);	
}
void ScrollThumbButtonThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, GetThumbBtmType(), SCRBS_HOT, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
		hr = DrawThemeBackground(m_hTheme, hDC, GetGripperType(), SCRBS_HOT, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_HOT );
	}
	pRenderTarget->ReleaseHDC(hDC);	
}
void ScrollThumbButtonThemeRender::DrawPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, GetThumbBtmType(), SCRBS_PRESSED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
		hr = DrawThemeBackground(m_hTheme, hDC, GetGripperType(), SCRBS_PRESSED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s  DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH );
	}
	pRenderTarget->ReleaseHDC(hDC);	
}

//////////////////////////////////////////////////////////////////////////

void ScrollBarBkgndThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	HDC hDC = pRenderTarget->GetHDC();		
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, GetThemeType(), SCRBS_NORMAL, prc, 0);
		if ( S_OK != hr )
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
	pRenderTarget->ReleaseHDC(hDC);	
}
SIZE ScrollBarBkgndThemeRender::GetDesiredSize()
{
	SIZE s = {GetSystemMetrics(SM_CXVSCROLL),GetSystemMetrics(SM_CYVSCROLL)}; 
	return s;
}

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


void ListboxBkThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case LISTBOX_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;

	case LISTBOX_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(pRenderTarget, prc);
		break;

	case LISTBOX_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;;

	default:
		this->DrawNormal(pRenderTarget, prc);
		break;
	}
}

void ListboxBkThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, LBCP_BORDER_NOSCROLL, LBPSH_DISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void ListboxBkThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, LBCP_BORDER_NOSCROLL, LBPSH_NORMAL, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
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

void ListboxBkThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, LBCP_BORDER_NOSCROLL, LBPSH_HOT, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
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
void ListboxBkThemeRender::DrawPress( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, LBCP_BORDER_NOSCROLL, LBPSH_FOCUSED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		if( m_pObject->IsReadonly() )
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


//////////////////////////////////////////////////////////////////////////


void MenuBkThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
// 	switch(nState)
// 	{
// 	default:
		this->DrawNormal(pRenderTarget, prc);
// 		break;
// 	}
}

void MenuBkThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPBACKGROUND, 1, (RECT*)prc, 0);
		if (SUCCEEDED(hr))
		{
			hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPBORDERS, 1, (RECT*)prc, 0);
			if (S_OK != hr)
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
			}
		}

		// 绘制图标列背景
		MenuBase* pMenu = dynamic_cast<MenuBase*>(m_pObject);
		if (NULL != pMenu)
		{
			CRect rcIconGutter(*prc);
			CRegion4 rcBorder;
			pMenu->GetBorderRegion(&rcBorder);
			
			Util::DeflatRect(&rcIconGutter, &rcBorder);
			rcIconGutter.right = rcIconGutter.left + pMenu->GetIconGutterWidth();

			hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPGUTTER, 1, (RECT*)&rcIconGutter, 0);
			if ( S_OK != hr )
			{
				UI_LOG_WARN(_T("%s DrawThemeBackground MENU_POPUPGUTTER failed."), FUNC_NAME);
			}
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
void MenuStringItemRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case MENU_STRING_ITEM_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;

	case MENU_STRING_ITEM_RENDER_STATE_PRESS:
		this->DrawPress(pRenderTarget, prc);
		break;

	case MENU_STRING_ITEM_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;;

	default:
		this->DrawNormal(pRenderTarget, prc);
		break;
	}
}

void MenuStringItemRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPITEM, MPI_DISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void MenuStringItemRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
// 	HDC hDC = pRenderTarget->GetHDC();
// 	if( m_hTheme )
// 	{
// 		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPITEM, MPI_NORMAL, (RECT*)prc, 0);
// 		if ( S_OK != hr )
// 		{
// 			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
// 		}
// 	}
// 	else
// 	{
// 	}
// 	pRenderTarget->ReleaseHDC(hDC);
}
void MenuStringItemRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPITEM, MPI_HOT, (RECT*)prc, 0);
		if ( S_OK != hr )
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
{

}

//////////////////////////////////////////////////////////////////////////

void MenuSeperatorThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	this->DrawNormal(pRenderTarget, prc);
}

void MenuSeperatorThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPSEPARATOR, 1, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), _T(__FUNCTION__));
		}
	}
	else
	{
		COLORREF col1 = RGB(128,128,128);
		COLORREF col2 = RGB(255,255,255);
		HPEN hPen = CreatePen(PS_SOLID,1,col1);
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

void MenuCheckedIconThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case MENU_ITEM_ICON_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;
	case MENU_ITEM_ICON_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;
	default:
		this->DrawNormal(pRenderTarget, prc);
		break;
	}
}

void MenuCheckedIconThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPCHECK, MC_CHECKMARKNORMAL, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), _T(__FUNCTION__));
		}
	}
	else
	{
		// 注：直接使用DrawFrameControl得到的图片是白底黑字（√）的，因此需要进行二次处理
		//     将白色背景去掉。见MSDN
		HDC hMemDC = UI_GetCacheDC();
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hMemDC, prc->Width(), prc->Height());
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);

		RECT rc = {0,0, prc->Width(), prc->Height()};
		::DrawFrameControl(hMemDC, (LPRECT)&rc, DFC_MENU, DFCS_MENUCHECK);
		::BitBlt(hDC, prc->left, prc->top, prc->Width(), prc->Height(), hMemDC, 0,0, SRCAND);

		SelectObject(hMemDC, hOldBmp);
		SAFE_DELETE_GDIOBJECT(hBitmap);
		UI_ReleaseCacheDC(hMemDC);
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void MenuCheckedIconThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPCHECK, MC_CHECKMARKNORMAL, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), _T(__FUNCTION__));
		}
	}
	else
	{
		HDC hMemDC = UI_GetCacheDC();
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hMemDC, prc->Width(), prc->Height());
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);

		RECT rc = {0,0, prc->Width(), prc->Height()};
		::DrawFrameControl(hMemDC, (LPRECT)&rc, DFC_MENU, DFCS_MENUCHECK);
		::BitBlt(hDC, prc->left, prc->top, prc->Width(), prc->Height(), hMemDC, 0,0, MERGEPAINT);

		SelectObject(hMemDC, hOldBmp);
		SAFE_DELETE_GDIOBJECT(hBitmap);
		UI_ReleaseCacheDC(hMemDC);
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void MenuCheckedIconThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPCHECK, MC_CHECKMARKDISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), _T(__FUNCTION__));
		}
	}
	else
	{
		// TODO: 有办法绘制Disable状态的样式吗？
		HDC hMemDC = UI_GetCacheDC();
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hMemDC, prc->Width(), prc->Height());
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);

		RECT rc = {0,0, prc->Width(), prc->Height()};
		::DrawFrameControl(hMemDC, (LPRECT)&rc, DFC_MENU, DFCS_MENUCHECK);
		::BitBlt(hDC, prc->left, prc->top, prc->Width(), prc->Height(), hMemDC, 0,0, SRCAND);

		SelectObject(hMemDC, hOldBmp);
		SAFE_DELETE_GDIOBJECT(hBitmap);
		UI_ReleaseCacheDC(hMemDC);
	}
	pRenderTarget->ReleaseHDC(hDC);
}


//////////////////////////////////////////////////////////////////////////

void MenuRadioIconThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case MENU_ITEM_ICON_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;
	case MENU_ITEM_ICON_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;
	default:
		this->DrawNormal(pRenderTarget, prc);
		break;
	}
}

void MenuRadioIconThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPCHECK, MC_BULLETNORMAL, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), _T(__FUNCTION__));
		}
	}
	else
	{
		HDC hMemDC = UI_GetCacheDC();
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hMemDC, prc->Width(), prc->Height());
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);

		RECT rc = {0,0, prc->Width(), prc->Height()};
		::DrawFrameControl(hMemDC, (LPRECT)&rc, DFC_MENU, DFCS_MENUBULLET);
		::BitBlt(hDC, prc->left, prc->top, prc->Width(), prc->Height(), hMemDC, 0,0, SRCAND);

		SelectObject(hMemDC, hOldBmp);
		SAFE_DELETE_GDIOBJECT(hBitmap);
		UI_ReleaseCacheDC(hMemDC);
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void MenuRadioIconThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPCHECK, MC_BULLETNORMAL, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), _T(__FUNCTION__));
		}
	}
	else
	{
		HDC hMemDC = UI_GetCacheDC();
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hMemDC, prc->Width(), prc->Height());
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);

		RECT rc = {0,0, prc->Width(), prc->Height()};
		::DrawFrameControl(hMemDC, (LPRECT)&rc, DFC_MENU, DFCS_MENUBULLET);
		::BitBlt(hDC, prc->left, prc->top, prc->Width(), prc->Height(), hMemDC, 0,0, MERGEPAINT);

		SelectObject(hMemDC, hOldBmp);
		SAFE_DELETE_GDIOBJECT(hBitmap);
		UI_ReleaseCacheDC(hMemDC);
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void MenuRadioIconThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPCHECK, MC_BULLETDISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), _T(__FUNCTION__));
		}		
	}
	else
	{
		HDC hMemDC = UI_GetCacheDC();
		HBITMAP hBitmap = ::CreateCompatibleBitmap(hMemDC, prc->Width(), prc->Height());
		HBITMAP hOldBmp = (HBITMAP)::SelectObject(hMemDC, hBitmap);

		RECT rc = {0,0, prc->Width(), prc->Height()};
		::DrawFrameControl(hMemDC, (LPRECT)&rc, DFC_MENU, DFCS_MENUBULLET);
		::BitBlt(hDC, prc->left, prc->top, prc->Width(), prc->Height(), hMemDC, 0,0, SRCAND);

		SelectObject(hMemDC, hOldBmp);
		SAFE_DELETE_GDIOBJECT(hBitmap);
		UI_ReleaseCacheDC(hMemDC);
	}
	pRenderTarget->ReleaseHDC(hDC);
}



//////////////////////////////////////////////////////////////////////////

void MenuRadioCheckIconBkThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
	{
	case MENU_ITEM_ICON_RENDER_STATE_DISABLE:
		this->DrawDisable(pRenderTarget, prc);
		break;
	case MENU_ITEM_ICON_RENDER_STATE_HOVER:
		this->DrawHover(pRenderTarget, prc);
		break;
	default:
		this->DrawNormal(pRenderTarget, prc);
		break;
	}
}

void MenuRadioCheckIconBkThemeRender::DrawNormal( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPCHECKBACKGROUND, MCB_NORMAL, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), _T(__FUNCTION__));
		}
	}
	else
	{
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void MenuRadioCheckIconBkThemeRender::DrawHover( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPCHECKBACKGROUND, MCB_NORMAL, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), _T(__FUNCTION__));
		}
	}
	else
	{
	}
	pRenderTarget->ReleaseHDC(hDC);
}

void MenuRadioCheckIconBkThemeRender::DrawDisable( IRenderTarget* pRenderTarget, const CRect* prc )
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPCHECKBACKGROUND, MCB_DISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
	}
	pRenderTarget->ReleaseHDC(hDC);
}

//////////////////////////////////////////////////////////////////////////

void MenuPopupTriangleRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	switch(nState)
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
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPSUBMENU, MSM_DISABLED, (RECT*)prc, 0);
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPSUBMENU, MSM_NORMAL, (RECT*)prc, 0);
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
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, MENU_POPUPSUBMENU, MSM_NORMAL, (RECT*)prc, 0);
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

void TooltipBkgndThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	if( m_hTheme )
	{
// 		COLORREF colBkgnd = ::GetSysColor(COLOR_INFOBK);
// 		COLORREF colBorder = RGB(0,0,0);
// 		Rectangle(pRenderTarget, prc, colBorder, colBkgnd );

// 		CRect r(*prc);
// 		r.top += 3;
// 		r.left += 3;
 		HDC hDC = pRenderTarget->GetHDC();
 		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, TTP_STANDARD, TTSS_NORMAL, (RECT*)prc, 0);
 		if ( S_OK != hr )
 		{
 			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
 		}
 		pRenderTarget->ReleaseHDC(hDC);
	}
	else
	{
		COLORREF colBkgnd = ::GetSysColor(COLOR_INFOBK);
		COLORREF colBorder = RGB(0,0,0);
		Rectangle(pRenderTarget, prc, colBorder, colBkgnd );
	}
	
}
SIZE TooltipBkgndThemeRender::GetDesiredSize()
{
	SIZE s = {0,0};
	return s;
}

//////////////////////////////////////////////////////////////////////////

void ProgressCtrlBkgndThemeRender::SetObject( Object* pObject ) 
{ 
	this->m_pObject = pObject; 
	m_pProgress = dynamic_cast<ProgressCtrl*>(pObject);
}
void  ProgressCtrlBkgndThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		int iPartId = PP_BAR;
		if (NULL != m_pProgress)
		{
			PROGRESS_SCROLL_DIRECTION_TYPE eType = m_pProgress->GetDirectionType();
			if (PROGRESS_SCROLL_BOTTOM_2_TOP == eType || PROGRESS_SCROLL_TOP_2_BOTTOM == eType)
			{
				iPartId = PP_BARVERT;
			}
		}
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, iPartId, PBFS_NORMAL, (RECT*)prc, 0);
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

void ProgressCtrlForegndThemeRender::SetObject( Object* pObject ) 
{ 
	this->m_pObject = pObject; 
	m_pProgress = dynamic_cast<ProgressCtrl*>(pObject);
}
void  ProgressCtrlForegndThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	CRect rc(prc);

	HDC hDC = pRenderTarget->GetHDC();
	if( m_hTheme )
	{
		if (UI_IsUnderXpOS())
		{
			rc.DeflateRect(3,3,3,3);
		}
		else
		{
			//rc.DeflateRect(1,1,1,1);
		}

		// 判断该进度条是否是纵向的
		bool bVert = false;
		if (NULL != m_pProgress)
		{
			PROGRESS_SCROLL_DIRECTION_TYPE eType = m_pProgress->GetDirectionType();
			if (PROGRESS_SCROLL_BOTTOM_2_TOP == eType || PROGRESS_SCROLL_TOP_2_BOTTOM == eType)
			{
				bVert = true;
			}
		}

		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, bVert?PP_FILLVERT:PP_FILL, PBFS_NORMAL, (RECT*)&rc, 0);
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
		if (NULL != m_pProgress)
		{
			eType = m_pProgress->GetDirectionType();
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

//////////////////////////////////////////////////////////////////////////

void SliderCtrlBkgndThemeRender::SetObject( Object* pObject )
{
	__super::SetObject(pObject);

	m_pSliderCtrl = dynamic_cast<SliderCtrl*>(pObject);
}
void SliderCtrlBkgndThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	CRect rc(prc);
	PROGRESS_SCROLL_DIRECTION_TYPE eType = PROGRESS_SCROLL_LEFT_2_RIGHT;
	if (NULL != m_pSliderCtrl)
	{
		eType = m_pSliderCtrl->GetDirectionType();
	}

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
	if (m_hTheme)
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, TKP_TRACK, TKS_NORMAL, (RECT*)&rc, 0);
		if (S_OK != hr)
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	else
	{
		DrawEdge(hDC, (RECT*)&rc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
	}
	pRenderTarget->ReleaseHDC(hDC);
}


void SliderTrackButtonThemeRender::SetObject( Object* pObject )
{
	__super::SetObject(pObject);

	if (NULL != pObject->GetParentObject())
		m_pSliderCtrl = dynamic_cast<SliderCtrl*>(pObject->GetParentObject());
}
SIZE SliderTrackButtonThemeRender::GetDesiredSize( )
{
	PROGRESS_SCROLL_DIRECTION_TYPE eType = PROGRESS_SCROLL_LEFT_2_RIGHT;
	if (NULL != m_pSliderCtrl)
	{
		eType = m_pSliderCtrl->GetDirectionType();
	}

	switch(eType)
	{
	case PROGRESS_SCROLL_TOP_2_BOTTOM:
	case PROGRESS_SCROLL_BOTTOM_2_TOP:
		{
			SIZE s = {22,11};
			return s;
		}

	default:
		{
			SIZE s = {11,22};
			return s;
		}
	}

	SIZE s = {11,22};
	return s;
}
void SliderTrackButtonThemeRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState)
{
	if (NULL != m_hTheme)
	{
		this->DrawThemeState(pRenderTarget, prc, nState);
		return;
	}
	else
	{
		HDC hDC = pRenderTarget->GetHDC();
		this->DrawNoThemeState(hDC, prc, nState);
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
	if (m_hTheme)
	{
		PROGRESS_SCROLL_DIRECTION_TYPE eType = PROGRESS_SCROLL_LEFT_2_RIGHT;
		bool bPointLeftTop = false;
		bool bPointRightBottom = false;
		if (NULL != m_pSliderCtrl)
		{
			eType = m_pSliderCtrl->GetDirectionType();
			bPointLeftTop = m_pSliderCtrl->TestStyle(SLIDER_STYLE_POINT_LEFT);
			bPointRightBottom = m_pSliderCtrl->TestStyle(SLIDER_STYLE_POINT_RIGHT);
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
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, iPart, nStateID, (RECT*)&rc, 0);
		if (S_OK != hr)
		{
			UI_LOG_WARN(_T("%s DrawThemeBackground failed."), FUNC_NAME);
		}
	}
	pRenderTarget->ReleaseHDC(hDC);
}
void SliderTrackButtonThemeRender::DrawNoThemeState( HDC hDC, const CRect* prc, int nDrawState)
{
	// TODO: 又没有直接的方法来绘制。。。 NND，随便画吧
	CRect rc(prc);

	HBRUSH hBrush = ::CreateSolidBrush(RGB(212,208,200));
	::FillRect(hDC, &rc, hBrush);
	::DeleteObject(hBrush);

	rc.bottom -= 7;
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

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                                  TextRender                                          //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


TextRenderBase::TextRenderBase()
{
	m_pObject = NULL;
	m_nTextRenderType = TEXTRENDER_TYPE_NULL;
	m_nDrawTextFlag = DT_SINGLELINE|DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_NOPREFIX;
	::SetRectEmpty(&m_rPadding);
}
void TextRenderBase::SetTextAlignment(int nDrawFlag)
{
	m_nDrawTextFlag = nDrawFlag;
}

bool TextRenderBase::SetAttribute( const String& strPrefix, ATTRMAP& mapAttrib )
{
	String strAttr = strPrefix + XML_TEXTRENDER_ALIGN;
	ATTRMAP::iterator iter = mapAttrib.find(strAttr);
	if (mapAttrib.end() != iter)
	{
		String& str = iter->second;
		vector<String> vecAlign;
		UI_Split(str, XML_FLAG_SEPARATOR, vecAlign);

		m_nDrawTextFlag = 0;

		int nSize = (int)vecAlign.size();
		for (int i = 0; i < nSize; i++)
		{
			const String& strType = vecAlign[i];

			if (strType == XML_TEXTRENDER_ALIGN_LEFT)
			{
				m_nDrawTextFlag |= DT_LEFT;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_RIGHT )
			{
				m_nDrawTextFlag |= DT_RIGHT;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_CENTER )
			{
				m_nDrawTextFlag |= DT_CENTER;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_TOP )
			{
				m_nDrawTextFlag |= DT_TOP;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_BOTTOM )
			{
				m_nDrawTextFlag |= DT_BOTTOM;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_VCENTER )
			{
				m_nDrawTextFlag |= DT_VCENTER|DT_SINGLELINE;
			}
			else 
			{
				UI_LOG_WARN(_T("%s unknow align type:%s"), FUNC_NAME, strType.c_str());
			}
		}
		
		mapAttrib.erase(iter);
	}

	strAttr = strPrefix + XML_TEXTRENDER_PADDING;
	iter = mapAttrib.find(strAttr);
	if (mapAttrib.end() != iter)
	{
		const String& str = iter->second;
		Util::TranslateRECT(str, &m_rPadding, XML_SEPARATOR);
		mapAttrib.erase(iter);
	}
	return true;
}
SIZE TextRenderBase::GetDesiredSize(const String& strText, int nLimitWidth)
{
	SIZE s = {0,0};
	HRFONT hRFont = this->GetHRFONT();
	if( NULL != hRFont )
	{
		s = ::MeasureString(hRFont,strText.c_str(),nLimitWidth );
	}

	s.cx += m_rPadding.left + m_rPadding.right;
	s.cy += m_rPadding.bottom + m_rPadding.top;
	return s;
}

TextRenderBase* TextRenderFactory::GetTextRender( const String& strType, Object* pObj )
{
	TextRenderBase*  pTextRender = NULL;

	if( XML_TEXTRENDER_TYPE_NORMAL == strType )
	{
		pTextRender = new TextRender();
		pTextRender->SetTextRenderType( TEXTRENDER_TYPE_NORMAL );
	}
	if( XML_TEXTRENDER_FONTCOLORLIST_COUNT == strType )
	{
		pTextRender = new FontColorListTextRender();
		pTextRender->SetTextRenderType( TEXTRENDER_TYPE_FONTCOLORLIST );
	}
	else
	{
		UI_LOG_WARN(_T("%s invalid render type %s"), _T(__FUNCTION__), strType.c_str() );
	}

	if( NULL != pTextRender )
	{
		pTextRender->SetObject(pObj);
	}

	return pTextRender;
}
TextRenderBase* TextRenderFactory::GetTextRender( TEXTRENDER_TYPE eType, Object* pObj )
{
	TextRenderBase*  pTextRender = NULL;

	if( TEXTRENDER_TYPE_NORMAL == eType )
	{
		pTextRender = new TextRender();
	}
	else if( TEXTRENDER_TYPE_FONTCOLORLIST == eType )
	{
		pTextRender = new FontColorListTextRender();
	}
	else if (TEXTRENDER_TYPE_COLORLIST == eType)
	{
		pTextRender = new ColorListTextRender();
	}
	else
	{
		UI_LOG_WARN(_T("%s invalid render type %d"), _T(__FUNCTION__),  eType );
	}

	if( NULL != pTextRender )
	{
		pTextRender->SetTextRenderType(eType);
		pTextRender->SetObject(pObj);
	}

	return pTextRender;
}



TextRender::TextRender()
{
	m_pColorText = NULL;
	m_hFont = NULL;
}
TextRender::~TextRender()
{
	SAFE_RELEASE(m_pColorText);
	if( NULL != m_hFont )
	{
		UI_ReleaseFont(m_hFont);
		m_hFont = NULL;
	}
}

bool TextRender::SetAttribute( const String& strPrefix, ATTRMAP& mapAttrib )
{
	if (false == __super::SetAttribute(strPrefix, mapAttrib))
		return false;

	// 颜色
	String strAttrib = strPrefix + XML_TEXTRENDER_COLOR;
	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		UI_GetColor( iter->second, &this->m_pColorText );
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_TEXTRENDER_FONT;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		m_hFont = UI_GetFont(iter->second, ::GetGraphicsRenderType(m_pObject) );
		mapAttrib.erase(iter);
	}
	if( NULL == m_hFont )
	{
		HRFONT hRFont = m_pObject->GetFont();
		if (NULL != hRFont)
		{
			m_hFont = UI_CopyFont(m_pObject->GetFont());
		}
		else
		{
			// 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
			m_hFont = UI_GetDefaultFont(GetGraphicsRenderType(m_pObject));
		}
	}
	return true;
}

void TextRender::SetHRFont(HRFONT hRFont)
{
	if( NULL != m_hFont )
	{
		UI_ReleaseFont(m_hFont);
		m_hFont = NULL;
	}
	if (NULL != hRFont)
	{
		m_hFont = UI_CopyFont(hRFont);
	}
}

void TextRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState, const String& strText, int nDrawTextFlag)
{
	if (NULL != m_hFont && strText.length() > 0)
	{
		COLORREF col = RGB(0,0,0);
		if( NULL != m_pColorText )
		{
			col = m_pColorText->GetColor();
		}

		CRect rcText(prc);
		rcText.DeflateRect(m_rPadding.left, m_rPadding.top, m_rPadding.right, m_rPadding.bottom);

		int nFlag = nDrawTextFlag==-1 ? m_nDrawTextFlag:nDrawTextFlag;
		DrawString( pRenderTarget, strText.c_str(), &rcText, nFlag, m_hFont, col );
	}
}

ColorListTextRender::ColorListTextRender()
{
	m_nCount = 0;
	m_hFont = NULL;
}
ColorListTextRender::~ColorListTextRender()
{
	this->Clear();
	if (NULL != m_hFont)
	{
		::UI_ReleaseFont(m_hFont);
		m_hFont = NULL;
	}
}
void ColorListTextRender::Clear()
{
	for (int i = 0; i < m_nCount; i++)
	{
		SAFE_RELEASE(m_vTextColor[i]);
	}
	m_vTextColor.clear();
	m_nCount = 0;
}
bool ColorListTextRender::SetAttribute( const String& strPrefix, map<String,String>& mapAttrib )
{
	bool bRet = __super::SetAttribute(strPrefix, mapAttrib);
	if (false == bRet)
		return false;

	String strAttrib = strPrefix + XML_TEXTRENDER_COLORLIST_COUNT;
	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		this->SetCount( _ttoi(iter->second.c_str()) );
		mapAttrib.erase(iter);
	}

	if ( 0 == m_nCount )
		return true;

	strAttrib = strPrefix + XML_TEXTRENDER_COLOR;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		String& strColors = iter->second;

		vector<String> vColors;
		UI_Split(strColors, XML_MULTI_SEPARATOR, vColors);
		int nCount = (int)vColors.size();

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vColors[i].empty() )
			{
				SAFE_RELEASE(m_vTextColor[i]);
				::UI_GetColor(vColors[i], &m_vTextColor[i]);
			}
		}
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_TEXTRENDER_FONT;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		String strFont = iter->second;

		m_hFont = UI_GetFont(strFont, ::GetGraphicsRenderType(m_pObject->GetHWND()) );
		mapAttrib.erase(iter);
	}

	if( NULL == m_hFont )
	{
		HRFONT hRFont = m_pObject->GetFont();
		if (NULL != hRFont)
		{
			m_hFont = UI_CopyFont(m_pObject->GetFont());
		}
		else
		{
			// 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
			m_hFont = UI_GetDefaultFont(GetGraphicsRenderType(m_pObject));
		}
	}

	return true;
}
void ColorListTextRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState, const String& strText, int nDrawTextFlag)
{
	nState = LOWORD(nState);
	int nRealState = nState;
	if( nState >= m_nCount )
	{
		nRealState = 0;
	}

	if( nRealState >= m_nCount )
		return;

	COLORREF col = RGB(0,0,0);
	if( NULL != m_vTextColor[nRealState] )
		col = m_vTextColor[nRealState]->GetColor();

	CRect rcText(prc);
	rcText.DeflateRect(m_rPadding.left, m_rPadding.top, m_rPadding.right, m_rPadding.bottom);
	int nFlag = nDrawTextFlag==-1 ? m_nDrawTextFlag:nDrawTextFlag;
	DrawString(pRenderTarget, strText.c_str(), &rcText, nFlag, m_hFont, col );
}
HRFONT ColorListTextRender::GetHRFONT()
{
	return m_hFont;
}
void ColorListTextRender::SetHRFont(HRFONT hRFont)
{
	if( NULL != m_hFont )
	{
		UI_ReleaseFont(m_hFont);
		m_hFont = NULL;
	}
	if (NULL != hRFont)
	{
		m_hFont = UI_CopyFont(hRFont);
	}
}

void ColorListTextRender::SetCount( int nCount )
{
	this->Clear();
	m_nCount = nCount;

	for (int i = 0; i < m_nCount; i++)
	{
		m_vTextColor.push_back(NULL);
	}
}
void ColorListTextRender::SetColor( int nIndex, COLORREF col )
{
	if( nIndex >= m_nCount )
		return;

	if( NULL != m_vTextColor[nIndex] )
	{
		SAFE_DELETE(m_vTextColor[nIndex]);
	}
	UIColor::CreateInstance(col, &m_vTextColor[nIndex]);
	m_vTextColor[nIndex]->AddRef();
}

FontColorListTextRender::FontColorListTextRender()
{
	m_nCount = 0;
}
FontColorListTextRender::~FontColorListTextRender()
{
	this->Clear();
}
void FontColorListTextRender::Clear()
{
	for (int i = 0; i < m_nCount; i++)
	{
		SAFE_RELEASE(m_vTextColor[i]);
		if( NULL != m_vTextFont[i] )
		{
			UI_ReleaseFont(m_vTextFont[i]);
			m_vTextFont[i] = NULL;
		}
	}
	m_vTextFont.clear();
	m_vTextColor.clear();
	m_nCount = 0;
}

bool FontColorListTextRender::SetAttribute( const String& strPrefix, map<String,String>& mapAttrib )
{
	bool bRet = __super::SetAttribute(strPrefix, mapAttrib);
	if (false == bRet)
		return false;

	String strAttrib = strPrefix + XML_TEXTRENDER_FONTCOLORLIST_COUNT;
	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		this->SetCount( _ttoi(iter->second.c_str()) );
		mapAttrib.erase(iter);
	}

	if ( 0 == m_nCount )
		return true;

	strAttrib = strPrefix + XML_TEXTRENDER_COLOR;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		String& strColors = iter->second;

		vector<String> vColors;
		UI_Split(strColors, XML_MULTI_SEPARATOR, vColors);
		int nCount = (int)vColors.size();

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vColors[i].empty() )
			{
				::UI_GetColor(vColors[i], &m_vTextColor[i]);
			}
		}
		mapAttrib.erase(iter);
	}

	strAttrib = strPrefix + XML_TEXTRENDER_FONT;
	iter = mapAttrib.find(strAttrib);
	if (mapAttrib.end() != iter)
	{
		String& strFonts = iter->second;

		vector<String> vFont;
		UI_Split(strFonts, XML_MULTI_SEPARATOR, vFont);
		int nCount = (int)vFont.size();

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vFont[i].empty() )
			{
				m_vTextFont[i] = UI_GetFont(vFont[i], ::GetGraphicsRenderType(m_pObject->GetHWND()) );
			}
		}
		mapAttrib.erase(iter);
	}

	for(int i = 0; i < m_nCount; i++ )
	{
		if( NULL == m_vTextFont[i] )
		{
			m_vTextFont[i] = UI_CopyFont(m_pObject->GetFont());
		}
	}

	return true;
}
void FontColorListTextRender::DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState, const String& strText, int nDrawTextFlag)
{
	nState = LOWORD(nState);
	int nRealState = nState;
	if( nState >= m_nCount )
	{
		nRealState = 0;
	}

	if( nRealState >= m_nCount )
		return;

	COLORREF col = RGB(0,0,0);
	if( NULL != m_vTextColor[nRealState] )
		col = m_vTextColor[nRealState]->GetColor();

	HRFONT hRFont = m_vTextFont[nRealState];

	int nFlag = nDrawTextFlag==-1 ? m_nDrawTextFlag:nDrawTextFlag;
	DrawString(pRenderTarget, strText.c_str(), prc, nFlag, hRFont, col );
}
HRFONT FontColorListTextRender::GetHRFONT()
{
	if( m_vTextFont.size() > 0 )
	{
		return m_vTextFont[0];
	}
	return NULL;
}
void FontColorListTextRender::SetHRFont(HRFONT hRFont)
{
	if (m_vTextFont.size() > 0)
	{
		if( NULL != m_vTextFont[0] )
		{
			UI_ReleaseFont(m_vTextFont[0]);
			m_vTextFont[0] = NULL;
		}
		if (NULL != hRFont)
		{
			m_vTextFont[0] = UI_CopyFont(hRFont);
		}
	}
}

void FontColorListTextRender::SetCount( int nCount )
{
	this->Clear();
	m_nCount = nCount;

	for (int i = 0; i < m_nCount; i++)
	{
		m_vTextColor.push_back(NULL);
		m_vTextFont.push_back(NULL);
	}
}
void FontColorListTextRender::SetColor( int nIndex, COLORREF col )
{
	if( nIndex >= m_nCount )
		return;

	if( NULL != m_vTextColor[nIndex] )
	{
		SAFE_DELETE(m_vTextColor[nIndex]);
	}
	UIColor::CreateInstance(col, &m_vTextColor[nIndex]);
	m_vTextColor[nIndex]->AddRef();
}
void FontColorListTextRender::SetFont( int nIndex, HRFONT hRFONG )
{
	if( nIndex >= m_nCount )
		return;

	m_vTextFont[nIndex] = UI_CopyFont(hRFONG);
}