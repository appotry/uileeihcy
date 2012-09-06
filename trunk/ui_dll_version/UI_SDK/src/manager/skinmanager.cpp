#include "StdAfx.h"


//////////////////////////////////////////////////////////////////////////

SkinInfoManager::SkinInfoManager()
{
	m_pSkinInfoParse = NULL;
}
SkinInfoManager::~SkinInfoManager()
{
	if( m_pSkinInfoParse != NULL )
	{
		m_pSkinInfoParse->Release(); 
		m_pSkinInfoParse = NULL;
	}
}
void SkinInfoManager::SetSkinInfoParse(ISkinInfoParse* p)
{
	if( m_pSkinInfoParse != NULL )
	{
		m_pSkinInfoParse->Release(); 
		m_pSkinInfoParse = NULL;
	}

	m_pSkinInfoParse = p;
}

bool SkinInfoManager::GetSkinInfo( CPojo_SkinInfo** pRet )
{
	if ( NULL == pRet )
	{
		return false;
	}

	*pRet = &m_skinInfo;
	return true;
}

bool SkinInfoManager::Load(const String& strDataSource )
{
	if( NULL != m_pSkinInfoParse )
		return false;

	m_pSkinInfoParse = IParseClassFactory::CreateSkinInfoParseInstance(strDataSource);
	return m_pSkinInfoParse->Load( &m_skinInfo );
}

bool SkinInfoManager::Save()
{
	assert( NULL != m_pSkinInfoParse );
	if( NULL == m_pSkinInfoParse )
		return false;

	return m_pSkinInfoParse->Save(&m_skinInfo);
}

//////////////////////////////////////////////////////////////////////////
//
//                             Skin Manager
//
//////////////////////////////////////////////////////////////////////////


//
//	pSkinItem中记录了该皮肤的名称以及该皮肤的路径
//
SkinManager::SkinManager(CPojo_ProjectSkinItem* pSkinItem)
{	
	m_pSkinInfoMgr = new SkinInfoManager();

	m_pPojoSkinItem = pSkinItem;

	m_pImageMgr = NULL;
	m_pColorMgr = NULL;
	m_pFontMgr  = NULL;
	m_pStyleMgr = NULL;
	m_pLayoutMgr= NULL;
}
SkinManager::~SkinManager()
{
	m_pPojoSkinItem = NULL;

	if( NULL != m_pSkinInfoMgr )
	{
		delete m_pSkinInfoMgr;
		m_pSkinInfoMgr = NULL;
	}
	if( NULL != m_pImageMgr )
	{
		delete m_pImageMgr;
		m_pImageMgr = NULL;
	}
	if( NULL != m_pColorMgr )
	{
		delete m_pColorMgr;
		m_pColorMgr = NULL;
	}
	if( NULL != m_pFontMgr )
	{
		delete m_pFontMgr;
		m_pFontMgr = NULL;
	}
	if( NULL != m_pStyleMgr )
	{
		delete m_pStyleMgr;
		m_pStyleMgr = NULL;
	}
	if( NULL != m_pLayoutMgr )
	{
		delete m_pLayoutMgr;
		m_pLayoutMgr = NULL;
	}
}

//
//	加载皮肤中的各种资源信息，如资源配置文件所在路径
//
bool SkinManager::LoadSkinInfo( )
{
	if( NULL == m_pPojoSkinItem || NULL == m_pSkinInfoMgr )
		return false;

	if( false == m_pSkinInfoMgr->Load( m_pPojoSkinItem->GetSkinXmlPath() ) )
	{
		UI_LOG_ERROR(_T("SkinManager::LoadSkinInfo load info failed."));
		return false;
	}

	return true;
}

// 重新从Temp文件中读取数据
bool SkinManager::ReloadResource(UI_RESOURCE_TYPE eResType)
{
	switch(eResType)
	{
	case UI_RESOURCE_IMAGE:
		return m_pImageMgr->Reload();
		break;
	case UI_RESOURCE_COLOR:
		return m_pColorMgr->Reload();
		break;
	case UI_RESOURCE_FONT:
		return m_pFontMgr->Reload();
		break;
	case  UI_RESOURCE_STYLE:
		return m_pStyleMgr->Reload();
		break;
	case UI_RESOURCE_LAYOUT:
		break;
	}

	return false;
}

bool SkinManager::Save(UI_RESOURCE_TYPE eResType)
{
	if( eResType == UI_RESOURCE_SKIN || eResType == UI_RESOURCE_NULL )
	{
		if( NULL != m_pSkinInfoMgr && false == m_pSkinInfoMgr->Save() )
		{	
			UI_LOG_ERROR(_T("SkinManager::Save  skin info save failed."));
			return false;
		}
	}
	if( eResType == UI_RESOURCE_IMAGE || eResType == UI_RESOURCE_NULL )
	{
		if( NULL != m_pImageMgr && false == m_pImageMgr->Save() )
		{
			UI_LOG_ERROR(_T("SkinManager::Save  image save failed."));
			return false;
		}
	}

	if( eResType == UI_RESOURCE_COLOR || eResType == UI_RESOURCE_NULL )
	{
		if( NULL != m_pColorMgr && false == m_pColorMgr->Save() )
		{
			UI_LOG_ERROR(_T("SkinManager::Save  color save failed."));
			return false;
		}
	}
	if( eResType == UI_RESOURCE_FONT || eResType == UI_RESOURCE_NULL )
	{
		if( NULL != m_pFontMgr && false == m_pFontMgr->Save() )
		{
			UI_LOG_ERROR(_T("FontManager::Save  font save failed."));
			return false;
		}
	}
	if( eResType == UI_RESOURCE_STYLE || eResType == UI_RESOURCE_NULL )
	{
		if( NULL != m_pStyleMgr && false == m_pStyleMgr->Save() )
		{
			UI_LOG_ERROR(_T("StyleManager::Save  style save failed."));
			return false;
		}
	}
	if( eResType == UI_RESOURCE_LAYOUT || eResType == UI_RESOURCE_NULL )
	{
		if( NULL != m_pLayoutMgr && false == m_pLayoutMgr->Save() )
		{
			UI_LOG_ERROR(_T("StyleManager::Save  layout save failed."));
			return false;
		}
	}

	return true;
}

//
//	获取皮肤名称
//
bool SkinManager::GetSkinName(TCHAR* szName)
{
	if( NULL == szName )
		return false;

	_tcscpy(szName, _T(""));

	if (NULL == m_pPojoSkinItem)
	{
		return false;
	}
	
	_tcscpy(szName, m_pPojoSkinItem->GetSkinName().c_str());
	return true;
}

const String SkinManager::GetSkinName()
{
	if (NULL == m_pPojoSkinItem)
	{
		return _T("");
	}

	return m_pPojoSkinItem->GetSkinName();
}

bool SkinManager::SetImageManager( ImageManager* pImageManager )
{
	if( NULL == pImageManager || NULL != m_pImageMgr )
	{
		return false;
	}

	m_pImageMgr = pImageManager;
	return true;
}

bool SkinManager::SetColorManager(ColorManager* pColorManager)
{
	if( NULL == pColorManager || NULL != m_pColorMgr )
	{
		return false;
	}

	m_pColorMgr = pColorManager;
	return true;
}

bool SkinManager::SetFontManager(FontManager* pFontManager)
{
	if( NULL == pFontManager || NULL != m_pFontMgr )
	{
		return false;
	}

	m_pFontMgr = pFontManager;
	return true;
}

bool SkinManager::SetStyleManager(StyleManager* pStyleManager)
{
	if( NULL == pStyleManager || NULL != m_pStyleMgr )
	{
		return false;
	}

	m_pStyleMgr = pStyleManager;
	return true;
}

bool SkinManager::SetLayoutManager(LayoutManager* pLayoutManager)
{
	if( NULL == pLayoutManager || NULL != m_pLayoutMgr )
	{
		return false;
	}

	m_pLayoutMgr = pLayoutManager;
	return true;
}

//
//	获取图片.xml所在路径，用于将导入的图片拷贝到该目录之下
//
bool SkinManager::GetResourceDataSourcePath( UI_RESOURCE_TYPE eResType, String& strRet )
{
	if ( NULL == m_pImageMgr )
	{
		return false;
	}
	
	switch (eResType)
	{
	case UI_RESOURCE_IMAGE:
		strRet = m_pImageMgr->GetImageParse()->GetDataSourceStringRef();
		break;

	case UI_RESOURCE_FONT:
		strRet = m_pFontMgr->GetFontParse()->GetDataSourceStringRef();
		break;

	case UI_RESOURCE_COLOR:
		strRet = m_pColorMgr->GetColorParse()->GetDataSourceStringRef();
		break;

	case UI_RESOURCE_STYLE:
		strRet = m_pStyleMgr->GetStyleParse()->GetDataSourceStringRef();
		break;

	case UI_RESOURCE_LAYOUT:
		strRet = m_pLayoutMgr->GetLayoutParse()->GetDataSourceStringRef();
		break;
	}
	
	return true;
}

//
//	过期，使用HRBITMAP代替
//
//	从当前皮肤中获取一张图片对象
//
// bool SkinManager::GetImage( const String& strImageID, UIImage** ppImage )
// {
// 	if ( NULL == m_pImageMgr )
// 	{
// 		UI_LOG_ERROR( _T("SkinManager::GetImage  failed."));
// 		return false;
// 	}
// 
// 	return this->m_pImageMgr->GetImage( strImageID, ppImage );
// }
HRBITMAP SkinManager::GetImage( const String& strImageID, GRAPHICS_RENDER_TYPE eRenderType )
{
	if ( NULL == m_pImageMgr )
	{
		UI_LOG_ERROR( _T("SkinManager::GetImage  failed."));
		return false;
	}

	return this->m_pImageMgr->GetImage( strImageID, eRenderType );
}

bool SkinManager::GetColor( const String& strColorID, UIColor** pCol )
{
	if ( NULL == m_pColorMgr )
	{
		UI_LOG_ERROR( _T("SkinManager::GetColor  failed."));
		return false;
	}

	return this->m_pColorMgr->GetColor( strColorID, pCol );
}
// bool SkinManager::GetFont( const String& strFontID, UIFont** ppFont )
// {
// 	if ( NULL == m_pFontMgr )
// 	{
// 		UI_LOG_ERROR( _T("SkinManager::GetFont  failed."));
// 		return false;
// 	}
// 
// 	return this->m_pFontMgr->GetFont( strFontID, ppFont );
// }
HRFONT SkinManager::GetFont( const String& strFontID,  GRAPHICS_RENDER_TYPE eRenderType )
{
 	if ( NULL == m_pFontMgr )
 	{
 		UI_LOG_ERROR( _T("SkinManager::GetFont  failed."));
 		return NULL;
 	}
 
 	return this->m_pFontMgr->GetFont( strFontID, eRenderType );
}
HRFONT SkinManager::GetDefaultFont( GRAPHICS_RENDER_TYPE eRenderType )
{
	if ( NULL == m_pFontMgr )
	{
		UI_LOG_ERROR( _T("SkinManager::GetDefalutFont failed."));
		return NULL;
	}

	return this->m_pFontMgr->GetDefaultFont( eRenderType );
}
HRFONT SkinManager::GetFontEx( HRFONT hRFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_TYPE eRenderType )
{
	if ( NULL == m_pFontMgr )
	{
		UI_LOG_ERROR( _T("SkinManager::GetFontEx  failed."));
		return NULL;
	}

	return this->m_pFontMgr->GetFontEx( hRFont, w, l, eRenderType );
}
bool SkinManager::GetCursor( const String& strCursorID, UICursor** ppCursor )
{
	if ( NULL == m_pImageMgr )
	{
		UI_LOG_ERROR( _T("SkinManager::GetCursor  failed."));
		return false;
	}

	return this->m_pImageMgr->GetCursor( strCursorID, ppCursor );
}

//
//	加载指定的对象的布局数据，在xml中将根据pRootObj的object name和id来查找对象及数据
//
bool SkinManager::LoadLayout(Object* pRootObj)
{
	if( NULL == m_pLayoutMgr )
	{
		UI_LOG_ERROR(_T("%s m_pLayoutMgr == NULL."), _T(__FUNCTION__));
		return false;
	}

	return m_pLayoutMgr->LoadLayout( pRootObj );
}

//
//	更新指定的对象的布局数据，在xml中将根据pRootObj的object name和id来查找原有对象的新属性和布局
//
bool SkinManager::ReLoadLayout( Object* pRootObj, list<Object*>& listAllChild )
{
	if( NULL == m_pLayoutMgr )
	{
		UI_LOG_ERROR(_T("SkinManager::ReLoadLayout m_pLayoutMgr == NULL."));
		return false;
	}

	return m_pLayoutMgr->ReLoadLayout( pRootObj, listAllChild );
}

bool SkinManager::LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle )
{
	if( NULL == m_pStyleMgr )
	{
		UI_LOG_ERROR(_T("SkinManager::LoadStyle m_pStyleMgr == NULL."));
		return false;
	}
	return m_pStyleMgr->LoadStyle( strTagName, strStyleClass, strID, mapStyle );
}

Menu* SkinManager::LoadMenu( const String& strMenuId )
{
	if( NULL == m_pLayoutMgr )
	{
		UI_LOG_ERROR(_T("%s m_pLayoutMgr == NULL."), _T(__FUNCTION__));
		return NULL;
	}

	return m_pLayoutMgr->LoadMenu( strMenuId );
}

int SkinManager::GetImageCount( )
{
	if( NULL == m_pImageMgr )
		return -1;

	return m_pImageMgr->GetImageCount();
}
bool SkinManager::GetImageItemInfo( int nIndex, IImageItemInfo** ppImageItemInfo )
{
	if( NULL == m_pImageMgr )
		return false;

	return m_pImageMgr->GetImageItemInfo(nIndex, ppImageItemInfo);
}

int SkinManager::GetColorCount( )
{
	if( NULL == m_pColorMgr )
		return -1;

	return m_pColorMgr->GetColorCount();
}
bool SkinManager::GetColorItemInfo( int nIndex, IColorItemInfo** ppColorItemInfo )
{
	if( NULL == m_pColorMgr )
		return false;

	return m_pColorMgr->GetColorItemInfo( nIndex, ppColorItemInfo );
}
int SkinManager::GetFontCount( )
{
	if( NULL == m_pFontMgr )
		return -1;

	return m_pFontMgr->GetFontCount();
}
bool SkinManager::GetFontItemInfo( int nIndex, IFontItemInfo** ppFontItemInfo )
{
	if( NULL == m_pFontMgr )
		return false;

	return m_pFontMgr->GetFontItemInfo(nIndex, ppFontItemInfo);
}

int SkinManager::GetStyleCount( )
{
	if( NULL == m_pStyleMgr )
		return false;

	return m_pStyleMgr->GetStyleCount();
}
bool SkinManager::GetStyleItemInfo( int nIndex, IStyleItemInfo** ppStyleItemInfo )
{
	if( NULL == m_pStyleMgr )
		return false;

	return m_pStyleMgr->GetStyleItemInfo( nIndex, ppStyleItemInfo );
}

bool SkinManager::ChangeSkinH(WORD wNewH)
{
	if( NULL == m_pImageMgr )
		return false;

	return m_pImageMgr->ChangeSkinH(wNewH);
}