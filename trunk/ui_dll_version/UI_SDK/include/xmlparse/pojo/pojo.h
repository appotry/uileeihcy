#pragma once

//
//	声明一个String类型变量的get/set方法
//
//	参数:
//		xxx
//			如果成员名为 m_strName，则xxx取值为 Name
//
//	注意：
//		在其它dll/exe模块中不要调用Get方法,而应调用GetRef方法,否则析构时会造成heap错误
//
#define DECLARE_STRING_SETGET(xxx)    \
	String           Get##xxx()      { return m_str##xxx; } \
	const String&    Get##xxx##Ref() { return m_str##xxx; } \
	void             Set##xxx( const String& str ) { m_str##xxx = str; };

#define DECLARE_bool_SETGET(xxx) \
	bool    Get##xxx() { return m_b##xxx; } \
	void    Set##xxx( bool b ) { m_b##xxx = b; }

#define DECLARE_byte_SETGET(xxx) \
	unsigned char  Get##xxx() { return m_byte##xxx; } \
	void    Set##xxx( byte b ) { m_byte##xxx = b; }

#define DECLARE_INT_SETGET(xxx) \
	int     Get##xxx() { return m_n##xxx; } \
	void    Set##xxx( int n ) { m_n##xxx = n; }

namespace UI
{
	class SkinManager;

	// 当前皮肤的色调数据
	struct SKIN_HLS_INFO
	{
		char   h;       // -120 ~ 120
		char   l;       // -100 ~ 100
		char   s;       // -100 ~ 100
		int   nFlag;
	};

	//
	//	用于暴露给外部的皮肤信息
	//
	class IProjectSkinItemInfo
	{
	public:
		virtual ~IProjectSkinItemInfo() = 0 {}
		virtual const String& GetSkinNameRef() = 0;
		virtual SKIN_HLS_INFO* GetSkinHlsInfo()= 0;
		virtual void SetHLS(char h, char l, char s, int nFlag) = 0;
	};

	//
	//	工程中的皮肤对象属性
	//
	class CPojo_ProjectSkinItem : public IProjectSkinItemInfo
	{
	public:
		CPojo_ProjectSkinItem();

	private:
		String   m_strSkinName;          // 皮肤项名称
		String   m_strSkinXmlPath;       // 皮肤描述文件所在路径

		SKIN_HLS_INFO  m_sHlsInfo;

	public:
		DECLARE_STRING_SETGET( SkinName );
		DECLARE_STRING_SETGET( SkinXmlPath );
		void    SetHLS(char h, char l, char s, int nFlag);
		virtual SKIN_HLS_INFO* GetSkinHlsInfo() { return &m_sHlsInfo; }
	};

	//
	//	一个工程对象属性
	//
	class CPojo_Project
	{
	public:
		CPojo_Project(){}
		~CPojo_Project();
	private:
		String   m_strProjectName;       // 工程名称
		String   m_strActiveSkinName;    // 当前使用的皮肤名称

		vector<CPojo_ProjectSkinItem*>   m_vSkins;   // 所有皮肤列表
		
	public:
		DECLARE_STRING_SETGET( ProjectName );
		DECLARE_STRING_SETGET( ActiveSkinName );
		
		CPojo_ProjectSkinItem*  GetSkinItem( int nIndex );
		CPojo_ProjectSkinItem*  GetSkinItem( const String& strSkinName );
		CPojo_ProjectSkinItem*  GetSkinItem( SkinManager* pSkinMgr );
		CPojo_ProjectSkinItem*  GetActiveSkinItem( );
		int  GetSkinItemCount();
		int  GetActiveSkinItemIndex();
		bool AddSkinItem( CPojo_ProjectSkinItem* pNewItem );
		bool GetActiveSkinPath( TCHAR* szPath );   // 获取当前活跃皮肤所在路径
		bool GetSkinPath( const String& strSkinName, TCHAR* szPath );
	};


	//
	//	皮肤信息描述，各元素xml位置 
	//
	class CPojo_SkinInfo
	{
	private:
		String   m_strImageXmlPath;       // 全路径，保存到文件中的才是相对路径 
		String   m_strFontXmlPath;
		String   m_strColorXmlPath;
		String   m_strStyleXmlPath;
		String   m_strLayoutXmlPath;
		String   m_strI18nXmlPath;

	public:

		DECLARE_STRING_SETGET( ImageXmlPath );
		DECLARE_STRING_SETGET( FontXmlPath );
		DECLARE_STRING_SETGET( ColorXmlPath );
		DECLARE_STRING_SETGET( StyleXmlPath );
		DECLARE_STRING_SETGET( LayoutXmlPath );
		DECLARE_STRING_SETGET( I18nXmlPath );
	};


	//
	//	为了外部能够遍历image列表而提供给外部的接口
	//
	class IImageItemInfo
	{
	public:
		virtual ~IImageItemInfo()=0 {}
		virtual const String& GetIDRef() = 0;
		virtual const String& GetPathRef() = 0;

		virtual long Release(){ return 0; }
	};

	//
	//	image中的一项信息数据
	//
	class CPojo_ImageItem : public IImageItemInfo
	{
	public:
		CPojo_ImageItem();
		~CPojo_ImageItem();

	private:
		String    m_strID;      // image id
		String    m_strPath;    // image path

		bool      m_bUseSkinHLS;         // 该图片是否参与皮肤色调改变 
		bool      m_bMustHasAlphaChannel;// 该图片是否需要创建alpha channel。1. 有alpha channel的图片一率采用gdi创建，采用alphablend绘制。2. 没有alpha channel的gdi图片将无法在分层窗口上面绘制
		ATTRMAP   m_mapAttribute;        // 为该图片配置的属性，例如imagelist的count，icon的width height
		IMAGE_ITEM_TYPE m_eType;         // 图片类型

		ImageData*            m_pOriginImageData;    // 该图片的原始数据（改变图片色调时使用）
		GDIRenderBitmap*      m_pGdiBitmap;          // 外部引用

    //  注：为了提高绘图效率，同时也避免一张图片加载多种形式，尽量使用GDI(AlphaBlend)来绘图。
	//	    需要注意的是：如果需要使用alpha通道时，必须强制创建一个32位的GDI BITMAP
 	//	GdiplusRenderBitmap*  m_pGdiplusBitmap;      // 外部引用
	//	Direct2DRenderBitmap* m_pDirect2DBitmap;     // 外部引用

	public:
		DECLARE_STRING_SETGET( ID );
		DECLARE_STRING_SETGET( Path );
		DECLARE_bool_SETGET(UseSkinHLS);

		bool     ModifyImage( const String& strPath );
		bool     ModifyHLS( short h, short l, short s, int nFlag );
		bool     ModifyHLS( IRenderBitmap* pBitmap, short h, short l, short s, int nFlag );
		HRBITMAP GetImage(GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI, bool* pbFirstTimeCreate=NULL);
		void     SetAttribute(const ATTRMAP& mapAttr);
	};

	//
	//	image列表
	//
	class CPojo_Image
	{
	public:
		CPojo_Image(SkinManager*  pSkinMgr);
		~CPojo_Image();
	private:
		vector<CPojo_ImageItem*>   m_vImages;

	public:
		bool LoadItem(ATTRMAP& mapAttr, const String& strFullPath);

		int  GetImageCount(); 
		CPojo_ImageItem* GetImageItem( int nIndex );
		CPojo_ImageItem* GetImageItem( const String& strID );

		bool InsertImage( const String& strID, const String& strPath, CPojo_ImageItem** pRet=NULL );
		bool ModifyImage( const String& strID, const String& strPath );
		bool RemoveImage( const String& strID );
		bool Clear();
		bool ChangeSkinHLS(short h, short l, short s, int nFlag);

		HRBITMAP GetImage( const String& strID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );

	private:
		SkinManager*  m_pSkinMgr;
	};

	//
	//	鼠标样式
	//
	class UIAPI UICursor
	{
	private:
		UICursor( UICursor** ppOutRef );
	public:
		~UICursor();

		static void CreateInstance( const String strCurFilePath, UICursor** pOutRef );
		static void CreateInstance( LPTSTR nCursorID, UICursor** ppOutRef );
		HCURSOR  GetCursor();
		bool     Load( LPTSTR nCursorID );
		void     Attach(HCURSOR hCursor);
		HCURSOR  Detach();
		long     AddRef();
		long     Release();

	private:
		long      m_dwRef;
		HCURSOR   m_hCursor;

		UICursor** m_pOutRef;
	};
	
	class CPojo_CursorItem
	{
	public:
		CPojo_CursorItem();

		DECLARE_STRING_SETGET( ID );
		DECLARE_STRING_SETGET( CurFilePath );

		bool  GetCursor( UICursor** ppCursor );
		bool  ModifyCursor( const String& strCurFilePath );

	public:
		String     m_strID;
		String     m_strCurFilePath;     // .cur文件路径
		UICursor*  m_pCursor;             
	};

	class CPojo_Cursor
	{
	public:
		CPojo_Cursor();
		~CPojo_Cursor();

	public:
		CPojo_CursorItem* GetCursorItem( int nIndex );
		CPojo_CursorItem* GetCursorItem( const String& strID );
		int   GetCursorCount();
		bool  GetCursor( const String& strID, UICursor** pCursorRet );
		bool  InsertCursor( const String& strID, const String& strCurFilePath );
		bool  ModifyCursor( const String& strID, const String& strCursor );
		bool  RemoveCursor( const String& strID );
		void  Clear();

	private:
		vector<CPojo_CursorItem*>   m_vCursors;
	};


	class CPojo_GifItem
	{
	public:
		CPojo_GifItem();
		~CPojo_GifItem();

	private:
		String      m_strID;      // image id
		String      m_strPath;    // image path
		bool        m_bUseSkinHLS;         // 该图片是否参与皮肤色调改变 
		ImageData*  m_pOriginImageData;    // 该图片的原始数据（改变图片色调时使用）

	public:
		bool  ModifyGif(const String& strPath) { UIASSERT(0); return false; } // TODO:
		bool  ModifyHLS(short h, short l, short s, int nFlag){ UIASSERT(0); return false; } // TODO:
		void  SetAttribute(const ATTRMAP& mapAttr);
		GifImageBase*  GetGifImage();

		DECLARE_STRING_SETGET( ID );
		DECLARE_STRING_SETGET( Path );
		DECLARE_bool_SETGET(UseSkinHLS);

	public:
		ATTRMAP   m_mapAttribute;
		GifImageBase*   m_pGifImage;
	};
	class CPojo_Gif
	{
	public:
		CPojo_Gif(){};
		~CPojo_Gif();

		bool LoadItem(ATTRMAP& mapAttr, const String& strFullPath);

		int  GetGifCount(); 
		CPojo_GifItem* GetGifItem( int nIndex );
		CPojo_GifItem* GetGifItem( const String& strID );

		bool InsertGif(const String& strID, const String& strPath, CPojo_GifItem** pRet);
		bool ModifyGif(const String& strID, const String& strPath);
		bool RemoveGif(const String& strID);
		bool Clear();
		bool ChangeSkinHLS(short h, short l, short s, int nFlag){UIASSERT(0); return false;};  // TODO:

		GifImageBase* GetGif( const String& strID);

	private:
		vector<CPojo_GifItem*>   m_vGifs;
	};
	

	//
	//	让外界通过指针来获取颜色资源，这样每次颜色变化时，可经立即通过获取指针指向的内容来刷新界面
	//
	class UIAPI UIColor
	{
	private:
		UIColor( UIColor**  ppOutRef );
	public:
		~UIColor();
		static   void  CreateInstance( const String& strColor, UIColor** ppOutRef );
		static   void  CreateInstance( const COLORREF& color, UIColor** ppOutRef );

		COLORREF GetColor();
		void     SetColor(COLORREF col);
		long     AddRef();
		long     Release();

	private:
		long      m_dwRef;
		COLORREF  m_col;
		UIColor**  m_pOutRef;  
	};
	
	// 外部获取信息的接口
	class IColorItemInfo
	{
	public:
		virtual ~IColorItemInfo() = 0 {};
		virtual const String& GetIDRef() = 0;
		virtual const String& GetColorStringRef() = 0;

		virtual long Release(){ return 0; }
	};
	//
	//	color中的一项信息数据
	//
	class  CPojo_ColorItem : public IColorItemInfo
	{
	public:
		CPojo_ColorItem();
		~CPojo_ColorItem();

	private:
		String    m_strID;
		String    m_strColor;     // 颜色字符串
		UIColor*  m_pColor;       // 颜色值，由m_strColor转换而来

		COLORREF* m_pOriginColorValue;  // 该COLOR未改变色调前的颜色值，用于还原

		// TODO: 可以再进行扩展。例如文字的黑白颜色变换，是根据文字后面的背景色来决定的，而不是配死的
		bool      m_bUseSkinHLS;  // 该COLOR是否参与皮肤色调改变 

	public:
		DECLARE_STRING_SETGET( ID );
		DECLARE_bool_SETGET(UseSkinHLS);
		const String&   GetColorStringRef();
		bool  GetColor( UIColor** pColor, bool *bFirstTimeCreate = NULL);
		void  SetColor( const String& strColor ); 

		void  SetAttribute(const ATTRMAP& mapAttr);
		bool  ModifyHLS( short h, short l, short s, int nFlag );
	};

	//
	//	color列表
	//
	class CPojo_Color
	{
	public:
		CPojo_Color(SkinManager* p){ m_pSkinMgr = p; }
		~CPojo_Color();
	private:
		vector<CPojo_ColorItem*>   m_vColors;

	public:
		CPojo_ColorItem* GetColorItem( int nIndex );
		CPojo_ColorItem* GetColorItem( const String& strID );
		int   GetColorCount() ;
		bool  GetColor( const String& strID, UIColor** pColorRet );
		bool  InsertColor( const String& strID, const String& strColor, CPojo_ColorItem** pItem );
		bool  ModifyColor( const String& strID, const String& strColor );
		bool  RemoveColor( const String& strID );
		void  Clear();

		bool  LoadItem(ATTRMAP& mapAttr, const String& strValue);
		bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

	private:
		SkinManager*  m_pSkinMgr;
	};

#if 0
	//
	//	描述一个字体对象引用，用于多个地方同时引用一个字体
	//
	class UIAPI UIFont 
	{
	private:
		UIFont(LOGFONT*  m_pLogFont, UIFont** ppOutRef);
	public:
		~UIFont();

	public:
		static void CreateInstance(LOGFONT* plogfont, UIFont** ppOutRef);
		HFONT    GetFont();
		LOGFONT* GetLogFont() { return m_pLogFont; }
		bool  Load( LOGFONT* plogfont );
		void  ModifyFont(LOGFONT* plogfont);
		void  Attach(HFONT hFont);
		HFONT Detach();
		long  AddRef();
		long  Release();

	private:
		long    m_dwRef;       
		HFONT   m_hFont;

	private:
		LOGFONT*  m_pLogFont;   // 引用CPojo_FontItem::m_lf   NULL 表示该HFONT是Attach得到的，不需要DeleteObject
		UIFont**  m_pOutRef;    // 引用CPojo_FontItem::m_pFont
	};
#endif
	

#define FONTITEM_FLAG_UNDERLINE 0x01
#define FONTITEM_FLAG_BOLD      0x02
#define	FONTITEM_FLAG_ITALIC    0x04
#define FONTITEM_FLAG_STRIKEOUT 0X08
	class IFontItemInfo
	{
	public:
		virtual ~IFontItemInfo() = 0 {}
		virtual const String& GetIDRef() = 0;
		virtual LOGFONT* GetLogFont() = 0;

		virtual long Release(){ return 0; }
	};
	//
	//	一条字体信息
	//
	class CPojo_FontItem : public IFontItemInfo
	{
	public:
		CPojo_FontItem();
		~CPojo_FontItem();

	public:
		DECLARE_STRING_SETGET( ID );
		LOGFONT* GetLogFont(){ return &m_lf; }
		void  GetFaceName( TCHAR* szOut ) { _tcscpy( szOut, m_lf.lfFaceName ); }
		void  SetFaceName( const String& str ) { _tcsncpy( m_lf.lfFaceName, str.c_str(), 31 ); m_lf.lfFaceName[31] = 0; }
		void  SetFontHeight( int nHeight ) { m_lf.lfHeight = nHeight; }
		int   GetFontSize( ) { return Util::FontHeight2Size(m_lf.lfHeight); }
		void  SetFontOrientation( int o ) { m_lf.lfOrientation=o; }
		int   GetFontOrientation(){return (int)m_lf.lfOrientation; }
		void  SetFontBold( bool b ) { b? m_lf.lfWeight=FW_BOLD : m_lf.lfWeight=FW_NORMAL; }
		bool  GetFontBold( ) { return m_lf.lfWeight==FW_BOLD; }
		void  SetFontItalic( bool b ) { b?m_lf.lfItalic=1:m_lf.lfItalic=0; }
		bool  GetFontItalic( ) { if(m_lf.lfItalic==0){return false;}else{return true;} }
		void  SetFontUnderline( bool b ) { b?m_lf.lfUnderline=1:m_lf.lfUnderline=0; }
		bool  GetFontUnderline( ) { if(m_lf.lfUnderline==0){return false;}else{return true;} }
		void  SetFontStrikeout( bool b ) { b?m_lf.lfStrikeOut=1:m_lf.lfStrikeOut=0; }
		bool  GetFontStrikeout( ) { if(m_lf.lfStrikeOut==0){return false;}else{return true;} }

		void   SetLogFont( LOGFONT* pLogFont );
		void   ModifyFont( LOGFONT* pLogFont );
		HRFONT GetFont( GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );

		WPARAM GetWParam() { return m_wParam; }
		LPARAM GetLParam() { return m_lParam; }
		void   SetWParam( WPARAM w ) { m_wParam = w; }
		void   SetLParam( LPARAM l ) { m_lParam = l; }

		bool   IsMyHRFont(HRFONT hRFont) { if( m_pGdiFont==hRFont || m_pGdiplusFont == hRFont ) {return true;}  return false;}

	private:
		String   m_strID;
		LOGFONT  m_lf;

		WPARAM   m_wParam;  
		LPARAM   m_lParam;  

		GDIRenderFont*       m_pGdiFont;
		GdiplusRenderFont*   m_pGdiplusFont;
		Direct2DRenderFont*  m_pD2DFont;
	};

	//
	//	font 列表
	//
	class UIAPI CPojo_Font
	{
	public:
		~CPojo_Font();
		 
	private:
		vector<CPojo_FontItem*>  m_vFonts;

	public:
		int  GetFontCount();
		CPojo_FontItem* GetFontItem( int nIndex );
		CPojo_FontItem* GetFontItem( const String& strID );
		bool InsertFont( const String& strID, LOGFONT* pLogFont );
		CPojo_FontItem* InsertFont( const String& strID, LOGFONT* pLogFont, WPARAM wParam, LPARAM lParam );
		bool ModifyFont( const String& strID, LOGFONT* pLogFont );
		bool RemoveFont( const String& strID );
		void Clear();

		bool GetHRFontID(HRFONT hRFont, String& strID);

		HRFONT GetFont( const String& strFontID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		HRFONT GetDefaultFont( GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		HRFONT GetFontEx( HRFONT hRFont, WPARAM wParam=0, LPARAM lParam=0, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
	};

	
	class UIAPI IStyleItemInfo
	{
	public:
		virtual ~IStyleItemInfo() = 0 {}
		virtual STYLE_SELECTOR_TYPE GetSelectorType() = 0;
		virtual const String& GetIDRef() = 0;
		virtual const String& GetInheritsRef() = 0;

		virtual int  GetAttributesCount() = 0;
		virtual bool GetAttribute( int nIndex, TCHAR* szkey, TCHAR* szValue ) = 0;

		static bool ParseInheritString(const String& strInherit, STYLE_SELECTOR_TYPE& eStyletype, TCHAR* szStyleName );
		static bool MakeInheritString(const STYLE_SELECTOR_TYPE& eStyletype, const String& strStypeName, TCHAR* szInherit );

		virtual long Release(){ return 0; }
	};
	class CPojo_StyleItem : public IStyleItemInfo
	{
	public:
		CPojo_StyleItem();
		~CPojo_StyleItem();

		DECLARE_STRING_SETGET( ID );

		STYLE_SELECTOR_TYPE GetSelectorType() { return m_eSelectorType; }
		void    SetSelectorType(const STYLE_SELECTOR_TYPE& type) { m_eSelectorType = type; }
		String  GetInherits()    { return m_strInherits; }
		String& GetInheritsRef() { return m_strInherits; }

		void   SetInherits( const String& str );
		int    GetInheritCount();
		bool   GetInheritItem(int i, String& str);
		bool   RemoveInheritItem( const String& str );

		int    GetAttributesCount();
		bool   GetAttribute( int nIndex, TCHAR* szkey, TCHAR* szValue );
		bool   GetAttribute( int nIndex, String& key, String& value );
		void   SetAttribute( const String& key, const String& value );
		bool   GetAttribute( const String& key, String& retValue );
		bool   InsertAttribute( const String& key, const String& value );
		bool   ModifyAttribute( const String& key, const String& value );
		bool   RemoveAttribute( const String& key );

		bool   InheritAttribute( const String& key, const String& value );
		bool   InheritMyAttributesToAnother( CPojo_StyleItem* pChild );
		bool   ApplyMyAttributesToAnExistMap( map<String,String>& mapAttrib );

	private:
		STYLE_SELECTOR_TYPE  m_eSelectorType;
		String               m_strID;          // 该选择器的ID
		String               m_strInherits;    // 继承列表
		map<String,String>   m_mapAttributes;  // 该选择器的属性集

		// 非持久数据
		vector<String>       m_vInheritList;   // 继续列表，运行中解释m_strInherits
	};

	class UIAPI CPojo_Style
	{
	public:
		~CPojo_Style();

		void Clear();
		bool InsertStyle( STYLE_SELECTOR_TYPE type, const String& strID, const String& strInherit );
		bool InsertStyle( CPojo_StyleItem* pNewItem );
		bool ModifyStyle( STYLE_SELECTOR_TYPE type,const String& strID, const String& strInherit );
		bool RemoveStyle( STYLE_SELECTOR_TYPE type, const String& strID );
		
		int  GetStyleCount();
		CPojo_StyleItem* GetStyleItem( int nIndex );
		CPojo_StyleItem* GetStyleItem( STYLE_SELECTOR_TYPE type, const String& strID );

		bool InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
		bool ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
		bool RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey );

		bool LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle );

	private:
		vector<CPojo_StyleItem*>   m_vStyles;
	};

	class UIAPI CPojo_Layout
	{
	public:
	};


}