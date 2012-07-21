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
			assert( 0 && _T("�Ҳ��������� LoadLibrary RICHED20.DLL failed."));
	}

	if (NULL == m_hWnd)
	{
		return false;
	}

	// ����Ĭ������
	CHARFORMAT cf;
	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_FACE|CFM_SIZE|CFM_BOLD;
	_tcsncpy(cf.szFaceName, _T("����"), LF_FACESIZE-1);
	cf.yHeight = 200;  // 10*20
	this->SetDefaultCharFormat(cf);

	// ����Ӣ���������������岻ͳһ������
	this->SendMessage(EM_SETLANGOPTIONS, IMF_DUALFONT, 0);

	// ���д���
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
��������
	�ú�������Ӧ�ó�����Ļص��������ú�����EM_SETWORDBREAKPROC�ź�һ��ʹ�ã�һ�����б༭����ÿ�������ж��ı���ʱ������EditwordBreakProc������EditwordBreakProc����������һ��ָ��˻ص�������ָ�룬EditwordBreakProc��һ��Ӧ�ó�����ĺ�������ռλ����

����ԭ��
	int CALLBACK EditWordBreakkPrOC��LPTSTR lpCh��int ichCurrent��int CCh int code����

����
	Ipch:ָ��༭�����ı���ָ�롣
	ichCurrent:ָ��һ���ı����������ַ�λ�õ����������ı���ʾ����Ӧ�ô���㿪ʼ����ֵ��жϡ�
	cch:ָ���༭�����ı����ַ�����Ŀ��
	code��ָ���ص�����Ҫ��ȡ�Ĵ�ʩ���˲�������������ֵ֮һ��
	WB_CLASSIFY������ָ��λ�õ��ַ������жϱ�־���ַ��࣬��ֵ��Ϊ�볬�ı��༭����һ��ʹ�á�
	WB_ISDECIMITER�������ָ��λ�õ��ַ��Ƿ��Ƿָ�����
	WB_LEFT����ָ��λ�õ���ߣ��ҵ��ֵĿ�ͷ��
	WB_LEFTBREAK����ָ��λ�õ���ߣ��ҵ��ֵĽ����ָ�������ֵ��Ϊ�볬�ı��༭����һ��ʹ�á�
	WB_MOVEWORDLEFT����ָ��λ�õ���ߣ��ҵ��ֵĿ�ͷ����ֵ����CTRL��RIGHTʹ��ʱ����ֵ��Ϊ�볬�ı��༭����һ��ʹ�á�
	WB_MOVEWDRDRIGHT����ָ��λ�õ��ұߣ��ҵ��ֵĿ�ͷ����ֵ����CTRL��ROGHTʹ��ʱ����ֵ��Ϊ�볬�ı��༭����һ��ʹ�á�
	WB_RIGHT����ָ��λ�õ��ұߣ��ҵ��ֵĿ�ͷ���������Ҷ���༭���ƺ����ã�
	WB_RIGHTBREAK����ָ��λ�õ��ұ��ҵ��ֽ����ָ�����������Ҷ���༭���ƺ����ã���ֵ��Ϊ�볬�ı��༭����һ��ʹ�á�

����ֵ
	����������ָ��WB_ISDELIMITER�������ָ��λ�õ��ַ�Ϊ�ָ������򷵻�ֵΪ���㣨TRUE�������򷵻�ֵΪ�㡣����������ָ��WB_CLASSIFY������ֵΪָ��λ�õ��ַ�����ַ����жϱ�־�����򣬷���ֵΪָ���ı��������Ŀ�ͷ��������

��ע
	һ���س���������һ�����з���һ�����ص���������Ϊһ����һ���֣�������һ�����з��������س�����Ҳһ��������Ϊ��һ�֡�
	һ��Ӧ�ó������ͨ��EM_SETWORDBREAKPROC��Ϣ��ָ���ص������ĵ�ַ����װ�ص�������
	���ڳ��ı��༭���ƣ�Ҳ����EM_SETWORDBREAKPROCEX��Ϣ��ȡ������EditWordBreadProcEx�ص�������ȱʡ��չ���жϳ��򣬸ú������ṩ�˹����ı���������Ϣ�����ַ�����

�ٲ�
	Windows NT��3.1�����ϰ汾��Windows��95�����ϰ汾��Windows CE����֧��;ͷ�ļ���winuser.h�����ļ����û��Զ��塣Unicode������ΪUnicode��ANSI����ԭ�͡�
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