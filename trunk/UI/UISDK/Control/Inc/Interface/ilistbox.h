#ifndef ILISTBOX_H_FFF69C0C_35D2_4ca2_985B_10E38FAB46C4
#define ILISTBOX_H_FFF69C0C_35D2_4ca2_985B_10E38FAB46C4
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{
#define  LISTITEM_TYPE_ListBoxItem  138101630

class ListBoxBase;
interface UICTRLAPI IListBoxBase : public IListCtrlBase
{
    UI_DECLARE_Ixxx_INTERFACE(IListBoxBase, ListBoxBase)
    bool  SetSel(int nIndex);
    void  SetBindObject(IObject* pCombobox);
};

class ListBoxItemShareData;
interface IListBoxItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IListBoxItemShareData, ListBoxItemShareData);
};


class ListBoxItem;
interface UICTRLAPI IListBoxItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IListBoxItem, ListBoxItem);
};

class ListBox;
interface UICTRLAPI IListBox : public IListBoxBase
{
    UI_DECLARE_Ixxx_INTERFACE(IListBox, ListBox);

    IListBoxItem*  AddString(const TCHAR* szText, int nAddItemFlags=0);
};


}

#endif // ILISTBOX_H_FFF69C0C_35D2_4ca2_985B_10E38FAB46C4