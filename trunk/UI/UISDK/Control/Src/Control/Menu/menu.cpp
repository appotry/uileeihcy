#include "stdafx.h"
#include "menu.h"
#include "UISDK\Control\Src\Control\ListBase\PopupControlWindow\popupcontrolwindow.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"
#include "UISDK\Control\Src\Control\Menu\layout\menulayout.h"

namespace UI
{

Menu::Menu()
{
    m_pIMenu = NULL;
    m_bLayered = false;
    m_bPopupAtParentRight = true;
    m_nTrackPopupMenuFlag = 0;
    m_nRetCmd = 0;
    m_pPopupWrapWnd = NULL;
    m_pNextMenu = NULL;
    m_pPrevMenu = NULL;
    m_nTimerIDShowPopupSubMenu = 0;
    m_nTimerIDHidePopupSubMenu = 0;

    m_nIconGutterWidth = 28;
    m_nPopupTriangleWidth = 20;
    m_nSubMenuGap = -2;
}

Menu::~Menu()
{
    _DestroyPopupWindow();
}

void  Menu::_DestroyAllSubMenu()
{
    IListItemBase* pItem = m_pIMenu->GetFirstItem();
    while (pItem)
    {
        if (!(pItem->GetItemFlag() & MF_POPUP))
        {
            pItem = pItem->GetNextItem();
            continue;
        }
        
        IMenu* pSubMenu = (IMenu*)pItem->GetCtrlData();
        if (!pSubMenu)
        {
            pItem = pItem->GetNextItem();
            continue;
        }

        if (!pSubMenu->TestStyleEx(MENU_STYLE_AUTO_DELETE_SUBMENU))
        {
            pItem = pItem->GetNextItem();
            continue;
        }


        SAFE_DELETE_Ixxx(pSubMenu);
        pItem = pItem->GetNextItem();
    }
}

// �������м���һ���˵���
bool  Menu::LoadMenu(IUIApplication* pUIApplication, const TCHAR* szMenuId, IMenu** ppMenu)
{
    if (NULL == szMenuId || NULL == ppMenu || NULL == pUIApplication)
        return false;

    ILayoutManager*  pLayoutMgr = pUIApplication->GetActiveSkinLayoutMgr();
    if (NULL == pLayoutMgr)
        return false;

    IObject* pObject = NULL;
    if (FAILED(pLayoutMgr->LoadRootLayout(Menu::GetXmlName(), szMenuId, &pObject)) || NULL == pObject)
        return false;

    *ppMenu = (IMenu*)pObject->QueryInterface(uiiidof(IMenu));
    if (NULL == ppMenu)
        return false;

    return true;
}

HRESULT  Menu::UIParseLayoutElement(IUIElement* pElem, IUIApplication*  pUIApp, IObject* pParent, IObject** ppOut)
{
    IMenu*  pIMenu = NULL;
    IMenu::CreateInstance(pUIApp, &pIMenu);
    if (NULL == pIMenu)
        return E_FAIL;

    *ppOut = static_cast<IObject*>(pIMenu);

    // ��������
    IMapAttribute*  pMapAttrib = NULL;
    IStyleManager*  pStyleMgr = pUIApp->GetActiveSkinStyleMgr();

    pElem->GetAttribList(&pMapAttrib);
    pStyleMgr->ParseStyle(Menu::GetXmlName(), pMapAttrib);

	SERIALIZEDATA data = {0};
	data.pMapAttrib = pMapAttrib;
	data.nFlag = SERIALIZEFLAG_LOAD;
	UISendMessage(pIMenu, UI_WM_SERIALIZE, (WPARAM)&data);
//    UISendMessage(pIMenu, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)false);
    SAFE_RELEASE(pMapAttrib);

    // ��������Ԫ��
    IUIChildNodeEnum* pEnum = pElem->EnumChild();
    if (pEnum)
    {
        IUIElement* pChildElement = NULL;
        while (pChildElement = pEnum->NextElement())
        {
            pIMenu->OnNewChildElement(pChildElement);
            SAFE_RELEASE(pChildElement);
        }

        SAFE_RELEASE(pEnum);
    }
    return S_OK;
}
void  Menu::OnNewChildElement(IUIElement* pUIElement)
{
    CComBSTR  bstrTagName;
    pUIElement->GetTagName(&bstrTagName);

    if (0 == bstrTagName.Length())
    {
        UI_LOG_WARN( _T("%s, xml invalid tag name."), FUNC_NAME );
        return;
    }

    //	������������
    IMapAttribute*  pMapAttrib = NULL;
    pUIElement->GetAttribList(&pMapAttrib);

    // ���⴦����չstyle��Ҫ��<string>��չΪmenu.string..
    String strTagName, strStyleClass, strID;

    const TCHAR* szText = pMapAttrib->GetAttr(XML_ID, false); // id = Menu.id����Ϊ�����id��������ֵ
    if (szText)
    {
        strID = Menu::GetXmlName();
        strID.push_back(XML_CHILD_SEPARATOR);
        strID.append(szText);
    }

    szText = pMapAttrib->GetAttr(XML_STYLECLASS, true);
    if (szText)
        strStyleClass = szText;

    strTagName = Menu::GetXmlName();   // tagName = Menu.String / Menu.Popup / Menu.Separator
    strTagName.push_back(XML_CHILD_SEPARATOR);
    strTagName.append((BSTR)bstrTagName);

    IStyleManager*  pStyleMgr = m_pIMenu->GetUIApplication()->GetActiveSkinStyleMgr();
    pStyleMgr->LoadStyle((BSTR)bstrTagName, strStyleClass.c_str(), strID.c_str(), pMapAttrib);

    // ���ز˵�����
    IListItemBase* pItem = this->LoadMenuItem(pUIElement, (BSTR)bstrTagName, pMapAttrib);
    SAFE_RELEASE(pMapAttrib);
}


// 
//	ͨ��xml���Լ��ز˵���
//
IListItemBase* Menu::LoadMenuItem(IUIElement* pUIElement, const TCHAR* szTagName, IMapAttribute* pMapAttrib)
{
    int  nID = 0;
    pMapAttrib->GetAttr_int(XML_ID, true, &nID);

    if (0 == _tcscmp(szTagName, XML_MENU_STRINGITEM))
    {
        String strText;
        bool bDisable = false;

        const TCHAR* szText= pMapAttrib->GetAttr(XML_TEXT, true);
        if (szText)
            strText = szText;

        pMapAttrib->GetAttr_bool(XML_MENU_DISABLE, true, &bDisable);

        IListItemBase* pItem = this->AppendString(strText.c_str(), nID);
        if (pItem)
        {
            if (bDisable)
                pItem->SetDisable(true);

			SERIALIZEDATA data = {0};
			data.pMapAttrib = pMapAttrib;
			data.nFlag = SERIALIZEFLAG_LOAD;
			UISendMessage(pItem, UI_WM_SERIALIZE, (WPARAM)&data);
//            UISendMessage(pItem, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)false);
        }
        return pItem;
    }
    else if (0 == _tcscmp(szTagName, XML_MENU_SEPARATORITEM))
    {
        IListItemBase* pItem = this->AppendSeparator(nID);
        if (pItem)
        {
			SERIALIZEDATA data = {0};
			data.pMapAttrib = pMapAttrib;
			data.nFlag = SERIALIZEFLAG_LOAD;
			UISendMessage(pItem, UI_WM_SERIALIZE, (WPARAM)&data);
//            UISendMessage(pItem, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)false);
        }
        return pItem;
    }
    else if (0 == _tcscmp(szTagName, XML_MENU_POPUPITEM))
    {
        String strText;
        String strID;
        bool bDisable = false;

        const TCHAR* szText= pMapAttrib->GetAttr(XML_TEXT, true);
        if (szText)
            strText = szText;

        pMapAttrib->GetAttr_bool(XML_MENU_DISABLE, true, &bDisable);

        IObject* pSubMenuObj = NULL;
        Menu::UIParseLayoutElement(pUIElement, m_pIMenu->GetUIApplication(), NULL, &pSubMenuObj);
        if  (NULL == pSubMenuObj)
            return NULL;

        IMenu* pSubMenu = (IMenu*)pSubMenuObj->QueryInterface(uiiidof(IMenu));
        if (NULL == pSubMenu)
        {
            SAFE_DELETE_Ixxx(pSubMenuObj);
            return NULL;
        }

        pSubMenu->ModifyStyleEx(MENU_STYLE_AUTO_DELETE_SUBMENU, 0, 0);
        IListItemBase* pItem = this->AppendPopup(strText.c_str(), nID, pSubMenu);
        if (NULL == pItem)
        {
            UI_LOG_WARN(_T("%s AppendMenu failed."), FUNC_NAME);
            SAFE_DELETE_Ixxx(pSubMenu);
        }
        else
        {
            if (bDisable)
                pItem->SetDisable(true);

			SERIALIZEDATA data = {0};
			data.pMapAttrib = pMapAttrib;
			data.nFlag = SERIALIZEFLAG_LOAD;
			UISendMessage(pItem, UI_WM_SERIALIZE, (WPARAM)&data);
//            UISendMessage(pItem, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)false);
        }
        return pItem;
    }
    else
    {
        UI_LOG_WARN(_T("%s unknown tagname:%s"), FUNC_NAME, szTagName);
    }
    return NULL;
}


IListItemBase*  Menu::AppendString(const TCHAR* szText, UINT nId)
{
    // ���ID�Ƿ����
    if (m_pIMenu->GetItemById(nId))
    {
        UI_LOG_WARN(_T("%s ��id�Ѿ�����, text=%s, id=%d"), FUNC_NAME, szText, nId);
        return NULL;
    }

    IMenuStringItem* pItem = NULL;
    IMenuStringItem::CreateInstance(m_pIMenu->GetUIApplication(), &pItem);

    if (false == m_pIMenu->AddItem(pItem, LISTITEM_OPFLAG_NOALL))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

    pItem->SetText(szText);
    pItem->SetId(nId);
    pItem->SetItemFlag(MF_STRING);

    return static_cast<IListItemBase*>(pItem);
}
IListItemBase*  Menu::AppendSeparator(UINT nId)
{
    IMenuSeparatorItem* pItem = NULL;
    IMenuSeparatorItem::CreateInstance(m_pIMenu->GetUIApplication(), &pItem);

    if (false == m_pIMenu->AddItem(pItem, LISTITEM_OPFLAG_NOALL))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

    pItem->SetId(nId);
    pItem->SetDisable(true);
    pItem->SetItemFlag(MF_SEPARATOR);

    return static_cast<IListItemBase*>(pItem);
}
IListItemBase*  Menu::AppendPopup(const TCHAR* szText, UINT nId, IMenu* pSubMenu)
{
    IMenuPopupItem* pItem = NULL;
    IMenuPopupItem::CreateInstance(m_pIMenu->GetUIApplication(), &pItem);

    if (false == m_pIMenu->AddItem(pItem, LISTITEM_OPFLAG_NOALL))
    {
        SAFE_DELETE_Ixxx(pItem);
        return NULL;
    }

    pItem->SetText(szText);
    pItem->SetId(nId);
    pItem->SetCtrlData((LPARAM)pSubMenu);
    pItem->SetItemFlag(MF_POPUP);

    return static_cast<IListItemBase*>(pItem);
}

HRESULT  Menu::FinalConstruct(IUIApplication* p)
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    MenuItemLayout* pLayout = NULL;
    MenuItemLayout::CreateInstance(&pLayout);
    m_pIMenu->SetLayout(pLayout);
	m_pIMenu->ModifyStyleEx(LISTCTRLBASE_STYLE_MENU, 0, true);

    return S_OK;
}
void  Menu::FinalRelease()
{
    SetMsgHandled(FALSE);
    _DestroyAllSubMenu();
}

// TODO: ���ڲ˵��ڽ��滻��ʱ����Ȼ�����ߵ�Menu::ResetAttribute
void Menu::ResetAttribute()
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);    

    if (m_pIMenu->GetIScrollBarMgr())
    {
        m_pIMenu->GetIScrollBarMgr()->SetScrollBarVisibleType(HSCROLLBAR,SCROLLBAR_VISIBLE_NONE);
        m_pIMenu->GetIScrollBarMgr()->SetScrollBarVisibleType(VSCROLLBAR,SCROLLBAR_VISIBLE_NONE);
    }

    // TODO: �ֶ��л�����ʱ���ȷ���Ƿ�Ҫ�޸���Щֵ��
    if (IsThemeActive())
    {
        m_pIMenu->SetItemHeight(22);
        m_nIconGutterWidth = 28;
    }
    else
    {
        m_pIMenu->SetItemHeight(18);
        m_nIconGutterWidth = 20;
    }
    m_nPopupTriangleWidth = 20;

    m_bLayered = false;
}

void  Menu::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    IUIApplication*  pUIApplication = m_pIMenu->GetUIApplication();

    const TCHAR* szText = NULL;
    
    szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_TYPE, false);
    if (szText && 0 == _tcscmp(szText, XML_WINDOW_TRANSPARENT_TYPE_LAYERED))
    {
        m_bLayered = true;
    }

    // Ĭ����������
    if (NULL == pMapAttrib->GetAttr(XML_TEXTRENDER_TYPE, false))
    {
        if (NULL == m_pIMenu->GetTextRender())
        {
            ITextRenderBase* pTextRender = NULL;
            pUIApplication->CreateTextRenderBase(TEXTRENDER_TYPE_THEME_MENU, m_pIMenu, &pTextRender);

            if (pTextRender)
            {
                pTextRender->SetAttribute(pMapAttrib, NULL, true);  // ���ڳ�ʼ������
                m_pIMenu->SetTextRender(pTextRender);
                SAFE_RELEASE(pTextRender);
            }
        }
    }

    DO_PARENT_PROCESS(IMenu, IListCtrlBase);

    pMapAttrib->GetAttr_int(XML_MENU_ICONGUTTERWIDTH,    true, &m_nIconGutterWidth);
    pMapAttrib->GetAttr_int(XML_MENU_POPUPTRIANGLEWIDTH, true, &m_nPopupTriangleWidth);
    pMapAttrib->GetAttr_int(XML_MENU_SUBMENU_GAP,        true, &m_nSubMenuGap);

    if (NULL == m_pIMenu->GetBkRender())
    {
        IRenderBase* pRenderBase = NULL;
        pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_MENU_BKGND, m_pIMenu, &pRenderBase);
        if (pRenderBase)
        {
            m_pIMenu->SetBkgndRender(pRenderBase);

            CRegion4 r(2,2,2,2);
            m_pIMenu->SetBorderRegion(&r);
            SAFE_RELEASE(pRenderBase);
        }
    }
    if (NULL == m_pIMenu->GetForeRender())
    {
        IRenderBase* pRenderBase = NULL;
        pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_MENUSTRINGITEM, m_pIMenu, &pRenderBase);
        if (pRenderBase)
        {
            m_pIMenu->SetForegndRender(pRenderBase);
            SAFE_RELEASE(pRenderBase);
        }
    }
//     if (NULL == m_pSeparatorRender)
//     {
//         pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_MENUSEPERATOR, m_pIMenu, &m_pSeparatorRender);
//     }
//     if (NULL == m_pPopupTriangleRender)
//     {
//         pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_MENUPOPUPTRIANGLE, m_pIMenu, &m_pPopupTriangleRender);
//     }
    if (m_pIMenu->GetTextRender())
    {	
        m_pIMenu->GetTextRender()->SetTextAlignment(DT_SINGLELINE|DT_VCENTER|DT_LEFT);
    }
//     if (NULL == m_pCheckIconRender)
//     {
//         pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_MENUCHECKICON, m_pIMenu, &m_pCheckIconRender);
//     }
//     if (NULL == m_pRadioIconRender)
//     {
//         pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_MENURADIOICON, m_pIMenu, &m_pRadioIconRender);
//     }
//     if (NULL == m_pRadioCheckIconBkRender)
//     {
//         pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_MENURADIOCHECKICONBK, m_pIMenu, &m_pRadioCheckIconBkRender);
//     }
}

// ����֧�ֲַ㴰����ʽ�Ĳ˵� 
// ���ڲ˵��մ���ʱû�д��ڣ���˸�Menu��������ʱ����֪��ʹ����������
// ��˸�Menu��һ��GetGraphicsRenderLibraryType����ȡ
LRESULT Menu::OnGetGraphicsRenderType()
{
    if (m_bLayered)
    {
        return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;
    }
    return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;
}


IListItemBase* Menu::GetMenuItemBySubMenu(IMenu* pSubMenu)
{
    IListItemBase* pItem = m_pIMenu->GetFirstItem();
    UINT nItemFlag = 0;
    while (pItem)
    {
        if (pItem->GetItemFlag() & MF_POPUP &&
            pItem->GetId() == (long)pSubMenu)
            return pItem;

        pItem = pItem->GetNextItem();
    }
    return NULL;
}

IMenu* Menu::GetSubMenuByPos(int nPos)
{
    IListItemBase* pItem = m_pIMenu->GetItemByPos(nPos);
    if (NULL == pItem)
        return NULL;

    if (pItem->GetItemFlag() & MF_POPUP)
    {
        return (IMenu*)pItem->GetCtrlData();
    }

    return NULL;
}
IMenu*  Menu::GetSubMenuById(long lId)
{
    IListItemBase* pItem = m_pIMenu->GetItemById(lId);
    if (NULL == pItem)
        return NULL;

    if (pItem->GetItemFlag() & MF_POPUP)
    {
        return (IMenu*)pItem->GetCtrlData();
    }

    return NULL;
}


int  Menu::TrackPopupMenu(UINT nFlag, int x, int y, IMessage* pNotifyObj, HWND hWndClickFrom, RECT* prcClickFrom)
{
    if (0 >= m_pIMenu->GetItemCount())
        return -1;

    m_nTrackPopupMenuFlag = nFlag;
    m_nRetCmd = 0;

    if (NULL == m_pPopupWrapWnd)
    {
        PopupMenuWindow::CreateInstance(m_pIMenu->GetUIApplication(), &m_pPopupWrapWnd);
        if (NULL == m_pPopupWrapWnd)
            return -1;

        m_pPopupWrapWnd->Create(m_pIMenu, _T("PopupMenuWindow"));
    }

    m_pIMenu->SetNotify(pNotifyObj, 0);
	m_pPopupWrapWnd->SetPopupFromInfo(hWndClickFrom, prcClickFrom);

    POINT pt = { x, y };
    m_pPopupWrapWnd->Show(pt, (nFlag&TPM_RETURNCMD)?TRUE:FALSE);

    return m_nRetCmd;
}

void  Menu::SetReturnCmd(UINT n) 
{
	m_nRetCmd = n; 
}

HWND Menu::GetPopupWindowHandle()
{
    if (NULL == m_pPopupWrapWnd)
    {
        return NULL;
    }
    return m_pPopupWrapWnd->GetHWND();
}
IWindow*  Menu::GetPopupWindow()
{
	return m_pPopupWrapWnd;
}

Menu* Menu::GetRootMenu()
{
    Menu* pRet = this;
    while (NULL != pRet)
    {
        if (NULL == pRet->GetPrevMenu())
            break;

        pRet = pRet->GetPrevMenu();
    }
    return pRet;
}

Menu* Menu::GetLastMenu()
{
    Menu* pRet = this;
    while (NULL != pRet)
    {
        if (NULL == pRet->GetNextMenu())
            break;

        pRet = pRet->GetNextMenu();
    }
    return pRet;
}

Menu* Menu::GetMenuByHWND(HWND hWnd)
{
    Menu* pMenu = this->GetRootMenu();
    while(NULL != pMenu)
    {
        if (hWnd == pMenu->GetPopupWindowHandle())
            return pMenu;

        pMenu = pMenu->GetNextMenu();
    }
    return NULL;
}


Menu* Menu::GetMenuByPos(const POINT& ptScreen)
{
    HWND hWnd = WindowFromPoint(ptScreen);
    return GetMenuByHWND(hWnd);
}

void Menu::_DestroyPopupWindow()
{
    m_pNextMenu = NULL;
    if (m_pPopupWrapWnd)
    {
        if (m_pPopupWrapWnd)
        {
            m_pPopupWrapWnd->Destroy();
            SAFE_DELETE_Ixxx(m_pPopupWrapWnd);
        }
    }
    if (0 != m_nTimerIDHidePopupSubMenu)
    {
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);
    }
    if (0 != m_nTimerIDShowPopupSubMenu)
    {
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
    }
    if (m_pPrevMenu)
    {
        m_pPrevMenu->m_pNextMenu = NULL;
        m_pPrevMenu = NULL;
    }
}

void  Menu::_HidePopupWindow()
{
    if (m_pNextMenu)
    {
        m_pNextMenu->_HidePopupWindow();
        m_pNextMenu = NULL;
    }

    if (m_pPopupWrapWnd)
    {
        m_pPopupWrapWnd->Hide();
    }
    if (m_pPrevMenu)
    {
        m_pPrevMenu->m_pNextMenu = NULL;
        m_pPrevMenu = NULL;
    }
}


// TODO: VK_LEFT VK_RIGHT��
void Menu::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    if (VK_DOWN == nChar)
    {
        IListItemBase* pOldHoverItem = m_pIMenu->GetHoverItem();
        m_pIMenu->SetHoverItem(GetNextSelectableItem(pOldHoverItem));

        // 		if (GetHoverItem() != pOldHoverItem)
        // 			this->ReDrawItem(GetHoverItem(), pOldHoverItem);
        m_pIMenu->InvalidateItem(m_pIMenu->GetHoverItem());
        m_pIMenu->InvalidateItem(pOldHoverItem);
        m_pIMenu->Refresh();
    }
    else if (VK_UP == nChar)
    {
        IListItemBase* pOldHoverItem = m_pIMenu->GetHoverItem();
        m_pIMenu->SetHoverItem(GetPrevSelectableItem(pOldHoverItem));

        if (m_pIMenu->GetHoverItem() != pOldHoverItem)
        {
            m_pIMenu->InvalidateItem(m_pIMenu->GetHoverItem());
            m_pIMenu->InvalidateItem(pOldHoverItem);
            m_pIMenu->Refresh();
        }
    }
    else if (VK_RIGHT == nChar)
    {
        IListItemBase*  pHoverItem = m_pIMenu->GetHoverItem();
        if (pHoverItem && (pHoverItem->GetItemFlag() & MF_POPUP))
        {
            if (0 != m_nTimerIDHidePopupSubMenu)
            {
                m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);
            }
            if (0 != m_nTimerIDShowPopupSubMenu)
            {
                m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
            }
            this->PopupSubMenu(pHoverItem);
        }
        else if (!m_bPopupAtParentRight && NULL != m_pPrevMenu)  // ���Լ��ڸ��˵�����൯��ʱ�����Ҽ���Ϊ�����Ӳ˵�
        {
            this->_HidePopupWindow();
        }
    }
    else if (VK_LEFT == nChar)
    {
        if (m_pPrevMenu)
        {
            this->_HidePopupWindow();
        }
    }
    else if (VK_ESCAPE == nChar)
    {
        this->_HidePopupWindow();
    }
    else if (VK_RETURN == nChar)
    {
        IListItemBase*  pItem = m_pIMenu->GetHoverItem();
        if (pItem)
        {
            OnClick(pItem);
        }
    }
}

//
//	��GetNextSelectableItem�ڲ�����
//  ��һ��û���ҵ�ʱ���ᱻ�ٵ���һ����ͷ����
//
IListItemBase* Menu::_GetNextSelectableItem(IListItemBase* pItem)
{
    if (NULL == pItem)
    {
        pItem = m_pIMenu->GetFirstItem();
        if (NULL == pItem)
            return NULL;
    }
    else
    {
        pItem = pItem->GetNextItem();
        if (NULL == pItem)
            return NULL;
    }

    while (pItem)	
    {
        if(pItem->IsDisable())
        {
            pItem = pItem->GetNextItem();
            continue;
        }
        else
        {
            break;
        }
    }

    return pItem;
}
IListItemBase* Menu::GetNextSelectableItem(IListItemBase* pItem)
{
    IListItemBase* pItemRet = _GetNextSelectableItem(pItem);
    if (NULL == pItemRet)
    {
        if (NULL == pItem)  // �Ѿ��Ǵ�ͷ��ʼ���ҵģ������Ƿ���NULL
            return NULL;
        else                // ����ͷ��һ��
            pItemRet = _GetNextSelectableItem(NULL);
    }

    return pItemRet;
}

IListItemBase* Menu::_GetPrevSelectableItem(IListItemBase* pItem)
{
    if (NULL == pItem)
    {
        pItem = m_pIMenu->GetLastItem();
        if (NULL == pItem)
            return NULL;
    }
    else
    {
        pItem = pItem->GetPrevItem();
        if (NULL == pItem)
            return NULL;
    }

    while (pItem)	
    {
        if(pItem->IsDisable())
        {
            pItem = pItem->GetPrevItem();
            continue;
        }
        else
        {
            break;
        }
    }

    return pItem;
}
IListItemBase* Menu::GetPrevSelectableItem(IListItemBase* pItem)
{
    IListItemBase* pItemRet = _GetPrevSelectableItem(pItem);
    if (NULL == pItemRet)
    {
        if (NULL == pItem)  // �Ѿ��Ǵ�ͷ��ʼ���ҵģ������Ƿ���NULL
            return NULL;
        else                // ����ͷ��һ��
            pItemRet = _GetPrevSelectableItem(NULL);
    }

    return pItemRet;
}

LRESULT  Menu::OnHoverItemChanged(WPARAM w, LPARAM l)
{
    IListItemBase*  pOldHover = (IListItemBase*)w;
    IListItemBase*  pNewHover = (IListItemBase*)l;

    if (pNewHover)
    {
        if ((pNewHover->GetItemFlag() & MF_POPUP) && pNewHover->GetCtrlData() && !pNewHover->IsDisable())
        {
            ShowPopupSubMenu(pNewHover);
        }
        else
        {
            HidePopupSubMenu();
        }
    }
    return 0;
}
//
//  ������Ӳ˵����ƶ�ʱ���ڸ���������ʾ����·�����ݹ飩
//
void Menu::OnMouseMove(UINT nFlags, CPoint point)
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);
    if (m_pPrevMenu)
    {
        m_pPrevMenu->OnSubMenuMouseMove(m_pIMenu);
    }
}
void Menu::OnMouseLeave()
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);

    // Ϊ�˽���Ӹ��˵���popup menu itemֱ���ƶ����Ӳ˵�����ʱ,
    // ���յ�һ��MOUSELEAVE��Ϣ������hover item��˸һ�µ����⡣
    // ������������Ƶ����Ӳ˵�����ʱ����ȥ��յ�ǰ��hover item
    POINT  pt;
    GetCursorPos(&pt);
    Menu* pMenu = this->GetMenuByPos(pt);
    if (IsMyChildMenu(pMenu))
    {
        SetMsgHandled(TRUE);
    }

    // ȡ����Ҫ��ʾ�Ĳ˵�����
    if (0 != m_nTimerIDShowPopupSubMenu)
    {
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
    }
}
void Menu::OnSubMenuMouseMove(IMenu* pSubMenu)
{
    if (0 != m_nTimerIDHidePopupSubMenu)  // ��ֹ���˵������ٶ�ʱ�����Ӳ˵���������
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);

    IListItemBase* pItem = m_pIMenu->GetFirstItem();
    while (pItem)
    {
        if (pItem->GetItemFlag() & MF_POPUP &&
            pItem->GetCtrlData() == (WPARAM)pSubMenu)
        {
            IListItemBase* pOldHover = m_pIMenu->GetHoverItem();
            m_pIMenu->SetHoverItem(pItem);

            if (m_pPrevMenu)
                m_pPrevMenu->OnSubMenuMouseMove(m_pIMenu);

            m_pIMenu->InvalidateItem(pOldHover);
            m_pIMenu->InvalidateItem(pItem);

            break;
        }
        pItem = pItem->GetNextItem();
    }

    m_pIMenu->Refresh();  // ���������Ӳ˵���mousemove��������ø��˵���refresh�������������ˢ��
}


void Menu::OnLButtonDown(UINT nFlags, POINT point)
{
    DO_PARENT_PROCESS(IMenu, IListCtrlBase);

    IListItemBase* pPressItem = m_pIMenu->GetPressItem();
    if (pPressItem && (pPressItem->GetItemFlag()&MF_POPUP))
    {
        if (0 != m_nTimerIDHidePopupSubMenu)
        {
            m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);
        }
        if (0 != m_nTimerIDShowPopupSubMenu)
        {
            m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
        }
        IMenu* pSubMenu = (IMenu*)pPressItem->GetCtrlData();
        if (pSubMenu)
        {
            this->PopupSubMenu(pPressItem);
        }
        m_pIMenu->SetPressItem(NULL);

        return;
    }
}
void Menu::OnLButtonUp(UINT nFlags, POINT point)
{
    SetMsgHandled(FALSE);

//     IListItemBase* pSavePress = m_pIMenu->GetPressItem();
//     IListItemBase* pSaveHover = m_pIMenu->GetHoverItem();
//     if (pSavePress && pSavePress == pSaveHover)
//     {
//         if (pSavePress->IsDisable())
//             return;
// 
//         OnClick(pSavePress);
//     }

	IListItemBase* pSaveHover = m_pIMenu->GetHoverItem();
	if (pSaveHover)
	{
		if (pSaveHover->IsDisable())
			return;

		OnClick(pSaveHover);
	}
}

int  Menu::PopupSubMenu(IListItemBase* pItem)
{
    if (NULL == pItem)
        return -1;

    if (!(pItem->GetItemFlag() & MF_POPUP))
        return -1;

    IMenu* pISubMenu = (IMenu*)pItem->GetCtrlData();
    if (NULL == pISubMenu)
        return -1;

    Menu* pSubMenu = pISubMenu->GetImpl();
    if(pSubMenu == m_pNextMenu)
    {
        UI_LOG_WARN(_T("%s m_pNextMenu is same to pSubMenu"), FUNC_NAME);
        return -1;
    }

    if (m_pNextMenu != NULL)
    {
        m_pNextMenu->_HidePopupWindow();
        m_pNextMenu = NULL;
    }

    int nRet = pSubMenu->PopupAsSubMenu(m_nTrackPopupMenuFlag, this, pItem);
    if (-1 != nRet)
    {
        m_pNextMenu = pSubMenu;
    }
    return nRet;
}

// �Լ���Ϊ�Ӳ˵�������
int  Menu::PopupAsSubMenu(UINT nFlags, Menu* pParentMenu, IListItemBase* pItem)
{
    if (0 >= m_pIMenu->GetItemCount())
        return -1;

    m_nTrackPopupMenuFlag = nFlags;
    m_pPrevMenu = pParentMenu;

    HWND hParentWnd = pParentMenu->GetPopupWindowHandle();

    if (NULL == m_pPopupWrapWnd)
    {
        PopupMenuWindow::CreateInstance(m_pIMenu->GetUIApplication(), &m_pPopupWrapWnd);
        if (NULL == m_pPopupWrapWnd)
            return -1;

        m_pPopupWrapWnd->Create(m_pIMenu, _T("PopupSubMenuWindow"));
    }

    // 	ATTRMAP map = m_mapAttribute;
    // 	map[XML_ID] = _T("SubPopupMenuWindow");
    // 	m_pPopupWrapWnd->SetAttribute(map, false);  // ʹ��ʣ�µ����Ը����ڸ�ֵ

    m_pIMenu->UpdateItemRect();  // ����ʾ֮ǰ������������λ��

    // ���㵯��λ��
    int x = 0, y = 0;
    CRect rcParent, rcItem, rcWindow;
    ::GetWindowRect(hParentWnd, &rcParent);
    m_pIMenu->GetWindowRect(&rcWindow);
    pItem->GetParentRect(&rcItem);

    x = rcParent.right + m_nSubMenuGap;
    y = rcParent.top + rcItem.top;

    CRect  rcWorkArea;
    SystemParametersInfo(SPI_GETWORKAREA,0,&rcWorkArea,0);
    m_bPopupAtParentRight = true;

    if (x + rcWindow.Width() > rcWorkArea.right)
    {
        m_bPopupAtParentRight = false;
        x = rcParent.left - rcWindow.Width() - m_nSubMenuGap;
    }

    if (x < rcWorkArea.left)
        x = rcWorkArea.left;

    if (y < rcWorkArea.top)
        y = rcWorkArea.top;
    if (y + rcWindow.Height() > rcWorkArea.bottom)
        y = rcWorkArea.bottom - rcWindow.Height();

    // ���ֱ����SetWindowPos ShowWindow�ᵼ�´����ڵ�һ����ʾʱ�ȱ��һ�Σ���һ��Bitbltʱ��
    // ��˽�ShowWindow�����Ƴ�����ԭ�������

    ::ShowWindow(m_pPopupWrapWnd->GetHWND(), SW_SHOWNA);// ����SetWindowPos���滹�ǻᵼ�º�һ��
    ::SetWindowPos(m_pPopupWrapWnd->GetHWND(), NULL,x,y, rcWindow.Width(), rcWindow.Height(), SWP_NOZORDER|/*SWP_SHOWWINDOW|*/SWP_NOACTIVATE);

    m_pIMenu->ClearNotify();
    pParentMenu->GetIMenu()->CopyNotifyTo(m_pIMenu);

    return 0;
}

void  Menu::OnClick(IListItemBase* pClickItem)
{
    if (NULL == pClickItem)
        return;

    if (pClickItem->GetItemFlag() & MF_POPUP)
    {
        PopupSubMenu(pClickItem);
        return;
    }

    this->GetRootMenu()->_HidePopupWindow();

    if (m_nTrackPopupMenuFlag & TPM_RETURNCMD)
    {
        this->GetRootMenu()->SetReturnCmd(pClickItem->GetId());
    }
    else
    {
        UIMSG  msg;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_MENU_CLICK;
        msg.wParam = (WPARAM)pClickItem->GetId();
        msg.pMsgFrom = m_pIMenu;
        m_pIMenu->DoNotify(&msg, true, m_pIMenu->GetUIApplication());
    }
}

void Menu::OnThemeChanged()
{
    SetMsgHandled(FALSE);

//     if (m_pSeparatorRender)
//         m_pSeparatorRender->CheckThemeChanged();
//     if (m_pPopupTriangleRender)
//         m_pPopupTriangleRender->CheckThemeChanged();
//     if (m_pRadioIconRender)
//         m_pRadioIconRender->CheckThemeChanged();
//     if (m_pCheckIconRender)
//         m_pCheckIconRender->CheckThemeChanged();
}



// ���pMenu�Ƿ����ҵ�����˵�
bool Menu::IsMyChildMenu(Menu* pMenu)
{
    if (NULL == pMenu)
        return false;

    Menu* pChildMenu = m_pNextMenu;
    while (NULL != pChildMenu)
    {
        if (pChildMenu == pMenu)
            return true;

        pChildMenu = pChildMenu->m_pNextMenu;
    }
    return false;
}


void Menu::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    TimerItem* pItem = (TimerItem*)lParam;
    if (NULL == pItem)
        return;

    if (nIDEvent == m_nTimerIDHidePopupSubMenu)
    {
        if (m_pNextMenu)
        {
            IListItemBase* pItem = GetMenuItemBySubMenu(m_pNextMenu->GetIMenu());  // ����DestroyPopupWindow֮ǰ����ΪDestroyPopupWindow֮�󣬻Ὣm_pNextMenu���

            m_pNextMenu->_HidePopupWindow();
            m_pNextMenu = NULL;

            if (pItem)
            {
                m_pIMenu->InvalidateItem(pItem);
                m_pIMenu->Refresh();
            }
        }
        m_nTimerIDHidePopupSubMenu = 0;
    }
    else if (nIDEvent == m_nTimerIDShowPopupSubMenu)
    {
        IListItemBase*  pMenuItem = (IListItemBase*)(pItem->wParam); 
        IMenu* pSubMenu = (IMenu*)pMenuItem->GetCtrlData();
        if (m_pNextMenu && pSubMenu != m_pNextMenu->GetIMenu())  // ��һ��δ�رյ�popup�˵�
        {
            m_pNextMenu->_HidePopupWindow();
            m_pNextMenu = NULL;
        }

        if (pSubMenu)
        {
            this->PopupSubMenu((IListItemBase*)(pItem->wParam));
        }

        m_nTimerIDShowPopupSubMenu = 0;
    }
}


void Menu::ShowPopupSubMenu(IListItemBase* pItem)
{
    if (0 != m_nTimerIDShowPopupSubMenu)
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
    if (0 != m_nTimerIDHidePopupSubMenu)
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDHidePopupSubMenu);

    if (m_pNextMenu && pItem->GetCtrlData() == (WPARAM)m_pNextMenu->GetIMenu()) // ��ǰ��ʾ�ľ��Ǹ��Ӳ˵�
        return;

    TimerItem  ti;
    ti.nRepeatCount = 1;
    ti.pNotify = m_pIMenu;
    ti.wParam = (WPARAM)pItem;
    m_nTimerIDShowPopupSubMenu = m_pIMenu->GetUIApplication()->SetTimer(500, &ti);
}
void Menu::HidePopupSubMenu()
{
    if (0 != m_nTimerIDShowPopupSubMenu)
    {
        m_pIMenu->GetUIApplication()->KillTimer(m_nTimerIDShowPopupSubMenu);
    }
    if (0 == m_nTimerIDHidePopupSubMenu && NULL != m_pNextMenu)  // �в˵�����û�йر�
    {
        TimerItem  ti;
        ti.nRepeatCount = 1;
        ti.pNotify = m_pIMenu;
        m_nTimerIDHidePopupSubMenu = m_pIMenu->GetUIApplication()->SetTimer(500, &ti);
    }
}


void Menu::OnInitPopupControlWindow()
{

}

// �Լ��Ĳ˵�����������
void Menu::OnUnInitPopupControlWindow()
{
    if (m_pNextMenu)
    {
        m_pNextMenu->_HidePopupWindow();
        m_pNextMenu = NULL;
    }
    if (m_pPrevMenu)
    {
        m_pPrevMenu->m_pNextMenu = NULL;
        m_pPrevMenu = NULL;
    }
    m_pIMenu->SetHoverItem(NULL);
}


// 
// bool Menu::IsItemHilight(MenuItem* p)
// {
// 	if (p->IsPress() || (NULL == GetPressItem() && p->IsHover()))
// 	{
// 		return true;
// 	}
// 	else if (m_pNextMenu && p->GetSubMenu() == m_pNextMenu->GetIMenu() && NULL == GetHoverItem())
// 	{
// 		// ע�����ж������ڷ�ֹ����p�ƶ��������Ӳ˵���ʱ���м����һ��pʧȥhover��״̬��Ȼ������Ϊ�ƶ����Ӳ˵�
// 		//     �����ֵõ�hover�����½�����˸һ�Ρ���˵�����û��hover��ʱ����p��Ϊhover����������ﷵ��true ��
// 		//     ��������������Ƶ�p����һ��itemʱ������ȥˢ��p
// 		SetHoverItem(p);
// 		return true;
// 	}
// 	else
// 	{
// 		return false;
// 	}
// }

}

