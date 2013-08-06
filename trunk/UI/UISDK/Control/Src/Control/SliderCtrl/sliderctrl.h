#pragma once
#include "UISDK\Control\Inc\Interface\isliderctrl.h"

namespace UI
{
interface IButton;

// 滑块控件(带按钮的滚动条)
class SliderCtrl : public MessageProxy
{
public:
	SliderCtrl();
	~SliderCtrl();

    UI_DECLARE_OBJECT3(SliderCtrl, OBJ_CONTROL|CONTROL_SLIDERCTRL, _T("UICtrl/Control"))

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_KEYDOWN(OnKeyDown)
		UIMSG_WM_KEYUP(OnKeyUp)
		UIMSG_WM_STATECHANGED2(OnStateChanged)
		UIMSG_WM_STYLECHANGED(OnStyleChanged)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
		// 按钮的事件
	UIALT_MSG_MAP(1)   
		UIMSG_WM_LBUTTONDOWN(OnBtnLButtonDown)	
		UIMSG_WM_MOUSEMOVE(OnBtnMouseMove)	
		UIMSG_WM_LBUTTONUP(OnBtnLButtonUp)
		UIMSG_WM_KEYDOWN(OnKeyDown)
		UIMSG_WM_KEYUP(OnKeyUp)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SliderCtrl, IControl)

public:
    void  SetISliderCtrl(ISliderCtrl* p) { m_pISliderCtrl = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

    HRESULT  FinalConstruct(IUIApplication* p);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);

    int   SetPos(int nPos, bool bUpdate=true);
    int   SetPage(int nPage, bool bUpdate=true);
    void  SetRange(int nLower, int nUpper, bool bUpdate=true);
    void  SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

    void  SetLine(int nLine){ m_nLine = nLine; }
    int   GetPos(){ return m_nCur; }
    int   GetPage(){ return m_nPage; }
    int   GetLine(){ return m_nLine; }
    void  GetRange(int& nLower, int& nUpper){ nLower = m_nMin; nUpper = m_nMax; }
    int   GetRange() { return m_nMax - m_nMin; }

    void  SetDirectionType(PROGRESS_SCROLL_DIRECTION_TYPE eType) { m_eDirectionType = eType; }
    PROGRESS_SCROLL_DIRECTION_TYPE  GetDirectionType() { return m_eDirectionType; }

	void  UpdateButtonRect();
	int   SetTickFreq(int nFreq);

protected:
	void  OnPaint(IRenderTarget* pRenderTarget);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnBtnLButtonDown(UINT nFlags, POINT point);
	void  OnBtnMouseMove(UINT nFlags, POINT point);
	void  OnBtnLButtonUp(UINT nFlags, POINT point);
	void  OnMove(POINT pt );
	void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags );
	void  OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnSize(UINT nType, int cx, int cy );
	void  OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct);
	void  OnStateChanged(UINT nMask);

	void  UpdateTicksData();
	void  UpdateUIData(bool bCalBeginEnd=false, bool bUpdateButtonRect=true);
	void  CalcCurPosByUITrackPos(int nWindowPos);

	int   WindowPoint2UITrackPos(const POINT* ptWindow);
	
    int   FixCurValue(int nCur);

protected:
    ISliderCtrl*  m_pISliderCtrl;

    int  m_nMin;        // 最小值
    int  m_nMax;        // 最大值
    int  m_nCur;        // 当前位置
    int  m_nLine;       // 每次按键时，滚动的距离
    int  m_nPage;       // 一个页面的大小
    PROGRESS_SCROLL_DIRECTION_TYPE   m_eDirectionType;       // 滚动条方向,类型

	// 界面显示数据
	int  m_nBegin;     // 对应m_nMin，进度条开始绘制的坐标，PADDING + half of button size
	int  m_nEnd;       // 对应m_nMax，进度条结束绘制的地方，Control Size - (PADDING + half of button size )
	int  m_nTrack;     // 对应m_nCur，当前按钮所在的坐标，middle of button

	// show
	IButton*  m_pButton;    // 拖拽用的按钮控件，子类负责实例化
	int  m_nDiffFix;   // 误差消除，拖动时，鼠标离core m_nTrack的偏差

	// 刻度绘制相关
	IRenderBase*  m_pTickRender; // 刻度绘制
	int  m_nFreq;       // auto tick时，指定显示刻度的频率
	struct TickInfo
	{
		int    nPos;              // 显示哪一个刻度
		CRect  rcRect;            // 该刻度最后要显示位置，（在大小、参数改变后，需要重新计算）
	};
	vector<TickInfo*>  m_vTickInfo;
};
}