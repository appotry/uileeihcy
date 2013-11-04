#pragma once
#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"

namespace UI
{


//
// ������ɫ+�߿���ɫ�����߶��ǿ�ѡ�ģ�
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

// ������ĳЩ�ؼ���Ҫʹ��ϵͳ������ɫ��������ϵͳ����ı�ʱ����Ӧ����ɫҲ��Ҫ�ı� 
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

    // ���ڸĳ�ÿ�����»�ȡ��ɫֵ����˲���Ҫ��Ӧ����ı����Ϣ
	// virtual const TCHAR* GetThemeName() { return NULL; }
	// virtual void  OnThemeChanged();

public:
    ISysColorRender*  m_pISysColorRender;
	int  m_nBkColorIndex;
	int  m_nBorderColorIndex;
    CRegion4  m_rcBorder;

	// COLORREF    m_bkColor;     // ������WM_THEMECHANGED��Ϣ�У�ֱ�ӵ���GetSysColor��ȡ����ֵ��Ȼ�п����Ǿɵ�
	// COLORREF    m_borderColor; // ����������������ɫֵ������ÿ�λ���ʱ������GetSysColor
};

// ���򽥱���ɫ����
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
//	ֱ��ʹ����ɫֵ��Ϊ����
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