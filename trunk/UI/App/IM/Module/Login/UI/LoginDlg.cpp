#include "StdAfx.h"
#include "LoginDlg.h"
#include <time.h>
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\imenu.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"
#include "UISDK\Control\Inc\Interface\iflashwrap.h"
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"
#include "App\IM\IM_Ctrls\inc\iqqloginaccountcombobox.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"
#include "LoginUI.h"

#define WINDOW_3D_ROTATE_DURATION  250   // ���ڷ�ת90�ȵ�ʱ��
#define TIMELINE_ID_GO_STEP1      1
#define TIMELINE_ID_GO_STEP2      3
#define TIMELINE_ID_BACK_STEP1    5
#define TIMELINE_ID_BACK_STEP2    7
#define WINDOW_3D_ROTATE_Z_DEEP   500

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
	GetUIApplication()->GetAnimateMgr()->SetFps(50);  // �ָ�֡��
    GetUIApplication()->KillTimerById(TIMER_ID_LOGING_TEXT_TRAIL_POINT,  static_cast<UI::IMessage*>(this));
	SAFE_RELEASE(m_pTrayIcon);
}

void CLoginDlg::OnInitWindow()
{
    ICustomWindow::nvProcessMessage(GetCurMsg(), 0, 0);

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

	// ��Win7 64λ���治����SetIconҲ����ȷ��ʾ������win7 32����ȴ������ʾ
// 	HICON hIconBig = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_IM_UI));
// 	HICON hIconSmall = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_SMALL));
// 	SendMessage(GetHWND(), WM_SETICON, ICON_BIG, (LPARAM)hIconBig);
// 	SendMessage(GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);
// 	DestroyIcon(hIconBig);
// 	DestroyIcon(hIconSmall);

	// ��������
//     UI::UICreateTrayIcon(GetUIApplication(), &m_pTrayIcon);
// 	HICON hIcon = ::LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_IM_UI));
// 	m_pTrayIcon->AddTrayIcon(GetHWND(), hIcon, L"IM_UI");


    // ��ʼ����¼�˺�
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

	UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
	pAnimateMgr->SetFps(100);  // �Ӵ�֡�ʣ���֤������
}

void  CLoginDlg::InitFlash()
{
    // ���ñ���flash
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

    String strUri;
    String strFlashFileName;
    if (ptm->tm_hour < 6)
    {
        strUri = _T("img\\login\\night.swf");
        strFlashFileName = _T("night.swf");
    }
    else if (ptm->tm_hour < 9)
    {
        strUri = _T("img\\login\\morning.swf");
        strFlashFileName = _T("morning.swf");
    }
    else if (ptm->tm_hour < 17)
    {
        strUri = _T("img\\login\\noon.swf");
        strFlashFileName = _T("noon.swf");
    }
    else if (ptm->tm_hour < 20)
    {
        strUri = _T("img\\login\\afternoon.swf");
        strFlashFileName = _T("afternoon.swf");
    }
    else
    {
        strUri = _T("img\\login\\night.swf");
        strFlashFileName = _T("night.swf");
    }

    UI::ISkinDataSource* pDataSource = GetUIApplication()->GetActiveSkinRes()->GetDataSource();
    UI::SKIN_PACKET_TYPE eType = pDataSource->GetType();
    if (eType == UI::SKIN_PACKET_TYPE_DIR)
    {
        strUri.insert(0, _T("skin\\2012\\"));
        m_pFlashCtrl->SetFlashUri((BSTR)strUri.c_str());
    }
    else
    {
        // ��zip�ļ��н�ѹ�����ŵ���ʱĿ¼����
        UI::IStreamBufferReader* pBuffer = NULL;
        pDataSource->Load_StreamBuffer(strUri.c_str(), &pBuffer);
        
        // Get the temp path.
        TCHAR szTempPath[MAX_PATH] = {0};
        TCHAR szTempFileName[MAX_PATH] = {0};
        GetTempPath(MAX_PATH, szTempPath); 
        int nLength = _tcslen(szTempPath);
        UIASSERT(nLength > 0);
        if (szTempPath[nLength-1] == _T('\\'))
            _tcscat(szTempPath, _T("uileeihcy_im\\"));
        else
            _tcscat(szTempPath, _T("\\uileeihcy_im\\"));

        CreateDirectory(szTempPath, NULL);

        // Create a temporary file. 
        //if (0 != GetTempFileName(szTempPath, _T("swf_"), 0, szTempFileName))
        _tcscpy(szTempFileName, szTempPath);
        _tcscat(szTempFileName, strFlashFileName.c_str());
        {
            if (!PathFileExists(szTempFileName))
                pBuffer->save(szTempFileName);
            
             m_pFlashCtrl->SetFlashUri(szTempFileName);
        }

        SAFE_RELEASE(pBuffer);
    }
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
    pMoveAlgo->SetParam1(360, 270, WINDOW_3D_ROTATE_DURATION, -500/1000.0f);
    pTimelineY->SetOutRef(p->GetRotateYPtr());

    UI::IIntLinearMove* pMoveAlgo2 = NULL;
    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo2);
    pMoveAlgo2->SetParam1(0, WINDOW_3D_ROTATE_Z_DEEP, WINDOW_3D_ROTATE_DURATION);
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
    pMoveAlgo->SetParam1(0, 90, WINDOW_3D_ROTATE_DURATION, 500/1000.0f);
	pTimelineY->SetOutRef(p->GetRotateYPtr());

    UI::IIntLinearMove* pMoveAlgo2 = NULL;
    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo2);
    pMoveAlgo2->SetParam1(0, WINDOW_3D_ROTATE_Z_DEEP, WINDOW_3D_ROTATE_DURATION);
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
    m_strLoginingText = _T("���ڵ�¼(");
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

    // ��ʾgif
    if (m_pGifLoadding)
        m_pGifLoadding->Start();

    // ���ö�ʱ����ʾ...
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
    
    // ����hover����
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
            break;  // break��Ϊ�˷�ֹ��Window3DAnimateɾ���ˣ�����һ��ѭ������ȥ���ʵ��±���
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

// 90�ȷ�ת�������е��������
void  CLoginDlg::OnAnimateTick_GoStep1(UI::IStoryboard* pStoryboard)
{
    if (!pStoryboard->IsFinish())
        return;

    // �ر�Flashˢ��
    if (m_pFlashLayer)
        m_pFlashLayer->SetLayerEnable(false);

    // �л���壬�л��л��漰������仯�����´���ˢ�£���˽�����ˢ�½���
    GetRenderChain()->SetCanCommit(false);
    if (m_pPanelNetSet)
        m_pPanelNetSet->SetVisible(true, false);

    if (m_pPanelLogin)
        m_pPanelLogin->SetVisible(false, false);
    GetRenderChain()->SetCanCommit(true);

    // ֻ���ƣ������ύ����������
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
    pMoveAlgo->SetParam1(270, 360, WINDOW_3D_ROTATE_DURATION, 500/1000.0f);
    pTimelineY->SetOutRef(p->GetRotateYPtr());

    UI::IIntLinearMove* pMoveAlgo2 = NULL;
    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo2);
    pMoveAlgo2->SetParam1(WINDOW_3D_ROTATE_Z_DEEP, 0, WINDOW_3D_ROTATE_DURATION);
    pTimelineZ->SetOutRef(p->GetOffsetZPtr());
    p->Begin();
}
// 90�Ȼط��������лص�¼���
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

    // ֻ���ƣ������ύ����������
    this->UpdateObject(false);

    UI::IAnimateManager* pAnimateMgr = GetUIApplication()->GetAnimateMgr();
    UI::IWindow3DAnimate* p = static_cast<UI::IWindow3DAnimate*>(
        pAnimateMgr->CreateWindowAnimateInstance(UI::E_WINDOW_ANIMATE_3D_ROTATE, static_cast<IWindowBase*>(this)));
    if (NULL == p)
        return;

    p->SetNotifyObj(static_cast<IMessage*>(this));
    p->SetWParam((WPARAM)p);  // �����ڻط������в��ϸ��¶���ͼƬ
    p->SetId(TIMELINE_ID_BACK_STEP2);

    UI::IIntAccelerateMove* pMoveAlgo = NULL;
    UI::IIntTimeline* pTimelineY = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 0, UI::ITMA_Accelerate, (UI::IMoveAlgorithm**)&pMoveAlgo);
    pMoveAlgo->SetParam1(90, 0, WINDOW_3D_ROTATE_DURATION, -500/1000.0f);
    pTimelineY->SetId(TIMELINE_ID_BACK_STEP2);
    pTimelineY->SetOutRef(p->GetRotateYPtr());

    UI::IIntLinearMove* pMoveAlgo2 = NULL;
    UI::IIntTimeline* pTimelineZ = (UI::IIntTimeline*)p->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo2);
    pMoveAlgo2->SetParam1(WINDOW_3D_ROTATE_Z_DEEP, 0, WINDOW_3D_ROTATE_DURATION);
    pTimelineZ->SetOutRef(p->GetOffsetZPtr());
    p->Begin();

    if (m_pFlashCtrl)
        m_pFlashCtrl->UpdateObject(false);
}
// �����ת�ص�¼���Ĺ����У�����ǰ����һ��ʱ�����¶���������flash�����νӲ�����
// �������ζ��������У���Ҫ��ͣ��ȥ���¶���Դ����
void  CLoginDlg::OnAnimateTick_BackStep2(UI::IStoryboard* pStoryboard)
{
    UI::IWindow3DAnimate* pWindow3DAnimate = (UI::IWindow3DAnimate*)pStoryboard->GetWParam();
    if (pWindow3DAnimate && !pStoryboard->IsFinish())
    {
        // flash������Ȼ�ڼ������Ų�����˫���壬��������Ȼ�Ǳ�SetCanRedraw(FALSE)
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
// //    param.szText = L"ut��Ҫ����Ա������������룬���ܼ���Ⱥ\"������������������������...\"";
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

	int nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, rc.left, rc.bottom, static_cast<IMessage*>(this), GetHWND(), &rc);
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
		::SetForegroundWindow(GetHWND());	// ���������仰����ѡ��˵���Ļ����˵�������ʧ

		int nRet = pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, static_cast<IMessage*>(this));
		SAFE_DELETE_Ixxx(pMenu);
	}
	SetMsgHandled(FALSE);

	return 0;
}
