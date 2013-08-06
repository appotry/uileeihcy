#pragma once
#include "UISDK\Control\Inc\Interface\ipropertyctrl.h"
#include "UISDK\Control\Src\Control\TreeView\normaltreeitem.h"

namespace UI
{

class PropertyCtrlGroupItem : public NormalTreeItem
{
public:
    PropertyCtrlGroupItem();
    ~PropertyCtrlGroupItem();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(NormalTreeItem)

    UI_DECLARE_QUERY_INTERFACE(PropertyCtrlGroupItem);

public:
    void  SetIPropertyCtrlGroupItem(IPropertyCtrlGroupItem* p)  
    { m_pIPropertyCtrlGroupItem = p; }

protected:
    void  OnPaint(IRenderTarget* pRenderTarget);

private:
    IPropertyCtrlGroupItem*  m_pIPropertyCtrlGroupItem;
};

}