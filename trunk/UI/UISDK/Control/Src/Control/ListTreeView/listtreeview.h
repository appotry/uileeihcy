#pragma once
#include "UISDK\Control\Inc\Interface\ilisttreeview.h"
#include "UISDK\Control\Src\Control\ListView\listview.h"

namespace UI
{

class ListTreeView : public ListView
{
public:
    ListTreeView();
    ~ListTreeView();

    UI_DECLARE_OBJECT3(ListTreeView, OBJ_CONTROL|CONTROL_TREEVIEWCTRL, _T("UICtrl/ListControl"));

    UI_BEGIN_MSG_MAP
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(ListView)
  
public:
    void  SetIListTreeView(IListTreeView* p) { m_pIListTreeView = p; }
    IListTreeView*  GetIListTreeView(IListTreeView* p) { return m_pIListTreeView; }
        
private:
    IListTreeView*  m_pIListTreeView;
};

#if 0
	class ListTreeViewCtrlBase;
	class ListTreeViewItem : public TreeItemBase
	{
	public:
		ListTreeViewItem(ListTreeViewCtrlBase* pCtrl);
		~ListTreeViewItem();

		operator LPLISTITEMBASE(){return this;}

	public:
		ListViewSubItemBase*   GetFirstSubItem();
		ListViewSubItemBase*   GetLastSubItem();

		void  AddColumnItem(ListViewSubItemBase* pSubItem);

	protected:
		ListViewSubItemBase*  m_pFirstSubItem;
	};

	class UIAPI ListTreeViewCtrlBase : public TreeCtrlBase
	{
	public:
		ListTreeViewCtrlBase();
		~ListTreeViewCtrlBase();

		virtual HRESULT InitialConstruct();

		UI_BEGIN_MSG_MAP	
			UIMSG_WM_SIZE(OnSize)
			UIMSG_WM_HSCROLL(OnHScroll)
			UIMSG_HDCN_TRACKING(OnHeaderCtrlTracking)
			UIMSG_HDCN_TOTAL_WIDTH_CHANGED(OnHeaderCtrlTotalWidthChanged)
		UI_END_MSG_MAP_CHAIN_PARENT(TreeCtrlBase)

	protected:
		void OnSize( UINT nType, int cx, int cy );
		BOOL OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth);
		void OnHeaderCtrlTotalWidthChanged(int nTotalWidth);
		void OnHScroll(int nSBCode, int nPos, Message* pMsgFrom);
	public:
		virtual  void ResetAttribute();
		virtual  bool SetAttribute(ATTRMAP& mapAttrib, bool bReload);

		// 继承ListCtrlBase的虚方法
		virtual  void OnDrawItem(IRenderTarget* pRenderTarget, ListItemBase* p);
		virtual  SIZE OnMeasureItem(ListItemBase* p);

		// 自己的虚函数
		virtual  void OnDrawMainItem(IRenderTarget* pRenderTarget, ListTreeViewItem* p, const CRect* prcItem, int nRenderState);
		virtual  void OnDrawSubItem(IRenderTarget* pRenderTarget, ListViewSubItemBase* p, const CRect* prcItem, int nRenderState);

	protected:
		virtual  UIHTREEITEM  CreateTreeItemInstance() { return new ListTreeViewItem(this); }

	public:
		bool   InsertColumn(int nColumn, const TCHAR* szText, int nWidth, int nFormat=DT_LEFT|DT_SINGLELINE|DT_VCENTER|DT_END_ELLIPSIS);
		bool   SetItemText(UIHTREEITEM hTreeItem, int nSubItem, const TCHAR* szText, bool bUpdate);

		int    GetColumnCount();
		void   GetSubItemRect(UIHTREEITEM hTreeItem, int nSubItem, CRect* prcOut);

	protected:
		ListViewItemBase*      GetItem(int nRow) { return (ListViewItemBase*)GetItemBase(nRow);}
		ListViewSubItemBase*   GetSubItem(int nRow, int nCol);
		ListViewSubItemBase*   GetSubItem(ListTreeViewItem* pItem, int nCol);
		void   GetAllSubItemRect(ListTreeViewItem* pItem, CRect* prcArray);
		void   UpdateHeaderCtrlPos();

	protected:
		HeaderCtrl*  m_pHeaderCtrl;

	};

	class ListTreeViewCtrl : public ListTreeViewCtrlBase
	{
	public:
		UI_DECLARE_OBJECT(ListTreeViewCtrl, OBJ_CONTROL|CONTROL_TREEVIEWCTRL)

	};
#endif
}