#include "stdafx.h"

#include "UISDK\Control\Src\Control\Button\button.h"
#include "UISDK\Control\Src\Control\RadioButton\radiobutton.h"
#include "UISDK\Control\Src\Renderbase\themerender.h"
#include "UISDK\Control\Src\Renderbase\focusrender.h"
#include "UISDK\Control\Src\Control\CompoundButton\compoundbutton.h"
#include "UISDK\Control\Src\Control\RadioButton\radiobutton.h"
#include "UISDK\Control\Src\Control\CheckButton\checkbutton.h"
#include "UISDK\Control\Src\Control\Edit\edit.h"
#include "UISDK\Control\Src\Control\Groupbox\groupbox.h"
#include "UISDK\Control\Src\Control\ProgressCtrl\progressctrl.h"
#include "UISDK\Control\Src\Control\SliderCtrl\sliderctrl.h"
#include "UISDK\Control\Src\Control\Label\label.h"
#include "UISDK\Control\Src\Control\LedCtrl\ledctrl.h"
#include "UISDK\Control\Src\Control\HeaderCtrl\headerlistctrl.h"
#include "UISDK\Control\Src\Control\FlashWrap\flashwrap.h"
#include "UISDK\Control\Src\Control\IE\iewrap.h"
#include "UISDK\Control\Src\Control\TrayIcon\trayicon.h"
#include "UISDK\Control\Src\Control\ListBox\listbox.h"
#include "UISDK\Control\Src\Control\ComboBox\combobox.h"
#include "UISDK\Control\Src\Control\Menu\menu.h"
#include "UISDK\Control\Src\Control\RichEdit\richedit.h"
#include "UISDK\Control\Src\Control\HyperLink\hyperlink.h"
#include "UISDK\Control\Src\Control\TabCtrl\tabctrl.h"
#include "UISDK\Control\Src\Control\TabCtrl\tabctrlbutton.h"
#include "UISDK\Control\Src\Control\ListView\listview.h"
#include "UISDK\Control\Src\Control\TreeView\treeview.h"
#include "UISDK\Control\Src\Control\ScrollBar\systemscrollbar.h"
#include "UISDK\Control\Src\Control\ListTreeView\listtreeview.h"
#include "UISDK\Control\Src\Control\PropertyCtrl\propertyctrl.h"
#include "UISDK\Control\Src\Control\SplitterBar\splitterbar.h"

namespace UI
{
    extern "C"
    __declspec(dllexport) bool RegisterUIObject(IUIApplication* p)
    {
        REGISTER_UI_OBJECT(p, Button);
        REGISTER_UI_OBJECT(p, CompoundButton);
        REGISTER_UI_OBJECT(p, RadioButton);
        REGISTER_UI_OBJECT(p, CheckButton);
        REGISTER_UI_OBJECT(p, HyperLink);
        REGISTER_UI_OBJECT(p, Edit);
        REGISTER_UI_OBJECT(p, GroupBox);
        REGISTER_UI_OBJECT(p, ProgressCtrl);
        REGISTER_UI_OBJECT(p, SliderCtrl);
        REGISTER_UI_OBJECT(p, Label);
        REGISTER_UI_OBJECT(p, PictureCtrl);
        REGISTER_UI_OBJECT(p, LEDCtrl);
        REGISTER_UI_OBJECT(p, FlashWrap);
        REGISTER_UI_OBJECT(p, IEWrap);
        REGISTER_UI_OBJECT(p, ListBox);
        REGISTER_UI_OBJECT(p, ComboBox);
        REGISTER_UI_OBJECT(p, RichEdit);
        REGISTER_UI_OBJECT(p, HyperLink);
        REGISTER_UI_OBJECT(p, GifCtrl);
        REGISTER_UI_OBJECT(p, ListView);
        REGISTER_UI_OBJECT(p, HeaderListCtrl);
        REGISTER_UI_OBJECT(p, TreeView);
        REGISTER_UI_OBJECT(p, ListTreeView);
        REGISTER_UI_OBJECT(p, PropertyCtrl);
        REGISTER_UI_OBJECT(p, SystemHScrollBar);
        REGISTER_UI_OBJECT(p, SystemVScrollBar);
        REGISTER_UI_OBJECT(p, SplitterBar);

        p->RegisterLayoutTagParseFunc(_T("Menu"), Menu::UIParseLayoutElement);  // REGISTER_UI_OBJECT(p, Menu);
        p->RegisterLayoutTagParseFunc(_T("TabCtrl"), TabCtrl::UIParseLayoutElement);  // REGISTER_UI_OBJECT(p, TabCtrl);
        REGISTER_UI_OBJECT(p, TabCtrlButton);

        REGISTER_UI_RENDERBASE(p, ButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, RadioButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, CheckButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, EditBkThemeRender);
        REGISTER_UI_RENDERBASE(p, GroupBoxBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ProgressCtrlBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, ProgressCtrlForegndThemeRender);
        REGISTER_UI_RENDERBASE(p, SliderCtrlBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, SliderTrackButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, HeaderCtrlItemBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, HeaderCtrlRightBlankThemeRender);
        REGISTER_UI_RENDERBASE(p, HScrollBarBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, VScrollBarBkgndThemeRender);
        REGISTER_UI_RENDERBASE(p, ScrollLineUpButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ScrollLineDownButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ScrollLineLeftButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ScrollLineRightButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, HScrollThumbButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, VScrollThumbButtonThemeRender);
        REGISTER_UI_RENDERBASE(p, ListboxBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ComboboxBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ComboboxButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuBkThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuSeperatorThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuStringItemRender);
        REGISTER_UI_RENDERBASE(p, MenuPopupTriangleRender);
        REGISTER_UI_RENDERBASE(p, MenuCheckedIconThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuRadioIconThemeRender);
        REGISTER_UI_RENDERBASE(p, MenuRadioCheckIconBkThemeRender);
        REGISTER_UI_RENDERBASE(p, TabCtrlButtonBkThemeRender);
        REGISTER_UI_RENDERBASE(p, ListViewCtrlForeThemeRender);
        REGISTER_UI_RENDERBASE(p, TreeViewCtrlExpandCollapseIconThemeRender);
        REGISTER_UI_RENDERBASE(p, ListViewFocusRender);
        
        REGISTER_UI_TEXTRENDERBASE(p, MenuTextThemeRender);
        REGISTER_UI_TEXTRENDERBASE(p, ListViewThemeTextRender);

        return true;
    }

    // 可以直接由IM_UI工程调用的导出接口
    bool UI_Ctrl_RegisterUIObject(IUIApplication* p)
    {
        return RegisterUIObject(p);
    }


    HRESULT UICreateTrayIcon(IUIApplication* pUIApplication, ITrayIcon** ppOut)
    {
        if (NULL == ppOut)
            return E_INVALIDARG;

        TrayIcon* p = new TrayIcon(pUIApplication);
        *ppOut = static_cast<ITrayIcon*>(p);
        p->AddRef();

        return S_OK;
    }

    bool  UILoadMenu(IUIApplication* pUIApplication, const TCHAR* szMenuId, IMenu** ppMenu)
    {
        return Menu::LoadMenu(pUIApplication, szMenuId, ppMenu);
    }
}