#include "stdafx.h"
#include "PropertyCtrlComboBoxItem.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"

namespace UI
{
#pragma region // share data
PropertyCtrlComboBoxItemShareData::PropertyCtrlComboBoxItemShareData()
{
    m_pIPropertyCtrlComboBoxItemShareData = NULL;
    pSlitterLinePos = NULL;
    pRenderFont = NULL;
    pComboBoxCtrl = NULL;
}
PropertyCtrlComboBoxItemShareData::~PropertyCtrlComboBoxItemShareData()
{
	
}
#pragma endregion 

//////////////////////////////////////////////////////////////////////////

PropertyCtrlComboBoxItem::PropertyCtrlComboBoxItem()
{
    m_pIPropertyCtrlComboBoxItem = NULL;
    m_pShareData = NULL;
}
PropertyCtrlComboBoxItem::~PropertyCtrlComboBoxItem()
{
    // 被删除时可能控件是失焦状态，自己的选中状态也被清除了
    // if (m_pIPropertyCtrlComboBoxItem->IsSelected())

    IObject* pObj = m_pShareData->pComboBoxCtrl->GetParentObject();
    if (pObj && pObj == m_pIPropertyCtrlComboBoxItem->GetRootPanel())
    {
        m_pShareData->pComboBoxCtrl->SetVisible(false, false, false);
        m_pShareData->pComboBoxCtrl->SetNotify(NULL, 0);
        m_pShareData->pComboBoxCtrl->RemoveMeInTheTree();
		m_pShareData->pComboBoxCtrl->ResetContent();
    }

	map<int, OptionItem*>::iterator iter = m_mapItems.begin();
	for (; iter != m_mapItems.end(); iter++)
	{
		OptionItem* pItem = iter->second;
		delete pItem;
	}
	m_mapItems.clear();
}

void  PropertyCtrlComboBoxItem::OnInitialize()
{
    DO_PARENT_PROCESS(IPropertyCtrlComboBoxItem, IListItemBase);

    m_pIPropertyCtrlComboBoxItem->SetItemType(LISTITEM_TYPE_PropertyCtrlCombobox);

    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlComboBoxItem->GetIListCtrlBase();
    IPropertyCtrlComboBoxItemShareData*  pData = static_cast<IPropertyCtrlComboBoxItemShareData*>(
        pListCtrl->GetItemTypeShareData(LISTITEM_TYPE_PropertyCtrlCombobox));

    if (!pData)
    {
        IPropertyCtrlComboBoxItemShareData::CreateInstance(pListCtrl->GetUIApplication(), &pData);
        m_pShareData = pData->GetImpl();
        pListCtrl->SetItemTypeShareData(LISTITEM_TYPE_PropertyCtrlCombobox, pData);
        m_pShareData->pSlitterLinePos = (UINT*)UISendMessage(pListCtrl, UI_PROPERTYCTRL_MSG_GETSPLITTERLINEPOS_PTR);
        m_pShareData->pComboBoxCtrl = (IComboBox*)UISendMessage(pListCtrl, UI_PROPERTYCTRL_MSG_GETCOMBOBOXCTRL);

        ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
        if (pTextRender)
            m_pShareData->pRenderFont = pTextRender->GetRenderFont();
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }

    m_pIPropertyCtrlComboBoxItem->CreateRootPanel();
}


void  PropertyCtrlComboBoxItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIPropertyCtrlComboBoxItem->GetIListCtrlBase();

    CRect rcParent;
    m_pIPropertyCtrlComboBoxItem->GetParentRect(&rcParent);

    CRect rcBlank(rcParent);
    rcBlank.right = pListCtrl->GetChildNodeIndent()* m_pIPropertyCtrlComboBoxItem->GetDepth();

    CRect rcLeft(rcParent);
    rcLeft.left = rcBlank.right;
    rcLeft.right = *(m_pShareData->pSlitterLinePos);

    CRect rcRight(rcParent);
    rcRight.left = rcLeft.right;

    // 左侧背景
    Color c(PROPERTYCTRL_LINE_COLOR);
    pRenderTarget->FillRect(&rcBlank, &c);

    // 中间的分隔线
    IRenderPen* pPen = pRenderTarget->CreateSolidPen(1, &c);
    pRenderTarget->DrawLine(rcLeft.right, rcLeft.top, rcLeft.right, rcLeft.bottom, pPen);

    // 一行的横线
//     IListItemBase* pNextItem = m_pIPropertyCtrlComboBoxItem->GetNextVisibleItem();
//     if (NULL == pNextItem || 
//         (pNextItem && pNextItem->GetDepth() == m_pIPropertyCtrlComboBoxItem->GetDepth()))
    {
        pRenderTarget->DrawLine(rcParent.left, rcParent.bottom-1, rcParent.right, rcParent.bottom-1, pPen);
    }
    SAFE_RELEASE(pPen);

    // 选中状态
    bool bSelectd = m_pIPropertyCtrlComboBoxItem->IsSelected();
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

    // 绘制文本
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
        param.szText = m_pIPropertyCtrlComboBoxItem->GetText();
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

void  PropertyCtrlComboBoxItem::OnSize(UINT nType, int cx, int cy)
{
    if (m_pIPropertyCtrlComboBoxItem->IsSelected() && m_pShareData->pComboBoxCtrl->IsMySelfVisible())
    {
        m_pShareData->pComboBoxCtrl->SetConfigLeft((*m_pShareData->pSlitterLinePos) + 1);

//         CRect rc;
//         m_pIPropertyCtrlComboBoxItem->GetParentRect(&rc);
// 
//         CRect rcEdit;
//         rcEdit.left = (*m_pShareData->pSlitterLinePos) + 1;
//         rcEdit.right = cx;
//         rcEdit.top = rc.top+1;
//         rcEdit.bottom = cy;
//         m_pShareData->pComboBoxCtrl->SetObjectPos(&rcEdit, SWP_NOREDRAW);
    }
    SetMsgHandled(FALSE);
}

void  PropertyCtrlComboBoxItem::SetValueText(const TCHAR* szText)
{
    if (szText)
        m_strValue = szText;
    else
        m_strValue.clear();

//     if (m_pIPropertyCtrlComboBoxItem->IsSelected() && m_pShareData->pComboBoxCtrl->IsMySelfVisible())
//     {
//         m_pShareData->pComboBoxCtrl->SetText(m_strValue.c_str());
//     }
}

void  PropertyCtrlComboBoxItem::SetDefaultValueText(const TCHAR* szText)
{
	if (szText)
		m_strDefautValue = szText;
	else
		m_strDefautValue.clear();
}

void  PropertyCtrlComboBoxItem::SetKeyText(const TCHAR* szText)
{
	if (szText)
		m_strKey = szText;
	else
		m_strKey.clear();
}

LRESULT  PropertyCtrlComboBoxItem::OnStateChanged(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (wParam & OSB_SELECTED)
    {
        bool bSelected = m_pIPropertyCtrlComboBoxItem->IsSelected();

        if (bSelected)
        {
            BeginEdit();
        }
        else
        {
            if (0 != _tcscmp(m_pShareData->pComboBoxCtrl->GetText(), m_strValue.c_str()))
            {
//                OnEditReturn(0,0);
            }
            CancelEdit();
        }
    }
    return 0;
}
LRESULT  PropertyCtrlComboBoxItem::OnSetValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const TCHAR* szText = (const TCHAR* )wParam;
	this->SetValueText(szText);

	return 0;
}
LRESULT  PropertyCtrlComboBoxItem::OnSetDefaultValue(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	const TCHAR* szText = (const TCHAR* )wParam;
	this->SetDefaultValueText(szText);

	return 0;
}

LRESULT  PropertyCtrlComboBoxItem::OnGetKey(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return (LRESULT)m_strKey.c_str();
}

LRESULT  PropertyCtrlComboBoxItem::OnComboBoxSelChanged(WPARAM wParam, LPARAM lParam)
{
	UIMSG  msg;
	msg.message = UI_PROPERTYCTRL_MSG_COMBOBOXITEM_ACCEPTCONTENT;

	PROPERTYCTRL_COMBOBOX_ITEM_ACCEPTCONTENT  param = {0};
	param.pItem = m_pIPropertyCtrlComboBoxItem;
	param.szKey = m_strKey.c_str();
	IListItemBase* pSelItem = (IListItemBase*)lParam;
	if (pSelItem)
	{
		int nData = (int)pSelItem->GetData();
		map<int, OptionItem*>::iterator iter = m_mapItems.find(nData);
		if (iter == m_mapItems.end())
			return 0;

		param.szNewValue = iter->second->strValue.c_str();
	}
	if (0 == _tcslen(param.szNewValue) || m_strDefautValue == param.szNewValue)
		param.bDefault = true;
	msg.wParam = (WPARAM)&param;

	m_pIPropertyCtrlComboBoxItem->GetIListCtrlBase()->DoNotify(&msg);

	return 0;
}
// LRESULT  PropertyCtrlComboBoxItem::OnEditEsc(WPARAM w, LPARAM l)
// {
//     m_pShareData->pComboBoxCtrl->SetText(m_strValue.c_str());
//     m_pShareData->pComboBoxCtrl->SetSel(0, -1);
//     return 0;
// }
// LRESULT  PropertyCtrlComboBoxItem::OnEditReturn(WPARAM w, LPARAM l)
// {   
//     UIMSG  msg;
//     msg.message = UI_PROPERTYCTRL_MSG_EDITITEM_ACCEPTCONTENT;
// 
// 	PROPERTYCTRL_EDIT_ITEM_ACCEPTCONTENT  param = {0};
// 	param.pItem = m_pIPropertyCtrlComboBoxItem;
// 	param.szKey = m_strKey.c_str();
// 	param.szNewValue = m_pShareData->pComboBoxCtrl->GetText();
// 	if (m_strDefautValue == param.szNewValue)
// 		param.bDefault = true;
// 	msg.wParam = (WPARAM)&param;
// 
//     m_pIPropertyCtrlComboBoxItem->GetIListCtrlBase()->DoNotify(&msg);
// 
//     return 0;
// }

void  PropertyCtrlComboBoxItem::GetValueColumnRect(CRect* prc)
{
    if (NULL == prc)
        return;

    CRect rcParent;
    m_pIPropertyCtrlComboBoxItem->GetParentRect(&rcParent);

    prc->SetRectEmpty();
    prc->right = rcParent.Width();
    prc->bottom = rcParent.Height();
    prc->left = *(m_pShareData->pSlitterLinePos);
}

void  PropertyCtrlComboBoxItem::CancelEdit()
{
    if (NULL == m_pShareData->pComboBoxCtrl)
        return;

    m_pShareData->pComboBoxCtrl->SetVisible(false, false, false);
    m_pShareData->pComboBoxCtrl->SetNotify(NULL, 0);
    m_pShareData->pComboBoxCtrl->RemoveMeInTheTree();
//    m_pShareData->pComboBoxCtrl->SetText(NULL);
	m_pShareData->pComboBoxCtrl->ResetContent();
}
void  PropertyCtrlComboBoxItem::BeginEdit()
{
    if (NULL == m_pShareData->pComboBoxCtrl)
        return;

    CRect rc;
    GetValueColumnRect(&rc);
    rc.DeflateRect(2,1,1,1);

    m_pShareData->pComboBoxCtrl->SetText(m_strValue.c_str());
	IListBox* pListBox = m_pShareData->pComboBoxCtrl->GetListBox();
	map<int, OptionItem*>::iterator iter = m_mapItems.begin();
	for (; iter != m_mapItems.end(); iter++)
	{
		OptionItem* pItem = iter->second;
		pListBox->AddString(pItem->strText.c_str(), LISTITEM_OPFLAG_NOALL)->SetData(pItem->nIndex);
	}
    IPanel* pPanel = m_pIPropertyCtrlComboBoxItem->GetRootPanel();
    pPanel->AddChild(m_pShareData->pComboBoxCtrl);

//    m_pShareData->pComboBoxCtrl->SetObjectPos(&rc, 0);
    m_pShareData->pComboBoxCtrl->SetConfigLeft(rc.left);
    m_pShareData->pComboBoxCtrl->SetConfigRight(1);
    m_pShareData->pComboBoxCtrl->SetConfigTop(1);
    m_pShareData->pComboBoxCtrl->SetConfigBottom(1);

    m_pShareData->pComboBoxCtrl->SetVisible(true, false, false);
    m_pShareData->pComboBoxCtrl->SetNotify(m_pIPropertyCtrlComboBoxItem, 0);
    pPanel->GetLayout()->Arrange(m_pShareData->pComboBoxCtrl, true);

    // 设置Edit焦点，立即启动编辑模式
    IListCtrlBase* pCtrl = m_pIPropertyCtrlComboBoxItem->GetIListCtrlBase();
    pCtrl->SetFocusObject(m_pShareData->pComboBoxCtrl);

    // 更新鼠标样式，有可能鼠标现在位于edit上
    // 然后更新EDIT中的光标位置为鼠标下的位置
    HWND  hWnd = pCtrl->GetHWND(); 
    POINT pt;
    GetCursorPos(&pt);
    MapWindowPoints(NULL, hWnd, &pt, 1);
    PostMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(pt.x, pt.y));
    PostMessage(hWnd, WM_LBUTTONUP, 0, MAKELPARAM(pt.x, pt.y));
}

void  PropertyCtrlComboBoxItem::AddOption(const TCHAR* szItemText, const TCHAR* szItemValue)
{
	OptionItem* pItem = new OptionItem;
	pItem->nIndex = m_mapItems.size()+1;
	if (szItemText)
		pItem->strText = szItemText;
	if (szItemValue)
		pItem->strValue = szItemValue;

	m_mapItems[pItem->nIndex] = pItem;
}

}