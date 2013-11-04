#include "stdafx.h"
#include "propertyctrledititem.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"

namespace UI
{
#pragma region // share data
PropertyCtrlEditItemShareData::PropertyCtrlEditItemShareData()
{
    m_pIPropertyCtrlEditItemShareData = NULL;
    pSlitterLinePos = NULL;
    pRenderFont = NULL;
    pEditCtrl = NULL;
}
PropertyCtrlEditItemShareData::~PropertyCtrlEditItemShareData()
{
}
#pragma endregion 

//////////////////////////////////////////////////////////////////////////

PropertyCtrlEditItem::PropertyCtrlEditItem()
{
    m_pIPropertyCtrlEditItem = NULL;
    m_pShareData = NULL;
}
PropertyCtrlEditItem::~PropertyCtrlEditItem()
{
    // ��ɾ��ʱ���ܿؼ���ʧ��״̬���Լ���ѡ��״̬Ҳ�������
    // if (m_pIPropertyCtrlEditItem->IsSelected())

    IObject* pObj = m_pShareData->pEditCtrl->GetParentObject();
    if (pObj && pObj == m_pIPropertyCtrlEditItem->GetRootPanel())
    {
        m_pShareData->pEditCtrl->SetVisible(false, false, false);
        m_pShareData->pEditCtrl->SetNotify(NULL, 0);
        m_pShareData->pEditCtrl->RemoveMeInTheTree();
    }
}

void  PropertyCtrlEditItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlEditItem, IListItemBase);

    m_pIPropertyCtrlEditItem->SetItemType(LISTITEM_TYPE_PropertyCtrlEdit);

    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlEditItem->GetIListCtrlBase();
    IPropertyCtrlEditItemShareData*  pData = static_cast<IPropertyCtrlEditItemShareData*>(
        pListCtrl->GetItemTypeShareData(LISTITEM_TYPE_PropertyCtrlEdit));

    if (!pData)
    {
        IPropertyCtrlEditItemShareData::CreateInstance(pListCtrl->GetUIApplication(), &pData);
        m_pShareData = pData->GetImpl();
        pListCtrl->SetItemTypeShareData(LISTITEM_TYPE_PropertyCtrlEdit, pData);
        m_pShareData->pSlitterLinePos = (UINT*)UISendMessage(pListCtrl, UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR);
        m_pShareData->pEditCtrl = (IEdit*)UISendMessage(pListCtrl, UI_PROPERTYCTRL_MSG_GETEDITCTRL);

        ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
        if (pTextRender)
            m_pShareData->pRenderFont = pTextRender->GetRenderFont();
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }

    m_pIPropertyCtrlEditItem->CreateRootPanel();
}


void  PropertyCtrlEditItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlEditItem->GetIListCtrlBase();

    CRect rcParent;
    m_pIPropertyCtrlEditItem->GetParentRect(&rcParent);

    CRect rcBlank(rcParent);
    rcBlank.right = pListCtrl->GetChildNodeIndent()* m_pIPropertyCtrlEditItem->GetDepth();

    CRect rcLeft(rcParent);
    rcLeft.left = rcBlank.right;
    rcLeft.right = *(m_pShareData->pSlitterLinePos);

    CRect rcRight(rcParent);
    rcRight.left = rcLeft.right;

    // ��౳��
    Color c(PROPERTYCTRL_LINE_COLOR);
    pRenderTarget->FillRect(&rcBlank, &c);

    // �м�ķָ���
    IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, &c);
    pRenderTarget->DrawLine(rcLeft.right, rcLeft.top, rcLeft.right, rcLeft.bottom, pPen);

    // һ�еĺ���
//     IListItemBase* pNextItem = m_pIPropertyCtrlEditItem->GetNextVisibleItem();
//     if (NULL == pNextItem || 
//         (pNextItem && pNextItem->GetDepth() == m_pIPropertyCtrlEditItem->GetDepth()))
    {
        pRenderTarget->DrawLine(rcParent.left, rcParent.bottom-1, rcParent.right, rcParent.bottom-1, pPen);
    }
    SAFE_RELEASE(pPen);

    // ѡ��״̬
    bool bSelectd = m_pIPropertyCtrlEditItem->IsSelected();
    bool bFocus = pListCtrl->IsFocus();

    if (bSelectd)
    {
        if (bFocus)
        {
            Color c(10,36,106,255);
            pRenderTarget->FillRect(&rcLeft, &c);
        }
        else
        {
            pRenderTarget->FillRect(&rcLeft, &c);
        }
    }

    // �����ı�
    if (m_pShareData->pRenderFont)
    {
        DRAWTEXTPARAM param;
        if (bSelectd && bFocus)
            param.color = 0xFFFFFFFF;
        else
            param.color = 0;

        param.nFormatFlag = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE;
        rcLeft.left += 2;
        param.prc = &rcLeft;
        param.szText = m_pIPropertyCtrlEditItem->GetText();
        pRenderTarget->DrawString(m_pShareData->pRenderFont, &param);

        if (!bSelectd)
        {
            DRAWTEXTPARAM param;
            param.color = 0;
            param.nFormatFlag = DT_LEFT|DT_VCENTER|DT_END_ELLIPSIS|DT_SINGLELINE;
            rcRight.left += 2;
            param.prc = &rcRight;
            param.szText = m_strValue.c_str();
            pRenderTarget->DrawString(m_pShareData->pRenderFont, &param);
        }
    }
}

void  PropertyCtrlEditItem::OnSize(UINT nType, int cx, int cy)
{
    if (m_pIPropertyCtrlEditItem->IsSelected() && m_pShareData->pEditCtrl->IsMySelfVisible())
    {
        m_pShareData->pEditCtrl->SetConfigLeft((*m_pShareData->pSlitterLinePos) + 1);

//         CRect rc;
//         m_pIPropertyCtrlEditItem->GetParentRect(&rc);
// 
//         CRect rcEdit;
//         rcEdit.left = (*m_pShareData->pSlitterLinePos) + 1;
//         rcEdit.right = cx;
//         rcEdit.top = rc.top+1;
//         rcEdit.bottom = cy;
//         m_pShareData->pEditCtrl->SetObjectPos(&rcEdit, SWP_NOREDRAW);
    }
    SetMsgHandled(FALSE);
}

void  PropertyCtrlEditItem::SetValueText(const TCHAR* szText)
{
    if (szText)
        m_strValue = szText;
    else
        m_strValue.clear();

//     if (m_pIPropertyCtrlEditItem->IsSelected() && m_pShareData->pEditCtrl->IsMySelfVisible())
//     {
//         m_pShareData->pEditCtrl->SetText(m_strValue.c_str());
//     }
}

void  PropertyCtrlEditItem::SetDefaultValueText(const TCHAR* szText)
{
	if (szText)
		m_strDefautValue = szText;
	else
		m_strDefautValue.clear();
}

void  PropertyCtrlEditItem::SetKeyText(const TCHAR* szText)
{
	if (szText)
		m_strKey = szText;
	else
		m_strKey.clear();
}

LRESULT  PropertyCtrlEditItem::OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam & OSB_SELECTED)
    {
        bool bSelected = m_pIPropertyCtrlEditItem->IsSelected();

        if (bSelected)
        {
            BeginEdit();
        }
        else
        {
            if (0 != _tcscmp(m_pShareData->pEditCtrl->GetText(), m_strValue.c_str()))
            {
                OnEditReturn(0,0);
            }
            CancelEdit();
        }
    }
    return 0;
}
LRESULT  PropertyCtrlEditItem::OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const TCHAR* szText = (const TCHAR* )wParam;
	this->SetValueText(szText);

	return 0;
}
LRESULT  PropertyCtrlEditItem::OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const TCHAR* szText = (const TCHAR* )wParam;
	this->SetDefaultValueText(szText);

	return 0;
}

LRESULT  PropertyCtrlEditItem::OnGetKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)m_strKey.c_str();
}

LRESULT  PropertyCtrlEditItem::OnEditEsc(WPARAM w, LPARAM l)
{
    m_pShareData->pEditCtrl->SetText(m_strValue.c_str());
    m_pShareData->pEditCtrl->SetSel(0, -1);
    return 0;
}
LRESULT  PropertyCtrlEditItem::OnEditReturn(WPARAM w, LPARAM l)
{   
    UIMSG  msg;
    msg.message = UI_PROPERTYCTRL_MSG_EDITITEM_ACCEPTCONTENT;

	PROPERTYCTRL_EDIT_ITEM_ACCEPTCONTENT  param = {0};
	param.pItem = m_pIPropertyCtrlEditItem;
	param.szKey = m_strKey.c_str();
	param.szNewValue = m_pShareData->pEditCtrl->GetText();
	if (0 == _tcslen(param.szNewValue) || m_strDefautValue == param.szNewValue)
		param.bDefault = true;
	msg.wParam = (WPARAM)&param;

    m_pIPropertyCtrlEditItem->GetIListCtrlBase()->DoNotify(&msg);

    return 0;
}

void  PropertyCtrlEditItem::GetValueColumnRect(CRect* prc)
{
    if (NULL == prc)
        return;

    CRect rcParent;
    m_pIPropertyCtrlEditItem->GetParentRect(&rcParent);

    prc->SetRectEmpty();
    prc->right = rcParent.Width();
    prc->bottom = rcParent.Height();
    prc->left = *(m_pShareData->pSlitterLinePos);
}

// 
// void PropertyCtrlEditItem::AcceptEditContent()
// {
//     if (NULL == m_pEditingItem)
//         return;
// 
//     ListViewSubItemBase* pSubItem = this->GetSubItem((ListTreeViewItem*)m_pEditingItem, PROP_VALUE_COL);
//     if (NULL != pSubItem)
//     {
//         int nLength = m_pEdit->GetTextLength();
//         TCHAR* pszText = new TCHAR[nLength+1];
//         m_pEdit->GetText(pszText, nLength+1);
//         pSubItem->SetText(pszText);
//         SAFE_ARRAY_DELETE(pszText);
//     }
// }
// void PropertyCtrlEditItem::RejestEditContent()
// {
//     if (NULL == m_pEditingItem)
//         return;
// 
//     ListViewSubItemBase* pSubItem = this->GetSubItem((ListTreeViewItem*)m_pEditingItem, PROP_VALUE_COL);
//     if (NULL != pSubItem)
//     {
//         m_pEdit->SetText(pSubItem->GetText());
//     }
// }

void  PropertyCtrlEditItem::CancelEdit()
{
    if (NULL == m_pShareData->pEditCtrl)
        return;

    m_pShareData->pEditCtrl->SetVisible(false, false, false);
    m_pShareData->pEditCtrl->SetNotify(NULL, 0);
    m_pShareData->pEditCtrl->RemoveMeInTheTree();
    m_pShareData->pEditCtrl->SetText(NULL);
}
void  PropertyCtrlEditItem::BeginEdit()
{
    if (NULL == m_pShareData->pEditCtrl)
        return;

    CRect rc;
    GetValueColumnRect(&rc);
    rc.DeflateRect(2,1,1,1);

    m_pShareData->pEditCtrl->SetText(m_strValue.c_str());

    IPanel* pPanel = m_pIPropertyCtrlEditItem->GetRootPanel();
    pPanel->AddChild(m_pShareData->pEditCtrl);

//    m_pShareData->pEditCtrl->SetObjectPos(&rc, 0);
    m_pShareData->pEditCtrl->SetConfigLeft(rc.left);
    m_pShareData->pEditCtrl->SetConfigRight(1);
    m_pShareData->pEditCtrl->SetConfigTop(1);
    m_pShareData->pEditCtrl->SetConfigBottom(1);

    m_pShareData->pEditCtrl->SetVisible(true, false, false);
    m_pShareData->pEditCtrl->SetNotify(m_pIPropertyCtrlEditItem, 0);
    pPanel->GetLayout()->Arrange(m_pShareData->pEditCtrl, true);

    // ����Edit���㣬���������༭ģʽ
    IListCtrlBase* pCtrl = m_pIPropertyCtrlEditItem->GetIListCtrlBase();
    pCtrl->SetFocusObject(m_pShareData->pEditCtrl);

    // ���������ʽ���п����������λ��edit��
    // Ȼ�����EDIT�еĹ��λ��Ϊ����µ�λ��
    HWND  hWnd = pCtrl->GetHWND(); 
    POINT pt;
    GetCursorPos(&pt);
    MapWindowPoints(NULL, hWnd, &pt, 1);
    PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}

}