#pragma  once

namespace UI
{
class ListViewFocusRender : public IThemeRenderBase
{
public:
	ListViewFocusRender();
	
    UI_DECLARE_RENDERBASE2(ListViewFocusRender, XML_RENDER_TYPE_THEME, RENDER_TYPE_THEME_LISTVIEW_FOCUS, CONTROL_LISTVIEWCTRL, -1);

    UI_BEGIN_MSG_MAP_Ixxx(ListViewFocusRender)
        UIMSG_WM_RENDERBASE_DRAWSTATE(OnDrawState)
        UIMSG_WM_RENDERBASE_GETTHEMENAME(OnGetThemeName)
    UI_END_MSG_MAP_CHAIN_PARENT(IThemeRenderBase)

    // PS: VSCLASS_LISTVIEW��û�л���FOCUS NOT SELECTED��state id������������˸�������ʵ�֣�
    // http://www.microsoftfaqs.com/Articles/16601610/How_to_draw_ListView_themed_focus_rectangle
    void  OnGetThemeName(TCHAR** ppszName) { *ppszName = _T("ItemsView"); }  // û���ĵ���¼��theme
    void  OnDrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

private:
	int  m_nDeflat;
};
}