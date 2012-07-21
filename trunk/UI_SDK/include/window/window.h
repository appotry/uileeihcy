#pragma once


namespace UI
{
 
class UIAPI Window : public WindowBase
{
public:
	Window(void);
	~Window(void);

	UI_DECLARE_OBJECT( Window, OBJ_WINDOW )

	UI_BEGIN_MSG_MAP
		UIMSG_BN_CLICKED( _T("sys_close"), OnSysClose )
		UIMSG_BN_CLICKED( _T("sys_minimize"), OnSysMinimize )
		UIMSG_BN_CLICKED( _T("sys_maximize"), OnSysMaximize )
		UIMSG_BN_CLICKED( _T("sys_restore"), OnSysRestore )
		UICHAIN_MSG_MAP(WindowBase)
	UI_END_MSG_MAP

	// 内部方法
protected:
	bool              testStateBit( WORD wBit );

	// 模拟系统标题栏按钮
	void              OnSysClose();
	void              OnSysMinimize();
	void              OnSysMaximize();
	void              OnSysRestore();
};



}