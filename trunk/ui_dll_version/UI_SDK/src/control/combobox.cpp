#include "stdafx.h"

ComboboxBase::ComboboxBase()
{
	UICreateInstance(&m_edit);
	UICreateInstance(&m_button);
	UICreateInstance(&m_listbox);

	// 修改一些默认属性
	m_edit->m_strID = COMBOBOX_EDIT_ID;
	m_edit->SetEditStyle(EDIT_STYLE_COMBOBOX);
	m_edit->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0);

	m_button->m_strID = COMBOBOX_BUTTON_ID;
	m_button->SetButtonStyle(BUTTON_STYLE_COMBOBOX);
	m_button->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
	m_button->SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);
	m_button->SetTabstop(false);

	m_listbox->m_strID = COMBOBOX_LIST_ID;
	m_listbox->ModifyStyle(LISTCTRLBASE_CONTENT_2_SIZE|LISTCTRLBASE_SORT_ASCEND|LISTCTRLBASE_SEL_HOVER_MODE, LISTCTRLBASE_SIZE_2_CONTENT);
	m_listbox->SetListBoxStyle(LISTBOX_STYLE_COMBOBOX);
	m_listbox->SetBindObject(this);
	
	this->AddChild(m_edit);
	this->AddChild(m_button);
	m_button->ClearNotify();
	m_button->AddHook(this,0,1);
}

ComboboxBase::~ComboboxBase()
{
	m_edit = NULL;     // 由父Obj释放
	m_button = NULL;   // 由父Obj释放

	SAFE_DELETE(m_listbox);
}

bool ComboboxBase::SetAttribute( ATTRMAP& mapAttrib, bool bReload )
{
	bool bRet = Control::SetAttribute(mapAttrib, bReload);
	if (false == bRet)
		return false;

	this->SetChildObjectAttribute(m_edit,    XML_COMBOBOX_EDIT_PRIFIX,    mapAttrib, bReload);
	this->SetChildObjectAttribute(m_button,  XML_COMBOBOX_BUTTON_PRIFIX,  mapAttrib, bReload);
	this->SetChildObjectAttribute(m_listbox, XML_COMBOBOX_LISTBOX_PRIFIX, mapAttrib, bReload);

	// 背景绘制 
	if (NULL == m_pBkgndRender)
	{
		m_pBkgndRender = RenderFactory::GetRender( RENDER_TYPE_THEME, this);
		this->ModifyStyle(OBJECT_STYLE_TRANSPARENT,0);
	}

	if (NULL == m_button->GetBkRender()) 
	{
		m_button->SetBkRender(RenderFactory::GetRender( RENDER_TYPE_THEME, m_button));
		m_button->ModifyStyle(OBJECT_STYLE_TRANSPARENT,0);
	}

	if (NULL == m_listbox->GetForeRender())
	{
		RenderBase* pForeRender = RenderFactory::GetRender(RENDER_TYPE_COLORLIST, m_listbox);
		ColorListRender* p = dynamic_cast<ColorListRender*>(pForeRender);
		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER, RGB(51,153,255),true, 0,false);
		p->SetStateColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED, RGB(51,153,255),true, 0,false);
		m_listbox->SetForeRender(pForeRender);
	}

	return true;
}
void ComboboxBase::ResetAttribute()
{
	m_edit->ResetAttribute();
	m_button->ResetAttribute();
	m_listbox->ResetAttribute();

	m_edit->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0);
	m_button->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
	m_button->SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);
}
SIZE ComboboxBase::GetAutoSize( HRDC hRDC )
{
	SIZE s1 = m_edit->GetAutoSize(hRDC);
	SIZE s2 = m_button->GetAutoSize(hRDC);

	SIZE s = {s1.cx+s2.cx, max(s1.cy,s2.cy)};
	return s;
}

void ComboboxBase::OnSize( UINT nType, int cx, int cy )
{
	CRect  rcClient;
	this->GetClientRectAsWin32(&rcClient);

	int nBtnW = m_button->GetWidth();
	if (0 == nBtnW)
	{
		SIZE s = m_button->GetDesiredSize(NULL);
		nBtnW = s.cx;
	}
	m_button->SetObjectPos(rcClient.right-nBtnW/*-2*/,rcClient.top/*+2*/, nBtnW,rcClient.Height()/*-4*/,SWP_NOREDRAW);
	m_edit->SetObjectPos(rcClient.left,rcClient.top, rcClient.Width()-nBtnW, rcClient.Height(),SWP_NOREDRAW);
}


void ComboboxBase::OnEraseBkgnd(HRDC hRDC)
{
	if( NULL != m_pBkgndRender )
	{
		CRect rc(0,0,GetWidth(),GetHeight());
		if (!this->IsEnable())
		{
			m_pBkgndRender->DrawState(hRDC, &rc, COMBOBOX_BKGND_RENDER_STATE_DISABLE);
		}
		else if (this->IsPress() /*|| m_button->IsPress() || m_edit->IsPress()*/)
		{
			m_pBkgndRender->DrawState(hRDC, &rc, COMBOBOX_BKGND_RENDER_STATE_PRESS);
		}
		else if (this->IsHover() /*|| m_button->IsHover() || m_edit->IsHover()*/)
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

void ComboboxBase::OnBtnLButtonDown(UINT nFlags, POINT point)
{
	if (m_button->IsForePress())
		return;

	m_listbox->DropDown();
	m_button->SetForcePress(true);
}

// PopupListBoxWindow显示/销毁时，发送过来的消息
void ComboboxBase::OnInitPopupControlWindow(Object* pObjMsgFrom)
{
	
}
void ComboboxBase::OnUnInitPopupControlWindow(Object* pObjMsgFrom)
{
	int nOldStateBits = m_button->GetStateBit();
	m_button->SetForcePress(false);
	::UISendMessage(m_button, UI_WM_STATECHANGED, nOldStateBits, m_button->GetStateBit() ); //刷新按钮
}

bool ComboboxBase::AddString(const String& strText, bool bUpdate)
{ 
	if (NULL != m_listbox)
	{
		m_listbox->AddString(strText, bUpdate) == NULL ? false :true;
	}

	return false;
}