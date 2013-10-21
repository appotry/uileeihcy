#include "stdafx.h"
#include "PluginBtnListBoxItem.h"
#include "UISDK\Control\Inc\Interface\icheckbutton.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"

UI_IMPLEMENT_Ixxx_INTERFACE2(IPluginBtnListBoxItem, PluginBtnListBoxItem, IListItemBase);
UI_IMPLEMENT_Ixxx_INTERFACE2(IPluginBtnListBoxShareData, PluginBtnListBoxShareData, IListItemTypeShareData);

void  IPluginBtnListBoxItem::SetIconPath(const TCHAR*  szPath)
{
    return m_pPluginBtnListBoxItemImpl->SetIconPath(szPath);
}
void  IPluginBtnListBoxItem::SetIcon(HBITMAP hBitmap)
{
    return m_pPluginBtnListBoxItemImpl->SetIcon(hBitmap);
}
void  IPluginBtnListBoxItem::SetSortValue(int n)
{
    m_pPluginBtnListBoxItemImpl->SetSortValue(n);
}
int   IPluginBtnListBoxItem::GetSortValue()
{
    return m_pPluginBtnListBoxItemImpl->GetSortValue();
}
void  IPluginBtnListBoxItem::SetCheck(bool b)
{
    m_pPluginBtnListBoxItemImpl->SetCheck(b);
}

PluginBtnListBoxShareData::PluginBtnListBoxShareData()
{
    m_pOddForegndRender = NULL;
    m_pIListCtrl = NULL;
}

PluginBtnListBoxShareData::~PluginBtnListBoxShareData()
{
    SAFE_RELEASE(m_pOddForegndRender);
}

void  PluginBtnListBoxShareData::Init(UI::IListCtrlBase* p)
{
    m_pIListCtrl = p;

    // 获取奇数行foregnd

    UI::IMapAttribute*  pMapAttrib = NULL;
    m_pIListCtrl->GetMapAttribute(&pMapAttrib);
    if (NULL == pMapAttrib)
        return;

    String  strPrefix = XML_FOREGND_RENDER_PREFIX;
    strPrefix.append(_T("odd."));

    pMapAttrib->GetAttr_RenderBase(strPrefix.c_str(), XML_RENDER_TYPE, false, m_pIListCtrl->GetUIApplication(), m_pIListCtrl, &m_pOddForegndRender);
    SAFE_RELEASE(pMapAttrib);
}


//////////////////////////////////////////////////////////////////////////

PluginBtnListBoxItem::PluginBtnListBoxItem()
{
    m_pIPluginBtnListBoxItem = NULL;
    m_pCheckBtn = NULL;
    m_pShareData = NULL;
    m_nSortValue = 0;
}

PluginBtnListBoxItem::~PluginBtnListBoxItem()
{
}

void  PluginBtnListBoxItem::OnPaint(UI::IRenderTarget* pRenderTarget)
{
    CRect rcItem;
    m_pIPluginBtnListBoxItem->GetParentRect(&rcItem);

    UI::IListCtrlBase*  pListCtrl = m_pIPluginBtnListBoxItem->GetIListCtrlBase();
    UINT nRenderState = m_pIPluginBtnListBoxItem->GetItemRenderState();

    int lineIndex = m_pIPluginBtnListBoxItem->GetLineIndex()%2;

    if (lineIndex)
    {
        if (m_pShareData && m_pShareData->m_pOddForegndRender)
            m_pShareData->m_pOddForegndRender->DrawState(pRenderTarget, &rcItem, nRenderState);
    }
    else
    {
        UI::IRenderBase* pRender = pListCtrl->GetForeRender();
        if (pRender)
            pRender->DrawState(pRenderTarget, &rcItem, nRenderState);
    }

    if (!m_icon.IsNull())
    {
        int  nHeight = m_icon.GetHeight();
        int y = rcItem.top + (rcItem.Height()-nHeight)/2;
        m_icon.Draw(pRenderTarget->GetBindHDC(), 75, y);
    }

    CRect rcText = rcItem;
    UI::ITextRenderBase* pTextRender = pListCtrl->GetTextRender();
    if (pTextRender)
    {
        rcText.left += 96;  // <-- todo:
        pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, m_pIPluginBtnListBoxItem->GetText());
    }
}

void  PluginBtnListBoxItem::OnInitialize()
{
    SetMsgHandled(FALSE);

    UI::IListCtrlBase*  pListCtrl = m_pIPluginBtnListBoxItem->GetIListCtrlBase();
    if (NULL == pListCtrl)
        return;

    IPluginBtnListBoxShareData* pData = (IPluginBtnListBoxShareData*)pListCtrl->GetItemTypeShareData(0);
    if (NULL == pData)
    {
        IPluginBtnListBoxShareData::CreateInstance(pListCtrl->GetUIApplication(), &pData);
        m_pShareData = pData->GetImpl();
        m_pShareData->Init(pListCtrl);

        pListCtrl->SetItemTypeShareData(0, pData);
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }

    // 创建CheckButton
    m_pCheckBtn = NULL;
    UI::ICheckButton::CreateInstance(pListCtrl->GetUIApplication(), &m_pCheckBtn);

    UI::IMapAttribute*  pMapAttrib = NULL;
    pListCtrl->GetMapAttribute(&pMapAttrib);
    m_pCheckBtn->SetAttributeByPrefix(_T("checkbtn."), pMapAttrib, false, false);
    pMapAttrib->Release();

    m_pCheckBtn->SetConfigLeft(25);
    m_pCheckBtn->SetConfigTop(5);
    m_pCheckBtn->SetConfigWidth(20);
    m_pCheckBtn->SetConfigHeight(20);

    m_pCheckBtn->SetNotify(m_pIPluginBtnListBoxItem, 1);
    m_pIPluginBtnListBoxItem->CreateRootPanel();
    UI::IPanel* pPanel = m_pIPluginBtnListBoxItem->GetRootPanel();
    pPanel->AddChild(m_pCheckBtn);
    pPanel->GetLayout()->Arrange(NULL);
}

void  PluginBtnListBoxItem::SetIconPath(const TCHAR*  szPath)
{
    if (NULL == szPath)
        return;

    m_icon.Destroy();
    if (UI::Util::IsFullPath(szPath))
    {
        m_icon.Load(szPath);
        return;
    }

    UI::IListCtrlBase*  pListCtrl = m_pIPluginBtnListBoxItem->GetIListCtrlBase();
    if (!pListCtrl)
        return;
    UI::ISkinRes* pSkinRes = pListCtrl->GetUIApplication()->GetActiveSkinRes();
    pSkinRes->GetDataSource()->Load_Image(&m_icon, szPath);    
}

void  PluginBtnListBoxItem::SetIcon(HBITMAP hBitmap)
{
    m_icon.Attach(hBitmap, true);
}

void  PluginBtnListBoxItem::SetCheck(bool b)
{
    if (!m_pCheckBtn)
        return;

    if (b)
        m_pCheckBtn->SetChecked();
    else
        m_pCheckBtn->SetUnChecked();
}

void  PluginBtnListBoxItem::OnBtnClicked(UI::IMessage*  pMsgFrom)
{
    UIASSERT(pMsgFrom == m_pCheckBtn);

     UI::IListCtrlBase*  pListCtrl = m_pIPluginBtnListBoxItem->GetIListCtrlBase();
     UI::UIMSG  msg;
     msg.message = UI_WM_NOTIFY;
     msg.nCode= LBN_PLUGIN_CHECKBTN_CLICKED;
     msg.wParam = (WPARAM)pListCtrl;

     PluginBtnListBoxItemCheckBtnClick nmhdr;
     nmhdr.bChecked = m_pCheckBtn->IsChecked();
     nmhdr.pItem = m_pIPluginBtnListBoxItem;
     msg.lParam = (LPARAM)&nmhdr;
     pListCtrl->DoNotify(&msg);
}