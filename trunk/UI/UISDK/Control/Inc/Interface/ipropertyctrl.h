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

// {19236792-9745-4d21-B04F-A44FEB740B2B}
static const GUID IID_UI_IPropertyCtrlComboBoxItem = 
{ 0x19236792, 0x9745, 0x4d21, { 0xb0, 0x4f, 0xa4, 0x4f, 0xeb, 0x74, 0xb, 0x2b } };


#define LISTITEM_TYPE_PropertyCtrlGroup     LISTITEM_TYPE_TREEITEM_NORMAL
#define LISTITEM_TYPE_PropertyCtrlEdit      136151908
#define LISTITEM_TYPE_PropertyCtrlCombobox  132321222

// ���Կؼ�������������ɫ
#define PROPERTYCTRL_LINE_COLOR 212,208,200,255  //(233,236,250,255);

// ��ȡ���Կؼ��ָ�����λ��ָ�루����ڿؼ���
// RETURN��UINT*
#define UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR 136152136

// ��ȡEDIT�ؼ�
// RETURN: IEdit*
#define UI_PROPERTYCTRL_MSG_GETEDITCTRL 136162254
#define UI_PROPERTYCTRL_MSG_GETCOMBOBOXCTRL 136162255
#define UI_PROPERTYCTRL_MSG_GETBUTTONCTRL 136162256


interface IPropertyCtrlEditItem;
struct PROPERTYCTRL_EDIT_ITEM_ACCEPTCONTENT
{
	IPropertyCtrlEditItem*  pItem;
	const TCHAR*  szKey;
	const TCHAR*  szNewValue;
	bool  bDefault;  
};
// Edititem���ܱ༭����
// WPARAM: IPropertyCtrlEditItem*
// LPARAM: const TCHAR* newText
// WPARAM:
#define UI_PROPERTYCTRL_MSG_EDITITEM_ACCEPTCONTENT 136221752

interface IPropertyCtrlComboBoxItem;
struct PROPERTYCTRL_COMBOBOX_ITEM_ACCEPTCONTENT
{
	IPropertyCtrlComboBoxItem*  pItem;
	const TCHAR*  szKey;
	const TCHAR*  szNewValue;
	bool  bDefault;  
};
#define UI_PROPERTYCTRL_MSG_COMBOBOXITEM_ACCEPTCONTENT 136221753

// �ⲿ��IListItemBase������Ϣ���������Ե�ֵ
// WPARAM: const TCHAR* szText
#define UI_PROPERTYCTRLITEM_MSG_SETVALUESTRING  130201219

// �ⲿ��IListItemBase������Ϣ���������Ե�Ĭ��ֵ
// WPARAM: const TCHAR* szText
#define UI_PROPERTYCTRLITEM_MSG_SETDEFAULTVALUESTRING  130202239

// �ⲿ��IListItemBase������Ϣ����ȡkey�ֶ�
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


class PropertyCtrlComboBoxItemShareData;
interface IPropertyCtrlComboBoxItemShareData : public IListItemTypeShareData
{
	UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlComboBoxItemShareData, PropertyCtrlComboBoxItemShareData);
};

class PropertyCtrlComboBoxItem;
interface UICTRLAPI IPropertyCtrlComboBoxItem : public IListItemBase
{
	UI_DECLARE_Ixxx_INTERFACE(IPropertyCtrlComboBoxItem, PropertyCtrlComboBoxItem);

	void  SetValueText(const TCHAR* szText);
	void  SetDefaultValueText(const TCHAR* szText);
	void  SetKeyText(const TCHAR* szText);
	void  AddOption(const TCHAR* szItemText, const TCHAR* szItemValue);
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

	IPropertyCtrlComboBoxItem*   InsertBoolProperty(const TCHAR* szText, const TCHAR* szValue, const TCHAR* szDesc, const TCHAR* szKey,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);

	IPropertyCtrlComboBoxItem*   InsertComboBoxProperty(const TCHAR* szText, const TCHAR* szValue, const TCHAR* szDesc, const TCHAR* szKey,
		IListItemBase* pParentItem, IListItemBase* pInsertAfter = UITVI_LAST, LISTITEM_OPFLAGS nInsertFlags=0);
};

}

#endif // IPROPERTYCTRL_H_2D8F8B87_E8DC_4995_82DF_F9D6155B970E