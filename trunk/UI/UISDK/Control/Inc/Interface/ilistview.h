#ifndef ILISTVIEW_H_2999C69E_54BD_473e_9DE6_06EA7AB660F6
#define ILISTVIEW_H_2999C69E_54BD_473e_9DE6_06EA7AB660F6
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{
interface IHeaderListCtrl;
#define LISTITEM_TYPE_LISTVIEWITEMBASE  136121301

// 设置子项的文本数据
//  Wparam: UINT nIndex
//  LPARAM: const TCHAR* szText;
#define UI_LVIM_SETSUBITEMTEXT  136121750

// 获取子项的文本数据
// WPARAM: UINT nIndex
// return: const TCUAR* 
#define UI_LVIM_GETSUBITEMTEXT  136121758

#define UI_LVIM_ADDCOLUMN       136121751
#define UI_LVIM_REMOVECOLUMN    136121752
#define UI_LVIM_CLEARCOLUMN     136121753




// 获取列信息指针
// return:  IColumnsInfo*
#define UI_LVM_GETCOLUMNSINFO  136121304

class ListViewItemBase;
interface UICTRLAPI IListViewItemBase : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IListViewItemBase, ListViewItemBase);
};

class ListViewItemBaseShareData;
interface IListViewItemBaseShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IListViewItemBaseShareData, ListViewItemBaseShareData);
};

class ListView;
interface UICTRLAPI IListView : public IListCtrlBase
{
    UI_DECLARE_Ixxx_INTERFACE(IListView, ListView);

    IHeaderListCtrl*  GetHeaderCtrl();

    IListViewItemBase*  AddTextItem(const TCHAR* szText);
    IListViewItemBase*  InsertTextItem(int nIndex, const TCHAR* szText);
    bool   SetItemText(int nItem, int nSubItem, const TCHAR* szText, bool bUpdate);
};


}

#endif // ILISTVIEW_H_2999C69E_54BD_473e_9DE6_06EA7AB660F6