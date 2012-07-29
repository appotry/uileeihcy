#include "stdafx.h"

ComboboxBase::ComboboxBase()
{
	// 修改一些默认属性
	m_edit.m_strID = COMBOBOX_EDIT_ID;
	m_edit.SetEditStyle(EDIT_STYLE_COMBOBOX);
	m_edit.ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0);

	m_button.m_strID = COMBOBOX_BUTTON_ID;
	m_button.SetButtonStyle(BUTTON_STYLE_COMBOBOX);
	m_button.SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
	m_button.SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);

	m_listbox.m_strID = COMBOBOX_LIST_ID;
	m_listbox.ModifyStyle(LISTCTRLBASE_CONTENT_2_SIZE, LISTCTRLBASE_SIZE_2_CONTENT);
	
	this->AddChild(&m_edit);
	this->AddChild(&m_button);
	m_button.ClearNotify();
	m_button.AddNotify(this,0);
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

	// 背景绘制 
	if (NULL == m_pBkgndRender)
	{
		m_pBkgndRender = RenderFactory::GetRender( RENDER_TYPE_THEME, this);
		this->ModifyStyle(OBJECT_STYLE_TRANSPARENT,0);
	}

	if (NULL == m_button.GetBkRender()) 
	{
		m_button.SetBkRender(RenderFactory::GetRender( RENDER_TYPE_THEME, &m_button));
		m_button.ModifyStyle(OBJECT_STYLE_TRANSPARENT,0);
	}

	return true;
}
void ComboboxBase::ResetAttribute()
{
	m_edit.ResetAttribute();
	m_button.ResetAttribute();
	m_listbox.ResetAttribute();

	m_edit.ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0);
	m_button.SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
	m_button.SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);
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
	m_button.SetObjectPos(rcClient.right-nBtnW/*-2*/,rcClient.top/*+2*/, nBtnW,rcClient.Height()/*-4*/,SWP_NOREDRAW);
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
		else if( this->IsPress() /*|| m_button.IsPress() || m_edit.IsPress()*/)
		{
			m_pBkgndRender->DrawState(hRDC, &rc, COMBOBOX_BKGND_RENDER_STATE_PRESS);
		}
		else if( this->IsHover() /*|| m_button.IsHover() || m_edit.IsHover()*/)
		{
			m_pBkgndRender->DrawState(hRDC, &rc, COMBOBOX_BKGND_RENDER_STATE_HOVER);
		}
		else
		{
			m_pBkgndRender->DrawState(hRDC, &rc, COMBOBOX_BKGND_RENDER_STATE_NORMAL);
		}
	}
}

void ComboboxBase::OnStateChanged(int nOld, int nNew)
{
	this->UpdateObject();
}

void ComboboxBase::OnBtnClicked( )
{
	m_listbox.AddString(_T("Test"),false);
	PopupListBoxWindow* p = new PopupListBoxWindow(&m_listbox, this);
	p->Create(_T(""),NULL/*GetHWND()*/);
	//::ShowWindow(p->m_hWnd,SW_SHOW);
	::SetWindowPos(p->m_hWnd, NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_SHOWWINDOW|SWP_NOACTIVATE);
	//::SetFocus(p->m_hWnd);
}