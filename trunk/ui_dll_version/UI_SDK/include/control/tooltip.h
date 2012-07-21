#pragma once 

//////////////////////////////////////////////////////////////////////////
//
//	提示条控件
//
//	libo 20120315 Create beijing
//
//////////////////////////////////////////////////////////////////////////

//
// 提示条控件基类
//
class _Tooltip : public WindowBase
{
public:
	UI_DECLARE_OBJECT( _Tooltip, OBJ_WINDOW )

	_Tooltip();
	~_Tooltip();

public:

};


#define TIMER_ID_TOOLTIP  1
#define TIMER_DELAY_SHOW  800

//
//	使用定时器来显示和隐藏窗口
//
class Tooltip : public _Tooltip
{
public:
	UI_DECLARE_OBJECT( Tooltip, OBJ_WINDOW )

	UI_BEGIN_MSG_MAP
		UIMSG_WM_TIMER( OnTimer )
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
	UI_END_MSG_MAP

public:
	void SetWindowBase(WindowBase* pWindowBase)
	{
		m_pWindow = pWindowBase;
	}
	void RelayEvent( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		switch(uMsg)
		{
		case WM_MOUSEMOVE:
			{
				OnWindowMouseMove();
			}
			break;
		}
	}
	void OnWindowMouseMove()
	{
		::KillTimer(m_hWnd, TIMER_ID_TOOLTIP);
		if( m_pWindow->GetHoverObject() != NULL )
		{
			::SetTimer( m_hWnd, TIMER_ID_TOOLTIP,TIMER_DELAY_SHOW,NULL);
		}
	}

	 void OnTimer(UINT_PTR nIDEvent)
	 {
		 ::KillTimer(m_hWnd, nIDEvent);
		 Show();
	 }

	 void Create()
	 {
		 __super::Create()
	 }
	 void Show()
	 {
		 if( NULL == m_hWnd )
			 return;
	 }

protected:
	void   OnLButtonDown(UINT nFlags, CPoint point);

	
private:
	WindowBase*   m_pWindow;

};

class TrackTooltip : public _Tooltip
{
public:
	


};

class InplaceTooltip : public _Tooltip
{
}


//////////////////////////////////////////////////////////////////////////

//
//	提示条渲染器
//
class _TooltipRender
{
public:
	
};

class SquareTooltipRender : _TooltipRender
{

};

class RoundTooltipRender : _TooltipRender
{

};

class StemTooltipRender : _TooltipRender
{

};