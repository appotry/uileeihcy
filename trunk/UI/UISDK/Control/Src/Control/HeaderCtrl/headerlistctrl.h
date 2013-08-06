#pragma once
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"

// TODO: ��������(û���뵽�������)
//
//  1. ���header itemʱ����������updateitemrect��������OnAddItem�У��ò�
//     ���ܴ�С��Ҳ��֪ͨ����listviewȥ����
//

namespace UI
{
enum HEADERLISTCTRL_HITTEST
{
	HDC_HT_ITEM_NOWHERE = 0,
	HDC_HT_ITEM = 1,                  // λ��һ��Header Item����
	HDC_HT_DIVIDER = 2,               // λ��һ���ָ�������
	HDC_HT_DIVIDER_HIDDEN_ITEM = 3,   // λ��һ���ָ������棬������һ��������ķָ���
};

class HeaderListCtrl;

class ColumnsInfoMgr : public IColumnsInfo
{
public:
    ColumnsInfoMgr(HeaderListCtrl* p);
    ~ColumnsInfoMgr();
    void  Destroy();

    void  Rebuild();
    void  UpdateWidth();

    UINT  GetCount() { return m_nCount; }
    ColumnInfo*  GetItemInfo(UINT n);

public:
    HeaderListCtrl*  m_pCtrl;
    UINT             m_nCount;
    ColumnInfo**     m_ppArray;
};

class HeaderListCtrl : public MessageProxy
{
public:
    HeaderListCtrl();
    ~HeaderListCtrl();

    UI_DECLARE_OBJECT3(HeaderListCtrl, OBJ_CONTROL|CONTROL_HEADERCTRL, _T("UICtrl/ListControl"))

    UI_BEGIN_MSG_MAP
        UIMSG_WM_MOUSEMOVE(OnMouseMove)
        UIMSG_WM_SETCURSOR(OnSetCursor)
        UIMESSAGE_HANDLER_EX(UI_LCM_HITTEST, OnHitTest)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONDBLCLK(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_CANCELMODE(OnCancelMode)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_CALCPARENTNONCLIENTRECT(OnCalcParentNonClientRect)

        UIMSG_WM_NOTIFY(UI_LCN_ITEMADD, OnAddItem)
        UIMSG_WM_NOTIFY(UI_LCN_POST_ITEMREMOVE, OnPostRemoveItem)
        UIMSG_WM_NOTIFY(UI_LCN_POST_ALLITEMREMOVE, OnPostRemoveAllItem)

        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(HeaderListCtrl, IListCtrlBase)

public:
    void  SetIHeaderListCtrl(IHeaderListCtrl* p) { m_pIHeaderListCtrl = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IHeaderListCtrl*  GetIHeaderListCtrl() { return m_pIHeaderListCtrl; }

    IHeaderListItem*   AddColumn(const TCHAR* szText, int nWidth, int nFormat=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
    IHeaderListItem*   InsertColumn(int nPos, const TCHAR* szText, int nWidth, int nFormat=DT_LEFT|DT_VCENTER|DT_SINGLELINE|DT_NOPREFIX|DT_END_ELLIPSIS);
    IRenderBase*  GetSortArrowRender() { return m_pSortArrowRender; }

    int   GetColumnCount();
    bool  CanDragColumn();
    void  SetHScrollOffset(int nhScrollOffset); 
    int   GetHScrollOffset();
    int   GetColumnTotalWidth();
    ColumnsInfoMgr*  GetColumnsInfo() { return &m_MgrColumnInfo; }

    IListItemBase*  HitTest(POINT ptWindow, HEADERLISTCTRL_HITTEST* peHitTest);

protected:
    HRESULT  FinalConstruct(IUIApplication* p);

    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void  ResetAttribute();
    BOOL  OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
    void  OnMouseMove(UINT nFlags, POINT point);
    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);
    void  OnCancelMode();
    void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
    void  GetDesiredSize(SIZE* pSize);
    LRESULT  OnHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  OnCalcParentNonClientRect(CRegion4* prcNonClientRegion);

    LRESULT  OnAddItem(WPARAM w, LPARAM l);
    LRESULT  OnPostRemoveItem(WPARAM w, LPARAM l);
    LRESULT  OnPostRemoveAllItem(WPARAM w, LPARAM l);
private:
    int   GetColumnItemIndex(IListItemBase* pItem);
    void  FireTotalWidthChanged();

private:
    IHeaderListCtrl*  m_pIHeaderListCtrl;
    ColumnsInfoMgr  m_MgrColumnInfo;

    POINT  m_ptLastDragDivider; // ��갴�½�����קʱ��λ��
    int    m_nOldDragingColoumnWidth;  // ������ק���е�ԭʼ���
    IListItemBase*  m_pLastDragItem;

    IUICursor*    m_pCursorDragDivider;
    IUICursor*    m_pCursorDragDividerHidden;

    IRenderBase*  m_pSortArrowRender;   // �����ͷ����

    bool   m_bDragingDivider;           // �Ƿ�����ק�ָ���
};
}
