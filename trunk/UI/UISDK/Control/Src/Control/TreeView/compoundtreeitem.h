#pragma once
#include "treeitem.h"

namespace UI
{

// ��������㣬�����Ԫ�ض���object
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