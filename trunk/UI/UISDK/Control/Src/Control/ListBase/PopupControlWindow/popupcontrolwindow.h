#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Inc\Interface\iuiinterface.h"

namespace UI
{

class PopupControlWindow : public ICustomWindow, public IPreTranslateMessage
{
public:
    PopupControlWindow();

    UI_BEGIN_MSG_MAP_Ixxx(PopupControlWindow)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_MOUSEACTIVATE(OnMouseActivate)
        UIMSG_WM_ACTIVATEAPP(OnActivateApp)
        UIMSG_WM_DESTROY(OnDestroy)
        UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
        UIMSG_WM_SKINCHANGING(OnSkinChanging)
        UIMSG_WM_INITIALIZE(OnInitialize)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
   UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

protected:
    virtual BOOL  PreTranslateMessage(MSG* pMsg);

    void  OnInitialize();
    BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    void  OnSkinChanging(BOOL* pbChange);
    BOOL  OnEraseBkgnd(IRenderTarget* pRenderTarget);
    void  OnDestroy();
    int   OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message);
    void  OnActivateApp(BOOL bActive, DWORD dwThreadID);
    LRESULT  OnGetGraphicsRenderType();

public:
    void  Create(IObject*  pBindObj, const TCHAR* szId, HWND hParentWnd = NULL);
    void  Show(POINT pt, BOOL bDoModal);
    void  Hide();
    void  Destroy();
	void  SetPopupFromInfo(HWND hWnd, RECT* prcClickInWnd);

protected:
    void   _DestroyPopupWindow();

protected:
    IObject*  m_pObject;   // ����������װ�صĶ���ָ�룬��ListBox* Menu*
    bool      m_bExitLoop; // ���յ�WM_EXITPOPUPLOOP֮�󣬸ñ���Ϊtrue
    bool      m_bMouseIn;  // ����ڴ����ϣ����ڴ���WM_MOUSELEAVE

	// ���ڽ���ڵ����˵�������ť���˵������غ�����ʾ����������
	HWND  m_hWndClickFrom; // ΪNULLʱ��ʾ���ж�
	CRect  m_rcClickFrom;  // ��������
};

class PopupListBoxWindow : public PopupControlWindow
{
public:
    PopupListBoxWindow();
   
    UI_BEGIN_MSG_MAP_Ixxx(PopupListBoxWindow)
        UIMSG_WM_KEYDOWN(OnKeyDown)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UIALT_MSG_MAP(1)
    UI_END_MSG_MAP_CHAIN_PARENT(PopupControlWindow)
 
protected:
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    void  OnInitialize();
    void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

class Menu;
class PopupMenuWindow : public PopupControlWindow
{
public:
    PopupMenuWindow();

    UI_BEGIN_MSG_MAP_Ixxx(PopupMenuWindow)
        UIMSG_WM_INITIALIZE(OnInitialize)
    UI_END_MSG_MAP_CHAIN_PARENT(PopupControlWindow)

protected:
    virtual BOOL  PreTranslateMessage(MSG* pMsg);
    void  OnInitialize();

protected:
    Menu*    m_pMenu;
    POINT    m_ptLastMousePos; // �����������Ӳ˵��������ƶ������ص���ϵͳ��������WM_MOUSEMOVE��Ϣ��
    Menu*    m_pLastHoverMenu; // TODO: ͻȻ���ֵ����⣬����Ƴ��˵����ղ���WM_MOUSELEAVE��Ϣ��
                               // Ŀǰ�Ʋ�����Ϊ���յ�һ������Ƴ���WM_MOUSEMOVE��ֱ��RETURN TRUE�ˣ�����
                               // TrackMouseEventû������MOUSELEAVE�����û�и����ڷ�����Ϣ��
                               // Ϊ�˽�������⣬�������¼һ����һ������µĲ˵���������Ƴ����ֶ�����
                               // WM_MOUSELEAVE
};
}