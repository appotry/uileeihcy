#pragma once
namespace UI
{

	//////////////////////////////////////////////////////////////////////////
	//                                                                      //
	//                             字体                                     //
	//                                                                      //
	//////////////////////////////////////////////////////////////////////////


// 	class FontItem
// 	{
// 	public:
// 		FontItem()
// 		{
// 			nHeight      = 180;
// 			nOrientation = 0;
// 			bBold        = false;
// 			bUnderline   = false;
// 			bStrikeout   = false;
// 			strFacename  = _T("宋体");
// 		}
// 
// 		String   strID;      //  font名称
// 
// 		int      nHeight;      //  大小
// 		int      nOrientation; //  旋转角度
// 		bool     bBold;        //  是否为粗体//int  nWeight;0-1000, 0为默认	400为NORMAL 700为BOLD
// 		bool     bItalic;      //  是否为斜体
// 		bool     bUnderline;   //  是否有下划线
// 		bool     bStrikeout;   //  是否有删除线
// 		String   strFacename;  //  字体名称
// 
// 	};

	// 在CUIApplication中被声明
	class FontManager
	{
	public:
		FontManager(void);
		~FontManager(void);

	public:
		bool       Create(const String& strXmlPath );
		bool       Load( const String& strXmlPath );
		bool       Reload();
		bool       Save();
		void       Clear();
		HRFONT     GetFont( const String& strID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		HRFONT     GetFontEx( HRFONT hRFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		int        GetFontCount();
		bool       GetFontItemInfo( int nIndex, IFontItemInfo** ppFontItemInfo );
		bool       InsertFontItem( const String& strID, LOGFONT* pLogFont );
		bool       ModifyFontItem( const String& strID, LOGFONT* pLogFont );
		bool       RemoveFontItem( const String& strID );
		
		IFontParse*   GetFontParse() { return m_pFontParse; }
		void          SetFontParse( IFontParse* p );

	private:
		/*map<String, HFONT>   m_map;*/

		// 数据持久层
		IFontParse*    m_pFontParse;

		// 对象属性
		CPojo_Font     m_pojoFont;
	};

}