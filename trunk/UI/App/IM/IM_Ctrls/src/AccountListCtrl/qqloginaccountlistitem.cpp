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
	m_pIQQLoginAccountListItem->SetText(m_strAccount.c_str());  // ������Combobox�е�edit����ʾ
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
//     // ����listitembase��OnLButtonDown��������굯��ʱѡ��
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
//     if (pIListCtrlBase->TestStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE))    // �������굯������������Ϊ��ѡ�е���
//     {
//         UI::IListItemBase* pNewHover = pIListCtrlBase->HitTest(point);  // ����ȡhover������Ϊ������Ƶ�listbox����ʱ������Ȼ�������һ��hover item
// 
//         int nUpdateFlag = 0;
//         if (NULL != pNewHover && pNewHover != pIListCtrlBase->GetFirstSelectItem())
//             pIListCtrlBase->SelectItem(pIListCtrlBase->GetHoverItem(), false);  // ��Ϊ����Ҫ�ر��ˣ����Բ�ˢ��
//     }
// }

