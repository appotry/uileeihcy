#pragma once

#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"

namespace UI
{

class Window : public WindowBase
{
public:
	Window();
	~Window(void);

	UI_DECLARE_OBJECT3(Window, OBJ_WINDOW, _T("Kernel/Window"))

	UI_BEGIN_MSG_MAP
		UIMSG_BN_CLICKED(XML_SYS_BTN_CLOSE,     OnSysClose )
		UIMSG_BN_CLICKED(XML_SYS_BTN_MINIMIZE,  OnSysMinimize )
		UIMSG_BN_CLICKED2(m_pBtnSystemMaximize, OnSysMaximize )
		UIMSG_BN_CLICKED2(m_pBtnSystemRestore,  OnSysRestore )
		UIMSG_WM_SKINCHANGED(_OnSkinChanged)
		UIMSG_WM_LBUTTONDBLCLK(_OnLButtonDblClk)
		UIMSG_WM_SYSCOMMAND(OnSysCommand)
        UIMSG_WM_CLOSE(OnClose)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
	UI_END_MSG_MAP_CHAIN_PARENT(WindowBase)

    IWindow* GetIWindow() { return m_pIWindow; }
    void SetIWindow(IWindow* p) { m_pIWindow = p; }

	// 内部方法
protected:
	virtual void OnInnerInitWindow();

	void  _OnSkinChanged();
	void  _OnLButtonDblClk(UINT nFlags, POINT point);

	// 模拟系统标题栏按钮
	void  OnSysClose();
	void  OnSysMinimize();
	void  OnSysMaximize();
	void  OnSysRestore();
	void  HandleSysBtnStyle();
	void  OnSysCommand(UINT nID, CPoint point);
    void  OnClose();

protected:
    IWindow*   m_pIWindow;
	IObject*   m_pBtnSystemMaximize;
	IObject*   m_pBtnSystemRestore;
	IObject*   m_pBtnSystemMinimize;
	IObject*   m_pBtnSystemClose;
};



}