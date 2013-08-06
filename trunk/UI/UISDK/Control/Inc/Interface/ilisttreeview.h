#ifndef ILISTTREEVIEWCTRL_H_44A1D78E_6D3B_4fe4_B981_11B97823854C
#define ILISTTREEVIEWCTRL_H_44A1D78E_6D3B_4fe4_B981_11B97823854C
#include "UISDK\Control\Inc\Interface\ilistview.h"

namespace UI
{

// {36CA5064-94E1-4c98-A8D4-DA2432898FFC}
static const GUID IID_UI_IListTreeViewItemBase = 
{ 0x36ca5064, 0x94e1, 0x4c98, { 0xa8, 0xd4, 0xda, 0x24, 0x32, 0x89, 0x8f, 0xfc } };


class ListTreeViewItemBase;
class IListTreeViewItemBase : public IListViewItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IListTreeViewItemBase, ListTreeViewItemBase)
};

class ListTreeView;
class IListTreeView : public IListView
{
    UI_DECLARE_Ixxx_INTERFACE(IListTreeView, ListTreeView);
};

}

#endif // ILISTTREEVIEWCTRL_H_44A1D78E_6D3B_4fe4_B981_11B97823854C