#include "StdAfx.h"


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             ͼƬ                                     //
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
//	�½�һ��ͼƬ�����ļ�
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
**		�ɹ�����true��ʧ�ܷ���false
**
**	Remark
**		�����е�ͼƬ��Դ������m_map���С�
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
//	Ϊ��ʵ���ڱ༭�����ܹ�ͬʱ��view/code����������б༭�����޸���code�����,
//  code�Ὣ����д�뵽һ��tmp�ļ��У�Ȼ��ImageManager��Ҫ���¶�ȡ������
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

#if 0  //-- ���ڣ�ʹ��HRBITMAP����
/*
**	[public]  bool GetImage( const String& strID, UIImage** ppImage )
**
**	Parameters
**		strID
**			[in] Ҫ��ȡ��ͼƬ���ƣ�Ҳ������xmlͼƬ�����е�name��
**
**		ppImage
**			[out] ����ͼƬָ�롣ʹ�ý�������Ҫ����CUIImage::Release()
**
**	Return
**		�ɹ�����true,ʧ�ܷ���false��ʧ��ԭ��μ���־
**
**	Remark
**		Ϊ�˷�ֹ������������ʹ�ý������ͷ�CUIImage�������ｫCUIImage��Ϊһ��������������һ������ֵ��
**		ǿ�Ƶ�����ΪCUIImage����һ��ָ�롣
**
*/
bool ImageManager::GetImage( const String& strID, UIImage** ppImage )
{
// 	if( 0 == this->m_map.count(strID) )
// 	{
// 		UI_LOG_ERROR( _T("ImageManager::GetImageʧ�ܣ�û���ҵ���%s"), strName.c_str() );
// 		return false;
// 	}
// 
// 	UIImage* pImage = this->m_map[strID];
// 	if( NULL == pImage )
// 	{
// 		UI_LOG_ERROR( _T("ImageManager::GetImageʧ�ܣ�ͼƬ���ƣ�%s"), strName.c_str() );
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

