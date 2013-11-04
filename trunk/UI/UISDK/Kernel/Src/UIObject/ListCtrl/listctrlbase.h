#pragma once
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\ListItemLayout\listitemlayout.h"
#include "UISDK\Kernel\Src\UIObject\Control\control.h"
#include "UISDK\Kernel\Src\UIObject\ScrollBar\scrollbarmanager.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\MouseKeyboard\listctrlmkmgr.h"


namespace UI
{
class ListCtrlBase;
interface IListItemBase;
enum LISTCTRL_NEED_UPDATE_FLAG
{
	LISTCTRL_NEED_UPDATE_FLAG_NONE = 0,  // ����Ҫˢ��
	LISTCTRL_NEED_UPDATE_FLAG_ITEM = 1,  // ����Ҫˢ��item��
	LISTCTRL_NEED_UPDATE_FLAG_ALL  = 2   // ��Ҫˢ�������ؼ�����������ˣ�
};
#define LISTCTRL_MAX_INVALIDATE_ITEM_COUNT 3

//////////////////////////////////////////////////////////////////////////
// �ڲ���Ϣ

// �ӳ�ɾ��
// message: UI_WM_NOTIFY
// code:    UI_LCN_INNER_DELAY_REMOVE_ITEM  
// wparam:  ListCtrlBase*
// lParam:  int nFlags
#define UI_LCN_INNER_DELAY_REMOVE_ITEM  138072146

class ListItemTypeShareData : public Message
{
public:
    ListItemTypeShareData();
    ~ListItemTypeShareData();

    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP

    void  SetIListItemTypeShareData(IListItemTypeShareData* p) { m_pIListItemTypeShareData = p; }
    void  SetListCtrlBase(ListCtrlBase* p);
    IListCtrlBase*  GetListCtrlBase();

private:
    IListItemTypeShareData*  m_pIListItemTypeShareData;
    ListCtrlBase*  m_pListCtrlBase;
};

// �б�ؼ�����
class  ListCtrlBase : public Control
{
public:
	ListCtrlBase();
	~ListCtrlBase();

	UI_BEGIN_MSG_MAP
// 		UICHAIN_MSG_MAP_POINT_MEMBER(m_pMouseMgr)
// 		UICHAIN_MSG_MAP_POINT_MEMBER(m_pKeyboardMgr)
        if (m_pMKMgr)
        {
            if (m_pMKMgr->ProcessMessage(pMsg))
            {
                return TRUE;
            }
        }
        UIMESSAGE_HANDLER_EX(UI_LCM_HITTEST, OnHitTest)
		UIMSG_WM_PAINT2(OnPaint)
        UIMESSAGE_HANDLER_EX(UI_WM_INERTIAVSCROLL, OnInertiaVScroll)
		UIMSG_WM_STATECHANGED2(OnStateChanged)
		UIMESSAGE_HANDLER_EX(UI_WM_GET_TOOLTIPINFO, OnGetToolTipInfo)
		UIMSG_WM_KEYDOWN(OnKeyDown)
		UIMSG_WM_SIZE(OnSize) 
		UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
        UIMESSAGE_HANDLER_EX(UI_WM_GET_MOUSEKEYBOARD_MANAGER, OnGetMouseKeyboardMgr)
		UIMSG_WM_VSCROLL(OnVScroll)
        UIMSG_WM_MOUSEWHEEL(OnMouseWheel)
		UICHAIN_MSG_MAP_POINT_MEMBER(m_pMgrScrollBar)
        UIMSG_WM_STYLECHANGED(OnStyleChanged)
        UIMSG_WM_NOTIFY(UI_LCN_INNER_DELAY_REMOVE_ITEM, OnDelayRemoveItem)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)            
        UIMSG_WM_FINALRELEASE(FinalRelease)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

    void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    IListCtrlBase*  GetIListCtrlBase() { return m_pIListCtrlBase; }

protected:
	// ��Ϣ����
    HRESULT  FinalConstruct(IUIApplication* p);
    void     FinalRelease();
	void     OnPaint(IRenderTarget* pRenderTarget, RenderContext* proc);
	void     OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	void     OnSize( UINT nType, int cx, int cy );
	LRESULT  OnGetGraphicsRenderType(){return GRAPHICS_RENDER_LIBRARY_TYPE_GDI;}  // ����popup listbox��ȡ����
	LRESULT  OnGetToolTipInfo(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void     OnStateChanged(UINT nMask);
	void     OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom);
    LRESULT  OnInertiaVScroll(UINT uMsg, WPARAM wParam, LPARAM lParam);
	void     GetDesiredSize(SIZE* pSize);
	void     ResetAttribute();
	void     SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    LRESULT  OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam);
    BOOL     OnMouseWheel(UINT nFlags, short zDelta, POINT pt);
    void     OnDrawItem(IRenderTarget* pRenderTarget, ListItemBase* p);
    void     OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
    LRESULT  OnGetMouseKeyboardMgr(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnDelayRemoveItem(WPARAM w, LPARAM l);

    void  OnKeyDown_Prev();
    void  OnKeyDown_Next();
    void  OnKeyDown_Home();
    void  OnKeyDown_End();
    void  OnKeyDown_Up();
    void  OnKeyDown_Down();

public:
	// ���ýӿ�
    bool  AddItem(ListItemBase* pItem, int nAddItemFlags=0);
    bool  InsertItem(ListItemBase* pItem, UINT nPos, int nAddItemFlags=0);
    bool  InsertItem(ListItemBase* pItem, ListItemBase* pInsertAfter, int nAddItemFlags=0);
    bool  InsertItem(ListItemBase* pItem, IListItemBase* pParent = UITVI_ROOT, IListItemBase* InsertAfter = UITVI_LAST, int nInsertFlags=0);
    void  RemoveItem(ListItemBase* pItem, int nRemoveFlag=0);
	void  RemoveItem(int nIndex, int nRemoveFlag);
    void  RemoveAllChildItems(ListItemBase* pParent, int nRemoveFlag);
	void  RemoveAllItem(int nRemoveFlag);
    void  DelayRemoveItem(ListItemBase* pItem, int nRemoveFlag = 0);

	void  SetSortCompareProc(ListItemCompareProc p);
    void  Sort();
    void  SwapItemPos(ListItemBase*  p1, ListItemBase* p2);

	int   GetItemCount() { return m_nItemCount; }
    int   GetVisibleItemCount();
	const TCHAR*  GetItemText(ListItemBase* pItem);
	
	ListItemBase*  GetItemByPos(UINT i, bool bVisibleOnly=true);
    ListItemBase*  GetItemById(long lId);
    ListItemBase*  FindItemByText(const TCHAR* szText, ListItemBase* pStart=NULL);
    ListItemBase*  FindChildItemByText(const TCHAR* szText, ListItemBase* pParent=NULL, ListItemBase* pStart=NULL);
	ListItemBase*  GetFirstItem() { return m_pFirstItem; }
	ListItemBase*  GetLastItem() { return m_pLastItem; }
	ListItemBase*  GetFirstDrawItem();
	ListItemBase*  GetLastDrawItem();
    ListItemBase*  FindVisibleItemFrom(ListItemBase* pFindFrom=NULL);
    ListItemBase*  EnumItemByProc(ListItemEnumProc pProc, ListItemBase* pEnumFrom = NULL, WPARAM w = 0, LPARAM l = 0);

	ListItemBase*  HitTest(POINT ptWindow);
	void  SetLayout(IListCtrlLayout* pLayout);
    void  SetLayoutDefaultV();
    void  SetLayoutDefaultHV();
    IListCtrlLayout*  GetLayout();

	void  SetMinWidth(int n)  { m_sizeMin.cx = n; }
	void  SetMinHeight(int n) { m_sizeMin.cy = n; }
	void  SetMaxWidth(int n)  { m_sizeMax.cx = n; }
	void  SetMaxHeight(int n) { m_sizeMax.cy = n; }
	int   GetMinWidth()  { return m_sizeMin.cx; }
	int   GetMinHeight() { return m_sizeMin.cy; }
	int   GetMaxWidth()  { return m_sizeMax.cx; }
	int   GetMaxHeight() { return m_sizeMax.cy; }
	SIZE  GetAdaptWidthHeight(int nWidth, int nHeight);

	short  GetVertSpacing() { return m_nVertSpacing; }
	short  GetHorzSpacing() { return m_nHorzSpacing; }
    void  GetTextIndent(int* pLeftIndent, int* pRightIndent);
	int   GetItemHeight() { return m_nItemHeight;}
	void  SetItemHeight(int nHeight, bool bUpdate=true);
    int   GetChildNodeIndent();
    void  SetChildNodeIndent(int n);

    void  CalcFirstLastVisibleItem();
    void  SetCalcFirstLastVisibleItemFlag();

	void  UpdateItemRect();
	void  UpdateItemRect(ListItemBase* pStart, bool bRedraw);
	IScrollBarManager*  GetIScrollBarMgr();
    void  UpdateItemIndex(ListItemBase* pStart);

    void  ToggleItemExpand(ListItemBase* pItem, bool bUpdate);
    void  CollapseItem(ListItemBase* pItem, bool bUpdate);
    void  ExpandItem(ListItemBase* pItem, bool bUpdate);

	// InvalidateItem ����
	void  InvalidateItem(ListItemBase* pItem);
	void  SetInvalidateAllItems();
	void  ClearInvalidateItems();
	int   GetInvalidateItemCount();
	void  Refresh();
	void  RedrawItem(ListItemBase** ppItemArray, int nCount);
	void  RedrawItemByInnerCtrl(IRenderTarget* pRenderTarget, ListItemBase* pItem);
    void  MakeItemVisible(ListItemBase* pItem, bool* pbNeedUpdate);

    IRenderBase*  GetFocusRender() { return m_pFocusRender; }
    void  SetFocusRender(IRenderBase* p);

	// select ����
	void  SelectItem(ListItemBase* pItem, bool bUpdate);
	void  AddSelectItem(ListItemBase* pItem);
	void  RemoveSelectItem(ListItemBase* pItem, bool bNotify);
	void  ClearSelectItem(bool bNotify);
	bool  IsSelected(ListItemBase* pItem);
	ListItemBase*  GetFirstSelectItem() { return m_pFirstSelectedItem; }
	ListItemBase*  GetLastSelectItem();

    void  SetHoverItem(ListItemBase* pItem);
    void  SetPressItem(ListItemBase* pItem);
	ListItemBase*  GetHoverItem();
	ListItemBase*  GetPressItem();
	ListItemBase*  GetFocusItem();
	void           SetFocusItem(ListItemBase* pItem);
    Object*        GetHoverObject();
    Object*        GetPressObject();
    void           SetFocusObject(Object* pObj);
    Object*        GetFocusObject();
//	void           CancelEdit();

    // item share data op
    IListItemTypeShareData*  GetItemTypeShareData(int lType);
    void  SetItemTypeShareData(int lType, IListItemTypeShareData* pData);
    void  RemoveItemTypeShareData(int lType);
    
    void  ItemRect2WindowRect(const CRect* prc, CRect* prcRet);
    void  WindowPoint2ItemPoint(ListItemBase* pItem, const POINT* pt, POINT* ptRet);
    BOOL  ProcessItemMKMessage(UIMSG* pMsg, ListItemBase* pItem);

    void  FireSelectItemChanged(ListItemBase* pOldValue);

    void  Scroll2Y(int nY, bool bUpdate);
    void  Scroll2X(int nX, bool bUpdate);
    void  ScrollY(int nY, bool bUpdate);
    void  ScrollX(int nX, bool bUpdate);
    void  SetScrollPos(int nX, int nY, bool bUpdate);

protected:
	// ����ӿ�
	bool  _InsertItem(ListItemBase* pItem, ListItemBase* pInsertAfter);
    bool  _InsertLastChild(ListItemBase* pNewItem, ListItemBase* pParent);
    bool  _InsertFirstChild(ListItemBase* pNewItem, ListItemBase* pParent);
    bool  _InsertFront(ListItemBase* pNewItem, ListItemBase* pInsertFront);
    bool  _InsertAfter(ListItemBase* pNewItem, ListItemBase* pInsertAfter);
    bool  _InsertRoot(ListItemBase* pNewItem);

	bool  _RemoveItem(ListItemBase* pItem);
	bool  _RemoveAllItem();
    bool  _RemoveAllChildItems(ListItemBase* pParent);

    bool  IsItemVisibleInScreen(ListItemBase* pItem);
    bool  IsItemVisibleInScreenEx(ListItemBase* pItem, LISTITEM_VISIBLE_POS_TYPE& ePos);

	void  MeasureItem(ListItemBase* pItem);
	void  MeasureAllItem();

protected:
    IListCtrlBase*  m_pIListCtrlBase;
    
    friend class  ListCtrlMouseMgr;
    ListCtrlMKMgrBase*    m_pMKMgr;

    ListCtrlLayoutMgr     m_MgrLayout;       // ���ֶ���
    ScrollBarManager*     m_pMgrScrollBar;   // ������

	// ���ݵ���
	ListItemBase*  m_pFirstItem;
	ListItemBase*  m_pLastItem;              // �������Ϳؼ���lastitemָlastrootitem
	ListItemBase*  m_pFirstDrawItem;   
	ListItemBase*  m_pLastDrawItem;   
	ListItemBase*  m_pFirstSelectedItem;
	ListItemBase*  m_pFocusItem;             // TODO: δʵ�� ���̽�����
    ListItemBase*  m_pEditingItem;           // ���ڱ༭�Ķ���

    // ����
    short  m_nVertSpacing;                   // �м��
    short  m_nHorzSpacing;                   // ������
    int    m_nChildNodeIndent;               // �ӽ������
    UINT   m_nItemCount;                     // �б�������
    int    m_nItemHeight;                    // �б���ĸ߶�
    SIZE   m_sizeMin;                        // auto size�б�����С�ߴ磬����nonclient region��������ָcontent size�����ǿؼ���С
    SIZE   m_sizeMax;                        // auto size�б������ߴ硣������������ width/height����ʱ��m_sizeMin/m_sizeMax������Ч��

    bool   m_bNeedCalcFirstLastVisibleItem;  // ��ʶ���¼�����Ҫ���ƵĶ���

    ListItemCompareProc   m_pCompareProc;    // ������
    map<int, IListItemTypeShareData*>  m_mapItemTypeShareData;  // item��չ��Ϊÿһ�����͵�item�ṩһ�����湲�����ݵķ���

    // ����
	LISTCTRL_NEED_UPDATE_FLAG  m_eInvalidateFlag;
	ListItemBase*  m_pInvalidateItems[LISTCTRL_MAX_INVALIDATE_ITEM_COUNT];  // ��Ҫˢ�µ�ITEM
    IRenderBase*   m_pFocusRender;
};
}