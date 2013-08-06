#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"


// ϵͳlistctrl����headerctrl�Ŀ����Ϊdesired width
//    ��������  ����AUTO������AUTO
//    ���֣�    ����ȡָ��ֵ(HeaderCtrl Width)��������������
//    AUTOSIZE: ����ָ��ֵ(HeaderCtrl Width)������total width

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