#include "stdafx.h"
#include "qqloginaccountlistitem.h"


QQLoginAccountListItem::QQLoginAccountListItem()
{
	m_nCurItemHeight = ITEM_NORMAL_HEIGHT;
	m_pFaceBitmap = NULL;
    m_pIQQLoginAccountListItem = NULL;
}
QQLoginAccountListItem::~QQLoginAccountListItem()
{
	SAFE_RELEASE(m_pFaceBitmap);
}

void  QQLoginAccountListItem::GetHeightRef(int** pp)
{
    if (NULL == pp)
        return;
     *pp = &m_nCurItemHeight;
}

void QQLoginAccountListItem::SetName(const TCHAR*  szName)
{
	if (NULL == szName)
	{
		m_strName.clear();
		return;
	}

	m_strName = szName;
}
void QQLoginAccountListItem::SetAccount(const TCHAR*  szAccount)
{
	if (NULL == szAccount)
	{
		m_strAccount.clear();
		return;
	}
	m_strAccount = szAccount;
	m_pIQQLoginAccountListItem->SetText(m_strAccount.c_str());  // 用于在Combobox中的edit中显示
}
void QQLoginAccountListItem::SetFaceImgFilePath(const TCHAR*  szFace)
{
	if (NULL == szFace)
	{
		m_strFaceImgFilePath.clear();
		return;
	}
	m_strFaceImgFilePath = szFace;
	
    UI::IListCtrlBase*  pListCtrl = m_pIQQLoginAccountListItem->GetIListCtrlBase();
    UI::UICreateRenderBitmap(UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, UI::IMAGE_ITEM_TYPE_IMAGE, &m_pFaceBitmap);
	if (m_pFaceBitmap)
	{
		m_pFaceBitmap->LoadFromFile(szFace, true);
	}
}


// void QQLoginAccountListItem::OnLButtonDown(UINT nFlags, POINT point)
// {
//     // 覆盖listitembase的OnLButtonDown，仅在鼠标弹起时选择
//     if (m_pIQQLoginAccountListItem->GetIListCtrlBase()->TestStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE)) 
//         return;
// 
//     SetMsgHandled(FALSE);
// }
// 
// void QQLoginAccountListItem::OnLButtonUp(UINT nFlags, POINT point)
// {
//     SetMsgHandled(FALSE);
//     UI::IListCtrlBase*  pIListCtrlBase = m_pIQQLoginAccountListItem->GetIListCtrlBase();
//     if (pIListCtrlBase->TestStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE))    // 以最后鼠标弹出来的那项作为被选中的项
//     {
//         UI::IListItemBase* pNewHover = pIListCtrlBase->HitTest(point);  // 重新取hover对象。因为当鼠标移到listbox外面时，会仍然保留最后一个hover item
// 
//         int nUpdateFlag = 0;
//         if (NULL != pNewHover && pNewHover != pIListCtrlBase->GetFirstSelectItem())
//             pIListCtrlBase->SelectItem(pIListCtrlBase->GetHoverItem(), false);  // 因为即将要关闭了，所以不刷新
//     }
// }

