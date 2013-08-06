#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\itreeview.h"
#include "UISDK\Control\Src\Control\TreeView\treeitem.h"
#include "UISDK\Control\Src\Control\TreeView\treeview.h"
#include "UISDK\Control\Src\Control\TreeView\normaltreeitem.h"
#include "UISDK\Control\Src\Control\TreeView\compoundtreeitem.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE2(ITreeItem, TreeItem, IListItemBase)
UI_IMPLEMENT_Ixxx_INTERFACE2(ITreeView, TreeView, IListCtrlBase)
UI_IMPLEMENT_Ixxx_INTERFACE(INormalTreeItem, NormalTreeItem, TreeItem)
UI_IMPLEMENT_Ixxx_INTERFACE(ICompoundTreeItem, CompoundTreeItem, TreeItem)
UI_IMPLEMENT_Ixxx_INTERFACE2(INormalTreeItemShareData, NormalTreeItemShareData, IListItemTypeShareData);

IRenderBase*  ITreeView::GetExpandIconRender()  { return m_pTreeViewImpl->GetExpandIconRender(); }
ITreeItem*  ITreeView::InsertNormalItem(UITVITEM* pItem, IListItemBase* pParent, IListItemBase* pInsertAfter, int nInsertFlags)
{ return m_pTreeViewImpl->InsertNormalItem(pItem, pParent, pInsertAfter, nInsertFlags); }
ITreeItem*  ITreeView::InsertNormalItem(const TCHAR* szText, IListItemBase* pParent, IListItemBase* pInsertAfter, int nInsertFlags)
{ return m_pTreeViewImpl->InsertNormalItem(szText, pParent, pInsertAfter, nInsertFlags); }

}