#include "StdAfx.h"
#include "RichEditCtrl.h"

HMODULE RichEditCtrl::s_RichEditDll = NULL;
LONG    RichEditCtrl::s_refDll = 0;

RichEditCtrl::RichEditCtrl(void)
{
}

RichEditCtrl::~RichEditCtrl(void)
{
}


bool RichEditCtrl::CreateControl(HWND hWndParent, CRect rect, UINT nCtrlID, bool bMultiLine)
{
	InitRichEidtDll();

	UINT nStyle = WS_CHILD;
	if (bMultiLine)
	{
		nStyle |= ES_MULTILINE;
	}
	__super::Create(hWndParent, rect, _T(""), nStyle, 0, nCtrlID);
	if (NULL == m_hWnd)
	{
		UINT nErr = GetLastError();
		if (1407 == nErr)  // 找不到窗口类
		{
			assert( 0 && _T("找不到窗口类 LoadLibrary RICHED20.DLL failed."));
		}
	}

	if (NULL == m_hWnd)
	{
		return false;
	}

	return true;
}
void RichEditCtrl::Destroy()
{
	ReleaseRichEidtDll();
}

void RichEditCtrl::InitRichEidtDll()
{
	if (NULL == s_RichEditDll)
	{
		s_RichEditDll = ::LoadLibrary(__super::GetLibraryName());
	}
	s_refDll++;
}
void RichEditCtrl::ReleaseRichEidtDll()
{
	s_refDll--;
	if (0 == s_refDll)
	{
		FreeLibrary(s_RichEditDll);
		s_RichEditDll = NULL;
	}
}

