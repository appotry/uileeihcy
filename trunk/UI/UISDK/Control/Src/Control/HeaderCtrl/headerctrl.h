#pragma once
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"

// �Ի���header ctrl���ѹ��ڣ���������չ��������headerlistctrl
#if 0
namespace UI
{

enum HEADER_CTRL_HITTEST
{
	HDC_HT_ITEM_NOWHERE = 0,
	HDC_HT_ITEM = 1,                  // λ��һ��Header Item����
	HDC_HT_DIVIDER = 2,               // λ��һ���ָ�������
	HDC_HT_DIVIDER_HIDDEN_ITEM = 3,   // λ��һ���ָ������棬������һ��������ķָ���
};

class HeaderItemBase
{
public:
	HeaderItemBase();

	int       m_nWidth;   
	String    m_strText;
	
	int       m_nFormat;  // ��ʾ��ʽ�������ֵ��DrawText��ʹ�õ�FLAG����ϵͳ��headerctrl format��һ��
	LPARAM    m_lParam;   
};

class  HeaderCtrl : public MessageProxy
{
public:
	HeaderCtrl();
	~HeaderCtrl();

	UI_DECLARE_OBJECT(HeaderCtrl, OBJ_CONTROL|CONTROL_HEADERCTRL)

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_MOUSEMOVE(OnMouseMove)
		UIMSG_WM_SETCURSOR(OnSetCursor)
		UIMSG_WM_MOUSELEAVE(OnMouseLeave)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_LBUTTONDBLCLK(OnLButtonDown)  // TODO: ����˫���Զ������п��� 
		UIMSG_WM_CANCELMODE(OnCancelMode)
		UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_GETSCROLLOFFSET(OnGetScrollOffset)
		UIMSG_WM_CALCPARENTNONCLIENTRECT(OnCalcParentNonClientRect)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(HeaderCtrl, IControl)

    void  SetIHeaderCtrl(IHeaderCtrl* p) { m_pIHeaderCtrl = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
	void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnMouseMove(UINT nFlags, POINT point);
	void  OnMouseLeave();
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);
	BOOL  OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void  OnCancelMode();
	void  OnCalcParentNonClientRect(CRegion4* prcNonClientRegion);
    void  OnGetScrollOffset(int* pxOffset, int* pyOffset);

public:
	void  GetDesiredSize(SIZE* pSize);
	void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  ResetAttribute();

	virtual  void OnDrawItem(IRenderTarget* pRenderTarget, HeaderItemBase* pItem, CRect* prcItem);

public:
    bool   AddColumn(const TCHAR* szText, int nWidth, int nFormat);
    bool   InsertColumn(int nPos, const TCHAR* szText, int nWidth, int nFormat);

public:
	bool  AddColumn(HeaderItemBase*);
	bool  InsertColumn(HeaderItemBase*, int nPos);
	bool  RemoveColumn(HeaderItemBase*);

	bool  SetItemWidth(int nColumn, int nNewWidth, bool bNotify);

	int   GetColumnCount();
	void  GetColumnWidthArray(int* pWidthArray);
	int   GetColumnTotalWidth();
	void  GetColumnLeftRight(int nColumnIndex, int* pLeft, int* pRight);

	HeaderItemBase* HitTest(POINT ptWindow, HEADER_CTRL_HITTEST* peHitTest=NULL);
	void  SetHoverItem(HeaderItemBase* pHoverItem, bool bUpdate);
	void  SetPressItem(HeaderItemBase* pPressItem, bool bUpdate);

	bool  CanDragColumn();
	void  SetHScrollOffset(int nhScrollOffset); 

protected:
	int   GetColumnItemIndex(HeaderItemBase* pItem);
	void  FireTotalWidthChanged();
	HeaderItemBase*  GetHeaderItem(int nCol);

protected:
    IHeaderCtrl*  m_pIHeaderCtrl;

	list<HeaderItemBase*>  m_listHeaderItem;
	HeaderItemBase*     m_pHoverItem;
	HeaderItemBase*     m_pPressItem;

	int    m_nDividerWidth;     // �ָ�����ק��ȣ�����HitTest
	POINT  m_ptLastDragDivider; // ��갴�½�����קʱ��λ��
	int    m_nOldDragingColoumnWidth;  // ������ק���е�ԭʼ���

	int    m_nHScrollOffset;

	IUICursor*    m_pCursorDragDivider;
	IUICursor*    m_pCursorDragDividerHidden;

	IRenderBase*  m_pRightBlankRender;  // �Ҳ�ʣ�౳��
	IRenderBase*  m_pSplitLineRender;   // ��m_bBkgndRenderContainSplitLineΪfalseʱ�����á�
	IRenderBase*  m_pSortArrowRender;   // �����ͷ����

	bool   m_bDragingDivider;          // �Ƿ�����ק�ָ���
	bool   m_bBkgndRenderContainSplitLine;  // ��ʶ��bkgnd render����ʱ���Ƿ����Ʒָ��������������������Ҫ�������m_pSplitLineRender���л���
	bool   m_bDrawOffsetPressDown;    // �ڰ���״̬ʱ���Ƿ񽫻�������ƫ��2px
};
	
	
}
#endif