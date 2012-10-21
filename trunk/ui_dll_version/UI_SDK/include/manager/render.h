#pragma once
#include "uxtheme.h"
#include "Vsstyle.h"
#pragma comment(lib, "uxtheme.lib")

namespace UI
{
// ע��libo 20120927
//     ÿ�ι��render����һ����ֵ�������޷�ֻ���������ֵ��ͼƬ����������ƥ�䣬�޷���������ж�
//     ��˾�����չDrawState��������λΪͼƬ����ֵ����λΪ״ֵ̬

#define RENDER_STATE_HOVER    0x00010000
#define RENDER_STATE_PRESS    0x00020000
#define RENDER_STATE_DISABLE  0x00040000
#define RENDER_STATE_SELECTED 0x00080000
#define RENDER_STATE_DEFAULT  0x00100000
#define RENDER_STATE_READONLY 0x00200000

const UINT  WINDOW_BKGND_RENDER_STATE_ACTIVE  = 0;   // ���ڼ�����ʽ
const UINT  WINDOW_BKGND_RENDER_STATE_INACTIVE = 1;   // ���ڷǼ�����ʽ

const UINT  BUTTON_BKGND_RENDER_STATE_NORMAL  = 0;
const UINT  BUTTON_BKGND_RENDER_STATE_HOVER   = RENDER_STATE_HOVER|1;
const UINT  BUTTON_BKGND_RENDER_STATE_PRESS   = RENDER_STATE_PRESS|2;
const UINT  BUTTON_BKGND_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE|3;
const UINT  BUTTON_BKGND_RENDER_STATE_DEFAULT = RENDER_STATE_DEFAULT|4;
const UINT  BUTTON_BKGND_RENDER_STATE_SELECTED_NORMAL   = RENDER_STATE_SELECTED|4;
const UINT  BUTTON_BKGND_RENDER_STATE_SELECTED_HOVER    = RENDER_STATE_HOVER|RENDER_STATE_SELECTED|5;
const UINT  BUTTON_BKGND_RENDER_STATE_SELECTED_PRESS    = RENDER_STATE_PRESS|RENDER_STATE_SELECTED|6;
const UINT  BUTTON_BKGND_RENDER_STATE_SELECTED_DISABLE  = RENDER_STATE_DISABLE|RENDER_STATE_SELECTED|7;

const UINT  EDIT_BKGND_RENDER_STATE_NORMAL  = 0;
const UINT  EDIT_BKGND_RENDER_STATE_HOVER   = RENDER_STATE_HOVER|1;
const UINT  EDIT_BKGND_RENDER_STATE_PRESS   = RENDER_STATE_PRESS|2;
const UINT  EDIT_BKGND_RENDER_STATE_DISABLE = 3;

const UINT  COMBOBOX_BKGND_RENDER_STATE_NORMAL     = 0;
const UINT  COMBOBOX_BKGND_RENDER_STATE_HOVER      = RENDER_STATE_HOVER|1;
const UINT  COMBOBOX_BKGND_RENDER_STATE_PRESS      = RENDER_STATE_PRESS|2;
const UINT  COMBOBOX_BKGND_RENDER_STATE_DISABLE    = RENDER_STATE_DISABLE|3;
const UINT  COMBOBOX_BKGND_RENDER_STATE_READONLY_NORMAL  = RENDER_STATE_READONLY | 0;
const UINT  COMBOBOX_BKGND_RENDER_STATE_READONLY_HOVER   = RENDER_STATE_READONLY |RENDER_STATE_HOVER| 1;
const UINT  COMBOBOX_BKGND_RENDER_STATE_READONLY_PRESS   = RENDER_STATE_READONLY |RENDER_STATE_PRESS| 2;
const UINT  COMBOBOX_BKGND_RENDER_STATE_READONLY_DISABLE = RENDER_STATE_READONLY |RENDER_STATE_DISABLE| 3;

const UINT  LISTBOX_BKGND_RENDER_STATE_NORMAL      = 0;
const UINT  LISTBOX_BKGND_RENDER_STATE_HOVER       = RENDER_STATE_HOVER|1;
const UINT  LISTBOX_BKGND_RENDER_STATE_PRESS       = RENDER_STATE_PRESS|2;
const UINT  LISTBOX_BKGND_RENDER_STATE_DISABLE     = RENDER_STATE_DISABLE|3;

const UINT  MENU_BKGND_RENDER_STATE_NORMAL         = 0;

const UINT  MENU_STRING_ITEM_RENDER_STATE_NORMAL   = 0;
const UINT  MENU_STRING_ITEM_RENDER_STATE_HOVER    = RENDER_STATE_HOVER|1;
const UINT  MENU_STRING_ITEM_RENDER_STATE_PRESS    = RENDER_STATE_PRESS|2;
const UINT  MENU_STRING_ITEM_RENDER_STATE_DISABLE  = RENDER_STATE_DISABLE|3;

const UINT  MENU_ITEM_ICON_RENDER_STATE_NORMAL     = 0;
const UINT  MENU_ITEM_ICON_RENDER_STATE_HOVER      = RENDER_STATE_HOVER|1;
const UINT  MENU_ITEM_ICON_RENDER_STATE_PRESS      = RENDER_STATE_PRESS|2;
const UINT  MENU_ITEM_ICON_RENDER_STATE_DISABLE    = RENDER_STATE_DISABLE|3;

const UINT  MENU_POPUPTRIANGLE_RENDER_STATE_NORMAL = 0;
const UINT  MENU_POPUPTRIANGLE_RENDER_STATE_HOVER  = RENDER_STATE_HOVER|1;
const UINT  MENU_POPUPTRIANGLE_RENDER_STATE_PRESS  = RENDER_STATE_PRESS|2;
const UINT  MENU_POPUPTRIANGLE_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE|3;

const UINT  GROUPBOX_BKGND_RENDER_STATE_NORMAL     = 0;
const UINT  GROUPBOX_BKGND_RENDER_STATE_DISABLE    = RENDER_STATE_DISABLE|1;

const UINT  BUTTON_ICON_RENDER_STATE_NORMAL        = 0;
const UINT  BUTTON_ICON_RENDER_STATE_HOVER         = RENDER_STATE_HOVER|1;
const UINT  BUTTON_ICON_RENDER_STATE_PRESS         = RENDER_STATE_PRESS|2;
const UINT  BUTTON_ICON_RENDER_STATE_DISABLE       = RENDER_STATE_DISABLE|3;
const UINT  BUTTON_ICON_RENDER_STATE_SELECTED_NORMAL  = RENDER_STATE_SELECTED|4;
const UINT  BUTTON_ICON_RENDER_STATE_SELECTED_HOVER   = RENDER_STATE_HOVER|RENDER_STATE_SELECTED|5;
const UINT  BUTTON_ICON_RENDER_STATE_SELECTED_PRESS   = RENDER_STATE_PRESS|RENDER_STATE_SELECTED|6;
const UINT  BUTTON_ICON_RENDER_STATE_SELECTED_DISABLE = RENDER_STATE_DISABLE|RENDER_STATE_SELECTED|7;

const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_NORMAL  = 0;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER   = RENDER_STATE_HOVER|1;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS   = RENDER_STATE_PRESS|2;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE|3;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL = RENDER_STATE_SELECTED|4;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER  = RENDER_STATE_HOVER|RENDER_STATE_SELECTED|5;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS  = RENDER_STATE_PRESS|RENDER_STATE_SELECTED|6;
const UINT  LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE = RENDER_STATE_DISABLE|RENDER_STATE_SELECTED|7;

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
		virtual bool SetAttribute( const String& strPrefix, map<String,String>& mapAttrib ){ return true; };
		virtual void DrawState(HRDC hRDC, const CRect* prc, int nState) {};
		virtual SIZE GetDesiredSize() { SIZE s = {0,0}; return s; }
		virtual void Init() {};
		virtual void SetObject( Object* pObject ) { this->m_pObject = pObject; }

	public:
		
		void         SetRenderType( const RENDER_TYPE& nType ){ m_nRenderType = nType ; }
		RENDER_TYPE  GetRenderType() { return m_nRenderType; }
		bool         IsThemeRender() {return m_nRenderType > RENDER_TYPE_THEME_FIRST && m_nRenderType < RENDER_TYPE_THEME_LAST; }

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

	class NullRenderBase : public RenderBase
	{
	public:

	};

	//
	// ������ɫ+�߿���ɫ�����߶��ǿ�ѡ�ģ�
	//
	class ColorRender : public RenderBase
	{
	public:
		ColorRender();
		~ColorRender();
		virtual bool SetAttribute( const String& strPrefix, map<String,String>& mapAttrib );
		virtual void DrawState(HRDC hRDC, const CRect* prc, int nState);

	public:
		void    SetBkColor( COLORREF col );
		void    SetBorderColor( COLORREF col );

	public:
		UIColor*     m_pBkColor;
		UIColor*     m_pBorderColor;
	};

	// ���򽥱���ɫ����
	class GradientRender : public RenderBase
	{
	public:
		GradientRender();
		~GradientRender();

		virtual bool SetAttribute( const String& strPrefix, map<String,String>& mapAttrib );
		virtual void DrawState(HRDC hRDC, const CRect* prc, int nState);

	public:
		UIColor*     m_pColorFrom;
		UIColor*     m_pColorTo;
		UIColor*     m_pBorderColor;
	};

	class ImageRender : public RenderBase
	{
	public:
		ImageRender();
		~ImageRender();

		virtual bool  SetAttribute( const String& strPrefix, map<String,String>& mapAttrib );
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize();
		virtual POINT GetBitmapSrcDrawPos() { POINT pt = {0,0}; return pt; }  // �������ṩһ�������Լ���ͼƬ�е�λ�õ��鷽��(��image list item)

		void    SetImageDrawType(int n) { m_nImageDrawType = n; }
		int     GetImageDrawType() { return m_nImageDrawType; }

	protected:
		IRenderBitmap*    m_pBitmap;
		UIColor*          m_pColorBk;
		Image9Region*     m_pRegion;   // �������ʱ����
		int               m_nImageDrawType;
	};

	// ����ͼƬ�б���ָ��������һ��
	class ImageListItemRender : public ImageRender 
	{
	public:
		ImageListItemRender();
		~ImageListItemRender();

		virtual bool  SetAttribute( const String& strPrefix, map<String,String>& mapAttrib );
		virtual SIZE  GetDesiredSize() ;
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual POINT GetBitmapSrcDrawPos() ;

	protected:
		IImageListRenderBitmap*  m_pImageList;
		int    m_nImagelistIndex;
	};

	//
	//	ֱ��ʹ����ɫֵ��Ϊ����
	//
	class ColorListRender : public RenderBase
	{
	public:
		ColorListRender(  );
		~ColorListRender( );
		void  Clear();

		virtual bool   SetAttribute( const String& strPrefix, map<String,String>& mapAttrib );
		virtual void   DrawState(HRDC hRDC, const CRect* prc, int nState);
		
		void    SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder);

	private:
		vector<UIColor*> m_vBkColor;
		vector<UIColor*> m_vBorderColor;
		int              m_nCount; 

	};

	class ListRenderBase : public RenderBase
	{
	public:
		virtual bool   SetAttribute(const String& strPrefix, ATTRMAP& mapAttrib);

	protected:
		map<int,int>   m_mapState2Index;    // Ҫ���Ƶ�״̬��ӦͼƬ���ĸ�item
	};
	//
	//	ͼƬ������ť
	//
	class ImageListRender : public ListRenderBase
	{
	public:
		ImageListRender( );
		~ImageListRender( );

		virtual bool   SetAttribute( const String& strPrefix, ATTRMAP& mapAttrib );
		virtual void   DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE   GetDesiredSize();

		int     GetItemWidth()  { return m_nItemWidth; }
		int     GetItemHeight() { return m_nItemHeight;}

		int     GetStateIndex(int nState);
		void    SetImageDrawType(int n) { m_nImageDrawType = n; }
	protected:
		HRBITMAP   m_hBitmap;

		
		IMAGELIST_LAYOUT_TYPE  m_eImageLayout;  // ͼƬ�е�ͼƬ�����з�ʽ		
		int        m_nImageDrawType; 
		int        m_nCount;              // ͼƬ����
		int        m_nItemWidth;          // ͼƬ��Ŀ��
		int        m_nItemHeight;         // ͼƬ��ĸ߶�

		Image9Region*    m_p9Region;

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
		virtual const TCHAR* GetThemeName() { return VSCLASS_BUTTON; }
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
		virtual const TCHAR* GetThemeName() { return VSCLASS_BUTTON; }
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
		virtual const TCHAR* GetThemeName() { return VSCLASS_EDIT; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	
	};

	class GroupBoxBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_BUTTON; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
// 		void DrawHover( HRDC hRDC, CRect* prc );
// 		void DrawPress( HRDC hRDC, CRect* prc );	
	};

	class GroupBoxBkNoThemeRender : public RenderBase
	{
	public:
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
	};

	class ComboboxButtonBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_COMBOBOX; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize();

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	

// 		void DrawReadOnlyDisable( HRDC hRDC, const CRect* prc );
// 		void DrawReadOnlyNormal( HRDC hRDC, const CRect* prc );
// 		void DrawReadOnlyHover( HRDC hRDC, const CRect* prc );
// 		void DrawReadOnlyPress( HRDC hRDC, const CRect* prc );

//		void DrawGlyph( HDC hDC, const CRect* prc, bool bPressDown=false);
	};

	class ComboboxBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_COMBOBOX; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	

		void DrawReadonlyDisable( HRDC hRDC, const CRect* prc );
		void DrawReadonlyNormal( HRDC hRDC, const CRect* prc );
		void DrawReadonlyHover( HRDC hRDC, const CRect* prc );
		void DrawReadonlyPress( HRDC hRDC, const CRect* prc );	
	};

	class ListboxBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_LISTBOX; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	
	};

	class MenuBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_MENU; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawNormal( HRDC hRDC, const CRect* prc );
	};

	class MenuStringItemRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_MENU; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawPress( HRDC hRDC, const CRect* prc );	
	};

	
	class MenuPopupTriangleRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_MENU; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawDisable( HRDC hRDC, const CRect* prc );
		void DrawTriangle( HDC hDC, const CRect* prc, int nState);
	};

	class MenuSeperatorThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_MENU; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawNormal( HRDC hRDC, const CRect* prc );
	};

	class MenuCheckedIconThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_MENU; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawDisable( HRDC hRDC, const CRect* prc );
	};

	class MenuRadioIconThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_MENU; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawDisable( HRDC hRDC, const CRect* prc );
	};

	class MenuRadioCheckIconBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_MENU; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);

		void DrawNormal( HRDC hRDC, const CRect* prc );
		void DrawHover( HRDC hRDC, const CRect* prc );
		void DrawDisable( HRDC hRDC, const CRect* prc );
	};

	class ScrollBarBkgndThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_SCROLLBAR; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize();

		virtual int   GetThemeType() { return SBP_LOWERTRACKVERT; }
	};

	class HScrollBarBkgndThemeRender : public ScrollBarBkgndThemeRender
	{
		virtual int   GetThemeType() { return SBP_LOWERTRACKHORZ; }
	};
	class VScrollBarBkgndThemeRender : public ScrollBarBkgndThemeRender
	{
		virtual int   GetThemeType() { return SBP_LOWERTRACKVERT; }
	};


	class ScrollLineButtonBkThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_SCROLLBAR; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize();

		void  DrawDisable( HRDC hRDC, const CRect* prc );
		void  DrawNormal( HRDC hRDC, const CRect* prc );
		void  DrawHover( HRDC hRDC, const CRect* prc );
		void  DrawPress( HRDC hRDC, const CRect* prc );	

		virtual int GetThemeStateDisable()   { return ABS_UPDISABLED; }
		virtual int GetThemeStateNormal()    { return ABS_UPNORMAL; }
		virtual int GetThemeStateHover()     { return ABS_UPHOT; }
		virtual int GetThemeStatePress()     { return ABS_UPPRESSED; }
		virtual int GetNoThemeState() { return DFCS_SCROLLUP; }
	};

	class ScrollLineUpButtonBkThemeRender : public ScrollLineButtonBkThemeRender
	{
		virtual int GetThemeStateDisable()   { return ABS_UPDISABLED; }
		virtual int GetThemeStateNormal()    { return ABS_UPNORMAL; }
		virtual int GetThemeStateHover()     { return ABS_UPHOT; }
		virtual int GetThemeStatePress()     { return ABS_UPPRESSED; }
		virtual int GetNoThemeState() { return DFCS_SCROLLUP; }
	};
	class ScrollLineDownButtonBkThemeRender : public ScrollLineButtonBkThemeRender
	{
		virtual int GetThemeStateDisable()   { return ABS_DOWNDISABLED; }
		virtual int GetThemeStateNormal()    { return ABS_DOWNNORMAL; }
		virtual int GetThemeStateHover()     { return ABS_DOWNHOT; }
		virtual int GetThemeStatePress()     { return ABS_DOWNPRESSED; }
		virtual int GetNoThemeState() { return DFCS_SCROLLDOWN; }
	};
	class ScrollLineLeftButtonBkThemeRender : public ScrollLineButtonBkThemeRender
	{
		virtual int GetThemeStateDisable()   { return ABS_LEFTDISABLED; }
		virtual int GetThemeStateNormal()    { return ABS_LEFTNORMAL; }
		virtual int GetThemeStateHover()     { return ABS_LEFTHOT; }
		virtual int GetThemeStatePress()     { return ABS_LEFTPRESSED; }
		virtual int GetNoThemeState() { return DFCS_SCROLLLEFT; }
	};
	class ScrollLineRightButtonBkThemeRender : public ScrollLineButtonBkThemeRender
	{
		virtual int GetThemeStateDisable()   { return ABS_RIGHTDISABLED; }
		virtual int GetThemeStateNormal()    { return ABS_RIGHTNORMAL; }
		virtual int GetThemeStateHover()     { return ABS_RIGHTHOT; }
		virtual int GetThemeStatePress()     { return ABS_RIGHTPRESSED; }
		virtual int GetNoThemeState()        { return DFCS_SCROLLRIGHT; }
	};

	class ScrollThumbButtonThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_SCROLLBAR; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize();

		void  DrawDisable( HRDC hRDC, const CRect* prc );
		void  DrawNormal( HRDC hRDC, const CRect* prc );
		void  DrawHover( HRDC hRDC, const CRect* prc );
		void  DrawPress( HRDC hRDC, const CRect* prc );
 
		virtual int GetGripperType()  { return  SBP_GRIPPERVERT; }
		virtual int GetThumbBtmType() { return  SBP_THUMBBTNVERT; }
	};
	class VScrollThumbButtonThemeRender : public ScrollThumbButtonThemeRender
	{
		virtual int GetGripperType()  { return  SBP_GRIPPERVERT; }
		virtual int GetThumbBtmType() { return  SBP_THUMBBTNVERT; }
	};
	class HScrollThumbButtonThemeRender : public ScrollThumbButtonThemeRender
	{
		virtual int GetGripperType()  { return  SBP_GRIPPERHORZ; }
		virtual int GetThumbBtmType() { return  SBP_THUMBBTNHORZ; }
	};

	class ScrollBarSizeBoxThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_SCROLLBAR; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize();
	};


	//////////////////////////////////////////////////////////////////////////

	class TooltipBkgndThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_TOOLTIP; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize();
	};
	
	//////////////////////////////////////////////////////////////////////////
	class ProgressCtrl;
	class ProgressCtrlBkgndThemeRender : public ThemeRenderBase
	{
	public:
		ProgressCtrlBkgndThemeRender() { m_pProgress = NULL; }
		virtual const TCHAR* GetThemeName() { return VSCLASS_PROGRESS; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual void  SetObject( Object* pObject );

	private:
		ProgressCtrl*   m_pProgress;
	};

	class ProgressCtrlForegndThemeRender : public ThemeRenderBase
	{
	public:
		ProgressCtrlForegndThemeRender() { m_pProgress = NULL; }
		virtual const TCHAR* GetThemeName() { return VSCLASS_PROGRESS; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual void  SetObject( Object* pObject );

	private:
		ProgressCtrl*   m_pProgress;
	};
	//////////////////////////////////////////////////////////////////////////

	class SliderCtrl;
	class SliderCtrlBkgndThemeRender : public ThemeRenderBase
	{
	public:
		SliderCtrlBkgndThemeRender() { m_pSliderCtrl = NULL; }
		virtual const TCHAR* GetThemeName() { return VSCLASS_TRACKBAR; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual void  SetObject( Object* pObject );

	private:
		SliderCtrl*   m_pSliderCtrl;
	};

	class SliderTrackButtonThemeRender : public ThemeRenderBase
	{
	public:
		SliderTrackButtonThemeRender() { m_pSliderCtrl = NULL; }
		virtual const TCHAR* GetThemeName() { return VSCLASS_TRACKBAR; }
		virtual void  DrawState(HRDC hRDC, const CRect* prc, int nState);
		virtual void  SetObject( Object* pObject );
		virtual SIZE  GetDesiredSize( );

		void DrawNoThemeState(HDC hDC, const CRect* prc, int nDrawState);
		void DrawThemeState(HRDC hRDC, const CRect* prc, int nDrawState);
		int  GetDrawThemeStateID(int iPartID, int nDrawState);

	private:
		SliderCtrl*   m_pSliderCtrl;
	};

	//////////////////////////////////////////////////////////////////////////
	//
	//                  TEXT  RENDER
	//
	//////////////////////////////////////////////////////////////////////////
	class TextRenderBase
	{
	public:
		TextRenderBase();
		virtual ~TextRenderBase(){};

	public:
		virtual bool     SetAttribute( const String& strPrefix, ATTRMAP& mapAttrib );
		virtual void     DrawState(HRDC hRDC, const CRect* prc, int nState, const String& strText, int nDrawTextFlag=-1) {};
		virtual HRFONT   GetHRFONT() = 0;
		virtual void     SetHRFont(HRFONT hRFont) = 0;
		virtual void     SetTextAlignment(int nDrawFlag);

	public:
		void             SetObject( Object* pObject ) { this->m_pObject = pObject; }
		void             SetTextRenderType( const TEXTRENDER_TYPE& nType ){ m_nTextRenderType = nType ; }
		TEXTRENDER_TYPE  GetTextRenderType() { return m_nTextRenderType; }
		SIZE             GetDesiredSize(const String& strText, int nLimitWidth=-1);
		int              GetDrawTextFlag() { return m_nDrawTextFlag; }
		void             GetTextPadding(CRegion4* pRegion4) { pRegion4->CopyRect(&m_rPadding); }
		void             SetTextPadding(CRegion4* pRegion4){ m_rPadding.CopyRect(pRegion4); }

	protected:
		Object*          m_pObject;          // �󶨵Ķ���Ҫ����˭������
		TEXTRENDER_TYPE  m_nTextRenderType;  // �Լ�������
		int              m_nDrawTextFlag;    // DrawText��flag��ʶ
		CRegion4         m_rPadding;         // �������ֵ��ڼ��
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

		virtual bool     SetAttribute( const String& strPrefix, map<String,String>& mapAttrib );
		virtual void     DrawState(HRDC hRDC, const CRect* prc, int nState, const String& strText, int nDrawTextFlag=-1);
		virtual HRFONT   GetHRFONT(){ return m_hFont; }
		virtual void     SetHRFont(HRFONT hRFont);

	private:
		UIColor*     m_pColorText;
		HRFONT       m_hFont;
	};

	class ColorListTextRender : public TextRenderBase
	{
	public:
		ColorListTextRender();
		~ColorListTextRender();

		virtual bool     SetAttribute( const String& strPrefix, map<String,String>& mapAttrib );
		virtual void     DrawState(HRDC hRDC, const CRect* prc, int nState, const String& strText, int nDrawTextFlag=-1);
		virtual HRFONT   GetHRFONT();
		virtual void     SetHRFont(HRFONT hRFont);

		void SetCount( int nCount );
		void SetColor( int nIndex, COLORREF col );

	protected:
		void Clear();

	private:
		vector<UIColor*>  m_vTextColor;
		HRFONT            m_hFont;
		int               m_nCount;
	};

#if 0
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

		virtual bool     SetAttribute( const String& strPrefix, map<String,String>& mapAttrib );
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