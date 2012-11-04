// IM_UI.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "IM_UI.h"
#include "LoginDlg.h"
#include "ChatDlg.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );

	TCHAR szPath[MAX_PATH] = _T("");
	Util::GetAppPath_(szPath);
	String str = szPath;
	str += _T("im\\im.uiproj");
	bool bRet = UI_Initialize(str.c_str());
	if (false == bRet)
		return 0;

// 	CLoginDlg   logindlg;
// 	logindlg.DoModal(_T("logindlg"), NULL);

	CChatDlg    chatDlg;
	chatDlg.DoModal(_T("chatdlg"),NULL);

	UI_Release();
	return 0;
}