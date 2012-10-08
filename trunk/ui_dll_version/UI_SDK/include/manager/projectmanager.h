#pragma once
namespace UI
{
	//////////////////////////////////////////////////////////////////////////
	//
	//	工程属性 -- 对应 xxx.uiproj
	//
	//		通过皮肤句柄HSKIN，向各皮肤skinmanager分发消息
	//

	// 在stdafx.h中定义(本工程中暂不使用，直接使用类型转换)
	

	class UIAPI ProjectManager
	{
	public:
		ProjectManager(); 
		~ProjectManager(); 

		// ---------- 
		bool Initialize( const String& strUIProjXmlPath );
		bool ChangeSkin( const String& strSkinName );
		bool ChangeSkin( int nIndex );
		bool ChangeSkinHLS( short h, short l, short s, int nFlag );
		bool ChangeSkin( CPojo_ProjectSkinItem* pSkinItem );

		// ---------

		IProjectParse*   GetProjectParse(){ return m_pProjectParse; }
		void             SetProjectParse(IProjectParse* p);

		CPojo_Project&   GetProjectRef() { return m_pojoProject; }
		bool             GetProjectDirectory( TCHAR* szRetDir );
		bool             GetProjectDirectory( String& );
		bool             GetProjectPath( TCHAR* szRetPath );
		bool             GetProjectName( TCHAR* szName, int* pnSize );

		bool             CreateProject( const String& strProjDir, const String& strProjName );
		bool             OpenProject( const String& strProjFilePath, bool bOnlyLoadActiveSkin );
		HSKIN            CreateSkin( const String& strSkinName,     const String& strImageXmlPath, 
			                         const String& strColorXmlPath, const String& strFontXmlPath,
									 const String& strLayoutXmlPath,const String& strStyleXmlPath );
		SkinManager*     LoadSkin(CPojo_ProjectSkinItem*  pSkinItem);
		bool             ReloadResource( HSKIN hSkin, UI_RESOURCE_TYPE eResType );

		bool             Save( HSKIN hSkin, UI_RESOURCE_TYPE eResType );
		bool             SaveProjectInfo();

		HSKIN            AddSkinMgrItem( const String& strSkinName, const String& strSkinXmlFullPath );
		bool             CreateSkinImageMgr( SkinManager* pSkinManager, const String&  strXmlPath );
		bool             CreateSkinColorMgr( SkinManager* pSkinManager, const String&  strXmlPath );
		bool             CreateSkinFontMgr ( SkinManager* pSkinManager, const String&  strXmlPath );
		bool             CreateSkinStyleMgr( SkinManager* pSkinManager, const String&  strXmlPath );
		bool             CreateSkinLayoutMgr(SkinManager* pSkinManager, const String&  strXmlPath );

		bool             InsertImageItem( HSKIN hSkin, const String& strID, const String& strPath );
		bool             InsertColorItem( HSKIN hSkin, const String& strID, const String& strColor );
		bool             InsertFontItem ( HSKIN hSkin, const String& strID, LOGFONT* pLogFont );
		bool             InsertStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit );
		bool             InsertStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
		bool             ModifyFontItem ( HSKIN hSkin, const String& strID, LOGFONT* pLogFont );
		bool             ModifyImageItem( HSKIN hSkin, const String& strID, const String& strPath );
		bool             ModifyColorItem( HSKIN hSkin, const String& strID, const String& strColor );
		bool             ModifyStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit );
		bool             ModifyStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
		bool             RemoveImageItem( HSKIN hSkin, const String& strID );
		bool             RemoveFontItem ( HSKIN hSkin, const String& strID );
		bool             RemoveColorItem( HSKIN hSkin, const String& strID );
		bool             RemoveStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID );
		bool             RemoveStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey );

		int              GetSkinCount();
		HSKIN            GetSkinHandle( int nIndex );
		bool             GetSkinName( HSKIN hSkin, String& str );
		bool             GetSkinName( int i, String& str );
		bool             GetSkinItemInfo( int nIndex, IProjectSkinItemInfo** pSkinItem );
		bool             GetSkinItemInfo( HSKIN hSkin, IProjectSkinItemInfo** pSkinItem );
		int              GetSkinImageCount( HSKIN );
		bool             GetSkinImageItemInfo( HSKIN, int nIndex, IImageItemInfo** ppImageItemInfo );
		int              GetSkinColorCount( HSKIN );
		bool             GetSkinColorItemInfo( HSKIN, int nIndex, IColorItemInfo** ppColorItemInfo );
		int              GetSkinFontCount( HSKIN );
		bool             GetSkinFontItemInfo( HSKIN, int nIndex, IFontItemInfo** pFontItemInfo );
		int              GetSkinStyleCount( HSKIN );
		bool             GetSkinStyleItemInfo( HSKIN, int nIndex, IStyleItemInfo** ppStyleItemInfo );

		bool             GetResourceDataSourcePath( HSKIN hSkin, UI_RESOURCE_TYPE eResType, String& strRet );
//		bool             GetImage( const String& strImageID, UIImage** ppImage, HSKIN hSkin = NULL );
		bool             GetColor( const String& strColorID, UIColor** pCol, HSKIN hSkin = NULL );
//		bool             GetFont( const String& strFontID, UIFont** ppFont, HSKIN hSkin = NULL );
		bool             GetCursor( const String& strCursorID, UICursor** ppCursor, HSKIN hSkin = NULL );
		bool             LoadLayout(Object* pRootObj);
		bool             LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle );
		Menu*            LoadMenu( const String& strMenuId );

		HRFONT           GetFont( const String& strFontID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI, HSKIN hSkin = NULL );
		HRFONT           GetDefaultFont( GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI, HSKIN hSkin = NULL );
		HRFONT           GetFontEx(HRFONT hRFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI, HSKIN hSkin = NULL );
		HRBITMAP         GetImage( const String& strImageID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI, HSKIN hSkin = NULL );

		bool             SetActiveSkin( HSKIN hSkin );
		int              GetActiveSkinIndex();

	private:
		int              _HSKIN_2_Index( HSKIN hSkin );
		SkinManager*     _GetSkinManagerByHSKIN( HSKIN hSkin );

	private:
		// 数据持久层
		IProjectParse*    m_pProjectParse;
		bool              m_bDirty;

		// 持久性属性对象
		CPojo_Project     m_pojoProject;

		// 运行时数据
		vector<SkinManager*>   m_vSkinMgrs;         // 所有的皮肤列表
		SkinManager*           m_pCurActiveSkinMgr; // 当前正在使用的皮肤
	};
}