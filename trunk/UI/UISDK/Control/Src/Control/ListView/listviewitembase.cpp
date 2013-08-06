#include "stdafx.h"
#include "listviewitembase.h"
#include "UISDK\Control\Inc\Interface\ilistview.h"

namespace UI
{

ListViewItemBaseShareData::ListViewItemBaseShareData()
{
    m_pIListViewItemBaseShareData = NULL;
    pColumnInfo = NULL;
}
ListViewItemBaseShareData::~ListViewItemBaseShareData()
{

}

ListViewItemBase::ListViewItemBase()
{
    m_pIListViewItemBase = NULL;
    m_pShareData = NULL;

    m_nSubItemCount = 0;
    m_ppSubItem = NULL;
}
ListViewItemBase::~ListViewItemBase()
{
    DestroySubItems();
}

void  ListViewItemBase::DestroySubItems()
{
    if (m_ppSubItem)
    {
        for (UINT i = 0; i < m_nSubItemCount; i++)
        {
            delete m_ppSubItem[i];
        }
        SAFE_ARRAY_DELETE(m_ppSubItem);
        m_nSubItemCount = 0;
    }
}

void  ListViewItemBase::OnInitialize()
{
    DO_PARENT_PROCESS(IListViewItemBase, IListItemBase);

    IListCtrlBase* pCtrl = m_pIListViewItemBase->GetIListCtrlBase();
    m_pIListViewItemBase->SetItemType(LISTITEM_TYPE_LISTVIEWITEMBASE);

    IListViewItemBaseShareData*  pData = static_cast<IListViewItemBaseShareData*>(
        pCtrl->GetItemTypeShareData(LISTITEM_TYPE_LISTVIEWITEMBASE));

    if (NULL == pData)
    {
        IListViewItemBaseShareData::CreateInstance(pCtrl->GetUIApplication(), &pData);
        m_pShareData = pData->GetImpl();
        pCtrl->SetItemTypeShareData(LISTITEM_TYPE_LISTVIEWITEMBASE, pData);

        m_pShareData->pColumnInfo = (IColumnsInfo*)UISendMessage(pCtrl, UI_LVM_GETCOLUMNSINFO);
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }

    // 初始化sub item
    if (m_pShareData->pColumnInfo)
    {
        int nCount = m_pShareData->pColumnInfo->GetCount();
        if (nCount > 1)
        {
            m_nSubItemCount = nCount-1;
            m_ppSubItem = new SubItemInfo*[m_nSubItemCount];
            for (UINT i = 0; i < m_nSubItemCount; i++)
            {
                SubItemInfo* p = new SubItemInfo;
                m_ppSubItem[i] = p;
            }
        }
    }
}


void  ListViewItemBase::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIListViewItemBase->GetIListCtrlBase();

    CRect  rcParent;
    m_pIListViewItemBase->GetParentRect(&rcParent);

    // 背景
    UINT nRenderState = 0;
    IRenderBase* pRender = pListCtrl->GetForeRender();
    if (pRender)
    {
        nRenderState = m_pIListViewItemBase->GetItemRenderState();
        pRender->DrawState(pRenderTarget, &rcParent, nRenderState);
    }

    if (m_pIListViewItemBase->IsFocus() && !m_pIListViewItemBase->IsSelected())
    {
        IRenderBase* pFocusRender = pListCtrl->GetFocusRender();
        if (pFocusRender)
        {
            bool bCtrlFocus = pListCtrl->IsFocus();
            pFocusRender->DrawState(pRenderTarget, &rcParent, bCtrlFocus ?
                    LISTCTRLITEM_FOCUS_RENDER_STATE_NORMAL:LISTCTRLITEM_FOCUS_RENDER_STATE_NORMAL);
        }
    }

    // 文本
    if (!m_pShareData->pColumnInfo)
        return;

    ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (!pTextRender)
        return;

    CRect rcSubItem = rcParent;
    rcSubItem.right = rcSubItem.left;

    int nCount = m_pShareData->pColumnInfo->GetCount();
    int nDrawPos = 0;  // 记录下当前绘制位置
    for (int i = 0; i < nCount; i++)
    {
        ColumnInfo* pInfo = m_pShareData->pColumnInfo->GetItemInfo(i);
        if (0 == pInfo->nWidth)
            continue;

        nDrawPos = rcSubItem.left + pInfo->nWidth;
        rcSubItem.right = nDrawPos;
        rcSubItem.left += pInfo->nIndentLeft;
        rcSubItem.right -= pInfo->nIndentRight;

        DrawSubItemData data = {i, pRenderTarget, pTextRender, pInfo, &rcSubItem, nRenderState};
        OnDrawSubItem(&data);
        
        rcSubItem.left = nDrawPos;
    }
}

void  ListViewItemBase::OnDrawSubItem(DrawSubItemData* pSubItem)
{
   if (0 == pSubItem->nColumn)
   {
        pSubItem->pTextRender->DrawState(
            pSubItem->pRenderTarget, 
            pSubItem->prc, 
            pSubItem->nRenderState, 
            m_pIListViewItemBase->GetText(),
            pSubItem->pInfo->nFormat);

        return;
   }

   UINT nIndex = pSubItem->nColumn - 1;
   if (nIndex >= m_nSubItemCount)
       return;

   pSubItem->pTextRender->DrawState(
       pSubItem->pRenderTarget, 
       pSubItem->prc, 
       pSubItem->nRenderState, 
       m_ppSubItem[nIndex]->m_strText.c_str(),
       pSubItem->pInfo->nFormat);
  
}

LRESULT  ListViewItemBase::OnSetSubItemText(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT nIndex = (UINT)wParam;
    if (0 == nIndex)
    {
        m_pIListViewItemBase->SetText((const TCHAR*)lParam);
        return 0;
    }

    if (nIndex > m_nSubItemCount)
        return 0;

    if (lParam)
        m_ppSubItem[nIndex-1]->m_strText = (const TCHAR*)lParam;
    else
        m_ppSubItem[nIndex-1]->m_strText.clear();

    return 0;
}
LRESULT  ListViewItemBase::OnGetSubItemText(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT nIndex = (UINT)wParam;
    if (0 == nIndex)
        return (LRESULT)m_pIListViewItemBase->GetText();

    if (nIndex > m_nSubItemCount)
        return NULL;

    return (LRESULT)m_ppSubItem[nIndex-1]->m_strText.c_str();
}

LRESULT  ListViewItemBase::OnAddColumn(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT nPos = (UINT)wParam;
    if (nPos > m_nSubItemCount)
        return 0;

    SubItemInfo* pSubItem = new SubItemInfo;

    SubItemInfo** pp = new SubItemInfo*[m_nSubItemCount+1];
    for (UINT i = 0, j = 0; i < m_nSubItemCount; i++, j++)
    {
        if (i == nPos)
        {
            pp[i] = pSubItem;
            j--;
        }
        else
        {
            pp[i] = m_ppSubItem[j];
        }
    }
    m_nSubItemCount ++;
    SAFE_ARRAY_DELETE(m_ppSubItem);
    m_ppSubItem = pp;

    return 0;
}

LRESULT  ListViewItemBase::OnRemoveColumn(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UINT nPos = (UINT)wParam;
    if (nPos >= m_nSubItemCount)
        return 0;

    if (1 >= m_nSubItemCount)
    {
        DestroySubItems();
    }
    else
    {
        SubItemInfo** pp = new SubItemInfo*[m_nSubItemCount-1];
        for (UINT i = 0, j = 0; i < m_nSubItemCount; i++, j++)
        {
            if (i == nPos)
            {
                j++;
            }
            pp[i] = m_ppSubItem[j];
        }
        m_nSubItemCount ++;
        SAFE_ARRAY_DELETE(m_ppSubItem);
        m_ppSubItem = pp;
    }
    return 0;
}

LRESULT  ListViewItemBase::OnClearColumn(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    DestroySubItems();
    return 0;
}

}