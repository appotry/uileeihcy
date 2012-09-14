#pragma  once

// 如果要从标准 C++ 库中的类型派生，并且要编译调试版本 (/MTd)，而且编译器错误信息引用 _Container_base，则在 Microsoft Visual C++ 2005 中可以忽略 C4251。---vector<Object*>会报错
#pragma warning(disable:4251)
// _vswprintf': This function or variable may be unsafe. Consider using vswprintf_s instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#pragma warning(disable:4996)

#include <windows.h>
#include <CommCtrl.h>
#pragma comment(lib,"comctl32.lib")  // TOOLTIP support
#include <vector>
#include <list>
#include <map>
#include <string>
#include <fstream>
#include <utility>
#include <iostream>
#include <tchar.h>
#include <assert.h>
#include <algorithm>

using namespace std;

#ifdef _WTYPES_NS
#undef _WTYPES_NS
#endif
#define _WTYPES_NS

#include <atltypes.h>
#define ATLTYPES_INLINE inline
#include <atltypes.inl>

#ifndef __ATLSTDTHUNK_H__  // 如果其它工程已经引用了，则不使用自己修改过了的
#include "atl\atlstdthunk.h"
#endif
#define __ATLSTDTHUNK_H__  // 使用自己的ATLSTDTHUNK.H

#include <atlbase.h>
#include <atlapp.h>
#include <atlwin.h>

#include <atlcrack.h>      // TODO: WTL的头文件，可以考虑移植


// 前置声明
namespace UI
{
class UIImage;
class UIFont;
class UIColor;
class UICursor;

class Object;
class Panel;
class Control;
class WindowBase;
class Window;
class Menu;

class UIApplication;
class IProjectSkinItemInfo;
class IImageItemInfo;
class IFontItemInfo;
class IColorItemInfo;
class IStyleItemInfo;
}


#ifndef _UNICODE 
	#error "UI sdk requirs a unicode build."
#endif

using namespace UI;

#include "..\\UIUtil\\h\\util.h"
#ifdef _UNICODE
#	pragma comment(lib,"..\\UI_SDK\\lib\\uiutil_u.lib")
#else
#	pragma comment(lib,"..\\UI_SDK\\lib\\uiutil.lib")
#endif

// 不需要使用import，直接include就行了
//#import "..\\UI_SDK\\bin\\UILog.dll" no_namespace named_guids  
// #include <comutil.h>  .. _bstr_t 的头文件
// #pragma comment(lib, "comsuppw.lib") .. _bstr_t 的库文件
#include "..\UILog\UILog_i.h"


#include "define.h"
#include "atl\image.h"
#include "manager\rendermanager\rendermanager.h"

//
// Global Function
//
namespace UI
{
	// 导出函数

	//////////////////////////////////////////////////////////////////////////
	// 
	// 外部调用接口

	UIAPI bool     UI_Initialize( const String& strUIProjXmlPath );
	UIAPI bool     UI_Release();
	UIAPI bool     UI_ChangeSkin( const String& strSkinName );
	UIAPI bool     UI_ChangeSkin( int nSkinIndex );
	UIAPI bool     UI_ChangeSkinHLS( short h, short l, short s, int nFlag );
	UIAPI void     UI_SetDesignMode( bool bDesignMode );
	UIAPI bool     UI_RegisterUIObjectCreateData( const TCHAR* szXmlName, s_UICreateInstancePtr pFunPtr);

//	UIAPI bool     UI_GetImage( const String& strImageID, UIImage** ppImage, HSKIN hSkin = NULL );
	UIAPI bool     UI_GetColor( const String& strColorID, UIColor** pColor, HSKIN hSkin = NULL  );
//	UIAPI bool     UI_GetFont( const String& strFontID, UIFont** ppFont, HSKIN hSkin = NULL  );
	UIAPI bool     UI_GetCursor( const String& strCursorID, UICursor** ppCursor, HSKIN hSkin = NULL );

	UIAPI void     UI_AttachFont(HRFONT* pOut, HFONT hFont, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI);
	UIAPI HRFONT   UI_GetFont( const String& strFontID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI, HSKIN hSkin = NULL );
	UIAPI HRFONT   UI_GetDefaultFont( GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI, HSKIN hSkin = NULL );
	UIAPI HRFONT   UI_GetFontEx( HRFONT hRFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI, HSKIN hSkin = NULL  );
	UIAPI HRFONT   UI_CopyFont( HRFONT hRFont );
	UIAPI void     UI_ReleaseFont( HRFONT hRFont );
	UIAPI GRAPHICS_RENDER_TYPE UI_GetFontRenderType(HRFONT hRFont);
	UIAPI HFONT    UI_GetFontHFONT(HRFONT hRFont);

	UIAPI Menu*    UI_LoadMenu( const String& strMenuID );

	UIAPI HRBITMAP UI_GetBitmap( const String& strImageID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI, HSKIN hSkin = NULL );
	UIAPI void     UI_ReleaseBitmap( HRBITMAP hBitmap );
	UIAPI int      UI_GetBitmapWidth( HRBITMAP hBitmap );
	UIAPI int      UI_GetBitmapHeight( HRBITMAP hBitmap );

	UIAPI bool     UI_SetActiveSkin( HSKIN hSkin );
	UIAPI HSKIN    UI_GetSkinHandle( int nIndex );
	UIAPI int      UI_GetActiveSkinIndex();

	UIAPI void     UI_UpdateLayout( WindowBase* pObj, BOOL bRedraw=TRUE );


	//////////////////////////////////////////////////////////////////////////
	//
	// UI Builder 获取函数

	UIAPI bool     UI_GetProjectName( TCHAR* szRet, int* pnSize );
	UIAPI bool     UI_GetProjectPath( TCHAR* szRet );
	UIAPI bool     UI_GetProjectDirectory( TCHAR* szRet );
	
	UIAPI int      UI_GetResourceDataSourcePath( HSKIN hSkin, UI_RESOURCE_TYPE eResType, TCHAR* szRet, int nSize );

	UIAPI int      UI_GetSkinCount( );
	UIAPI int      UI_GetSkinName( HSKIN hSkin, TCHAR* szRet, int nSize );
	UIAPI int      UI_GetSkinName( int nIndex, TCHAR* szRet, int nSize );
	
	UIAPI int      UI_GetProjectSkinCount();
	UIAPI bool     UI_GetProjectSkinItemInfo( int nIndex, IProjectSkinItemInfo** ppSkinItem );

	UIAPI int      UI_GetSkinImageCount( HSKIN );
	UIAPI bool     UI_GetSkinImageItemInfo( HSKIN, int nIndex, IImageItemInfo** ppImageItemInfo );

	UIAPI int      UI_GetSkinColorCount( HSKIN );
	UIAPI bool     UI_GetSkinColorItemInfo( HSKIN, int nIndex, IColorItemInfo** ppColorItemInfo );

	UIAPI int      UI_GetSkinFontCount( HSKIN );
	UIAPI bool     UI_GetSkinFontItemInfo( HSKIN, int nIndex, IFontItemInfo** pFontItemInfo );

	UIAPI int      UI_GetSkinStyleCount( HSKIN );
	UIAPI bool     UI_GetSkinStyleItemInfo( HSKIN, int nIndex, IStyleItemInfo** ppStyleItemInfo );

	//////////////////////////////////////////////////////////////////////////
	//
	// UI BUILDER 接口

	UIAPI bool     UI_CreateProject  ( const String& strProjDir, const String& strProjName );
	UIAPI HSKIN    UI_CreateSkin     ( const String& strSkinName,     const String& strImageXmlPath,
		                               const String& strColorXmlPath, const String& strFontXmlPath,
									   const String& strLayoutXmlPath,const String& strStyleXmlPath );
	UIAPI bool     UI_OpenProject( const String& strProjFilePath );
//	UIAPI HSKIN    UI_GetNextProjectSkin( HSKIN hSkin = NULL );

	UIAPI bool     UI_InsertImageItem( HSKIN hSkin, const String& strID, const String& strPath );
	UIAPI bool     UI_InsertColorItem( HSKIN hSkin, const String& strID, const String& strColor );
	UIAPI bool     UI_InsertFontItem ( HSKIN hSkin, const String& strID, LOGFONT* pLogfont );
	UIAPI bool     UI_InsertStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit );
	UIAPI bool     UI_InsertStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
	UIAPI bool     UI_ModifyImageItem( HSKIN hSkin, const String& strID, const String& strPath );
	UIAPI bool     UI_ModifyFontItem ( HSKIN hSkin, const String& strID, LOGFONT* pLogfont );
	UIAPI bool     UI_ModifyColorItem( HSKIN hSkin, const String& strID, const String& strColor );
	UIAPI bool     UI_ModifyStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit );
	UIAPI bool     UI_ModifyStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strKey, const String& strValue );
	UIAPI bool     UI_RemoveImageItem( HSKIN hSkin, const String& strID );
	UIAPI bool     UI_RemoveFontItem ( HSKIN hSkin, const String& strID );
	UIAPI bool     UI_RemoveColorItem( HSKIN hSkin, const String& strID );
	UIAPI bool     UI_RemoveStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID );
	UIAPI bool     UI_RemoveStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strKey );

	UIAPI bool     UI_ReloadResource(HSKIN hSkin, UI_RESOURCE_TYPE eResType=UI_RESOURCE_NULL);
	UIAPI void     UI_Save(HSKIN hSkin=NULL, UI_RESOURCE_TYPE eResType=UI_RESOURCE_NULL);

	UIAPI void     _cdecl UI_LOG_DEBUG( TCHAR*, ... );
	UIAPI void     _cdecl UI_LOG_INFO( TCHAR*, ... );
	UIAPI void     _cdecl UI_LOG_WARN( TCHAR*, ... );
	UIAPI void     _cdecl UI_LOG_ERROR( TCHAR*, ... );
	UIAPI void     _cdecl UI_LOG_FATAL( TCHAR*, ... );

	//////////////////////////////////////////////////////////////////////////
	// 非导出函数，仅全局调用

	bool     UI_LoadLayout( Object* pRootObj );
	bool     UI_LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle );
	void     UI_UpdateTopWindowLayout( WindowBase* pWindow );

	void     _cdecl MOUSE_LOG_DEBUG( TCHAR*, ... );

	// helper 方法
	void     UI_ExtendPath( String& strPath );
	void     UI_Split( const String& str, TCHAR szSep, vector<String>& pvRet );
	
	HDC      UI_GetCacheDC();
	void     UI_ReleaseCacheDC(HDC hDC);
	void     UI_AddCreateWndData(_AtlCreateWndData* pData, void* pThis);
	void*    UI_ExtractCreateWndData();
	HWND     UI_GetForwardPostMessageWnd();

	BOOL     UI_IsDialogMessage(MSG* pMsg);
}

#include "message.h"
#include "manager\keyboardmanager.h"
#include "manager\mousemanager.h"
#include "manager\rendermanager\gdirender.h"
#include "manager\rendermanager\gdiplusrender.h"
#include "manager\rendermanager\direct2drender.h"
#include "xmlparse\pojo\pojo.h"
#include "xmlparse\dao\idao.h"
#include "..\src\xmlparse\dao\xml\xmldao.h"
#include "manager\imagemanager.h"
#include "manager\colormanager.h"
#include "manager\fontmanager.h"
#include "manager\stylemanager.h"
#include "manager\layoutmanager.h"
#include "manager\skinmanager.h"
#include "manager\projectmanager.h"
#include "manager\topwindowmanager.h"
#include "manager\layout.h"
#include "manager\animate.h"
#include "manager\render.h"
#include "manager\timermanager.h"
#include "objtree.h"
#include "objcreator.h"
#include "object.h"

#include "control\buttonrender.h"
#include "control\control.h"
#include "control\HwndHost.h"
#include "control\scrollbar.h"
#include "window\panel.h"
#include "panel\groupbox.h"
#include "panel\scrollpanel.h"
#include "window\SyncWindowHelper.h"
#include "window\windowbase.h"
#include "window\window.h"
#include "window\customwindow.h"
#include "window\popupcontrolwindow.h"

// control ,在添加include的时候，别忘了去LayoutXmlParse::mapXmlToClass
#include "control\button.h"
#include "control\label.h"
#include "control\progress.h"
#include "control\CaretWindow.h"
#include "control\edit.h"
#include "control\richedit.h"
#include "control\listbox.h"
#include "control\menu.h"
#include "control\combobox.h"
#include "control\ledctrl.h"

#include "uiapplication.h"


