#pragma once
#include "treeitem.h"

namespace UI
{

// 复合树结点，里面的元素都是object
class CompoundTreeItem : public TreeItem
{
public:
    CompoundTreeItem();
    UI_BEGIN_MSG_MAP

    UI_END_MSG_MAP_CHAIN_PARENT(TreeItem)

    void  SetICompoundTreeItem(ICompoundTreeItem* p) { m_pICompoundTreeItem = p; }

private:
    ICompoundTreeItem*  m_pICompoundTreeItem;
};

}