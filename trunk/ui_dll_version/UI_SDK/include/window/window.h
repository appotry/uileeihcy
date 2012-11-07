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
		UIMSG_BN_CLICKED(XML_SYS_BTN_CLOSE,     OnSysClose )
		UIMSG_BN_CLICKED(XML_SYS_BTN_MINIMIZE,  OnSysMinimize )
		UIMSG_BN_CLICKED2(m_pBtnSystemMaximize, OnSysMaximize )
		UIMSG_BN_CLICKED2(m_pBtnSystemRestore,  OnSysRestore )
		UIMSG_WM_SKINCHANGED(_OnSkinChanged)
		UIMSG_WM_LBUTTONDBLCLK(_OnLButtonDblClk)
		UIMSG_WM_SYSCOMMAND(OnSysCommand)
	UI_END_MSG_MAP_CHAIN_PARENT(WindowBase)

	// �ڲ�����
protected:
	virtual  void OnInitWindow();

	void     _OnSkinChanged();
	void     _OnLButtonDblClk(UINT nFlags, POINT point);

	// ģ��ϵͳ��������ť
	void      OnSysClose();
	void      OnSysMinimize();
	void      OnSysMaximize();
	void      OnSysRestore();
	void      HandleSysBtnStyle();
	void      OnSysCommand(UINT nID, CPoint point);

protected:
	Object*   m_pBtnSystemMaximize;
	Object*   m_pBtnSystemRestore;
	Object*   m_pBtnSystemMinimize;
};



}