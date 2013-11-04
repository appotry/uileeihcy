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
// ��ʾ���Ĺ���
// ���ڹ�����ּ�ʱ������Ϣת������ʵ�ָ����߼���ʹ��IToolTipUI�Ϳؼ�����ȥ����Щ�߼�
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
	IToolTipUI*        m_pToolTipUI;      // ���ǿؼ���tooltip
//	list<IToolTipUI*>  m_listMessageBox;  // ������ʵ�����ڵ���ʾ��ָ���б���messagebox tooltip.Ŀǰδ����...
	UINT_PTR           m_nTimerWait2Show; // ��ʱ��ID
	TOOLTIPITEM        m_tooltipItem;     // ��ǰ������ʾ�������ڵȴ���ʾ����ʾ����Ϣ

	CTimerProcThunk    m_thunk;
    UIApplication*     m_pUIApplication;
};

}

#endif // TOOLTIPMANAGER_H_8861C608_0281_42fc_B665_0F005E07D027