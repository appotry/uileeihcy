#ifndef ILISTITEMBASE_H_5825D731_6107_4ced_A0C5_FA6097119404
#define ILISTITEMBASE_H_5825D731_6107_4ced_A0C5_FA6097119404
#include "UISDK\Kernel\Inc\Interface\imessage.h"

namespace UI
{
interface IPanel;
class ListItemBase;
interface IListCtrlBase;

//////////////////////////////////////////////////////////////////////////
// ListItem派生类消息

// 在绘制之前处理延迟操作
#define UI_LISTITEM_MSG_DELAY_OP  139222216

enum LISTITEM_DELAY_OP
{
    // 在控件的大小发生改变后，通知子对象。如果子对象目前没有处于可视区域，则稍后在自己可见时再更新自己的布局
    DELAY_OP_SIZE_CHANGED = 0x01,

    // 在控件布局重新改变之后，通知子对象。子对象在自己刷新的时候再更新自己的布局
    DELAY_OP_LAYOUT_CHANGED = 0x02,
};

interface UISDKAPI IListItemBase : public IMessage
{
    UI_DECLARE_Ixxx_INTERFACE(IListItemBase, ListItemBase);    

    void  SetIListCtrlBase(IListCtrlBase* p);
    IListCtrlBase*  GetIListCtrlBase();

    IListItemBase*  GetNextItem();
    IListItemBase*  GetPrevItem();
    IListItemBase*  GetParentItem();
    IListItemBase*  GetChildItem();
    IListItemBase*  GetPrevSelection();
    IListItemBase*  GetNextSelection();

    IListItemBase*  GetNextVisibleItem();
    IListItemBase*  GetPrevVisibleItem();
    IListItemBase*  GetNextTreeItem(); 
    IListItemBase*  GetPrevTreeItem();

    IListItemBase*  GetLastChildItem();
    bool  IsMyChildItem(IListItemBase* pChild, bool bTestGrandChildren);
    bool  HasChild();

    void  SetNextItem(IListItemBase* p);
    void  SetPrevItem(IListItemBase* p);
    void  SetParentItem(IListItemBase* p);
    void  SetChildItem(IListItemBase* p);
    void  SetNextSelection(IListItemBase* p);
    void  SetPrevSelection(IListItemBase* p);
	void  AddChild(IListItemBase* p);
	void  AddChildFront(IListItemBase* p);

    const CRect* GetParentRect();
    void  GetParentRect(CRect* prc);
    void  SetParentRect(CRect* prc);

    long  GetId();
    void  SetId(long n);
    int   GetItemType();
    void  SetItemType(long);
    UINT  GetItemFlag();
    void  SetItemFlag(int n);
    void  SetText(const TCHAR* szText);
    const TCHAR*  GetText();
    void  SetToolTip(const TCHAR* szText);
    const TCHAR*  GetToolTip();
    LPARAM  GetData();
    void  SetData(LPARAM p);
    LPARAM  GetCtrlData();
    void  SetCtrlData(LPARAM p);
    int   GetLineIndex() ;
    void  SetLineIndex(int n);
    void  SetTreeIndex(int n);
    void  SetNeighbourIndex(int n);
    void  SetDepth(int n);
    int   GetTreeIndex();
    int   GetNeighbourIndex();
    int   GetDepth();
    void  SetConfigWidth(int n);
    void  SetConfigHeight(int n);


    void  GetDesiredSize(SIZE* pSize);

    IRenderBase*  GetIconRender();
    void  SetIconRender(IRenderBase* p);
    void  SetIconRenderType(RENDER_TYPE eType);
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

    IPanel*  GetRootPanel();
    bool  CreateRootPanel();
    void  DrawItemInnerControl(IRenderTarget* pRenderTarget);
    int   GetItemRenderState();

};

}

#endif // ILISTITEMBASE_H_5825D731_6107_4ced_A0C5_FA6097119404