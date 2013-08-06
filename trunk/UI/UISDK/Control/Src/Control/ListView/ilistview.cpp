#include "stdafx.h"
#include "UISDK\Control\Inc\Interface\ilistview.h"
#include "UISDK\Control\Src\Control\listview\listview.h"
#include "UISDK\Control\Src\Control\ListView\listviewitembase.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE2(IListView, ListView, IListCtrlBase);
UI_IMPLEMENT_Ixxx_INTERFACE2(IListViewItemBase, ListViewItemBase, IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE2(IListViewItemBaseShareData, ListViewItemBaseShareData, IListItemTypeShareData);

IHeaderListCtrl*  IListView::GetHeaderCtrl()                         { return m_pListViewImpl->GetHeaderCtrl(); }

IListViewItemBase*  IListView::AddTextItem(const TCHAR* szText)      { return m_pListViewImpl->AddTextItem(szText); }
IListViewItemBase*  IListView::InsertTextItem(int nIndex, const TCHAR* szText) { return m_pListViewImpl->InsertTextItem(nIndex, szText); }
bool  IListView::SetItemText(int nItem, int nSubItem, const TCHAR* szText, bool bUpdate)  { return m_pListViewImpl->SetItemText(nItem, nSubItem, szText, bUpdate); }

}