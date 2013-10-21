#include "StdAfx.h"
#include "LoginDlg.h"
#include <time.h>
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Control\Inc\Interface\iflashwrap.h"
#include "App\IM\IM_Ctrls\inc\iqqloginaccountcombobox.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "LoginUI.h"

#define WINDOW_3D_ROTATE_DURATION  300   // 窗口翻转90度的时间
#define TIMELINE_ID_GO_STEP1      1
#define TIMELINE_ID_GO_STEP2      3
#define TIMELINE_ID_BACK_STEP1    5
#define TIMELINE_ID_BACK_STEP2    7

#define SHOW_DURATION          300
#define HIDE_DURATION          300
#define STORYBOARD_ID_SHOW            9
#define TIMELINE_ID_HIDE_QUIT      10
#define TIMELINE_ID_HIDE_DESTROY  11

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
    GetUIApplication()->KillTimerById(TIMER_ID_LOGING_TEXT_TRAIL_POINT,  static_cast<UI::IMessage*>(this));
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
// 	HICON hIconBig = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_IM_UI));
// 	HICON hIconSmall = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SMALL));
// 	SendMessage(GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
// 	SendMessage(GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
// 	DestroyIcon(hIconBig);
// 	DestroyIcon(hIconSmall);

	// 创建托盘
//     UI::UICreateTrayIcon(GetUIApplication(), &m_pTrayIcon);
// 	HICON hIcon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_IM_UI));
// 	m_pTrayIcon->AddTrayIcon(GetHWND(), hIcon, L"IM_UI");


    // 初始化登录账号
	TCHAR szPath[MAX_PATH] = _T("");
    UI::Util::GetAppPath_(szPath);
	String str = szPath;
	str += _T("skin\\2012\\img\\common\\qq_100_100.gif");
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
        m_pFlashCtrl->SetFlashUri(_T("skin\\2012\\img\\login\\night.swf"));
    else if (ptm->tm_hour < 9)
        m_pFlashCtrl->SetFlashUri(_T("skin\\2012\\img\\login\\morning.swf"));
    else if (ptm->tm_hour < 17)
        m_pFlashCtrl->SetFlashUri(_T("skin\\2012\\img\\login\\noon.swf"));
    else if (ptm->tm_hour < 19)
        m_pFlashCtrl->SetFlashUri(_T("skin\\2012\\img\\login\\afternoon.swf"));
    else
        m_pFlashCtrl->SetFlashUri(_T("skin\\2012\\img\\login\\night.swf"));
}

void  CLoginDlg::Show()
{
    AnimateShowWindow(STORYBOARD_ID_SHOW);
}
void  CLoginDlg::Hide(bool bQuitOrDestroy)
{
    if (bQuitOrDestroy)
        AnimateHideWindow(TIMELINE_ID_HIDE_QUIT);
    else
        AnimateHideWindow(TIMELINE_ID_HIDE_DESTROY);
}

LRESULT  CLoginDlg::OnShowModal(UINT, WPARAM, LPARAM)
{
    Show();
    return 1;
}
void  CLoginDlg::OnSysCommand(UINT nID, CPoint point)
{
    if (SC_CLOSE != nID)
    {
        SetMsgHandled(FALSE);
        return;
    }

    Hide(true);
    return;
}
void  CLoginDlg::AnimateHideWindow(int nId)
{
    UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
    UI::IWindowUpDownAlphaShowAnimate* p = static_cast<UI::IWindowUpDownAlphaShowAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_UPDOWNALPHASHOW, static_cast<UI::IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->Hide(nId, HIDE_DURATION);
}
void  CLoginDlg::AnimateShowWindow(int nId)
{
    UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
    UI::IWindowUpDownAlphaShowAnimate* p = static_cast<UI::IWindowUpDownAlphaShowAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_UPDOWNALPHASHOW, static_cast<UI::IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->Show(nId, SHOW_DURATION);
}

void CLoginDlg::OnBtnNetSetCancel()
{
    UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
    UI::IWindow3DAnimate* p = static_cast<UI::IWindow3DAnimate*>(pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_3D_ROTATE, static_cast<UI::IWindowBase*>(this)));
	if (NULL == p)
		return;

    p->SetNotifyObj(static_cast<UI::IMessage*>(this));
    p->SetId(TIMELINE_ID_BACK_STEP1);

    UI::IIntAccelerateMove*  pMoveAlgo = NULL;
    UI::IIntTimeline* pTimelineY = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 0, UI::ITMA_Accelerate, (UI::IMoveAlgorithm**)&pMoveAlgo);
    pMoveAlgo->SetParam1(360, 270, WINDOW_3D_ROTATE_DURATION, -300/1000.0f);
    pTimelineY->SetOutRef(p->GetRotateYPtr());

    UI::IIntLinearMove* pMoveAlgo2 = NULL;
    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo2);
    pMoveAlgo2->SetParam1(0, 500, WINDOW_3D_ROTATE_DURATION);
	pTimelineZ->SetOutRef(p->GetOffsetZPtr());
    p->Begin();

	return;
}

void CLoginDlg::OnBtnSet()
{
	if (m_pFlashLayer)
		m_pFlashLayer->SetCanRedraw(false);

    UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
	UI::IWindow3DAnimate* p = static_cast<UI::IWindow3DAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_3D_ROTATE, static_cast<IWindowBase*>(this)));
	if (NULL == p)
		return;
    p->SetNotifyObj(static_cast<IMessage*>(this));
    p->SetId(TIMELINE_ID_GO_STEP1);

    UI::IIntAccelerateMove* pMoveAlgo = NULL;
    UI::IIntTimeline* pTimelineY = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 0, UI::ITMA_Accelerate, (UI::IMoveAlgorithm**)&pMoveAlgo);
    pMoveAlgo->SetParam1(0, 90, WINDOW_3D_ROTATE_DURATION, 300/1000.0f);
	pTimelineY->SetOutRef(p->GetRotateYPtr());

    UI::IIntLinearMove* pMoveAlgo2 = NULL;
    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo2);
    pMoveAlgo2->SetParam1(0, 500, WINDOW_3D_ROTATE_DURATION);
	pTimelineZ->SetOutRef(p->GetOffsetZPtr());
    p->Begin();
}
void CLoginDlg::OnBtnLogin()
{
//     UI::IStoryboard* pStoryboard = GetUIApplication()->GetAnimateMgr()->CreateStoryboard();
//     UI::IIntLinearMove* pMoveAlgo = NULL;
//     pStoryboard->CreateTimeline(UI::TLV_INT, 0, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo);
//     pMoveAlgo->SetParam1(0,100,100000);
//     pStoryboard->SetNotifyObj(this);
//     pStoryboard->Begin();
   
    if (NULL == m_pAccoutCombobox)
        return;
    const TCHAR* szAccount = m_pAccoutCombobox->GetText();
    if (NULL == szAccount)
        return;

    if (0 == _tcslen(szAccount))
        return;

    String  strPassword;
    m_pLoginUI->Login(szAccount, strPassword.c_str());
    return;
}

void CLoginDlg::ShowLogining(const TCHAR* szAccount)
{
    m_strLoginingText = _T("正在登录(");
    if (NULL == szAccount)
         szAccount = m_pAccoutCombobox->GetText();
    if (szAccount)
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
        m_pLabelLoginingText->SetText(m_strLoginingText.c_str(), false);

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
    m_pLoginUI->CancelLogin();
}

void  CLoginDlg::OnCancelLoginOk()
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
    
    // 重置hover对象
    POINT pt;
    GetCursorPos(&pt);
    ScreenToClient(GetHWND(), &pt);
    ::PostMessage(GetHWND(), WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
}

void  CLoginDlg::OnLoginResult(long lRstCode, const TCHAR* szDesc)
{
    m_nTrailPointCount = 0;
    m_strLoginingText.clear();

    if (m_pGifLoadding)
        m_pGifLoadding->Stop();
    GetUIApplication()->KillTimerById(TIMER_ID_LOGING_TEXT_TRAIL_POINT, this);
}

void  CLoginDlg::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    m_nTrailPointCount++;
//     if (3 == m_nTrailPointCount)
//     {
//         this->OnLogonSuccess();
//         return;
//     }

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
	
        int nStoryboardId = pStoryboard->GetId();
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
        else if (nStoryboardId == TIMELINE_ID_HIDE_DESTROY || 
            nStoryboardId == TIMELINE_ID_HIDE_QUIT)
        {
            this->OnAnimateTick_Hide(pStoryboard);
        }
        else if (nStoryboardId == STORYBOARD_ID_SHOW)
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

    UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
    UI::IWindow3DAnimate* p = static_cast<UI::IWindow3DAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_3D_ROTATE, static_cast<IWindowBase*>(this)));
    if (NULL == p)
        return;
    p->SetNotifyObj(static_cast<IMessage*>(this));
    p->SetId(TIMELINE_ID_GO_STEP2);

    UI::IIntAccelerateMove* pMoveAlgo = NULL;
    UI::IIntTimeline* pTimelineY = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 0, UI::ITMA_Accelerate, (UI::IMoveAlgorithm**)&pMoveAlgo);
    pMoveAlgo->SetParam1(270, 360, WINDOW_3D_ROTATE_DURATION, 400/1000.0f);
    pTimelineY->SetOutRef(p->GetRotateYPtr());

    UI::IIntLinearMove* pMoveAlgo2 = NULL;
    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo2);
    pMoveAlgo2->SetParam1(500, 0, WINDOW_3D_ROTATE_DURATION);
    pTimelineZ->SetOutRef(p->GetOffsetZPtr());
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

    UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
    UI::IWindow3DAnimate* p = static_cast<UI::IWindow3DAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_3D_ROTATE, static_cast<IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->SetNotifyObj(static_cast<IMessage*>(this));
    p->SetWParam((WPARAM)p);  // 用于在回翻过程中不断更新动画图片
    p->SetId(TIMELINE_ID_BACK_STEP2);

    UI::IIntLinearMove* pMoveAlgo = NULL;
    UI::IIntTimeline* pTimelineY = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 0, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo);
    pMoveAlgo->SetParam1(90, 0, WINDOW_3D_ROTATE_DURATION);
    pTimelineY->SetId(TIMELINE_ID_BACK_STEP2);
    pTimelineY->SetOutRef(p->GetRotateYPtr());

    UI::IIntLinearMove* pMoveAlgo2 = NULL;
    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo2);
    pMoveAlgo2->SetParam1(500, 0, WINDOW_3D_ROTATE_DURATION);
    pTimelineZ->SetOutRef(p->GetOffsetZPtr());
    p->Begin();

    if (m_pFlashCtrl)
        m_pFlashCtrl->UpdateObject(false);
}
// 在最后翻转回登录面板的过程中，动画前面有一段时间差，导致动画结束后flash画面衔接不连贯
// 因此在这段动画过程中，需要不停的去更新动画源数据
void  CLoginDlg::OnAnimateTick_BackStep2(UI::IStoryboard* pStoryboard)
{
    UI::IWindow3DAnimate* pWindow3DAnimate = (UI::IWindow3DAnimate*)pStoryboard->GetWParam();
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
        if (p->GetId() == TIMELINE_ID_HIDE_DESTROY)
        {
            m_pLoginUI->Destroy();
        }
        else if (p->GetId() == TIMELINE_ID_HIDE_QUIT)
        {
            m_pLoginUI->Quit();
        }
    }
}
void  CLoginDlg::OnAnimateTick_Show(UI::IStoryboard* p)
{
    
}

void CLoginDlg::OnAnimateOver()
{
	 this->UpdateObject();
}
// #include "3rd/gdiplus/gdiplusfix.h"
// #pragma comment(lib, "gdiplus.lib")
void CLoginDlg::OnPaint(UI::IRenderTarget* pRenderTarget)
{
//     Gdiplus::Graphics graphics(pRenderTarget->GetBindHDC());
// 
//     Gdiplus::SolidBrush  solidBrush(Gdiplus::Color(255, 255, 0, 0)); 
//     Gdiplus::FontFamily  fontFamily(L"Times New Roman");
//     Gdiplus::Font        font(&fontFamily, 24, Gdiplus::FontStyleRegular, Gdiplus::UnitPixel);
// 
//     Gdiplus::StringFormat stringFormat;
//     stringFormat.SetTrimming(Gdiplus::StringTrimmingEllipsisCharacter);
// 
//     String str = L"One two three four five";
//     String strFix;
//     for (int i = 0; i < (int)str.length(); i++)
//     {
//         strFix.push_back(str[i]);
//         strFix.push_back(L'\t');
//     }
//     Gdiplus::REAL tabs = 0;
//     stringFormat.SetTabStops(10, 10, &tabs);
// 
//     graphics.DrawString(
//         strFix.c_str(), 
//         -1,  // string length
//         &font, 
//         Gdiplus::RectF(30, 30, 180, 60), 
//         &stringFormat, 
//         &solidBrush);
// 
//     // Draw the rectangle that encloses the text.
//     Gdiplus::Pen pen(Gdiplus::Color(255, 255, 0, 0));
//     graphics.DrawRectangle(&pen, 30, 30, 180, 60);

// 
// 
//     CRect rc(10, 10, 200, 75);
//     UI::DRAWTEXTPARAM param;
// //    param.szText = L"ut需要管理员邀请或主动申请，才能加入群\"。。。。。。。。。。。。...\"";
//     param.szText = L"StringTrimmingE llipsisCharacter Stri ngTrimmingEllipsis Character StringTrimmingEllipsisCharacter StringTrimmingEllipsisCharacter";
//     param.prc = &rc;
//     pRenderTarget->DrawString(GetRenderFont(), &param);
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
        UILoadMenu(GetUIApplication(), _T("login.status"), &pMenu);

		CPoint pt;
		::GetCursorPos(&pt);
		::SetForegroundWindow(GetHWND());	// 如果不加这句话，不选择菜单项的话，菜单不会消失

		int nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, static_cast<IMessage*>(this));
		SAFE_DELETE_Ixxx(pMenu);
	}
	SetMsgHandled(FALSE);

	return 0;
}
