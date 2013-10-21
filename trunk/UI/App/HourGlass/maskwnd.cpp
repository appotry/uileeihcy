#include "stdafx.h"
#include "maskwnd.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Project\UIUtil\h\util.h"
#include "UISDK\Kernel\Inc\Util\iimage.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"

int CMaskWnd::m_nCurPicIndex = 0;
CMaskWnd::CMaskWnd()
{
#ifdef _DEBUG
	m_nTick = 15;
#else
	m_nTick = GetConfig()->GetBreakTime();
#endif

	m_nTickSwitchPic = 0;
	m_pBtn = NULL;
	m_pBmp = NULL;
	m_pTemp = NULL;
	m_nAlphaBmp = 0;
}

CMaskWnd::~CMaskWnd()
{
	SAFE_RELEASE(m_pBmp);
	GetUIApplication()->GetAnimateMgr()->ClearStoryboardOfNotify(this);
}

void  CMaskWnd::OnInitialize()
{
	__super::xProcessMessage(GetCurMsg(), 0, 0);

	SetWindowResizeType(0);
	m_pBtn = (UI::IButton*)FindChildObject(_T("idok"));

	SetTimer(GetHWND(), 1, 1000, NULL);
	SetFolder(GetConfig()->GetPicFolder());

	if (m_vecPicInFolder.size() > 0)
	{
		//m_pCurPic = new UI::IImage;
		UI::UICreateRenderBitmap(UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, UI::IMAGE_ITEM_TYPE_IMAGE, &m_pBmp);
		GetNextPicIndex();
		m_pBmp->LoadFromFile(m_vecPicInFolder[m_nCurPicIndex].c_str(), true);
		
		GetUIApplication()->GetAnimateMgr()->SetFps(50);
		UI::IStoryboard* pStoryboard = GetUIApplication()->GetAnimateMgr()->CreateStoryboard();
		UI::IIntLinearMove* pMoveAlgo = NULL;
		UI::IIntTimeline* pTimeline = (UI::IIntTimeline*)pStoryboard->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo);
		pMoveAlgo->SetParam1(0, 255, 300);
		pTimeline->SetOutRef(&m_nAlphaBmp);
		pStoryboard->SetId(1);
		pStoryboard->SetNotifyObj(this);
		pStoryboard->Begin();
	}
	srand(GetTickCount());
}
void  CMaskWnd::OnDestroy()
{
	KillTimer(GetHWND(), 1);
}
void  CMaskWnd::OnBtnClick(IMessage* pMsgFrom)
{
	if (m_nTick < 0)
	{
		EndDialog(0);
	}
}

BOOL  CMaskWnd::PreCreateWindow(CREATESTRUCT* pcs)
{
	__super::xProcessMessage(GetCurMsg(), 0, 0);

#ifdef _DEBUG
	pcs->dwExStyle |= WS_EX_TOOLWINDOW;
#else
	pcs->dwExStyle |= WS_EX_TOOLWINDOW|WS_EX_TOPMOST;
#endif
	return TRUE;
}
void  CMaskWnd::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
	if (m_nTick < 0)
	{
		KillTimer(GetHWND(), 1);

		UI::IStoryboard* pStoryboard = GetUIApplication()->GetAnimateMgr()->CreateStoryboard();
		UI::IIntLinearMove* pMoveAlgo = NULL;
		UI::IIntTimeline* pTimeline = (UI::IIntTimeline*)pStoryboard->CreateTimeline(UI::TLV_INT, 1, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo);
		pMoveAlgo->SetParam1(255, 0, 300);
		pTimeline->SetOutRef(&m_nAlphaBmp);
		pStoryboard->SetId(3);
		pStoryboard->SetNotifyObj(this);
		pStoryboard->Begin();

		return;
	}

	if (m_pBtn)
	{
		TCHAR  szText[64] = _T("");
		_stprintf(szText, _T("(%d)"), m_nTick--);
		m_pBtn->SetText(szText);
	}

	m_nTickSwitchPic++;
	if (m_nTickSwitchPic > 10)
	{
		OnSwitchPic();
		m_nTickSwitchPic = 0;
	}
}
void  CMaskWnd::OnClose()
{
	if (m_nTick < 0)
	{
		SetMsgHandled(FALSE);
	}
}

void CMaskWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_DOWN == nChar)
	{
		m_nTick --;
	}
}

void  CMaskWnd::GetNextPicIndex()
{
#if 0 // Ë³Ðò²¥·Å
	m_nCurPicIndex ++;
	if (m_nCurPicIndex >= (int)m_vecPicInFolder.size())
		m_nCurPicIndex = 0;
#elif 1 // Ëæ»ú²¥·Å
	if (m_vecPicInFolder.size() > 0)
	{
		m_nCurPicIndex = rand()%m_vecPicInFolder.size();
	}
	else
	{
		m_nCurPicIndex = 0;
	}
#endif
}

void  CMaskWnd::OnSwitchPic()
{
	if (!m_pBmp)
		return;

	GetNextPicIndex();
	if (!m_pTemp)
	{
		UI::UICreateRenderBitmap(UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDI, UI::IMAGE_ITEM_TYPE_IMAGE, &m_pTemp);
	}
	m_pTemp->LoadFromFile(m_vecPicInFolder[m_nCurPicIndex].c_str(), true);

	UI::IStoryboard* pStoryboard = GetUIApplication()->GetAnimateMgr()->CreateStoryboard();

	UI::IIntLinearMove* pMoveAlgoNew = NULL;
	UI::IIntTimeline* pTimelineNew = (UI::IIntTimeline*)pStoryboard->CreateTimeline(UI::TLV_INT, 2, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgoNew);
	pMoveAlgoNew->SetParam1(0, 255, 300);
	pTimelineNew->SetOutRef(&m_nAlphaBmp);
	m_nAlphaBmp = 0;

	pStoryboard->SetNotifyObj(this);
	pStoryboard->SetId(2);
	pStoryboard->Begin();

	this->UpdateObject(true);
}
bool CALLBACK _EnumFileInDirProc(const TCHAR* szDir, const TCHAR* szName, WPARAM wParam)
{
	String str = szDir;
	str.append(szName);

	((CMaskWnd*)wParam)->AddPic(str);
	return true;
}
void  CMaskWnd::SetFolder(const TCHAR* szFolder)
{
	if (!szFolder)
		return;

	UI::Util::EnumFileInDirectory(szFolder, _EnumFileInDirProc, (WPARAM)this);
}
void  CMaskWnd::AddPic(const String& strPath)
{
	m_vecPicInFolder.push_back(strPath);
}

void  CMaskWnd::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
	for (int i = 0; i < nCount; i++)
	{
		if (ppArray[i]->GetId() == 2 && ppArray[i]->IsFinish())
		{
			SAFE_RELEASE(m_pBmp);
			m_pBmp = m_pTemp;
			m_pTemp = NULL;
			m_nAlphaBmp = 255;
		}
		if (ppArray[i]->GetId() == 3 && ppArray[i]->IsFinish())
		{
			EndDialog(0);
		}
	}
	this->UpdateObject();
}
void  CMaskWnd::OnEraseBkgnd(UI::IRenderTarget* pRenderTarget)
{
	if (m_nCurPicIndex >= (int)m_vecPicInFolder.size())
		return;

	if (!m_pBmp)
		return;

	HDC hDC = pRenderTarget->GetBindHDC();
	CRect rc;
	::GetClientRect(GetHWND(), &rc);

	UI::DRAWBITMAPPARAM param;
	param.xSrc = 0;
	param.ySrc = 0;
	param.xDest = 0;
	param.yDest = 0;
	param.wDest = rc.Width();
	param.hDest = rc.Height();
    param.nFlag = UI::DRAW_BITMAP_STRETCH;

	param.wSrc = m_pBmp->GetWidth();
	param.hSrc = m_pBmp->GetHeight();
	param.nAlpha = m_pTemp?255:m_nAlphaBmp;
	pRenderTarget->DrawBitmap(m_pBmp, &param);

	if (m_pTemp)
	{
		param.wSrc = m_pTemp->GetWidth();
		param.hSrc = m_pTemp->GetHeight();
		param.nAlpha = m_nAlphaBmp;
		pRenderTarget->DrawBitmap(m_pTemp, &param);
	}
}