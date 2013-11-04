#ifndef LISTITEMROOTPANEL_H_2754B693_6417_48ca_A061_CEEFDDAD0BA6
#define LISTITEMROOTPANEL_H_2754B693_6417_48ca_A061_CEEFDDAD0BA6
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Src\UIObject\Panel\panel.h"

namespace UI
{

interface IListCtrlBase;
interface IListItemBase;

class ListItemRootPanel;
interface  IListItemRootPanel : public IPanel
{
    UI_DECLARE_Ixxx_INTERFACE(IListItemRootPanel, ListItemRootPanel);

    void  SetListCtrlItemPtr(IListCtrlBase* p1, IListItemBase* p2);
    IListItemBase*  GetListItemPtr();
};


// 内部子控件父Panel. 父对象为ListCtrl,但listctrl不知道自己有这个child object
// 该Panel的区域由ListItemBase::SetParentRect设置
class ListItemRootPanel : public Panel
{
public:
    ListItemRootPanel();

    UI_DECLARE_OBJECT(ListItemRootPanel, OBJ_PANEL|PANEL_LISTCTRLITEMROOT);

    UI_BEGIN_MSG_MAP
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT(Panel)

    void  SetIListItemRootPanel(IListItemRootPanel* p) { m_pIListItemRootPanel = p; }
    IListItemRootPanel*  GetIListItemRootPanel() { return m_pIListItemRootPanel; }

protected:
    void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
    void  ResetAttribute();

public:
    void  SetListCtrlItemPtr(IListCtrlBase* p1, IListItemBase* p2);
    IListItemBase*  GetListItemPtr() { return m_pIListItem; }

protected:
    IListItemRootPanel*  m_pIListItemRootPanel;

    IListCtrlBase*   m_pIListCtrl;
    IListItemBase*   m_pIListItem;
};

}


#endif // LISTITEMROOTPANEL_H_2754B693_6417_48ca_A061_CEEFDDAD0BA6