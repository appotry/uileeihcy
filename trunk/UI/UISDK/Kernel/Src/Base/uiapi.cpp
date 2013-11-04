#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\uiapi.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\UIObject\Control\control.h"
#include "UISDK\Kernel\Src\Base\Applicatoin\uiapplication.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdi\gdifont.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdiplus\gdiplusfont.h"
#include "UISDK\Kernel\Src\ATL\image.h"
#include "UISDK\Kernel\Src\SkinParse\msxmlwrap\msxmlwrap.h"
#include "UISDK\Kernel\Src\Util\Buffer\BufferT.h"

// 内部全局方法
namespace UI
{
 
//
//	[global] void  UI_LOG_***** ( String strContent )
//
//	Parameters
//		strContent
//			[in] 要输出到日志中的内容
//
//	Return
//		NA
//
//	Remark
//		该函数直接调用LogManager的Log方法。
//		在任何想输出*****级别的UI日志的地方调用该函数即可
//		[注意]:可变参数中的 %s，不支持用String赋值，请使用 String::c_str()形式
    //
    //	See Also
    //		LogManager::Debug
    //

#define UI_LOG_LEVEL(l,c)     LOG_LEVEL(UIApplication::s_pLog, UIApplication::s_lUiLogCookie,l,c)
//#define MOUSE_LOG_LEVEL(l,c)  if (NULL == g_pUIApplication) return ; LOG_LEVEL( g_pUIApplication->m_pLog, _T("mouse"),l,c )

void UI_LOG_DEBUG( TCHAR* szContent, ... )
{
    UI_LOG_LEVEL(LOG_LEVEL_DEBUG, szContent );
}
void UI_LOG_INFO( TCHAR* szContent, ... )
{
    UI_LOG_LEVEL(LOG_LEVEL_INFO, szContent );
}
void UI_LOG_WARN( TCHAR* szContent, ... )
{
    UI_LOG_LEVEL(LOG_LEVEL_WARN, szContent );
}
void UI_LOG_ERROR( TCHAR* szContent, ... )
{
    UI_LOG_LEVEL(LOG_LEVEL_ERROR, szContent );
}
void UI_LOG_FATAL( TCHAR* szContent, ...)
{
    UI_LOG_LEVEL(LOG_LEVEL_FATAL, szContent );
}
// void MOUSE_LOG_DEBUG( TCHAR* szContent, ...)
// {
// 	MOUSE_LOG_LEVEL(LOG_LEVEL_DEBUG,szContent);
// }


HRESULT CreateUIApplicationInstance(IUIApplication** pp)
{
    if (NULL == pp)
        return E_INVALIDARG;

    IUIApplication* p = new IUnknownImpl<IUIApplication>;
    p->AddRef();

    *pp = p;
    return S_OK;
}

void UI_AttachFont(IRenderFont** pOut, HFONT hFont, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType )
{
    if( NULL == hFont || NULL == pOut)
        return;

    switch (eRenderType)
    {
    case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
        {
            GDIRenderFont::CreateInstance(pOut);
            IRenderFont* pRenderFont = (IRenderFont*)*pOut;
            pRenderFont->Attach(hFont);
        }
        break;

    case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
        {
            GdiplusRenderFont::CreateInstance(pOut);
            IRenderFont* pRenderFont = (IRenderFont*)*pOut;
            pRenderFont->Attach(hFont);
        }
        break;

#ifdef UI_D2D_RENDER
    case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
        {
            Direct2DRenderFont::CreateInstance(pOut);
            IRenderFont* pRenderFont = (IRenderFont*)*pOut;
            pRenderFont->Attach(hFont);
        }
        break;
#endif

    default: 
        return ;
    }

    return ;
}

void  UI_ExtendPath(String& strPath)
{
    if (FALSE == Util::IsFullPath(strPath.c_str()))
    {
        TCHAR szModulePath[MAX_PATH] = _T("");

        GetModuleFileName(/*UIApplication::GetModuleInstance()*/g_hInstance, szModulePath, MAX_PATH);
        TCHAR* szTemp = _tcsrchr(szModulePath, _T('\\'));
        if (szTemp)
            *(szTemp+1) = 0;

        String strTemp = szModulePath;
        strPath = strTemp;
        strPath.append(strPath);
    }
}


//
// 字符串拆分
//
//	Remark:
//		在向DLL之间使用stl作为参数进行传递，会导致内存释放时的崩溃，因此这里没有直接去调用UI_Split方法
//		而是直接在ULDLL中重写这么一个函数
//
void  UI_Split(const String& str, TCHAR szSep, vector<String>& vRet)
{
    int nIndex = 0;

    while (true)
    {
        int nResult = (int)str.find( szSep, nIndex );
        if (-1 == nResult)
        {
            vRet.push_back( str.substr( nIndex, str.length()-nIndex ) );
            break;
        }
        else
        {
            vRet.push_back( str.substr( nIndex, nResult-nIndex ) );
            nIndex = ++nResult;
        }
    }
}

#if 0
//
//	从工厂方法中创建出一个render
//
//	parameter
//		strType
//			[in]	render类型 
//		
//		pObj
//			[in]	窗口指针
//
//	return
//		失败返回NULL，失败时可参见日志
//
//
HRESULT CreateRenderBaseByName(const TCHAR* szType, IObject* pObj, IRenderBase** ppOut)
{
    if (NULL == pObj || NULL == ppOut)
        return E_INVALIDARG;

    RENDER_TYPE eType = RENDER_TYPE_NULL;

    if (XML_RENDER_TYPE_THEME == strType || XML_RENDER_TYPE_NOTHEME == strType)
    {
        OBJ_TYPE objCtrlType = pObj->GetObjectExtentType();
#pragma  region // Button Theme render
        if (objCtrlType == CONTROL_BUTTON)
        {
            ButtonBase* pButton = dynamic_cast<ButtonBase*>(pObj);
            if (NULL == pButton)
                return NULL;

            int nButtonStyle = pButton->GetButtonStyle();
            switch(nButtonStyle)
            {
            case BUTTON_STYLE_COMBOBOX:
                eType = RENDER_TYPE_THEME_COMBOBOX_BUTTON;
                break;

            case BUTTON_STYLE_SCROLLLINEUP:
                eType = RENDER_TYPE_THEME_SCROLLBAR_LINE_UP_BUTTON;
                break;

            case BUTTON_STYLE_SCROLLLINEDOWN:
                eType = RENDER_TYPE_THEME_SCROLLBAR_LINE_DOWN_BUTTON;
                break;

            case BUTTON_STYLE_SCROLLLINELEFT:
                eType = RENDER_TYPE_THEME_SCROLLBAR_LINE_LEFT_BUTTON;
                break;

            case BUTTON_STYLE_SCROLLLINERIGHT:
                eType = RENDER_TYPE_THEME_SCROLLBAR_LINE_RIGHT_BUTTON;
                break;

            case BUTTON_STYLE_VSCROLLTHUMB:
                eType = RENDER_TYPE_THEME_VSCROLLBAR_THUMB_BUTTON;
                break;

            case BUTTON_STYLE_HSCROLLTHUMB:
                eType = RENDER_TYPE_THEME_HSCROLLBAR_THUMB_BUTTON;
                break;

            case BUTOTN_STYLE_SLIDERTRACKBUTTON:
                eType = RENDER_TYPE_THEME_SLIDER_TRACK_BUTTON;
                break;

            default:
                eType = RENDER_TYPE_THEME_BUTTON_BKGND;
                break;
            }
        }
#pragma  endregion
        else if (objCtrlType == CONTROL_RADIOBUTTON)
        {
            eType = RENDER_TYPE_THEME_RADIO_BUTTON;
        }
        else if (objCtrlType == CONTROL_CHECKBUTTON)
        {
            eType = RENDER_TYPE_THEME_CHECK_BUTTON;
        }
        else if (objCtrlType == CONTROL_EDIT)
        {
            eType = RENDER_TYPE_THEME_EDIT_BKGND;
        }
        else if (objCtrlType == CONTROL_GROUPBOX)
        {
            eType = RENDER_TYPE_THEME_GROUPBOX_BKGND;
        }
        else if (objCtrlType == CONTROL_COMBOBOX)
        {
            eType = RENDER_TYPE_THEME_COMBOBOX_BKGND;
        }
        else if (objCtrlType == CONTROL_LISTBOX)
        {	
            eType = RENDER_TYPE_THEME_LISTBOX_BKGND;

            ListBox* pListBox = dynamic_cast<ListBox*>(pObj);
            if (pListBox)
            {
                if (LISTBOX_STYLE_COMBOBOX == pListBox->GetListBoxStyle())
                {
                    eType = RENDER_TYPE_THEME_COMBOBOX_LISTBOX_BKGND;
                }
            }
        }
        else if (objCtrlType == CONTROL_MENU)
        {
            eType = RENDER_TYPE_THEME_MENU_BKGND;
        }
        else if (objCtrlType == CONTROL_TOOLTIPWINDOW)
        {
            eType = RENDER_TYPE_THEME_TOOLTIP_WINDOW_BKGND;
        }
    }
    // 	else if (XML_RENDER_TYPE_NOTHEME == strType)
    // 		eType = RENDER_TYPE_NOTHEME;

    else if (XML_RENDER_TYPE_IMAGE == strType)
        eType = RENDER_TYPE_IMAGE;

    else if (XML_RENDER_TYPE_IMAGELIST == strType)
        eType = RENDER_TYPE_IMAGELIST;

    else if (XML_RENDER_TYPE_COLOR == strType)
        eType = RENDER_TYPE_COLOR;

    else if (XML_RENDER_TYPE_IMAGELISTITEM == strType)
        eType = RENDER_TYPE_IMAGELISTITEM;

    else if (XML_RENDER_TYPE_COLORLIST == strType)
        eType = RENDER_TYPE_COLORLIST;

    else if (XML_RENDER_TYPE_GRADIENT_H == strType)
        eType = RENDER_TYPE_GRADIENTH;

    else if (XML_RENDER_TYPE_GRADIENT_V == strType)
        eType = RENDER_TYPE_GRADIENTV;

    else if (XML_RENDER_TYPE_NULL == strType)
        eType = RENDER_TYPE_NULL;

    else
        return NULL;

    RenderBase* pRender = GetRender(eType, pObj);
    if (pRender && XML_RENDER_TYPE_NOTHEME == strType)  // 强制使用非主题新式
    {
        ThemeRenderBase* pThemeRenderBase = dynamic_cast<ThemeRenderBase*>(pRender);
        if (pThemeRenderBase)
        {
            pThemeRenderBase->SetNoTheme();
        }
    }

    return pRender;
}

HRESULT CreateRenderBase(RENDER_TYPE eType, IObject* pObj, IRenderBase** ppOut)
{
    if (NULL == ppOut || NULL == pObj)
        return E_INVALIDARG;

    IRenderBase*  pRender = NULL;
    if (RENDER_TYPE_THEME_COMBOBOX_BUTTON == eType)
    {
        pRender = new ComboboxButtonBkThemeRender();
    }
    else if (RENDER_TYPE_THEME_SCROLLBAR_LINE_UP_BUTTON == eType)
    {
        pRender = new ScrollLineUpButtonBkThemeRender();
    }
    else if (RENDER_TYPE_THEME_SCROLLBAR_LINE_DOWN_BUTTON == eType)
    {
        pRender = new ScrollLineDownButtonBkThemeRender();
    }
    else if (RENDER_TYPE_THEME_SCROLLBAR_LINE_LEFT_BUTTON == eType)
    {
        pRender = new ScrollLineLeftButtonBkThemeRender();
    }
    else if (RENDER_TYPE_THEME_SCROLLBAR_LINE_RIGHT_BUTTON == eType)
    {
        pRender = new ScrollLineRightButtonBkThemeRender();
    }
    else if (RENDER_TYPE_THEME_VSCROLLBAR_THUMB_BUTTON == eType)
    {
        pRender = new VScrollThumbButtonThemeRender();
    }
    else if (RENDER_TYPE_THEME_HSCROLLBAR_THUMB_BUTTON == eType)
    {
        pRender = new HScrollThumbButtonThemeRender();
    }
    else if (RENDER_TYPE_THEME_SLIDER_TRACK_BUTTON == eType)
    {
        pRender = new SliderTrackButtonThemeRender();
    }
    else if (RENDER_TYPE_THEME_BUTTON_BKGND == eType)
    {
        pRender = new ButtonBkThemeRender();
    }
    else if (RENDER_TYPE_THEME_RADIO_BUTTON == eType)
    {
        pRender = new RadioButtonThemeRender();
    }
    else if (RENDER_TYPE_THEME_CHECK_BUTTON == eType)
    {
        pRender = new CheckButtonThemeRender();
    }
    else if (RENDER_TYPE_THEME_EDIT_BKGND == eType)
    {
        pRender = new EditBkThemeRender();
        CRegion4 r(2,2,2,2);
        pObj->SetBorderRegion(&r);
    }
    else if (RENDER_TYPE_THEME_GROUPBOX_BKGND == eType)
    {
        pRender = new GroupBoxBkThemeRender();
    }
    else if (RENDER_TYPE_THEME_COMBOBOX_BKGND == eType)
    {
        pRender = new ComboboxBkThemeRender();
    }
    else if (RENDER_TYPE_THEME_COMBOBOX_LISTBOX_BKGND == eType)
    {
        ColorRender* pTemp = new ColorRender();
        pTemp->SetBkColor(RGB(255,255,255));
        pTemp->SetBorderColor(RGB(0,0,0));
        pRender = static_cast<RenderBase*>(pTemp);

        int nBorder = 1;
        CRegion4 r(nBorder, nBorder, nBorder, nBorder);
        pObj->SetBorderRegion(&r);
    }
    else if (RENDER_TYPE_THEME_LISTBOX_BKGND == eType)
    {
        pRender = new ListboxBkThemeRender();
        CRegion4 r(2,2,2,2);
        pObj->SetBorderRegion(&r);
    }
    else if (RENDER_TYPE_THEME_LISTVIEW_ITEM == eType)
    {
        pRender = new ListViewCtrlForeThemeRender();
    }
    else if (RENDER_TYPE_THEME_MENU_BKGND == eType)
    {
        pRender = new MenuBkThemeRender();
        CRegion4 r(2,2,2,2);
        pObj->SetBorderRegion(&r);
    }
    else if (RENDER_TYPE_THEME_TOOLTIP_WINDOW_BKGND == eType)
    {
        pRender = new TooltipBkgndThemeRender;
    }
    else if (RENDER_TYPE_IMAGE == eType)
    {
        pRender = new ImageRender();
    }
    else if( RENDER_TYPE_COLORLIST == eType )
    {
        pRender = new ColorListRender();
    }
    else if( RENDER_TYPE_IMAGELIST == eType )
    {
        pRender = new ImageListRender();
    }
    else if (RENDER_TYPE_IMAGELISTITEM == eType)
    {
        pRender = new ImageListItemRender();
    }
    else if (RENDER_TYPE_COLOR == eType)
    {
        pRender = new ColorRender();
    }
    else if (RENDER_TYPE_THEME_SYSCOLOR == eType)
    {
        pRender = new SysColorRender();
    }
    else if (RENDER_TYPE_GRADIENTH == eType)
    {
        pRender = new GradientRender();
    }
    else if (RENDER_TYPE_GRADIENTV == eType)
    {
        pRender = new GradientRender();
    }
    else if (RENDER_TYPE_THEME_MENUSTRINGITEM == eType)
    {
        pRender = new MenuStringItemRender();

        // 注：由于不同的主题下面，字体render也不一样。为了解决在使用主题样式下面字体颜色的问题，
        //     当菜单使用了主题前景时，自动修改它的字体颜色样式
        TextRenderBase* pTextRender = TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_COLORLIST, pObj);
        if (pTextRender)
        {
            ColorListTextRender* p = dynamic_cast<ColorListTextRender*>(pTextRender);

            IRenderFont* pRenderFont = pObj->GetRenderFont();
            p->SetRenderFont(pRenderFont);
            p->SetCount(4);
            p->SetColor(0, RGB(0,0,0));
            p->SetColor(1, RGB(255,255,255));
            p->SetColor(2, RGB(0,0,0));
            p->SetColor(3, RGB(192,192,192));

            OSVERSIONINFOEX os;
            ::ZeroMemory(&os,sizeof(os));
            os.dwOSVersionInfoSize=sizeof(os);
            ::GetVersionEx(reinterpret_cast<LPOSVERSIONINFO>(&os));


            // 在WIN7开启主题的情况下，将菜单项的hover颜色改成黑色
            IUIApplication* pUIApp = NULL;
            if (pObj)
                pUIApp = pObj->GetUIApplication();

            if(NULL != pUIApp && pUIApp->IsVistaOrWin7etc())  // visita or higher
            {
                //HTHEME h = OpenThemeData(NULL, _T("MENU"));
                if (IsThemeActive())
                {
                    p->SetColor(1, RGB(0,0,0));
                    //	CloseThemeData(h);
                }
            }
            pObj->SetTextRender(p);
            SAFE_RELEASE(p);
        }

    }
    else if (RENDER_TYPE_THEME_MENUSEPERATOR == eType)
    {
        pRender = new MenuSeperatorThemeRender();
    }
    else if (RENDER_TYPE_THEME_MENUPOPUPTRIANGLE == eType)
    {
        pRender = new MenuPopupTriangleRender();
    }
    else if (RENDER_TYPE_THEME_MENUCHECKICON == eType)
    {
        pRender = new MenuCheckedIconThemeRender();
    }
    else if (RENDER_TYPE_THEME_MENURADIOICON == eType)
    {
        pRender = new MenuRadioIconThemeRender();
    }
    else if (RENDER_TYPE_THEME_MENURADIOCHECKICONBK == eType)
    {
        pRender = new MenuRadioCheckIconBkThemeRender();
    }
    else if (RENDER_TYPE_THEME_VSCROLLBARBACKGND == eType)
    {
        pRender = new VScrollBarBkgndThemeRender();
    }
    else if (RENDER_TYPE_THEME_HSCROLLBARBACKGND == eType)
    {
        pRender = new HScrollBarBkgndThemeRender();
    }
    else if (RENDER_TYPE_THEME_SCROLLBAR_SIZEBOX == eType)
    {
        pRender = new ScrollBarSizeBoxThemeRender();
    }
    else if (RENDER_TYPE_THEME_PROGRESS_FOREGND == eType)
    {
        pRender = new ProgressCtrlForegndThemeRender();
    }
    else if (RENDER_TYPE_THEME_PROGRESS_BKGND == eType)
    {
        pRender = new ProgressCtrlBkgndThemeRender();
    }
    else if (RENDER_TYPE_THEME_TRACKBAR_BKGND == eType)
    {
        pRender = new SliderCtrlBkgndThemeRender();
    }
    else if (HEADER_TYPE_THEME_HEADERCTRL_ITEM_BKGND == eType)
    {
        pRender = new HeaderCtrlItemBkgndThemeRender();
    }
    else if (RENDER_TYPE_THEME_HEADERCTRL_RIGHT_BLANK == eType)
    {
        pRender = new HeaderCtrlRightBlankThemeRender();
    }
    else if (RENDER_TYPE_THEME_TREEVIEW_EXPANDCOLLAPSE_ICON == eType)
    {
        pRender = new TreeViewCtrlExpandCollapseIconThemeRender();
    }
    else if (RENDER_TYPE_THEME_LISTVIEW_FOCUS == eType)
    {
        pRender = new ListViewFocusRender();
    }
    else if (RENDER_TYPE_NULL == eType)
    {
        pRender = new NullRenderBase();
    }
    else
    {
        UI_LOG_WARN(_T("%s invalid render type %d"), FUNC_NAME,  eType );
    }

    if( NULL != pRender )
    {
        pRender->SetRenderType(eType);
        pRender->SetObject(pObj);
        pRender->Init();
        pRender->AddRef();
    }
    return S_OK;
}

HRESULT CreateTextRenderBaseByName(const TCHAR* szType, IObject* pObj, ITextRenderBase** ppOut)
{
    if (NULL == szType || NULL == ppOut)
        return E_INVALIDARG;

    if (XML_TEXTRENDER_TYPE_NORMAL == strType)
    {
        return GetTextRender(TEXTRENDER_TYPE_NORMAL, pObj);
    }
    else if (XML_TEXTRENDER_TYPE_FONTCOLORLIST == strType)
    {
        return GetTextRender(TEXTRENDER_TYPE_FONTCOLORLIST, pObj);
    }
    else if (XML_TEXTRENDER_TYPE_COLORLIST == strType)
    {
        return GetTextRender(TEXTRENDER_TYPE_COLORLIST, pObj);
    }
    else if (XML_TEXTRENDER_TYPE_FONTLIST == strType)
    {
        return NULL;  // 未实现。直接使用XML_TEXTRENDER_TYPE_FONTCOLORLIST代替吧
    }
    else
    {
        UI_LOG_WARN(_T("%s invalid render type %s"), FUNC_NAME, szType);
    }
    return NULL;
}


HRESULT CreateTextRenderBase(TEXTRENDER_TYPE eType, IObject* pObj, ITextRenderBase** pp)
{
    TextRenderBase*  pTextRender = NULL;

    if (TEXTRENDER_TYPE_NORMAL == eType)
    {
        pTextRender = new TextRender();
    }
    else if (TEXTRENDER_TYPE_FONTCOLORLIST == eType)
    {
        pTextRender = new FontColorListTextRender();
    }
    else if (TEXTRENDER_TYPE_COLORLIST == eType)
    {
        pTextRender = new ColorListTextRender();
    }
    else if (TEXTRENDER_TYPE_THEME_LISTVIEW == eType)
    {
        pTextRender = new ListViewThemeTextRender();
    }
    else
    {
        UI_LOG_WARN(_T("%s invalid render type %d"), _T(__FUNCTION__),  eType );
    }

    if( NULL != pTextRender )
    {
        pTextRender->SetTextRenderType(eType);
        pTextRender->SetObject(pObj);
        pTextRender->Init();
    }
    pTextRender->AddRef();
    return pTextRender;
}
#endif


HBITMAP CreateMemBitmap(int nWidth, int nHeight, int* pnPitch, byte** ppBits)
{
    Image image;
    image.Create(nWidth, nHeight, 32, Image::createAlphaChannel);

    if (ppBits)
        *ppBits = (BYTE*)image.GetBits();

    if (pnPitch)
        *pnPitch = image.GetPitch();
    
    return image.Detach();
}

void  UILoadXmlDocument(const TCHAR* szXmlPath, IUIDocument** ppDoc)
{
    if (NULL == szXmlPath || NULL == ppDoc)
        return;

    if (!PathFileExists(szXmlPath))
        return;

    UIMarkup* p = UIMarkup::New();
    if (!p->LoadFile(szXmlPath))
    {
        SAFE_RELEASE(p);
        return;
    }

    *ppDoc = static_cast<IUIDocument*>(p);
}

void  CreateTCHARBuffer(IBuffer** pBuffer)
{
    if (!pBuffer)
        return;

    CBuffer* p = new CBuffer;
    *pBuffer = p;
}

// #include "UISDK\Kernel\Src\UIEffect\高斯模糊\gaussblur.h"
// void  RecursiveBlur(HBITMAP hBitmap, double radius, RECT* prc, int nFlag)
// {
//     recursive_blur blur;
//     blur.blur(hBitmap, radius, prc, nFlag);
// }

} // namespace
