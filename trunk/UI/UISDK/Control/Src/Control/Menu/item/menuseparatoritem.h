#pragma once
#include "UISDK\Control\Inc\Interface\imenu.h"

namespace UI
{

class MenuSeparatorItemShareData : public MessageProxy 
{
public:
    MenuSeparatorItemShareData();
    ~MenuSeparatorItemShareData();
    
    UI_BEGIN_MSG_MAP
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuSeparatorItemShareData, IListItemTypeShareData)

    void  SetIMenuSeparatorItemShareData(IMenuSeparatorItemShareData* p)
    { m_pIMenuSeparatorItemShareData = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IMenuSeparatorItemShareData*  GetIIMenuSeparatorItemShareData() { return m_pIMenuSeparatorItemShareData; }

protected:
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);

public:
    IMenuSeparatorItemShareData*  m_pIMenuSeparatorItemShareData;
    IListCtrlBase*  m_pListCtrl;
    IRenderBase*    m_pSeparatorRender;

    int  nIconGutterWidth;
    int  nHeight;
};

class MenuSeparatorItem : public MessageProxy
{
public:
    MenuSeparatorItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MenuSeparatorItem, IListItemBase)

    void  SetIMenuSeparatorItem(IMenuSeparatorItem* p)
    { m_pIMenuSeparatorItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

protected:
    void  GetDesiredSize(SIZE* pSize);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();

private:
    IMenuSeparatorItem*  m_pIMenuSeparatorItem;
    MenuSeparatorItemShareData*  m_pShareData;

    int nMenuIconGutter;
};


}