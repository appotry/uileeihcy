#include "stdafx.h"
namespace UI
{


//
//	[global] void  UI_LOG_***** ( String strContent )
//
//	Parameters
//		strContent
//			[in] 要输出到日志中的内容
//
//	Return
//		NA
//
//	Remark
//		该函数直接调用LogManager的Log方法。
//		在任何想输出*****级别的UI日志的地方调用该函数即可
//		[注意]:可变参数中的 %s，不支持用String赋值，请使用 String::c_str()形式
//
//	See Also
//		LogManager::Debug
//

#define UI_LOG_LEVEL(l,c)     LOG_LEVEL( g_pUIApplication->m_pLog, _T("ui"),l,c )
#define MOUSE_LOG_LEVEL(l,c)  LOG_LEVEL( g_pUIApplication->m_pLog, _T("mouse"),l,c )

void UI_LOG_DEBUG( TCHAR* szContent, ... )
{
	UI_LOG_LEVEL(LOG_LEVEL_DEBUG, szContent );
}
void UI_LOG_INFO( TCHAR* szContent, ... )
{
	UI_LOG_LEVEL(LOG_LEVEL_INFO, szContent );
}
void UI_LOG_WARN( TCHAR* szContent, ... )
{
	UI_LOG_LEVEL(LOG_LEVEL_WARN, szContent );
}
void UI_LOG_ERROR( TCHAR* szContent, ... )
{
	UI_LOG_LEVEL(LOG_LEVEL_ERROR, szContent );
}
void UI_LOG_FATAL( TCHAR* szContent, ...)
{
	UI_LOG_LEVEL(LOG_LEVEL_FATAL, szContent );
}
void MOUSE_LOG_DEBUG( TCHAR* szContent, ...)
{
	MOUSE_LOG_LEVEL(LOG_LEVEL_DEBUG,szContent);
}



//
// 应该程序调用，初始化
//
bool UI_Initialize( const String& strUIProjXmlPath )
{
	if( NULL == g_pUIApplication )
	{
		g_pUIApplication = new UIApplication;
	}
	return g_pUIApplication->Initialize( strUIProjXmlPath );
}
//
//	释放所有资源，退出. 但必须保证所有窗口已经退出并析构，否则会导致资源指针野掉
//
bool UI_Exit()
{
	if( NULL != g_pUIApplication )
	{
		int nCount = g_pUIApplication->m_TopWindowMgr.GetTopWindowCount();
		if( 0 != nCount )   // <-- 该方法有可能还是不准，有可能窗口被销毁了，但窗口对象还没有析构
		{
			UI_LOG_WARN(_T("UI_Exit TopWindowCount=%d"),nCount);
			UIASSERT(0);
			return false;
		}
	}
	SAFE_DELETE(g_pUIApplication);
	return true;
}

bool UI_ChangeSkin( const String& strSkinName )
{
	if( NULL == g_pUIApplication )
	{
		return false;
	}

	return g_pUIApplication->m_ProjectMgr.ChangeSkin( strSkinName );
}

bool UI_ChangeSkin( int nIndex )
{
	if( NULL == g_pUIApplication )
	{
		return false;
	}

	return g_pUIApplication->m_ProjectMgr.ChangeSkin( nIndex );
}

bool UI_ChangeSkinH( WORD newH )
{
	if( NULL == g_pUIApplication )
	{
		return false;
	}

	return g_pUIApplication->m_ProjectMgr.ChangeSkinH( newH );
}


//
//	设置当前程序的模式，在皮肤编辑器中为designMode
//
void UI_SetDesignMode( bool bDesignMode )
{
	if( NULL == g_pUIApplication )
	{
		g_pUIApplication = new UIApplication;
	}
	g_pUIApplication->SetDesignMode(bDesignMode);
}


//
//	创建一个工程
//
//	Parameter
//		strProjDir
//			[in]	新建工程所在目录
//	
//		strProjName
//			[in]	新建工程名称
//
bool UI_CreateProject  ( const String&   strProjDir, const String& strProjName )
{
	if( NULL == g_pUIApplication )
	{
		g_pUIApplication = new UIApplication;
	}
	return g_pUIApplication->m_ProjectMgr.CreateProject(strProjDir,strProjName);
}

//
// 打开工程（仅限皮肤编辑器调用，应该程序应该调用UI_Initialize函数）
//
bool UI_OpenProject( const String& strProjFilePath )
{
	if( NULL == g_pUIApplication )
	{
		g_pUIApplication = new UIApplication;
	}
	return g_pUIApplication->m_ProjectMgr.OpenProject( strProjFilePath, false );
}

//
//	创建一个皮肤。在工程名称下，创建一个皮肤文件夹，然后在皮肤文件夹下面
//	创建皮肤相关xml
//
//	Parameter
//		strSkinName
//			[in]	皮肤名称
//
//		strImageXmlPath
//			[in]	皮肤中的图片集路径
//
//	Return
//		成功返回皮肤句柄，失败返回NULL
//
HSKIN UI_CreateSkin     ( const String& strSkinName, const String& strImageXmlPath, 
						  const String& strColorXmlPath, const String& strFontXmlPath,
						  const String& strLayoutXmlPath,const String& strStyleXmlPath )
{
	if( NULL == g_pUIApplication )
		return NULL;

	return g_pUIApplication->m_ProjectMgr.CreateSkin( strSkinName, strImageXmlPath, strColorXmlPath, strFontXmlPath, strLayoutXmlPath, strStyleXmlPath );
} 

// 枚举工程中的皮肤
// HSKIN UI_GetNextProjectSkin( HSKIN hSkin/* = NULL*/ )
// {
// 	return g_pUIApplication->GetNextProjectSkin( hSkin );
// }

// 从临时文件中重新加载资源数据
bool UI_ReloadResource( HSKIN hSkin, UI_RESOURCE_TYPE eResType )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.ReloadResource( hSkin, eResType );
}

bool UI_InsertImageItem( HSKIN    hSkin, const String& strID, const String& strPath )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.InsertImageItem(hSkin, strID, strPath );
}
bool UI_InsertColorItem( HSKIN hSkin, const String& strID, const String& strColor )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.InsertColorItem(hSkin, strID, strColor );
}
bool UI_InsertFontItem ( HSKIN hSkin, const String& strID, LOGFONT* pLogfont )
{
	return g_pUIApplication->m_ProjectMgr.InsertFontItem(hSkin, strID, pLogfont);
}
bool UI_InsertStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.InsertStyleItem( hSkin, type, strID, strInherit );
}
bool UI_InsertStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.InsertStyleAttribute( hSkin, type, strID, strKey, strValue );
}
bool UI_ModifyImageItem( HSKIN hSkin, const String& strID, const String& strPath )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.ModifyImageItem(hSkin, strID, strPath );
}
bool UI_ModifyFontItem ( HSKIN hSkin, const String& strID, LOGFONT* pLogfont )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.ModifyFontItem(hSkin, strID, pLogfont);
}
bool UI_ModifyColorItem( HSKIN hSkin, const String& strID, const String& strColor )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.ModifyColorItem(hSkin, strID, strColor);
}
bool UI_ModifyStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.ModifyStyleItem(hSkin, type, strID, strInherit);
}
bool UI_ModifyStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strKey, const String& strValue )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.ModifyStyleAttribute( hSkin, type, strID, strKey, strValue );
}
bool UI_RemoveImageItem( HSKIN hSkin, const String& strID )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.RemoveImageItem( hSkin, strID );
}
bool UI_RemoveFontItem( HSKIN hSkin, const String& strID )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.RemoveFontItem(hSkin, strID);
}
bool UI_RemoveColorItem( HSKIN hSkin, const String& strID )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.RemoveColorItem(hSkin,strID);
}
bool UI_RemoveStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.RemoveStyleItem(hSkin, type, strID);
}
bool UI_RemoveStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strKey )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.RemoveStyleAttribute(hSkin, type, strID, strKey );
}

//
//	获取指定皮肤的图标存放路径
//
//	Parameter
//		hSkin
//			[in]	皮肤句柄
//
//		szRet
//			[in,out] 返回路径。需要调用者分配/释放内存
//
//		nSize
//			[in]	 指定szRet的大小。如果为0，则函数返回所需要的（字符）长度（包括NULL).
//
int UI_GetResourceDataSourcePath( HSKIN hSkin, UI_RESOURCE_TYPE eResType, TCHAR* szRet, int nSize )
{
	if( NULL == g_pUIApplication )
		return 0;

	String str;
	bool bRet = g_pUIApplication->m_ProjectMgr.GetResourceDataSourcePath(hSkin, eResType, str);
	if( false == bRet )
		return 0;

	if( nSize == 0 )
		return str.length() + 1;

	if ( NULL == szRet )
		return 0;

	_tcscpy( szRet, _T("") );
	_tcsncpy( szRet, str.c_str(), nSize-1 );
	szRet[nSize-1] = _T('');

	return min((int)str.length()+1,nSize);
}
 
bool UI_GetProjectName( TCHAR* szRet, int* pnSize )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetProjectName(szRet,pnSize);
}
void UI_Save(HSKIN hSkin/*=NULL*/, UI_RESOURCE_TYPE eResType/*=UI_RESOURCE_NULL*/)
{
	if( NULL == g_pUIApplication )
		return;

	g_pUIApplication->Save(hSkin,eResType);
}
bool UI_GetProjectPath( TCHAR* szRet )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetProjectPath(szRet);
}
bool UI_GetProjectDirectory( TCHAR* szRet )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetProjectDirectory(szRet);
}
//
//	获取指定皮肤的名称
//
//	Parameter
//		hSkin
//			[in]	皮肤句柄
//
//		szRet
//			[in,out] 返回路径。需要调用者分配/释放内存
//
//		nSize
//			[in]	 指定szRet的大小。如果为0，则函数返回所需要的（字符）长度（包括NULL).
//
int UI_GetSkinName( HSKIN hSkin, TCHAR* szRet, int nSize )
{
	if( NULL == g_pUIApplication )
		return 0;

	String str;
	bool bRet = g_pUIApplication->m_ProjectMgr.GetSkinName(hSkin, str);
	if( false == bRet )
		return -1;

	if( nSize == 0 )
		return str.length() + 1;

	if ( NULL == szRet )
		return -1;

	_tcsncpy( szRet, str.c_str(), nSize );

	return min((int)str.length()+1,nSize);
}

int UI_GetSkinName( int nIndex, TCHAR* szRet, int nSize )
{
	if( NULL == g_pUIApplication )
		return 0;

	String str;
	bool bRet = g_pUIApplication->m_ProjectMgr.GetSkinName(nIndex, str);
	if( false == bRet )
		return -1;

	if( nSize == 0 )
		return str.length() + 1;

	if ( NULL == szRet )
		return -1;

	_tcsncpy( szRet, str.c_str(), nSize );

	return min((int)str.length()+1,nSize);
}

int UI_GetSkinCount( )
{
	if( NULL == g_pUIApplication )
		return 0;

	return g_pUIApplication->m_ProjectMgr.GetSkinCount();
}
HSKIN UI_GetSkinHandle( int nIndex )
{
	if( NULL == g_pUIApplication )
		return NULL;

	return g_pUIApplication->m_ProjectMgr.GetSkinHandle(nIndex);
}

int UI_GetSkinImageCount( HSKIN hSkin )
{
	if( NULL == g_pUIApplication )
		return 0;

	return g_pUIApplication->m_ProjectMgr.GetSkinImageCount(hSkin);
}
bool UI_GetSkinImageItemInfo( HSKIN hSkin, int nIndex, IImageItemInfo** ppImageItemInfo )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetSkinImageItemInfo( hSkin, nIndex, ppImageItemInfo );
}
int UI_GetSkinColorCount( HSKIN h )
{
	if( NULL == g_pUIApplication )
		return 0;

	return g_pUIApplication->m_ProjectMgr.GetSkinColorCount(h);
}
bool UI_GetSkinColorItemInfo( HSKIN h, int nIndex, IColorItemInfo** ppColorItemInfo )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetSkinColorItemInfo( h, nIndex, ppColorItemInfo );
}

int UI_GetSkinFontCount( HSKIN h )
{
	if( NULL == g_pUIApplication )
		return 0;

	return g_pUIApplication->m_ProjectMgr.GetSkinFontCount(h);
}
bool UI_GetSkinFontItemInfo( HSKIN h, int nIndex, IFontItemInfo** ppFontItemInfo )
{	
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetSkinFontItemInfo(h,nIndex,ppFontItemInfo);
}

int UI_GetSkinStyleCount( HSKIN h)
{
	if( NULL == g_pUIApplication )
		return 0;

	return g_pUIApplication->m_ProjectMgr.GetSkinStyleCount(h);
}
bool UI_GetSkinStyleItemInfo( HSKIN h, int nIndex, IStyleItemInfo** ppStyleItemInfo )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetSkinStyleItemInfo(h, nIndex, ppStyleItemInfo);
}

bool UI_SetActiveSkin( HSKIN hSkin )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.SetActiveSkin(hSkin);
}

int UI_GetActiveSkinIndex()
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetActiveSkinIndex();
}

int   UI_GetProjectSkinCount()
{
	if( NULL == g_pUIApplication )
		return 0;

	return g_pUIApplication->m_ProjectMgr.GetSkinCount();
}
bool  UI_GetProjectSkinItemInfo( int nIndex, IProjectSkinItemInfo** pSkinItem )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetSkinItemInfo(nIndex, pSkinItem);
}

//	过期，使用UI_GetBitmap代替
//
// 获取图片资源,返回的UIImage必须使用UIImage::Release进行释放
//
// bool UI_GetImage( const String& strImageID, UIImage** ppImage, HSKIN hSkin/* = NULL*/ )
// {
// 	if( NULL == g_pUIApplication )
// 		return false;
// 
// 	return g_pUIApplication->m_ProjectMgr.GetImage( strImageID, ppImage, hSkin );
// }

 bool UI_GetColor( const String& strColorID, UIColor** pColor, HSKIN hSkin/* = NULL*/  )
 {
	 if( NULL == g_pUIApplication )
		 return false;

	 return g_pUIApplication->m_ProjectMgr.GetColor( strColorID, pColor, hSkin );
 }

// bool UI_GetFont( const String& strFontID, UIFont** ppFont, HSKIN hSkin /*= NULL*/  )
// {
// 	if( NULL == g_pUIApplication )
// 		return false;
// 
// 	return g_pUIApplication->m_ProjectMgr.GetFont( strFontID, ppFont, hSkin );
// }

bool UI_GetCursor( const String& strCursorID, UICursor** ppCursor, HSKIN hSkin /*= NULL*/ )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.GetCursor( strCursorID, ppCursor, hSkin );
}

HRFONT   UI_GetFontEx( HRFONT hRFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_TYPE eRenderType, HSKIN hSkin )
{
	if( NULL == g_pUIApplication )
		return NULL;

	return g_pUIApplication->m_ProjectMgr.GetFontEx( hRFont, w, l, eRenderType, hSkin );
}
HRFONT   UI_GetFont( const String& strFontID, GRAPHICS_RENDER_TYPE eRenderType, HSKIN hSkin )
{
	if( NULL == g_pUIApplication )
		return NULL;

	return g_pUIApplication->m_ProjectMgr.GetFont( strFontID, eRenderType, hSkin );
}

HRFONT   UI_CopyFont( HRFONT hFont )
{
	if( NULL == hFont )
		return NULL;

	IRenderFont* pRenderFont = (HRFONT)hFont;
	pRenderFont->AddRef();
	return hFont;
}
void     UI_ReleaseFont( HRFONT hFont )
{
	if( NULL == hFont )
		return;

	IRenderFont* pRenderFont = (HRFONT)hFont;
	pRenderFont->Release();
}
void UI_AttachFont(HRFONT* pOut, HFONT hFont, GRAPHICS_RENDER_TYPE eRenderType )
{
	if( NULL == hFont || NULL == pOut)
		return;

	switch (eRenderType)
	{
	case GRAPHICS_RENDER_TYPE_GDI:
		{
			GDIRenderFont::CreateInstance(pOut);
			IRenderFont* pRenderFont = (HRFONT)*pOut;
			pRenderFont->Attach(hFont);
			pRenderFont->AddRef();
		}
		break;

	case GRAPHICS_RENDER_TYPE_GDIPLUS:
		{
			GdiplusRenderFont::CreateInstance(pOut);
			IRenderFont* pRenderFont = (HRFONT)*pOut;
			pRenderFont->Attach(hFont);
			pRenderFont->AddRef();
		}
		break;

	default: 
		return ;
	}

	return ;
}

GRAPHICS_RENDER_TYPE UI_GetFontRenderType(HRFONT hRFont)
{
	if( NULL == hRFont )
		return GRAPHICS_RENDER_TYPE_GDI;

	IRenderFont* pRenderFont = (HRFONT)hRFont;
	return pRenderFont->GetRenderType();
}

HFONT  UI_GetFontHFONT(HRFONT hRFont)
{
	if( NULL == hRFont )
		return NULL;

	IRenderFont* pRenderFont = (HRFONT)hRFont;
	return pRenderFont->GetHFONT();
}

HRBITMAP UI_GetBitmap( const String& strImageID, GRAPHICS_RENDER_TYPE eRenderType, HSKIN hSkin )
{
	if( NULL == g_pUIApplication )
		return NULL;

	return g_pUIApplication->m_ProjectMgr.GetImage( strImageID, eRenderType, hSkin );
}

void UI_ReleaseBitmap( HRBITMAP hBitmap )
{
	if( NULL == hBitmap )
		return;

	IRenderBitmap* pRenderBitmap = (HRBITMAP)hBitmap;
	pRenderBitmap->Release();
}
int  UI_GetBitmapWidth( HRBITMAP hBitmap )
{
	if( NULL == hBitmap )
		return 0;

	IRenderBitmap* pRenderBitmap = (HRBITMAP)hBitmap;
	return pRenderBitmap->GetWidth();
}
int  UI_GetBitmapHeight( HRBITMAP hBitmap )
{
	if( NULL == hBitmap )
		return 0;

	IRenderBitmap* pRenderBitmap = (HRBITMAP)hBitmap;
	return pRenderBitmap->GetHeight();
}

/*
** 从layout.xml中加载一个指定对象的所有子对象
*/
bool  UI_LoadLayout( Object* pRootObj )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.LoadLayout( pRootObj );
}

//
//	加载样式
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
bool UI_LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.LoadStyle( strTagName, strStyleClass, strID, mapStyle );
}

//
// 更新顶层窗口布局，该函数只负责计算了主窗口的rcParent,rcWindow，没有调用SetWindowPos
//
void  UI_UpdateTopWindowLayout( WindowBase* pWindow )
{
	if( NULL == pWindow )
		return;

	DesktopLayout   dl;
	dl.m_pWindow = pWindow;
	
	// 创建一个临时的DC用于在布局过程中测量字符串的大小
	HDC hDC = UI_GetCacheDC();

	HRDC hRDC = GetHRDC(hDC,pWindow->m_hWnd);
	dl.Arrange(hRDC);
	ReleaseHRDC(hRDC);

	::UI_ReleaseCacheDC(hDC);
}

//
//	在一个对象状态发生改变时，更新它的布局
//	
void UI_UpdateLayout( WindowBase* pObj, BOOL bRedraw )
{
	if( NULL == pObj )
		return;

	HDC hDC = UI_GetCacheDC();
	HRDC hRDC = GetHRDC(hDC, pObj->m_hWnd);
//	pObj->PrepareDC(hRDC);   // 由于不用求WindowBase* pObj的大小，导致它没有去设置DC属性，因此需要在这里设置一下
	
	if( NULL != pObj->GetLayout() )
	{
		pObj->GetLayout()->Arrange(hRDC);
	}

	ReleaseHRDC(hRDC);
	UI_ReleaseCacheDC(hDC);

	if( bRedraw )
		pObj->UpdateObject();
}



//
//	将当前路径扩展为全路径，以uidll作为当前路径
//
void  UI_ExtendPath( String& strPath )
{
	if( FALSE == Util::IsFullPath( strPath.c_str() ) )
	{
		TCHAR szModulePath[MAX_PATH] = _T("");

		GetModuleFileName( g_pUIApplication->GetModuleInstance(), szModulePath, MAX_PATH );
		TCHAR* szTemp = _tcsrchr(szModulePath, _T('\\'));
		if( NULL != szTemp )
			*(szTemp+1) = 0;

		String strTemp = szModulePath;
		strPath =  strTemp + strPath;
	}
}


//
// 字符串拆分
//
//	Remark:
//		在向DLL之间使用stl作为参数进行传递，会导致内存释放时的崩溃，因此这里没有直接去调用UI_Split方法
//		而是直接在ULDLL中重写这么一个函数
//
void  UI_Split( const String& str, TCHAR szSep, vector<String>& vRet )
{
	int nIndex = 0;

	while( true )
	{
		int nResult = (int)str.find( szSep, nIndex );
		if( -1 == nResult )
		{
			vRet.push_back( str.substr( nIndex, str.length()-nIndex ) );
			break;
		}
		else
		{
			vRet.push_back( str.substr( nIndex, nResult-nIndex ) );
			nIndex = ++nResult;
		}
	}
}

//
//	整个应该程序中临时需要用到HDC，则可以调用该函数进行获取，最后调用
//	UI_ReleaseCacheDC释放
//
HDC   UI_GetCacheDC()
{
	if( NULL == g_pUIApplication )
		return NULL;

	return g_pUIApplication->m_dcCache.GetDC();
}
void  UI_ReleaseCacheDC(HDC hDC)
{
	if( NULL == g_pUIApplication )
		return;

	g_pUIApplication->m_dcCache.ReleaseDC(hDC);
}

//
//	在创建一个窗口之前，调用该函数保存HWND对应的窗口对象的指针，然后在窗口的第一个消息中，
//	用UI_ExtractCreateWndData再获取出来，接着便可以利用atl的thunk来进行子类化
//
void   UI_AddCreateWndData(_AtlCreateWndData* pData, void* pThis)
{
	if( NULL == g_pUIApplication )
		return;

	return g_pUIApplication->m_create_wnd_data.AddCreateWndData(pData, pThis);
}
void*  UI_ExtractCreateWndData()
{
	if( NULL == g_pUIApplication )
		return NULL;

	return g_pUIApplication->m_create_wnd_data.ExtractCreateWndData();
}


BOOL  UI_IsDialogMessage( MSG* pMsg )
{
	if( NULL == pMsg )
		return FALSE;

	if((pMsg->message < WM_KEYFIRST || pMsg->message > WM_KEYLAST) &&
		(pMsg->message < WM_MOUSEFIRST || pMsg->message > WM_MOUSELAST))
		return FALSE;

	UIASSERT( NULL != g_pUIApplication );

	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB )
	{
		// 获取这个消息的窗口所在的顶层窗口，因为导航是针对是整个顶层窗口进行的
		HWND hWndTop = pMsg->hwnd;
		while (1)
		{
			if( NULL == hWndTop )
				return FALSE;

			LONG lStyle = ::GetWindowLong(hWndTop, GWL_STYLE);
			if( lStyle & WS_CHILD )
			{
				hWndTop = ::GetParent(hWndTop);
			}
			else
			{
				break;
			}
		}

		// 判断这个窗口是否属性UI管理的一个顶层窗口
		WindowBase* pWindow = g_pUIApplication->m_TopWindowMgr.GetWindowBase(hWndTop);
		if( NULL == pWindow )
			return FALSE;

		// 导航
		if( Util::IsKeyDown(VK_SHIFT) )
		{
			pWindow->GetKeyboardMgr().Tab_2_PrevControl();
		}
		else
		{
			pWindow->GetKeyboardMgr().Tab_2_NextControl();
		}
		return TRUE;
	}

	return FALSE;
}


} // namespace

