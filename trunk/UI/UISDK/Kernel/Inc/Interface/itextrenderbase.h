#ifndef ITEXTRENDERBASE_H_19F7C909_6E06_4b1f_B5A2_0F670807EF6A
#define ITEXTRENDERBASE_H_19F7C909_6E06_4b1f_B5A2_0F670807EF6A
#include "UISDK\Kernel\Inc\Interface\irenderbase.h"
#include "UISDK\Kernel\Inc\Interface\renderlibrary.h"
namespace UI
{

enum TEXTRENDER_TYPE
{
    TEXTRENDER_TYPE_NULL,
    TEXTRENDER_TYPE_NORMAL,
    TEXTRENDER_TYPE_COLORLIST,
    TEXTRENDER_TYPE_FONTLIST,
    TEXTRENDER_TYPE_FONTCOLORLIST,

    TEXTRENDER_TYPE_THEME_FIRST,
    TEXTRENDER_TYPE_THEME_MENU,
    TEXTRENDER_TYPE_THEME_LISTVIEW,
    TEXTRENDER_TYPE_THEME_LAST,

    TEXTRENDER_TYPE_CONTRAST_FIRST,
    TEXTRENDER_TYPE_CONTRASTCOLOR,
    TEXTRENDER_TYPE_CONTRASTCOLORLIST,
    TEXTRENDER_TYPE_CONTRAST_LAST,
};

#define TEXTRENDER_STATE_MASK                0xFFFF0000
#define TEXTRENDER_STATE_NORMAL              0x00010000
#define TEXTRENDER_STATE_SELECTED            0x00020000
#define TEXTRENDER_STATE_SELECTED_NOTFOCUS   0x00030000

interface IRenderFont;
class TextRenderBase; 
interface UISDKAPI ITextRenderBase : public IMessage
{
    UI_DECLARE_Ixxx_INTERFACE(ITextRenderBase, TextRenderBase)

    long  AddRef();
    long  Release();

    IRenderFont*  GetRenderFont();

    void  SetObject(IUIApplication*  pUIApp, IObject* pObject);
    IObject*  GetObject();

    void  SetTextRenderType(const TEXTRENDER_TYPE& nType);
    TEXTRENDER_TYPE  GetTextRenderType();
    void  SetTextAlignment(int nDrawFlag);
    int   GetTextAlignment();
    void  SetTextEffect(TEXT_EFFECT);
    TEXT_EFFECT  GetTextEffect();
    void  SetDrawTextParam(WPARAM w, LPARAM l);

    void  SetAttribute(IMapAttribute* pMapAttrib, const TCHAR* szPrefix, bool bErase);
    SIZE  GetDesiredSize(const TCHAR* szText, int nLimitWidth=-1);
    void  DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState, const TCHAR* szText, int nDrawTextFlag=-1);
    void  Init();
    
    void  CheckSkinMaterialChanged();

protected:
    long  m_lRef;
};

class ThemeTextRenderBase;
interface UISDKAPI IThemeTextRenderBase : public ITextRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IThemeTextRenderBase, ThemeTextRenderBase)

    const TCHAR*  GetThemeName();
    HTHEME  GetTHEME();
};

class NormalTextRender;
interface UISDKAPI INormalTextRender : public ITextRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(INormalTextRender, NormalTextRender);

    void  SetRenderFont(IRenderFont*);
    void  SetColor(Color* pColText);
};

class ContrastColorTextRender;
interface UISDKAPI IContrastColorTextRender : public ITextRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IContrastColorTextRender, ContrastColorTextRender)
};

class ContrastColorListTextRender;
interface UISDKAPI IContrastColorListTextRender : public ITextRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IContrastColorListTextRender, ContrastColorListTextRender)
};

class ColorListTextRender;
interface UISDKAPI IColorListTextRender : public ITextRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IColorListTextRender, ColorListTextRender)

    void  SetRenderFont(IRenderFont*);
    void  SetCount(int nCount);
    void  SetColor(int nIndex, COLORREF col);
};

class FontColorListTextRender;
interface UISDKAPI IFontColorListTextRender : public ITextRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IFontColorListTextRender, FontColorListTextRender);

    void  SetCount(int nCount);
	void  SetColor(int nIndex, DWORD color);
	void  SetFont(int nIndex, IRenderFont*);
};
}

#endif // ITEXTRENDERBASE_H_19F7C909_6E06_4b1f_B5A2_0F670807EF6A