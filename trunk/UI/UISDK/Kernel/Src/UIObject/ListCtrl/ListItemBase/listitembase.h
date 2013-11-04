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
//	�б������ݣ�����item�ɸ�����
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
    

    // listctrl���ã���Ҫ�Լ�ˢ��
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

    // ����
protected:
    ListItemBase*  m_pPrev;
    ListItemBase*  m_pNext;
    ListItemBase*  m_pParent;
    ListItemBase*  m_pChild;
    ListItemBase*  m_pNextSelection;  // ��һ����ѡ�еĶ���(�Լ��ѱ�ѡ��)
    ListItemBase*  m_pPrevSelection;  // ��һ����ѡ�еĶ���(�Լ��ѱ�ѡ��)

    // ����
protected:
    long    m_lId;
    long    m_lItemType;      // һ�������п����ж������͵�item��m_nItemType���ڱ�ʶ����item����
    String  m_strText;  
    String  m_strToolTip;
    UINT    m_nItemFlag;      // ����Ϊ�ؼ���ʶһ��item�����ͻ��־������˵�Ϊ�����ʶMF_POPUP/MF_SEPARATOR/MF_STRING
    LPARAM  m_pData;          // ÿ��ITEM���Զ�������
    LPARAM  m_pCtrlData;      // ���ڿؼ�Ϊÿ��ITEM����һ���������͡���m_pData��ͬ��m_pData�������ⲿ���õ��߼����ݣ���
                              // m_pCtrlData�ǿؼ�Ϊitem���õ�һ���������ݣ�����MenuΪPopupMenuItem����һ��pSubMenu.

    // �����������
    IRenderBase*  m_pIconRender;
    UINT    m_nItemState;       // hover/press/selected/checked/disable...
    UINT    m_nStateInner;      // �����ڲ���ʶ���ֶΣ������Ƿ���Ҫ���²���
    UINT    m_nFlagsDelayOp;    // ��ʱ����
    CRect   m_rcParent;         // �����б�ؼ���client ���� 
    int     m_nLineIndex;       // ��¼����λ�ڵڼ���(�ɼ���)
    int     m_nTreeIndex;       // m_nLineIndex��ʶ�����з�����item�е�������m_nTreeIndex��ʶ�����ϵ�λ��
    int     m_nNeighbourIndex;  // m_nNeighbourIndex��ʶ��ͬһ�������е�����
    int     m_nDepth;           // ���

    int     m_nConfigWidth;
    int     m_nConfigHeight;

    // ������������
    IListItemBase*  m_pIListItemBase;
    ListCtrlBase*   m_pListCtrlBase;
    IPanel*         m_pPanelRoot;
};
typedef ListItemBase* LPLISTITEMBASE;
}