#include "StdAfx.h"
#include "RichEditCtrl.h"
#include "EditWordBreak.h"
#include "Objbase.h"

HMODULE RichEditCtrl::s_RichEditDll = NULL;
LONG    RichEditCtrl::s_refDll = 0;

int CALLBACK EditWordBreakProc(LPTSTR lpch,
							   int ichCurrent,
							   int cch,
							   int code
							   );


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

	this->SendMessage(EM_SETOLECALLBACK, 0, (LPARAM)this);



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

// *** IUnknown methods ***
HRESULT CRichEditOleCallbackImpl::QueryInterface(REFIID riid, LPVOID FAR * lplpObj)
{
	return E_NOTIMPL;
}
// ����richeditʱ������
ULONG   CRichEditOleCallbackImpl::AddRef()
{
	return 1;
}
// ����richeditʱ������
ULONG   CRichEditOleCallbackImpl::Release()
{
	return 0;
}

// *** IRichEditOleCallback methods ***

// This method must be implemented to allow cut, copy, paste, drag, 
// and drop operations of Component Object Model (COM) objects.
// ������richedit���������һ�������ϵ�ͼ�꣬�ͻ���øú���
HRESULT CRichEditOleCallbackImpl::GetNewStorage(LPSTORAGE FAR * lplpstg)
{
	if (NULL == lplpstg)
	{
		return E_INVALIDARG;
	}
	LPLOCKBYTES lpLockBytes = NULL;
	SCODE sc = ::CreateILockBytesOnHGlobal(NULL, TRUE, &lpLockBytes);
	if (sc != S_OK)
	{
		return E_OUTOFMEMORY;
	}

	sc = ::StgCreateDocfileOnILockBytes(lpLockBytes,
		STGM_SHARE_EXCLUSIVE|STGM_CREATE|STGM_READWRITE, 0, lplpstg);
	if (sc != S_OK)
	{
		return E_OUTOFMEMORY;
	}

	return S_OK;
}
HRESULT CRichEditOleCallbackImpl::GetInPlaceContext(LPOLEINPLACEFRAME FAR * lplpFrame,
													LPOLEINPLACEUIWINDOW FAR * lplpDoc,
													LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
	return E_NOTIMPL;
}
HRESULT CRichEditOleCallbackImpl::ShowContainerUI(BOOL fShow)
{
	return E_NOTIMPL;
}
// �ڴ��ⲿ����һ���ļ���richeditʱ������Ӧ��GetNewStorage�ɹ��󣬾ͻ��ٵ�������ӿں���
// ������S_OKʱ��������󽫱����룬����FALSEʱ�����󽫲��ᱻ����
HRESULT CRichEditOleCallbackImpl::QueryInsertObject(LPCLSID lpclsid, LPSTORAGE lpstg,
													LONG cp)
{
	return S_OK;
}
// ���罫richedit�е�һ��COM����ɾ����������һ�θýӿں���
// ���罫richedit�е�һ��COM�����������ק����һ��λ�ã�������һ�θýӿں���
// �ú�������һ��֪ͨ������������һ������Ҫ��deleted from rich edit control;
HRESULT CRichEditOleCallbackImpl::DeleteObject(LPOLEOBJECT lpoleobj)
{
	return S_OK;
}

// ��richedit��ʹ�� CTRL+Vʱ������
HRESULT CRichEditOleCallbackImpl::QueryAcceptData(LPDATAOBJECT lpdataobj,
												  CLIPFORMAT FAR * lpcfFormat, DWORD reco,
												  BOOL fReally, HGLOBAL hMetaPict)
{
	return E_NOTIMPL;
}
HRESULT CRichEditOleCallbackImpl::ContextSensitiveHelp(BOOL fEnterMode)
{
	return E_NOTIMPL;
}
// ��richedit��ʹ�� CTRL+C ʱ������
HRESULT CRichEditOleCallbackImpl::GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco,
												   LPDATAOBJECT FAR * lplpdataobj)
{
	return E_NOTIMPL;
}

// ��richedit��ʹ�������קʱ������
HRESULT CRichEditOleCallbackImpl::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState,
													LPDWORD pdwEffect)
{
	if (!fDrag) // allowable dest effects
	{
		DWORD dwEffect;
		// check for force link
		if ((grfKeyState & (MK_CONTROL|MK_SHIFT)) == (MK_CONTROL|MK_SHIFT))
			dwEffect = DROPEFFECT_LINK;
		// check for force copy
		else if ((grfKeyState & MK_CONTROL) == MK_CONTROL)
			dwEffect = DROPEFFECT_COPY;
		// check for force move
		else if ((grfKeyState & MK_ALT) == MK_ALT)
			dwEffect = DROPEFFECT_MOVE;
		// default -- recommended action is move
		else
			dwEffect = DROPEFFECT_MOVE;
		if (dwEffect & *pdwEffect) // make sure allowed type
			*pdwEffect = dwEffect;
	}
	return S_OK;
}

// �һ�RichEditʱ�����ã���������Ҽ�ʱ���������Ķ���Ĳ�ͬ���õ��Ĳ���Ҳ��ͬ��
// �����ڿհ״��һ���seltype=0, lpoleobj=NULL
// ������һ��COM�����һ�������seltype=2, lpoleobj = xxx;
HRESULT CRichEditOleCallbackImpl::GetContextMenu(WORD seltype, LPOLEOBJECT lpoleobj,
												 CHARRANGE FAR * lpchrg,
												 HMENU FAR * lphmenu)
{
#ifdef _DEBUG
	HMENU& hMenu = *lphmenu;
	TCHAR szInfo[128] = _T("");
	_stprintf(szInfo, _T("GetContextMenu Args: seltype=%d, lpoleobj=%08x, lpchrg=%d,%d"),
		seltype, lpoleobj, lpchrg->cpMin, lpchrg->cpMax);

	hMenu = CreatePopupMenu();
	BOOL bRet = ::AppendMenu(hMenu, MF_STRING, 10001, szInfo);
#endif
	return S_OK;
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