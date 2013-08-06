#pragma  once
#include "UISDK\Control\Inc\Interface\ilistbox.h"

//////////////////////////////////////////////////////////////////////////
//
//                             列表框控件
//
//////////////////////////////////////////////////////////////////////////

namespace UI
{
class ListBoxItem;

class ListBoxBase : public MessageProxy
{
public:
	ListBoxBase();
	~ListBoxBase();

	UI_BEGIN_MSG_MAP
        UIMSG_WM_NOTIFY(UI_LCN_DRAWITEM, OnDrawItem)
        UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
        UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
        UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListBoxBase, IListCtrlBase)

    void  SetIListBoxBase(IListBoxBase* p) { m_pIListBoxBase = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IListBoxBase*  GetIListBoxBase() { return m_pIListBoxBase; }

public:
    bool  SetSel(int nIndex);
    int   GetListBoxStyle();
    void  SetListBoxStyle(int n);
    void  SetBindObject(IObject* pCombobox);

protected:
	LRESULT  OnDrawItem(WPARAM, LPARAM);
    LRESULT  OnGetGraphicsRenderType();

    HRESULT  FinalConstruct(IUIApplication* p);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void  OnInitPopupControlWindow();
    void  OnUnInitPopupControlWindow();

private:
    bool  IsPopup();

protected:
    IListBoxBase*  m_pIListBoxBase;
    IObject*  m_pBindObject;   // 例如从combobox弹出来的列表框，m_pBindObject将指向Combobox*
};


class ListBox : public ListBoxBase
{
public:
    ListBox();
    ~ListBox();

    UI_DECLARE_OBJECT3(ListBox, OBJ_CONTROL|CONTROL_LISTBOX, _T("UICtrl/ListControl"))
    UI_BEGIN_MSG_MAP
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
	UI_END_MSG_MAP_CHAIN_PARENT(ListBoxBase)

    void  SetIListBox(IListBox* p) { m_pIListBox = p; }

    IListBoxItem*  AddString(const TCHAR* szText, int nAddItemFlags=0);
    bool  RemoveItem(IListBoxItem* pItem, int nRemoveFlag=0);
public:

protected:
    IListBox*  m_pIListBox;
};
}