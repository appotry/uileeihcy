#pragma once

//
//	����һ��String���ͱ�����get/set����
//
//	����:
//		xxx
//			�����Ա��Ϊ m_strName����xxxȡֵΪ Name
//
//	ע�⣺
//		������dll/exeģ���в�Ҫ����Get����,��Ӧ����GetRef����,��������ʱ�����heap����
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

	//
	//	���ڱ�¶���ⲿ��Ƥ����Ϣ
	//
	class IProjectSkinItemInfo
	{
	public:
		virtual ~IProjectSkinItemInfo() = 0 {}
		virtual const String& GetSkinNameRef() = 0;
	};

	//
	//	�����е�Ƥ����������
	//
	class CPojo_ProjectSkinItem : public IProjectSkinItemInfo
	{
	private:
		String   m_strSkinName;          // Ƥ��������
		String   m_strSkinXmlPath;       // Ƥ�������ļ�����·��

	public:
		DECLARE_STRING_SETGET( SkinName );
		DECLARE_STRING_SETGET( SkinXmlPath );
	};

	//
	//	һ�����̶�������
	//
	class CPojo_Project
	{
	public:
		CPojo_Project(){}
		~CPojo_Project();
	private:
		String   m_strProjectName;       // ��������
		String   m_strActiveSkinName;    // ��ǰʹ�õ�Ƥ������

		vector<CPojo_ProjectSkinItem*>   m_vSkins;   // ����Ƥ���б�
		
	public:
		DECLARE_STRING_SETGET( ProjectName );
		DECLARE_STRING_SETGET( ActiveSkinName );
		
		CPojo_ProjectSkinItem*  GetSkinItem( int nIndex );
		CPojo_ProjectSkinItem*  GetSkinItem( const String& strSkinName );
		CPojo_ProjectSkinItem*  GetActiveSkinItem( );
		int  GetSkinItemCount();
		int  GetActiveSkinItemIndex();
		bool AddSkinItem( CPojo_ProjectSkinItem* pNewItem );
		bool GetActiveSkinPath( TCHAR* szPath );   // ��ȡ��ǰ��ԾƤ������·��
		bool GetSkinPath( const String& strSkinName, TCHAR* szPath );
	};


	//
	//	Ƥ����Ϣ��������Ԫ��xmlλ�� 
	//
	class CPojo_SkinInfo
	{
	private:
		String   m_strImageXmlPath;       // ȫ·�������浽�ļ��еĲ������·�� 
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

#if 0  // UIImage�����ˣ�ʹ��HRBITMAP����
	//
	// ��װһ��Image*������Image���������ڹ���
	// ��һ��ͼƬû�б�ʹ��ʱ�������أ�����ʹ��ʱ���ͷ����ڴ�
	//
	//	ʹ�� ::UI_GetImage ��ȡ��ʹ��Release�ͷ�
	//
	class UIAPI UIImage : public Image  
	{
	private:    // ֻ��ͨ��Create������
		UIImage(UIImage** pOutRef);
	public:
		~UIImage();

	public:
		static void  CreateInstance( const String& str, UIImage** pOutRef );
		long AddRef();
		long Release();

		void Modify( const String& str );
	private:
		// 		String   m_strFilePath;
		// 		struct
		// 		{
		// 			HINSTANCE hInst;
		// 			UINT      nID;
		// 			TCHAR     szResType[8];
		// 		} m_sDllRes;
		// 
		// 		enum E_RESTYPE{ IMAGE_RES_TYPE_FILE, IMAGE_RES_TYPE_DLL };
		// 		E_RESTYPE  m_nResType;
		long       m_dwRef;

		UIImage**  m_pOutRef;   // �ⲿ���ã����ڴ���������release�Զ��ͷ�
		// Ϊ�˽����Ҫ��CPojo_ImageItem�б���һ��UIImage*ָ�룬Ҫ�ⲿGetʱ��ֵ������������UIImage*�ֲ��ܸ����������á�
		// �������ⲿ��UIImage* Release������CPojo_ImageItem::UIImage*ȴ�޷��ÿգ�������������һ��m_pOutRef��ָ�����
		// UIImage*����UIImage�����������н����ÿ�
	};
#endif

	//
	//	Ϊ���ⲿ�ܹ�����image�б���ṩ���ⲿ�Ľӿ�
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
	//	image�е�һ����Ϣ����
	//
	class CPojo_ImageItem : public IImageItemInfo
	{
	public:
		CPojo_ImageItem();
		~CPojo_ImageItem();

	private:
		String    m_strID;      // image id
		String    m_strPath;    // image path


		bool                  m_bUseSkinHue;         // ��ͼƬ�Ƿ����Ƥ��ɫ���ı� 
		IMAGE_ITEM_TYPE       m_eImageItemType;
		int   m_nIconWidth;
		int   m_nIconHeight;
		int   m_nImageListCount;
		IMAGELIST_LAYOUT_TYPE m_eImageListLayoutType;


		ImageData*            m_pOriginImageData;    // ��ͼƬ��ԭʼ���ݣ��ı�ͼƬɫ��ʱʹ�ã�

		GDIRenderBitmap*      m_pGdiBitmap;          // �ⲿ����
		GdiplusRenderBitmap*  m_pGdiplusBitmap;      // �ⲿ����

	public:
		DECLARE_STRING_SETGET( ID );
		DECLARE_STRING_SETGET( Path );
		DECLARE_bool_SETGET(UseSkinHue);

		bool     ModifyImage( const String& strPath );
		bool     ModifyHLS( short h, short l, short s, int nFlag );
		HRBITMAP GetImage( GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
	};

	//
	//	image�б�
	//
	class CPojo_Image
	{
	public:
		CPojo_Image();
		~CPojo_Image();
	private:
		vector<CPojo_ImageItem*>   m_vImages;

	public:
		int  GetImageCount(); 
		CPojo_ImageItem* GetImageItem( int nIndex );
		CPojo_ImageItem* GetImageItem( const String& strID );

		bool InsertImage( const String& strID, const String& strPath, CPojo_ImageItem** pRet=NULL );
		bool ModifyImage( const String& strID, const String& strPath );
		bool RemoveImage( const String& strID );
		bool Clear();
		bool ChangeSkinHLS(short h, short l, short s, int nFlag);

		HRBITMAP GetImage( const String& strID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
	};

	//
	//	�����ʽ
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
		String     m_strCurFilePath;     // .cur�ļ�·��
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

	
	//
	//	ͼ����ʽ
	//
// 	class UIAPI UIIcon
// 	{
// 	private:
// 		UIIcon( UIIcon** ppOutRef );
// 	public:
// 		~UIIcon();
// 
// 		static void CreateInstance( const String strCurFilePath, UIIcon** pOutRef );
// 		static void CreateInstance( LPTSTR nCursorID, UIIcon** ppOutRef );
// 		HCURSOR  GetCursor();
// 		bool     Load( LPTSTR nCursorID );
// 		void     Attach(HCURSOR hCursor);
// 		HCURSOR  Detach();
// 		long     AddRef();
// 		long     Release();
// 
// 	private:
// 		long      m_dwRef;
// 		HICON     m_hIcon;
// 
// 		UICursor** m_pOutRef;
// 	};
// 	class CPojo_IconItem
// 	{
// 	public:
// 		CPojo_IconItem();
// 
// 		DECLARE_STRING_SETGET( ID );
// 		DECLARE_STRING_SETGET( IconFilePath );
// 
// 		bool  GetIcon( UIIcon** ppIcon );
// 		bool  ModifyCursor( const String& strIconFilePath );
// 
// 	public:
// 		String     m_strID;
// 		String     m_strIconFilePath;     // .cur�ļ�·��
// 		UIIcon*    m_pIcon;             
// 	};
// 	class CPojo_Icon
// 	{
// 	public:
// 		CPojo_Icon();
// 		~CPojo_Icon();
// 
// 	public:
// 		CPojo_Icon* GetCursorItem( int nIndex );
// 		CPojo_Icon* GetCursorItem( const String& strID );
// 		int   GetIconCount();
// 		bool  GetIcon( const String& strID, UICursor** pCursorRet );
// 		bool  InsertIcon( const String& strID, const String& strCurFilePath );
// 		bool  ModifyIcon( const String& strID, const String& strCursor );
// 		bool  RemoveIcon( const String& strID );
// 		void  Clear();
// 
// 	private:
// 		vector<CPojo_IconItem*>   m_vIcons;
// 	};

	//
	//	�����ͨ��ָ������ȡ��ɫ��Դ������ÿ����ɫ�仯ʱ���ɾ�����ͨ����ȡָ��ָ���������ˢ�½���
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
	
	// �ⲿ��ȡ��Ϣ�Ľӿ�
	class IColorItemInfo
	{
	public:
		virtual ~IColorItemInfo() = 0 {};
		virtual const String& GetIDRef() = 0;
		virtual const String& GetColorStringRef() = 0;

		virtual long Release(){ return 0; }
	};
	//
	//	color�е�һ����Ϣ����
	//
	class  CPojo_ColorItem : public IColorItemInfo
	{
	public:
		CPojo_ColorItem();
		~CPojo_ColorItem();

	private:
		String    m_strID;
		String    m_strColor;   // ��ɫ�ַ���
		UIColor*  m_pColor;     // ��ɫֵ����m_strColorת������

	public:
		DECLARE_STRING_SETGET( ID );
		const String&   GetColorStringRef();
		bool  GetColor( UIColor** pColor );
		void  SetColor( const String& strColor ); 
	};

	//
	//	color�б�
	//
	class CPojo_Color
	{
	public:
		~CPojo_Color();
	private:
		vector<CPojo_ColorItem*>   m_vColors;

	public:
		CPojo_ColorItem* GetColorItem( int nIndex );
		CPojo_ColorItem* GetColorItem( const String& strID );
		int   GetColorCount() ;
		bool  GetColor( const String& strID, UIColor** pColorRet );
		bool  InsertColor( const String& strID, const String& strColor );
		bool  ModifyColor( const String& strID, const String& strColor );
		bool  RemoveColor( const String& strID );
		void  Clear();
	};

#if 0
	//
	//	����һ������������ã����ڶ���ط�ͬʱ����һ������
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
		LOGFONT*  m_pLogFont;   // ����CPojo_FontItem::m_lf   NULL ��ʾ��HFONT��Attach�õ��ģ�����ҪDeleteObject
		UIFont**  m_pOutRef;    // ����CPojo_FontItem::m_pFont
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
	//	һ��������Ϣ
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

		GDIRenderFont*     m_pGdiFont;
		GdiplusRenderFont* m_pGdiplusFont;
	};

	//
	//	font �б�
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
		String               m_strID;          // ��ѡ������ID
		String               m_strInherits;    // �̳��б�
		map<String,String>   m_mapAttributes;  // ��ѡ���������Լ�

		// �ǳ־�����
		vector<String>       m_vInheritList;   // �����б������н���m_strInherits
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