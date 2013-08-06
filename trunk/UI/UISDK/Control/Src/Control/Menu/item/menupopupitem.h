#pragma once
#include "UISDK\Control\Inc\Interface\imenu.h"

namespace UI
{

class MenuPopupItemShareData : public MessageProxy 
{
public:
    MenuPopupItemShareData();
    ~MenuPopupItemShareData();
    
    UI_BEGIN_MSG_MAP
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuPopupItemShareData, IListItemTypeShareData)

    void  SetIMenuPopupItemShareData(IMenuPopupItemShareData* p)
    { m_pIMenuPopupItemShareData = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IMenuPopupItemShareData*  GetIMenuPopupItemShareData() { return m_pIMenuPopupItemShareData; }

protected:
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);

public:
    IMenuPopupItemShareData*  m_pIMenuPopupItemShareData;
    IListCtrlBase*   m_pListCtrl;
    IRenderBase*     m_pPopupTriangleRender;

    int  nIconGutterWidth;
    int  nPopupTriangleWidth;
    int  nTextIndentLeft;
    int  nTextIndentRight;
};

class MenuPopupItem : public MessageProxy
{
public:
    MenuPopupItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuPopupItem, IListItemBase)

    void  SetIMenuPopupItem(IMenuPopupItem* p)
    { m_pIMenuPopupItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IMenuPopupItem*  GetIMenuPopupItem() { return m_pIMenuPopupItem; }

protected:
    void  GetDesiredSize(SIZE* pSize);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();

private:
    IMenuPopupItem*  m_pIMenuPopupItem;
    MenuPopupItemShareData*  m_pShareData;

    int nMenuIconGutter;
};


}