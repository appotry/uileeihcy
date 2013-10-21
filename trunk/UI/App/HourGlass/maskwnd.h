#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
namespace UI
{
	interface IButton;
	interface IImage;
	interface IStoryboard;
}

class CMaskWnd : public UI::ICustomWindow
{
public:
	CMaskWnd();
	~CMaskWnd();

	UI_BEGIN_MSG_MAP_Ixxx(CMaskWnd)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_TIMER(OnTimer)
		UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
		UIMSG_BN_CLICKED3(OnBtnClick)
		UIMSG_WM_KEYDOWN(OnKeyDown)
		UIMSG_WM_INITIALIZE(OnInitialize)
		UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
		UIMSG_WM_CLOSE(OnClose)
		UIMSG_WM_DESTROY(OnDestroy)
	UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

	BOOL  PreCreateWindow(CREATESTRUCT* pcs);
	void  OnInitialize();
	void  OnDestroy();
	void  OnClose();
	void  OnBtnClick(IMessage*  pMsgFrom);
	void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
	void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnEraseBkgnd(UI::IRenderTarget* pRenderTarget);
	void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);

public:
	void  OnSwitchPic();
	void  SetFolder(const TCHAR* szFolder);
	void  AddPic(const String& strPath);
	void  GetNextPicIndex();

public:
	UI::IButton*  m_pBtn;
	int  m_nTick;  // 180s倒计时
	
	int  m_nTickSwitchPic;  // 切换图片
	static int  m_nCurPicIndex;    // 当前显示的图片
	String  m_strFolder;    // 图片目录 
	vector<String>  m_vecPicInFolder;

	UI::IRenderBitmap*  m_pBmp;
	UI::IRenderBitmap*  m_pTemp;
	int  m_nAlphaBmp;
};