#include "StdAfx.h"


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             图片                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


ImageManager::ImageManager(void)
{
	m_pImageParse = NULL;
}

ImageManager::~ImageManager(void)
{
	this->Clear();
	if ( NULL != m_pImageParse )
	{
		m_pImageParse->Release();
		m_pImageParse = NULL;
	}
}

void ImageManager::SetImageParse( IImageParse* p )
{
	if ( NULL != m_pImageParse )
	{
		m_pImageParse->Release();
		m_pImageParse = NULL;
	}

	m_pImageParse = p;
}

//
//	新建一个图片管理文件
//
bool ImageManager::Create( const String& strXmlPath )
{
	if( NULL != m_pImageParse )
	{
		UI_LOG_WARN(_T("ImageManager::Create image parse already exist."));
		return false;
	}

	m_pImageParse = IParseClassFactory::CreateImageParseInstance( strXmlPath );
	if( false == m_pImageParse->Create() )
	{
		UI_LOG_ERROR(_T("ImageManager::Create create image parse failed. path=%s"), strXmlPath.c_str());
		return false;
	}

	return true;
}

bool ImageManager::Save()
{
	assert( NULL != m_pImageParse );
	if( NULL == m_pImageParse )
		return false;

	return m_pImageParse->Save(&m_pojoImage);
}
/*
**	[public]  bool Load( )
**
**	Parameters
**		NA
**
**	Return
**		成功返会true，失败返回false
**
**	Remark
**		将所有的图片资源保存在m_map当中。
**
**	See Also
**		UIGlobal:: LoadImage
**
*/
bool ImageManager::Load( const String& strXmlPath )
{
	this->Clear();

	if( NULL != m_pImageParse )
	{
		UI_LOG_WARN(_T("ImageManager::Load image parse already exist."));
		return false;
	}

	m_pImageParse = IParseClassFactory::CreateImageParseInstance( strXmlPath );

	if( false == m_pImageParse->Load( &m_pojoImage ) )
		return false;

	return true;
}

//
//	为了实现在编辑器中能够同时用view/code两个界面进行编辑，在修改了code代码后,
//  code会将数据写入到一个tmp文件中，然后ImageManager需要重新读取新数据
//
bool ImageManager::Reload()
{
	if( NULL == m_pImageParse )
	{
		UI_LOG_ERROR(_T("ImageManager::Reload m_pImageParse == NULL."));
		return false;
	}

	this->Clear();
	return  m_pImageParse->Reload( &m_pojoImage );
}

bool ImageManager::ChangeSkinH(WORD wNewH)
{
	return m_pojoImage.ChangeSkinH(wNewH);
}

void ImageManager::Clear()
{
	m_pojoImage.Clear();
}

#if 0  //-- 过期，使用HRBITMAP代替
/*
**	[public]  bool GetImage( const String& strID, UIImage** ppImage )
**
**	Parameters
**		strID
**			[in] 要获取的图片名称，也就是在xml图片配置中的name。
**
**		ppImage
**			[out] 返回图片指针。使用结束后需要调用CUIImage::Release()
**
**	Return
**		成功返回true,失败返回false。失败原因参见日志
**
**	Remark
**		为了防止调用者忘记在使用结束后释放CUIImage，在这里将CUIImage作为一个参数，而不是一个返回值，
**		强制调用者为CUIImage声明一个指针。
**
*/
bool ImageManager::GetImage( const String& strID, UIImage** ppImage )
{
// 	if( 0 == this->m_map.count(strID) )
// 	{
// 		UI_LOG_ERROR( _T("ImageManager::GetImage失败，没有找到：%s"), strName.c_str() );
// 		return false;
// 	}
// 
// 	UIImage* pImage = this->m_map[strID];
// 	if( NULL == pImage )
// 	{
// 		UI_LOG_ERROR( _T("ImageManager::GetImage失败，图片名称：%s"), strName.c_str() );
// 		return false;
// 	}
// 
// 	pImage->AddRef();
// 
// 	*ppImage = pImage;

	return m_pojoImage.GetImage( strID, ppImage );
}
#endif

HRBITMAP ImageManager::GetImage( const String& strID, GRAPHICS_RENDER_TYPE eRenderType )
{
	return m_pojoImage.GetImage(strID, eRenderType);
}

bool ImageManager::GetCursor( const String& strID, UICursor** ppCursor )
{
	return m_pojoCursor.GetCursor(strID, ppCursor);
}

bool ImageManager::InsertImageItem( const String& strID, const String& strPath )
{
	if( NULL == m_pImageParse )
		return false;

	if( false == m_pojoImage.InsertImage( strID, strPath ) )
	{
		UI_LOG_ERROR(_T("ImageManager::InsertImageItem  m_pojoImage.InsertImage strID=%s,strPath=%s Failed. "), strID.c_str(), strPath.c_str() );
		return false;
	}

	return m_pImageParse->InsertImage( this->m_pojoImage.GetImageItem(strID) );
}
bool ImageManager::ModifyImageItem( const String& strID, const String& strPath )
{
	if( NULL == m_pImageParse )
		return false;

	if( false == m_pojoImage.ModifyImage( strID, strPath ) )
	{
		UI_LOG_ERROR(_T("ImageManager::ModifyImageItem  m_pojoImage.ModifyImage strID=%s,strPath=%s Failed. "), strID.c_str(), strPath.c_str() );
		return false;
	}

	return m_pImageParse->ModifyImage( this->m_pojoImage.GetImageItem(strID) );
}
bool ImageManager::RemoveImageItem( const String& strID )
{
	if( NULL == m_pImageParse )
		return false;

	if( false == m_pojoImage.RemoveImage( strID ) )
	{
		UI_LOG_ERROR(_T("ImageManager::RemoveImageItem  m_pojoImage.RemoveImage strID=%s Failed. "), strID.c_str() );
		return false;
	}

	return m_pImageParse->RemoveImage( strID );
}

int ImageManager::GetImageCount( )
{
	return m_pojoImage.GetImageCount();
}
bool ImageManager::GetImageItemInfo( int nIndex, IImageItemInfo** ppImageItemInfo )
{
	if( NULL == ppImageItemInfo )
		return false;

	CPojo_ImageItem* pItem = m_pojoImage.GetImageItem(nIndex);
	*ppImageItemInfo = pItem;

	return true;
}

