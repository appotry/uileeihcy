#pragma once

namespace UI
{

#if 0
	//
	//	描述一个字体对象引用，用于多个地方同时引用一个字体
	//
	class UIFont 
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



//
//	一条字体信息
//
class GDIRenderFont;
class GdiplusRenderFont;

class FontResItem
{
public:
	FontResItem();
	~FontResItem();

    IFontResItem*  GetIFontResItem();

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
	IRenderFont* GetFont( GRAPHICS_RENDER_LIBRARY_TYPE eRenderType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI );

	WPARAM GetWParam() { return m_wParam; }
	LPARAM GetLParam() { return m_lParam; }
	void   SetWParam( WPARAM w ) { m_wParam = w; }
	void   SetLParam( LPARAM l ) { m_lParam = l; }

	bool   IsMyRenderFont(IRenderFont* pRenderFont);

private:
    IFontResItem*  m_pIFontResItem;

	String   m_strID;
	LOGFONT  m_lf;

	WPARAM   m_wParam;  
	LPARAM   m_lParam;  

	GDIRenderFont*       m_pGdiFont;
	GdiplusRenderFont*   m_pGdiplusFont;

#ifdef UI_D2D_RENDER
	Direct2DRenderFont*  m_pD2DFont;
#endif
};

//
//	font 列表
//
class FontRes
{
public:
    FontRes();
	~FontRes();

    IFontRes*  GetIFontRes();
	 
	long     GetFontCount();
	HRESULT  GetFontResItem(long lIndex, IFontResItem** ppResItem);
	HRESULT  GetFont(BSTR bstrFontID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut);
	HRESULT  GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont);
	HRESULT  GetFontEx(IRenderFont* pFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont);

public:
	FontResItem* GetFontItem( int nIndex );
	FontResItem* GetFontItem( const String& strID );
	bool InsertFont( const String& strID, LOGFONT* pLogFont );
	FontResItem* InsertFont( const String& strID, LOGFONT* pLogFont, WPARAM wParam, LPARAM lParam );
	bool ModifyFont( const String& strID, LOGFONT* pLogFont );
	bool RemoveFont( const String& strID );
	void Clear();

	bool GetRenderFontID(IRenderFont* pFont, String& strID);

private:
    IFontRes*  m_pIFontRes;
    vector<FontResItem*>  m_vFonts;
};
}