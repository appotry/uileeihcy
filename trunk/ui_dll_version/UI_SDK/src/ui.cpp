#include "stdafx.h"
namespace UI
{


//
//	[global] void  UI_LOG_***** ( String strContent )
//
//	Parameters
//		strContent
//			[in] Ҫ�������־�е�����
//
//	Return
//		NA
//
//	Remark
//		�ú���ֱ�ӵ���LogManager��Log������
//		���κ������*****�����UI��־�ĵط����øú�������
//		[ע��]:�ɱ�����е� %s����֧����String��ֵ����ʹ�� String::c_str()��ʽ
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
// Ӧ�ó�����ã���ʼ��
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
//	�ͷ�������Դ���˳�. �����뱣֤���д����Ѿ��˳�������������ᵼ����Դָ��Ұ��
//
bool UI_Exit()
{
	if( NULL != g_pUIApplication )
	{
		int nCount = g_pUIApplication->m_TopWindowMgr.GetTopWindowCount();
		if( 0 != nCount )   // <-- �÷����п��ܻ��ǲ�׼���п��ܴ��ڱ������ˣ������ڶ���û������
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
//	���õ�ǰ�����ģʽ����Ƥ���༭����ΪdesignMode
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
//	����һ������
//
//	Parameter
//		strProjDir
//			[in]	�½���������Ŀ¼
//	
//		strProjName
//			[in]	�½���������
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
// �򿪹��̣�����Ƥ���༭�����ã�Ӧ�ó���Ӧ�õ���UI_Initialize������
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
//	����һ��Ƥ�����ڹ��������£�����һ��Ƥ���ļ��У�Ȼ����Ƥ���ļ�������
//	����Ƥ�����xml
//
//	Parameter
//		strSkinName
//			[in]	Ƥ������
//
//		strImageXmlPath
//			[in]	Ƥ���е�ͼƬ��·��
//
//	Return
//		�ɹ�����Ƥ�������ʧ�ܷ���NULL
//
HSKIN UI_CreateSkin     ( const String& strSkinName, const String& strImageXmlPath, 
						  const String& strColorXmlPath, const String& strFontXmlPath,
						  const String& strLayoutXmlPath,const String& strStyleXmlPath )
{
	if( NULL == g_pUIApplication )
		return NULL;

	return g_pUIApplication->m_ProjectMgr.CreateSkin( strSkinName, strImageXmlPath, strColorXmlPath, strFontXmlPath, strLayoutXmlPath, strStyleXmlPath );
} 

// ö�ٹ����е�Ƥ��
// HSKIN UI_GetNextProjectSkin( HSKIN hSkin/* = NULL*/ )
// {
// 	return g_pUIApplication->GetNextProjectSkin( hSkin );
// }

// ����ʱ�ļ������¼�����Դ����
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
//	��ȡָ��Ƥ����ͼ����·��
//
//	Parameter
//		hSkin
//			[in]	Ƥ�����
//
//		szRet
//			[in,out] ����·������Ҫ�����߷���/�ͷ��ڴ�
//
//		nSize
//			[in]	 ָ��szRet�Ĵ�С�����Ϊ0��������������Ҫ�ģ��ַ������ȣ�����NULL).
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
//	��ȡָ��Ƥ��������
//
//	Parameter
//		hSkin
//			[in]	Ƥ�����
//
//		szRet
//			[in,out] ����·������Ҫ�����߷���/�ͷ��ڴ�
//
//		nSize
//			[in]	 ָ��szRet�Ĵ�С�����Ϊ0��������������Ҫ�ģ��ַ������ȣ�����NULL).
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

//	���ڣ�ʹ��UI_GetBitmap����
//
// ��ȡͼƬ��Դ,���ص�UIImage����ʹ��UIImage::Release�����ͷ�
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
** ��layout.xml�м���һ��ָ������������Ӷ���
*/
bool  UI_LoadLayout( Object* pRootObj )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.LoadLayout( pRootObj );
}

//
//	������ʽ
//	Parameter
//		strTagName
//			[in]	Ҫƥ��ı�ǩ��
//		strStyleClass
//			[in]	Ҫƥ���class��ʽ
//		strID
//			[in]	Ҫƥ���id��ʽ
//
//		mapStyle
//			[in/out] ��������ص���map��
//
bool UI_LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle )
{
	if( NULL == g_pUIApplication )
		return false;

	return g_pUIApplication->m_ProjectMgr.LoadStyle( strTagName, strStyleClass, strID, mapStyle );
}

//
// ���¶��㴰�ڲ��֣��ú���ֻ��������������ڵ�rcParent,rcWindow��û�е���SetWindowPos
//
void  UI_UpdateTopWindowLayout( WindowBase* pWindow )
{
	if( NULL == pWindow )
		return;

	DesktopLayout   dl;
	dl.m_pWindow = pWindow;
	
	// ����һ����ʱ��DC�����ڲ��ֹ����в����ַ����Ĵ�С
	HDC hDC = UI_GetCacheDC();

	HRDC hRDC = GetHRDC(hDC,pWindow->m_hWnd);
	dl.Arrange(hRDC);
	ReleaseHRDC(hRDC);

	::UI_ReleaseCacheDC(hDC);
}

//
//	��һ������״̬�����ı�ʱ���������Ĳ���
//	
void UI_UpdateLayout( WindowBase* pObj, BOOL bRedraw )
{
	if( NULL == pObj )
		return;

	HDC hDC = UI_GetCacheDC();
	HRDC hRDC = GetHRDC(hDC, pObj->m_hWnd);
//	pObj->PrepareDC(hRDC);   // ���ڲ�����WindowBase* pObj�Ĵ�С��������û��ȥ����DC���ԣ������Ҫ����������һ��
	
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
//	����ǰ·����չΪȫ·������uidll��Ϊ��ǰ·��
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
// �ַ������
//
//	Remark:
//		����DLL֮��ʹ��stl��Ϊ�������д��ݣ��ᵼ���ڴ��ͷ�ʱ�ı������������û��ֱ��ȥ����UI_Split����
//		����ֱ����ULDLL����д��ôһ������
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
//	����Ӧ�ó�������ʱ��Ҫ�õ�HDC������Ե��øú������л�ȡ��������
//	UI_ReleaseCacheDC�ͷ�
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
//	�ڴ���һ������֮ǰ�����øú�������HWND��Ӧ�Ĵ��ڶ����ָ�룬Ȼ���ڴ��ڵĵ�һ����Ϣ�У�
//	��UI_ExtractCreateWndData�ٻ�ȡ���������ű��������atl��thunk���������໯
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
		// ��ȡ�����Ϣ�Ĵ������ڵĶ��㴰�ڣ���Ϊ������������������㴰�ڽ��е�
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

		// �ж���������Ƿ�����UI�����һ�����㴰��
		WindowBase* pWindow = g_pUIApplication->m_TopWindowMgr.GetWindowBase(hWndTop);
		if( NULL == pWindow )
			return FALSE;

		// ����
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

