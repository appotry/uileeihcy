#pragma once
#include "UISDK\Control\Inc\Interface\icombobox.h"
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"

namespace UI
{

interface IEdit;
interface IButton;
interface IListBox;
class PopupListBoxWindow;

class ComboBoxBase : public MessageProxy
{
public:
	ComboBoxBase();
	~ComboBoxBase();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_STATECHANGED2(OnStateChanged)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_REDRAWOBJECT(OnRedrawObject)
		UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_NOTIFY(UI_CBN_CLOSEUP, OnDefaultCloseUp)
		UIMSG_WM_STYLECHANGED(OnStyleChanged)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONDBLCLK(OnLButtonDown)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)

	UIALT_MSG_MAP(COMBOBOX_BUTTON_MSG_HOOK_MSG_ID)  // Button hook
		UIMSG_WM_LBUTTONDOWN(OnBtnLButtonDown)
		UIMSG_WM_STATECHANGED2(OnChildObjStateChanged)

	UIALT_MSG_MAP(COMBOBOX_EDIT_MSG_HOOK_MSG_ID)
		UIMSG_WM_STATECHANGED2(OnChildObjStateChanged)

    UIALT_MSG_MAP(COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID)
        UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
        UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)

	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ComboBoxBase, IControl)

    void  SetIComboBoxBase(IComboBoxBase* p) { m_pIComboBoxBase = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IComboBoxBase*  GetIComboBoxBase() { return m_pIComboBoxBase; }

public:
    void  CloseUp();
    void  SetComboboxStyleType(int n);
    void  SetDropDownObjectPtr(IObject* p) { m_pDropDownObject = p; }
    IEdit*  GetEdit() { return m_pEdit; }
    const TCHAR*  GetText();

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
	void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  ResetAttribute();
	void  GetAutoSize(SIZE* pSize);

public:
	

protected:
	void  OnStateChanged(UINT nMask);
	void  OnChildObjStateChanged(UINT nMask);
	void  OnRedrawObject();

	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
	void  OnSize(UINT nType, int cx, int cy);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnBtnLButtonDown(UINT nFlags, POINT point);
	void  OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	
	void  OnInitPopupControlWindow();
	void  OnUnInitPopupControlWindow();

    void  OnDrawStaticCurSel(IRenderTarget* pRenderTarget, ITextRenderBase* pTextRender);

    LRESULT  OnDefaultCloseUp(WPARAM, LPARAM);
    bool  _DefaultDropDown();

protected:
    IComboBoxBase*  m_pIComboBoxBase;
	IEdit*  m_pEdit;
	IButton*  m_pButton;
    
    PopupListBoxWindow*  m_pPopupWrapWnd;
    IObject*  m_pDropDownObject;  // 仅用于引用，子类创建后同时也给该对象赋值，这样就能默认弹出下拉列表
};

class ComboBox : public ComboBoxBase
{
public:
    ComboBox();
    UI_DECLARE_OBJECT3(ComboBox, OBJ_CONTROL|CONTROL_COMBOBOX, _T("UICtrl/ListControl"));

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_NOTIFY(UI_CBN_DROPDOWN, OnDropDown)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_FINALRELEASE(FinalRelease)
    
    UIALT_MSG_MAP(COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID)  // ListBox Notify
        UIMSG_LCN_SELCHANGED(OnLCNSelChanged)
        UIMSG_WM_NOTIFY(UI_LCN_CLICK, OnLCNClick)
    UI_END_MSG_MAP_CHAIN_PARENT(ComboBoxBase)

public:
    void  SetIComboBox(IComboBox* p) { m_pIComboBox = p; }
    IComboBox*  GetIComboBox() { return m_pIComboBox; }

    bool  AddString(const TCHAR*, int nAddItemFlags=0);
    IListBoxItem*  AddStringEx(const TCHAR*, int nAddItemFlags=0);
    bool  SetCurSel(int nIndex);
    IListBox*  GetListBox() { return m_pDropDownCtrl; }

protected:
    void  OnPaint(IRenderTarget*);
    HRESULT  FinalConstruct(IUIApplication* p);
    void     FinalRelease();
    HRESULT  OnDropDown(WPARAM, LPARAM);
    void     SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void     ResetAttribute();
    void     OnLCNSelChanged(IMessage* pMsgFrom, IListItemBase* pOldSelItem, IListItemBase* pSelItem);
    LRESULT  OnLCNClick(WPARAM, LPARAM);

protected:
    IComboBox*  m_pIComboBox;
    IListBox*   m_pDropDownCtrl;
};


}