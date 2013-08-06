#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
namespace UI
{
class HeaderListItemLayout : public IListCtrlLayout
{
public:
    HeaderListItemLayout() { m_pIListCtrlBase = NULL; }

    static HeaderListItemLayout*  CreateInstance() { return new HeaderListItemLayout; }
    virtual void  SetIListCtrlBase(IListCtrlBase* p){ m_pIListCtrlBase = p; };
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release();

    int  GetItemWidth(IListItemBase*  pItem);

private:
    UI::IListCtrlBase*  m_pIListCtrlBase;
};
}