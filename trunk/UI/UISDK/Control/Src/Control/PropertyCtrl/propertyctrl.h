#pragma once
#include "UISDK\Control\Inc\Interface\ipropertyctrl.h"
#include "UISDK\Control\Src\Control\TreeView\treeview.h"

namespace UI
{

interface IEdit;
interface IButton;
interface IComboBox;

class PropertyCtrl : public TreeView
{
public:
    PropertyCtrl();
    ~PropertyCtrl();

    UI_DECLARE_OBJECT3(PropertyCtrl, OBJ_CONTROL|CONTROL_TREEVIEWCTRL, _T("UICtrl/ListControl"))

    UI_BEGIN_MSG_MAP
        UIMSG_WM_SIZE(OnSize)
        UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR, OnGetSplitterLinePosPtr)
        UIMESSAGE_HANDLER_EX(UI_WM_SHOW_TOOLTIP, OnPreShowTooltip)
        UIMESSAGE_HANDLER_EX(UI_PROPERTYCTRL_MSG_GETEDITCTRL, OnGetEditCtrl)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_FINALRELEASE(FinalRelease)
    UI_END_MSG_MAP_CHAIN_PARENT(TreeView)
  
public:
    void  SetIPropertyCtrl(IPropertyCtrl* p) { m_pIPropertyCtrl = p; }
    IPropertyCtrl*  GetIPropertyCtrl(IPropertyCtrl* p) { return m_pIPropertyCtrl; }
        
    IPropertyCtrlGroupItem*  InsertGroupItem(
        const TCHAR* szName, 
        const TCHAR* szDesc, 
        IListItemBase* pParent = UITVI_ROOT, 
        IListItemBase* pInsertAfter = UITVI_LAST, 
        LISTITEM_OPFLAGS nInsertFlags=0);

    IPropertyCtrlEditItem*   InsertEditProperty(
        const TCHAR* szKey, 
        const TCHAR* szValue, 
        const TCHAR* szDesc, 
        IListItemBase* pParentItem, 
        IListItemBase* pInsertAfter = UITVI_LAST, 
        LISTITEM_OPFLAGS nInsertFlags=0);

    // InsertComboProperty
    // InsertColorProperty
    // Insertxxxx

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    void  FinalRelease();
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
    void  OnSize(UINT nType, int cx, int cy);
    LRESULT  OnGetSplitterLinePosPtr(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnGetEditCtrl(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnPreShowTooltip(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    IEdit*      m_pEdit;
    IButton*    m_pButton;
    IComboBox*  m_pComboBox;

private:
    IPropertyCtrl*  m_pIPropertyCtrl;
    UINT  m_nSplitterLinePercent;   // 分隔线位置，用千分比来表示
    UINT  m_nSplitterLinePos;       // 分隔线位置，px，由percent计算而来
};

#if 0
	class PropertyCtrl;
	class PropertyCtrlItem : public ListTreeViewItem
	{
	public:
		PropertyCtrlItem(PropertyCtrl* p):ListTreeViewItem((ListTreeViewCtrlBase*)p)
		{
			m_bGroup = false;
		};

		bool  IsGroup() { return m_bGroup;}
		void  SetGroup(bool b){ m_bGroup = b; }

	protected:
		bool  m_bGroup;
	};

	class UIAPI PropertyCtrl : public ListTreeViewCtrlBase
	{
	public:
		PropertyCtrl();
		~PropertyCtrl();

#define PROP_KEY_COL   0  // Key所在列
#define PROP_VALUE_COL 1  // Value所在列

		virtual HRESULT InitialConstruct();
		UI_DECLARE_OBJECT(PropertyCtrl, OBJ_CONTROL|CONTROL_TREEVIEWCTRL)

			UI_BEGIN_MSG_MAP	
			UIMSG_HDCN_TRACKING(OnHeaderCtrlTracking)
			UIMSG_HDCN_BEGIN_TRACK(OnHeaderCtrlBeginTrack)
			UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
			UIMSG_WM_SIZE(OnSize)

			UIALT_MSG_MAP(LISTCTRLBASE_KEYBOARDMOUSEMGR_PREMSG_ID)
			UIMSG_WM_KEYDOWN(OnPreKeyDown)
			UIMSG_WM_KILLFOCUS(OnPreKillFocus)

			UIALT_MSG_MAP(LISTCTRLBASE_KEYBOARDMOUSEMGR_POSTMSG_ID)
			UIMSG_WM_LBUTTONDOWN(OnPostLButtonDown)

			UI_END_MSG_MAP_CHAIN_PARENT(ListTreeViewCtrlBase)

	public:
		virtual  void  ResetAttribute();
		virtual  bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload);

		void     OnEraseBkgnd(IRenderTarget* pRenderTarget);
		BOOL     OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth);
		LRESULT  OnHeaderCtrlBeginTrack(int nColumnIndex);
		void     OnPostLButtonDown(UINT nFlags, POINT point);
		void     OnPreKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
		void     OnPreKillFocus(Object* pNewFocusObj);
		void     OnSize(UINT nType, int cx, int cy);

	protected:
		virtual  UIHTREEITEM  CreateTreeItemInstance() { return new PropertyCtrlItem(this); }
		virtual  void OnDrawItem(IRenderTarget* pRenderTarget, ListItemBase* p);
		virtual  void OnDrawMainItem(IRenderTarget* pRenderTarget, ListTreeViewItem* p, const CRect* prcItem, int nRenderState);
		virtual  void OnDrawSubItem(IRenderTarget* pRenderTarget, ListViewSubItemBase* p, const CRect* prcItem, int nRenderState);
	public:
		UIHTREEITEM  InsertGroupProperty(const TCHAR* szName, UIHTREEITEM hParent, int nFlags);
		UIHTREEITEM  InsertProperty(const TCHAR* szKey, const TCHAR* szValue, UIHTREEITEM hParent, int nFlags);

	protected:
		void   AcceptEditContent();
		void   RejestEditContent();
		void   BeginEdit(PropertyCtrlItem* pItem);

	protected:
		TextRenderBase*  m_pGroupPropTextRender;
		TextRenderBase*  m_pPropValueTextRender;
		RenderBase*      m_pGroupPropForegndRender;
		UIColor*         m_pColorGrid;
	};
#endif
}

