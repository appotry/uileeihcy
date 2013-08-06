#pragma once
#include "UISDK\Control\Inc\Interface\ilistview.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"
namespace UI
{

class ListView;
class ListViewSubItemBase;

// 直接支持多列的列表控件
class  ListView : public MessageProxy
{
public:
	ListView();
	~ListView();

	UI_DECLARE_OBJECT3(ListView, OBJ_CONTROL|CONTROL_LISTVIEWCTRL, _T("UICtrl/ListControl"))

	UI_BEGIN_MSG_MAP	
		UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_HSCROLL(OnHScroll)
        UIMSG_WM_MOUSEWHEEL(OnMouseWheel)
		UIMSG_HDCN_TRACKING(OnHeaderCtrlTracking)
		UIMSG_HDCN_TOTAL_WIDTH_CHANGED(OnHeaderCtrlTotalWidthChanged)
        UIMESSAGE_HANDLER_EX(UI_LVM_GETCOLUMNSINFO, OnGetColumnsInfo)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListView, IListCtrlBase)

public:
    void  SetIListView(IListView* p) { m_pIListView = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IListView*  GetIListView() { return m_pIListView; }
    IHeaderListCtrl*  GetHeaderCtrl() { return m_pHeaderCtrl; }

    IListViewItemBase*  AddTextItem(const TCHAR* szText);
    IListViewItemBase*  InsertTextItem(int nIndex, const TCHAR* szText);
    bool   SetItemText(int nItem, int nSubItem, const TCHAR* szText, bool bUpdate);
    
protected:
	void  OnSize( UINT nType, int cx, int cy );
	BOOL  OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth);
	void  OnHeaderCtrlTotalWidthChanged(int nTotalWidth);
	void  OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom);
    BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
    HRESULT  FinalConstruct(IUIApplication* p);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
    LRESULT  OnGetColumnsInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);

public:

	int    GetColumnCount();

protected:
	void   GetAllSubItemRect(ListViewItemBase* pItem, CRect* prcArray);
	void   UpdateHeaderCtrlPos();

protected:
    IListView*  m_pIListView;
	IHeaderListCtrl*  m_pHeaderCtrl;
};
}