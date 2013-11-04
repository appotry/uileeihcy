#ifndef ILISTCTRLBASE_H_B087355B_4D7A_4e74_AE27_5FF37A91293C
#define ILISTCTRLBASE_H_B087355B_4D7A_4e74_AE27_5FF37A91293C
#include "UISDK\Kernel\Inc\Interface\icontrol.h"

namespace UI
{
class ListCtrlBase;
interface IListItemBase;
interface IScrollBarManager;
interface IListCtrlLayout;
interface IListItemTypeShareData;

typedef bool (*ListItemCompareProc)(IListItemBase* p1, IListItemBase* p2);
typedef bool (*ListItemEnumProc)(IListItemBase* pItem, WPARAM, LPARAM);   // 返回false时停止enum

enum LISTITEM_VISIBLE_POS_TYPE
{
    LISTITEM_UNVISIBLE_TOP = 1,      
    LISTITEM_VISIBLE_COVERTOP,       
    LISTITEM_VISIBLE,
    LISTITEM_VISIBLE_COVERBOTTOM,
    LISTITEM_UNVISIBLE_BOTTOM,
};

#define UITVI_ROOT    ((UI::IListItemBase*)TVI_ROOT)
#define UITVI_FIRST   ((UI::IListItemBase*)TVI_FIRST)
#define UITVI_LAST    ((UI::IListItemBase*)TVI_LAST)


// 添加列表项的一些标识，主要是用于提高批量添加时的效率
#define LISTITEM_OPFLAG_NOREDRAW           0x0001    // 不刷新 
#define LISTITEM_OPFLAG_NOUPDATEITEMRECT   0x0002    // 不去计算每个子项的位置。最后必须调用一次UpdateItemRectAndScrollRange(NULL);
#define LISTITEM_OPFLAG_NOSORT             0x0004    // 不去排序。最后必须调用一次
#define LISTITEM_OPFLAG_NOALL   (LISTITEM_OPFLAG_NOREDRAW|LISTITEM_OPFLAG_NOUPDATEITEMRECT|LISTITEM_OPFLAG_NOSORT)
typedef int LISTITEM_OPFLAGS;

// 消息映射ID分类 
// #define LISTCTRLBASE_KEYBOARDMOUSEMGR_PREMSG_ID   1   // mousemgr处理前先发给listctrl处理的消息映射ID
// #define LISTCTRLBASE_KEYBOARDMOUSEMGR_POSTMSG_ID  2   // mousemgr处理后再发给listctrl处理的消息映射ID
// #define LISTCTRLBASE_INNER_CONTROL_MSG_ID  3  // 内部控件ID。当内部控件调用DoNotify是，将发到这个ID上	
#define LISTCTRLBASE_EDIT_CONTROL_MSG_ID   4  // 编辑控件消息通知ID



//////////////////////////////////////////////////////////////////////////
// ListCtrl Notify/Message
//////////////////////////////////////////////////////////////////////////

//	双击listctrl
//		message: UI_WM_NOTIFY
//		code:    UI_LCN_DBCLICK
//		wparam:  POINT
//		lparam:  ListItemBase*
#define UI_LCN_DBCLICK   135311303

//	点击listctrl
//		message: UI_WM_NOTIFY
//		code:    UI_LCN_CLICK
//		lparam:  ListItemBase*
#define UI_LCN_CLICK     135311304

//  右击listctrl
//		message: UI_WM_NOTIFY
//		code:    UI_LCN_RCLICK
//		wparam:  POINT
//		lparam:  ListItemBase* 
#define UI_LCN_RCLICK     136291126

//	listctrl 当前选中项改变(combobox)
//		message: UI_WM_NOTIFY
//		code:    UI_LCN_SELCHANGED
//		wparam:  IListItemBase* pOld
//		lparam:  IListItemBase* pNew
#define UI_LCN_SELCHANGED_SELF  135311304   // 给自己派生类发送的
#define UI_LCN_SELCHANGED  135311305  // 给自己发送的



//  listctrl当前hover项改变\
//		message: UI_WM_NOTIFY
//		code:    UI_LCN_HOVERITEMCHANGED
//		wparam:  IListItemBase* pOld
//		lparam:  IListItemBase* pNew    
#define UI_LCN_HOVERITEMCHANGED 135311306

// DrawItem
//      wParam: IRenderTarget*
//      lParam: IListItemBase*
#define UI_LCN_DRAWITEM 135311307

// 检测一个坐标位于哪个可见item上
// HitTest
//     wParam: POINT (控件坐标，已计算了偏移，滚动等)
//     ret: IListItemBase*
//
#define UI_LCM_HITTEST  136041628

//
// 通知listctrl派生类添加了一个ITEM
// Message: UI_WM_NOTIFY
// Code:    UI_LCN_REMOVEITEM
// WPARAM:  IListItemBase*
// LPARAM:  (int)nAddFlag 是否刷新、排序等
//
#define UI_LCN_ITEMADD      136131220

// 通知listctrl派生类正在删除一个ITEM
// Message: UI_WM_NOTIFY
// Code:    UI_LCN_PRE_ITEMREMOVE
// WPARAM:  IListItemBase*
#define UI_LCN_PRE_ITEMREMOVE    136131221

// 通知listctrl派生类正在删除一个ITEM
// Message: UI_WM_NOTIFY
// Code:    UI_LCN_PRE_ITEMREMOVE
// WPARAM:  long lItemId
#define UI_LCN_POST_ITEMREMOVE    136131632

// 通知listctrl派生类正在删除所有ITEM
// Message: UI_WM_NOTIFY
// Code:    UI_LCN_REMOVEALLITEM
#define UI_LCN_PRE_ALLITEMREMOVE 136131222

// 通知listctrl派生类正在删除所有ITEM
// Message: UI_WM_NOTIFY
// Code:    UI_LCN_REMOVEALLITEM
#define UI_LCN_POST_ALLITEMREMOVE 136131223


interface UISDKAPI IListCtrlBase : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IListCtrlBase, ListCtrlBase);

    int   GetItemCount();
    int   GetVisibleItemCount();
    void  CalcFirstLastVisibleItem();
    void  SetCalcFirstLastVisibleItemFlag();

    bool  AddItem(IListItemBase* pItem, int nAddItemFlags=0);
    bool  InsertItem(IListItemBase* pItem, int nPos, int nAddItemFlags=0);
    bool  InsertItem(IListItemBase* pItem, IListItemBase* pInsertAfter, int nAddItemFlags=0);
    bool  InsertItem(IListItemBase* pItem, IListItemBase* pParent = UITVI_ROOT, IListItemBase* pInsertAfter = UITVI_LAST, int nInsertFlags=0);
    void  RemoveItem(IListItemBase* pItem, int nRemoveFlag=0);
    void  RemoveItem(int nIndex, int nRemoveFlag);
    void  RemoveAllChildItems(IListItemBase* pParent, int nRemoveFlag);
    void  RemoveAllItem(int nRemoveFlag);
    void  DelayRemoveItem(IListItemBase* pItem, int nRemoveFlag = 0);

    void  ToggleItemExpand(IListItemBase* pItem, bool bUpdate);
    void  CollapseItem(IListItemBase* pItem, bool bUpdate);
    void  ExpandItem(IListItemBase* pItem, bool bUpdate);

    IListItemBase*  GetHoverItem();
    IListItemBase*  GetPressItem();
    IListItemBase*  GetFocusItem();
    void  SetHoverItem(IListItemBase* pItem);
    void  SetPressItem(IListItemBase* pItem);

    IObject*  GetHoverObject();
    IObject*  GetPressObject();
    void      SetFocusObject(IObject* pObj);
    IObject*  GetFocusObject();

    IListItemBase*  GetItemByPos(int i, bool bVisibleOnly=true);
    IListItemBase*  GetItemById(long lId);
    IListItemBase*  FindItemByText(const TCHAR* szText, IListItemBase* pStart=NULL);
    IListItemBase*  FindChildItemByText(const TCHAR* szText, IListItemBase* pParent=NULL, IListItemBase* pStart=NULL);
    IListItemBase*  GetFirstItem();
    IListItemBase*  GetLastItem();
    IListItemBase*  GetFirstDrawItem();
    IListItemBase*  GetLastDrawItem();
    IListItemBase*  FindVisibleItemFrom(IListItemBase* pFindFrom=NULL);
    IListItemBase*  EnumItemByProc(ListItemEnumProc pProc, IListItemBase* pEnumFrom = NULL, WPARAM w = 0, LPARAM l = 0);

    IListItemBase*  GetFirstSelectItem();
    IListItemBase*  GetLastSelectItem();

    IListItemTypeShareData*  GetItemTypeShareData(int lType);
    void  SetItemTypeShareData(int lType, IListItemTypeShareData* pData);
    void  RemoveItemTypeShareData(int lType);

    IListItemBase*  HitTest(POINT ptWindow);
    IScrollBarManager*  GetIScrollBarMgr();
    void  SetSortCompareProc(ListItemCompareProc p);
    void  Sort();
    void  SwapItemPos(IListItemBase*  p1, IListItemBase* p2);
    void  ItemRect2WindowRect(const CRect* prc, CRect* prcRet);

    IRenderBase*  GetFocusRender();
    void  SetFocusRender(IRenderBase* p);
    void  SetFocusItem(IListItemBase* pItem);
    void  SelectItem(IListItemBase* pItem, bool bUpdate);
   
    void  InvalidateItem(IListItemBase* pItem);
    void  SetInvalidateAllItems();
    void  ClearInvalidateItems();
    int   GetInvalidateItemCount();
    void  Refresh();
    void  RedrawItem(IListItemBase** ppItemArray, int nCount);
    void  RedrawItemByInnerCtrl(IRenderTarget* pRenderTarget, IListItemBase* pItem);
    void  MakeItemVisible(IListItemBase* pItem, bool* pbNeedUpdate);

    void  UpdateItemRect();
    void  UpdateItemRect(IListItemBase* pStart, bool bRedraw);
    void  SetLayout(IListCtrlLayout* pLayout);
    IListCtrlLayout*  GetLayout();
    void  SetLayoutDefaultV();
    void  SetLayoutDefaultHV();
    void  UpdateItemIndex(IListItemBase* pStart);

    short  GetVertSpacing();
    short  GetHorzSpacing();
    SIZE  GetAdaptWidthHeight(int nWidth, int nHeight);
    int   GetChildNodeIndent();
    void  SetChildNodeIndent(int n);

    void  SetMinWidth(int n);
    void  SetMinHeight(int n);
    void  SetMaxWidth(int n);
    void  SetMaxHeight(int n);
    int   GetMinWidth(); 
    int   GetMinHeight();
    int   GetMaxWidth(); 
    int   GetMaxHeight();

    int   GetItemHeight();
    void  SetItemHeight(int nHeight, bool bUpdate=true);

    void  Scroll2Y(int nY, bool bUpdate);
    void  Scroll2X(int nX, bool bUpdate);
    void  ScrollY(int nY, bool bUpdate);
    void  ScrollX(int nX, bool bUpdate);
    void  SetScrollPos(int nX, int nY, bool bUpdate);
};


class ListItemTypeShareData;
interface UISDKAPI IListItemTypeShareData : public IMessage
{
    UI_DECLARE_Ixxx_INTERFACE(IListItemTypeShareData, ListItemTypeShareData);
    IListCtrlBase*  GetListCtrlBase();
};

// 每种布局要实现的基类
interface IListCtrlLayout
{
    virtual ~IListCtrlLayout() = 0 {};
    virtual void  SetIListCtrlBase(IListCtrlBase* p) = 0;
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent) = 0;
    virtual void  Measure(SIZE* pSize) = 0;
    virtual void  Release() = 0;
};

interface IListCtrlLayoutMgr
{
    virtual void  SetIListCtrlBase(IListCtrlBase* p) = 0;
    virtual bool  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent) = 0;
    virtual void  Measure(SIZE* pSize) = 0;

    virtual void  SetLayout(IListCtrlLayout* p) = 0;
    virtual IListCtrlLayout*  GetLayout() = 0;
};


}

#endif // ILISTCTRLBASE_H_B087355B_4D7A_4e74_AE27_5FF37A91293C