#pragma once 
#include "UISDK\Control\Inc\Interface\ilistbox.h"

namespace UI
{
class ListBoxItemShareData : public MessageProxy
{
public:
    ListBoxItemShareData();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListBoxItemShareData, IListItemTypeShareData)

    void  SetIListBoxItemShareData(IListBoxItemShareData* p)
    { m_pIListBoxItemShareData = p; SetIMessageProxy(static_cast<IMessage*>(m_pIListBoxItemShareData)); }

    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
public:
    IListBoxItemShareData*  m_pIListBoxItemShareData;
    int  m_nTextIndent;
};

class ListBoxItem : public MessageProxy
{
public:
    ListBoxItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListBoxItem, IListItemBase)

    void  SetIListBoxItem(IListBoxItem* p) { m_pIListBoxItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

public:
    void  OnInitialize();
    void  GetDesiredSize(SIZE* pSize);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);
    void  OnPaint(IRenderTarget* pRenderTarget);

protected:
    IListBoxItem*  m_pIListBoxItem;
    ListBoxItemShareData*  m_pShareData;
};
}