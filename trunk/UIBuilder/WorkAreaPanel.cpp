#include "StdAfx.h"
#include "WorkAreaPanel.h"
#include <atlframe.h>

BOOL CWorkAreaPanel::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
// 	CMDIChildWindow* pChildWnd = new CMDIChildWindow;
// 	RECT rc = {0,0,300,300};
// 	HWND hWnd = pChildWnd->Create(m_hWnd,&rc,_T("D"));
// 	pChildWnd->ShowWindow(SW_SHOW);
// 
// 	CChildFrame* pChild = new CChildFrame;
// 	pChild->CreateEx(m_hWndClient);
	return TRUE;
}

//
//	打开（激活）一个窗口
//
bool CWorkAreaPanel::OpenWindow( UI_RESOURCE_TYPE eResType,  void* data )
{
// 	switch(eResType)
// 	{
// 		
// 	}
	return false;
}


bool CWorkAreaPanel::IsWindowOpened( UI_RESOURCE_TYPE,  void* data )
{

	return false;
}