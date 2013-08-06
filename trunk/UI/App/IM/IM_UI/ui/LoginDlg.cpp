#include "StdAfx.h"
#include "LoginDlg.h"
#include "..\Resource.h"
#include <time.h>
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Control\Inc\Interface\iflashwrap.h"
#include "App\IM\IM_Ctrls\inc\iqqloginaccountcombobox.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"

#define WINDOW_3D_ROTATE_DURATION  300   // 窗口翻转90度的时间
#define TIMELINE_ID_GO_STEP1      1
#define TIMELINE_ID_GO_STEP2      3
#define TIMELINE_ID_BACK_STEP1    5
#define TIMELINE_ID_BACK_STEP2    7

#define SHOW_HIDE_DURATION          400
#define TIMELINE_ID_SHOW            9
#define TIMELINE_ID_HIDE_CLOSE      10
#define TIMELINE_ID_HIDE_LOGINSUCCESS  11

#define TIMER_ID_LOGING_TEXT_TRAIL_POINT 12

CLoginDlg::CLoginDlg(void)
{
	m_pBtnSet = NULL;
	m_pBtnLoginState = NULL;
	m_pTrayIcon = NULL;
    m_pFlashCtrl = NULL;
    m_pFlashLayer = NULL;
	m_pAccoutCombobox = NULL;
	m_pBtnNetSetCancel = NULL;
	m_pPanelLogin = NULL;
	m_pBtnLogon = NULL;
	m_pPanelLogin = NULL;
    m_pPanelLoginInput = NULL;
    m_pPanelLogining = NULL;
    m_pGifLoadding = NULL;
    m_pBtnLogonCacel = NULL;
    m_nTrailPointCount = 1;
    m_pLabelLoginingText = NULL;
}

CLoginDlg::~CLoginDlg(void)
{
	SAFE_RELEASE(m_pTrayIcon);
}

void CLoginDlg::OnInitWindow()
{
    ICustomWindow::xProcessMessage(GetCurMsg(), 0, 0);

    m_pAccoutCombobox = (IQQLoginAccountComboBox*)FindChildObject(_T("combo_account"));
    m_pPanelLogin = (UI::IPanel*)FindChildObject(_T("panel_login"));
	if (m_pPanelLogin)
	{
        m_pPanelLoginInput = (UI::IPanel*)m_pPanelLogin->FindChildObject(_T("panel_login_input"));
        m_pPanelLogining = (UI::IPanel*)m_pPanelLogin->FindChildObject(_T("panel_logining"));
        m_pBtnSet  = (UI::IButton*)m_pPanelLogin->FindChildObject(_T("btn_setting"));

        if (m_pPanelLoginInput)
        {
		    m_pBtnLoginState = (UI::IButton*)m_pPanelLoginInput->FindChildObject(_T("btn_onlinestate"));
		    m_pBtnLogon  = (UI::IButton*)m_pPanelLoginInput->FindChildObject(_T("btn_login"));
        }
        if (m_pPanelLogining)
        {
            m_pBtnLogonCacel = (UI::IButton*)m_pPanelLogining->FindChildObject(_T("btn_login_cancel"));
            m_pGifLoadding = (UI::IGifCtrl*)m_pPanelLogining->FindChildObject(_T("gif_logining_loading"));
            m_pLabelLoginingText = (UI::ILabel*)m_pPanelLogining->FindChildObject(_T("logining_text"));
        }
	}
	m_pPanelNetSet = (UI::IPanel*)FindChildObject(_T("panel_login_setting"));
	if (m_pPanelNetSet)
	{	
		m_pBtnNetSetCancel = (UI::IButton*)m_pPanelNetSet->FindChildObject(_T("btn_login.netset.cancel"));
	}

	::SetWindowText(GetHWND(), _T("IM_UI"));

	// 在Win7 64位下面不调用SetIcon也能正确显示，但在win7 32下面却不能显示
	HICON hIconBig = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_IM_UI));
	HICON hIconSmall = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SMALL));
	SendMessage(GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
	SendMessage(GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
	DestroyIcon(hIconBig);
	DestroyIcon(hIconSmall);

	// 创建托盘
    UI::UICreateTrayIcon(g_pUIApp, &m_pTrayIcon);
	HICON hIcon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_IM_UI));
	m_pTrayIcon->AddTrayIcon(GetHWND(), hIcon, L"IM_UI");


    // 初始化登录账号
	TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
	String str = szPath;
	str += _T("im\\skin\\2012\\img\\common\\qq_100_100.gif");
	if (m_pAccoutCombobox)
	{
		m_pAccoutCombobox->AddAccount(_T("111111111"), _T("leeihcy"), str.c_str());
		m_pAccoutCombobox->AddAccount(_T("222222222"), _T("leeihcy"), str.c_str());
  		m_pAccoutCombobox->AddAccount(_T("333333333"), _T("leeihcy"), str.c_str());
//		m_pAccoutCombobox->AddAccount(_T("444444444"), _T("leeihcy"), str.c_str());

		m_pAccoutCombobox->SetCurSel(0);
	}

    this->InitFlash();
}

void  CLoginDlg::InitFlash()
{
    // 设置背景flash
    UI::IRenderChain*  pRenderChain = GetIRenderChain();
    if (!pRenderChain)
        return;

    m_pFlashLayer = (UI::IBufferRenderLayer*)pRenderChain->FindLayer(_T("flash_bk"));
    if (!m_pFlashLayer)
        return;

    m_pFlashCtrl = (UI::IFlashWrap*)m_pFlashLayer->FindChildObject(_T("flash"));
    if (!m_pFlashCtrl)
        return;

    time_t t;
    time(&t);
    tm* ptm = localtime(&t);

    if (ptm->tm_hour < 6)
        m_pFlashCtrl->SetFlashUri(_T("im\\skin\\2012\\img\\login\\night.swf"));
    else if (ptm->tm_hour < 9)
        m_pFlashCtrl->SetFlashUri(_T("im\\skin\\2012\\img\\login\\morning.swf"));
    else if (ptm->tm_hour < 17)
        m_pFlashCtrl->SetFlashUri(_T("im\\skin\\2012\\img\\login\\noon.swf"));
    else if (ptm->tm_hour < 19)
        m_pFlashCtrl->SetFlashUri(_T("im\\skin\\2012\\img\\login\\afternoon.swf"));
    else
        m_pFlashCtrl->SetFlashUri(_T("im\\skin\\2012\\img\\login\\night.swf"));
}

LRESULT  CLoginDlg::OnShowModal(UINT, WPARAM, LPARAM)
{
    AnimateShowWindow(TIMELINE_ID_SHOW);
    return 1;
}
void  CLoginDlg::OnSysCommand(UINT nID, CPoint point)
{
    if (SC_CLOSE != nID)
    {
        SetMsgHandled(FALSE);
        return;
    }

    AnimateHideWindow(TIMELINE_ID_HIDE_CLOSE);
    return;
}
void  CLoginDlg::AnimateHideWindow(int nId)
{
    UI::IAnimateManager* pAnimateMgr = g_pUIApp->GetAnimateMgr();
    UI::IWindowUpDownAlphaShowAnimate* p = static_cast<UI::IWindowUpDownAlphaShowAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_UPDOWNALPHASHOW, static_cast<UI::IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->Hide(nId, SHOW_HIDE_DURATION);
}
void  CLoginDlg::AnimateShowWindow(int nId)
{
    UI::IAnimateManager* pAnimateMgr = g_pUIApp->GetAnimateMgr();
    UI::IWindowUpDownAlphaShowAnimate* p = static_cast<UI::IWindowUpDownAlphaShowAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_UPDOWNALPHASHOW, static_cast<UI::IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->Show(nId, SHOW_HIDE_DURATION);
}

void CLoginDlg::OnBtnNetSetCancel()
{
    UI::IAnimateManager* pAnimateMgr = g_pUIApp->GetAnimateMgr();
    UI::IWindow3DAnimate* p = static_cast<UI::IWindow3DAnimate*>(pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_3D_ROTATE, static_cast<UI::IWindowBase*>(this)));
	if (NULL == p)
		return;

    p->SetNotifyObj(static_cast<UI::IMessage*>(this));
    p->SetID(TIMELINE_ID_BACK_STEP1);

	UI::IIntTimeline* pTimelineY = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
	pTimelineY->SetFromTo(360, 270);
	pTimelineY->SetDuration(WINDOW_3D_ROTATE_DURATION);
	pTimelineY->SetSpeed(-300);
	pTimelineY->SetOutRef(p->GetRotateYPtr());
    pTimelineY->SetID(0);
	p->AddTimeline(pTimelineY);

	UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
	pTimelineZ->SetFromTo(0, 500);
	pTimelineZ->SetDuration(WINDOW_3D_ROTATE_DURATION);
	//	pTimelineZ->SetSpeed(300);
	pTimelineZ->SetOutRef(p->GetOffsetZPtr());
    pTimelineZ->SetID(1);
	p->AddTimeline(pTimelineZ);
    p->Begin();

	return;
}

void CLoginDlg::OnBtnSet()
{
	if (m_pFlashLayer)
		m_pFlashLayer->SetCanRedraw(false);

    UI::IAnimateManager* pAnimateMgr = g_pUIApp->GetAnimateMgr();
	UI::IWindow3DAnimate* p = static_cast<UI::IWindow3DAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_3D_ROTATE, static_cast<IWindowBase*>(this)));
	if (NULL == p)
		return;
    p->SetNotifyObj(static_cast<IMessage*>(this));
    p->SetID(TIMELINE_ID_GO_STEP1);

	UI::IIntTimeline* pTimelineY = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
	pTimelineY->SetFromTo(0, 90);
	pTimelineY->SetDuration(WINDOW_3D_ROTATE_DURATION);
	pTimelineY->SetSpeed(300);
    pTimelineY->SetID(0);
	pTimelineY->SetOutRef(p->GetRotateYPtr());
	p->AddTimeline(pTimelineY);

	UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
	pTimelineZ->SetFromTo(0, 500);
	pTimelineZ->SetDuration(WINDOW_3D_ROTATE_DURATION);
	pTimelineZ->SetOutRef(p->GetOffsetZPtr());
    pTimelineZ->SetID(1);
	p->AddTimeline(pTimelineZ);
    p->Begin();
}
void CLoginDlg::OnBtnLogon()
{
    if (NULL == m_pAccoutCombobox)
        return;
    const TCHAR* szAccount = m_pAccoutCombobox->GetText();
    if (NULL == szAccount)
        return;

    if (0 == _tcslen(szAccount))
        return;

    m_strLoginingText = _T("正在登录(");
    m_strLoginingText.append(szAccount);
    m_strLoginingText.append(_T(")."));

    if (NULL == m_pPanelLogining || NULL == m_pPanelLoginInput)
        return;

	if (m_pFlashCtrl)
	{ 
        m_pFlashCtrl->CallFlashFunction(_T("logon"), _T("<true/>"), NULL);
	}
    
    m_pPanelLoginInput->SetVisible(false, false, false);
    m_pPanelLogining->SetVisible(true, false, true);

    if (m_pLabelLoginingText)
        m_pLabelLoginingText->SetText(m_strLoginingText.c_str());

    // 显示gif
    if (m_pGifLoadding)
        m_pGifLoadding->Start();

    // 设置定时器显示...
    UI::TimerItem  ti;
    ti.nId = TIMER_ID_LOGING_TEXT_TRAIL_POINT;
    ti.pNotify = this;
    GetUIApplication()->SetTimer(800, &ti);

    this->UpdateObject();
}
void  CLoginDlg::OnBtnLogonCancel()
{
    if (NULL == m_pPanelLogining || NULL == m_pPanelLoginInput)
        return;

    if (m_pFlashCtrl)
    { 
        m_pFlashCtrl->CallFlashFunction(_T("logon"), _T("<false/>"), NULL);
    }

    m_pPanelLogining->SetVisible(false, false, false);
    m_pPanelLoginInput->SetVisible(true, false, true);

    if (m_pGifLoadding)
        m_pGifLoadding->Stop();
    GetUIApplication()->KillTimerById(TIMER_ID_LOGING_TEXT_TRAIL_POINT, this);
    m_nTrailPointCount = 1;

    this->UpdateObject();
}

void  CLoginDlg::OnLogonSuccess()
{
    m_nTrailPointCount = 0;
    m_strLoginingText.clear();

    if (m_pGifLoadding)
        m_pGifLoadding->Stop();
    GetUIApplication()->KillTimerById(TIMER_ID_LOGING_TEXT_TRAIL_POINT, this);

    AnimateHideWindow(TIMELINE_ID_HIDE_LOGINSUCCESS);
}

void  CLoginDlg::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    m_nTrailPointCount++;
    if (3 == m_nTrailPointCount)
    {
        this->OnLogonSuccess();
        return;
    }

    int n = m_nTrailPointCount%3 + 1;
    String  str = m_strLoginingText;
    switch (n)
    {
    case 1:
        {
            m_pLabelLoginingText->SetText(str.c_str());
        }
        break;

    case 2:
        {
            str.append(_T("."));
            m_pLabelLoginingText->SetText(str.c_str());
        }
        break;

    case 3:
        {
            str.append(_T(".."));
            m_pLabelLoginingText->SetText(str.c_str());
        }
        break;
    }
}
void CLoginDlg::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
	for (int i = 0; i < nCount; i++)
	{
		UI::IStoryboard* pStoryboard = ppArray[i];
	
        int nStoryboardId = pStoryboard->GetID();
		if (nStoryboardId== TIMELINE_ID_GO_STEP1)
		{
			this->OnAnimateTick_GoStep1(pStoryboard); 
            break;  // break是为了防止将Window3DAnimate删除了，但下一个循环中又去访问导致崩溃
		}
		else if (nStoryboardId == TIMELINE_ID_BACK_STEP1)
		{
			this->OnAnimateTick_BackStep1(pStoryboard);
            break;
		}
        else if (nStoryboardId == TIMELINE_ID_BACK_STEP2)
        {
            this->OnAnimateTick_BackStep2(pStoryboard);
            break;
        }
        else if (nStoryboardId == TIMELINE_ID_HIDE_LOGINSUCCESS || 
            nStoryboardId == TIMELINE_ID_HIDE_CLOSE)
        {
            this->OnAnimateTick_Hide(pStoryboard);
        }
        else if (nStoryboardId == TIMELINE_ID_SHOW)
        {
            this->OnAnimateTick_Show(pStoryboard);
        }
	}
}

// 90度翻转结束后，切到设置面板
void  CLoginDlg::OnAnimateTick_GoStep1(UI::IStoryboard* pStoryboard)
{
    if (!pStoryboard->IsFinish())
        return;

    // 关闭Flash刷新
    if (m_pFlashLayer)
        m_pFlashLayer->SetLayerEnable(false);

    // 切换面板，切换中会涉及到焦点变化，导致窗口刷新，因此将窗口刷新禁掉
    GetRenderChain()->SetCanCommit(false);
    if (m_pPanelNetSet)
        m_pPanelNetSet->SetVisible(true, false);

    if (m_pPanelLogin)
        m_pPanelLogin->SetVisible(false, false);
    GetRenderChain()->SetCanCommit(true);

    // 只绘制，但不提交到窗口上面
    this->UpdateObject(false);

    UI::IAnimateManager* pAnimateMgr = g_pUIApp->GetAnimateMgr();
    UI::IWindow3DAnimate* p = static_cast<UI::IWindow3DAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_3D_ROTATE, static_cast<IWindowBase*>(this)));
    if (NULL == p)
        return;
    p->SetNotifyObj(static_cast<IMessage*>(this));
    p->SetID(TIMELINE_ID_GO_STEP2);

    UI::IIntTimeline* pTimelineY = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
    pTimelineY->SetFromTo(270, 360);
    pTimelineY->SetDuration(WINDOW_3D_ROTATE_DURATION);
    pTimelineY->SetSpeed(400);
    pTimelineY->SetOutRef(p->GetRotateYPtr());
    pTimelineY->SetID(0);
    p->AddTimeline(pTimelineY);

    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
    pTimelineZ->SetFromTo(500, 0);
    pTimelineZ->SetDuration(WINDOW_3D_ROTATE_DURATION);
    //	pTimelineZ->SetSpeed(500);
    pTimelineZ->SetOutRef(p->GetOffsetZPtr());
    pTimelineZ->SetID(1);
    p->AddTimeline(pTimelineZ);
    p->Begin();
}
// 90度回翻结束后，切回登录面板
void  CLoginDlg::OnAnimateTick_BackStep1(UI::IStoryboard* pStoryboard)
{
    if (!pStoryboard->IsFinish())
        return;

    GetRenderChain()->SetCanCommit(false);
    if (m_pPanelNetSet)
        m_pPanelLogin->SetVisible(true, false);
    if (m_pPanelLogin)
        m_pPanelNetSet->SetVisible(false, false);
    GetRenderChain()->SetCanCommit(true);

    if (m_pFlashLayer)
    {
        m_pFlashLayer->SetCanRedraw(true);
        m_pFlashLayer->SetLayerEnable(true);
    }

    // 只绘制，但不提交到窗口上面
    this->UpdateObject(false);

    UI::IAnimateManager* pAnimateMgr = g_pUIApp->GetAnimateMgr();
    UI::IWindow3DAnimate* p = static_cast<UI::IWindow3DAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_3D_ROTATE, static_cast<IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->SetNotifyObj(static_cast<IMessage*>(this));
    p->SetUserData((LPARAM)p);  // 用于在回翻过程中不断更新动画图片
    p->SetID(TIMELINE_ID_BACK_STEP2);

    UI::IIntTimeline* pTimelineY = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
    pTimelineY->SetFromTo(90, 0);
    pTimelineY->SetDuration(WINDOW_3D_ROTATE_DURATION);
    //	pTimelineY->SetSpeed(-400);
    pTimelineY->SetID(TIMELINE_ID_BACK_STEP2);
    pTimelineY->SetOutRef(p->GetRotateYPtr());
    pTimelineY->SetID(0);
    p->AddTimeline(pTimelineY);

    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)pAnimateMgr->CreateTimelineInstance(UI::E_TIMELINE_INT);
    pTimelineZ->SetFromTo(500, 0);
    pTimelineZ->SetDuration(WINDOW_3D_ROTATE_DURATION);
    //	pTimelineZ->SetSpeed(500);
    pTimelineZ->SetOutRef(p->GetOffsetZPtr());
    pTimelineZ->SetID(1);
    p->AddTimeline(pTimelineZ);
    p->Begin();

    if (m_pFlashCtrl)
        m_pFlashCtrl->UpdateObject(false);
}
// 在最后翻转回登录面板的过程中，动画前面有一段时间差，导致动画结束后flash画面衔接不连贯
// 因此在这段动画过程中，需要不停的去更新动画源数据
void  CLoginDlg::OnAnimateTick_BackStep2(UI::IStoryboard* pStoryboard)
{
    UI::IWindow3DAnimate* pWindow3DAnimate = (UI::IWindow3DAnimate*)pStoryboard->GetUserData();
    if (pWindow3DAnimate && !pStoryboard->IsFinish())
    {
        // flash动画仍然在继续播放并更新双缓冲，但窗口仍然是被SetCanRedraw(FALSE)
        pWindow3DAnimate->UpdateWindowSourceImage();
    }
}

void  CLoginDlg::OnAnimateTick_Hide(UI::IStoryboard* p)
{
    if (p->IsFinish())
    {
        if (p->GetID() == TIMELINE_ID_HIDE_LOGINSUCCESS)
            this->EndDialog(IDOK);
        else
            this->EndDialog(0);
    }
}
void  CLoginDlg::OnAnimateTick_Show(UI::IStoryboard* p)
{
    
}

void CLoginDlg::OnAnimateOver()
{
	 this->UpdateObject();
}
void CLoginDlg::OnPaint(UI::IRenderTarget* pRenderTarget)
{
	return;
}

void CLoginDlg::OnBtnClickLoginStatus()
{
    UI::IMenu* pMenu = NULL;
    UI::UILoadMenu(GetUIApplication(), _T("login.status"), &pMenu);
    if (NULL == pMenu)
        return;

	CRect rc;
	m_pBtnLoginState->GetWindowRect(&rc);
	::MapWindowPoints(GetHWND(), NULL, (LPPOINT)&rc, 2);

	int nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, static_cast<IMessage*>(this));
	SAFE_DELETE_Ixxx(pMenu);
}
LRESULT CLoginDlg::OnTrayIconNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (lParam == WM_CONTEXTMENU)
	{
        UI::IMenu* pMenu = NULL;
        UILoadMenu(g_pUIApp, _T("login.status"), &pMenu);

		CPoint pt;
		::GetCursorPos(&pt);
		::SetForegroundWindow(GetHWND());	// 如果不加这句话，不选择菜单项的话，菜单不会消失

		int nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, static_cast<IMessage*>(this));
		SAFE_DELETE_Ixxx(pMenu);
	}
	SetMsgHandled(FALSE);

	return 0;
}
