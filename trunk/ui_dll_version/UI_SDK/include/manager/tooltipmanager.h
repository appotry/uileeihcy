#pragma once

namespace UI
{
#define TOOL_TIMER_TIME    600
#define TOOLTIP_MAX_WIDTH  256

//
// ������ʾ�����Ƶĳ�����	
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
	TOOLTIP_ACTION_FLAG_NORMAL = 0,              // ����ϵͳ��ʾ���������ؼ�ʱ����ʱ��ʾ���ƿ��ؼ�ʱ����������
	TOOLTIP_ACTION_FLAG_CAN_MOUSE_HOVER = 1,     // �����ؼ�ʱ����ʱ��ʾ���ƿ��ؼ�ʱ����ʱ���أ���������ʾ�������Ǳ�����ʾ
	TOOLTIP_ACTION_FLAG_INPLACE = 2,             // ������ʾ�б�ؼ�һ����ʾ���µ����֣�������ʾ����������
	TOOLTIP_ACTION_FLAG_MESSAGEBOX = 4,          // ���ڴ���/��Ϣ��ʾ�������������������ʾ���ܹ����洰���ƶ���һ���Ǵ�����ͷ������
};

// Show�Ĳ���
struct TOOLTIPITEM
{
	TOOLTIPITEM() { memset(this, 0, sizeof(TOOLTIPITEM)); }

	Object*   pNotifyObj;   // Ҫ��ʾ��ʾ���Ŀؼ����Լ���Ϣ֪ͨ�Ķ���
	void*     pItemData;    // �����Ҫ��ʾ�ؼ��ڲ�һ��item����ʾ��Ϣ���ɽ���ֵ����Ϊ���ʵ�ֵ��Ȼ��mgr����pNotifyObj����UI_WM_GET_TOOLTIPINFO��Ϣ
	int       nToolTipFlag; // ��ʾ������Ϊ��־�� ��20120928 ����ֻ֧��normal��
};

//
// ��ʾ���Ĺ���
// ���ڹ�����ּ�ʱ������Ϣת������ʵ�ָ����߼���ʹ��IToolTipUI�Ϳؼ�����ȥ����Щ�߼�
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
	IToolTipUI*        m_pToolTipUI;      // ���ǿؼ���tooltip
	list<IToolTipUI*>  m_listMessageBox;  // ������ʵ�����ڵ���ʾ��ָ���б���messagebox tooltip
	UINT_PTR           m_nTimerWait2Show; // ��ʱ��ID
	TOOLTIPITEM        m_tooltipItem;     // ��ǰ������ʾ�������ڵȴ���ʾ����ʾ����Ϣ
};

}