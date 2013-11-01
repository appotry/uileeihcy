#ifndef IPROPERTYCTRL_H_2D8F8B87_E8DC_4995_82DF_F9D6155B970E
#define IPROPERTYCTRL_H_2D8F8B87_E8DC_4995_82DF_F9D6155B970E
#include "UISDK\Control\Inc\Interface\itreeview.h"

namespace UI
{

// {87AE2A12-319E-4dde-9C5A-7CDA620F4944}
static const GUID IID_UI_IPropertyCtrlEditItem = 
{ 0x87ae2a12, 0x319e, 0x4dde, { 0x9c, 0x5a, 0x7c, 0xda, 0x62, 0xf, 0x49, 0x44 } };

// {8530DBFB-493E-49be-852D-52FA42FA13A4}
static const GUID IID_UI_IPropertyCtrlGroupItem = 
{ 0x8530dbfb, 0x493e, 0x49be, { 0x85, 0x2d, 0x52, 0xfa, 0x42, 0xfa, 0x13, 0xa4 } };

#define LISTITEM_TYPE_PropertyCtrlGroup  LISTITEM_TYPE_TREEITEM_NORMAL
#define LISTITEM_TYPE_PropertyCtrlEdit   136151908

// 属性控件线条及背景颜色
#define PROPERTYCTRL_LINE_COLOR 212,208,200,255  //(233,236,250,255);

// 获取属性控件分隔条的位置指针（相对于控件）
// RETURN：UINT*
#define UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR 136152136

// 获取EDIT控件
// RETURN: IEdit*
#define UI_PROPERTYCTRL_MSG_GETEDITCTRL 136162254


interface IPropertyCtrlEditItem;
struct PROPERTYCTRL_EDIT_ITEM_ACCEPTCONTENT
{
	IPropertyCtrlEditItem*  pItem;
	const TCHAR*  szKey;
	const TCHAR*  szNewValue;
	bool  bDefault;  
};
// Edititem接受编辑内容
// WPARAM: IPropertyCtrlEditItem*
// LPARAM: const TCHAR* newText
// WPARAM:
#define UI_PROPERTYCTRL_MSG_EDITITEM_ACCEPTCONTENT 136221752

// 外部给IListItemBase发送消息，设置属性的值
// WPARAM: const TCHAR* szText
#define UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING  130201219

// 外部给IListItemBase发送消息，设置属性的默认值
// WPARAM: const TCHAR* szText
#define UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING  130202239

// 外部给IListItemBase发送消息，获取key字段
// Return: const TCHAR* szText
#define UI_PROPERTYCTRLITEM_MSG_GETKEYSTRING  132311511

class PropertyCtrlEditItemShareData;
interface IPropertyCtrlEditItemShareData : public IListItemTypeShareData
{
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlEditItemShareData, PropertyCtrlEditItemShareData);
};

class PropertyCtrlEditItem;
interface UICTRLAPI IPropertyCtrlEditItem : public IListItemBase
{
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlEditItem, PropertyCtrlEditItem);

    void  SetValueText(const TCHAR* szText);
	void  SetDefaultValueText(const TCHAR* szText);
	void  SetKeyText(const TCHAR* szText);
};



class PropertyCtrlGroupItem;
interface UICTRLAPI IPropertyCtrlGroupItem : public INormalTreeItem
{
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlGroupItem, PropertyCtrlGroupItem)
};

class PropertyCtrl;
interface UICTRLAPI IPropertyCtrl : public ITreeView
{
    UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrl, PropertyCtrl);

    IPropertyCtrlGroupItem*  InsertGroupItem(const TCHAR* szName, const TCHAR* szDesc, 
        IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);

    IPropertyCtrlEditItem*   InsertEditProperty(const TCHAR* szText, const TCHAR* szValue, const TCHAR* szDesc, const TCHAR* szKey,
        IListItemBase* pParentItem, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);
};

}

#endif // IPROPERTYCTRL_H_2D8F8B87_E8DC_4995_82DF_F9D6155B970E