#pragma once
#include "UISDK\Control\Inc\Interface\iscrollbar.h"
#include "UISDK\Kernel\Inc\Interface\iscrollbarmanager.h"

namespace UI
{
interface  IButton;

#define MIN_TRACK_BTN_SIZE 20

#define ALT_MSG_ID_BUTTON1       1
#define ALT_MSG_ID_BUTTON2       2
#define ALT_MSG_ID_THUMB_BTN     3

// 操作系统类型滚动条：两侧两个按钮 + 中间一个THUBM按钮
class SystemScrollBar : public MessageProxy
{
public:
	SystemScrollBar();
	virtual ~SystemScrollBar();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_STATECHANGED2(OnStateChanged)
		UIMSG_WM_TIMER(OnTimer)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMESSAGE_HANDLER_EX(UI_WM_SCROLLBAR_INFO_CHANGED, OnScrollBarInfoChanged)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMESSAGE_HANDLER_EX(UI_WM_SCROLLBAR_SETISCROLLBARMGR, OnSetIScrollBarMgr)

	UIALT_MSG_MAP(ALT_MSG_ID_BUTTON1)
		UIMSG_WM_LBUTTONDOWN(OnBtn1LButtonDown)	
		UIMSG_WM_LBUTTONUP(OnBtn1LButtonUp)
		UIMSG_WM_KILLFOCUS(OnBtn1KillFocus)

	UIALT_MSG_MAP(ALT_MSG_ID_BUTTON2)
		UIMSG_WM_LBUTTONDOWN(OnBtn2LButtonDown)	
		UIMSG_WM_LBUTTONUP(OnBtn2LButtonUp)
		UIMSG_WM_KILLFOCUS(OnBtn2KillFocus)

	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SystemScrollBar, IControl)

public:
    void  SetISystemScrollBar(ISystemScrollBar* p) { m_pISystemScrollBar = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    void  SetIScrollBarMgr(IScrollBarManager* p);

	void  CalcScrollBarVisible();
	void  ResetAttribute();
	void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  GetDesiredSize(SIZE* pSize);

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
	void  OnPaint(IRenderTarget*);
	void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
	void  OnStateChanged(UINT nMask);
	void  OnBtn1LButtonDown(UINT nFlags, POINT point);
	void  OnBtn2LButtonDown(UINT nFlags, POINT point);
	void  OnBtn1LButtonUp(UINT nFlags, POINT point);
	void  OnBtn1KillFocus(IObject* pNewFocusObj);
	void  OnBtn2LButtonUp(UINT nFlags, POINT point);
	void  OnBtn2KillFocus(IObject* pNewFocusObj);
    LRESULT  OnScrollBarInfoChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnSetIScrollBarMgr(UINT uMsg, WPARAM wParam, LPARAM lParam);
protected:
	IObject*  GetBindObject();

	bool  CalcThumbButtonPos(bool bNeedUpdateThumbButtonSize); 
	void  CalcChannelRect(CRect* prc);
	int   CalcThumbButtonSize();

    virtual SCROLLBAR_DIRECTION_TYPE  GetDirType() { return VSCROLLBAR; }

private:
	UINT_PTR  m_nTimer1IDToScroll;    // 按住lineup button后，500ms后开始滚动
	UINT_PTR  m_nTimer2IDToScroll;    // 按住linedown button后，500ms后开始滚动
	UINT_PTR  m_nTimer1IDScrolling;   // 按住lineup button后，连续滚动
	UINT_PTR  m_nTimer2IDScrolling;   // 按住linedown button后，连续滚动

protected:
    ISystemScrollBar*   m_pISystemScrollBar;
    IScrollBarManager*  m_pMgrScrollBar;

	IButton*  m_pBtnLineUpLeft;
	IButton*  m_pBtnLineDownRight;
	IButton*  m_pBtnThumb;

	// 用于检测数据发生改变时，刷新thumbbtn的位置
	int  m_nOldPage;
	int  m_nOldRange;
	int  m_nOldPos;

	int   m_nClickDiff;   // 鼠标在Thumb btn按下时的位置标识
	bool  m_bTracking;    // 是否正在拖拽Thumb btn滚动
};  


class SystemVScrollBar : public SystemScrollBar
{
public:
    SystemVScrollBar();

    UI_DECLARE_OBJECT3(SystemVScrollBar, OBJ_CONTROL|CONTROL_VSCROLLBAR, _T("UICtrl/Control"));
    UI_BEGIN_MSG_MAP
        UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_CALCPARENTNONCLIENTRECT(OnCalcParentNonClientRect)
        UIMESSAGE_HANDLER_EX(UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED, OnBindObjSize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    
    UIALT_MSG_MAP(ALT_MSG_ID_THUMB_BTN)
        UIMSG_WM_LBUTTONDOWN(OnThumbBtnLButtonDown)	
        UIMSG_WM_LBUTTONUP(OnThumbBtnLButtonUp)	
        UIMSG_WM_MOUSEMOVE(OnThumbBtnMousemove)

   UI_END_MSG_MAP_CHAIN_PARENT(SystemScrollBar)

public:
    void  SetISystemVScrollBar(ISystemVScrollBar* p) { m_pISystemVScrollBar = p; }
    virtual SCROLLBAR_DIRECTION_TYPE  GetDirType() { return VSCROLLBAR; }

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
    void  OnSize(UINT nType, int cx, int cy);
    void  OnCalcParentNonClientRect(CRegion4* prcNonClientRegion);
    LRESULT  OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);

    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);

    int   WindowPoint2TrackPos(int nUIPos);
    void  OnThumbBtnLButtonDown(UINT nFlags, POINT point);
    void  OnThumbBtnLButtonUp(UINT nFlags, POINT point);
    void  OnThumbBtnMousemove(UINT nFlags, POINT point);

private:
    ISystemVScrollBar*  m_pISystemVScrollBar;
};
class SystemHScrollBar : public SystemScrollBar
{
public:
	SystemHScrollBar();
    UI_DECLARE_OBJECT3(SystemHScrollBar, OBJ_CONTROL|CONTROL_HSCROLLBAR, _T("UICtrl/Control"));

	UI_BEGIN_MSG_MAP
		UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_LBUTTONUP(OnLButtonUp)
		UIMSG_WM_CALCPARENTNONCLIENTRECT(OnCalcParentNonClientRect)
        UIMESSAGE_HANDLER_EX(UI_WM_SCROLLBAR_BINDOBJSIZE_CHANGED, OnBindObjSize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)

	UIALT_MSG_MAP(ALT_MSG_ID_THUMB_BTN)
		UIMSG_WM_LBUTTONDOWN(OnThumbBtnLButtonDown)	
		UIMSG_WM_LBUTTONUP(OnThumbBtnLButtonUp)	
		UIMSG_WM_MOUSEMOVE(OnThumbBtnMousemove)

	UI_END_MSG_MAP_CHAIN_PARENT(SystemScrollBar)

public:
    void  SetISystemHScrollBar(ISystemHScrollBar* p) { m_pISystemHScrollBar = p; }
    virtual SCROLLBAR_DIRECTION_TYPE  GetDirType() { return HSCROLLBAR; }

protected:
    HRESULT  FinalConstruct(IUIApplication* p);
	void  OnSize(UINT nType, int cx, int cy);
	void  OnCalcParentNonClientRect(CRegion4* prcNonClientRegion);
	LRESULT  OnBindObjSize(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);

	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);
	int   WindowPoint2TrackPos(int nUIPos);

	void  OnThumbBtnLButtonDown(UINT nFlags, POINT point);
	void  OnThumbBtnLButtonUp(UINT nFlags, POINT point);
	void  OnThumbBtnMousemove(UINT nFlags, POINT point);

private:
    ISystemHScrollBar*  m_pISystemHScrollBar;
};

}
