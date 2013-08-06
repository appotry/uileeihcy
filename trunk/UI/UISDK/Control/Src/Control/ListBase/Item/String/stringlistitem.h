#pragma once
#include "UISDK\Control\Inc\Interface\istringlistitem.h"

namespace UI
{

class StringListItemShareData : public MessageProxy 
{
public:
    StringListItemShareData()
    {
        m_rcItemPadding.SetRectEmpty();
        m_pIStringListItemShareData = NULL;
    }
    
    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(StringListItemShareData, IListItemTypeShareData)

    void  SetIStringListItemShareData(IStringListItemShareData* p)
    { m_pIStringListItemShareData = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

    void  GetItemPadding(CRegion4* prc) { prc->CopyRect(&m_rcItemPadding); }
    void  SetItemPadding(CRegion4* prc) { m_rcItemPadding.CopyRect(prc); }

public:
    CRegion4  m_rcItemPadding;  
    IStringListItemShareData*  m_pIStringListItemShareData;
};


class StringListItem : public MessageProxy
{
public:
    StringListItem();
    ~StringListItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(StringListItem, IListItemBase)

    UI_DECLARE_QUERY_INTERFACE(StringListItem);

public:
    void  SetIStringListItem(IStringListItem* p) { m_pIStringListItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

protected:
    void  GetDesiredSize(SIZE* pSize);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();

protected:
    IStringListItem*  m_pIStringListItem;
    StringListItemShareData*  m_pShareData;
};

}