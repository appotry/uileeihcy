#pragma once
#include "UISDK\Control\Inc\Interface\itreeview.h"
#include "UISDK\Control\Src\Control\TreeView\treeitem.h"

namespace UI
{
class TreeView : public MessageProxy
{
public:
    TreeView();
    ~TreeView();
    UI_DECLARE_OBJECT3(TreeView, OBJ_CONTROL|CONTROL_TREEVIEWCTRL, _T("UICtrl/ListControl"))

    UI_BEGIN_MSG_MAP
        UIMSG_WM_KEYDOWN(OnKeyDown)
        UIMESSAGE_HANDLER_EX(UI_TVM_GETEXPANDICON_RENDER, OnGetExpandIconRender)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(TreeView, IListCtrlBase)

public:
    void  SetITreeView(ITreeView* p) { m_pITreeView = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    
    ITreeItem*  InsertNormalItem(UITVITEM* pItem, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, int nInsertFlags=0);
    ITreeItem*  InsertNormalItem(const TCHAR* szText, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, int nInsertFlags=0);

    IRenderBase*  GetExpandIconRender() { return m_pExpandIcon; }

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);

    LRESULT  OnGetExpandIconRender(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    ITreeView*  m_pITreeView;
    IRenderBase*   m_pExpandIcon;    // 展开/收缩图标绘制
};
}