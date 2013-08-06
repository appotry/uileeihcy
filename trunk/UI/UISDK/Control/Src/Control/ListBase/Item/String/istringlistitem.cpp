#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\istringlistitem.h"
#include "UISDK\Control\Src\Control\ListBase\Item\String\stringlistitem.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE2(IStringListItem, StringListItem, IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE2(IStringListItemShareData, StringListItemShareData, IListItemTypeShareData);

void  IStringListItemShareData::GetItemPadding(CRegion4* prc)
{
    m_pStringListItemShareDataImpl->GetItemPadding(prc);
}
void  IStringListItemShareData::SetItemPadding(CRegion4* prc)
{
    m_pStringListItemShareDataImpl->SetItemPadding(prc);
}
}