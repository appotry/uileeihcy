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
	UI_END_MSG_MAP_CHAIN_PARENT(WindowBase)

	// �ڲ�����
protected:
	// ģ��ϵͳ��������ť
	void              OnSysClose();
	void              OnSysMinimize();
	void              OnSysMaximize();
	void              OnSysRestore();
};



}