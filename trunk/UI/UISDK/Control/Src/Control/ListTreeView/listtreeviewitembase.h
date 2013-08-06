#pragma once
#include "UISDK\Control\Inc\Interface\ilisttreeview.h"
#include "UISDK\Control\Src\Control\ListView\listviewitembase.h"

namespace UI
{

class ListTreeViewItemBase : public ListViewItemBase
{
public:
    ListTreeViewItemBase();
    ~ListTreeViewItemBase();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(ListViewItemBase)

    UI_DECLARE_QUERY_INTERFACE(ListTreeViewItemBase);

public:
    void  SetIListTreeViewItemBase(IListTreeViewItemBase* p)  { m_pIListViewItemBase = p;}

private:
    IListTreeViewItemBase*  m_pIListTreeViewItemBase;
};

}