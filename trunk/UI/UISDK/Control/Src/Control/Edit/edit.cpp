#include "stdafx.h"
#include "edit.h"
#include "UISDK\Kernel\Inc\Interface\iuiapplication.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\iwindowmousemgr.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"

#include "UISDK\Control\Inc\Interface\imenu.h"
namespace UI
{

//
//	Remark:
//		m_EditData.SetCaret( nCP, true, bUpdate1);
//		this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2);
//
//		SetCaret�������������������ѡ������
//		MoveCaret���������������������Ӧ����Ļ��ʾ���꣬���������������
//
//		����ٵ���updateObjectˢ��Edit������SetCaretPos���ù��λ��
//

EditData::EditData()
{
	m_pEdit = NULL;

	m_nMaxChar = -1;
	m_nSelStart = 0;
	m_nCaret = 0;

	ZeroMemory( &m_ScriptControl, sizeof( SCRIPT_CONTROL));
	ZeroMemory( &m_ScriptState, sizeof( SCRIPT_STATE));
	ScriptApplyDigitSubstitution( NULL, &m_ScriptControl, &m_ScriptState);
	m_Analysis = NULL;
	m_nTextWidth = 0;

	m_bInsertMode = true;
}

void EditData::BindToEdit(Edit* pEdit)
{
	m_pEdit = pEdit;
}

EditData::~EditData()
{
	if (m_Analysis)
	{
		ScriptStringFree(&m_Analysis);
		m_Analysis = NULL;
	}
	m_pEdit = NULL;
}

void EditData::SetText(const TCHAR* szText, bool& bUpdate)
{
	bUpdate = false;

	String strInput;
	if (false == this->FilterString(szText, strInput))
		return;

	if (0 == m_nMaxChar)
		return;

	bUpdate = true;
	if (m_nMaxChar != -1)
	{
		if ((int)strInput.length() > m_nMaxChar)
		{
			m_strText = strInput.substr(0, m_nMaxChar);
		}
		else
		{
			m_strText = strInput;
		}
	}
	else
	{
		m_strText = strInput;
	}

    m_nSelStart = m_nCaret = m_strText.length();
	this->Fire_Text_Changed(TRUE);
}

//
//	�ڵ�ǰλ���������һ���ַ�
//
void EditData::ReplaceChar(const TCHAR& c, bool& bUpdate)
{
	bUpdate = false;

	TCHAR cInput = _T('');
	if (false == this->FilterChar(c, cInput))
		return;

	bool bInsertOrOverride = m_bInsertMode;

	// �����ǰ�б�ѡ������֣���ô�ø�����Щ����
	if (IsSelectionExist())
	{
		bInsertOrOverride = true;  // ����Ǹ���ģʽ����ɾ����ѡ��֮��Ӧ��תΪ���룬�γɸ���ѡ��������

		bUpdate = true;
		DeleteSelectionText();
	}

	if (m_nCaret >= (int)m_strText.length())
	{
		bInsertOrOverride = true;  // ���������ĩβʱ��Ӧ���ǲ���
	}

	if (bInsertOrOverride)
	{
		// ��������
		if ( m_nMaxChar >= 0 && (int)this->m_strText.length() >= (UINT)m_nMaxChar)
		{

		}
		else
		{
			bUpdate = true;
			this->m_strText.insert( m_nCaret, 1, cInput);
			m_nCaret += 1;
			m_nSelStart = m_nCaret;
		}
	}
	else
	{	
		// if (cInput != m_strText[m_nCaret]) ��ʹҪ���ǵ��ַ�һ����ҲҪ���͸ı��֪ͨ
		{
			bUpdate = true; 
			m_strText.replace(m_nCaret,1,1,cInput);
			m_nCaret += 1;
			m_nSelStart = m_nCaret;
		}
	}

	if (bUpdate)
	{
		this->Fire_Text_Changed();
	}
}

//
//	�ڵ�ǰλ����������ַ�,��Copy,Paste
//
void EditData::ReplaceString(const String& str, bool& bUpdate)
{
	bUpdate = false;

	String strInput;
	if (false == this->FilterString(str.c_str(), strInput))
		return;

	bool bInsertOrOverride = m_bInsertMode;

	// �����ǰ�б�ѡ������֣���ô�ø�����Щ����
	if (IsSelectionExist())
	{
		bInsertOrOverride = true;   // ����Ǹ���ģʽ����ɾ����ѡ��֮��Ӧ��תΪ���룬�γɸ���ѡ��������

		bUpdate = true;
		DeleteSelectionText();
	}

	if(m_nCaret >= (int)m_strText.length())
	{
		bInsertOrOverride = true;  // ���������ĩβʱ��Ӧ���ǲ���
	}

	if (bInsertOrOverride)
	{
		// ��������
		if (m_nMaxChar >= 0)
		{
			int nRemain = m_nMaxChar - this->m_strText.length();
			if (0 == nRemain)
			{
				if (bUpdate)
				{
					this->Fire_Text_Changed();  // ����ɾ����ѡ������text changed
				}
				return;
			}

			if (nRemain < (int)strInput.length())
			{
				strInput = strInput.substr(0,nRemain);
			}
		}

		this->m_strText.insert(m_nCaret, strInput);
	}
	else
	{
		// ��������
		if ( m_nMaxChar >= 0)
		{
			int nRemain = m_nMaxChar - m_nCaret;
			if (0 == nRemain)
			{
				if (bUpdate)
				{
					this->Fire_Text_Changed();  // ����ɾ����ѡ������text changed
				}
				return;
			}

			if (nRemain < (int)strInput.length())
			{
				strInput = strInput.substr(0,nRemain);
			}
		}
		this->m_strText.replace( m_nCaret, strInput.length(), strInput.c_str());
	}

	bUpdate = true;
	m_nCaret += strInput.length();
	m_nSelStart = m_nCaret;

	if (bUpdate)
	{
		this->Fire_Text_Changed();
	}
}

//
//	����ɾ��
//
void EditData::Delete(bool& bUpdate)
{
	bUpdate = true;

	// ɾ����ǰ��ѡ�������
	if (IsSelectionExist())
	{
		this->DeleteSelectionText();
	}
	// ����ɾ��һ���ַ�
	else
	{
		if (this->m_strText.length() == 0 || m_nCaret >= (int)this->m_strText.length())
		{
			bUpdate = false;
		}
		else
		{
			this->m_strText.erase( m_nCaret, 1);
		}
	}

	m_nSelStart = m_nCaret;
	if (bUpdate)
	{
		this->Fire_Text_Changed();
	}
}

//
//	��ǰɾ��
//
void EditData::BackSpace(bool& bUpdate)
{
	bUpdate = true;

	// ɾ����ǰ��ѡ�������
	if (IsSelectionExist())
	{
		this->DeleteSelectionText();
	}
	// ��ǰɾ��һ���ַ�
	else
	{
		if (m_nCaret <= 0 || this->m_strText.length() == 0)
		{
			bUpdate = false;
		}
		else
		{
			m_nCaret--;
			this->m_strText.erase( m_nCaret, 1);
		}
	}

	m_nSelStart = m_nCaret;
	if (bUpdate)
	{
		this->Fire_Text_Changed();
	}
}

//
//	���ù���λ�ã�ͬʱ����ѡ����ʼ��λ��
//
void EditData::SetCaret(int nCaret, bool bSetSelStart, bool& bUpdate)
{
	bUpdate = false;

	int nOldSelStart = m_nSelStart;
	int nOldCaret = m_nCaret;

	if (nCaret < 0)
	{
		nCaret = 0;
	}
	else if (nCaret > (int)m_strText.length())
	{
		nCaret = m_strText.length();
	}

	m_nCaret = nCaret;
	if (bSetSelStart)
	{
		m_nSelStart = m_nCaret;
	}

	if (m_nSelStart==m_nCaret && nOldCaret==nOldSelStart)           // û��ѡ���Ĺ���ƶ�
	{
	}
	else if (m_nSelStart == nOldCaret && m_nCaret == nOldSelStart) // ѡ����
	{
	}
	else
	{
		bUpdate = true;
	}
}
void EditData::GetPriorItemPos( int nCP, int* pPrior)
{
	if (NULL == pPrior)
		return ;

	*pPrior = nCP;  // Default is the char itself

	if (NULL == m_Analysis)
	{
		if (false ==this->StringAnalysis())
			return;
	}

	const SCRIPT_LOGATTR* pLogAttr = ScriptString_pLogAttr( m_Analysis);
	if (!pLogAttr)
		return;

	if (!ScriptString_pcOutChars(m_Analysis))
		return;
	int nInitial = *ScriptString_pcOutChars(m_Analysis);
	if (nCP - 1 < nInitial)
		nInitial = nCP - 1;
	for (int i = nInitial; i > 0; --i)
		if (pLogAttr[i].fWordStop ||       // Either the fWordStop flag is set
			( !pLogAttr[i].fWhiteSpace &&  // Or the previous char is whitespace but this isn't.
			pLogAttr[i - 1].fWhiteSpace))
		{
			*pPrior = i;
			return;
		}
		// We have reached index 0.  0 is always a break point, so simply return it.
		*pPrior = 0;
}
void EditData::GetNextItemPos(int nCP, int* pNext) 
{
	if (NULL == pNext)
		return;

	*pNext = nCP;  // Default is the char itself

	if (NULL == m_Analysis)
	{
		if (false ==this->StringAnalysis())
			return;
	}

	const SCRIPT_LOGATTR* pLogAttr = ScriptString_pLogAttr(m_Analysis);
	if (!pLogAttr)
		return;

	if (!ScriptString_pcOutChars(m_Analysis))
		return;
	int nInitial = *ScriptString_pcOutChars(m_Analysis);
	if (nCP + 1 < nInitial)
		nInitial = nCP + 1;

	int i = nInitial;
	int limit = *ScriptString_pcOutChars(m_Analysis);
	while (limit > 0 && i < limit - 1)
	{
		if (pLogAttr[i].fWordStop)      // Either the fWordStop flag is set
		{
			*pNext = i;
			return;
		}
		else if (pLogAttr[i].fWhiteSpace &&  // Or this whitespace but the next char isn't.
			!pLogAttr[i + 1].fWhiteSpace)
		{
			*pNext = i + 1;  // The next char is a word stop
			return;
		}

		++i;
		limit = *ScriptString_pcOutChars( m_Analysis);
	}
	// We have reached the end. It's always a word stop, so simply return it.
	*pNext = *ScriptString_pcOutChars(m_Analysis) - 1;

}


//
//  ɾ����ǰѡ���ı������û��ѡ����ִ��
//
void  EditData::DeleteSelectionText(bool& bUpdate)
{
    // ɾ����ǰ��ѡ�������
    if (IsSelectionExist())
    {
        bUpdate = true;
        this->DeleteSelectionText();

        m_nSelStart = m_nCaret;
        this->Fire_Text_Changed();
    }
}   

//
//	ɾ����ǰѡ���ڵ�����
//
//	ע���ú������ᴥ��text changed
//
void EditData::DeleteSelectionText()
{
 	if (IsSelectionExist())
 	{
 		if (m_nCaret > m_nSelStart)  // ��ǰ����ѡ��
 		{
 			this->m_strText.erase(m_nSelStart, m_nCaret-m_nSelStart);
 			m_nCaret = m_nSelStart;
 		}
 		else                   // �Ӻ���ǰѡ���
 		{
 			this->m_strText.erase(m_nCaret , m_nSelStart-m_nCaret);
			m_nSelStart = m_nCaret;
 		}
 	}
}
bool EditData::IsSelectionExist()
{
	return (m_nCaret!=m_nSelStart);
}

//
// BOOL bSetText����ʾ�Ƿ�����Ϊ����Edit.SetText��������Change
//
void EditData::Fire_Text_Changed(BOOL bSetText)
{
	this->StringAnalysis();
	this->CP2X(m_strText.length(), &m_nTextWidth);

    UIMSG msg;
    msg.pMsgFrom = m_pEdit->GetIEdit();
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_EN_CHANGE;
    msg.wParam = (WPARAM)bSetText;
    m_pEdit->GetIEdit()->DoNotify(&msg);
}

bool EditData::FilterString(const TCHAR* szSrc, String& strDest)
{
    if (szSrc)
        strDest = szSrc; // TODO

	return true;
}
bool EditData::FilterChar(const TCHAR& cSrc, TCHAR& cDest)
{
	cDest = cSrc;      // TODO
	return true;
}

void EditData::SetMaxChar(int nMax)
{
	bool bUpdate = false;
	this->SetMaxChar(nMax, bUpdate);
}

void EditData::SetMaxChar(int nMax, bool bUpdate)
{
	bUpdate = false;
	m_nMaxChar = nMax;

	if (-1 != m_nMaxChar && (int)m_strText.length() > m_nMaxChar)
	{
		bUpdate = true;
		m_strText = m_strText.substr(0,m_nMaxChar);
		this->Fire_Text_Changed();
	}
}

void EditData::SetInsertMode( bool bInsertOrOverride)
{
	m_bInsertMode = bInsertOrOverride;
}

// ���ⲿ���巢���ı�ʱ����Ҫ���´���
void EditData::DestroyStringAnalysis()
{
	if (m_Analysis)
	{
		ScriptStringFree(&m_Analysis);
		m_Analysis = NULL;
	}
}
//
//	��ʼ����ǰ�ַ���m_Analysis
//
bool EditData::StringAnalysis()
{
	if (m_Analysis)
	{
		ScriptStringFree(&m_Analysis);
		m_Analysis = NULL;
	}

    IUIApplication* pUIApp = m_pEdit->GetIEdit()->GetUIApplication();
	HDC    hDC =  pUIApp->GetCacheDC();
	IRenderFont* pRenderFont = m_pEdit->GetIEdit()->GetRenderFont();
	HFONT  hFont = pRenderFont->GetHFONT();
	HFONT  hOldFont = (HFONT)::SelectObject(hDC, hFont);

	HRESULT hr = ScriptStringAnalyse(
		hDC,
		this->m_strText.c_str(),
		this->m_strText.length() + 1,			// ����NULL.��֤�����Ե������һ���ַ��ĺ��档
		this->m_strText.length()*3/2 + 16,      // MSDN�Ƽ�ֵ
		-1,
		SSA_BREAK | SSA_GLYPHS | SSA_FALLBACK | SSA_LINK,
		0,
		&m_ScriptControl,
		&m_ScriptState,
		NULL,
		NULL,
		NULL,
		&m_Analysis
		);

	::SelectObject(hDC, hOldFont);
	pUIApp->ReleaseCacheDC(hDC);
	if (FAILED(hr) || NULL == m_Analysis)
		return false;

	return true;
}

bool EditData::CP2X(int nCP, int* pX)
{
	if (NULL == pX)
		return false;

	if (NULL == m_Analysis)
	{
		if (false ==this->StringAnalysis())
			return false;
	}

	int	nX = 0;
	HRESULT hr = ScriptStringCPtoX(
		m_Analysis, 
		nCP,           // Ҫ�����һ���ַ�
		FALSE,		   // ������ַ�ǰ�滹�Ǻ���
		pX			   // ����ֵ
		);	

	if (FAILED(hr))
		return false;

	return true;
}

bool EditData::X2CP(int nX, int* pnCP, int* pbTrailOrLead)
{
	if (NULL == pnCP || NULL == pbTrailOrLead)
		return false;

	if (NULL == m_Analysis)
	{
		if (false ==this->StringAnalysis())
			return false;
	}

	HRESULT hr = ScriptStringXtoCP(
		m_Analysis, 
		nX,
		pnCP,			
		pbTrailOrLead		// ������ַ�ǰ�滹�Ǻ���
		);	

	if (FAILED(hr))
		return false;

	return true;
}


// If nStartChar is 0 and nEndChar is �C1, all the text in the edit control is selected. 
// If nStartChar is �C1, any current selection is removed.
void EditData::SetSelectionInfo(int nStart, int nEnd, bool& bUpdate)
{
	if (-1 == nStart)
	{
//		m_nSelStart = m_nCaret;
		SetCaret(m_nCaret, true, bUpdate);
	}
	else if (-1 == nEnd)
	{
// 		m_nSelStart = nStart;
// 		m_nCaret = m_strText.length();

		SetCaret(nStart, true, bUpdate);
		SetCaret(m_strText.length(), false, bUpdate);
	}
	else
	{
// 		m_nSelStart = nStart;
// 		m_nCaret = nEnd;

		SetCaret(nStart, true, bUpdate);
		SetCaret(nEnd, false, bUpdate);
	}
}

void EditData::GetSelectionInfo(int& nLeft, int& nRight) const
{
	if (m_nSelStart < m_nCaret)
	{
		nLeft = m_nSelStart;
		nRight = m_nCaret;
	}
	else
	{
		nLeft = m_nCaret;
		nRight = m_nSelStart;
	}
}

void EditData::GetSelectionText(String& str)
{
	int nLeft = 0, nRight = 0;
	this->GetSelectionInfo(nLeft, nRight);

	str = m_strText.substr(nLeft, nRight-nLeft);
}

void EditData::CutToClipboard()
{
	if (IsSelectionExist())
	{
		this->CopyToClipboard();

		bool bUpdate = false;
		this->Delete(bUpdate);
	}
}

void EditData::CopyToClipboard()
{
	if (IsSelectionExist() && OpenClipboard(NULL))
	{
		EmptyClipboard();

		String strSelectionText;
		this->GetSelectionText(strSelectionText);

		int nSize = sizeof(TCHAR) * (strSelectionText.length() + 1);
		HGLOBAL hBlock = GlobalAlloc(GMEM_MOVEABLE, nSize);
		if (hBlock)
		{
			void* p = GlobalLock( hBlock);
			memcpy(p, strSelectionText.c_str(), nSize);
			GlobalUnlock( hBlock);
		}
		SetClipboardData (CF_UNICODETEXT, hBlock);

		CloseClipboard();
		// We must not free the object until CloseClipboard is called.
		if (hBlock)
			GlobalFree( hBlock);
	}
}


void EditData::PasteFromClipboard()
{
	DeleteSelectionText();

	if (OpenClipboard(NULL))
	{
		HANDLE handle = GetClipboardData( CF_UNICODETEXT);
		if (handle)
		{
			// Convert the ANSI string to Unicode, then
			// insert to our buffer.
			WCHAR* pwszText = ( WCHAR*)GlobalLock( handle);
			if (pwszText)
			{
				// Copy all characters up to null.
				bool bUpdate = false;
				ReplaceString(String(pwszText), bUpdate);
				GlobalUnlock(handle);
			}
		}
		CloseClipboard();
	}
}




//////////////////////////////////////////////////////////////////////////

Edit::Edit()
{
    m_pIEdit = NULL;
	this->m_pColor = NULL;
	this->m_pColorSelect = NULL;
	this->m_pColorSelectBk= NULL;
	
	m_bMouseDrag   = false;
	m_bNeedUpdateCaretPos = false;
    m_bNeedFixGdiAlpha = false;

	m_nXScroll     = 0;
	m_nCaretHeight = 16;

	m_nXSelStart   = 0;
	m_nXSelEnd	   = 0;
	m_nXCaretPos   = 0;

	m_nDrawFlags   = 0;

	m_EditData.BindToEdit(this);
  //  GetTsf()->Init();
}


Edit::~Edit()
{
  //  GetTsf()->Release();
	SAFE_RELEASE(m_pColor);
	SAFE_RELEASE(m_pColorSelect);
	SAFE_RELEASE(m_pColorSelectBk);
}

const TCHAR*  Edit::GetText()
{
    return this->m_EditData.GetText().c_str();
}

int Edit::GetTextLength()
{
	return this->m_EditData.GetText().length();
}
// int Edit::GetText(TCHAR* szBuffer, int nBufferSize)
// {
// 	int nLength = this->GetTextLength();
// 	if (nBufferSize <= 0)
// 	{
// 		return nLength+1;
// 	}
// 	
// 	if (NULL == szBuffer)
// 		return 0;
// 
// 	int nCpy = min(nBufferSize-1, nLength);
// 
// 	_tcsncpy(szBuffer, m_EditData.GetText().c_str(), nCpy);
// 	szBuffer[nCpy] = _T('');
// 
// 	return nCpy;
// }

// 
void Edit::SetText(const TCHAR* szText)
{
	bool  bUpdate = false;
    bool  bUpdate2 = false;
    int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.SetText(szText, bUpdate);
    this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate2);

    if (bUpdate || bUpdate2)
    {
        m_pIEdit->UpdateObject();
    }
    if (nOldXCaretPos != m_nXCaretPos)
    {
        this->UpdateCaretByPos();
    }
}

int  Edit::GetEditStyle()
{
    return GETCONTROLSUBTYPE(m_pIEdit->GetStyleEx());
}
void Edit::SetEditStyle(int n)
{
    m_pIEdit->SetStyleEx(MAKECONTROLSUBTYPE(m_pIEdit->GetStyleEx(), n));
}

void  Edit::DeleteSelectionText(bool bUpdate)
{
    bool bNeedUpdate1 = false;
    bool bNeedUpdate2 = false;
    int  nOldXCaretPos = m_nXCaretPos;

    m_EditData.DeleteSelectionText(bNeedUpdate1);
    this->CalcCaretPos(m_EditData.GetCaretIndex(), bNeedUpdate2);

    if (bUpdate && (bNeedUpdate1 || bNeedUpdate2))
    {
        m_pIEdit->UpdateObject();
    }
    if (nOldXCaretPos != m_nXCaretPos)
    {
        this->UpdateCaretByPos();
    }
}

bool  Edit::DoCut()
{
    OnKeyDown_Ctrl_X();
    return true;
//     if (DoCopy())
//     {
//         this->DeleteSelectionText(true);
//         return true;
//     }
//     return false;
}
bool  Edit::DoCopy()
{
    OnKeyDown_Ctrl_C();
    return true;
//     String strSelText;
//     m_EditData.GetSelectionText(strSelText);
// 
//     if (0 == strSelText.length())
//         return false;
// 
//     if (::OpenClipboard(m_pIEdit->GetHWND()))
//     {
//         ::EmptyClipboard();
// #ifdef _UNICODE
//         {
//             HGLOBAL hGlobal = ::GlobalAlloc(GMEM_FIXED, sizeof(WCHAR)*(strSelText.length()+1));
//             WCHAR* pBits = (WCHAR*)::GlobalLock(hGlobal);
//             wcscpy(pBits, strSelText.c_str());
//             ::GlobalUnlock(hGlobal);
//             ::SetClipboardData(CF_UNICODETEXT, hGlobal);
//         }
// 
//         {
//             int nSize = Util::U2A(strSelText.c_str(), NULL, 0);
//             char* pAnsiText = new char[nSize];
//             Util::U2A(strSelText.c_str(), pAnsiText, nSize);
// 
//             HGLOBAL hGlobal = ::GlobalAlloc(GMEM_FIXED, nSize+1);
//             char* pBits = (char*)::GlobalLock(hGlobal);
//             strcpy(pBits, pAnsiText);
//             ::GlobalUnlock(hGlobal);
//             SAFE_ARRAY_DELETE(pAnsiText);
//             ::SetClipboardData(CF_TEXT, hGlobal);
//         }
// #else
//         // TODO:
//         UIASSERT(0);
// #endif
//         ::CloseClipboard();
//         return true;
//     }
//     return false;
}

bool  Edit::DoPaste()
{
    OnKeyDown_Ctrl_V();
    return true;
}

void Edit::SetSel(int nStartChar, int nEndChar)
{
	bool bUpdate = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.SetSelectionInfo(nStartChar, nEndChar, bUpdate);
	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate2);

	if (bUpdate || bUpdate2)
	{
		m_pIEdit->UpdateObject();
	}
	if (nOldXCaretPos != m_nXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}
void Edit::GetSel(int& nStartChar,int& nEndChar) const
{
	m_EditData.GetSelectionInfo(nStartChar, nEndChar);
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                                 �麯��                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

void Edit::ResetAttribute()
{
    DO_PARENT_PROCESS(IEdit, IControl);

	m_nDrawFlags = 0;
	m_nCaretHeight = 16;
	SAFE_RELEASE(m_pColor);
	SAFE_RELEASE(m_pColorSelect);
	SAFE_RELEASE(m_pColorSelectBk);
	this->m_EditData.SetMaxChar(-1);
	this->m_EditData.DestroyStringAnalysis();
    m_pIEdit->ModifyStyle(OBJECT_STYLE_ENABLE_IME, 0, false);
}
void Edit::SetAttribute(IMapAttribute* pMatAttrib, bool bReload)
{
    bool bPaddingConfig = pMatAttrib->HasAttrib(XML_PADDING);

	DO_PARENT_PROCESS(IEdit, IControl);

	IColorRes* pColorRes = m_pIEdit->GetUIApplication()->GetActiveSkinColorRes();
	if (NULL == pColorRes)
		return;

	// ��ɫ
	const TCHAR* szText = pMatAttrib->GetAttr(XML_EDIT_COLOR, true);
	if (szText)
		pColorRes->GetColor((BSTR)szText, &this->m_pColor);

	szText = pMatAttrib->GetAttr(XML_EDIT_SELECT_COLOR, true);
	if (szText)
		pColorRes->GetColor((BSTR)szText, &this->m_pColorSelect);

	szText = pMatAttrib->GetAttr(XML_EDIT_SELECT_BK_COLOR, true);
	if (szText)
		pColorRes->GetColor((BSTR)szText, &this->m_pColorSelectBk);

	// ���ֵ
	szText = pMatAttrib->GetAttr(XML_EDIT_MAX_LENGTH, true);
	if (szText)
		this->m_EditData.SetMaxChar(_ttoi(szText));
	

	// �������� 
	if (NULL == m_pIEdit->GetBkRender() && 0 == this->GetEditStyle())
	{
        IRenderBase* p = NULL;
        m_pIEdit->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_EDIT_BKGND, m_pIEdit, &p);
        if (p)
        {
            p->SetAttribute(pMatAttrib, XML_BACKGND_RENDER_PREFIX, true);
            m_pIEdit->SetBkgndRender(p);
            SAFE_RELEASE(p);
		    m_pIEdit->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, 0);

            if (!bPaddingConfig)
            {
                CRegion4 r(3,0,3,0);
                m_pIEdit->SetPaddingRegion(&r);
            }
        }
	}

	// ����(������������ maxlength֮����ܸ�ֵ -- ����¶�����Ը�xml
// 	if (mapAttrib.count( XML_EDIT_TEXT))
// 	{
// 		this->SetText( mapAttrib[XML_EDIT_TEXT]);
// 		__super::m_mapAttribute.erase( XML_EDIT_TEXT);
// 	}

// 	if (NULL == m_pEraseBkgndRender)   // ����һ��Ĭ�ϰ�ɫ����
// 	{
// 		m_pEraseBkgndRender = ObjectEraseBkgndRenderFactory::GetRender(OBJECT_ERASEBKGND_TYPE_COLOR, this);
// 		((ColorEraseBkgndRender*)m_pEraseBkgndRender)->SetBkColor( RGB(255,255,255)); 
// 		((ColorEraseBkgndRender*)m_pEraseBkgndRender)->SetBorderColor( RGB(183,217,237)); 
// 	}

	szText = pMatAttrib->GetAttr(XML_TEXTRENDER_TYPE, true);
	if (szText)
	{
        ITextRenderBase* p = NULL;
        m_pIEdit->GetUIApplication()->CreateTextRenderBaseByName((BSTR)szText, m_pIEdit, &p);
        if (p)
        {
		    p->SetTextAlignment(DT_SINGLELINE|DT_LEFT|DT_VCENTER);
		    p->SetAttribute(pMatAttrib, NULL, true);
            m_pIEdit->SetTextRender(p);
            SAFE_RELEASE(p);
        }
        else
        {
            m_pIEdit->SetTextRender(NULL);
        }
	}
	else if (NULL == m_pIEdit->GetTextRender())
	{
        ITextRenderBase* p = NULL;
        m_pIEdit->GetUIApplication()->CreateTextRenderBase(TEXTRENDER_TYPE_COLORLIST, m_pIEdit, &p);
        if (p)
        {
            p->SetTextAlignment(DT_SINGLELINE|DT_LEFT|DT_VCENTER);
            p->SetAttribute(pMatAttrib, NULL, true);

            IColorListTextRender* pColorListTextRender = static_cast<IColorListTextRender*>(p);
            pColorListTextRender->SetCount(2);
            pColorListTextRender->SetColor(EDIT_TEXTREDNER_STATE_NORMAL, RGBA(0,0,0,255));
            if (m_pColorSelect)
                pColorListTextRender->SetColor(EDIT_TEXTRENDER_STATE_SELECTED, m_pColorSelect->m_col);
            else 
                pColorListTextRender->SetColor(EDIT_TEXTRENDER_STATE_SELECTED, RGBA(255,255,255,255));

            m_pIEdit->SetTextRender(p);
            SAFE_RELEASE(p);
        }
    }

    if (MAPATTR_RET_NOT_EXIST == pMatAttrib->GetAttr_int(XML_EDIT_CARET_HEIGHT, true, &m_nCaretHeight))
    {
        IRenderFont* pFont = m_pIEdit->GetRenderFont();
        if (pFont)
        {
            if (0 == (m_nDrawFlags & EDIT_DRAW_FLAG_CARET_HEIGHT_CONFIGED))
            {
                m_nCaretHeight = pFont->GetTextMetricsHeight();
            }
        }
    }
    else
    {
        m_nDrawFlags |= EDIT_DRAW_FLAG_CARET_HEIGHT_CONFIGED;
    }
}

void  Edit::GetDesiredSize(SIZE* pSize)
{
	String strTest = this->m_EditData.GetText();
	if (strTest.empty())
	{
		strTest = _T("T");  // ��textΪ��ʱ��������GetTextExtentPoint32���صĸ߶�Ϊ0
	}

	pSize->cx = pSize->cy = 0;
	IRenderFont* pFont = m_pIEdit->GetRenderFont();
	if (pFont)
	{
		*pSize = pFont->MeasureString(strTest.c_str());
	}
	
	pSize->cx += m_pIEdit->GetPaddingW();
	pSize->cy += m_pIEdit->GetPaddingH();

	pSize->cy +=6; // ��ֹedit��С����ӵ�ֵ
}


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               ˽�з���                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
/*
**	[virtual] TODO. δʵ��
**
**	����Edit�������й���һЩ����
**
**	����true��ʾ�������룬false��ʾ��ֹ����
*/
bool Edit::FilterInputChar( UINT nChar)
{
	return true;
}

// ע��2013.4.25 Ϊʲô����ʹ��Gdiplus����EDIT����
//     USP�Ķ�λ�ǻ���gdi�����Ű�ģ���Gdiplus�����ֻ��Ʋ�����һ����ֱ����޹صļ�����
//     �������������ʾ��USP�Ķ�λ��һ�£���ʾ����������������GDI+FixGdiAlpha�ķ���
//   
void Edit::DrawNormal(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return;

	CRect rcClient;
	m_pIEdit->GetClientRectAsWin32(&rcClient);
    rcClient.left -= m_nXScroll;

	// ����
 	IRenderFont* pFont = m_pIEdit->GetRenderFont();
    if (GRAPHICS_RENDER_LIBRARY_TYPE_GDI == pFont->GetGraphicsRenderLibraryType())
    {
        if (m_pIEdit->GetTextRender())
            m_pIEdit->GetTextRender()->DrawState(pRenderTarget, &rcClient, EDIT_TEXTREDNER_STATE_NORMAL, m_EditData.GetText().c_str());

        return;
    }
    else
    {
 	    HFONT  hFont = pFont->GetHFONT();

 	    HDC    hDC = pRenderTarget->GetBindHDC();
 	    HFONT  hOldFont = (HFONT)SelectObject(hDC, hFont);

 	    COLORREF oldTextCol = ::SetTextColor(hDC, 0x00000000);
     
 	    CRect rcText = rcClient;
 	    rcText.top = rcClient.top + (rcClient.Height() - m_nCaretHeight)/2;
 	    rcText.bottom = rcText.top + m_nCaretHeight;
     
  	    ExtTextOut( 
  		    hDC, rcClient.left, rcText.top, 
  		    ETO_CLIPPED, 
  		    &rcText, 
  		    m_EditData.GetText().c_str(), 
  		    m_EditData.GetTextLength(), NULL);

        ::SetTextColor(hDC, oldTextCol);
        ::SelectObject(hDC, hOldFont);
    }
#ifdef _DEBUG
	{
	//	UI_LOG_DEBUG(_T("Edit::Draw, CaretIndex:%d, CaretPos:%d, ScrollX:%d, String:%s"),
	//		m_EditData.GetCaretIndex(), m_nXCaretPos, m_nXScroll, m_EditData.GetText().c_str());
	}
#endif
}

void Edit::DrawFocus(IRenderTarget* pRenderTarget)
{
	CRect rcClient;
	m_pIEdit->GetClientRectAsWin32(&rcClient);

	// ���Ʊ�ѡ�������
	if (m_EditData.IsSelectionExist())
	{
		int nSelLeft = 0;
		int nSelRight = 0;
		m_EditData.GetSelectionInfo(nSelLeft, nSelRight);
		
		int x = 0, x2 = 0; 
		if (m_EditData.GetCaretIndex() == nSelLeft)
		{
			x = m_nXCaretPos;
			
			m_EditData.CP2X( nSelRight, &x2);
			x2 -= m_nXScroll;
		}
		else
		{
			m_EditData.CP2X( nSelLeft, &x);
			x -= m_nXScroll;

			x2 = m_nXCaretPos;
		}

		rcClient.left = x;
		rcClient.right = x2;

		// ����ѡ�б���ɫ
		CRect rcSelBk = rcClient;
		rcSelBk.top = rcClient.top + (rcClient.Height() - m_nCaretHeight)/2;
		rcSelBk.bottom = rcSelBk.top + m_nCaretHeight;
		if (m_pColorSelectBk)
        {
			pRenderTarget->FillRect(&rcSelBk, m_pColorSelectBk);
        }
		else
        {
            Color c(49,106,197,255);
			pRenderTarget->FillRect(&rcSelBk, &c);
        }

		// ����ѡ������
		String selString;
		m_EditData.GetSelectionText(selString);

        IRenderFont* pFont = m_pIEdit->GetRenderFont();
        if (GRAPHICS_RENDER_LIBRARY_TYPE_GDI == pFont->GetGraphicsRenderLibraryType())
        {
            if (m_pIEdit->GetTextRender())
		        m_pIEdit->GetTextRender()->DrawState(pRenderTarget, &rcClient, EDIT_TEXTRENDER_STATE_SELECTED, selString.c_str());
        }
        else
        {
            HFONT  hFont = pFont->GetHFONT();
            HDC    hDC = pRenderTarget->GetBindHDC();
            HFONT  hOldFont = (HFONT)SelectObject(hDC, hFont);

            COLORREF oldTextCol = ::SetTextColor(hDC, 0x00FFFFFF);

            String selString;
            m_EditData.GetSelectionText(selString);

            CRect rcText = rcClient;
            rcText.top = rcClient.top + (rcClient.Height() - m_nCaretHeight)/2;
            rcText.bottom = rcText.top + m_nCaretHeight;

            ExtTextOut( 
                hDC, x, rcText.top, 
                ETO_CLIPPED, 
                &rcText, 
                selString.c_str(),
                nSelRight-nSelLeft, NULL);

            ::SetTextColor(hDC, oldTextCol);
            ::SelectObject(hDC, hOldFont);
        }

#if 0
		IRenderFont* pFont = this->GetFont();
		HFONT  hFont = pFont->GetHFONT();
		HDC    hDC = pRenderTarget->GetHDC();
		SelectObject(hDC, hFont);

		COLORREF oldBkCol = ::SetBkColor(hDC, RGB(49,106,197));
		COLORREF oldTextCol = ::SetTextColor(hDC, RGB(255,255,255));
		int		 oldBkMode = ::SetBkMode(hDC, OPAQUE);

		// m_nSelCount���Ϊ�����Ļ�������ʱ��Ӻ���ǰѡ��

		String selString;
		m_EditData.GetSelectionText(selString);

		// �Ƿ���Կ���ʹ��MaskBlt(���ÿ��ǻ���Clip)

		CRect rcText = rcClient;
		rcText.top = rcClient.top + (rcClient.Height() - m_nCaretHeight)/2;
		rcText.bottom = rcText.top + m_nCaretHeight;

		ExtTextOut( 
			hDC, x, rcText.top, 
			ETO_CLIPPED, 
			&rcText, 
			selString.c_str(),
			nSelRight-nSelLeft, NULL);

		::SetBkMode( hDC, oldBkMode);
		::SetTextColor( hDC, oldTextCol);
		::SetBkColor( hDC, oldBkCol);

		pRenderTarget->ReleaseHDC(hDC);
#endif
	}
}

//
//	�����λ�������ڵ�N���ַ�ǰ�档�ú������������ݣ����������ù��λ��
//
void Edit::CalcCaretPos(int nCaretIndex, bool& bUpdate)
{
	bUpdate = false;

	int nX = 0;
	m_EditData.CP2X(nCaretIndex, &nX);
	
	CRect rcClient;
	m_pIEdit->GetClientRect(&rcClient);

	int nEditW  = rcClient.Width();
	int nXLeft  = m_nXScroll;
	int nXRight = m_nXScroll + nEditW;

	// �������ֹ���
	// ���1�����й���ʱ��Edit����治Ӧ���ǿյġ�
	if (m_nXScroll  > 0)
	{
		int nRemain = nXRight - m_EditData.GetTextWidth();
		if (nRemain > 0)	// �ұ���ʣ��ռ�
		{
			m_nXScroll -= nRemain;
			if (m_nXScroll < 0)
				m_nXScroll = 0;

			bUpdate = true;
		}
	}
	nXLeft = m_nXScroll;
	nXRight = nXLeft + nEditW;

	// ���2����곬����߽�
	if (nX < nXLeft)
	{
		m_nXScroll = nX;
		bUpdate = true;
		m_nXCaretPos = 0;
	}
	// ���3����곬���ұ߽�
	else if (nX > nXRight)
	{
		m_nXScroll += (nX - nXRight);
		bUpdate = true;
		m_nXCaretPos = nEditW;
	}
	// ������ƶ�
	else
	{
		m_nXCaretPos = nX-m_nXScroll;
	}
}

void Edit::UpdateCaretByPos()
{
	CRect rcClient;
	m_pIEdit->GetClientRect/*InWindow*/(&rcClient);

	// ������TEXTһ�����������
	int y = rcClient.top + (rcClient.Height()-m_nCaretHeight)/2;
	POINT ptCaret = {m_nXCaretPos + rcClient.left, y};

    m_caret.HideCaret(false);
	if (ptCaret.x < rcClient.left || ptCaret.x > rcClient.right)
	{
		// �������������
		m_caret.SetCaretPos(m_pIEdit, rcClient.right,y, false); // ��Win7һ�����ں��档�ǻ᲻����ĳЩ�������Ҫ����ǰ�棿
		//m_caret.SetCaretPos(-1,-1); ���ֻ��ϵͳ�����Ч����ģ����û��
	}
	else
	{
		m_caret.SetCaretPos(m_pIEdit, ptCaret.x, ptCaret.y);
	}
    m_caret.ShowCaret(m_pIEdit, true);
}

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                                 ��Ϣ��Ӧ                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


void Edit::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    IRenderBase* pBkgndRender = m_pIEdit->GetBkRender();
	if (pBkgndRender)
	{
		CRect rc(0, 0, m_pIEdit->GetWidth(), m_pIEdit->GetHeight());
		if (!m_pIEdit->IsEnable())
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, EDIT_BKGND_RENDER_STATE_DISABLE);
		}
		else if (m_pIEdit->IsFocus() || m_pIEdit->IsPress())
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, EDIT_BKGND_RENDER_STATE_PRESS);
		}
		else if (m_pIEdit->IsHover())
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, EDIT_BKGND_RENDER_STATE_HOVER);
		}
		else
		{
			pBkgndRender->DrawState(pRenderTarget, &rc, EDIT_BKGND_RENDER_STATE_NORMAL);
		}
	}

    // ��Ϊm_caret��λ���ǻ���edit�ģ�������client
    m_caret.OnControlPaint(m_pIEdit, pRenderTarget);
}
void Edit::OnPaint(IRenderTarget* pRenderTarget)
{
    if (m_bNeedUpdateCaretPos)
    {
        m_bNeedUpdateCaretPos = false;
        this->UpdateCaretByPos();
    }

	this->DrawNormal(pRenderTarget);

	if (m_pIEdit->IsFocus())
		this->DrawFocus(pRenderTarget);

    if (m_bNeedFixGdiAlpha)
    {
        CRect  rc;
        m_pIEdit->GetObjectVisibleClientRect(&rc, false);
        Util::FixGdiAlpha(pRenderTarget->GetBindHDC(), &rc);
    }
}

BOOL Edit::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message)
{
	if (m_pIEdit->IsEnable())
		::SetCursor(::LoadCursor(NULL, IDC_IBEAM));
	else
		SetMsgHandled(FALSE);
	
	return TRUE;
}

void Edit::OnSetFocus(IObject*)
{
    m_caret.CreateCaret(m_pIEdit, NULL, 1, m_nCaretHeight, /*m_bNeedFixGdiAlpha?CARET_TYPE_WINDOW:*/CARET_TYPE_UNKNOWN);
	this->UpdateCaretByPos();
    m_caret.ShowCaret(m_pIEdit);
}
void Edit::OnKillFocus(IObject*)
{
	m_bMouseDrag = false;
	m_caret.DestroyCaret(m_pIEdit, false);
    m_pIEdit->UpdateObject();
}

void Edit::OnFontModifyed(IRenderFont* pFont)
{
	if (pFont == m_pIEdit->GetRenderFont())
	{
		if (pFont)
		{
			if (0 == (m_nDrawFlags & EDIT_DRAW_FLAG_CARET_HEIGHT_CONFIGED))
				m_nCaretHeight = pFont->GetTextMetricsHeight();

			m_bNeedUpdateCaretPos = true;
			m_EditData.DestroyStringAnalysis();  // ����ı����Ҫ���´���SA
		}
	}
}

void Edit::OnWindowPosChanged(LPWINDOWPOS)
{
    // onsize����Ҫ���¸����ı����������λ�õ�
    bool bNeedUpdate = false;
    CalcCaretPos(m_EditData.GetCaretIndex(), bNeedUpdate);

	// ע�����������ֱ��ȥ���ù���λ�ã��ᵼ�¹�����ƶ�����λ��
	//     Ȼ�����ˢ�£�������µ���λ�ã���˹�������ûͬ���ƶ���
    //     Ҳ�п��ܹ����Ƴ���EDIT��Χ����ʱ������ʾ���
	m_bNeedUpdateCaretPos = true;
}

//
//	BACKSPACE ��ǰɾ�������һ��WM_CHAR�¼�
//
//  case 24:  // Ctrl-X Cut
//  case VK_CANCEL: // Ctrl-C Copy
//  case VK_BACK:   // Backspace
//  case 22:  // Ctrl-V Paste
//  case 1:   // Ctrl-A Select All
//  case 26:  // Ctrl Z
//  case 2:   // Ctrl B
//  case 14:  // Ctrl N
//  case 19:  // Ctrl S
//  case 4:   // Ctrl D
//  case 6:   // Ctrl F
//  case 7:   // Ctrl G
//  case 10:  // Ctrl J
//  case 11:  // Ctrl K
//  case 12:  // Ctrl L
//  case 17:  // Ctrl Q
//  case 23:  // Ctrl W
//  case 5:   // Ctrl E
//  case 18:  // Ctrl R
//  case 20:  // Ctrl T
//  case 25:  // Ctrl Y
//  case 21:  // Ctrl U
//  case 9:   // Ctrl I
//  case 15:  // Ctrl O
//  case 16:  // Ctrl P
//  case 27:  // Ctrl [
//  case 29:  // Ctrl ]
//  case 28:  // Ctrl \ 
//
//
// case 0x09:          // Tab 
// 	// Tab stops exist every four spaces, so add 
// 	// spaces until the user hits the next tab. 
// 
// 	do 
// 	{ 
// 		SendMessage(hwnd, WM_CHAR, ' ', 1L); 
// 	} while (nCaretPosX % 4 != 0); 
// 	break; 
//
//
void Edit::OnChar( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_ESCAPE == nChar || VK_RETURN == nChar || VK_TAB == nChar)
		return;

	bool bCtrlDown = Util::IsKeyDown(VK_CONTROL);

	// ������Ϣ����
	if (VK_BACK == nChar)
	{
		this->OnKeyDown_Backspace(bCtrlDown);
		return ;
	}

	// �����ַ�����
	if (false == bCtrlDown) 
	{
		this->OnInputChar(nChar);
		return;
	}

	return ;
}
void Edit::OnInputChar(UINT nChar)
{
	if (IsReadOnly())
		return;

	if (false == FilterInputChar(nChar))
		return;

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	this->m_EditData.ReplaceChar(nChar, bUpdate1);
	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate2);

    if (nOldXCaretPos != m_nXCaretPos)
    {
        this->UpdateCaretByPos();
    }

	if (bUpdate1 || bUpdate2)
	{
		m_pIEdit->UpdateObject();
	}
	
}

void Edit::OnLButtonDown(UINT nFlags, POINT point)
{
	// �����λ��ת��Ϊ����ַ�����λ��
	POINT ptClient;
	m_pIEdit->WindowPoint2ObjectClientPoint(&point, &ptClient);
	int x = ptClient.x + m_nXScroll;
	//int x = point.x - m_rcWindow.left - m_rcPadding.left + m_nXScroll;
	
	int nCP = 0, bTrailOrLead = 0;
	m_EditData.X2CP( x, &nCP, &bTrailOrLead);
	nCP += bTrailOrLead;
	 
	if (nCP > (int)this->m_EditData.GetTextLength())
		nCP = this->m_EditData.GetTextLength();

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.SetCaret( nCP, true, bUpdate1);
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2);

	if (bUpdate1 || bUpdate2)
	{
		m_pIEdit->UpdateObject();
	}
	if (nOldXCaretPos != m_nXCaretPos && m_pIEdit->IsFocus())
	{
		this->UpdateCaretByPos();
	}

	m_bMouseDrag = true;
}

void Edit::OnLButtonUp(UINT nFlags, POINT point)
{
	m_bMouseDrag = false;
}	

void Edit::OnRButtonDown(UINT nFlags, CPoint point)
{
	SetMsgHandled(FALSE);

// 	if (!m_pIEdit->IsFocus())
// 	{
// 		IWindowBase* pWindow = m_pIEdit->GetWindowObject();
// 		if (pWindow)
// 		{
// 			pWindow->GetKeyboardMgr()->SetFocusObject(m_pIEdit->GetObjectImpl());
// 		}
// 	}
}

void Edit::OnRButtonUp(UINT nFlags, CPoint point)
{
    IMenu* pMenu = NULL;
    IMenu::CreateInstance(m_pIEdit->GetUIApplication(), &pMenu);
    pMenu->InitDefaultAttrib();

#define MENU_ID_CUT     1
#define MENU_ID_COPY    2
#define MENU_ID_PASTE   3
#define MENU_ID_SELALL  4

    IListItemBase* pCutItem   = pMenu->AppendString(_T("����"), MENU_ID_CUT);
    IListItemBase* pCopyItem  = pMenu->AppendString(_T("����"), MENU_ID_COPY);
    IListItemBase* pPasteItem = pMenu->AppendString(_T("ճ��"), MENU_ID_PASTE);
    pMenu->AppendSeparator(-1);
    IListItemBase* pSelAllItem = pMenu->AppendString(_T("ȫѡ"), MENU_ID_SELALL);
    
    if (!m_EditData.IsSelectionExist())
    {
        pCutItem->SetDisable(true);
        pCopyItem->SetDisable(true);
    }
    if (!::IsClipboardFormatAvailable(CF_TEXT) &&
        !::IsClipboardFormatAvailable(CF_UNICODETEXT))
    {
        pPasteItem->SetDisable(true);
    }
    if (0 == GetTextLength())
    {
        pSelAllItem->SetDisable(true);
    }
    
    CPoint pt;
    ::GetCursorPos(&pt);
    UINT nRetCmd = pMenu->TrackPopupMenu(TPM_RETURNCMD, pt.x, pt.y, m_pIEdit);
    SAFE_DELETE_Ixxx(pMenu);

    switch (nRetCmd)
    {
    case MENU_ID_CUT:
        DoCut();
        break;

    case MENU_ID_COPY:
        DoCopy();
        break;

    case MENU_ID_PASTE:
        DoPaste();
        break;

    case MENU_ID_SELALL:
        SetSel(0, -1);
        break;
    }
}

//
//	TODO: �� aa___bb
//			     |������˫�����ᵼ��aabb����ѡ��
//
void Edit::OnLButtonDblClk(UINT nFlags, POINT point)
{
	m_bMouseDrag = false;  // Ϊ�˷�ֹ���ڹ���λ�øı䣬�����ָ������ڷ�����һ��MOUSEMOVE��Ϣ��ʹ��˫��ѡ�е�ѡ���������޸�
						   // ����ǿ�ƽ�m_bMouseDrag�ÿգ����ٴ�����ק

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	int nCaretStart = 0, nCaretEnd = 0;
	m_EditData.GetPriorItemPos(m_EditData.GetCaretIndex(), &nCaretStart);
	m_EditData.GetNextItemPos(m_EditData.GetCaretIndex(), &nCaretEnd);

	m_EditData.SetCaret(nCaretStart, true,  bUpdate1);
	m_EditData.SetCaret(nCaretEnd, false,  bUpdate1);
	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate2);

	m_pIEdit->UpdateObject();
	
	if (nOldXCaretPos != m_nXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}
void Edit::OnMouseMove(UINT nFlags, POINT point)
{
	if (m_bMouseDrag)
	{
		// �����λ��ת��Ϊ����ַ�����λ��
		POINT ptWindow = m_pIEdit->GetRealPosInWindow();
		int x = point.x - ptWindow.x - m_pIEdit->GetPaddingL() + m_nXScroll;

		int nCP = 0, bTrailOrLead = 0;
		m_EditData.X2CP( x, &nCP, &bTrailOrLead);
		nCP += bTrailOrLead;

		if (nCP > (int)this->m_EditData.GetTextLength())
			nCP = this->m_EditData.GetTextLength();

		bool bUpdate1 = false, bUpdate2 = false;
		int  nOldXCaretPos = m_nXCaretPos;

		m_EditData.SetCaret( nCP, false, bUpdate1);
		this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2);

		if (bUpdate1 || bUpdate2)
		{
			m_pIEdit->UpdateObject();
		}
		if (nOldXCaretPos != m_nXCaretPos)
		{
			this->UpdateCaretByPos();
		}
	}
}



/*
**	DELETE          ����ɾ��,ɾ��ѡ��
**  SHIFT + DELETE  ��ǰɾ��,ɾ��ѡ��
**
**	VK_LEFT         �����ǰ�ƶ�
**  VK_RIGHT        ��������ƶ�
**  VK_UP           �����ǰ�ƶ�
**  VK_DOWN         ��������ƶ�
**
**	SHIFT + LEFT    ��ǰѡ��
**	SHIFT + UP      ��ǰѡ��
**	SHIFT + RIGHT   ����ѡ��
**	SHIFT + DOWN    ����ѡ��
**
*/
void Edit::OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags)
{
	bool bCtrlDown = Util::IsKeyDown(VK_CONTROL);
	if (bCtrlDown)
	{
        switch (nChar)
        {
        case _T('A'):
		    {
			    this->OnKeyDown_Ctrl_A();
			    return ;
		    }
            break;
        case _T('X'):
		    {
			    this->OnKeyDown_Ctrl_X();
			    return;
		    }
            break;
        case _T('C'):
		    {
			    this->OnKeyDown_Ctrl_C();
			    return ;
		    }
            break;
        case _T('V'):
		    {
			    this->OnKeyDown_Ctrl_V();
			    return ;
		    }
            break;
        case _T('Z'):
		    {
			    this->OnKeyDown_Ctrl_Z();
			    return;
		    }
            break;
        case _T('Y'):
		    {
			    this->OnKeyDown_Ctrl_Y();
			    return;
		    }
            break;
        }
	}

    switch(nChar)
    {
    case VK_DELETE:
	    {
		    this->OnKeyDown_Delete(bCtrlDown);
		    return ;
	    }
        break;
// 	case VK_BACK:   // ��VK_DELETE��һ����VK_BACK�����WM_CHAR��Ϣ����˽�����Ϣ��OnChar�д���
// 	{
// 		this->OnKeyDown_Backspace();
// 		return ;
// 	}
//      break;         
    case VK_LEFT:
    case VK_UP:
	    {
		    this->OnKeyDown_Left_Top(bCtrlDown);
		    return;
	    }
        break;
    case VK_RIGHT:
    case VK_DOWN:
	    {
		    this->OnKeyDown_Right_Down(bCtrlDown);
		    return;
	    }
        break;
    case VK_HOME:
	    {
		    this->OnKeyDown_Home(bCtrlDown);
		    return;
	    }
        break;
    case VK_END:
	    {
		    this->OnKeyDown_End(bCtrlDown);
		    return ;
	    }
        break;
    case VK_INSERT:
	    {
		    this->OnKeyDown_Insert(bCtrlDown);
	    }
        break;

    case VK_RETURN:
        {
            OnKeyDown_Return();
        }
        break;
    case VK_ESCAPE:
        {
            OnKeydown_Esc();
        }
        break;

    case VK_TAB:
        {
            OnKeydown_Tab();
        }
        break;
    }
}

void Edit::OnKeyDown_Ctrl_A()
{
	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;
	int  nOldSelectionLen = m_EditData.GetSelectionLength();
	
	m_EditData.SetCaret(0,true,bUpdate1);
	m_EditData.SetCaret(m_EditData.GetTextLength(),false,bUpdate1);
	this->CalcCaretPos(m_EditData.GetCaretIndex(),bUpdate2);

	if (nOldSelectionLen != m_EditData.GetTextLength() || bUpdate2)  // ע�����ﲻ�����ǵ���������SetCaret�����µڶ��ε��õ�bUpdate1�ض�Ϊtrue�����������ͨ���жϾɵ�ѡ�������������Ƿ���Ҫˢ��
	{
		m_pIEdit->UpdateObject();
	}
	if (nOldXCaretPos != m_nXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}

void Edit::OnKeyDown_Ctrl_X()
{
	if (IsReadOnly())
	{
		this->OnKeyDown_Ctrl_C();
		return;
	}

	if (m_EditData.IsSelectionExist())
	{
		m_EditData.CutToClipboard();

		bool bUpdate = false;
		int  nOldXCaretPos = m_nXCaretPos;
		this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate);

		m_pIEdit->UpdateObject();
		
		if (nOldXCaretPos != m_nXCaretPos)
		{
			this->UpdateCaretByPos();
		}
	}
}

void Edit::OnKeyDown_Ctrl_C()
{
	m_EditData.CopyToClipboard();
}
void Edit::OnKeyDown_Ctrl_V()
{
	if (IsReadOnly())
		return;

	bool bUpdate = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.PasteFromClipboard();
	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate);

	// ǿ��ˢ��
	m_pIEdit->UpdateObject();

	if (nOldXCaretPos != m_nXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}
void Edit::OnKeyDown_Ctrl_Z()
{

}
void Edit::OnKeyDown_Ctrl_Y()
{

}
void Edit::OnKeyDown_Backspace(bool bCtrlDown)
{
	if (IsReadOnly())
		return;

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.BackSpace(bUpdate1);
	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate2);

	if (bUpdate1 || bUpdate2)
	{
		m_pIEdit->UpdateObject();
	}
	if (nOldXCaretPos != m_nXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}
void Edit::OnKeyDown_Delete(bool bCtrlDown)
{
	if (IsReadOnly())
		return;

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	bool bShiftDown =  Util::IsKeyDown( VK_SHIFT);
	if (bShiftDown)  // SHIFT + DELETE ��ǰɾ��
	{
		 m_EditData.BackSpace(bUpdate1);
	}
	else             // DELETE ����ɾ��
	{ 
		m_EditData.Delete(bUpdate1);
	}

	this->CalcCaretPos(m_EditData.GetCaretIndex(), bUpdate2);

	if (bUpdate1 || bUpdate2)
	{
		m_pIEdit->UpdateObject();
	}
	if (nOldXCaretPos != m_nXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}

void Edit::OnKeyDown_Left_Top(bool bCtrlDown)
{
	bool bShiftDown = Util::IsKeyDown( VK_SHIFT);

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	int nDestCaret = 0;
	if (bCtrlDown)
	{
		m_EditData.GetPriorItemPos(m_EditData.GetCaretIndex(), &nDestCaret);
	}
	else
	{
		nDestCaret = m_EditData.GetCaretIndex()-1;
	}

	m_EditData.SetCaret(nDestCaret, !bShiftDown, bUpdate1);
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2);

	
	if (bUpdate1 || bUpdate2)
	{
		m_pIEdit->UpdateObject();
	}
	if (nOldXCaretPos != m_nXCaretPos)
	{
		this->UpdateCaretByPos();
	}
	
}
void Edit::OnKeyDown_Right_Down(bool bCtrlDown)
{
	bool bShiftDown = Util::IsKeyDown( VK_SHIFT);

	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	int nDestCaret = 0;
	if (bCtrlDown)
	{
		m_EditData.GetNextItemPos(m_EditData.GetCaretIndex(), &nDestCaret);
	}
	else
	{
		nDestCaret = m_EditData.GetCaretIndex()+1;
	}

	m_EditData.SetCaret(nDestCaret, !bShiftDown, bUpdate1);
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2);

	if (bUpdate1 || bUpdate2)
	{
		m_pIEdit->UpdateObject();
	}
	if (m_nXCaretPos != nOldXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}

void Edit::OnKeyDown_Home(bool bCtrlDown)
{
	bool bShiftDown =  Util::IsKeyDown( VK_SHIFT);
	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.SetCaret(0, !bShiftDown, bUpdate1);
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2);

	if (bUpdate1 || bUpdate2)
	{
		m_pIEdit->UpdateObject();
	}
	if (m_nXCaretPos != nOldXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}
void Edit::OnKeyDown_End(bool bCtrlDown)
{

	bool bShiftDown = Util::IsKeyDown( VK_SHIFT);
	bool bUpdate1 = false, bUpdate2 = false;
	int  nOldXCaretPos = m_nXCaretPos;

	m_EditData.SetCaret(m_EditData.GetTextLength(), !bShiftDown, bUpdate1);
	this->CalcCaretPos( m_EditData.GetCaretIndex(), bUpdate2);

	if (bUpdate1 || bUpdate2)
	{
		m_pIEdit->UpdateObject();
	}
	if (m_nXCaretPos != nOldXCaretPos)
	{
		this->UpdateCaretByPos();
	}
}
void Edit::OnKeyDown_Insert(bool bCtrlDown)
{
	if (bCtrlDown)
	{
		this->OnKeyDown_Ctrl_C();
	}
	else if (Util::IsKeyDown( VK_SHIFT))
	{
		this->OnKeyDown_Ctrl_V();
	}
	else
	{
		m_EditData.SetInsertMode(!m_EditData.GetInsertMode());
	}
}
void  Edit::OnKeyDown_Return()
{
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_EN_RETURN;
    msg.pMsgFrom = m_pIEdit;
    m_pIEdit->DoNotify(&msg);
}
void  Edit::OnKeydown_Esc()
{
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_EN_ESC;
    msg.pMsgFrom = m_pIEdit;
    m_pIEdit->DoNotify(&msg);
}

void  Edit::OnKeydown_Tab()
{
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_EN_TAB;
    msg.pMsgFrom = m_pIEdit;
    m_pIEdit->DoNotify(&msg);
}

void Edit::OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags)
{
}


//
//	������ﲻ����Ļ���windowless richeidt�����յ�
//  kill focusʱ���ٹ�꣬����ʱ�����ѱ�ˢ�£��ٵ���DestroyCaretʱ�������ֻ�ˢ��
//  һ��������״�����¹�������
//
void Edit::OnShowWindow(BOOL bShow, UINT nStatus)
{
	
}
void Edit::OnVisibleChanged(BOOL bVisible, IObject* pParent)
{
    SetMsgHandled(FALSE);
	OnShowWindow(bVisible, 0);
}

UINT  Edit::OnGetDlgCode(LPMSG lpMsg)
{
    UINT nRet = DLGC_WANTARROWS;

    if (lpMsg && lpMsg->message == WM_KEYDOWN && lpMsg->wParam == VK_RETURN)
    {
        // Ŀǰ�Լ�ֻ�ǵ��еģ�����Ҫ�س���
    }
    else
    {
        nRet |= DLGC_WANTCHARS|DLGC_WANTALLKEYS;
    }

    if (m_pIEdit->GetStyleEx() & EDIT_STYLE_WANTTAB)
    {
        if (!Util::IsKeyDown(VK_CONTROL))
        {
            nRet |= DLGC_WANTTAB;
        }
    }
    return nRet;
}

LRESULT  Edit::OnWindowLayeredChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    bool bLayered = wParam?true:false;

    IRenderLayer*  pRenderLayer = m_pIEdit->GetRenderLayer();
    if ((pRenderLayer && pRenderLayer->IsTransparent()) || bLayered)
        m_bNeedFixGdiAlpha = true;
    else
        m_bNeedFixGdiAlpha = false;

    return 0;
}

LRESULT  Edit::OnImeRequest(UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    SetMsgHandled(FALSE);
//     if (wParam == IMR_QUERYCHARPOSITION)
//     {
//         IMECHARPOSITION* pImeCharPosition = (IMECHARPOSITION*)lParam;
//         if (pImeCharPosition)
//         {
//             CRect  rc;
//             m_pIEdit->GetWindowRect(&rc);
//             ::MapWindowPoints(m_pIEdit->GetHWND(), NULL, (LPPOINT)&rc, 2);
//             ::CopyRect(&pImeCharPosition->rcDocument, &rc);
//             pImeCharPosition->dwCharPos = 100;
//             pImeCharPosition->pt.x = 100;
//             pImeCharPosition->pt.y = 100;
//             pImeCharPosition->cLineHeight = 20;
//             SetMsgHandled(TRUE);
//             return 1;
//         }
//     }
//     else if (wParam == IMR_COMPOSITIONWINDOW)
//     {
//         int a = 0;
//     }
//     else
//     {
//         int a = 0;
//     }
    return 0;
}

// �������뷨������
LRESULT  Edit::OnImeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SetMsgHandled(FALSE);
//     if (wParam == IMN_OPENCANDIDATE)
//     {
//         return 0;
//     }
//     else
//     {
//         SetMsgHandled(1);
//     }

    return 0;
}

LRESULT  Edit::OnImeComposition(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SetMsgHandled(0);
    return 0;
}
LRESULT  Edit::OnImeSetContext(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    lParam = ISC_SHOWUICANDIDATEWINDOW;// &= ~ISC_SHOWUICOMPOSITIONWINDOW;
    return DefWindowProc(m_pIEdit->GetHWND(), uMsg, wParam, lParam);
    return 0;
}

LRESULT  Edit::OnStartComposition(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    SetMsgHandled(FALSE);
    if (m_caret.GetCaretType() != CARET_TYPE_API) // ʹ���Դ���caret���ùܣ����뷨���Լ�ʶ��
    {
        SetMsgHandled(TRUE);

        HWND  hWnd = m_pIEdit->GetHWND();
        HIMC himc = ImmGetContext(m_pIEdit->GetHWND());
        if (himc)
        {
            COMPOSITIONFORM form = {0};
            form.dwStyle = 1;

            POINT ptCtrl = m_pIEdit->GetRealPosInWindow();
            form.ptCurrentPos.x = m_caret.m_ptLast.x + ptCtrl.x;
            form.ptCurrentPos.y = m_caret.m_ptLast.y + ptCtrl.y;

            ImmSetCompositionWindow(himc, &form);
            ImmReleaseContext(hWnd, himc);
        }
        return 0;
    }
    return 0;
}

bool Edit::IsReadOnly()
{
    return m_pIEdit->TestStyleEx(EDIT_STYLE_READONNLY);
}

void Edit::SetReadOnly(bool b, bool bUpdate)
{
    if (b == IsReadOnly())
        return;

    if (b)
    {
        m_pIEdit->ModifyStyleEx(EDIT_STYLE_READONNLY, 0, true);
    }
    else
    {
        m_pIEdit->ModifyStyleEx(0, EDIT_STYLE_READONNLY, true);
    }
    if (bUpdate)
    {
        m_pIEdit->UpdateObject();
    }
}
//////////////////////////////////////////////////////////////////////////
#if 0
TsfHelper*  GetTsf()
{
    static TsfHelper _s;
    return &_s;
}

TsfHelper::TsfHelper()
{
    m_lRef = 0;
    m_pThreadMgr = NULL;
    m_pTsfSink = NULL;
    m_dwUIElementSinkCookie = 0;
}
TsfHelper::~TsfHelper()
{  
   
}

// �ؼ���ʼ��ʱ����
void TsfHelper::Init()
{
    if (0 == m_lRef)
    {
        Create();
    }
    m_lRef++;
}

// �ؼ�����ʱ����
void TsfHelper::Release()
{
    m_lRef --;
    if (0 == m_lRef)
    {
        Destroy();
    }
}

bool TsfHelper::Create()
{
    if (NULL != m_pThreadMgr)
        return true;

    // ITfThreadMgrEx is available on Vista or later.
    HRESULT hr = S_OK;
    hr = CoCreateInstance(CLSID_TF_ThreadMgr, 
        NULL, 
        CLSCTX_INPROC_SERVER, 
        __uuidof(ITfThreadMgrEx), 
        (void**)&m_pThreadMgr);

    if (hr != S_OK)
    {
        return false;
    }

    // ready to start interacting
    TfClientId cid;	// not used
    if (FAILED(m_pThreadMgr->ActivateEx(&cid, TF_TMAE_UIELEMENTENABLEDONLY)))
    {
        return false;
    }

    // Setup sinks
    BOOL bRc = FALSE;
    m_pTsfSink = new CUIElementSink();
    if (m_pTsfSink)
    {
        ITfSource *srcTm;
        if (SUCCEEDED(hr = m_pThreadMgr->QueryInterface(__uuidof(ITfSource), (void **)&srcTm)))
        {
            // Sink for reading window change
            if (SUCCEEDED(hr = srcTm->AdviseSink(__uuidof(ITfUIElementSink), (ITfUIElementSink*)m_pTsfSink, &m_dwUIElementSinkCookie)))
            {
                // Sink for input locale change
//                 if (SUCCEEDED(hr = srcTm->AdviseSink(__uuidof(ITfInputProcessorProfileActivationSink), 
//                     (ITfInputProcessorProfileActivationSink*)m_pTsfSink, &m_dwAlpnSinkCookie)))
//                 {
//                     if (SetupCompartmentSinks())	// Setup compartment sinks for the first time
//                     {
//                         bRc = TRUE;
//                     }
//                 }
            }
            srcTm->Release();
        }
    }
    return true;
}


void TsfHelper::Destroy()
{
    SAFE_RELEASE(m_pTsfSink);
    SAFE_RELEASE(m_pThreadMgr);
    m_dwUIElementSinkCookie = 0;
}



CUIElementSink::CUIElementSink()
{
    _cRef = 0;
}


CUIElementSink::~CUIElementSink()
{
}

STDAPI CUIElementSink::QueryInterface(REFIID riid, void **ppvObj)
{
    if (ppvObj == NULL)
        return E_INVALIDARG;

    *ppvObj = NULL;

    if (IsEqualIID(riid, IID_IUnknown))
    {
        *ppvObj = reinterpret_cast<IUnknown *>(this);
    }
    else if (IsEqualIID(riid, __uuidof(ITfUIElementSink)))
    {
        *ppvObj = (ITfUIElementSink *)this;
    }
    else if (IsEqualIID(riid, __uuidof(ITfInputProcessorProfileActivationSink)))
    {
        *ppvObj = (ITfInputProcessorProfileActivationSink*)this;
    }
    else if (IsEqualIID(riid, __uuidof(ITfCompartmentEventSink)))
    {
        *ppvObj = (ITfCompartmentEventSink*)this;
    }

    if (*ppvObj)
    {
        AddRef();
        return S_OK;
    }

    return E_NOINTERFACE;
}

STDAPI_(ULONG) CUIElementSink::AddRef()
{
    return ++_cRef;
}

STDAPI_(ULONG) CUIElementSink::Release()
{
    LONG cr = --_cRef;

    if (_cRef == 0)
    {
        delete this;
    }

    return cr;
}

STDAPI CUIElementSink::BeginUIElement(DWORD dwUIElementId, BOOL *pbShow)
{
    *pbShow = TRUE;
    return S_OK;
}

STDAPI CUIElementSink::UpdateUIElement(DWORD dwUIElementId)
{
    return S_OK;
}

STDAPI CUIElementSink::EndUIElement(DWORD dwUIElementId)
{
    return S_OK;
}
#endif

}
