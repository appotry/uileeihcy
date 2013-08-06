#ifndef IICONSTRINGLISTITEM_H_3409EB46_BF0B_4754_B9ED_A9407873811E
#define IICONSTRINGLISTITEM_H_3409EB46_BF0B_4754_B9ED_A9407873811E
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"

namespace UI
{
#define LISTITEM_TYPE_ICONSTRING  136071627

// {DD1C2158-CDBE-461d-95CA-DD8121132DED}
static const GUID IID_UI_IIconStringListItem = 
{ 0xdd1c2158, 0xcdbe, 0x461d, { 0x95, 0xca, 0xdd, 0x81, 0x21, 0x13, 0x2d, 0xed } };

class IconStringListItemShareData;
interface IIconStringListItemShareData : public IListItemTypeShareData 
{
    UI_DECLARE_Ixxx_INTERFACE(IIconStringListItemShareData, IconStringListItemShareData);
};


class IconStringListItem;
interface UICTRLAPI IIconStringListItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IIconStringListItem, IconStringListItem);
};

}

#endif // IICONSTRINGLISTITEM_H_3409EB46_BF0B_4754_B9ED_A9407873811E