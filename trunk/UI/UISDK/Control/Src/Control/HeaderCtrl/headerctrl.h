#pragma once
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"

// 自绘型header ctrl，已过期（不利于扩展），改用headerlistctrl
#if 0
namespace UI
{

enum HEADER_CTRL_HITTEST
{
	HDC_HT_ITEM_NOWHERE = 0,
	HDC_HT_ITEM = 1,                  // 位于一个Header Item上面
	HDC_HT_DIVIDER = 2,               // 位于一个分隔条上面
	HDC_HT_DIVIDER_HIDDEN_ITEM = 3,   // 位于一个分隔条上面，但是是一个隐藏项的分隔条
};

class HeaderItemBase
{
public:
	HeaderItemBase();

	int       m_nWidth;   
	String    m_strText;
	
	int       m_nFormat;  // 显示格式，这里的值是DrawText所使用的FLAG，与系统的headerctrl format不一样
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
		UIMSG_WM_LBUTTONDBLCLK(OnLButtonDown)  // TODO: 增加双击自动设置列宽功能 
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

	int    m_nDividerWidth;     // 分隔条拖拽宽度，用于HitTest
	POINT  m_ptLastDragDivider; // 鼠标按下进行拖拽时的位置
	int    m_nOldDragingColoumnWidth;  // 正在拖拽的列的原始宽度

	int    m_nHScrollOffset;

	IUICursor*    m_pCursorDragDivider;
	IUICursor*    m_pCursorDragDividerHidden;

	IRenderBase*  m_pRightBlankRender;  // 右侧剩余背景
	IRenderBase*  m_pSplitLineRender;   // 当m_bBkgndRenderContainSplitLine为false时才有用。
	IRenderBase*  m_pSortArrowRender;   // 排序箭头绘制

	bool   m_bDragingDivider;          // 是否在拖拽分隔条
	bool   m_bBkgndRenderContainSplitLine;  // 标识：bkgnd render绘制时，是否会绘制分隔条。如果不包含，则需要另外调用m_pSplitLineRender进行绘制
	bool   m_bDrawOffsetPressDown;    // 在按下状态时，是否将绘制区域偏移2px
};
	
	
}
#endif