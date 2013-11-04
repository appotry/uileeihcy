#include "stdafx.h"
#include "listitemrootpanel.h"
#include "UISDK\Kernel\Src\UIObject\ListCtrl\listctrlbase.h"

namespace UI
{

UI_IMPLEMENT_Ixxx_INTERFACE(IListItemRootPanel, ListItemRootPanel, Panel);

void  IListItemRootPanel::SetListCtrlItemPtr(IListCtrlBase* p1, IListItemBase* p2)
{
    m_pListItemRootPanelImpl->SetListCtrlItemPtr(p1, p2);
}
IListItemBase*  IListItemRootPanel::GetListItemPtr()
{
    return m_pListItemRootPanelImpl->GetListItemPtr();
}

ListItemRootPanel::ListItemRootPanel()
{
    m_pIListItemRootPanel = NULL;

    m_pIListCtrl = NULL;
    m_pIListItem = NULL;
}

void  ListItemRootPanel::ResetAttribute()
{
    DO_PARENT_PROCESS(IListItemRootPanel, IPanel);
    m_pIListItemRootPanel->ModifyStyle(0, OBJECT_STYLE_TRANSPARENT, false);  // ��֤�ɼ��������Ӷ����ڵ���ˢ��ʱ���Ͳ��ü������ϱ�����
}
void  ListItemRootPanel::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    if (NULL == pRenderTarget)
        return;

    if (NULL == m_pIListCtrl || NULL == m_pIListItem)
        return;

    //m_pIListCtrl->RedrawItem(&m_pIListItem, 1);   // �ر�����һ��ˢ�º��������Ż�

    // ��DCƫ�����Ƶ�listctrl���Ͻ�
    POINT ptSave = {0, 0};
    pRenderTarget->GetViewportOrgEx(&ptSave);

    CRect  rcWindow;
    this->GetWindowRect(&rcWindow);
    RenderContext roc(&rcWindow, false);
    
    roc.m_ptOffset.x -= m_pIListItemRootPanel->GetParentRectL();
    roc.m_ptOffset.y -= m_pIListItemRootPanel->GetParentRectT();
    roc.Update(pRenderTarget);

    m_pIListCtrl->RedrawItemByInnerCtrl(pRenderTarget, m_pIListItem);
    pRenderTarget->SetViewportOrgEx(ptSave.x, ptSave.y, NULL);
}

void  ListItemRootPanel::SetListCtrlItemPtr(IListCtrlBase* p1, IListItemBase* p2)
{
    m_pIListCtrl = p1;
    m_pIListItem = p2;
}


}