#pragma once
#include "UISDK\Control\Inc\Interface\iiconstringlistitem.h"

namespace UI
{

class IconStringListItemShareData : public MessageProxy 
{
public:
    IconStringListItemShareData()
    {
        m_rcItemPadding.SetRectEmpty();
        m_pIIconStringListItemShareData = NULL;
    }
    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(IconStringListItemShareData, IListItemTypeShareData)

    void  SetIIconStringListItemShareData(IIconStringListItemShareData* p)
    { m_pIIconStringListItemShareData = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

    void  GetItemPadding(CRegion4* prc) { prc->CopyRect(&m_rcItemPadding); }
    void  SetItemPadding(CRegion4* prc) { m_rcItemPadding.CopyRect(prc); }

public:
    IIconStringListItemShareData*  m_pIIconStringListItemShareData;
    CRegion4  m_rcItemPadding;
};

class IconStringListItem : public MessageProxy
{
public:
    IconStringListItem();
    ~IconStringListItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(IconStringListItem, IListItemBase)

    UI_DECLARE_QUERY_INTERFACE(IconStringListItem);
public:
    void  SetIIconStringListItem(IIconStringListItem* p) { m_pIIconStringListItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

protected:
    void  GetDesiredSize(SIZE* pSize);
    void  OnPaint(IRenderTarget* pRenderTarget);
    void  OnInitialize();

protected:
    IIconStringListItem*  m_pIIconStringListItem;
    IconStringListItemShareData*  m_pShareData;

    IRenderBase*  m_pIconRender;
};

}