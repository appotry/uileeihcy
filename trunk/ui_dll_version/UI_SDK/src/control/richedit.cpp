#include "stdafx.h"

RichEditBase::RichEditBase():m_wrapRichEidt(this)
{

}

// HRESULT RichEditBase::InitialConstruct()
// {
// 	HRESULT hr = __super::InitialConstruct();
// 	if (FAILED(hr))
// 		return hr;
// 
// 	return S_OK;
// }

SIZE RichEditBase::GetAutoSize( HRDC hRDC )
{ 
	SIZE  s = {0,0}; 
	return s; 
}
bool RichEditBase::SetAttribute(ATTRMAP& mapAttrib, bool bReload)
{
	bool bRet = __super::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;


	return true;
}
void RichEditBase::ResetAttribute()
{
	__super::ResetAttribute();
}

void RichEditBase::OnObjectLoaded()
{
	m_wrapRichEidt.Create(GetHWND());
}
void RichEditBase::OnEraseBkgnd( HRDC hRDC )
{
	SetMsgHandled(FALSE);
}
void RichEditBase::OnPaint( HRDC hRDC )
{
	HDC hDC = GetHDC(hRDC);
	m_wrapRichEidt.Draw(hDC);
	ReleaseHDC(hRDC, hDC);
}
LRESULT RichEditBase::OnMouseRangeMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, uMsg, wParam, lParam, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
		return 0;
	}
	return lRet;
}
void RichEditBase::OnSetFocus( Object* pOldFocusObj )
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_SETFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
		return;
	}
}
void RichEditBase::OnKillFocus( Object* pNewFocusObj )
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, WM_KILLFOCUS, NULL, NULL, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
	}
}

LRESULT RichEditBase::OnForwardMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	BOOL bRet = m_wrapRichEidt.ProcessWindowMessage(NULL, uMsg, wParam, lParam, lRet);
	if (!bRet)
	{
		SetMsgHandled(FALSE);
		return 0;
	}
	return lRet;
}