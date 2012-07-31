#include "StdAfx.h"



//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             字体                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////




FontManager::FontManager(void)
{
	m_pFontParse = NULL;
}

FontManager::~FontManager(void)
{
	this->Clear();
	if( NULL!=m_pFontParse)
	{
		m_pFontParse->Release();
		m_pFontParse = NULL;
	}
}

bool FontManager::Create( const String& strXmlPath )
{
	if( NULL != m_pFontParse )
	{
		UI_LOG_WARN(_T("FontManager::Create font parse already exist."));
		return false;
	}

	m_pFontParse = IParseClassFactory::CreateFontParseInstance( strXmlPath );
	if( false == m_pFontParse->Create() )
	{
		UI_LOG_ERROR(_T("FontManager::Create create font parse failed. path=%s"), strXmlPath.c_str());
		return false;
	}

	return true;
}
bool FontManager::Load( const String& strXmlPath )
{
	if( NULL != m_pFontParse )
	{
		UI_LOG_WARN(_T("FontManager::Load font parse already exist."));
		return false;
	}
	m_pFontParse = IParseClassFactory::CreateFontParseInstance( strXmlPath );

	if( false == m_pFontParse->Load(&m_pojoFont) )
		return false;

#if 0 // TODO unimple
	this->Clear();

	// 从xml中解析出所有的图片列表
	FontXmlParse  parse;
	if( false == parse.LoadXml( strXmlPath ) )
		return false;

	vector<FontItem>  vItems;

	if( false == parse.ParseFontItems( vItems ) )
		return false;

	// 解析xml数据,保存
	size_t  size = vItems.size();
	for ( size_t i = 0; i < size; i++ )
	{

		String  strID = vItems[i].strID;
		
		int nWeight = FW_NORMAL;
		if( vItems[i].bBold )
			nWeight = FW_BOLD;

		HFONT   hFont = ::CreateFont(
			vItems[i].nHeight,            // nHeight
			0,                            // nWidth
			0,                            // nEscapement
			vItems[i].nOrientation,       // nOrientation
			nWeight,                      // nWeight
			vItems[i].bItalic,            // bItalic
			vItems[i].bUnderline,         // bUnderline
			vItems[i].bStrikeout,         // cStrikeOut
			ANSI_CHARSET,                 // nCharSet
			OUT_DEFAULT_PRECIS,           // nOutPrecision
			CLIP_DEFAULT_PRECIS,          // nClipPrecision
			DEFAULT_QUALITY,              // nQuality
			DEFAULT_PITCH | FF_SWISS,     // nPitchAndFamily
			vItems[i].strFacename.c_str() // lpszFacename
			);                 
		if( NULL == hFont )
		{
			UI_LOG_ERROR( _T("FontManager::Load, CreateFont(%s) return NULL"), vItems[i].strFacename.c_str() );
			continue;
		}
		 
		this->m_map.insert( make_pair(strID, hFont) );
	}
#endif
	return true;
}

bool FontManager::Reload()
{
	if( NULL == m_pFontParse )
	{
		UI_LOG_ERROR(_T("FontManager::Reload m_pFontParse == NULL."));
		return false;
	}

	this->Clear();
	return  m_pFontParse->Reload( &m_pojoFont );
}
bool FontManager::Save()
{
	assert( NULL != m_pFontParse );
	if( NULL == m_pFontParse )
		return false;

	return m_pFontParse->Save(&m_pojoFont);
}

/*
**	清除所有图片资源
*/
void FontManager::Clear()
{
	m_pojoFont.Clear();
}


HRFONT FontManager::GetFont( const String& strID, GRAPHICS_RENDER_TYPE eRenderType )
{
	return m_pojoFont.GetFont( strID, eRenderType );
}
HRFONT FontManager::GetDefaultFont( GRAPHICS_RENDER_TYPE eRenderType )
{
	return m_pojoFont.GetDefaultFont( eRenderType );
}
HRFONT FontManager::GetFontEx( HRFONT hRFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_TYPE eRenderType )
{
	return m_pojoFont.GetFontEx(hRFont, w, l, eRenderType );
}

int FontManager::GetFontCount()
{
	return m_pojoFont.GetFontCount();
}
bool FontManager::GetFontItemInfo( int nIndex, IFontItemInfo** ppFontItemInfo )
{
	CPojo_FontItem* pItem = m_pojoFont.GetFontItem(nIndex);
	if( NULL == pItem )
		return false;
	
	*ppFontItemInfo = (IFontItemInfo*)pItem;
	return true;
}

void FontManager::SetFontParse( IFontParse* p )
{
	if ( NULL != m_pFontParse )
	{
		m_pFontParse->Release();
		m_pFontParse = NULL;
	}

	m_pFontParse = p;
}

bool FontManager::InsertFontItem( const String& strID, LOGFONT* pLogFont )
{
	if( NULL == m_pFontParse )
		return false;

	if( false == m_pojoFont.InsertFont(strID,pLogFont) )
	{
		UI_LOG_ERROR(_T("FontManager::InsertFontItem  m_pojoFont.InsertFont strID=%s Failed. "), strID.c_str() );
		return false;
	}

	return m_pFontParse->InsertFont( this->m_pojoFont.GetFontItem(strID) );
}
bool FontManager::ModifyFontItem( const String& strID, LOGFONT* pLogFont )
{
	if( NULL == m_pFontParse )
		return false;

	if( false == m_pojoFont.ModifyFont(strID,pLogFont) )
	{
		UI_LOG_ERROR(_T("FontManager::ModifyFontItem  m_pojoFont.ModifyFont strID=%s Failed. "), strID.c_str() );
		return false;
	}

	return m_pFontParse->ModifyFont( this->m_pojoFont.GetFontItem(strID) );
}
bool FontManager::RemoveFontItem( const String& strID )
{
	if( NULL == m_pFontParse )
		return false;

	if( false == m_pojoFont.RemoveFont(strID) )
	{
		UI_LOG_ERROR(_T("FontManager::RemoveFontItem  m_pojoFont.RemoveFont strID=%s Failed. "), strID.c_str() );
		return false;
	}

	return m_pFontParse->RemoveFont( strID );
}