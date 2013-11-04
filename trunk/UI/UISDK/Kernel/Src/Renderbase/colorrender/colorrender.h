#pragma once
#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"

namespace UI
{


//
// 背景颜色+边框颜色（两者都是可选的）
//
class ColorRender : public RenderBase
{
public:
    ColorRender();
    ~ColorRender();

    UI_DECLARE_RENDERBASE(ColorRender, XML_RENDER_TYPE_COLOR, RENDER_TYPE_COLOR)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

    void  SetIColorRender(IColorRender* p) { m_pIColorRender = p; }

	void  SetAttribute(SetAttrPrefixData* pData);
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

public:
	void  SetBkColor(COLORREF col);
    void  SetBorderColor(COLORREF col);
    void  SetBorderRegion(const CRegion4* prc);
public:
    IColorRender*  m_pIColorRender;

	Color*  m_pBkColor;
    Color*  m_pBorderColor;
    CRegion4  m_rcBorder;
};

// 特用于某些控件需要使用系统主题颜色背景，当系统主题改变时，相应的颜色也需要改变 
class SysColorRender : public /*Theme*/RenderBase
{
public:
    SysColorRender();
    ~SysColorRender();

    UI_DECLARE_RENDERBASE(SysColorRender, XML_RENDER_TYPE_SYSCOLOR, RENDER_TYPE_THEME_SYSCOLOR)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

    void  SetISysColorRender(ISysColorRender* p) { m_pISysColorRender = p; }

	void  SetAttribute(SetAttrPrefixData* pData);
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    void  SetBkColor(int nColorIndex);
    void  SetBorderColor(int nColorIndex);

    // 由于改成每次重新获取颜色值，因此不需要响应主题改变的消息
	// virtual const TCHAR* GetThemeName() { return NULL; }
	// virtual void  OnThemeChanged();

public:
    ISysColorRender*  m_pISysColorRender;
	int  m_nBkColorIndex;
	int  m_nBorderColorIndex;
    CRegion4  m_rcBorder;

	// COLORREF    m_bkColor;     // 由于在WM_THEMECHANGED消息中，直接调用GetSysColor获取到的值仍然有可能是旧的
	// COLORREF    m_borderColor; // 因此这里放弃保存颜色值，而是每次绘制时都调用GetSysColor
};

// 横向渐变颜色背景
class GradientRender : public RenderBase
{
public:
    GradientRender();
    ~GradientRender();

    UI_DECLARE_RENDERBASE(GradientRender, XML_RENDER_TYPE_GRADIENT, RENDER_TYPE_GRADIENT)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

    void  SetIGradientRender(IGradientRender* p) { m_pIGradientRender = p; }

	void  SetAttribute(SetAttrPrefixData* pData);
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

public:
    IGradientRender*  m_pIGradientRender;
	Color*  m_pColorFrom;
	Color*  m_pColorTo;
    Color*  m_pBorderColor;
    CRegion4  m_rcBorder;

    int  m_nGradientDir;
};



//
//	直接使用颜色值作为背景
//
class ColorListRender : public RenderBase
{
public:
    ColorListRender();
    ~ColorListRender();

    UI_DECLARE_RENDERBASE(ColorListRender, XML_RENDER_TYPE_COLORLIST, RENDER_TYPE_COLORLIST)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)
	
    void  SetIColorListRender(IColorListRender* p) { m_pIColorListRender = p; }
	void  Clear();

    void  SetAttribute(SetAttrPrefixData* pData);
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

	void  SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder);
	void  SetCount(int n);

private:
    IColorListRender*  m_pIColorListRender;
	vector<Color*>  m_vBkColor;
	vector<Color*>  m_vBorderColor;
	int   m_nCount; 

};
}