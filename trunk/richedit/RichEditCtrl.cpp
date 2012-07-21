#include "StdAfx.h"
#include "RichEditCtrl.h"
#include "EditWordBreak.h"

HMODULE RichEditCtrl::s_RichEditDll = NULL;
LONG    RichEditCtrl::s_refDll = 0;
RichEditCtrl* g_pThis = NULL;
int CALLBACK EditWordBreakProc(LPTSTR lpch,
							   int ichCurrent,
							   int cch,
							   int code
							   );


RichEditCtrl::RichEditCtrl(void)
{
	g_pThis = this;
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
		nStyle |= ES_MULTILINE|WS_VSCROLL;
	}
	__super::Create(hWndParent, rect, _T(""), nStyle, 0, nCtrlID);
	if (NULL == m_hWnd)
	{
		UINT nErr = GetLastError();
		if (1407 == nErr)
			assert( 0 && _T("找不到窗口类 LoadLibrary RICHED20.DLL failed."));
	}

	if (NULL == m_hWnd)
	{
		return false;
	}

	// 设置默认字体
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_FACE|CFM_SIZE|CFM_BOLD;
	_tcsncpy(cf.szFaceName, _T("宋体"), LF_FACESIZE-1);
	cf.yHeight = 200;  // 10*20
	this->SetDefaultCharFormat(cf);

	// 避免英文字体与中文字体不统一的问题
	this->SendMessage(EM_SETLANGOPTIONS, IMF_DUALFONT, 0);

	// 换行处理
	// TODO: 
//	SetTargetDevice(NULL, 1);
	this->SendMessage(EM_SETWORDWRAPMODE, WBF_WORDBREAK, 0);
//	this->SendMessage(EM_SETWORDBREAKPROC,0,(LPARAM)EditWordBreakProc3/*customWordBreakProc*//*EditWordBreakProc*/);
//	this->SendMessage(EM_SETWORDWRAPMODE, WBF_WORDBREAK, 0);

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





/*
函数功能
	该函数是由应用程序定义的回调函数，该函数与EM_SETWORDBREAKPROC信号一起使用，一个多行编辑控制每当必须中断文本行时都调用EditwordBreakProc函数。EditwordBreakProc函数定义了一个指向此回调函数的指针，EditwordBreakProc是一个应用程序定义的函数名的占位符。

函数原型
	int CALLBACK EditWordBreakkPrOC（LPTSTR lpCh，int ichCurrent，int CCh int code）；

参数
	Ipch:指向编辑控制文本的指针。
	ichCurrent:指定一个文本缓冲区中字符位置的索引，该文本表示函数应该从这点开始检查字的中断。
	cch:指定编辑控制文本中字符的数目。
	code：指定回调函数要采取的措施，此参数可以是下列值之一。
	WB_CLASSIFY：检索指定位置的字符的字中断标志和字符类，此值是为与超文本编辑控制一起使用。
	WB_ISDECIMITER：检查在指定位置的字符是否是分隔符。
	WB_LEFT：在指定位置的左边，找到字的开头。
	WB_LEFTBREAK：在指定位置的左边，找到字的结束分隔符，此值是为与超文本编辑控制一起使用。
	WB_MOVEWORDLEFT：在指定位置的左边，找到字的开头，此值用于CTRL＋RIGHT使用时，此值是为与超文本编辑控制一起使用。
	WB_MOVEWDRDRIGHT：在指定位置的右边，找到字的开头，此值用于CTRL＋ROGHT使用时，此值是为与超文本编辑控制一起使用。
	WB_RIGHT：在指定位置的右边，找到字的开头。（对于右对齐编辑控制很有用）
	WB_RIGHTBREAK：在指定位置的右边找到字结束分隔符，（这对右对齐编辑控制很有用）此值是为与超文本编辑控制一起使用。

返回值
	如果代码参数指定WB_ISDELIMITER，且如果指定位置的字符为分隔符，则返回值为非零（TRUE），否则返回值为零。如果代码参数指定WB_CLASSIFY，返回值为指定位置的字符类和字符字中断标志，否则，返回值为指向文本缓冲区的开头的索引。

备注
	一个回车操作跟着一个换行符，一定被回调函数看作为一个单一的字，紧跟着一个换行符的两个回车操作也一定被看作为单一字。
	一个应用程序必须通过EM_SETWORDBREAKPROC消息中指定回调函数的地址来安装回调函数。
	对于超文本编辑控制，也可用EM_SETWORDBREAKPROCEX消息来取代带有EditWordBreadProcEx回调函数的缺省扩展字中断程序，该函数还提供了关于文本的其他信息，如字符集。

速查
	Windows NT：3.1及以上版本；Windows：95及以上版本；Windows CE：不支持;头文件：winuser.h；库文件：用户自定义。Unicode：定义为Unicode和ANSI两种原型。
*/
int CALLBACK EditWordBreakProc(LPTSTR lpch,
							   int ichCurrent,
							   int cch,
							   int code
							   )
{
	TCHAR szInfo[128];
	_stprintf(szInfo,_T("EditWordBreakProc, code=%d, ich=%d, cch=%d\n"),code, ichCurrent, cch);
	OutputDebugString(szInfo);

	switch (code)
	{
	case WB_ISDELIMITER:
		if (lpch[ichCurrent]==_T(' '))
		{
			if (ichCurrent>0 && lpch[ichCurrent-1] == _T(' '))
			{
				return FALSE;
			}
			return TRUE;
		}
		return FALSE;
 	case WB_MOVEWORDLEFT:
 		return ichCurrent-1;
 	case WB_MOVEWORDRIGHT:
 		return ichCurrent+1;
	case WB_LEFT:
		return ichCurrent;
	case WB_LEFTBREAK:
		{
			ichCurrent--;
			while (ichCurrent >= 0 && !customWordBreakProc(lpch, ichCurrent, cch, WB_ISDELIMITER))
				ichCurrent--;
			return ++ichCurrent;
		}
		return ichCurrent;
	default:
		return ichCurrent;
	}


	return TRUE;
}