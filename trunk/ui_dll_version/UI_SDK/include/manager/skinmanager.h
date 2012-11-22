#pragma once
namespace UI
{
	class ISkinInfoManager
	{
	public:
		virtual ~ISkinInfoManager() = 0 {}
		virtual bool Load(const String& strDataSource ) = 0;
		virtual bool Save() = 0;
		virtual void SetSkinInfoParse(ISkinInfoParse* p) = 0;
		virtual bool GetSkinInfo( CPojo_SkinInfo** pRet ) = 0;
	};

	//
	//	皮肤属性 -- 对应 ui.xml
	//
	class SkinInfoManager : public ISkinInfoManager
	{
	public:
		SkinInfoManager();
		~SkinInfoManager();

		ISkinInfoParse*   GetSkinInfoParse(){ return m_pSkinInfoParse; }
		void              SetSkinInfoParse(ISkinInfoParse* p);
		bool              GetSkinInfo( CPojo_SkinInfo** pRet );

		void              SetSkinInfoImagePath(const String& str ){ m_skinInfo.SetImageXmlPath(str); }
		
		bool              Load(const String& strDataSource );
		bool              Save();

	private:
		// 数据持久层
		ISkinInfoParse*  m_pSkinInfoParse;

		// 属性对象
		CPojo_SkinInfo   m_skinInfo;

	};

	class ISkinManager
	{
	public:
		virtual ~ISkinManager() = 0{};
	};
	//
	//	一款皮肤资源管理
	//	
	class SkinManager : public ISkinManager
	{
	public:
		SkinManager(CPojo_ProjectSkinItem* pSkinItem);
		~SkinManager();

	public:
		ISkinInfoManager*    GetSkinInfoManager() { return m_pSkinInfoMgr; }
		ImageManager*        GetImageManager()    { return m_pImageMgr;    }
		ColorManager*        GetColorManager()    { return m_pColorMgr;    }
		FontManager*         GetFontManager()     { return m_pFontMgr;     }
		StyleManager*        GetStyleManager()    { return m_pStyleMgr;    }
		LayoutManager*       GetLayoutManager()   { return m_pLayoutMgr;   }
		

		bool                 SetImageManager(ImageManager* pImageManager);
		bool                 SetColorManager(ColorManager* pColorManager);
		bool                 SetFontManager(FontManager* pFontManager);
		bool                 SetStyleManager(StyleManager* pStyleManager);
		bool                 SetLayoutManager(LayoutManager* pLayoutManager);

		bool                 GetSkinName(TCHAR* szName);
		const String         GetSkinName();

		bool                 LoadSkinInfo();
		bool                 Save(UI_RESOURCE_TYPE eResType);
		bool                 ReloadResource(UI_RESOURCE_TYPE eResType);

		bool                 GetResourceDataSourcePath( UI_RESOURCE_TYPE eResType, String& strRet );
//		bool                 GetImage( const String& strImageID, UIImage** ppImage );
		bool                 GetColor( const String& strColorID, UIColor** pCol );
//		bool                 GetFont( const String& strImageID, UIFont** ppFont );
		bool                 GetCursor( const String& strCursorID, UICursor** ppCursor );

		HRFONT               GetFont( const String& strFontID,  GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		HRFONT               GetDefaultFont( GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		HRFONT               GetFontEx( HRFONT hRFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		HRBITMAP             GetImage( const String& strImageID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		GifImageBase*        GetGifImage(const String& strID);

		bool                 LoadLayout(Object* pRootObj);
		bool                 ReLoadLayout( Object* pRootObj, list<Object*>& listAllChild );
		bool                 LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle );
		bool                 ChangeSkinHLS(short h, short l, short s, int nFlag);
		Menu*                LoadMenu( const String& strMenuId );

		int                  GetImageCount( );
		bool                 GetImageItemInfo( int nIndex, IImageItemInfo** ppImageItemInfo );
		int                  GetColorCount( );
		bool                 GetColorItemInfo( int nIndex, IColorItemInfo** ppColorItemInfo );
		int                  GetFontCount( );
		bool                 GetFontItemInfo( int nIndex, IFontItemInfo** ppFontItemInfo );
		int                  GetStyleCount( );
		bool                 GetStyleItemInfo( int nIndex, IStyleItemInfo** ppStyleItemInfo );

	private:
		CPojo_ProjectSkinItem* m_pPojoSkinItem;  // 提供本皮肤的一些基本信息数据
		ISkinInfoManager*      m_pSkinInfoMgr;   // 皮肤各资源信息解析

		ImageManager*          m_pImageMgr;
		ColorManager*          m_pColorMgr;
		FontManager*           m_pFontMgr;
		StyleManager*          m_pStyleMgr;
		LayoutManager*         m_pLayoutMgr;
	};

}