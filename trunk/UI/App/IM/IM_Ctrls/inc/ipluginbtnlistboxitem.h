#ifndef IPLUGINBTNLISTBOXITEM_H_7617AE14_D713_467a_980C_D167E76165F5
#define IPLUGINBTNLISTBOXITEM_H_7617AE14_D713_467a_980C_D167E76165F5

interface IPluginBtnListBoxItem;

// 点击checkbox通知事件code
// Wparam:  IListCtrlBase
// LParam:  PluginBtnListBoxItemCheckBtnClick*
// 
#define  LBN_PLUGIN_CHECKBTN_CLICKED  135241301   
struct PluginBtnListBoxItemCheckBtnClick : public UI::UINMHDR
{
    IPluginBtnListBoxItem*  pItem;
    BOOL  bChecked;
};

class PluginBtnListBoxShareData;
interface IPluginBtnListBoxShareData : public UI::IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IPluginBtnListBoxShareData, PluginBtnListBoxShareData);
};

class PluginBtnListBoxItem;
interface IM_CTRLS_API IPluginBtnListBoxItem : public UI::IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IPluginBtnListBoxItem, PluginBtnListBoxItem);

    void  SetIconPath(const TCHAR*  szPath);
    void  SetSortValue(int n);
    void  SetCheck(bool b);
};


#endif // IPLUGINBTNLISTBOXITEM_H_7617AE14_D713_467a_980C_D167E76165F5