#include "stdafx.h"
#include "combobox.h"
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\icolorrender.h"
#include "UISDK\Control\Src\Control\ListBase\PopupControlWindow\popupcontrolwindow.h"
#include "UISDK\Control\Src\Control\ListBox\listbox.h"

namespace UI
{

ComboBoxBase::ComboBoxBase()
{
    m_pIComboBoxBase = NULL;
	m_pEdit = NULL;
	m_pButton = NULL;

    m_pPopupWrapWnd = NULL;
    m_pDropDownObject = NULL;
}

ComboBoxBase::~ComboBoxBase()
{
	m_pEdit = NULL;     // 由父Obj释放
	m_pButton = NULL;   // 由父Obj释放
    m_pDropDownObject = NULL;
    if (m_pPopupWrapWnd)
    {
        m_pPopupWrapWnd->Destroy();
        SAFE_DELETE_Ixxx(m_pPopupWrapWnd);
    }
}

HRESULT  ComboBoxBase::FinalConstruct(IUIApplication* p)
{
    m_pIComboBoxBase->ModifyStyle(CONTROL_STYLE_TABSTOP, 0, false);
    m_pIComboBoxBase->ModifyStyleEx(COMBOBOX_STYLE_EDITDROPDOWNLIST, COMBOBOX_STYLE_DROPDOWN, false);

    IEdit::CreateInstance(p, &m_pEdit);
    IButton::CreateInstance(p, &m_pButton);

    // 修改一些默认属性
    m_pEdit->SetID(COMBOBOX_EDIT_ID);
    m_pEdit->SetStyleEx(MAKECONTROLSUBTYPE(m_pEdit->GetStyleEx(), EDIT_STYLE_COMBOBOX));
    m_pEdit->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, false);

    m_pButton->SetID(COMBOBOX_BUTTON_ID);
    m_pButton->SetButtonStyle(BUTTON_STYLE_COMBOBOX);
    m_pButton->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
    m_pButton->SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);
    m_pButton->SetTabstop(false);

    m_pIComboBoxBase->AddChild(m_pEdit);
    m_pIComboBoxBase->AddChild(m_pButton);
    m_pButton->ClearNotify();
    m_pButton->AddHook(m_pIComboBoxBase, 0, COMBOBOX_BUTTON_MSG_HOOK_MSG_ID);
    m_pEdit->ClearNotify();
    m_pEdit->AddHook(m_pIComboBoxBase, 0, COMBOBOX_EDIT_MSG_HOOK_MSG_ID);

    // Object将调用 ResetAttribute ，但是Combobox中的ResetAttribute是基于子控件已创建基础上的，因此将该语句调后
    DO_PARENT_PROCESS(IComboBoxBase, IControl);
    return GetCurMsg()->lRet;
}

void ComboBoxBase::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    bool bBorderConfiged = pMapAttrib->HasAttrib(XML_BORDER);
	DO_PARENT_PROCESS(IComboBoxBase, IControl);

    IUIApplication* pUIApplication = m_pIComboBoxBase->GetUIApplication();
    if (m_pEdit)
        m_pEdit->SetAttributeByPrefix(XML_COMBOBOX_EDIT_PRIFIX, pMapAttrib, bReload, true);
    if (m_pButton)
        m_pButton->SetAttributeByPrefix(XML_COMBOBOX_BUTTON_PRIFIX, pMapAttrib, bReload, true);

    ITextRenderBase*  pTextRenderBase = NULL;
    pMapAttrib->GetAttr_TextRenderBase(XML_TEXTRENDER_TYPE, true, pUIApplication, m_pIComboBoxBase, &pTextRenderBase);
    if (pTextRenderBase)
    {
        m_pIComboBoxBase->SetTextRender(pTextRenderBase);
        SAFE_RELEASE(pTextRenderBase);
    }
    if (NULL == m_pIComboBoxBase->GetTextRender())
    {
        pUIApplication->CreateTextRenderBase(TEXTRENDER_TYPE_NORMAL, m_pIComboBoxBase, &pTextRenderBase);
        if (pTextRenderBase)
        {
            pTextRenderBase->SetAttribute(pMapAttrib, NULL, true);
            m_pIComboBoxBase->SetTextRender(pTextRenderBase);
            pTextRenderBase->Release();
        }
    }

	// 背景绘制 
	if (NULL == m_pIComboBoxBase->GetBkRender())
	{
        IRenderBase*  pRenderBase = NULL;
        pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_COMBOBOX_BKGND, m_pIComboBoxBase, &pRenderBase);
        if (pRenderBase)
        {
            m_pIComboBoxBase->SetBkgndRender(pRenderBase);
		    m_pIComboBoxBase->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, false);
            SAFE_RELEASE(pRenderBase);

            if (!bBorderConfiged)
            {
                CRegion4 r (2,0,0,0);  // 为Edit留两个像素
                m_pIComboBoxBase->SetBorderRegion(&r);
            }
        }
	}

	if (NULL == m_pButton->GetBkRender()) 
	{
        IRenderBase*  pRenderBase = NULL;
        pUIApplication->CreateRenderBase(RENDER_TYPE_THEME_COMBOBOX_BUTTON, m_pButton, &pRenderBase);
        if (pRenderBase)
        {
            m_pButton->SetBkgndRender(pRenderBase);
            m_pButton->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, false);
            SAFE_RELEASE(pRenderBase);
        }
	}
}
void ComboBoxBase::ResetAttribute()
{
	DO_PARENT_PROCESS(IComboBoxBase, IControl);

	m_pIComboBoxBase->ModifyStyle(0, CONTROL_STYLE_TABSTOP, false);
	
	if (m_pEdit)
	{
        UISendMessage(m_pEdit, UI_WM_RESETATTRIBUTE);
		m_pEdit->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, false);
	}
	if (m_pButton)
	{
        UISendMessage(m_pButton, UI_WM_RESETATTRIBUTE);
		m_pButton->SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
		m_pButton->SetAutoSizeType(BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE);
		m_pButton->SetTabstop(false);
	}
// 	if (m_pCustom)
// 	{
// 		m_pCustom->OnResetAttribute();
// 	}
}

void  ComboBoxBase::GetAutoSize(SIZE* pSize)
{
    SIZE s1 = m_pEdit->GetDesiredSize();
	SIZE s2 = m_pButton->GetDesiredSize();

	pSize->cx = s1.cx + s2.cx;
    pSize->cy = max(s1.cy,s2.cy);
}

void ComboBoxBase::OnSize(UINT nType, int cx, int cy)
{
	CRect  rcClient;
	m_pIComboBoxBase->GetClientRectAsWin32(&rcClient);

	int nBtnW = m_pButton->GetWidth();
	if (0 == nBtnW)
	{
		SIZE s = m_pButton->GetDesiredSize();
		nBtnW = s.cx;
	}

	CRegion4 rcMarginBtn;
	m_pButton->GetMarginRegion(&rcMarginBtn);
	m_pButton->SetObjectPos(
		rcClient.right - nBtnW - rcMarginBtn.right,
		rcClient.top + rcMarginBtn.top, 
		nBtnW,
		rcClient.Height() - rcMarginBtn.Height(),
		SWP_NOREDRAW);

	CRegion4 rcMarginEdit;
	m_pEdit->GetMarginRegion(&rcMarginEdit);
	m_pEdit->SetObjectPos(
		rcClient.left + rcMarginEdit.left,
		rcClient.top + rcMarginEdit.top, 
		rcClient.Width()- nBtnW - rcMarginEdit.left - rcMarginEdit.right - rcMarginBtn.left - rcMarginBtn.right,
		rcClient.Height() - rcMarginEdit.top - rcMarginEdit.bottom, 
		SWP_NOREDRAW);
}


void ComboBoxBase::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    IRenderBase* pRenderBase = m_pIComboBoxBase->GetBkRender();
    bool  bReadOnly = m_pIComboBoxBase->TestStyleEx(COMBOBOX_STYLE_DROPDOWN);

	if (pRenderBase)
	{
		CRect rc(0,0, m_pIComboBoxBase->GetWidth(), m_pIComboBoxBase->GetHeight());

		if (!m_pIComboBoxBase->IsEnable())
		{
			pRenderBase->DrawState(pRenderTarget, &rc, bReadOnly?COMBOBOX_BKGND_RENDER_STATE_READONLY_DISABLE:COMBOBOX_BKGND_RENDER_STATE_DISABLE);
		}
		else if (m_pIComboBoxBase->IsPress() /*|| m_pButton->IsPress() || m_pEdit->IsPress()*/)
		{
			pRenderBase->DrawState(pRenderTarget, &rc, bReadOnly?COMBOBOX_BKGND_RENDER_STATE_READONLY_PRESS:COMBOBOX_BKGND_RENDER_STATE_PRESS);
		}
		else if (m_pIComboBoxBase->IsHover() || m_pEdit->IsFocus() /*|| m_pButton->IsHover() || m_pEdit->IsHover()*/)
		{
			pRenderBase->DrawState(pRenderTarget, &rc, bReadOnly?COMBOBOX_BKGND_RENDER_STATE_READONLY_HOVER:COMBOBOX_BKGND_RENDER_STATE_HOVER);
		}
		else
		{
			pRenderBase->DrawState(pRenderTarget, &rc, bReadOnly?COMBOBOX_BKGND_RENDER_STATE_READONLY_NORMAL:COMBOBOX_BKGND_RENDER_STATE_NORMAL);
		}
	}
}

// 避免鼠标从COMBOBOX移动到EDIT/BUTTON上时刷新为normal状态
void ComboBoxBase::OnStateChanged(UINT nMask)
{
//	this->UpdateObject();

	UIMSG msg;
	msg.message = UI_WM_REDRAWOBJECT;
	msg.pMsgTo = m_pIComboBoxBase;
	UIPostMessage(m_pIComboBoxBase->GetUIApplication(), &msg);
}
void ComboBoxBase::OnChildObjStateChanged(UINT nMask)
{
	SetMsgHandled(FALSE);  // 设置为TRUE会导致子控件刷新不正常，如EDIT不显示光标
	
    IObject* pObj = static_cast<IObject*>(GetCurMsg()->pMsgTo);
    if (nMask & (OSB_HOVER | OSB_FORCEHOVER | OSB_PRESS | OSB_FORCEPRESS | OSB_FOCUS))
    {
        m_pIComboBoxBase->SetForceHover(pObj->IsHover()||pObj->IsPress() || pObj->IsFocus(), false);

        // 因为子控件现在被HOOK掉没刷新了，由Combobox来刷新
        UIMSG msg;
        msg.pMsgFrom = m_pIComboBoxBase;
        msg.pMsgTo = m_pIComboBoxBase;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FORCEHOVER;
        UIPostMessage(m_pIComboBoxBase->GetUIApplication(), &msg);  // 有可能是从子对象上移到COMBOBOX，因此要延迟刷新

        SetMsgHandled(TRUE);
    }
}

const TCHAR*  ComboBoxBase::GetText()
{
    if (!m_pEdit)
        return NULL;

    return m_pEdit->GetText();
}
void  ComboBoxBase::SetText(const TCHAR* szText)
{
	if (!m_pEdit)
		return;

	m_pEdit->SetText(szText);
}
void  ComboBoxBase::SetReadOnly(bool b)
{
    if (b)
    {
        m_pIComboBoxBase->ModifyStyleEx(COMBOBOX_STYLE_DROPDOWN, COMBOBOX_STYLE_EDITDROPDOWNLIST, true);
    }
    else
    {
        m_pIComboBoxBase->ModifyStyleEx(COMBOBOX_STYLE_EDITDROPDOWNLIST, COMBOBOX_STYLE_DROPDOWN, true);
    }
    if (m_pEdit)
        m_pEdit->SetReadOnly(b, false);
}
bool  ComboBoxBase::IsReadOnly()
{
    return m_pIComboBoxBase->TestStyleEx(COMBOBOX_STYLE_DROPDOWN);
}
void ComboBoxBase::OnRedrawObject()
{
	m_pIComboBoxBase->UpdateObject();
}

void ComboBoxBase::OnLButtonDown(UINT nFlags, POINT point)
{
	if (m_pIComboBoxBase->TestStyleEx(COMBOBOX_STYLE_DROPDOWN))
	{
		if (m_pIComboBoxBase->IsForcePress())
			return;

        UIMSG  msg;
        msg.message = UI_WM_NOTIFY;
        msg.nCode = UI_CBN_DROPDOWN;
        msg.pMsgTo = m_pIComboBoxBase;
        msg.pMsgFrom = m_pIComboBoxBase;

        LRESULT lRet = 0;
        lRet = UISendMessage(&msg);

        // 派生类没有处理，则交给外部处理
        if (!msg.bHandled)
        {
            msg.pMsgTo = NULL;
            msg.bHandled = FALSE;
            lRet = m_pIComboBoxBase->DoNotify(&msg);
        }
        if (!msg.bHandled)
        {
            lRet = _DefaultDropDown();
        }

        if (lRet)
        {
           m_pIComboBoxBase->SetForcePress(true);
        }
	}
}
void ComboBoxBase::OnBtnLButtonDown(UINT nFlags, POINT point)
{
	if (m_pButton->IsForcePress())
		return;

    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_CBN_DROPDOWN;
    msg.pMsgTo = m_pIComboBoxBase;
    msg.pMsgFrom = m_pIComboBoxBase;

    LRESULT lRet = 0;
    lRet = UISendMessage(&msg);

    // 派生类没有处理，则交给外部处理
    if (!msg.bHandled)
    {
        msg.pMsgTo = NULL;
        msg.bHandled = FALSE;
        lRet = m_pIComboBoxBase->DoNotify(&msg);
    }
    if (!msg.bHandled)
    {
        lRet = _DefaultDropDown();
    }

    if (lRet)
    {
        m_pButton->SetForcePress(true);
	    if (m_pIComboBoxBase->TestStyleEx(COMBOBOX_STYLE_DROPDOWN) && NULL != m_pEdit) // 可编辑，将焦点设置给编辑框
	    {
		    IWindowBase* pWindow = m_pIComboBoxBase->GetWindowObject();
		    if (pWindow)
			    pWindow->SetFocusObject(m_pEdit);
	    }
    }
}

void ComboBoxBase::OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	SetMsgHandled(FALSE);

    if (nStyleType == GWL_EXSTYLE)
    {
	    if (!(lpStyleStruct->styleOld & COMBOBOX_STYLE_DROPDOWN)  &&
		    lpStyleStruct->styleNew & COMBOBOX_STYLE_DROPDOWN)
	    {
            if (m_pEdit)
            {
                m_pEdit->SetVisible(false,true);
            }
            if (m_pButton)
            {
                m_pButton->SetVisible(false,true);
            }
	    }
	    else if (!(lpStyleStruct->styleOld & COMBOBOX_STYLE_EDITDROPDOWNLIST)  &&
		    lpStyleStruct->styleNew & COMBOBOX_STYLE_EDITDROPDOWNLIST)
	    {
            if (m_pEdit)
            {
                m_pEdit->SetVisible(true,true);
            }
            if (m_pButton)
            {
                m_pButton->SetVisible(true,true);
            }
	    }
    }
}

// PopupListBoxWindow显示/销毁时，发送过来的消息
void ComboBoxBase::OnInitPopupControlWindow()
{
	SetMsgHandled(FALSE);
}
void ComboBoxBase::OnUnInitPopupControlWindow()
{
    SetMsgHandled(FALSE);

	if (m_pIComboBoxBase->TestStyleEx(COMBOBOX_STYLE_DROPDOWN))
	{
		m_pIComboBoxBase->SetForcePress(false);   
		m_pIComboBoxBase->UpdateObject();
	}
	else
	{
		int nOldStateBits = m_pButton->GetStateBit();
		m_pButton->SetForcePress(false);  
	}
}


bool ComboBoxBase::_DefaultDropDown()
{
    if (NULL == m_pPopupWrapWnd)
    {
        IUIApplication*  pUIApplication = m_pIComboBoxBase->GetUIApplication();
        PopupListBoxWindow::CreateInstance(pUIApplication, &m_pPopupWrapWnd);
        if (NULL == m_pPopupWrapWnd)
            return false;

        // 指定一个父窗口，而不是采用置顶的方式。避免在EDIT输入中文时，listbox遮挡了输入法窗口
        m_pPopupWrapWnd->Create(m_pDropDownObject, _T("PopupListBox"), m_pIComboBoxBase->GetHWND());
    }

	HWND hWnd = m_pIComboBoxBase->GetHWND();
    CRect rcWindow;
    m_pIComboBoxBase->GetWindowRect(&rcWindow);
    ::MapWindowPoints(hWnd, NULL, (LPPOINT)&rcWindow, 2);

	if (m_pButton && m_pButton->IsMySelfVisible())
	{
		CRect rcClickFrom;
		m_pButton->GetWindowRect(&rcClickFrom);
		m_pPopupWrapWnd->SetPopupFromInfo(hWnd, &rcClickFrom);
	}
	else
	{
		m_pPopupWrapWnd->SetPopupFromInfo(hWnd, &rcWindow);
	}
    POINT pt = { rcWindow.left, rcWindow.bottom };
    m_pPopupWrapWnd->Show(pt, FALSE);  // <-- 这里不要用模态的，否则直接点击窗口关闭按钮时会导致崩溃

    return true;
}

void ComboBoxBase::CloseUp()
{
    UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode = UI_CBN_CLOSEUP;
    msg.pMsgTo = m_pIComboBoxBase;
    msg.pMsgFrom = m_pIComboBoxBase;

    LRESULT lRet = 0;
    lRet = UISendMessage(&msg);
}

LRESULT ComboBoxBase::OnDefaultCloseUp(WPARAM, LPARAM)
{
    if (m_pPopupWrapWnd)
    {
        m_pPopupWrapWnd->Hide();
    }
    return 0;
}
//////////////////////////////////////////////////////////////////////////

ComboBox::ComboBox()
{
    m_pIComboBox = NULL; 
    m_pDropDownCtrl = NULL; 
}

HRESULT  ComboBox::FinalConstruct(IUIApplication* p)
{
    __super::FinalConstruct(p);

    IListBox::CreateInstance(p, &m_pDropDownCtrl);
    if (m_pDropDownCtrl)
    {
        m_pDropDownCtrl->SetID(COMBOBOX_LIST_ID);
    	m_pDropDownCtrl->ModifyStyleEx(LISTCTRLBASE_STYLE_POPUPLISTBOX, 0, true);
        m_pDropDownCtrl->SetBindObject(m_pIComboBox);
        m_pDropDownCtrl->AddHook(m_pIComboBox, 0, COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID);
        m_pDropDownCtrl->SetNotify(m_pIComboBox, COMBOBOX_LISTBOX_MSG_HOOK_MSG_ID);

        this->SetDropDownObjectPtr(m_pDropDownCtrl);
    }

    return S_OK;
}
void  ComboBox::FinalRelease()
{
    SetMsgHandled(FALSE);

    SAFE_DELETE_Ixxx(m_pDropDownCtrl);
}

HRESULT  ComboBox::OnDropDown(WPARAM, LPARAM)
{
    if (NULL == m_pDropDownCtrl)
        return FALSE;

    if (0 == m_pDropDownCtrl->GetItemCount())
        return FALSE;

    SetMsgHandled(FALSE);
    return FALSE;
}


bool  ComboBox::AddString(const TCHAR* szText, int nAddItemFlags)
{ 
    if (this->AddStringEx(szText, nAddItemFlags))
        return true;

    return false;
}
IListBoxItem* ComboBox::AddStringEx(const TCHAR* szText, int nAddItemFlags)
{
    if (NULL == m_pDropDownCtrl)
        return NULL;

    IListBoxItem* p = m_pDropDownCtrl->AddString(szText, nAddItemFlags);
    if (NULL == p)
        return NULL;

    return static_cast<IListBoxItem*>(p);
}

bool  ComboBox::SetCurSel(int nIndex)
{
    if (NULL == m_pDropDownCtrl)
        return false;

    return m_pDropDownCtrl->SetSel(nIndex);
}

void  ComboBox::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    IUIApplication*  pUIApp = m_pIComboBox->GetUIApplication();
	// 设置listbox默认字体
    const TCHAR* szText = pMapAttrib->GetAttr(XML_COMBOBOX_LISTBOX_PRIFIX XML_TEXTRENDER_TYPE, true);
	if (NULL == szText)
	{
		if (NULL == m_pDropDownCtrl->GetTextRender())
		{
			ITextRenderBase* pTextRender = NULL;
            pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_COLORLIST, m_pDropDownCtrl, &pTextRender);

			if (pTextRender)
			{
				IColorListTextRender* p = (IColorListTextRender*)pTextRender->QueryInterface(uiiidof(IColorListTextRender));
                if (p)
                {
				    p->SetCount(8);
				    p->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_NORMAL,  RGBA(0,0,0, 255));
				    p->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_HOVER,   RGBA(255,255,255, 255));
				    p->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_PRESS,   RGBA(255,255,255, 255));
				    p->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_DISABLE, RGBA(255,255,255, 255));

				    p->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL,  RGBA(255,255,255, 255));
				    p->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_HOVER,   RGBA(255,255,255, 255));
				    p->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_PRESS,   RGBA(255,255,255, 255));
				    p->SetColor(LISTCTRLITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE, RGBA(255,255,255, 255));

                    IRenderFont* pRenderFont = m_pIComboBox->GetRenderFont();
				    p->SetRenderFont(pRenderFont);

				    p->SetTextAlignment(DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS|DT_NOPREFIX);
                }

                m_pDropDownCtrl->SetTextRender(pTextRender);
                pTextRender->SetAttribute(pMapAttrib, XML_COMBOBOX_LISTBOX_PRIFIX, true);
                SAFE_RELEASE(pTextRender);
			}
			SAFE_RELEASE(pTextRender);
		}
	}

	m_pDropDownCtrl->SetAttributeByPrefix(XML_COMBOBOX_LISTBOX_PRIFIX, pMapAttrib, bReload, true);
    
    __super::SetAttribute(pMapAttrib, bReload);

    if (!bReload)
    {
        // 这个要放在设置完EDIT/BUTTON后面，否则EDIT在readonly下面又会被显示出来
        bool bReadonly = false;
        if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_READONLY, true, &bReadonly))
        {
            SetReadOnly(bReadonly);
        }
    }
}
void  ComboBox::ResetAttribute()
{
    __super::ResetAttribute();

	if (NULL == m_pDropDownCtrl)
		return;

    UISendMessage(m_pDropDownCtrl, UI_WM_RESETATTRIBUTE);
}

void  ComboBox::OnPaint(IRenderTarget* pRenderTarget)
{
    if (!m_pIComboBoxBase->TestStyleEx(COMBOBOX_STYLE_DROPDOWN))
        return;

	if (NULL == m_pDropDownCtrl)
		return;

    ITextRenderBase*  pTextRender = m_pIComboBox->GetTextRender();

	IListItemBase* pItem = m_pDropDownCtrl->GetFirstSelectItem();
	if (pItem)
	{
//        UISendMessage(pItem, WM_PAINT, (WPARAM)pRenderTarget, 0); <-- 有可能item rect还没有update

        // 用padding作为readonly的文字间距
		CRect rc;
		m_pIComboBox->GetClientRectAsWin32(&rc);
		pTextRender->DrawState(pRenderTarget, &rc, 0, pItem->GetText(), DT_SINGLELINE|DT_VCENTER|DT_LEFT);
	}
}


void ComboBox::OnLCNSelChanged(IMessage* pObjMsgFrom, IListItemBase* pOldSelItem, IListItemBase* pSelItem)
{
    if (m_pIComboBoxBase->TestStyle(COMBOBOX_STYLE_EDITDROPDOWNLIST))
    {
        m_pIComboBoxBase->UpdateObject();
    }
    else if (pSelItem)
    {
        m_pEdit->SetText(pSelItem->GetText());
        m_pEdit->SetSel(0,-1);   // 全选
    }

    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode    = UI_CBN_SELCHANGED;
    msg.wParam  = (WPARAM)pOldSelItem;
    msg.lParam  = (LPARAM)pSelItem;
    msg.pMsgFrom = m_pIComboBox;
    m_pIComboBox->DoNotify( &msg );
}

LRESULT  ComboBox::OnLCNClick(WPARAM, LPARAM)
{
    this->OnDefaultCloseUp(0, 0);
    return 0;
}

void  ComboBox::ResetContent()
{
	if (m_pDropDownCtrl)
	{
		m_pDropDownCtrl->RemoveAllItem(0);
	}
}

}