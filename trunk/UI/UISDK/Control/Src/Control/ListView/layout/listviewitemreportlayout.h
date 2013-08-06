#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"


// 系统listctrl，以headerctrl的宽度作为desired width
//    滚动条：  横向AUTO，纵向AUTO
//    布局：    横向取指定值(HeaderCtrl Width)，纵向依次排列
//    AUTOSIZE: 横向指定值(HeaderCtrl Width)，纵向total width

namespace UI
{

class ListViewItemReportLayout : public IListCtrlLayout
{
public:
    ListViewItemReportLayout();
    static ListViewItemReportLayout* CreateInstance() { return new ListViewItemReportLayout(); }

    virtual void  SetIListCtrlBase(IListCtrlBase* p);
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

    void  SetHeaderCtrlWidth(int n) { m_nHeaderCtrlWidth = n; }

protected:
    IListCtrlBase*  m_pListCtrlBase;
    int   m_nHeaderCtrlWidth;
};

}