#ifndef ICOMBOBOX_H_2548D733_F520_415e_9D3E_1DE433307792
#define ICOMBOBOX_H_2548D733_F520_415e_9D3E_1DE433307792
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{

#define COMBOBOX_BUTTON_MSG_HOOK_MSG_ID  1
#define COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID 2
#define COMBOBOX_EDIT_MSG_HOOK_MSG_ID    3

#define COMBOBOX_BUTTON_ID _T("combobox_button")
#define COMBOBOX_EDIT_ID   _T("combobox_edit")
#define COMBOBOX_LIST_ID   _T("combobox_list")

interface IListBoxItem;
interface IListBox;
interface IEdit;

class ComboBoxBase;
interface UICTRLAPI IComboBoxBase : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IComboBoxBase, ComboBoxBase);

    const TCHAR*  GetText();
    void  CloseUp();
    void  SetComboboxStyleType(int n);
    void  SetDropDownObjectPtr(IObject* p);
    IEdit*  GetEdit();
};


class ComboBox;
interface UICTRLAPI IComboBox : public IComboBoxBase
{
    UI_DECLARE_Ixxx_INTERFACE(IComboBox, ComboBox)

    bool  AddString(const TCHAR*, int nAddItemFlags=0);
    IListBoxItem*  AddStringEx(const TCHAR*, int nAddItemFlags=0);
    bool  SetCurSel(int nIndex);
    IListBox*  GetListBox();
};

}

#endif // ICOMBOBOX_H_2548D733_F520_415e_9D3E_1DE433307792