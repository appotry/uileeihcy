#pragma once
#include "uxtheme.h"
#include "Vsstyle.h"
#pragma comment(lib, "uxtheme.lib")

//
//	libo 20110817 (hangzhou xingyao jiari jiudian 8332)
//	
//	为了实现用一按钮类就能完成各种样式按钮的展现，同时还能将按钮的逻辑和绘制分开，
//	因此从_Button类中添加一个ButtonBkRender的指针，根据不同的配置来实现不同的展现
//
//	而在render这一块，分为bk和icontext，然后icontext分为icon和text，充分考虑每一种
//	绘制元素的多样性
//
namespace UI
{
class ButtonBase;
class Edit;
class GroupBox;


class _ButtonIconRender : public Message
{
public:
	_ButtonIconRender( ButtonBase* pButton );
	virtual ~_ButtonIconRender();

	UI_BEGIN_MSG_MAP
	UI_END_MSG_MAP

public:

	virtual  void  ResetAttribute();
	virtual  bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	virtual  SIZE  GetAutoSize( HRDC hRDC );

	virtual void DrawDisable( HRDC hRDC, RECT* prc ){};
	virtual void DrawNormal( HRDC hRDC, RECT* prc ){};
	virtual void DrawHover( HRDC hRDC, RECT* prc ){};
	virtual void DrawPress( HRDC hRDC, RECT* prc ){};

protected:
	HRBITMAP    m_hIcon;    
	ButtonBase*    m_pButton;
};

//
//	静态一张图标的绘制
//
class ButtonIconStaticRender : public _ButtonIconRender
{
public:
	ButtonIconStaticRender( ButtonBase* pButton );
	~ButtonIconStaticRender();

	void DrawDisable( HRDC hRDC, RECT* prc );
	void DrawNormal( HRDC hRDC, RECT* prc );
	void DrawHover( HRDC hRDC, RECT* prc );
	void DrawPress( HRDC hRDC, RECT* prc );

};

//
//	radio/check按钮的8态图标绘制
//
//  默认8态图片的顺序为:
//		normal      hover    press    disable
//		check_n     check_h  check_p  chech_d
//
class ButtonIcon8StateRender : public _ButtonIconRender
{
public:
	ButtonIcon8StateRender( ButtonBase* pButton );
	~ButtonIcon8StateRender();

	virtual  bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	virtual  SIZE  GetAutoSize( HRDC hRDC );

	void DrawDisable( HRDC hRDC, RECT* prc );
	void DrawNormal( HRDC hRDC, RECT* prc );
	void DrawHover( HRDC hRDC, RECT* prc );
	void DrawPress( HRDC hRDC, RECT* prc );

protected:
	void draw_item( HRDC hRDC, CRect* prc, int xIcon, int yIcon );

private:
	int  m_nIconItemWidth;   // 8个状态中的一个状态的高和宽
	int  m_nIconItemHeight;
};

//
//	主题样式的单选图标
//
class ButtonIconThemeRadioRender : public _ButtonIconRender
{
public:
	ButtonIconThemeRadioRender( ButtonBase* pButton );
	~ButtonIconThemeRadioRender();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(_ButtonIconRender)
	UI_END_MSG_MAP

	void DrawDisable( HRDC hRDC, RECT* prc );
	void DrawNormal( HRDC hRDC, RECT* prc );
	void DrawHover( HRDC hRDC, RECT* prc );
	void DrawPress( HRDC hRDC, RECT* prc );

	void     OnThemeChanged();
	virtual  SIZE  GetAutoSize( HRDC hRDC );

private:
	HTHEME    m_hTheme;
};
//
//	主题样式的复选图标
//
class ButtonIconThemeCheckRender : public _ButtonIconRender
{
public:
	ButtonIconThemeCheckRender( ButtonBase* pButton );
	~ButtonIconThemeCheckRender();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(_ButtonIconRender)
	UI_END_MSG_MAP

	void DrawDisable( HRDC hRDC, RECT* prc );
	void DrawNormal( HRDC hRDC, RECT* prc );
	void DrawHover( HRDC hRDC, RECT* prc );
	void DrawPress( HRDC hRDC, RECT* prc );

	void     OnThemeChanged();
	virtual  SIZE  GetAutoSize( HRDC hRDC );

private:
	HTHEME    m_hTheme;
};

class _ButtonTextRender
{
public:
	_ButtonTextRender( ButtonBase* pButton );
	~_ButtonTextRender();

	virtual  bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	virtual  SIZE  GetAutoSize( HRDC hRDC );

	virtual void DrawDisable( HRDC hRDC, RECT* prc ){};
	virtual void DrawNormal( HRDC hRDC, RECT* prc ){};
	virtual void DrawHover( HRDC hRDC, RECT* prc ){};
	virtual void DrawPress( HRDC hRDC, RECT* prc ){};

protected:
	ButtonBase*   m_pButton;

	// 文字颜色
	COLORREF   m_colNormal;
	COLORREF   m_colHover;
	COLORREF   m_colPress;
	COLORREF   m_colDisable;

	// 文本内容
	String     m_text;
};

class ButtonTextSinglelineRender : public _ButtonTextRender
{
public:
	ButtonTextSinglelineRender( ButtonBase*  pButton );
	~ButtonTextSinglelineRender();

public:
	virtual void DrawDisable( HRDC hRDC, RECT* prc );
	virtual void DrawNormal( HRDC hRDC, RECT* prc );
	virtual void DrawHover( HRDC hRDC, RECT* prc );
	virtual void DrawPress( HRDC hRDC, RECT* prc );
};

class ButtonTextHyperLinkRender : public ButtonTextSinglelineRender
{
public:
	ButtonTextHyperLinkRender( ButtonBase*  pButton );
	~ButtonTextHyperLinkRender();

public:
	virtual void DrawDisable( HRDC hRDC, RECT* prc );
	virtual void DrawNormal( HRDC hRDC, RECT* prc );
	virtual void DrawHover( HRDC hRDC, RECT* prc );
	virtual void DrawPress( HRDC hRDC, RECT* prc );

protected:
//	HFONT   CreateFontUnderLine( HRDC );

};

#define BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_LEFT    0x00     // 按钮位于文字左侧
#define BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_TOP     0x01     // 按钮位于文字上侧
#define BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_RIGHT   0x02     // 按钮位于文字右侧
#define BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_BOTTOM  0x04     // 按钮位于文字下侧

class ButtonIconTextRender : public Message
{
public:
	ButtonIconTextRender( ButtonBase* pButton );
	~ButtonIconTextRender();

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP_POINT_MEMBER(m_pIconRender)
	UI_END_MSG_MAP

public:
	
	void ResetAttribute();
	bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );
	void DrawFocus( HRDC hRDC, RECT* prcIcon, RECT* prcText );

	SIZE GetAutoSize(HRDC hRDC);

	void SetTextRenderType( BUTTON_TEXT_RENDER_TYPE eType );

private:
	void analyzeIconTextRect( HRDC hRDC, RECT& rcIcon, RECT& rcText);

public:
	_ButtonIconRender*   m_pIconRender;
	_ButtonTextRender*   m_pTextRender;

protected:
	ButtonBase*  m_pButton;

	int     m_nFlag;             // 图标与文本的位置关系
	int     m_nIconMarginText;   // 图标距离文本的大小
	int     m_nDrawFocusType;    // 如何绘制焦点样式 
};

//
//	背景渲染基类
//
//	Remark 
//		从Message继承，是为了处理Theme变化时，接收该消息
//
class BackRenderBase : public Message
{
public:
	BackRenderBase( Object* m_pObject );

	UI_BEGIN_MSG_MAP
	UI_END_MSG_MAP

public:
	static BackRenderBase*  CreateInstance(const String& strType, Object* pObj); 

	void ResetAttribute(){};
	virtual bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false ){return true;};

	virtual void DrawDisable( HRDC hRDC  )=0;
	virtual void DrawNormal( HRDC hRDC )=0;
	virtual void DrawHover( HRDC hRDC )=0;
	virtual void DrawPress( HRDC hRDC )=0;

	virtual SIZE GetAutoSize( HRDC hRDC ){ SIZE s={0,0}; return s; };  // 按钮按钮背景大小，目前只对于图片背景有用

protected:
	Object*   m_pObject;
};

#if 0
//
//	直接使用颜色值作为背景
//
//	Q: 如何实现不需要背景的状态，例如工具栏按钮的正常状态，是没有背景的?
//	A: 用状态位来实现，设置了某个状态的颜色值，就表示该状态需要绘制背景，否则就不绘制
//
class ColorBackRender : public BackRenderBase
{
public:
	ColorBackRender( Object* pObj );

	bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	
private:
	void drawbk( HRDC hRDC, COLORREF colbrush, COLORREF colpen );

private:
	COLORREF   m_colNormalPen;
	COLORREF   m_colNormalBrush;

	COLORREF   m_colHoverPen;
	COLORREF   m_colHoverBrush;

	COLORREF   m_colPressPen;
	COLORREF   m_colPressBrush;

	COLORREF   m_colDisablePen;
	COLORREF   m_colDisableBrush;

	int        m_nFlag;
};

//
//	图片背景按钮
//
class ImageListRender : public BackRenderBase
{
public:
	ImageListRender( Object* pObj );
	~ImageListRender( );

	bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	

	virtual SIZE GetAutoSize( HRDC hRDC );

private:
	HRBITMAP   m_hBitmap;

	int        m_nImageDirection;
	int        m_nItemWidth;          // 图片项的宽度
	int        m_nItemHeight;         // 图片项的高度
};

//
//	主题背景按钮
//
class ButtonBkThemeRender : public BackRenderBase
{
public:
	ButtonBkThemeRender( ButtonBase* pButton );
	~ButtonBkThemeRender( );

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(BackRenderBase)
	UI_END_MSG_MAP
public:

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	

	void OnThemeChanged();
private:
	HTHEME    m_hTheme;
};


//
//	主题背景按钮
//
class EditBkThemeRender : public BackRenderBase
{
public:
	EditBkThemeRender( Edit* pEdit );
	~EditBkThemeRender( );

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(BackRenderBase)
	UI_END_MSG_MAP
public:

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	

	void OnThemeChanged();
private:
	HTHEME    m_hTheme;
};

class GroupBoxBkThemeRender : public BackRenderBase
{
public:
	GroupBoxBkThemeRender( GroupBox* pGroupBox );
	~GroupBoxBkThemeRender( );

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(BackRenderBase)
	UI_END_MSG_MAP
public:

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	

	void OnThemeChanged();
private:
	HTHEME    m_hTheme;
};

#endif
//
//	按钮的渲染类，包含三个元素的渲染：背景、图标、文字。被_Button引用
//
class ButtonRender : public Message
{
public:
	ButtonRender( );
	~ButtonRender();

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP_POINT_MEMBER(m_pBkRender)
		UICHAIN_MSG_MAP_POINT_MEMBER(m_pIconTextRender)
	UI_END_MSG_MAP

public:

	void Draw(HRDC hRDC);   // 分发各种绘制状态

	void DrawDisable( HRDC hRDC );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );

	void ResetAttribute();
	bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	void SetButtonPtr( ButtonBase* ptr ) { m_pButton = ptr; }
	SIZE GetAutoSize(HRDC hRDC);

	void SetTextRenderType( BUTTON_TEXT_RENDER_TYPE eType );
	_ButtonTextRender*    GetTextRender();
	_ButtonIconRender*    GetIconRender();
	ButtonIconTextRender* GetIconTextRender();

private:
	ButtonBase*            m_pButton;              // 为了方便引用按钮的一些数据
	BackRenderBase*        m_pBkRender;            // 背景渲染器
	ButtonIconTextRender*  m_pIconTextRender;

	int                    m_nAutoSizeType;        // 按钮自动调整大小类型
};

}

