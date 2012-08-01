#pragma once
#include "uxtheme.h"
#include "Vsstyle.h"
#pragma comment(lib, "uxtheme.lib")

namespace UI
{
#define WINDOW_BKGND_RENDER_STATE_ACTIVE   0   // 窗口激活样式
#define WINDOW_BKGND_RENDER_STATE_INACTIVE 1   // 窗口非激活样式

#define BUTTON_BKGND_RENDER_STATE_NORMAL   0
#define BUTTON_BKGND_RENDER_STATE_HOVER    1
#define BUTTON_BKGND_RENDER_STATE_PRESS    2
#define BUTTON_BKGND_RENDER_STATE_DISABLE  3
#define BUTTON_BKGND_RENDER_STATE_DEFAULT  4

#define EDIT_BKGND_RENDER_STATE_NORMAL   0
#define EDIT_BKGND_RENDER_STATE_HOVER    1
#define EDIT_BKGND_RENDER_STATE_PRESS    2
#define EDIT_BKGND_RENDER_STATE_DISABLE  3

#define COMBOBOX_BKGND_RENDER_STATE_NORMAL   0
#define COMBOBOX_BKGND_RENDER_STATE_HOVER    1
#define COMBOBOX_BKGND_RENDER_STATE_PRESS    2
#define COMBOBOX_BKGND_RENDER_STATE_DISABLE  3

#define LISTBOX_BKGND_RENDER_STATE_NORMAL   0
#define LISTBOX_BKGND_RENDER_STATE_HOVER    1
#define LISTBOX_BKGND_RENDER_STATE_PRESS    2
#define LISTBOX_BKGND_RENDER_STATE_DISABLE  3

#define GROUPBOX_BKGND_RENDER_STATE_NORMAL  0
#define GROUPBOX_BKGND_RENDER_STATE_DISABLE 1

#define BUTTON_ICON_RENDER_STATE_NORMAL    0
#define BUTTON_ICON_RENDER_STATE_HOVER     1
#define BUTTON_ICON_RENDER_STATE_PRESS     2
#define BUTTON_ICON_RENDER_STATE_DISABLE   3
#define BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL    4
#define BUTTON_ICON_RENDER_STATE_SELECTED_HOVER     5
#define BUTTON_ICON_RENDER_STATE_SELECTED_PRESS     6
#define BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE   7

#define LISTCTRLITEM_FOREGND_RENDER_STATE_NORMAL   0
#define LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER    1
#define LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS    2
#define LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE  3
#define LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED 4

	class IconTextRenderBase
	{
	public:
		virtual void  Draw(HRDC hRDC, RECT rcClient) = 0;
		virtual SIZE  GetDesireSize() = 0;
	};


	// 窗口背景绘制基类，用于实现各种样式窗口背景的绘制，如：平铺，居中，拉伸，9宫等
	class RenderBase
	{
	public:
		RenderBase();
		virtual ~RenderBase(){};
	
	public:
		//
		//	由于Render可以被用在很多地方，如背景、图标等等。为了区分设置的是哪个部分的Render属性，
		//	会在Render属性前面添加一个前缀，如bkgnd.render.type  icon.render.type
		//	在SetAttribute中，需要同时根据前缀+属性名来进行操作
		//
		//	另外IRender不需要提供RetAttribute操作，因为Object在Reset的时候，一般是delete irender
		//	因此这里也没有添加 bool bReload 参数
		//
		virtual bool SetAttribute( const String& strPrifix, map<String,String>& mapAttrib ){ return true; };
		virtual void DrawState(HRDC hRDC, const CRect* prc, int nState) {};
		virtual SIZE GetDesiredSize() { SIZE s = {0,0}; return s; }
		virtual void Init() {};

	public:
		void         SetObject( Object* pObject ) { this->m_pObject = pObject; }
		void         SetRenderType( const RENDER_TYPE& nType ){ m_nRenderType = nType ; }
		RENDER_TYPE  GetRenderType() { return m_nRenderType; }

	protected:
		Object*        m_pObject;      // 绑定的对象，要绘制谁的背景
		RENDER_TYPE    m_nRenderType;  // 自己的类型
	};

	// 工厂类
	class RenderFactory
	{
	public:
		static RenderBase* GetRender( const String& strType, Object* pObj );
		static RenderBase* GetRender( RENDER_TYPE eType, Object* pObj );
	};


	//
	// 背景颜色+边框颜色（两者都是可选的）
	//
	class ColorRender : public RenderBase
	{
	public:
		ColorRender();
		~ColorRender();
		virtual bool SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void DrawState(HRDC hRDC, const CRect* prc, int nState);

	public:
		void    SetBkColor( COLORREF col );
		void    SetBorderColor( COLORREF col );
		void    SetBorder(int n){ m_nBorder = n; }
		int     GetBorder(){ return m_nBorder; }

	public:
		UIColor*     m_pBkColor;
		UIColor*     m_pBorderColor;
		int	         m_nBorder;
	};

	// 横向渐变颜色背景
	class GradientRender : public RenderBase
	{
	public:
		GradientRender();
		~GradientRender();

		virtual bool SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void DrawState(HRDC hRDC, const CRect* prc, int nState);

	public:
		UIColor*     m_pColorFrom;
		UIColor*     m_pColorTo;
		UIColor*     m_pBorderColor;
	};

	class SimpleImageRender : public RenderBase
	{
	public:
		SimpleImageRender();
		~SimpleImageRender();

		virtual bool SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE GetDesiredSize() ;
	public:
		HRBITMAP     m_hBitmap;
		UIColor*     m_pColorBk;
	};

	class StretchImageRender : public RenderBase
	{
	public:
		StretchImageRender();
		~StretchImageRender();

		virtual bool SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE GetDesiredSize() ;

	public:
		HRBITMAP      m_hBitmap;
		Image9Region  m_region;
		UIColor*      m_pColorBk;
	};

	class TileImageRender : public RenderBase
	{
	public:
		TileImageRender();
		~TileImageRender();

		virtual bool   SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void   DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE   GetDesiredSize() ;

	public:
		HRBITMAP   m_hBitmap;
	};

	//
	//	直接使用颜色值作为背景
	//
	//	Q: 如何实现不需要背景的状态，例如工具栏按钮的正常状态，是没有背景的?
	//	A: 用状态位来实现，设置了某个状态的颜色值，就表示该状态需要绘制背景，否则就不绘制
	//
	class ColorListRender : public RenderBase
	{
	public:
		ColorListRender(  );
		~ColorListRender( );
		void  Clear();

		virtual bool   SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void   DrawState(HRDC hRDC, const CRect* prc, int nState);
		
		void    SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder);

	private:
		vector<UIColor*> m_vBkColor;
		vector<UIColor*> m_vBorderColor;
		int              m_nCount; 

	};

	//
	//	图片背景按钮
	//
	class ImageListRender : public RenderBase
	{
	public:
		ImageListRender( );
		~ImageListRender( );

		virtual bool   SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void   DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE   GetDesiredSize() ;

	protected:
		HRBITMAP   m_hBitmap;

		IMAGELIST_LAYOUT_TYPE  m_eImageLayout;  // 图片中的图片项排列方式
		int        m_nCount;              // 图片项数
		int        m_nItemWidth;          // 图片项的宽度
		int        m_nItemHeight;         // 图片项的高度
	};

	class ImageListStretchRender : public ImageListRender
	{
	public:
		virtual bool   SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void   DrawState(HRDC hRDC, const CRect* prc, int nState);
	private:
		Image9Region    m_9Region;
	};

//	class ImageListIndexRender : public 

	//
	//	主题背景按钮
	//
	class ThemeRenderBase : public RenderBase
	{
	public:
		ThemeRenderBase();
		~ThemeRenderBase();

//  	UI_BEGIN_MSG_MAP
//  		UIMSG_WM_THEMECHANGED( OnThemeChanged )
//  		UICHAIN_MSG_MAP(RenderBase)
//  	UI_END_MSG_MAP

		void      OnThemeChanged();
		void      CreateTheme();

		virtual   void Init();
		virtual   const TCHAR* GetThemeName() = 0;
	protected:
		HTHEME    m_hTheme;
	};

	class ButtonBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return _T("Button"); }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	
	};


	//
	//	主题样式的单选图标
	//
	class RadioButtonThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return _T("Button"); }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize( );

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );
		void DrawCheckDisable( HRDC hRDC, const CRect* prc );
		void DrawCheckNormal( HRDC hRDC, const CRect* prc );
		void DrawCheckHover( HRDC hRDC, const CRect* prc );
		void DrawCheckPress( HRDC hRDC, const CRect* prc );
	};
	//
	//	主题样式的复选图标
	//
	class CheckButtonThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return _T("Button"); }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize( );

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );
		void DrawCheckDisable( HRDC hRDC, const CRect* prc );
		void DrawCheckNormal( HRDC hRDC, const CRect* prc );
		void DrawCheckHover( HRDC hRDC, const CRect* prc );
		void DrawCheckPress( HRDC hRDC, const CRect* prc );

	};

	//
	//	主题背景按钮
	//
	class EditBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return _T("Edit"); }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	
	};

	class GroupBoxBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return _T("Button"); }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
// 		void DrawHover( HRDC hRDC, CRect* prc );
// 		void DrawPress( HRDC hRDC, CRect* prc );	
	};

	class ComboboxButtonBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return _T("COMBOBOX"); }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE GetDesiredSize();

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	

		void DrawGlyph( HDC hDC, const CRect* prc, bool bPressDown=false);
	};

	class ComboboxBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return _T("COMBOBOX"); }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	
	};

	class ListboxBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return _T("LISTBOX"); }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	
	};
	//////////////////////////////////////////////////////////////////////////

	
	class TextRenderBase
	{
	public:
		TextRenderBase();
		virtual ~TextRenderBase(){};

	public:
		virtual bool     SetAttribute( const String& strPrifix, map<String,String>& mapAttrib ){ return true; };
		virtual void     DrawState(HRDC hRDC, const CRect* prc, int nState, const String& strText, int nDrawTextFlag=-1) {};
		virtual HRFONT   GetHRFONT() = 0;
		virtual void     SetHRFont(HRFONT hRFont) = 0;
		virtual void     SetTextAlignment(int nDrawFlag);

	public:
		void             SetObject( Object* pObject ) { this->m_pObject = pObject; }
		void             SetTextRenderType( const TEXTRENDER_TYPE& nType ){ m_nTextRenderType = nType ; }
		TEXTRENDER_TYPE  GetTextRenderType() { return m_nTextRenderType; }

		SIZE             GetDesiredSize(const String& strText, int nLimitWidth=-1);

	protected:
		Object*          m_pObject;          // 绑定的对象，要绘制谁的文字
		TEXTRENDER_TYPE  m_nTextRenderType;  // 自己的类型
		int              m_nDrawTextFlag;
	};

	// 工厂类
	class TextRenderFactory
	{
	public:
		static TextRenderBase* GetTextRender( const String& strType, Object* pObj );
		static TextRenderBase* GetTextRender( TEXTRENDER_TYPE eType, Object* pObj );
	};

	class TextRender : public TextRenderBase
	{
	public:
		TextRender();
		~TextRender();

		virtual bool     SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void     DrawState(HRDC hRDC, const CRect* prc, int nState, const String& strText, int nDrawTextFlag=-1);
		virtual HRFONT   GetHRFONT(){ return m_hFont; }
		virtual void     SetHRFont(HRFONT hRFont);

	private:
		UIColor*     m_pColorText;
		HRFONT       m_hFont;
	};

#if 0
	class ColorListTextRender : public TextRenderBase
	{
	private:
		vector<UIColor*>  m_vTextColor;
		HRFONT            m_hTextFont;
	};

	class FontListTextRender : public TextRenderBase
	{
	private:
		UIColor*          m_pTextColor;
		vector<HRFONT>    m_vTextFont;
	};
#endif
	class FontColorListTextRender : public TextRenderBase
	{
	public:
		FontColorListTextRender();
		~FontColorListTextRender();

		virtual bool     SetAttribute( const String& strPrifix, map<String,String>& mapAttrib );
		virtual void     DrawState(HRDC hRDC, const CRect* prc, int nState, const String& strText, int nDrawTextFlag=-1);
		virtual HRFONT   GetHRFONT();
		virtual void     SetHRFont(HRFONT hRFont);

		void SetCount( int nCount );
		void SetColor( int nIndex, COLORREF col );
		void SetFont( int nIndex, HRFONT hRFONG );

	protected:
		void Clear();

	private:
		vector<UIColor*>  m_vTextColor;
		vector<HRFONT>    m_vTextFont;

		int   m_nCount;
	};

}