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
#if 0
	RenderBase*  pRender = NULL; 

	if( XML_RENDER_TYPE_COLOR == strType )
	{
		pRender = new ColorRender();
		pRender->SetRenderType( RENDER_TYPE_COLOR );
	}

	else if( XML_RENDER_TYPE_GRADIENT_H == strType )
	{
		pRender = new GradientRender();
		pRender->SetRenderType( RENDER_TYPE_GRADIENTH );
	}
	else if( XML_RENDER_TYPE_GRADIENT_V == strType )
	{
		pRender = new GradientRender();
		pRender->SetRenderType( RENDER_TYPE_GRADIENTV );
	}
	else if( XML_RENDER_TYPE_IMAGE_SIMPLE == strType )
	{
		pRender = new SimpleImageRender();
		pRender->SetRenderType( RENDER_TYPE_IMAGESIMPLE );
	}
	else if( XML_RENDER_TYPE_IMAGE_STRETCH == strType )
	{
		pRender = new StretchImageRender();
		pRender->SetRenderType( RENDER_TYPE_IMAGESTRETCH );
	}
	else if( XML_RENDER_TYPE_IMAGE_TILE == strType )
	{
		pRender = new TileImageRender();
		pRender->SetRenderType( RENDER_TYPE_IMAGETILE );
	}
	else if( XML_RENDER_TYPE_COLORLIST == strType )
	{
		pRender = new ColorListRender();
		pRender->SetRenderType( RENDER_TYPE_COLORLIST );
	}
	else if( XML_RENDER_TYPE_IMAGELIST == strType )
	{
		pRender = new ImageListRender();
		pRender->SetRenderType( RENDER_TYPE_IMAGELIST );
	}
	else if( XML_RENDER_TYPE_IMAGELISTSTRETCH == strType )
	{
		pRender = new ImageListStretchRender();
		pRender->SetRenderType( RENDER_TYPE_IMAGELISTSTRETCH );
	}
	else if( XML_RENDER_TYPE_THEME == strType )
	{
		if( _T("Button") == pObj->GetObjectName() )
		{
			pRender = new ButtonBkThemeRender();
			pRender->SetRenderType( RENDER_TYPE_THEME );
		}
		else if( _T("Edit") == pObj->GetObjectName() )
		{
			pRender = new EditBkThemeRender();
			pRender->SetRenderType( RENDER_TYPE_THEME );
		}
		else if( _T("GroupBox") == pObj->GetObjectName() )
		{
			pRender = new GroupBoxBkThemeRender();
			pRender->SetRenderType( RENDER_TYPE_THEME );
		}
	}
	else
	{
		UI_LOG_WARN(_T("%s invalid render type %s"), _T(__FUNCTION__), strType.c_str() );
	}

	if( NULL != pRender )
	{
		pRender->SetObject(pObj);
	}

	return pRender;
#else
	RENDER_TYPE eType = RENDER_TYPE_THEME;

	if( XML_RENDER_TYPE_COLOR == strType )
		eType = RENDER_TYPE_COLOR;

	else if( XML_RENDER_TYPE_GRADIENT_H == strType )
		eType = RENDER_TYPE_GRADIENTH;

	else if( XML_RENDER_TYPE_GRADIENT_V == strType )
		eType = RENDER_TYPE_GRADIENTV;

	else if( XML_RENDER_TYPE_IMAGE_SIMPLE == strType )
		eType = RENDER_TYPE_IMAGESIMPLE;

	else if( XML_RENDER_TYPE_IMAGE_STRETCH == strType )
		eType = RENDER_TYPE_IMAGESTRETCH;

	else if( XML_RENDER_TYPE_IMAGE_TILE == strType )
		eType = RENDER_TYPE_IMAGETILE;

	else if( XML_RENDER_TYPE_COLORLIST == strType )
		eType = RENDER_TYPE_COLORLIST;

	else if( XML_RENDER_TYPE_IMAGELIST == strType )
		eType = RENDER_TYPE_IMAGELIST;

	else if( XML_RENDER_TYPE_IMAGELISTSTRETCH == strType )
		eType = RENDER_TYPE_IMAGELISTSTRETCH;

	else if( XML_RENDER_TYPE_THEME == strType )
		eType = RENDER_TYPE_THEME;

	else
		return NULL;

	return GetRender(eType, pObj);
#endif
}

RenderBase* RenderFactory::GetRender( RENDER_TYPE eType, Object* pObj )
{
	RenderBase*  pRender = NULL;

	if( RENDER_TYPE_COLOR == eType )
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

	else if( RENDER_TYPE_IMAGESIMPLE == eType )
	{
		pRender = new SimpleImageRender();
	}
	else if( RENDER_TYPE_IMAGESTRETCH == eType )
	{
		pRender = new StretchImageRender();
	}
	else if( RENDER_TYPE_IMAGETILE == eType )
	{
		pRender = new TileImageRender();
	}
	else if( RENDER_TYPE_COLORLIST == eType )
	{
		pRender = new ColorListRender();
	}
	else if( RENDER_TYPE_IMAGELIST == eType )
	{
		pRender = new ImageListRender();
	}
	else if( RENDER_TYPE_IMAGELISTSTRETCH == eType )
	{
		pRender = new ImageListStretchRender();
	}
	else if( RENDER_TYPE_THEME == eType )
	{
		if( _T("Button") == pObj->GetObjectName() )
		{
			Button* pButton = dynamic_cast<Button*>(pObj);
			if (NULL == pButton)
				return NULL;

			if (BUTTON_STYLE_COMBOBOX == pButton->GetButtonStyle())
			{
				pRender = new ComboboxButtonBkThemeRender();
			}
			else
			{
				pRender = new ButtonBkThemeRender();
			}
		}
		else if( _T("RadioButton") == pObj->GetObjectName() )
		{
			pRender = new RadioButtonThemeRender();
		}
		else if( _T("CheckButton") == pObj->GetObjectName() )
		{
			pRender = new CheckButtonThemeRender();
		}
		else if( _T("Edit") == pObj->GetObjectName() )
		{
			pRender = new EditBkThemeRender();
		}
		else if( _T("GroupBox") == pObj->GetObjectName() )
		{
			pRender = new GroupBoxBkThemeRender();
		}
		else if (_T("Combobox") == pObj->GetObjectName() )
		{
			pRender = new ComboboxBkThemeRender();
		}
		else if (_T("ListBox") == pObj->GetObjectName())
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
				}
				else
					pRender = new ListboxBkThemeRender();
			}
		}
	}
	else
	{
		UI_LOG_WARN(_T("%s invalid render type %d"), _T(__FUNCTION__),  eType );
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

bool ColorRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrifix + XML_RENDER_COLOR;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strColID = mapAttrib[strAttrib];
		::UI_GetColor( strColID, &m_pBkColor );
		this->m_pObject->EraseAttribute(strAttrib);
	}

	strAttrib = strPrifix + XML_RENDER_BORDERCOLOR;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strColID = mapAttrib[strAttrib];
		::UI_GetColor( strColID, &m_pBorderColor );
		this->m_pObject->EraseAttribute(strAttrib);
	}

	return true;
} 

void ColorRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	if( NULL == m_pBorderColor )   // 不绘制边框
	{
		if( NULL != m_pBkColor )
		{
			FillRect(hRDC, prc, m_pBkColor->GetColor());
		}
	}
	else                           // 绘制边框
	{
		if( NULL != m_pBkColor )
			Rectangle( hRDC, prc, m_pBorderColor->GetColor(), m_pBkColor->GetColor() );
		else
			Rectangle( hRDC, prc, m_pBorderColor->GetColor(), 0,1,true );
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

bool GradientRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrifix + XML_RENDER_COLORFROM;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strColID = mapAttrib[strAttrib];
		::UI_GetColor( strColID, &m_pColorFrom );
		this->m_pObject->EraseAttribute(strAttrib);
	}

	strAttrib = strPrifix + XML_RENDER_COLORTO;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strColID = mapAttrib[strAttrib];
		::UI_GetColor( strColID, &m_pColorTo );
		this->m_pObject->EraseAttribute(strAttrib);
	}

	strAttrib = strPrifix + XML_RENDER_BORDERCOLOR;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strColID = mapAttrib[strAttrib];
		::UI_GetColor( strColID, &m_pBorderColor );
		this->m_pObject->EraseAttribute(strAttrib);
	}
	return true;
} 

void GradientRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	if( NULL == m_pColorFrom || NULL == m_pColorTo)
		return;

	CRect rc(0,0, this->m_pObject->GetWidth(), this->m_pObject->GetHeight());
	if( m_nRenderType == RENDER_TYPE_GRADIENTH )
	{
		GradientFillH( hRDC, &rc, m_pColorFrom->GetColor(), m_pColorTo->GetColor() );
	}
	else if( m_nRenderType == RENDER_TYPE_GRADIENTV )
	{
		GradientFillV( hRDC, &rc, m_pColorFrom->GetColor(), m_pColorTo->GetColor() );
	}

	if( NULL != m_pBorderColor )
	{
		Rectangle( hRDC, prc, m_pBorderColor->GetColor(), 0,1,true );
	}

}


 
//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               SimpleImageRender                            //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


SimpleImageRender::SimpleImageRender()
{
	m_hBitmap = NULL;
	m_pColorBk = NULL;
}
SimpleImageRender::~SimpleImageRender()
{
	if( NULL != m_hBitmap )
	{
		UI_ReleaseBitmap(m_hBitmap);
		m_hBitmap = NULL;
	}
	if( NULL != m_pColorBk )
	{
		m_pColorBk->Release();
		m_pColorBk = NULL;
	}
}

bool SimpleImageRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrifix + XML_RENDER_IMAGE;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strImageID = mapAttrib[strAttrib];
		m_hBitmap = ::UI_GetBitmap( strImageID, ::GetGraphicsRenderType(m_pObject) );
		this->m_pObject->EraseAttribute(strAttrib);
	}

	strAttrib = strPrifix + XML_RENDER_COLOR;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strColID = mapAttrib[strAttrib];
		::UI_GetColor( strColID, &m_pColorBk );
		this->m_pObject->EraseAttribute(strAttrib);
	}
	return true;
} 

void SimpleImageRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	if( NULL != m_pColorBk )
	{
		FillRect(hRDC, prc, m_pColorBk->GetColor() );
	}
	if( NULL != m_hBitmap )
	{
		DrawBitmap(hRDC,m_hBitmap,prc->left,prc->top);
	}
}
SIZE SimpleImageRender::GetDesiredSize()
{
	SIZE s = {0,0};
	if( NULL == m_hBitmap )
		return s;

	s.cx = UI_GetBitmapWidth(m_hBitmap);
	s.cy = UI_GetBitmapHeight(m_hBitmap);
	return s;
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               StretchImageRender                            //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


StretchImageRender::StretchImageRender()
{
	m_pColorBk = NULL;
	m_hBitmap = NULL;
	m_region.Set(0);
}
StretchImageRender::~StretchImageRender()
{
	if( NULL != m_hBitmap )
	{
		UI_ReleaseBitmap(m_hBitmap);
		m_hBitmap = NULL;
	}
	SAFE_RELEASE(m_pColorBk);
}

bool StretchImageRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrifix + XML_RENDER_IMAGE;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strImageID = mapAttrib[strAttrib];
		m_hBitmap = ::UI_GetBitmap( strImageID, ::GetGraphicsRenderType(m_pObject) );
		this->m_pObject->EraseAttribute(strAttrib);
	}

	strAttrib = strPrifix + XML_RENDER_IMAGE9REGION;
	if( mapAttrib.count( strAttrib ))
	{
		const String& str9Region = mapAttrib[strAttrib];
		Util::TranslateImage9Region( str9Region, (void*)&m_region );
		this->m_pObject->EraseAttribute(strAttrib);
	}

	strAttrib = strPrifix + XML_RENDER_COLOR;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strColID = mapAttrib[strAttrib];
		::UI_GetColor( strColID, &m_pColorBk );
		this->m_pObject->EraseAttribute(strAttrib);
	}
	return true;
} 

void StretchImageRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	if( NULL != m_pColorBk )
	{
		FillRect(hRDC, prc, m_pColorBk->GetColor() );
	}
	if( NULL != m_hBitmap )
	{
		DrawBitmap(hRDC, m_hBitmap, prc->left,prc->top,prc->Width(), prc->Height(), 
			0,0, UI_GetBitmapWidth(m_hBitmap), UI_GetBitmapHeight(m_hBitmap), &m_region );
	}
}

SIZE StretchImageRender::GetDesiredSize()
{
	SIZE s = {0,0};
	if( NULL == m_hBitmap )
		return s;

	s.cx = UI_GetBitmapWidth(m_hBitmap);
	s.cy = UI_GetBitmapHeight(m_hBitmap);
	return s;
}


//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               TileImageRender                                        //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


TileImageRender::TileImageRender()
{
	m_hBitmap = NULL;
}
TileImageRender::~TileImageRender()
{
	if( NULL != m_hBitmap )
	{
		UI_ReleaseBitmap(m_hBitmap);
		m_hBitmap = NULL;
	}
}

bool TileImageRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrifix + XML_RENDER_IMAGE;
	if( mapAttrib.count( strAttrib ))
	{
		const String& strImageID = mapAttrib[strAttrib];
		m_hBitmap = ::UI_GetBitmap( strImageID, ::GetGraphicsRenderType(m_pObject) );
		this->m_pObject->EraseAttribute(strAttrib);
	}

	return true;
} 

void TileImageRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	if( NULL != m_hBitmap )
	{
		TileRect(hRDC, prc, m_hBitmap);
	}
}

SIZE TileImageRender::GetDesiredSize()
{
	SIZE s = {0,0};
	if( NULL == m_hBitmap )
		return s;

	s.cx = UI_GetBitmapWidth(m_hBitmap);
	s.cy = UI_GetBitmapHeight(m_hBitmap);
	return s;
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

bool ColorListRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrifix + XML_RENDER_COLORLIST_COUNT;
	if( mapAttrib.count(strAttrib) )
	{
		this->Clear();

		m_nCount = _ttoi(mapAttrib[strAttrib].c_str());

		for(int i = 0; i < m_nCount; i++ )
		{
			m_vBkColor.push_back((UIColor*)NULL);
			m_vBorderColor.push_back((UIColor*)NULL);
		}

		m_pObject->EraseAttribute(strAttrib);
	}

	if ( 0 == m_nCount )
		return true;

	strAttrib = strPrifix + XML_RENDER_COLOR;
	if( mapAttrib.count(strAttrib) )
	{
		String strColors = mapAttrib[strAttrib];

		vector<String> vColors;
		UI_Split(strColors, XML_MULTI_SEPERATOR, vColors);
		int nCount = (int)vColors.size();

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vColors[i].empty() )
			{
				::UI_GetColor(vColors[i], &m_vBkColor[i]);
			}
		}

		m_pObject->EraseAttribute(strAttrib);
	}

	strAttrib = strPrifix + XML_RENDER_BORDERCOLOR;
	if( mapAttrib.count(strAttrib) )
	{
		String strColors = mapAttrib[strAttrib];

		vector<String> vColors;
		UI_Split(strColors, XML_MULTI_SEPERATOR, vColors);
		int nCount = (int)vColors.size();

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vColors[i].empty() )
			{
				::UI_GetColor(vColors[i], &m_vBorderColor[i]);
			}
		}

		m_pObject->EraseAttribute(strAttrib);
	}

	return true;
}
void ColorListRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
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
			FillRect(hRDC, prc, m_vBkColor[nRealState]->GetColor());
		}
	}
	else                           // 绘制边框
	{
		if( NULL != m_vBkColor[nRealState] )
			Rectangle( hRDC, prc, m_vBorderColor[nRealState]->GetColor(), m_vBkColor[nRealState]->GetColor() );
		else
			Rectangle( hRDC, prc, m_vBorderColor[nRealState]->GetColor(), 0,1,true );
	}
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
}
ImageListRender::~ImageListRender( )
{
	if( NULL != m_hBitmap )
	{
		::UI_ReleaseBitmap(m_hBitmap);
		m_hBitmap = NULL;
	}
}

bool ImageListRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrifix + XML_RENDER_IMAGELIST_COUNT;
	if( mapAttrib.count(strAttrib) )
	{
		m_nCount = _ttoi(mapAttrib[strAttrib].c_str());
		m_pObject->EraseAttribute(strAttrib);
	}

	if ( 0 == m_nCount )
		return true;

	strAttrib = strPrifix + XML_RENDER_IMAGE;
	if( 0 != mapAttrib.count(strAttrib) )
	{
		if( NULL != m_hBitmap )
		{
			::UI_ReleaseBitmap(m_hBitmap);
			m_hBitmap = NULL;
		}
		m_hBitmap = ::UI_GetBitmap( mapAttrib[strAttrib], ::GetGraphicsRenderType(m_pObject->GetHWND()) );

		this->m_pObject->EraseAttribute(strAttrib);
	}
	if( NULL == m_hBitmap )
		return true;

	strAttrib = strPrifix + XML_RENDER_IMAGELIST_LAYOUT;
	if( 0 != mapAttrib.count(strAttrib) )
	{
		String str = mapAttrib[strAttrib];
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
		this->m_pObject->EraseAttribute(strAttrib);
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
	return true;
}
void ImageListRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	int nRealState = nState;
	if( nState >= m_nCount )
	{
		nRealState = 0;
	}

	if( nRealState >= m_nCount )
		return;

	if( NULL != m_hBitmap )
	{
		if( m_eImageLayout == IMAGELIST_LAYOUT_TYPE_H )
		{
			DrawBitmap(hRDC, m_hBitmap, prc->left,prc->top,m_nItemWidth,m_nItemHeight, nRealState*m_nItemWidth,0,m_nItemWidth,m_nItemHeight );
		}
		else
		{
			DrawBitmap(hRDC, m_hBitmap, prc->left,prc->top,m_nItemWidth,m_nItemHeight, 0, nRealState*m_nItemHeight,m_nItemWidth,m_nItemHeight );
		}
	}
}
SIZE ImageListRender::GetDesiredSize()
{
	SIZE s = {m_nItemWidth, m_nItemHeight};
	return s;
}

//////////////////////////////////////////////////////////////////////////

bool ImageListStretchRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	bool  bRet = ImageListRender::SetAttribute(strPrifix, mapAttrib);
	if( false == bRet )
		return false;

	String strAttrib = strPrifix + XML_RENDER_IMAGE9REGION;
	if( mapAttrib.count( strAttrib ))
	{
		const String& str9Region = mapAttrib[strAttrib];
		Util::TranslateImage9Region( str9Region, (void*)&m_9Region );
		this->m_pObject->EraseAttribute(strAttrib);
	}
	
	return true;
}
void ImageListStretchRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	int nRealState = nState;
	if( nState >= m_nCount )
	{
		nRealState = 0;
	}

	if( nRealState >= m_nCount )
		return;

	if( NULL != m_hBitmap )
	{
		if( m_eImageLayout == IMAGELIST_LAYOUT_TYPE_H )
		{
			DrawBitmap(hRDC, m_hBitmap, prc->left,prc->top,prc->Width(),prc->Height(), 
				nRealState*m_nItemWidth,0,m_nItemWidth,m_nItemHeight,
				&m_9Region);
		}
		else
		{
			DrawBitmap(hRDC, m_hBitmap, prc->left,prc->top,prc->Width(),prc->Height(), 
				0, nRealState*m_nItemHeight,m_nItemWidth,m_nItemHeight,
				&m_9Region);
		}
	}
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

void ButtonBkThemeRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(hRDC, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(hRDC, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(hRDC, (CRect*)prc);
		break;;

	default:
		this->DrawNormal(hRDC, (CRect*)prc);
		break;
	}
}

void ButtonBkThemeRender::DrawDisable( HRDC hRDC, const CRect* prc  )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, PBS_DISABLED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_INACTIVE );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonBkThemeRender::DrawNormal( HRDC hRDC, const CRect* prc  )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, ((ButtonBase*)m_pObject)->IsDefault()?PBS_DEFAULTED:PBS_NORMAL, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH );
	}
	ReleaseHDC(hRDC, hDC);
}

void ButtonBkThemeRender::DrawHover( HRDC hRDC, const CRect* prc  )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, PBS_HOT, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_HOT );
	}
	ReleaseHDC(hRDC, hDC);
}
void ButtonBkThemeRender::DrawPress( HRDC hRDC, const CRect* prc  )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, BP_PUSHBUTTON, PBS_PRESSED, prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawFrameControl(hDC, (RECT*)prc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED );
	}
	ReleaseHDC(hRDC, hDC);
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
void RadioButtonThemeRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_ICON_RENDER_STATE_NORMAL:
		this->DrawNormal(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_HOVER:
		this->DrawHover(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_PRESS:
		this->DrawPress(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_DISABLE:
		this->DrawDisable(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:
		this->DrawCheckNormal(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:
		this->DrawCheckHover(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:
		this->DrawCheckPress(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:
		this->DrawCheckDisable(hRDC, prc);
		break;
	}
}

void RadioButtonThemeRender::DrawDisable( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void RadioButtonThemeRender::DrawNormal( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void RadioButtonThemeRender::DrawHover( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void RadioButtonThemeRender::DrawPress( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}

void RadioButtonThemeRender::DrawCheckDisable( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void RadioButtonThemeRender::DrawCheckNormal( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void RadioButtonThemeRender::DrawCheckHover( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void RadioButtonThemeRender::DrawCheckPress( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
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

void CheckButtonThemeRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_ICON_RENDER_STATE_NORMAL:
		this->DrawNormal(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_HOVER:
		this->DrawHover(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_PRESS:
		this->DrawPress(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_DISABLE:
		this->DrawDisable(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL:
		this->DrawCheckNormal(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_HOVER:
		this->DrawCheckHover(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_PRESS:
		this->DrawCheckPress(hRDC, prc);
		break;
	case BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE:
		this->DrawCheckDisable(hRDC, prc);
		break;
	}
}

void CheckButtonThemeRender::DrawDisable( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void CheckButtonThemeRender::DrawNormal( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void CheckButtonThemeRender::DrawHover( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void CheckButtonThemeRender::DrawPress( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}



void CheckButtonThemeRender::DrawCheckDisable( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void CheckButtonThemeRender::DrawCheckNormal( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void CheckButtonThemeRender::DrawCheckHover( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void CheckButtonThemeRender::DrawCheckPress( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
//////////////////////////////////////////////////////////////////////////

void EditBkThemeRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	switch(nState)
	{
	case EDIT_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(hRDC, prc);
		break;

	case EDIT_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(hRDC, prc);
		break;

	case EDIT_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(hRDC, prc);
		break;;

	default:
		this->DrawNormal(hRDC, prc);
		break;
	}
}

void EditBkThemeRender::DrawDisable( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void EditBkThemeRender::DrawNormal( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}

void EditBkThemeRender::DrawHover( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}
void EditBkThemeRender::DrawPress( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
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
	ReleaseHDC(hRDC, hDC);
}



//////////////////////////////////////////////////////////////////////////

void GroupBoxBkThemeRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	switch(nState)
	{
	case GROUPBOX_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(hRDC, prc);
		break;
	default:
		this->DrawNormal(hRDC, prc);
		break;
	}
}

void GroupBoxBkThemeRender::DrawDisable( HRDC hRDC, const CRect* prc )
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
void GroupBoxBkThemeRender::DrawNormal( HRDC hRDC, const CRect* prc )
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

// void GroupBoxBkThemeRender::DrawHover( HRDC hRDC, RECT* prc )
// {
// }
// void GroupBoxBkThemeRender::DrawPress( HRDC hRDC, RECT* prc )
// {
// }

SIZE ComboboxButtonBkThemeRender::GetDesiredSize()
{
	SIZE s = {18,0};

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

void ComboboxButtonBkThemeRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	switch(nState)
	{
	case BUTTON_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(hRDC, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(hRDC, (CRect*)prc);
		break;

	case BUTTON_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(hRDC, (CRect*)prc);
		break;;

	default:
		this->DrawNormal(hRDC, (CRect*)prc);
		break;
	}
}

void ComboboxButtonBkThemeRender::DrawDisable( HRDC hRDC, const CRect* prc  )
{
	HDC hDC = GetHDC(hRDC);
	CRect rc = *prc;
	if( m_hTheme )
	{
		rc.DeflateRect(1,1,1,1);
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTON, CBXSR_DISABLED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		rc.DeflateRect(0,2,2,2);
		DrawFrameControl(hDC, (RECT*)&rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_INACTIVE );
		DrawGlyph(hDC, &rc);
	}
	ReleaseHDC(hRDC, hDC);
}
void ComboboxButtonBkThemeRender::DrawNormal( HRDC hRDC, const CRect* prc  )
{
	HDC hDC = GetHDC(hRDC);
	CRect rc = *prc;
	if( m_hTheme )
	{
		rc.DeflateRect(1,1,1,1);
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTON, CBXSR_NORMAL, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		rc.DeflateRect(0,2,2,2);
		DrawFrameControl(hDC, (RECT*)&rc, DFC_BUTTON, DFCS_BUTTONPUSH );
		DrawGlyph(hDC, &rc);
	}
	ReleaseHDC(hRDC, hDC);
}

void ComboboxButtonBkThemeRender::DrawHover( HRDC hRDC, const CRect* prc  )
{
	HDC hDC = GetHDC(hRDC);

	CRect rc = *prc;
	if( m_hTheme )
	{
		rc.DeflateRect(1,1,1,1);
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTON, CBXSR_HOT, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		rc.DeflateRect(0,2,2,2);
		DrawFrameControl(hDC, (RECT*)&rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_HOT );
		DrawGlyph(hDC, &rc);
	}
	ReleaseHDC(hRDC, hDC);
}
void ComboboxButtonBkThemeRender::DrawPress( HRDC hRDC, const CRect* prc  )
{
	HDC hDC = GetHDC(hRDC);
	CRect rc = *prc;
	if( m_hTheme )
	{
		rc.DeflateRect(1,1,1,1);
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_DROPDOWNBUTTON, CBXSR_PRESSED, &rc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("ComboboxButtonBkThemeRender::DrawNormal  DrawThemeBackground failed."));
		}
	}
	else
	{
		rc.DeflateRect(0,2,2,2);
		DrawFrameControl(hDC, (RECT*)&rc, DFC_BUTTON, DFCS_BUTTONPUSH|DFCS_PUSHED|DFCS_FLAT );
		DrawGlyph(hDC, &rc, true);
	}
	ReleaseHDC(hRDC, hDC);
}

// 绘制按钮上的箭头
void ComboboxButtonBkThemeRender::DrawGlyph( HDC hDC, const CRect* prc, bool bPressDown )
{
	POINT pt[4] = 
	{
		{-3,-2}, {3,-2}, {0,1}, {-3,-2}
	};

	int x = 0; int y = 0;
	x = (prc->left + prc->right)/2;
	y = (prc->top + prc->bottom)/2;

	for (int i = 0; i < 4; i++)
	{
		pt[i].x += x;
		pt[i].y += y;

		if(bPressDown)
		{
		//	pt[i].x += 1;
			pt[i].y += 1;
		}
	}

	HBRUSH hBlackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
	HBRUSH hOldBrush = (HBRUSH)::SelectObject(hDC, hBlackBrush);
	::Polygon(hDC, pt, 4);
	::SelectObject(hDC,hOldBrush);
}

//////////////////////////////////////////////////////////////////////////


void ComboboxBkThemeRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	switch(nState)
	{
	case COMBOBOX_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(hRDC, prc);
		break;

	case COMBOBOX_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(hRDC, prc);
		break;

	case COMBOBOX_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(hRDC, prc);
		break;;

	default:
		this->DrawNormal(hRDC, prc);
		break;
	}
}

void ComboboxBkThemeRender::DrawDisable( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_BORDER, CBXS_DISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawDisable  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
	}
	ReleaseHDC(hRDC, hDC);
}
void ComboboxBkThemeRender::DrawNormal( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_BORDER, CBXS_NORMAL, (RECT*)prc, 0);
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
	ReleaseHDC(hRDC, hDC);
}

void ComboboxBkThemeRender::DrawHover( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_BORDER, CBXS_HOT, (RECT*)prc, 0);
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
	ReleaseHDC(hRDC, hDC);
}
void ComboboxBkThemeRender::DrawPress( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, CP_BORDER, CBXS_PRESSED, (RECT*)prc, 0);
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
	ReleaseHDC(hRDC, hDC);
}


//////////////////////////////////////////////////////////////////////////


void ListboxBkThemeRender::DrawState(HRDC hRDC, const CRect* prc, int nState)
{
	switch(nState)
	{
	case LISTBOX_BKGND_RENDER_STATE_DISABLE:
		this->DrawDisable(hRDC, prc);
		break;

	case LISTBOX_BKGND_RENDER_STATE_PRESS:
		this->DrawPress(hRDC, prc);
		break;

	case LISTBOX_BKGND_RENDER_STATE_HOVER:
		this->DrawHover(hRDC, prc);
		break;;

	default:
		this->DrawNormal(hRDC, prc);
		break;
	}
}

void ListboxBkThemeRender::DrawDisable( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, LBCP_BORDER_NOSCROLL, LBPSH_DISABLED, (RECT*)prc, 0);
		if ( S_OK != hr )
		{
			UI_LOG_WARN(_T("EditBkThemeRender::DrawDisable  DrawThemeBackground failed."));
		}
	}
	else
	{
		DrawEdge(hDC, (RECT*)prc, EDGE_SUNKEN, BF_RECT|BF_MIDDLE);
	}
	ReleaseHDC(hRDC, hDC);
}
void ListboxBkThemeRender::DrawNormal( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, LBCP_BORDER_NOSCROLL, LBPSH_NORMAL, (RECT*)prc, 0);
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
	ReleaseHDC(hRDC, hDC);
}

void ListboxBkThemeRender::DrawHover( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, LBCP_BORDER_NOSCROLL, LBPSH_HOT, (RECT*)prc, 0);
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
	ReleaseHDC(hRDC, hDC);
}
void ListboxBkThemeRender::DrawPress( HRDC hRDC, const CRect* prc )
{
	HDC hDC = GetHDC(hRDC);
	if( m_hTheme )
	{
		HRESULT hr = DrawThemeBackground(m_hTheme, hDC, LBCP_BORDER_NOSCROLL, LBPSH_FOCUSED, (RECT*)prc, 0);
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
	ReleaseHDC(hRDC, hDC);
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
	m_nDrawTextFlag = DT_SINGLELINE|DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER;
}
void TextRenderBase::SetTextAlignment(int nDrawFlag)
{
	m_nDrawTextFlag = nDrawFlag;
}

SIZE TextRenderBase::GetDesiredSize(const String& strText, int nLimitWidth)
{
	SIZE s = {0,0};
	HRFONT hRFont = this->GetHRFONT();
	if( NULL != hRFont )
	{
		s = ::MeasureString(hRFont,strText.c_str(),nLimitWidth );
	}
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

bool TextRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	// 颜色
	String strAttrib = strPrifix + XML_TEXTRENDER_COLOR;
	if( mapAttrib.count( strAttrib ) )
	{
		UI_GetColor( mapAttrib[strAttrib], &this->m_pColorText );
		m_pObject->EraseAttribute(strAttrib);
	}

	strAttrib = strPrifix + XML_TEXTRENDER_FONT;
	if( mapAttrib.count( strAttrib ) )
	{
		m_hFont = UI_GetFont(mapAttrib[strAttrib], ::GetGraphicsRenderType(m_pObject->GetHWND()) );
		m_pObject->EraseAttribute(strAttrib);
	}
	if( NULL == m_hFont )
	{
		HRFONT hRFont = m_pObject->GetFont();
		if (NULL != hRFont)
		{
			m_hFont = UI_CopyFont(m_pObject->GetFont());
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

void TextRender::DrawState(HRDC hRDC, const CRect* prc, int nState, const String& strText, int nDrawTextFlag)
{
	if( NULL != m_hFont )
	{
		COLORREF col = RGB(0,0,0);
		if( NULL != m_pColorText )
		{
			col = m_pColorText->GetColor();
		}

		int nFlag = nDrawTextFlag==-1 ? m_nDrawTextFlag:nDrawTextFlag;
		DrawString( hRDC, strText.c_str(), prc, nFlag, m_hFont, col );
	}
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

bool FontColorListTextRender::SetAttribute( const String& strPrifix, map<String,String>& mapAttrib )
{
	String strAttrib = strPrifix + XML_TEXTRENDER_FONTCOLORLIST_COUNT;
	if( mapAttrib.count( strAttrib ) )
	{
		this->SetCount( _ttoi( mapAttrib[strAttrib].c_str()) );
		m_pObject->EraseAttribute(strAttrib);
	}

	if ( 0 == m_nCount )
		return true;

	strAttrib = strPrifix + XML_TEXTRENDER_COLOR;
	if( mapAttrib.count(strAttrib) )
	{
		String strColors = mapAttrib[strAttrib];

		vector<String> vColors;
		UI_Split(strColors, XML_MULTI_SEPERATOR, vColors);
		int nCount = (int)vColors.size();

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vColors[i].empty() )
			{
				::UI_GetColor(vColors[i], &m_vTextColor[i]);
			}
		}
		m_pObject->EraseAttribute(strAttrib);
	}

	strAttrib = strPrifix + XML_TEXTRENDER_FONT;
	if( mapAttrib.count(strAttrib) )
	{
		String strFonts = mapAttrib[strAttrib];

		vector<String> vFont;
		UI_Split(strFonts, XML_MULTI_SEPERATOR, vFont);
		int nCount = (int)vFont.size();

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vFont[i].empty() )
			{
				m_vTextFont[i] = UI_GetFont(vFont[i], ::GetGraphicsRenderType(m_pObject->GetHWND()) );
			}
		}
		m_pObject->EraseAttribute(strAttrib);
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
void FontColorListTextRender::DrawState(HRDC hRDC, const CRect* prc, int nState, const String& strText, int nDrawTextFlag)
{
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
	DrawString(hRDC, strText.c_str(), prc, nFlag, hRFont, col );
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