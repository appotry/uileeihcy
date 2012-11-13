#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////
//
//	CPojo_Project
//

CPojo_ProjectSkinItem::CPojo_ProjectSkinItem()
{
	memset(&m_sHlsInfo, 0, sizeof(SKIN_HLS_INFO));
}

void CPojo_ProjectSkinItem::SetHLS(char h, char l, char s, int nFlag)
{
	if (nFlag & CHANGE_SKIN_HLS_FLAG_H)
	{
		m_sHlsInfo.h = h;
	}

	if (nFlag & CHANGE_SKIN_HLS_FLAG_L)
	{
		m_sHlsInfo.l = l;
	}

	if (nFlag & CHANGE_SKIN_HLS_FLAG_S)
	{
		m_sHlsInfo.s = s;
	}

	m_sHlsInfo.nFlag |= nFlag;
}

CPojo_Project::~CPojo_Project()
{
	vector<CPojo_ProjectSkinItem*>::iterator iter = m_vSkins.begin();
	vector<CPojo_ProjectSkinItem*>::iterator iterEnd = m_vSkins.end();
	for ( ; iter != iterEnd; iter ++ )
	{
		CPojo_ProjectSkinItem* pItem = *iter;
		delete pItem;
		pItem = NULL;
	}
	m_vSkins.clear();
}


int CPojo_Project::GetSkinItemCount() 
{
	return (int)m_vSkins.size(); 
}

CPojo_ProjectSkinItem* CPojo_Project::GetSkinItem( int nIndex )
{
	if( nIndex < 0 || nIndex >= (int)m_vSkins.size() )
	{
		return NULL;
	}
	return m_vSkins[nIndex];
}
CPojo_ProjectSkinItem* CPojo_Project::GetSkinItem( const String& strSkinName )
{
	if( strSkinName.empty())
		return NULL;

	vector<CPojo_ProjectSkinItem*>::iterator iter = m_vSkins.begin();
	vector<CPojo_ProjectSkinItem*>::iterator iterEnd = m_vSkins.end();
	for ( ; iter != iterEnd; iter ++ )
	{
		CPojo_ProjectSkinItem* pItem = *iter;
		if( pItem->GetSkinName() == strSkinName )
			return pItem;
	}

	return NULL;
}

CPojo_ProjectSkinItem* CPojo_Project::GetSkinItem( SkinManager* pSkinMgr )
{
	if (NULL == pSkinMgr)
		return NULL;

	return GetSkinItem(pSkinMgr->GetSkinName());
}

CPojo_ProjectSkinItem* CPojo_Project::GetActiveSkinItem( )
{
	return GetSkinItem(m_strActiveSkinName);
}

int CPojo_Project::GetActiveSkinItemIndex()
{
	vector<CPojo_ProjectSkinItem*>::iterator iter = m_vSkins.begin();
	vector<CPojo_ProjectSkinItem*>::iterator iterEnd = m_vSkins.end();
	for ( int i = 0; iter != iterEnd; iter ++, i++ )
	{
		CPojo_ProjectSkinItem* pItem = *iter;
		if( pItem->GetSkinName() == m_strActiveSkinName )
			return i;
	}
	return -1;
}
bool CPojo_Project::AddSkinItem( CPojo_ProjectSkinItem* pNewItem )
{
	if( NULL == pNewItem )
		return false;

	if( NULL != this->GetSkinItem(pNewItem->GetSkinName()) )
		return false;

	m_vSkins.push_back(pNewItem);
	return true;
}
// 获取当前活跃皮肤所在路径
bool CPojo_Project::GetActiveSkinPath( TCHAR* szPath )
{
	return GetSkinPath( m_strActiveSkinName, szPath );
}
bool CPojo_Project::GetSkinPath( const String& strSkinName, TCHAR* szPath )
{
	if( NULL == szPath )
		return false;

	_tcscpy(szPath, _T(""));

	CPojo_ProjectSkinItem* pItem = this->GetSkinItem(strSkinName);
	if( NULL == pItem )
		return false;

	_tcscpy( szPath, pItem->GetSkinXmlPath().c_str() );
	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	CPojo_ImageItem
//

#if 0 // -- 过期，使用HRBITMAP代替
UIImage::UIImage(UIImage** pOutRef)
{
	m_dwRef = 0;
	*pOutRef = this;
	m_pOutRef = pOutRef;
}

UIImage::~UIImage()
{
	if(!__super::IsNull() )
	{
		__super::Destroy();
	}

	if(m_pOutRef!=NULL)
	{
		*m_pOutRef = NULL;
		m_pOutRef = NULL;
	}

}

void  UIImage::CreateInstance( const String& str, UIImage** pOutRef  )
{
	UIImage* pImage = new UIImage( pOutRef );
	pImage->Load( str.c_str() );
	pImage->AddRef();
}

long UIImage::AddRef()
{
	m_dwRef ++;
	return m_dwRef;
}

long UIImage::Release()
{
	m_dwRef--;
	if( 0 >= m_dwRef )
	{
		if(!__super::IsNull() )
		{
			__super::Destroy();
		}
		m_dwRef = 0;
		delete this;
		return 0;
	}

	return m_dwRef;
}

void UIImage::Modify( const String& str )
{
	if( m_dwRef != 0 )
	{
		// 该图片已经在被使用了，替换
		if(!__super::IsNull() )
		{
			__super::Destroy();
		}
		__super::Load( str.c_str() );
	}
}
#endif

//////////////////////////////////////////////////////////////////////////
CPojo_ImageItem::CPojo_ImageItem()
{
	m_pGdiBitmap = NULL;
	m_pGdiplusBitmap = NULL;
	m_pDirect2DBitmap = NULL;
	m_pOriginImageData = NULL;

	m_bUseSkinHLS = true;
//	u.icon.m_nWidth = u.icon.m_nHeight = 0;

}
CPojo_ImageItem::~CPojo_ImageItem()
{
	SAFE_DELETE(m_pGdiBitmap);
	SAFE_DELETE(m_pGdiplusBitmap);
	SAFE_DELETE(m_pDirect2DBitmap);
	SAFE_DELETE(m_pOriginImageData);
	
}

HRBITMAP CPojo_ImageItem::GetImage( GRAPHICS_RENDER_TYPE eRenderType, bool* pbFirstTimeCreate )
{	
	switch( eRenderType )
	{
	case GRAPHICS_RENDER_TYPE_GDI:
		{
			if (NULL == m_pGdiBitmap)
			{
				if (NULL != pbFirstTimeCreate)
					*pbFirstTimeCreate = true;

				//GDIRenderBitmap::CreateInstance( (IRenderBitmap**)&m_pGdiBitmap );
				RenderBitmapFactory::CreateInstance((IRenderBitmap**)&m_pGdiBitmap, eRenderType, m_mapAttribute, m_strPath);
				if( NULL != m_pGdiBitmap )
				{
					m_pGdiBitmap->LoadFromFile(m_strPath);
					UI_LOG_DEBUG(_T("%s gdi bitmap create: %s \tPtr=0x%08X"),FUNC_NAME, m_strID.c_str(), m_pGdiBitmap);
				}
			}

			if( NULL != m_pGdiBitmap )
			{
				m_pGdiBitmap->AddRef();
			}
			return (HRBITMAP)m_pGdiBitmap;
		}
		break;

	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		{
			if (NULL == m_pGdiplusBitmap)
			{
				if (NULL != pbFirstTimeCreate)
					*pbFirstTimeCreate = true;

				//GdiplusRenderBitmap::CreateInstance( (IRenderBitmap**)&m_pGdiplusBitmap );
				RenderBitmapFactory::CreateInstance((IRenderBitmap**)&m_pGdiplusBitmap, eRenderType, m_mapAttribute, m_strPath);
				if( NULL != m_pGdiplusBitmap )
				{
					m_pGdiplusBitmap->LoadFromFile(m_strPath);
					UI_LOG_DEBUG(_T("%s gdiplus bitmap create: %s \tPtr=0x%08X"), FUNC_NAME, m_strID.c_str(), m_pGdiplusBitmap );

#if 1  // 转化为Gdi类型  <-- 但最后发现其实两者最后的绘制内存占用率也差不多....为什么呢？按理gdiplus应该比gdi慢N倍的
					Gdiplus::Bitmap* pBitmap = m_pGdiplusBitmap->GetBitmap();
					Gdiplus::PixelFormat eSrcPixelFormat = pBitmap->GetPixelFormat();
					if (Gdiplus::IsAlphaPixelFormat(eSrcPixelFormat))
					{
						if (NULL == m_pGdiBitmap)
						{
							if (NULL != pbFirstTimeCreate)
								*pbFirstTimeCreate = true;
							else
								*pbFirstTimeCreate = false;

							RenderBitmapFactory::CreateInstance((IRenderBitmap**)&m_pGdiBitmap, GRAPHICS_RENDER_TYPE_GDI, m_mapAttribute, m_strPath);
							if (NULL != m_pGdiBitmap)
							{
								m_pGdiBitmap->GetBitmap()->CreateFromGdiplusBitmap(*pBitmap);
								UI_LOG_DEBUG(_T("%s change to gdi bitmap: %s \tPtr=0x%08X"),FUNC_NAME, m_strID.c_str(), m_pGdiBitmap);
							}
						}
						

						if( NULL != m_pGdiBitmap )
						{
							m_pGdiBitmap->AddRef();
						}
						SAFE_DELETE(m_pGdiplusBitmap); 
						return (HRBITMAP)m_pGdiBitmap;
					}
#endif
				}
			}
			if( NULL != m_pGdiplusBitmap )
			{
				m_pGdiplusBitmap->AddRef();
			}
			return (HRBITMAP)m_pGdiplusBitmap;
		}
		break;

	case GRAPHICS_RENDER_TYPE_DIRECT2D:
		{
			if (NULL == m_pDirect2DBitmap)
			{
				if (NULL != pbFirstTimeCreate)
					*pbFirstTimeCreate = true;

				RenderBitmapFactory::CreateInstance((IRenderBitmap**)&m_pDirect2DBitmap, eRenderType, m_mapAttribute, m_strPath);
				if (NULL != m_pDirect2DBitmap)
				{
					m_pDirect2DBitmap->LoadFromFile(m_strPath);
					UI_LOG_DEBUG(_T("%s direct2d bitmap create: %s \tPtr=0x%08X"), FUNC_NAME, m_strID.c_str(), m_pDirect2DBitmap );
				}
			}
			if (NULL != m_pDirect2DBitmap)
			{
				m_pDirect2DBitmap->AddRef();
			}
			return (HRBITMAP)m_pDirect2DBitmap;
		}
		break;

	default:
		return NULL;
	}
	return NULL;
}

bool CPojo_ImageItem::ModifyHLS(short h, short l, short s, int nFlag)
{
	if (false == m_bUseSkinHLS)
		return true;

	ModifyHLS(m_pGdiBitmap, h,l,s,nFlag);
	ModifyHLS(m_pGdiplusBitmap, h,l,s,nFlag);

	UIASSERT(0);
	ModifyHLS(m_pDirect2DBitmap, h,l,s,nFlag);

// 	if( NULL != m_pGdiBitmap )
// 	{
// 		if( NULL == m_pOriginImageData )
// 		{
// 			m_pOriginImageData = new ImageData;
// 			if( false == m_pGdiBitmap->SaveBits(m_pOriginImageData) )
// 			{
// 				UI_LOG_WARN(_T("%s not support this image to change hue. id=%s"), _T(__FUNCTION__), m_strID.c_str() );
// 				m_bUseSkinHLS = false;
// 				SAFE_DELETE(m_pOriginImageData);
// 				return false;
// 			}
// 		}
// 		m_pGdiBitmap->ChangeHLS(m_pOriginImageData, h,l,s,nFlag);
// 	}
// 	if( NULL != m_pGdiplusBitmap )
// 	{
// 		if( NULL == m_pOriginImageData )
// 		{
// 			m_pOriginImageData = new ImageData;
// 			if( false == m_pGdiplusBitmap->SaveBits(m_pOriginImageData) )
// 			{
// 				UI_LOG_WARN(_T("%s not support this image to change hue. id=%s"), _T(__FUNCTION__), m_strID.c_str() );
// 				m_bUseSkinHLS = false;
// 				SAFE_DELETE(m_pOriginImageData);
// 			}
// 		}
// 		m_pGdiplusBitmap->ChangeHLS(m_pOriginImageData,  h,l,s,nFlag);
// 	}
	return true;
}
bool CPojo_ImageItem::ModifyHLS( IRenderBitmap* pBitmap, short h, short l, short s, int nFlag )
{
	if (false == m_bUseSkinHLS)
		return true;

	if( NULL != pBitmap)
	{
		if( NULL == m_pOriginImageData )
		{
			m_pOriginImageData = new ImageData;
			if( false == pBitmap->SaveBits(m_pOriginImageData) )
			{
				UI_LOG_WARN(_T("%s not support this image to change hue. id=%s"), _T(__FUNCTION__), m_strID.c_str() );
				m_bUseSkinHLS = false;
				SAFE_DELETE(m_pOriginImageData);
			}
		}
		pBitmap->ChangeHLS(m_pOriginImageData,  h,l,s,nFlag);
	}
	return true;
}
bool CPojo_ImageItem::ModifyImage( const String& strPath )
{
	m_strPath = strPath;
	// TODO: Modify ...
	UIASSERT(0);

	return true;
}

void CPojo_ImageItem::SetAttribute(const ATTRMAP& mapAttr) 
{ 
	m_mapAttribute = mapAttr; 

	bool   bUseSkinHLS = true;

	ATTRMAP::const_iterator iter = mapAttr.find(XML_IMAGE_USESKINHLS);
	if (mapAttr.end() != iter)
	{
		const String& strUseSkinHLS = iter->second;
		if (strUseSkinHLS ==_T("0") || strUseSkinHLS == _T("false"))
			bUseSkinHLS = false;
	}

	this->SetUseSkinHLS(bUseSkinHLS);
}

//////////////////////////////////////////////////////////////////////////
//
//	CPojo_Image
//

CPojo_Image::CPojo_Image(SkinManager*  pSkinMgr)
{ m_pSkinMgr = pSkinMgr; }

CPojo_Image::~CPojo_Image()
{	
	this->Clear(); 
}

//
// 从文件中加载一项(由CXmlImageParse::load_from_file中调用)
//
bool CPojo_Image::LoadItem(ATTRMAP& mapAttr, const String& strFullPath)
{
	String strID;

	ATTRMAP::iterator iter;
	iter = mapAttr.find(XML_ID);
	if (mapAttr.end() != iter)
	{
		strID = iter->second;
	}

	CPojo_ImageItem* pItem = NULL;
	if (this->InsertImage(strID, strFullPath, &pItem))
	{
		pItem->SetAttribute(mapAttr);
		return true;
	}
	else
	{
		UI_LOG_WARN( _T("%s insert image m_strID=%s, path=%s failed."), FUNC_NAME, strID.c_str(), strFullPath.c_str());
		return false;
	}
}
int CPojo_Image::GetImageCount() 
{
	return (int)m_vImages.size();
}

CPojo_ImageItem* CPojo_Image::GetImageItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if( nIndex >= (int)m_vImages.size() )
		return NULL;

	return m_vImages[nIndex];
}

CPojo_ImageItem* CPojo_Image::GetImageItem( const String& strID )
{
	vector<CPojo_ImageItem*>::iterator  iter = m_vImages.begin();
	vector<CPojo_ImageItem*>::iterator  iterEnd = m_vImages.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_ImageItem* p = *iter;
		if( p->GetIDRef() == strID )
			return p;
	}
	return NULL;
}

bool CPojo_Image::InsertImage( const String& strID, const String& strPath, CPojo_ImageItem** pRet )
{
	CPojo_ImageItem* pItem = this->GetImageItem(strID);
	if( NULL != pItem )
	{
		UI_LOG_WARN(_T("CPojo_Image::InsertImage failed, insert item=%s, path=%s"), strID.c_str(), strPath.c_str() );
		return false;
	}

	CPojo_ImageItem*  pImageItem = new CPojo_ImageItem;
	pImageItem->SetID( strID );
	pImageItem->SetPath( strPath );

	this->m_vImages.push_back(pImageItem);  
	if( NULL != pRet )
		*pRet = pImageItem;
	return true;
}

bool CPojo_Image::ModifyImage( const String& strID, const String& strPath )
{
	CPojo_ImageItem* p = this->GetImageItem(strID);
	if( NULL != p )
	{
		return p->ModifyImage(strPath);
	}

	UI_LOG_WARN(_T("CPojo_Image::ModifyImage failed. modify image id=%s, path=%s"), strID.c_str(), strPath.c_str() );
	return false;
}
bool CPojo_Image::RemoveImage( const String& strID )
{
	vector<CPojo_ImageItem*>::iterator  iter = m_vImages.begin();
	vector<CPojo_ImageItem*>::iterator  iterEnd = m_vImages.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_ImageItem* p = *iter;
		if( p->GetIDRef() == strID )
		{
			delete p;
			p = NULL;
			m_vImages.erase(iter);
			return true;
		}
	}
	return false;
}
bool CPojo_Image::Clear()
{
	vector<CPojo_ImageItem*>::iterator  iter = m_vImages.begin();
	vector<CPojo_ImageItem*>::iterator  iterEnd = m_vImages.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_ImageItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vImages.clear();
	return true;
}

bool CPojo_Image::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	vector<CPojo_ImageItem*>::iterator  iter = m_vImages.begin();
	vector<CPojo_ImageItem*>::iterator  iterEnd = m_vImages.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_ImageItem* p = *iter;
		p->ModifyHLS(h,l,s,nFlag);
	}

	return true;
}

HRBITMAP CPojo_Image::GetImage( const String& strID, GRAPHICS_RENDER_TYPE eRenderType )
{
	CPojo_ImageItem* pItem = this->GetImageItem(strID);
	if( NULL == pItem )
	{
		UI_LOG_ERROR( _T("%s，GetImageItem：%s failed 1"), FUNC_NAME, strID.c_str() );
		return NULL;
	}

	bool bFirstTimeCreate = false;
	IRenderBitmap* pBitmap = pItem->GetImage( eRenderType, &bFirstTimeCreate );
	if (NULL == pBitmap)
	{
		UI_LOG_ERROR( _T("%s，GetImage：%s failed 2"), FUNC_NAME, strID.c_str() );
		return NULL;
	}

	if (bFirstTimeCreate)
	{
		// 检查当前皮肤的HLS
		IProjectSkinItemInfo* pSkinItemInfo;
		UI_GetProjectSkinItemInfo((HSKIN)m_pSkinMgr, &pSkinItemInfo);
		if (NULL != pSkinItemInfo)
		{
			SKIN_HLS_INFO* pHLSInfo = pSkinItemInfo->GetSkinHlsInfo();
			if ( 0 != pHLSInfo->h || 0 != pHLSInfo->l || 0 != pHLSInfo->s)
			{
				pItem->ModifyHLS(pBitmap, pHLSInfo->h, pHLSInfo->l, pHLSInfo->s, pHLSInfo->nFlag|CHANGE_SKIN_HLS_FLAG_HLS);
			}
		}
	}
	
	return pBitmap;
}
//////////////////////////////////////////////////////////////////////////
//
//	CPojo_CursorItem
//

UICursor::UICursor( UICursor** ppOutRef )
{
	m_dwRef = 0;
	m_hCursor = NULL;

	*ppOutRef = this;
	m_pOutRef = ppOutRef;
}
UICursor::~UICursor()
{
	*m_pOutRef = NULL;
	m_pOutRef = NULL;
}

void UICursor::CreateInstance( const String strCurFilePath, UICursor** pOutRef )
{
	assert( 0 && _T("TODO: UICursor::CreateInstance not implement"));
	//UICursor* pCursor = new UICursor(ppOutRef);
	//
}
void UICursor::CreateInstance( LPTSTR nCursorID, UICursor** ppOutRef )
{
	UICursor* pCursor = new UICursor(ppOutRef);
	pCursor->Load(nCursorID);
	pCursor->AddRef();
}
HCURSOR UICursor::GetCursor()
{ 
	return m_hCursor; 
}
bool  UICursor::Load( LPTSTR nCursorID )
{
	if( NULL != m_hCursor )
	{
		::DeleteObject(m_hCursor);
		m_hCursor = NULL;
	}
	m_hCursor = ::LoadCursor(NULL, nCursorID );
	return m_hCursor!=NULL ? true:false;
}
void  UICursor::Attach(HCURSOR hCursor)
{
	UIASSERT( NULL != hCursor );
	m_hCursor = hCursor;
}
HCURSOR  UICursor::Detach()
{
	UIASSERT( NULL!= m_hCursor );
	HCURSOR hOldCursor = m_hCursor;
	m_hCursor = NULL;

	return hOldCursor;
}
long  UICursor::AddRef()
{
	m_dwRef++;
	return m_dwRef;
}
long  UICursor::Release()
{
	m_dwRef--;
	if( 0 >= m_dwRef )
	{
		if( NULL != m_hCursor )
		{
			DeleteObject(m_hCursor);
			m_hCursor = NULL;
		}
		m_dwRef = 0;
		delete this;
		return 0;
	}
	return m_dwRef;
}

//////////////////////////////////////////////////////////////////////////
CPojo_CursorItem::CPojo_CursorItem()
{
	m_pCursor = NULL;
};

bool  CPojo_CursorItem::GetCursor( UICursor** ppCursor )
{
	if ( NULL == m_pCursor )
	{
		if( m_strCurFilePath.empty() )
		{
			if( m_strID == _T("idc_arrow"))
				UICursor::CreateInstance( IDC_ARROW, &m_pCursor );
			else if( m_strID == _T("idc_ibeam"))
				UICursor::CreateInstance( IDC_IBEAM, &m_pCursor );
			else if( m_strID == _T("idc_wait"))
				UICursor::CreateInstance( IDC_WAIT, &m_pCursor );
			else if( m_strID == _T("idc_cross"))
				UICursor::CreateInstance( IDC_CROSS, &m_pCursor );
			else if( m_strID == _T("idc_uparrow"))
				UICursor::CreateInstance( IDC_UPARROW, &m_pCursor );
			else if( m_strID == _T("idc_size"))
				UICursor::CreateInstance( IDC_SIZE, &m_pCursor );
			else if( m_strID == _T("idc_icon"))
				UICursor::CreateInstance( IDC_ICON, &m_pCursor );
			else if( m_strID == _T("idc_sizenwse"))
				UICursor::CreateInstance( IDC_SIZENWSE, &m_pCursor );
			else if( m_strID == _T("idc_sizenesw"))
				UICursor::CreateInstance( IDC_SIZENESW, &m_pCursor );
			else if( m_strID == _T("idc_sizewe"))
				UICursor::CreateInstance( IDC_SIZEWE, &m_pCursor );
			else if( m_strID == _T("idc_sizens"))
				UICursor::CreateInstance( IDC_SIZENS, &m_pCursor );
			else if( m_strID == _T("idc_sizeall"))
				UICursor::CreateInstance( IDC_SIZEALL, &m_pCursor );
			else if( m_strID == _T("idc_no"))
				UICursor::CreateInstance( IDC_NO, &m_pCursor );
			else if( m_strID == _T("idc_hand"))
				UICursor::CreateInstance( IDC_HAND, &m_pCursor );
			else if( m_strID == _T("idc_appstarting"))
				UICursor::CreateInstance( IDC_APPSTARTING, &m_pCursor );
			else if( m_strID == _T("idc_help"))
				UICursor::CreateInstance( IDC_HELP, &m_pCursor );
			else
				return false;
		}
		else
			return false;
	}
	else
	{
		m_pCursor->AddRef();
	}

	if( NULL == m_pCursor )
	{
		*ppCursor = NULL;
		return false;
	}

	*ppCursor = m_pCursor;
	return true;
}
bool  CPojo_CursorItem::ModifyCursor( const String& strCurFilePath )
{
	assert( false && _T("TODO: CPojo_CursorItem::ModifyCursor not implement"));
	return false;
}

CPojo_Cursor::CPojo_Cursor()
{
	// 添加默认的系统图标
	this->InsertCursor( _T("idc_arrow"), _T(""));
	this->InsertCursor( _T("idc_ibeam"), _T(""));
	this->InsertCursor( _T("idc_wait"), _T(""));
	this->InsertCursor( _T("idc_cross"), _T(""));
	this->InsertCursor( _T("idc_uparrow"), _T(""));
	this->InsertCursor( _T("idc_size"), _T(""));
	this->InsertCursor( _T("idc_icon"), _T(""));
	this->InsertCursor( _T("idc_sizenwse"), _T(""));
	this->InsertCursor( _T("idc_sizenesw"), _T(""));
	this->InsertCursor( _T("idc_sizewe"), _T(""));
	this->InsertCursor( _T("idc_sizens"), _T(""));
	this->InsertCursor( _T("idc_sizeall"), _T(""));
	this->InsertCursor( _T("idc_no"), _T(""));
	this->InsertCursor( _T("idc_hand"), _T(""));
	this->InsertCursor( _T("idc_appstarting"), _T(""));
	this->InsertCursor( _T("idc_help"), _T(""));
}
CPojo_Cursor::~CPojo_Cursor()
{
	this->Clear();
}
CPojo_CursorItem* CPojo_Cursor::GetCursorItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if( nIndex >= (int)m_vCursors.size() )
		return NULL;

	return m_vCursors[nIndex];
}	
CPojo_CursorItem* CPojo_Cursor::GetCursorItem( const String& strID )
{
	vector<CPojo_CursorItem*>::iterator  iter = m_vCursors.begin();
	vector<CPojo_CursorItem*>::iterator  iterEnd = m_vCursors.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_CursorItem* p = *iter;
		if( p->GetIDRef() == strID )
		{
			return p;
		}
	}
	return NULL;
}
int   CPojo_Cursor::GetCursorCount() 
{
	return (int)m_vCursors.size();
}
bool  CPojo_Cursor::GetCursor( const String& strID, UICursor** pCursorRet )
{
	if( NULL == pCursorRet )
		return false;

	CPojo_CursorItem* p = this->GetCursorItem(strID);
	if( NULL != p )
	{
		return p->GetCursor(pCursorRet);
	}

	UI_LOG_ERROR( _T("CPojo_Cursor::GetCursor failed, id=%s"), strID.c_str() );
	return false;
}
bool  CPojo_Cursor::InsertCursor( const String& strID, const String& strCurFilePath )
{
	CPojo_CursorItem* p = this->GetCursorItem(strID);
	if( NULL != p )
	{
		UI_LOG_WARN(_T("CPojo_Cursor::InsertCursor failed, insert item=%s, cursor=%s"), strID.c_str(), strCurFilePath.c_str() );
		return false;
	}

	CPojo_CursorItem*  pCursorItem = new CPojo_CursorItem;
	pCursorItem->SetID( strID );
	pCursorItem->SetCurFilePath( strCurFilePath );

	this->m_vCursors.push_back(pCursorItem);  
	return true;
}
bool  CPojo_Cursor::ModifyCursor( const String& strID, const String& strCursor )
{
	CPojo_CursorItem* p = this->GetCursorItem(strID);
	if( NULL != p )
	{
		p->ModifyCursor(strCursor);
		return true;
	}

	return false;
}

bool  CPojo_Cursor::RemoveCursor( const String& strID )
{
	vector<CPojo_CursorItem*>::iterator  iter = m_vCursors.begin();
	vector<CPojo_CursorItem*>::iterator  iterEnd = m_vCursors.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_CursorItem* p = *iter;
		if( p->GetIDRef() == strID )
		{
			delete p;
			p = NULL;
			m_vCursors.erase(iter);
			return true;
		}
	}

	return false;
}
void  CPojo_Cursor::Clear()
{
	vector<CPojo_CursorItem*>::iterator  iter = m_vCursors.begin();
	vector<CPojo_CursorItem*>::iterator  iterEnd = m_vCursors.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_CursorItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vCursors.clear();
}


//////////////////////////////////////////////////////////////////////////
//
//	CPojo_ColorItem
//

UIColor::UIColor( UIColor**  ppOutRef ) 
{
	m_dwRef = 0; 
	m_col = 0;

	m_pOutRef = ppOutRef;
	*ppOutRef = this;
}
UIColor::~UIColor()
{
	if(NULL != m_pOutRef)
	{
		*m_pOutRef = NULL;
	}
}
void  UIColor::CreateInstance( const String& strColor, UIColor** ppOutRef )
{
	UIColor* p = new UIColor(ppOutRef);
	p->SetColor(Util::TranslateRGB( strColor, XML_SEPARATOR ));
}
void  UIColor::CreateInstance( const COLORREF& color, UIColor** ppOutRef )
{
	UIColor* p = new UIColor(ppOutRef);
	p->SetColor(color);
}
COLORREF UIColor::GetColor()
{ 
	return m_col; 
}
void UIColor::SetColor(COLORREF col) 
{ 
	m_col = col; 
}
long UIColor::AddRef()
{
	m_dwRef ++;
	return m_dwRef;
}
long UIColor::Release()
{
	m_dwRef--;
	if( 0 >= m_dwRef )
	{
		m_dwRef = 0;
		delete this;
		return 0;
	}

	return m_dwRef;
}

CPojo_ColorItem::CPojo_ColorItem()
{
	m_bUseSkinHLS = true;
	m_pOriginColorValue = NULL;
	m_pColor = NULL;
}
CPojo_ColorItem::~CPojo_ColorItem()
{
	SAFE_DELETE(m_pColor);
	SAFE_DELETE(m_pOriginColorValue);
}
const String& CPojo_ColorItem::GetColorStringRef()
{ 
	return m_strColor; 
}

bool CPojo_ColorItem::GetColor(UIColor** pColor, bool* pbFirstTimeCreate) 
{
	if( NULL == pColor )
		return false;

	if( NULL == m_pColor )
	{
		 UIColor::CreateInstance(m_strColor, &m_pColor);
		 if (NULL != pbFirstTimeCreate)
		 {
			 *pbFirstTimeCreate = true;
		 }
	}
	m_pColor->AddRef();

	*pColor = m_pColor;
	return true;
}
void CPojo_ColorItem::SetColor( const String& strColor ) 
{
	m_strColor = strColor;

	if( NULL != m_pColor )
	{
		m_pColor->SetColor(Util::TranslateRGB( m_strColor, XML_SEPARATOR ));
	}
}

void CPojo_ColorItem::SetAttribute(const ATTRMAP& mapAttr)
{
	bool   bUseSkinHLS = true;

	ATTRMAP::const_iterator iter = mapAttr.find(XML_COLOR_USESKINHLS);
	if (mapAttr.end() != iter)
	{
		const String& strUseSkinHLS = iter->second;
		if (strUseSkinHLS ==_T("0") || strUseSkinHLS == _T("false"))
			bUseSkinHLS = false;
	}

	this->SetUseSkinHLS(bUseSkinHLS);
}

bool CPojo_ColorItem::ModifyHLS( short h, short l, short s, int nFlag )
{
	if (false == m_bUseSkinHLS)
		return true;

	if( NULL != m_pColor )
	{
		if( NULL == m_pOriginColorValue )
		{
			m_pOriginColorValue = new COLORREF;
			*m_pOriginColorValue = m_pColor->GetColor();
		}

		BYTE R = GetRValue(*m_pOriginColorValue);
		BYTE G = GetGValue(*m_pOriginColorValue);
		BYTE B = GetBValue(*m_pOriginColorValue);

		if (ChangeColorHLS(R,G,B,h,l,s,nFlag))
			m_pColor->SetColor(RGB(R,G,B));
	}
	return true;
}

CPojo_Color::~CPojo_Color()
{
	this->Clear();
}

int CPojo_Color::GetColorCount() 
{
	return (int)m_vColors.size();
}

CPojo_ColorItem* CPojo_Color::GetColorItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if( nIndex >= (int)m_vColors.size() )
		return NULL;

	return m_vColors[nIndex];
}

CPojo_ColorItem* CPojo_Color::GetColorItem( const String& strID )
{
	vector<CPojo_ColorItem*>::iterator  iter = m_vColors.begin();
	vector<CPojo_ColorItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_ColorItem* p = *iter;
		if( p->GetIDRef() == strID )
		{
			return p;
		}
	}
	return NULL;
}

bool CPojo_Color::GetColor( const String& strID, UIColor** pColorRet )
{
	if( NULL == pColorRet )
		return false;

	CPojo_ColorItem* p = this->GetColorItem(strID);
	if( NULL != p )
	{
		bool bFirstTimeCreate = false;
		if (false == p->GetColor(pColorRet, &bFirstTimeCreate) )
			return false;

		if (bFirstTimeCreate)
		{
			// 检查当前皮肤的HLS
			IProjectSkinItemInfo* pSkinItemInfo;
			UI_GetProjectSkinItemInfo((HSKIN)m_pSkinMgr, &pSkinItemInfo);
			if (NULL != pSkinItemInfo)
			{
				SKIN_HLS_INFO* pHLSInfo = pSkinItemInfo->GetSkinHlsInfo();
				if ( 0 != pHLSInfo->h || 0 != pHLSInfo->l || 0 != pHLSInfo->s)
				{
					p->ModifyHLS(pHLSInfo->h, pHLSInfo->l, pHLSInfo->s, pHLSInfo->nFlag|CHANGE_SKIN_HLS_FLAG_HLS);
				}
			}
		}
		return true;
	}

	UI_LOG_ERROR( _T("CPojo_Color::GetColor failed, id=%s"), strID.c_str() );
	return false;
}


//
// 从文件中加载一项(由CXmlImageParse::load_from_file中调用)
//
bool CPojo_Color::LoadItem(ATTRMAP& mapAttr, const String& strValue)
{
	String strID;

	ATTRMAP::iterator iter;
	iter = mapAttr.find(XML_ID);
	if (mapAttr.end() != iter)
	{
		strID = iter->second;
	}

	CPojo_ColorItem* pItem = NULL;
	if (this->InsertColor(strID, strValue, &pItem))
	{
		pItem->SetAttribute(mapAttr);
		return true;
	}
	else
	{
		UI_LOG_WARN( _T("%s insert image m_strID=%s, path=%s failed."), FUNC_NAME, strID.c_str(), strValue.c_str());
		return false;
	}
}
bool CPojo_Color::InsertColor( const String& strID, const String& strColor, CPojo_ColorItem** pItem )
{
	CPojo_ColorItem* p = this->GetColorItem(strID);
	if( NULL != p )
	{
		UI_LOG_WARN(_T("CPojo_Color::InsertColor failed, insert item=%s, color=%s"), strID.c_str(), strColor.c_str() );
		return false;
	}

	CPojo_ColorItem*  pColorItem = new CPojo_ColorItem;
	pColorItem->SetID( strID );
	pColorItem->SetColor( strColor );

	this->m_vColors.push_back(pColorItem); 

	if (NULL != pItem)
	{
		*pItem = pColorItem;
	}
	return true;
}

bool CPojo_Color::ModifyColor( const String& strID, const String& strColor )
{
	CPojo_ColorItem* p = this->GetColorItem(strID);
	if( NULL != p )
	{
		p->SetColor(strColor);
		return true;
	}

	return false;
}

bool CPojo_Color::RemoveColor(const String& strID )
{
	vector<CPojo_ColorItem*>::iterator  iter = m_vColors.begin();
	vector<CPojo_ColorItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_ColorItem* p = *iter;
		if( p->GetIDRef() == strID )
		{
			delete p;
			p = NULL;
			m_vColors.erase(iter);
			return true;
		}
	}

	return false;
}
void CPojo_Color::Clear()
{
	vector<CPojo_ColorItem*>::iterator  iter = m_vColors.begin();
	vector<CPojo_ColorItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_ColorItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vColors.clear();
}

bool CPojo_Color::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	vector<CPojo_ColorItem*>::iterator  iter = m_vColors.begin();
	vector<CPojo_ColorItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_ColorItem* p = *iter;
		p->ModifyHLS(h,l,s,nFlag);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
//
//	CPojo_ColorItem
//
#if 0

UIFont::UIFont(LOGFONT* plogfont, UIFont** ppOutRef)
{ 
	m_hFont = NULL; 
	m_dwRef = 0; 
	m_pLogFont = plogfont;

	m_pOutRef = ppOutRef;
	*ppOutRef = this;
}
UIFont::~UIFont() 
{ 
	if( -1 == m_dwRef )
	{
		m_hFont = NULL;
	}
	else
	{
		if( NULL != m_hFont )
		{
			::DeleteObject(m_hFont); 
			m_hFont = NULL;
		}
	}
	m_dwRef = 0; 
	m_pLogFont = NULL;

	if(m_pOutRef != NULL)
	{
		*m_pOutRef = NULL;
		m_pOutRef = NULL;
	}
}


/*static*/ void UIFont::CreateInstance(LOGFONT* plogfont, UIFont** ppOutRef) 
{ 
	UIFont* pFont = new UIFont(plogfont, ppOutRef); 
	
	if( NULL != plogfont )
	{
		pFont->Load(plogfont);
	}
	pFont->AddRef();
}

bool UIFont::Load( LOGFONT* plogfont )
{
	(if( NULL == plogfont )
	{
		UI_LOG_WARN(_T("UIFont::Load plogfont == NULL"));
		return false;
	}

	m_pLogFont = plogfont;
	m_hFont = ::CreateFontIndirect(m_pLogFont);
	if( NULL == m_hFont )
	{
		UI_LOG_WARN(_T("UIFont::SetLogFont CreateFontIndirect failed, facename=%s"), m_pLogFont->lfFaceName );
		return false;
	}

	return true;
}

HFONT UIFont::GetFont()
{ 
	return m_hFont; 
}

void UIFont::ModifyFont(LOGFONT* plogfont)
{
	m_pLogFont = plogfont;
	if( NULL != m_hFont )
	{
		::DeleteObject(m_hFont);
		m_hFont = ::CreateFontIndirect(m_pLogFont);
	}
}

//
//	TODO: Attach/Detach怎么与GetFont/Release统一起来使用？
//
//	--> 目前 Detach全由Release来替代，Release自己判断是否需要释放FONT对象
//
void UIFont::Attach(HFONT hFont)
{	
	UIASSERT(hFont != NULL);
	m_hFont = hFont;
	m_pLogFont = NULL;
}
HFONT UIFont::Detach()
{
	UIASSERT(-1 == m_dwRef);
	UIASSERT(m_hFont != NULL);

	HFONT hOldFont = m_hFont;
	m_hFont = NULL;

	return hOldFont;
}
long UIFont::AddRef()
{
	if ( NULL == m_hFont && NULL != m_pLogFont )
	{
		m_hFont = ::CreateFontIndirect( m_pLogFont );
		if( NULL == m_hFont )
		{
			UI_LOG_WARN( _T("UIFont::AddRef, Create font failed, font facename=%s size=%d"), m_pLogFont->lfFaceName, m_pLogFont->lfHeight );
		}
	}

	m_dwRef ++;
	return m_dwRef;
}
long UIFont::Release()
{
	m_dwRef--;
	if( 0 >= m_dwRef )
	{
		if( NULL != m_hFont && NULL != m_pLogFont )
		{
			::DeleteObject(m_hFont);
		}
		m_hFont = NULL;

		m_dwRef = 0;
		delete this;
		return 0;
	}

	return m_dwRef;
}
#endif

CPojo_FontItem::CPojo_FontItem() 
{ 
	m_pGdiFont = NULL;
	m_pGdiplusFont = NULL;
	m_pD2DFont = NULL;
	m_wParam = 0;
	m_lParam = 0;
}
CPojo_FontItem::~CPojo_FontItem()
{ 
	SAFE_DELETE(m_pGdiFont);
	SAFE_DELETE(m_pGdiplusFont);
	SAFE_DELETE(m_pD2DFont);
}

//
// 通过结构体来快速赋值
//
void CPojo_FontItem::SetLogFont( LOGFONT* pLogFont )
{
	if ( NULL==pLogFont)
		return;

	memcpy( &m_lf, pLogFont, sizeof(LOGFONT) );
	_tcscpy( m_lf.lfFaceName, pLogFont->lfFaceName );
	m_lf.lfEscapement = m_lf.lfOrientation;    // TODO: 实现字体角度的话，这两个值好像要相等
}

void CPojo_FontItem::ModifyFont( LOGFONT* pLogFont )
{
	this->SetLogFont( pLogFont );

	if( NULL != m_pGdiFont )
	{
		m_pGdiFont->ModifyFont(pLogFont);
	}
	if( NULL != m_pGdiplusFont )
	{
		m_pGdiplusFont->ModifyFont(pLogFont);
	}
	if (NULL != m_pD2DFont)
	{
		m_pD2DFont->ModifyFont(pLogFont);
	}
}

HRFONT CPojo_FontItem::GetFont( GRAPHICS_RENDER_TYPE eRenderType )
{
	switch(eRenderType)
	{
	case GRAPHICS_RENDER_TYPE_GDI:
		{
			if( NULL == m_pGdiFont )
			{
				GDIRenderFont::CreateInstance((IRenderFont**)&m_pGdiFont);
				if( NULL != m_pGdiFont )
				{
					m_pGdiFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("%s gdi font create: %s \tPtr=0x%08X"), FUNC_NAME, m_strID.c_str(), m_pGdiFont );
				}
			}
			if( NULL != m_pGdiFont )
			{
				m_pGdiFont->AddRef();
			}
			return (HRFONT)m_pGdiFont;
		}
		break;

	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		{
			if (NULL == m_pGdiplusFont)
			{
				GdiplusRenderFont::CreateInstance((IRenderFont**)&m_pGdiplusFont);
				if( NULL != m_pGdiplusFont )
				{
					m_pGdiplusFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("%s gdiplus font create: %s \tPtr=0x%08X"), FUNC_NAME, m_strID.c_str(), m_pGdiplusFont );
				}
			}
			if (NULL != m_pGdiplusFont)
			{
				m_pGdiplusFont->AddRef();
			}
			return (HRFONT)m_pGdiplusFont;
		}
		break;

	case GRAPHICS_RENDER_TYPE_DIRECT2D:
		{
			if (NULL == m_pD2DFont)
			{
				Direct2DRenderFont::CreateInstance((IRenderFont**)&m_pD2DFont);
				if( NULL != m_pD2DFont )
				{
					m_pD2DFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("%s direct2d font create: %s \tPtr=0x%08X"), FUNC_NAME, m_strID.c_str(), m_pGdiplusFont );
				}
			}
			if (NULL != m_pD2DFont)
			{
				m_pD2DFont->AddRef();
			}
			return (HRFONT)m_pD2DFont;
		}
		break;;

	default:
		return NULL;
	}

	return NULL;
}

CPojo_Font::~CPojo_Font()
{
	this->Clear();
}

int CPojo_Font::GetFontCount() 
{ 
	return (int) m_vFonts.size(); 
}
CPojo_FontItem* CPojo_Font::GetFontItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if( nIndex >= (int)m_vFonts.size() )
		return NULL;

	return m_vFonts[nIndex];
}
CPojo_FontItem* CPojo_Font::GetFontItem( const String& strID )
{
	vector<CPojo_FontItem*>::iterator  iter = m_vFonts.begin();
	vector<CPojo_FontItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_FontItem* p = *iter;
		if( p->GetIDRef() == strID )
			return p;
	}
	return NULL;
}
HRFONT CPojo_Font::GetFont( const String& strFontID, GRAPHICS_RENDER_TYPE eRenderType )
{
	vector<CPojo_FontItem*>::iterator iter = m_vFonts.begin();
	vector<CPojo_FontItem*>::iterator iterEnd = m_vFonts.end();

	CPojo_FontItem* pBaseFont = NULL;
	for( ; iter != iterEnd; iter++ )
	{
		CPojo_FontItem* p = *iter;
		if( p->GetIDRef() == strFontID )
		{
			if( p->GetWParam() == 0 && p->GetLParam() == 0 )
			{
				return p->GetFont(eRenderType);
			}
		}
	}
	return NULL;
}

HRFONT CPojo_Font::GetDefaultFont( GRAPHICS_RENDER_TYPE eRenderType )
{
	if (0 == (int)m_vFonts.size())
		return NULL;

	if (NULL != m_vFonts[0])
		return m_vFonts[0]->GetFont(eRenderType);

	return NULL;
}
bool  CPojo_Font::GetHRFontID(HRFONT hRFont, String& strID)
{
	vector<CPojo_FontItem*>::iterator iter = m_vFonts.begin();
	vector<CPojo_FontItem*>::iterator iterEnd = m_vFonts.end();

	CPojo_FontItem* pBaseFont = NULL;
	for( ; iter != iterEnd; iter++ )
	{
		CPojo_FontItem* p = *iter;
		if( p->IsMyHRFont(hRFont) )
		{
			strID = p->GetIDRef();
			return true;
		}
	}
	return false;
}

HRFONT CPojo_Font::GetFontEx( HRFONT hRFont, WPARAM wParam, LPARAM lParam, GRAPHICS_RENDER_TYPE eRenderType )
{
	String strFontID;
	if( false == GetHRFontID(hRFont, strFontID) )
	{
		return NULL;
	}
	vector<CPojo_FontItem*>::iterator iter = m_vFonts.begin();
	vector<CPojo_FontItem*>::iterator iterEnd = m_vFonts.end();

	CPojo_FontItem* pBaseFont = NULL;
	for( ; iter != iterEnd; iter++ )
	{
		CPojo_FontItem* p = *iter;
		if( p->GetIDRef() == strFontID )
		{
			if( p->GetWParam() == wParam && p->GetLParam() == lParam )
			{
				return p->GetFont(eRenderType);
			}
			else
			{
				if( p->GetWParam() == 0 && p->GetLParam() == 0 )
				{
					pBaseFont = p;
				}
			}
		}
	}

	if( NULL != pBaseFont )  // 没有找到 strFontID+wparam+lparam对应的字体，但找到了strFontID对应的字体
	{
		LOGFONT lf;
		memcpy( &lf, pBaseFont->GetLogFont(), sizeof(LOGFONT) );
		if( wParam & FONTITEM_FLAG_UNDERLINE )
		{
			lf.lfUnderline = 1;
		}
		if( wParam & FONTITEM_FLAG_BOLD )
		{
			lf.lfHeight = FW_BOLD;
		}
		if( wParam & FONTITEM_FLAG_STRIKEOUT )
		{
			lf.lfStrikeOut = 1;
		}
		if ( wParam & FONTITEM_FLAG_ITALIC )
		{
			lf.lfItalic = 1;
		}
		CPojo_FontItem* pNewFontItem = this->InsertFont(strFontID, &lf, wParam, lParam );
		if( NULL == pNewFontItem )
		{
			UI_LOG_ERROR( _T("%s InsertFont failed, id=%s wParam=%d lParam=%d"), _T(__FUNCTION__), strFontID.c_str(), wParam, lParam );
			return NULL;
		}

		return pNewFontItem->GetFont(eRenderType);
	}

	return NULL;
}

bool CPojo_Font::InsertFont( const String& strID, LOGFONT* pLogFont )
{
	CPojo_FontItem* pItem = this->InsertFont(strID, pLogFont, 0,0 );
	if( NULL == pItem )
		return false;
	
	return true;
}
CPojo_FontItem* CPojo_Font::InsertFont( const String& strID, LOGFONT* pLogFont, WPARAM wParam, LPARAM lParam )
{
	if( NULL == pLogFont )
	{
		UI_LOG_ERROR(_T("CPojo_Font::InsertFont failed."));
		return NULL;
	}
	vector<CPojo_FontItem*>::iterator  iter = m_vFonts.begin();
	vector<CPojo_FontItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_FontItem* p = *iter;

		if (p->GetIDRef() == strID && p->GetWParam() == wParam && p->GetLParam() == lParam )
		{
			UI_LOG_WARN(_T("CPojo_Font::InsertFont failed, insert item=%s"), strID.c_str() );
			return NULL;
		}
	}

	CPojo_FontItem*  pFontItem = new CPojo_FontItem;
	pFontItem->SetID( strID );
	pFontItem->SetLogFont( pLogFont );
	pFontItem->SetWParam(wParam);
	pFontItem->SetLParam(lParam);

	this->m_vFonts.push_back(pFontItem);  
	return pFontItem;
}
bool CPojo_Font::ModifyFont( const String& strID, LOGFONT* pLogFont )
{
	if( NULL == pLogFont )
	{
		UI_LOG_ERROR(_T("CPojo_Font::ModifyFont failed."));
		return NULL;
	}
	vector<CPojo_FontItem*>::iterator  iter = m_vFonts.begin();
	vector<CPojo_FontItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_FontItem* p = *iter;

		if (p->GetIDRef() == strID )
		{
			p->ModifyFont( pLogFont );
			return true;
		}
	}
	UI_LOG_WARN(_T("CPojo_Font::InsertFont failed, insert item=%s"), strID.c_str() );
	return false;
}
bool CPojo_Font::RemoveFont( const String& strID )
{
	vector<CPojo_FontItem*>::iterator  iter = m_vFonts.begin();
	vector<CPojo_FontItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_FontItem* p = *iter;

		if (p->GetIDRef() == strID )
		{
			delete p;
			p = NULL;
			m_vFonts.erase(iter);
			return true;
		}
	}
	return false;
}
void CPojo_Font::Clear()
{
	vector<CPojo_FontItem*>::iterator  iter = m_vFonts.begin();
	vector<CPojo_FontItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_FontItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vFonts.clear();
}


//////////////////////////////////////////////////////////////////////////
//
//	CPojo_StyleItem
//

//
// 解析一个继承字符串所属的样式类型，如将#button解析为 id选择类型，id=button
//
/*static*/ bool IStyleItemInfo::ParseInheritString(const String& strInherit, STYLE_SELECTOR_TYPE& eStyletype, TCHAR* szStyleName )
{
	if( strInherit.length() <= 0 || NULL == szStyleName )
		return false;

	if( strInherit[0] == STYLE_ID_PREFIX )
	{
		eStyletype = STYLE_SELECTOR_TYPE_ID;
		_tcsncpy( szStyleName, strInherit.substr( 1, strInherit.length()-1 ).c_str(), MAX_STRING_LEN-1 );
	}
	else if( strInherit[0] == STYLE_CLASS_PREFIX )
	{
		eStyletype = STYLE_SELECTOR_TYPE_CLASS;
		_tcsncpy( szStyleName, strInherit.substr( 1, strInherit.length()-1 ).c_str(), MAX_STRING_LEN-1 );
	}
	else
	{
		eStyletype = STYLE_SELECTOR_TYPE_TAG;
		_tcsncpy( szStyleName, strInherit.c_str(), MAX_STRING_LEN-1 );
	}
	return true;
}
/*static*/ bool IStyleItemInfo::MakeInheritString(const STYLE_SELECTOR_TYPE& eStyletype, const String& strStypeName, TCHAR* szInherit )
{
	if( strStypeName.length() <= 0 || NULL == szInherit )
		return false;

	if( eStyletype == STYLE_SELECTOR_TYPE_ID )
	{
		_tcscpy( szInherit, _T(" ") );
		szInherit[0] = STYLE_ID_PREFIX ;
		_tcscat( szInherit, strStypeName.c_str() );
	}
	else if( eStyletype == STYLE_SELECTOR_TYPE_CLASS )
	{
		_tcscpy( szInherit, _T(" ") );
		szInherit[0] = STYLE_CLASS_PREFIX ;
		_tcscat( szInherit, strStypeName.c_str() );
	}
	else
	{
		_tcscpy( szInherit, strStypeName.c_str() );
	}
	return true;
}


CPojo_StyleItem::CPojo_StyleItem()
{
	m_eSelectorType = STYLE_SELECTOR_TYPE_TAG;
}
CPojo_StyleItem::~CPojo_StyleItem()
{
	m_mapAttributes.clear();
}


void CPojo_StyleItem::SetInherits( const String& str )
{
	m_strInherits = str;
	m_vInheritList.clear();

	vector<String>  vStrArray;
	UI_Split(str, XML_SEPARATOR, vStrArray);
	int nSize = (int)vStrArray.size();
	if( 0 != nSize )
	{
		for(int i = 0; i < nSize; i++ )
		{
			m_vInheritList.push_back(vStrArray[i]);
		}
	}
}
int CPojo_StyleItem::GetInheritCount() 
{
	return (int) m_vInheritList.size(); 
}

bool CPojo_StyleItem::GetInheritItem(int i, String& str)
{
	if( i < 0 || i >= GetInheritCount() )
		return false;

	str = m_vInheritList[i];
	return true;
}

//
// 仅从m_vInheritList中删除，不从m_strInherit中删除
//
bool CPojo_StyleItem::RemoveInheritItem( const String& str )
{
	vector<String>::iterator iter = m_vInheritList.begin();
	vector<String>::iterator iterEnd = m_vInheritList.end();
	for ( ; iter != iterEnd; iter++ )
	{
		if( *iter == str )
		{
			m_vInheritList.erase(iter);
			return true;
		}
	}

	return false;
}


int CPojo_StyleItem::GetAttributesCount() 
{
	return (int)m_mapAttributes.size();
}

bool CPojo_StyleItem::GetAttribute( int nIndex, TCHAR* szkey, TCHAR* szValue )
{
	if( NULL == szkey || NULL == szValue )
		return false;

	String strKey, strValue;
	if( false == this->GetAttribute( nIndex, strKey, strValue ) )
		return false;

	_tcsncpy( szkey, strKey.c_str(), MAX_STRING_LEN-1 );
	_tcsncpy( szValue, strValue.c_str(), MAX_STRING_LEN-1 );

	return true;
}
bool CPojo_StyleItem::GetAttribute( int nIndex, String& key, String& value )
{
	if( nIndex < 0 || nIndex >= this->GetAttributesCount() )
		return false;

	map<String,String>::iterator iter = m_mapAttributes.begin();
	for ( int i = 0; i < nIndex; i++ )
	{
		if( iter == m_mapAttributes.end() )
			return false;

		iter ++;
	}

	key   = iter->first;
	value = iter->second;

	return true;
}

void CPojo_StyleItem::SetAttribute( const String& key, const String& value )
{
	m_mapAttributes[key] = value;
}

bool CPojo_StyleItem::GetAttribute( const String& key, String& retValue )
{
	ATTRMAP::iterator iter = m_mapAttributes.find(key);
	if (m_mapAttributes.end() != iter)
	{
		retValue = iter->second;
		return true;
	}
	return false;
}

bool CPojo_StyleItem::InsertAttribute( const String& key, const String& value )
{
	m_mapAttributes[key] = value;
	return true;
}

bool CPojo_StyleItem::ModifyAttribute( const String& key, const String& value )
{
	if( 0 == m_mapAttributes.count(key) )
		return false;

	m_mapAttributes[key] = value;
	return true;
}

bool CPojo_StyleItem::RemoveAttribute( const String& key )
{
	if( 0 == m_mapAttributes.count(key) )
		return false;

	m_mapAttributes.erase(key);
	return true;
}

bool CPojo_StyleItem::InheritAttribute( const String& key, const String& value )
{
	// 继承一个属性值。如果自己已经定义了这个属性，则使用自身的值
	if( 0 == m_mapAttributes.count(key) )
	{
		m_mapAttributes[key] = value;
		return true;
	}
	return false;
}

//
// 将自己的属性继承给另一个item
//	
//	如果pChild已经有这个属性了，则使用pChild的属性
//
bool CPojo_StyleItem::InheritMyAttributesToAnother( CPojo_StyleItem* pChild )
{
	if( NULL == pChild )
		return false;

	map<String,String>::iterator iter = m_mapAttributes.begin();
	map<String,String>::iterator iterEnd = m_mapAttributes.end();
	for ( ; iter != iterEnd; iter++ )
	{
		pChild->InheritAttribute(iter->first, iter->second);	
	}
	return true;
}

//
//	将自己的属性应用到一个已有的map中，如果map中已有这个属性，则使用该map的属性
//
bool CPojo_StyleItem::ApplyMyAttributesToAnExistMap( map<String,String>& mapAttrib )
{
	map<String,String>::iterator iter = m_mapAttributes.begin();
	map<String,String>::iterator iterEnd = m_mapAttributes.end();
	for ( ; iter != iterEnd; iter++ )
	{
		if( mapAttrib.count( iter->first ) != 0 )
			continue;

		mapAttrib[iter->first] = iter->second;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// 
//   CPojo_Style
//

CPojo_Style::~CPojo_Style()
{
	this->Clear();
}

void CPojo_Style::Clear()
{
	vector<CPojo_StyleItem*>::iterator  iter = m_vStyles.begin();
	vector<CPojo_StyleItem*>::iterator  iterEnd = m_vStyles.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_StyleItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vStyles.clear();
}

bool CPojo_Style::InsertStyle( STYLE_SELECTOR_TYPE type, const String& strID, const String& strInherit )
{
	CPojo_StyleItem* pStyleItem = new CPojo_StyleItem;
	pStyleItem->SetSelectorType(type);
	pStyleItem->SetID(strID);
	pStyleItem->SetInherits(strInherit);

	bool bRet = this->InsertStyle(pStyleItem);
	if( false == bRet )
	{
		delete pStyleItem;
		pStyleItem = NULL;
	}

	return bRet;
}

bool CPojo_Style::InsertStyle( CPojo_StyleItem* pNewItem )
{
	if( NULL == pNewItem )
		return false;

	CPojo_StyleItem* p = this->GetStyleItem(pNewItem->GetSelectorType(), pNewItem->GetIDRef());
	if( NULL != p )
	{
		UI_LOG_WARN(_T("CPojo_Style::InsertStyle failed, insert item=%s"), pNewItem->GetIDRef().c_str() );
		return false;
	}

	this->m_vStyles.push_back(pNewItem);  
	return true;
}

bool CPojo_Style::ModifyStyle( STYLE_SELECTOR_TYPE type,const String& strID, const String& strInherit )
{
	CPojo_StyleItem* p = this->GetStyleItem(type, strID);
	if( NULL == p )
	{
		UI_LOG_WARN(_T("CPojo_Style::ModifyStyle failed, modify item=%s"),strID.c_str() );
		return false;
	}

	p->SetInherits(strInherit);
	return true;
}

bool CPojo_Style::RemoveStyle( STYLE_SELECTOR_TYPE type, const String& strID )
{
	vector<CPojo_StyleItem*>::iterator  iter = m_vStyles.begin();
	vector<CPojo_StyleItem*>::iterator  iterEnd = m_vStyles.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_StyleItem* p = *iter;
		if( p->GetSelectorType() == type && p->GetIDRef() == strID )
		{
			delete p;
			p = NULL;
			m_vStyles.erase(iter);
			return true;
		}
	}
	return false;
}

int CPojo_Style::GetStyleCount() 
{
	return (int)m_vStyles.size();
}


CPojo_StyleItem* CPojo_Style::GetStyleItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if( nIndex >= (int)m_vStyles.size() )
		return NULL;

	return m_vStyles[nIndex];
}

CPojo_StyleItem* CPojo_Style::GetStyleItem( STYLE_SELECTOR_TYPE type, const String& strID )
{
	vector<CPojo_StyleItem*>::iterator  iter = m_vStyles.begin();
	vector<CPojo_StyleItem*>::iterator  iterEnd = m_vStyles.end();

	for( ; iter != iterEnd; iter++ )
	{
		CPojo_StyleItem* p = *iter;
		if( p->GetSelectorType() == type && p->GetIDRef() == strID )
		{
			return p;
		}
	}
	return NULL;
}

bool CPojo_Style::InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	CPojo_StyleItem* pItem = this->GetStyleItem(type, strID);
	if( false == pItem )
		return false;

	return pItem->InsertAttribute( strKey, strValue );
}
bool CPojo_Style::ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	CPojo_StyleItem* pItem = this->GetStyleItem(type, strID);
	if( false == pItem )
		return false;

	return pItem->ModifyAttribute( strKey, strValue );
}
bool CPojo_Style::RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey )
{
	CPojo_StyleItem* pItem = this->GetStyleItem(type, strID);
	if( false == pItem )
		return false;

	return pItem->RemoveAttribute( strKey );
}

//
//	返回匹配的样式
//
//	Parameter
//		strTagName
//			[in]	要匹配的标签名
//		strStyleClass
//			[in]	要匹配的class样式
//		strID
//			[in]	要匹配的id样式
//
//		mapStyle
//			[in/out] 将结果返回到该map中
//
//	Remark
//		优化级定义：
//			级别1. 在layout.xml中直接指定的属性
//			级别2. 通过ID匹配的属性
//			级别3. 通过CLASS匹配的属性
//			级别4. CLASS列表中，排后面的样式匹配的属性 > 前面的样式匹配的属性
//			级别5. 通过TAG匹配的属性
//
bool CPojo_Style::LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle )
{
	if( !strID.empty() )
	{
		CPojo_StyleItem* pIDStyleItem = this->GetStyleItem( STYLE_SELECTOR_TYPE_ID, strID );
		if( NULL != pIDStyleItem )
		{
			pIDStyleItem->ApplyMyAttributesToAnExistMap(mapStyle);
		}
	}

	// strStyleClass可能有多个
	if( !strStyleClass.empty() )
	{
		vector<String>  vStrArray;
		UI_Split( strStyleClass, XML_SEPARATOR, vStrArray );
		int nCount = (int)vStrArray.size();
		if( 0 != nCount )
		{
			for ( int i = 0; i < nCount; i++ )
			{
				if( vStrArray[i].empty() )
					continue;

				CPojo_StyleItem* pClassStyleItem = this->GetStyleItem( STYLE_SELECTOR_TYPE_CLASS, vStrArray[i] );
				if( NULL != pClassStyleItem )
				{
					pClassStyleItem->ApplyMyAttributesToAnExistMap(mapStyle);
				}
			}
		}
	}

	if( !strTagName.empty() )
	{
		CPojo_StyleItem* pTagStyleItem = this->GetStyleItem( STYLE_SELECTOR_TYPE_TAG, strTagName );
		if( NULL != pTagStyleItem )
		{
			pTagStyleItem->ApplyMyAttributesToAnExistMap(mapStyle);
		}
	}

	return false;
}