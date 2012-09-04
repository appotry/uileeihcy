#pragma once
namespace UI
{
	enum PROGRESS_SCROLL_DIRECTION_TYPE
	{
		PROGRESS_SCROLL_LEFT_2_RIGHT,
		PROGRESS_SCROLL_BOTTOM_2_TOP,
		PROGRESS_SCROLL_RIGHT_2_LEFT,
		PROGRESS_SCROLL_TOP_2_BOTTOM
	};

//
//   不可以拖动的进度条
//
class UIAPI ProgressCtrlBase : public Control
{
public:
	ProgressCtrlBase();

public:
	// 从Object继承来的虚函数
	virtual      bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);

	// 从Control继承过来的虚函数
	virtual      SIZE  GetAutoSize(HRDC hRDC);

	// 自己的虚函数
	virtual     int    SetPos(int nPos, bool bUpdate=true);
	virtual     int    SetPage(int nPage, bool bUpdate=true);
	virtual     void   SetRange(int nLower, int nUpper, bool bUpdate=true);
	virtual     void   SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

public:
	void        SetLine(int nLine){ m_nLine = nLine; }
	int         GetPos(){ return m_nCur; }
	int         GetPage(){ return m_nScrollPage; }
	int         GetLine(){ return m_nLine; }
	void        GetRange(int& nLower, int& nUpper){ nLower = m_nMin; nUpper = m_nMax; }
	int         GetRange() { return m_nMax - m_nMin - m_nScrollPage; }

	void        SetDirectionType(PROGRESS_SCROLL_DIRECTION_TYPE eType) { m_eDirectionType = eType; }
	int         GetProgressStyle();
	void        SetProgressStyle(int n);

	// 属性
protected:

	int         m_nMin;        // 最小值
	int         m_nMax;        // 最大值
	int         m_nCur;        // 当前位置
	int         m_nLine;       // 每次按键时，滚动的距离
	int         m_nScrollPage; // 一个页面的大小，仅对SCROLLBAR类型有效，其它类型时为0

	PROGRESS_SCROLL_DIRECTION_TYPE   m_eDirectionType;       // 滚动条方向,类型
};

// 滑块控件(带按钮的滚动条)
class UIAPI SliderCtrlBase : public ProgressCtrlBase
{
public:
	SliderCtrlBase();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_SIZE(OnSize)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
//		UIMSG_WM_SETCURSOR(OnSetCursor)
		UIMSG_WM_KEYDOWN(OnKeyDown)
		UIMSG_WM_KEYUP(OnKeyUp)

	// 按钮的事件
	UIALT_MSG_MAP(1)   
		UIMSG_WM_LBUTTONDOWN(OnBtnLButtonDown)	
		UIMSG_WM_MOUSEMOVE(OnBtnMouseMove)	
		UIMSG_WM_SETCURSOR(OnSetCursor)
		UIMSG_WM_LBUTTONUP(OnBtnLButtonUp)
		UIMSG_WM_KEYDOWN(OnKeyDown)
		UIMSG_WM_KEYUP(OnKeyUp)

	UI_END_MSG_MAP_CHAIN_PARENT(ProgressCtrlBase)

public:
	virtual       void ResetAttribute();
	virtual       bool SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);

	virtual      int    SetPos(int nPos, bool bUpdate=true);
	virtual      int    SetPage(int nPage, bool bUpdate=true);
	virtual      void   SetRange(int nLower, int nUpper, bool bUpdate=true);
	virtual      void   SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);
	
	void         UpdateButtonRect();

protected:
	void         OnPaint( HRDC hDC );
	void		 OnLButtonDown(UINT nFlags, POINT point);
	void		 OnBtnLButtonDown(UINT nFlags, POINT point);
	void		 OnBtnMouseMove(UINT nFlags, POINT point);
	void         OnBtnLButtonUp(UINT nFlags, POINT point);
	void         OnMove( POINT pt );
	void         OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	void         OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void         OnSize( UINT nType, int cx, int cy );

	void         UpdateUIData(bool bCalBeginEnd=false, bool bUpdateButtonRect=true);
	void         CalcCurPosByUITrackPos(int nWindowPos);

	int          WindowPoint2UITrackPos(const POINT* ptWindow);
protected:
	// 界面显示数据
	int          m_nBegin;     // 对应m_nMin，进度条开始绘制的坐标，PADDING + half of button size
	int          m_nEnd;       // 对应m_nMax，进度条结束绘制的地方，Control Size - ( PADDING + half of button size )
	int          m_nTrack;     // 对应m_nCur，当前按钮所在的坐标，middle of button
	
	// show
	ButtonBase*    m_pButton;     // 拖拽用的按钮控件，子类负责实例化
	int            m_nDiffFix;    // 误差消除，拖动时，鼠标离core m_nTrack的偏差
};

class UIAPI SliderCtrl : public SliderCtrlBase
{
public:
	SliderCtrl();
	~SliderCtrl();

	UI_DECLARE_OBJECT( SliderCtrl, OBJ_CONTROL )

};

}