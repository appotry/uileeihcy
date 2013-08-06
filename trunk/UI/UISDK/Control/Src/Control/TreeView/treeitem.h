#pragma once
#include "UISDK\Control\Inc\Interface\itreeview.h"

namespace UI
{
class TreeItem : public MessageProxy
{
public:
    TreeItem();
    ~TreeItem();

    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(TreeItem, IListItemBase)

public:
    void  SetITreeItem(ITreeItem* p) { m_pITreeItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    ITreeItem*  GetITreeItem() { return m_pITreeItem; }

    void  SetExpand(bool b);
    
protected:

protected:
    LRESULT  OnBtnExpandCollapse(BOOL& bHandled);
    void  CreateExpandCollapseCheckBtn();

protected:
    ITreeItem*  m_pITreeItem;
};

}