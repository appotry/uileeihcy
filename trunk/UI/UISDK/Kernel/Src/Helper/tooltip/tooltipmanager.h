#ifndef TOOLTIPMANAGER_H_8861C608_0281_42fc_B665_0F005E07D027
#define TOOLTIPMANAGER_H_8861C608_0281_42fc_B665_0F005E07D027

namespace UI
{
#define TOOL_TIMER_TIME    600
#define TOOLTIP_MAX_WIDTH  500/*256*/


//////////////////////////////////////////////////////////////////////////
// TimerProc thunk

class CTimerProcThunk
{
public:
	CStdCallThunk thunk;

	BOOL Init(TIMERPROC proc, void* pThis)
	{
		return thunk.Init((DWORD_PTR)proc, pThis);
	}
	TIMERPROC  GetTIMERPROC()
	{
		return (TIMERPROC)thunk.GetCodeAddress();
	}
};

//
// 提示条的管理
// 用于管理各种计时器及消息转发，并实现各种逻辑，使得IToolTipUI和控件不用去管这些逻辑
//
class ToolTipManager
{
public:
	ToolTipManager(UIApplication* p);
	~ToolTipManager();

	void   Init(IToolTipUI* pTooltipUI=NULL);
	void   Release();
	bool   Hide();
	bool   Show(TOOLTIPITEM* pItemInfo);
	static VOID CALLBACK ToolTipManagerTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );
	void   OnTimer( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

protected:
	IToolTipUI*        m_pToolTipUI;      // 就是控件的tooltip
//	list<IToolTipUI*>  m_listMessageBox;  // 允许多个实例存在的提示条指针列表，如messagebox tooltip.目前未开发...
	UINT_PTR           m_nTimerWait2Show; // 计时器ID
	TOOLTIPITEM        m_tooltipItem;     // 当前正在显示或者正在等待显示的提示条信息

	CTimerProcThunk    m_thunk;
    UIApplication*     m_pUIApplication;
};

}

#endif // TOOLTIPMANAGER_H_8861C608_0281_42fc_B665_0F005E07D027