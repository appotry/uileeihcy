#pragma once
#include "UISDK\Control\Inc\Interface\itabctrl.h"

namespace UI
{

class TabCtrlItemBase
{
public:
    TabCtrlItemBase();
    ~TabCtrlItemBase(){};

public:
    int       m_nId;
    IObject*  m_pBtn;
    IObject*  m_pContent;
    LPARAM    m_pData;
    int       m_nIndex;

    TabCtrlItemBase*  m_pPrev;
    TabCtrlItemBase*  m_pNext;
};

// 注： 2013.5.13 23:00 为了支持点击TabCtrl空白处实现窗口拖拽，将tabctrl做为panel类型来实现，而不是Control类型
class TabCtrlBase : public MessageProxy
{
public:
    TabCtrlBase();
    ~TabCtrlBase();
    UI_DECLARE_OBJECT3(TabCtrlBase, OBJ_PANEL|CONTROL_TABCTRL, _T("UICtrl/Control"))

    UI_BEGIN_MSG_MAP
        UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UIALT_MSG_MAP(1)
        UIMSG_BN_CLICKED3(OnBnClicked)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(TabCtrlBase, IPanel)

    void  SetITabCtrlBase(ITabCtrlBase* p) { m_pITabCtrlBase = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

public:
    void  AddItem(int nId, IObject* pBtn, IObject* pContent);
    void  RelayoutButton(int x, int y, int width, int height);

    IPanel*  GetHeadPanel() { return m_pPanelHead; }
    IPanel*  GetContentPanel() { return m_pPanelContent; }

protected:
    void  OnSize(UINT nType, int cx, int cy);
    void  OnBnClicked(IMessage*  pMsgFrom);
    HRESULT  FinalConstruct(IUIApplication* p);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);

    void  SetCurItem(TabCtrlItemBase*  pItem);
    TabCtrlItemBase*  FindItemByBtn(IObject*  p);

protected:
    ITabCtrlBase*  m_pITabCtrlBase;

    TabCtrlItemBase*  m_pFirstItem;
    TabCtrlItemBase*  m_pLastItem;
    TabCtrlItemBase*  m_pCurItem;

    IPanel*  m_pPanelHead;
    IPanel*  m_pPanelContent;

    TABCTRL_HEAD_LAYOUT_TYPE  m_eHeadLayoutType;
    int  m_nCount;
    int  m_nHeadHeight;
};


class TabCtrl : public TabCtrlBase
{
public:
    TabCtrl();
    ~TabCtrl();

    UI_DECLARE_OBJECT3(TabCtrl, OBJ_PANEL|CONTROL_TABCTRL, _T("UICtrl/Control"))

    UI_BEGIN_MSG_MAP
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(TabCtrlBase)

    void  SetITabCtrl(ITabCtrl* p) { m_pITabCtrl = p; }
    static HRESULT UIParseLayoutElement(IUIElement*, IUIApplication*  pUIApp, IObject* pParent, IObject** ppOut);
    HRESULT  UIParseNewItem(IUIElement* pChildElem);

public:
    void  AddItem(const TCHAR*  szText, IObject* pContentObj);
private:
    ITabCtrl*  m_pITabCtrl;
};

}