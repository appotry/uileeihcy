#include "stdafx.h"

ComboboxBase::ComboboxBase()
{
	m_edit.m_strID = _T("combobox_edit");
	m_button.m_strID = _T("combobox_button");
	m_edit.SetEditStyle(EDIT_STYLE_COMBOBOX);
	m_button.SetButtonStyle(BUTTON_STYLE_COMBOBOX);

	this->AddChild(&m_edit);
	this->AddChild(&m_button);
}

ComboboxBase::~ComboboxBase()
{

}

bool ComboboxBase::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	bool bRet = Control::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	this->SetChildObjectAttribute(&m_edit, XML_COMBOBOX_EDIT_PRIFIX, mapAttrib, bReload);
	this->SetChildObjectAttribute(&m_button, XML_COMBOBOX_BUTTON_PRIFIX,mapAttrib, bReload);

	// ±³¾°»æÖÆ 
	if( NULL == m_pBkgndRender )
	{
		m_pBkgndRender = RenderFactory::GetRender( RENDER_TYPE_THEME, this);
	}

	return true;
}
void ComboboxBase::ResetAttribute()
{
	m_edit.ResetAttribute();
	m_button.ResetAttribute();
	m_listbox.ResetAttribute();
}
SIZE ComboboxBase::GetAutoSize( HRDC hRDC )
{
	SIZE s1 = m_edit.GetAutoSize(hRDC);
	SIZE s2 = m_button.GetAutoSize(hRDC);

	SIZE s = {s1.cx+s2.cx, max(s1.cy,s2.cy)};
	return s;
}

void ComboboxBase::OnSize( UINT nType, int cx, int cy )
{
	CRect  rcClient;
	this->GetClientRectAsWin32(&rcClient);

	int nBtnW = m_button.GetWidth();
	if (0 == nBtnW)
	{
		SIZE s = m_button.GetDesiredSize(NULL);
		nBtnW = s.cx;
	}
	m_button.SetObjectPos(rcClient.right-nBtnW-2,rcClient.top+2, nBtnW,rcClient.Height()-4,SWP_NOREDRAW);
	m_edit.SetObjectPos(rcClient.left,rcClient.top, rcClient.Width()-nBtnW, rcClient.Height(),SWP_NOREDRAW);
}


void ComboboxBase::OnEraseBkgnd(HRDC hRDC)
{
	if( NULL != m_pBkgndRender )
	{
		CRect rc(0,0,GetWidth(),GetHeight());
		if( !this->IsEnable() )
		{
			m_pBkgndRender->DrawState(hRDC, &rc, COMBOBOX_BKGND_RENDER_STATE_DISABLE);
		}
		else if( this->IsFocus() || this->IsPress() )
		{
			m_pBkgndRender->DrawState(hRDC, &rc, COMBOBOX_BKGND_RENDER_STATE_PRESS);
		}
		else if( this->IsHover() )
		{
			m_pBkgndRender->DrawState(hRDC, &rc, COMBOBOX_BKGND_RENDER_STATE_HOVER);
		}
		else
		{
			m_pBkgndRender->DrawState(hRDC, &rc, COMBOBOX_BKGND_RENDER_STATE_NORMAL);
		}
	}
}