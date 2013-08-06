#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"


// �˵��� [�ǹ̶��и�]
//    ��������  ����NONE������AUTO
//    ��  �֣�  ����ȡmax width������client width��������������
//    AUTOSIZE: ����max width, ����total height

namespace UI
{
class MenuItemLayout : public IListCtrlLayout
{
public:
    MenuItemLayout();

    static void  CreateInstance(MenuItemLayout** pp) { *pp = new MenuItemLayout(); }
    virtual void  SetIListCtrlBase(IListCtrlBase* p);
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};

}