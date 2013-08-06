#pragma  once
#include <Vsstyle.h>

namespace UI
{

class ButtonBkThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(ButtonBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_BUTTON_BKGND, CONTROL_BUTTON, BUTTON_STYLE_PUSHBUTTON)

    UI_BEGIN_MSG_MAP_Ixxx(ButtonBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(OnDrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_BUTTON; }
    void  OnDrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    int   GetThemeStateId(int nState);
    UINT  GetNoThemeStateId(int nState);
};


//
//	主题样式的单选图标
//
class RadioButtonThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(RadioButtonThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_RADIO_BUTTON, CONTROL_RADIOBUTTON, -1)


    UI_BEGIN_MSG_MAP_Ixxx(RadioButtonThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_BUTTON; }
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

	void  GetDesiredSize(SIZE* pSize);
    int   GetThemeStateId(int nState);
    UINT  GetNoThemeStateId(int nState);
};

//
//	主题样式的复选图标
//
class CheckButtonThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(CheckButtonThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_CHECK_BUTTON, CONTROL_CHECKBUTTON, -1)

    UI_BEGIN_MSG_MAP_Ixxx(CheckButtonThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_BUTTON; }
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
	
    void  GetDesiredSize(SIZE* pSize);
    int   GetThemeStateId(int nState);
    UINT  GetNoThemeStateId(int nState);
};


//
//	编辑框主题背景
//
class EditBkThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(EditBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_EDIT_BKGND, CONTROL_EDIT, -1)

    UI_BEGIN_MSG_MAP_Ixxx(EditBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_EDIT; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    int   GetThemeStateId(int nState);
};


class GroupBoxBkThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(GroupBoxBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_GROUPBOX_BKGND, CONTROL_GROUPBOX, -1)

    UI_BEGIN_MSG_MAP_Ixxx(GroupBoxBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
     UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_BUTTON; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

	void  DrawDisable( IRenderTarget*, const CRect* prc );
	void  DrawNormal( IRenderTarget*, const CRect* prc );
};

//////////////////////////////////////////////////////////////////////////
interface IProgressCtrl;
class ProgressCtrlBkgndThemeRender : public IThemeRenderBase
{
public:
    ProgressCtrlBkgndThemeRender() { m_pIProgressCtrl = NULL; }
    UI_DECLARE_RENDERBASE2(ProgressCtrlBkgndThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_PROGRESS_BKGND, CONTROL_PROGRESSCTRL, -1)

    UI_BEGIN_MSG_MAP_Ixxx(ProgressCtrlBkgndThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
        UIMSG_WM_INITIALIZE(OnInit)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_PROGRESS; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  OnInit();

protected:
    IProgressCtrl*   m_pIProgressCtrl;
};

class ProgressCtrlForegndThemeRender : public IThemeRenderBase
{
public:
    ProgressCtrlForegndThemeRender() { m_pIProgressCtrl = NULL; }
    UI_DECLARE_RENDERBASE2(ProgressCtrlForegndThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_PROGRESS_FOREGND, CONTROL_PROGRESSCTRL, -1)

    UI_BEGIN_MSG_MAP_Ixxx(ProgressCtrlForegndThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
        UIMSG_WM_INITIALIZE(OnInit)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_PROGRESS; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  OnInit();

protected:
    IProgressCtrl*   m_pIProgressCtrl;

};

//////////////////////////////////////////////////////////////////////////

interface ISliderCtrl;
class SliderCtrlBkgndThemeRender : public IThemeRenderBase
{
public:
    SliderCtrlBkgndThemeRender() { m_pISliderCtrl = NULL; }
    UI_DECLARE_RENDERBASE2(SliderCtrlBkgndThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_SLIDERCTRL_BKGND, CONTROL_SLIDERCTRL, -1)

    UI_BEGIN_MSG_MAP_Ixxx(SliderCtrlBkgndThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
        UIMSG_WM_INITIALIZE(OnInit)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_TRACKBAR; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  OnInit();

private:
    ISliderCtrl*   m_pISliderCtrl;
};


class SliderTrackButtonThemeRender : public IThemeRenderBase
{
public:
    SliderTrackButtonThemeRender() { m_pISliderCtrl = NULL; }
    UI_DECLARE_RENDERBASE2(SliderTrackButtonThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_SLIDER_TRACK_BUTTON, CONTROL_BUTTON, BUTOTN_STYLE_SLIDERTRACKBUTTON)

    UI_BEGIN_MSG_MAP_Ixxx(SliderTrackButtonThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
        UIMSG_WM_INITIALIZE(OnInit)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_TRACKBAR; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  OnInit();
    void  GetDesiredSize(SIZE* pSize);

public:
    void  DrawNoThemeState(HDC hDC, const CRect* prc, int nDrawState);
    void  DrawThemeState(IRenderTarget* , const CRect* prc, int nDrawState);
    int   GetDrawThemeStateID(int iPartID, int nDrawState);

private:
    ISliderCtrl*   m_pISliderCtrl;
};

//////////////////////////////////////////////////////////////////////////

class HeaderCtrlItemBkgndThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(HeaderCtrlItemBkgndThemeRender, XML_RENDER_TYPE_THEME, HEADER_TYPE_THEME_HEADERCTRL_ITEM_BKGND, CONTROL_HEADERCTRL, -1)

    UI_BEGIN_MSG_MAP_Ixxx(HeaderCtrlItemBkgndThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_HEADER; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  GetDesiredSize(SIZE* pSize);
};

class HeaderCtrlRightBlankThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(HeaderCtrlRightBlankThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_HEADERCTRL_RIGHT_BLANK, CONTROL_HEADERCTRL, -1)

    UI_BEGIN_MSG_MAP_Ixxx(HeaderCtrlRightBlankThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_HEADER; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
};


class ScrollBarBkgndThemeRender : public IThemeRenderBase
{
public:
    UI_BEGIN_MSG_MAP_Ixxx(ScrollBarBkgndThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_SCROLLBAR; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  GetDesiredSize(SIZE* pSize);

protected:
    virtual int   GetThemeType() { return SBP_LOWERTRACKVERT; }
};

class HScrollBarBkgndThemeRender : public ScrollBarBkgndThemeRender
{
public:
    UI_DECLARE_RENDERBASE2(HScrollBarBkgndThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_HSCROLLBARBACKGND, CONTROL_HSCROLLBAR, -1)
    
    UI_BEGIN_MSG_MAP_Ixxx(HScrollBarBkgndThemeRender)
    UI_END_MSG_MAP_CHAIN_PARENT(ScrollBarBkgndThemeRender)
protected:
    virtual int  GetThemeType() { return SBP_LOWERTRACKHORZ; }
};
class VScrollBarBkgndThemeRender : public ScrollBarBkgndThemeRender
{
public:
    UI_DECLARE_RENDERBASE2(VScrollBarBkgndThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_VSCROLLBARBACKGND, CONTROL_VSCROLLBAR, -1)

    UI_BEGIN_MSG_MAP_Ixxx(VScrollBarBkgndThemeRender)
    UI_END_MSG_MAP_CHAIN_PARENT(ScrollBarBkgndThemeRender)
protected:
    virtual int  GetThemeType() { return SBP_LOWERTRACKVERT; }
};


class ScrollLineButtonBkThemeRender : public IThemeRenderBase
{
public:
    UI_BEGIN_MSG_MAP_Ixxx(ScrollLineButtonBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_SCROLLBAR; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  GetDesiredSize(SIZE* pSize);

    int  GetThemeStateId(int nState);
    int  GetNoThemeStateId(int nState);

protected:
    virtual int GetThemeStateDisable()   { return ABS_UPDISABLED; }
    virtual int GetThemeStateNormal()    { return ABS_UPNORMAL; }
    virtual int GetThemeStateHover()     { return ABS_UPHOT; }
    virtual int GetThemeStatePress()     { return ABS_UPPRESSED; }
    virtual int GetNoThemeState() { return DFCS_SCROLLUP; }
};

class ScrollLineUpButtonBkThemeRender : public ScrollLineButtonBkThemeRender
{
public:
    UI_DECLARE_RENDERBASE2(ScrollLineUpButtonBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_SCROLLBAR_LINE_UP_BUTTON, CONTROL_VSCROLLBAR, -1);
protected:
    virtual int GetThemeStateDisable()   { return ABS_UPDISABLED; }
    virtual int GetThemeStateNormal()    { return ABS_UPNORMAL; }
    virtual int GetThemeStateHover()     { return ABS_UPHOT; }
    virtual int GetThemeStatePress()     { return ABS_UPPRESSED; }
    virtual int GetNoThemeState() { return DFCS_SCROLLUP; }
};
class ScrollLineDownButtonBkThemeRender : public ScrollLineButtonBkThemeRender
{
public:
    UI_DECLARE_RENDERBASE2(ScrollLineDownButtonBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_SCROLLBAR_LINE_DOWN_BUTTON, CONTROL_VSCROLLBAR, -1);
protected:
    virtual int GetThemeStateDisable()   { return ABS_DOWNDISABLED; }
    virtual int GetThemeStateNormal()    { return ABS_DOWNNORMAL; }
    virtual int GetThemeStateHover()     { return ABS_DOWNHOT; }
    virtual int GetThemeStatePress()     { return ABS_DOWNPRESSED; }
    virtual int GetNoThemeState() { return DFCS_SCROLLDOWN; }
};
class ScrollLineLeftButtonBkThemeRender : public ScrollLineButtonBkThemeRender
{
public:
    UI_DECLARE_RENDERBASE2(ScrollLineLeftButtonBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_SCROLLBAR_LINE_LEFT_BUTTON, CONTROL_HSCROLLBAR, -1);
protected:
    virtual int GetThemeStateDisable()   { return ABS_LEFTDISABLED; }
    virtual int GetThemeStateNormal()    { return ABS_LEFTNORMAL; }
    virtual int GetThemeStateHover()     { return ABS_LEFTHOT; }
    virtual int GetThemeStatePress()     { return ABS_LEFTPRESSED; }
    virtual int GetNoThemeState() { return DFCS_SCROLLLEFT; }
};
class ScrollLineRightButtonBkThemeRender : public ScrollLineButtonBkThemeRender
{
public:
    UI_DECLARE_RENDERBASE2(ScrollLineRightButtonBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_SCROLLBAR_LINE_RIGHT_BUTTON, CONTROL_HSCROLLBAR, -1);
protected:
    virtual int GetThemeStateDisable()   { return ABS_RIGHTDISABLED; }
    virtual int GetThemeStateNormal()    { return ABS_RIGHTNORMAL; }
    virtual int GetThemeStateHover()     { return ABS_RIGHTHOT; }
    virtual int GetThemeStatePress()     { return ABS_RIGHTPRESSED; }
    virtual int GetNoThemeState()        { return DFCS_SCROLLRIGHT; }
};


class ScrollThumbButtonThemeRender : public IThemeRenderBase
{
public:

    UI_BEGIN_MSG_MAP_Ixxx(ScrollThumbButtonThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_SCROLLBAR; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  GetDesiredSize(SIZE* pSize);

protected:
    int  GetThemeStateId(int nState);
    int  GetNoThemeStateId(int nState);

    virtual int  GetGripperType()  { return  SBP_GRIPPERVERT; }
    virtual int  GetThumbBtmType() { return  SBP_THUMBBTNVERT; }
};

class VScrollThumbButtonThemeRender : public ScrollThumbButtonThemeRender
{
public:
    UI_DECLARE_RENDERBASE2(VScrollThumbButtonThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_VSCROLLBAR_THUMB_BUTTON, CONTROL_VSCROLLBAR, -1);
protected:

    virtual int GetGripperType()  { return  SBP_GRIPPERVERT; }
    virtual int GetThumbBtmType() { return  SBP_THUMBBTNVERT; }
};
class HScrollThumbButtonThemeRender : public ScrollThumbButtonThemeRender
{
public:
    UI_DECLARE_RENDERBASE2(HScrollThumbButtonThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_HSCROLLBAR_THUMB_BUTTON, CONTROL_HSCROLLBAR, -1);
protected:

    virtual int GetGripperType()  { return  SBP_GRIPPERHORZ; }
    virtual int GetThumbBtmType() { return  SBP_THUMBBTNHORZ; }
};

// 列表框背景绘制
class ListboxBkThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(ListboxBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_LISTBOX_BKGND, CONTROL_LISTBOX, -1);

    UI_BEGIN_MSG_MAP_Ixxx(ListboxBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_LISTBOX; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    int  GetThemeStateId(int nState);
};

class ComboboxButtonBkThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(ComboboxButtonBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_COMBOBOX_BUTTON, CONTROL_BUTTON, BUTTON_STYLE_COMBOBOX);

    UI_BEGIN_MSG_MAP_Ixxx(ComboboxButtonBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_COMBOBOX; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  GetDesiredSize(SIZE* pSize);

    int  GetThemeStateId(int nState);
    int  GetNoThemeStateId(int nState);

    //		void DrawGlyph( HDC hDC, const CRect* prc, bool bPressDown=false);
};

class ComboboxBkThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(ComboboxBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_COMBOBOX_BKGND, CONTROL_COMBOBOX, -1);

    UI_BEGIN_MSG_MAP_Ixxx(ComboboxBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_COMBOBOX; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    int   GetThemeStateId(int nState);

    void DrawDisable(IRenderTarget*, const CRect* prc );
    void DrawNormal(IRenderTarget*, const CRect* prc );
    void DrawHover(IRenderTarget*, const CRect* prc );
    void DrawPress(IRenderTarget*, const CRect* prc );	

    void DrawReadonlyDisable(IRenderTarget*, const CRect* prc);
    void DrawReadonlyNormal(IRenderTarget*, const CRect* prc);
    void DrawReadonlyHover(IRenderTarget*, const CRect* prc);
    void DrawReadonlyPress(IRenderTarget*, const CRect* prc);	
};

interface IMenu;
class MenuBkThemeRender : public IThemeRenderBase
{
public:
    MenuBkThemeRender();

    UI_DECLARE_RENDERBASE2(MenuBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_MENU_BKGND, CONTROL_MENU, -1);

    UI_BEGIN_MSG_MAP_Ixxx(MenuBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_MENU; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  OnInitialize();

private:
    IMenu*  m_pIMenu;
};


class MenuSeperatorThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(MenuSeperatorThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_MENUSEPERATOR, CONTROL_MENU, -1);

    UI_BEGIN_MSG_MAP_Ixxx(MenuSeperatorThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_MENU; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
};

class MenuStringItemRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(MenuStringItemRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_MENUSTRINGITEM, CONTROL_MENU, -1);

    UI_BEGIN_MSG_MAP_Ixxx(MenuStringItemRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_MENU; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    void  DrawDisable(IRenderTarget*, const CRect* prc);
    void  DrawNormal(IRenderTarget*, const CRect* prc);
    void  DrawHover(IRenderTarget*, const CRect* prc);
    void  DrawPress(IRenderTarget*, const CRect* prc);	
};


class MenuPopupTriangleRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(MenuPopupTriangleRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_MENUPOPUPTRIANGLE, CONTROL_MENU, -1);

    UI_BEGIN_MSG_MAP_Ixxx(MenuPopupTriangleRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_MENU; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    void  DrawNormal( IRenderTarget*, const CRect* prc );
    void  DrawHover( IRenderTarget*, const CRect* prc );
    void  DrawDisable( IRenderTarget*, const CRect* prc );
    void  DrawTriangle( HDC hDC, const CRect* prc, int nState);
};

class MenuCheckedIconThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(MenuCheckedIconThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_MENUCHECKICON, CONTROL_MENU, -1);

    UI_BEGIN_MSG_MAP_Ixxx(MenuCheckedIconThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_MENU; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    void  DrawNormal( IRenderTarget*, const CRect* prc );
    void  DrawHover( IRenderTarget*, const CRect* prc );
    void  DrawDisable( IRenderTarget*, const CRect* prc );
};

class MenuRadioIconThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(MenuRadioIconThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_MENURADIOICON, CONTROL_MENU, -1);

    UI_BEGIN_MSG_MAP_Ixxx(MenuRadioIconThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_MENU; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

    void  DrawNormal(IRenderTarget*, const CRect* prc);
    void  DrawHover(IRenderTarget*, const CRect* prc);
    void  DrawDisable(IRenderTarget*, const CRect* prc);
};

class MenuRadioCheckIconBkThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(MenuRadioCheckIconBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_MENURADIOCHECKICONBK, CONTROL_MENU, -1);

    UI_BEGIN_MSG_MAP_Ixxx(MenuRadioCheckIconBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_MENU; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
};

class MenuTextThemeRender : public IThemeTextRenderBase
{
public:
    MenuTextThemeRender();
    ~MenuTextThemeRender();

    UI_DECLARE_TEXTRENDERBASE2(MenuTextThemeRender, XML_RENDER_TYPE_THEME, TEXTRENDER_TYPE_THEME_MENU, CONTROL_MENU, -1);

    UI_BEGIN_MSG_MAP_Ixxx(MenuTextThemeRender)
        UIMSG_WM_TEXTRENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
        UIMSG_WM_GETRENDERFONT(OnGetFont)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeTextRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_MENU; }
    void  DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct);
    void  SetAttribute(SetAttrPrefixData* pData);
    IRenderFont*  OnGetFont() { return m_pRenderFont; }

    void  SetRenderFont(IRenderFont* pRenderFont);
private:
    IRenderFont*  m_pRenderFont;
};


class ListViewThemeTextRender : public IThemeTextRenderBase
{
public:
    ListViewThemeTextRender();
    ~ListViewThemeTextRender();

    UI_DECLARE_TEXTRENDERBASE2(ListViewThemeTextRender, XML_RENDER_TYPE_THEME, TEXTRENDER_TYPE_THEME_LISTVIEW, -1, -1);

    UI_BEGIN_MSG_MAP_Ixxx(ListViewThemeTextRender)
        UIMSG_WM_TEXTRENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
        UIMSG_WM_GETRENDERFONT(OnGetFont)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeTextRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_LISTVIEW; }

protected:
    void  SetAttribute(SetAttrPrefixData* pData);
    void  DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct);
    IRenderFont*  OnGetFont() { return m_pRenderFont; }
    void  SetRenderFont(IRenderFont* pRenderFont);

protected:
    IRenderFont*  m_pRenderFont;
    bool  m_bUseExplorerTheme;  // 是否使用explorer样式主题
};

class TabCtrlButtonBkThemeRender : public IThemeRenderBase
{
public:
    TabCtrlButtonBkThemeRender();

    UI_DECLARE_RENDERBASE2(TabCtrlButtonBkThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_TABCTRL_BUTTON_BKGND,-1,-1);

    UI_BEGIN_MSG_MAP_Ixxx(TabCtrlButtonBkThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void*  QueryInterface(const IID* pIID);
    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_TAB; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  SetDirection(long n);

    int   GetStateId(int nState);
    long  m_nDir;  // 1表示DownEdge
};


// 列表控件前景
class ListViewCtrlForeThemeRender : public IThemeRenderBase
{
public:
    UI_DECLARE_RENDERBASE2(ListViewCtrlForeThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_LISTVIEW_ITEM,-1,-1);

    UI_BEGIN_MSG_MAP_Ixxx(ListViewCtrlForeThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = VSCLASS_LISTVIEW; }
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
};

class TreeViewCtrlExpandCollapseIconThemeRender : public IThemeRenderBase
{
public:
    TreeViewCtrlExpandCollapseIconThemeRender();

    UI_DECLARE_RENDERBASE2(TreeViewCtrlExpandCollapseIconThemeRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_TREEVIEW_EXPANDCOLLAPSE_ICON,-1,-1);

    UI_BEGIN_MSG_MAP_Ixxx(TreeViewCtrlExpandCollapseIconThemeRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    void  OnGetThemeName(TCHAR** ppszName);
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
    void  GetDesiredSize(SIZE* pSize);

protected:
    void  DrawNoTheme(IRenderTarget* pRenderTarget, const CRect* prc, int nState);
    void  DrawTheme(IRenderTarget* pRenderTarget, const CRect* prc, int nState);

private:
    bool  m_bUseExplorerTheme;  // 是否使用explorer样式主题
};

#if 0


	class ScrollBarSizeBoxThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_SCROLLBAR; }
		virtual void  DrawState(IRenderTarget*, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize();
	};

	//////////////////////////////////////////////////////////////////////////

	class TooltipBkgndThemeRender : public ThemeRenderBase
	{
	public:
		virtual const TCHAR* GetThemeName() { return VSCLASS_TOOLTIP; }
		virtual void  DrawState(IRenderTarget*, const CRect* prc, int nState);
		virtual SIZE  GetDesiredSize();
	};

	//////////////////////////////////////////////////////////////////////////

	// 	class ListViewCtrlBkgndThemeRender : public ThemeRenderBase
	// 	{
	// 		virtual const TCHAR* GetThemeName() { return VSCLASS_LISTVIEW; }
	// 		virtual void  DrawState(IRenderTarget* pRenderTarget, const CRect* prc, int nState);
	// 	};

	
#endif
}