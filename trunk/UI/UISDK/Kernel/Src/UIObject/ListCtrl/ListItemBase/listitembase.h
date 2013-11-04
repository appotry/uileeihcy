#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"
#include "UISDK\Kernel\Src\Base\Message\message.h"

namespace UI
{

class RenderContext;
class ListItemRootPanel;
interface IPanel;
interface IListCtrlBase;
class ListCtrlBase;
//
//	列表项内容，子类item由该派生
//
class ListItemBase : public Message
{
public:
    ListItemBase();
    virtual ~ListItemBase();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_RBUTTONUP(OnRButtonUp)
        UIMSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
        if (m_pPanelRoot && ProcessItemMKMessage(pMsg))
        {
            return TRUE;
        }
        UIMESSAGE_HANDLER_EX(UI_LISTITEM_MSG_DELAY_OP, OnDelayOp)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP

    void  SetIListItemBase(IListItemBase* p) { m_pIListItemBase = p; }
    IListItemBase*  GetIListItemBase() { return m_pIListItemBase; }
    void  SetIListCtrlBase(IListCtrlBase* p);
    IListCtrlBase*  GetIListCtrlBase();

public:
    ListItemBase*  GetNextItem() { return m_pNext; }
    ListItemBase*  GetPrevItem() { return m_pPrev; }
    ListItemBase*  GetParentItem() { return m_pParent; }
    ListItemBase*  GetChildItem()  { return m_pChild; }
    ListItemBase*  GetPrevSelection() { return m_pPrevSelection; }
    ListItemBase*  GetNextSelection() { return m_pNextSelection; }

    ListItemBase*  GetNextVisibleItem();
    ListItemBase*  GetPrevVisibleItem();
    ListItemBase*  GetNextTreeItem(); 
    ListItemBase*  GetPrevTreeItem();

    ListItemBase*  GetLastChildItem();
    bool  HasChild() { return NULL != m_pChild; }
    bool  IsMyChildItem(ListItemBase* pChild, bool bTestGrandChildren);

    void  SetNextItem(ListItemBase* p) { m_pNext = p; }
    void  SetPrevItem(ListItemBase* p) { m_pPrev = p; }
    void  SetParentItem(ListItemBase* p) { m_pParent = p; }
    void  SetChildItem(ListItemBase* p)  { m_pChild = p; }
    void  SetNextSelection(ListItemBase* p){ m_pNextSelection = p; }
    void  SetPrevSelection(ListItemBase* p){ m_pPrevSelection = p; }
	void  AddChild(ListItemBase* p);
	void  AddChildFront(ListItemBase* p);
    
    const CRect*  GetParentRect() { return &m_rcParent; }
    void  GetParentRect(CRect* prc){ prc->CopyRect(&m_rcParent); }
    void  SetParentRect(CRect* prc);

    void  SetText(const TCHAR* szText);
    const TCHAR*  GetText()                   { return m_strText.c_str(); }
    void  SetToolTip(const TCHAR* szText);
    const TCHAR*  GetToolTip()                { return m_strToolTip.c_str(); }
    LPARAM  GetData()                         { return m_pData; } 
    void  SetData(LPARAM p)                   { m_pData = p; }
    LPARAM  GetCtrlData()                     { return m_pCtrlData; }
    void  SetCtrlData(LPARAM p)               { m_pCtrlData = p; }
    int   GetLineIndex()                      { return m_nLineIndex; }
    void  SetLineIndex(int n)                 { m_nLineIndex = n; }
    void  SetTreeIndex(int n)                 { m_nTreeIndex = n; }
    void  SetNeighbourIndex(int n)            { m_nNeighbourIndex = n; }
    void  SetDepth(int n)                     { m_nDepth = n; }
    int   GetTreeIndex()                      { return m_nTreeIndex; }
    int   GetNeighbourIndex()                 { return m_nNeighbourIndex; }
    int   GetDepth()                          { return m_nDepth; }
    void  SetConfigWidth(int n)               { m_nConfigWidth = n; }
    void  SetConfigHeight(int n)              { m_nConfigHeight = n; }

    void  GetDesiredSize(SIZE* pSize);

    IRenderBase*  GetIconRender() { return m_pIconRender; }
    void  SetIconRender(IRenderBase* p);
    void  SetIconRenderType(RENDER_TYPE eType);
    int   GetItemRenderState();
    void  SetIconFromFile(const TCHAR* szIconPath);

    UINT  GetItemState(); 
    UINT  GetItemDelayOp();
    void  AddItemDelayOp(int n);
    void  RemoveDelayOp(int n);
    void  ClearDelayOp();

    bool  IsMySelfVisible();
    bool  IsVisible();
    bool  IsDisable();
    bool  IsHover();
    bool  IsPress();
    bool  IsFocus();
    bool  IsCollapsed();
    bool  IsExpand();
    bool  IsSelected();
    bool  IsChecked();
    bool  IsRadioChecked();

    void  SetPress(bool b, bool bNotify=true);
    void  SetHover(bool b, bool bNotify=true);
    void  SetExpand(bool b, bool bNotify=true);
    void  SetFocus(bool b, bool bNotify=true);
    void  SetSelected(bool b, bool bNotify=true);
    void  SetDisable(bool b, bool bNotify=true);
    void  SetChecked(bool b, bool bNotify=true);
    void  SetRadioChecked(bool b, bool bNotify=true);
   
    // obj item 
    IPanel*  GetRootPanel();
    bool  CreateRootPanel();

    long  GetId() { return m_lId; }
    void  SetId(long n) { m_lId = n; }
    long  GetItemType() { return m_lItemType; }
    void  SetItemType(long n) { m_lItemType = n; }
    UINT  GetItemFlag() { return m_nItemFlag; }
    void  SetItemFlag(int n) { m_nItemFlag = n; }
    

    // listctrl调用，需要自己刷新
    void  Draw(IRenderTarget* pRenderTarget, RenderContext* proc);
    void  DrawItemInnerControl(IRenderTarget* pRenderTarget, RenderContext* proc);

    BOOL  ProcessItemMKMessage(UIMSG* pMSG);
    void  Fire_ClickNotify();

protected:
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);
    void  OnRButtonUp(UINT nFlags, POINT point);
    void  OnLButtonDblClk(UINT nFlags, POINT point);
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
    void*  QueryInterface(const IID* pIID);
    void  OnSize();
    LRESULT  OnDelayOp(UINT, WPARAM, LPARAM);

    // 导航
protected:
    ListItemBase*  m_pPrev;
    ListItemBase*  m_pNext;
    ListItemBase*  m_pParent;
    ListItemBase*  m_pChild;
    ListItemBase*  m_pNextSelection;  // 下一个被选中的对象(自己已被选中)
    ListItemBase*  m_pPrevSelection;  // 上一个被选中的对象(自己已被选中)

    // 属性
protected:
    long    m_lId;
    long    m_lItemType;      // 一个类型中可能有多种类型的item，m_nItemType用于标识各种item类型
    String  m_strText;  
    String  m_strToolTip;
    UINT    m_nItemFlag;      // 用于为控件标识一个item的类型或标志。例如菜单为子项标识MF_POPUP/MF_SEPARATOR/MF_STRING
    LPARAM  m_pData;          // 每个ITEM的自定义数据
    LPARAM  m_pCtrlData;      // 用于控件为每个ITEM分配一个数据类型。与m_pData不同，m_pData可能是外部设置的逻辑数据，而
                              // m_pCtrlData是控件为item设置的一个关联数据，例如Menu为PopupMenuItem关联一个pSubMenu.

    // 绘制相关数据
    IRenderBase*  m_pIconRender;
    UINT    m_nItemState;       // hover/press/selected/checked/disable...
    UINT    m_nStateInner;      // 用于内部标识的字段，例如是否需要更新布局
    UINT    m_nFlagsDelayOp;    // 延时操作
    CRect   m_rcParent;         // 基于列表控件的client 区域 
    int     m_nLineIndex;       // 记录该项位于第几行(可见行)
    int     m_nTreeIndex;       // m_nLineIndex标识在所有非隐藏item中的索引，m_nTreeIndex标识在树上的位置
    int     m_nNeighbourIndex;  // m_nNeighbourIndex标识在同一级对象中的索引
    int     m_nDepth;           // 深度

    int     m_nConfigWidth;
    int     m_nConfigHeight;

    // 其它依赖对象
    IListItemBase*  m_pIListItemBase;
    ListCtrlBase*   m_pListCtrlBase;
    IPanel*         m_pPanelRoot;
};
typedef ListItemBase* LPLISTITEMBASE;
}