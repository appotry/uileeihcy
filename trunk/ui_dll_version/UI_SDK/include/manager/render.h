#pragma once
#include "uxtheme.h"
#include "Vsstyle.h"
#pragma comment(lib, "uxtheme.lib")

namespace UI
{
#define WINDOW_BKGND_RENDER_STATE_ACTIVE   0   // ���ڼ�����ʽ
#define WINDOW_BKGND_RENDER_STATE_INACTIVE 1   // ���ڷǼ�����ʽ

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


	// ���ڱ������ƻ��࣬����ʵ�ָ�����ʽ���ڱ����Ļ��ƣ��磺ƽ�̣����У����죬9����
	class RenderBase
	{
	public:
		RenderBase();
		virtual ~RenderBase(){};
	
	public:
		//
		//	����Render���Ա����ںܶ�ط����米����ͼ��ȵȡ�Ϊ���������õ����ĸ����ֵ�Render���ԣ�
		//	����Render����ǰ�����һ��ǰ׺����bkgnd.render.type  icon.render.type
		//	��SetAttribute�У���Ҫͬʱ����ǰ׺+�����������в���
		//
		//	����IRender����Ҫ�ṩRetAttribute��������ΪObject��Reset��ʱ��һ����delete irender
		//	�������Ҳû����� bool bReload ����
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
		Object*        m_pObject;      // �󶨵Ķ���Ҫ����˭�ı���
		RENDER_TYPE    m_nRenderType;  // �Լ�������
	};

	// ������
	class RenderFactory
	{
	public:
		static RenderBase* GetRender( const String& strType, Object* pObj );
		static RenderBase* GetRender( RENDER_TYPE eType, Object* pObj );
	};


	//
	// ������ɫ+�߿���ɫ�����߶��ǿ�ѡ�ģ�
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

	// ���򽥱���ɫ����
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
	//	ֱ��ʹ����ɫֵ��Ϊ����
	//
	//	Q: ���ʵ�ֲ���Ҫ������״̬�����繤������ť������״̬����û�б�����?
	//	A: ��״̬λ��ʵ�֣�������ĳ��״̬����ɫֵ���ͱ�ʾ��״̬��Ҫ���Ʊ���������Ͳ�����
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
	//	ͼƬ������ť
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

		IMAGELIST_LAYOUT_TYPE  m_eImageLayout;  // ͼƬ�е�ͼƬ�����з�ʽ
		int        m_nCount;              // ͼƬ����
		int        m_nItemWidth;          // ͼƬ��Ŀ��
		int        m_nItemHeight;         // ͼƬ��ĸ߶�
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
	//	���ⱳ����ť
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
	//	������ʽ�ĵ�ѡͼ��
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
	//	������ʽ�ĸ�ѡͼ��
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
	//	���ⱳ����ť
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
		Object*          m_pObject;          // �󶨵Ķ���Ҫ����˭������
		TEXTRENDER_TYPE  m_nTextRenderType;  // �Լ�������
		int              m_nDrawTextFlag;
	};

	// ������
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