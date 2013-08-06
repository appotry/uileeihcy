#ifndef ISTRINGLISTITEM_H_3409EB46_BF0B_4754_B9ED_A9407873811E
#define ISTRINGLISTITEM_H_3409EB46_BF0B_4754_B9ED_A9407873811E
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"

namespace UI
{
#define LISTITEM_TYPE_STRING  136061745

// {DE36A197-DD34-4a97-B63A-E31A196E6D80}
static const GUID IID_UI_IStringListItem = 
{ 0xde36a197, 0xdd34, 0x4a97, { 0xb6, 0x3a, 0xe3, 0x1a, 0x19, 0x6e, 0x6d, 0x80 } };

class StringListItemShareData;
interface IStringListItemShareData : public IListItemTypeShareData 
{
    UI_DECLARE_Ixxx_INTERFACE(IStringListItemShareData, StringListItemShareData);

    void  GetItemPadding(CRegion4* prc);
    void  SetItemPadding(CRegion4* prc);
};


class StringListItem;
interface UICTRLAPI IStringListItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IStringListItem, StringListItem);
};

}

#endif