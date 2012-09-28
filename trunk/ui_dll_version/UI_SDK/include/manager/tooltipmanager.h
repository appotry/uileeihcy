#pragma once

namespace UI
{
#define TOOL_TIMER_TIME    600
#define TOOLTIP_MAX_WIDTH  256

//
// 各种提示条绘制的抽象类	
//
class IToolTipUI
{
public:
	virtual ~IToolTipUI() = 0 {};
	virtual bool  Create()  = 0;
	virtual bool  Destroy() = 0;
	virtual bool  SetText(const String& strText) = 0;
	virtual bool  SetTitle(const String& strText) = 0;
	virtual bool  Show() = 0;
	virtual bool  Hide() = 0;
	virtual bool  SetAttribute(const ATTRMAP& mapAttrib) = 0;
};

enum
{
	TOOLTIP_ACTION_FLAG_NORMAL = 0,              // 正常系统提示条，悬浮控件时，计时显示；移开控件时，立即隐藏
	TOOLTIP_ACTION_FLAG_CAN_MOUSE_HOVER = 1,     // 悬浮控件时，计时显示；移开控件时，计时隐藏；鼠标放在提示窗口上是保持显示
	TOOLTIP_ACTION_FLAG_INPLACE = 2,             // 用于显示列表控件一行显示不下的文字，立即显示，立即隐藏
	TOOLTIP_ACTION_FLAG_MESSAGEBOX = 4,          // 用于错误/信息提示，允许弹出多个，立即显示，能够跟随窗口移动，一般是带根箭头的气泡
};

// Show的参数
struct TOOLTIPITEM
{
	TOOLTIPITEM() { memset(this, 0, sizeof(TOOLTIPITEM)); }

	Object*   pNotifyObj;   // 要显示提示条的控件，以及消息通知的对象
	void*     pItemData;    // 如果是要显示控件内部一个item的提示信息，可将该值设置为合适的值，然后mgr会向pNotifyObj发送UI_WM_GET_TOOLTIPINFO消息
	int       nToolTipFlag; // 提示条的行为标志。 （20120928 现在只支持normal）
};

//
// 提示条的管理
// 用于管理各种计时器及消息转发，并实现各种逻辑，使得IToolTipUI和控件不用去管这些逻辑
//
class ToolTipManager
{
public:
	ToolTipManager();
	~ToolTipManager();

	void   Init();
	bool   Hide();
	bool   Show(TOOLTIPITEM* pItemInfo);
	static VOID CALLBACK ToolTipManagerTimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );
	void   OnTimer( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime );

protected:
	IToolTipUI*        m_pToolTipUI;      // 就是控件的tooltip
	list<IToolTipUI*>  m_listMessageBox;  // 允许多个实例存在的提示条指针列表，如messagebox tooltip
	UINT_PTR           m_nTimerWait2Show; // 计时器ID
	TOOLTIPITEM        m_tooltipItem;     // 当前正在显示或者正在等待显示的提示条信息
};

}