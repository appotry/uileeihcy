#include "StdAfx.h"
#include "ControlDemoWindow.h"
#include "resource.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Control\Inc\Interface\ilistbox.h"
#include "UISDK\Control\Inc\Interface\istringlistitem.h"
#include "UISDK\Control\Inc\Interface\iiconstringlistitem.h"
#include "UISDK\Control\Inc\Interface\ilistview.h"
#include "UISDK\Control\Inc\Interface\iheaderctrl.h"
#include "UISDK\Control\Inc\Interface\ipropertyctrl.h"
#include "UISDK\Control\Inc\Interface\icombobox.h"

CControlDemoWindow::CControlDemoWindow()
{
}

CControlDemoWindow::~CControlDemoWindow(void)
{
}

void CControlDemoWindow::OnInitWindow()
{
    __super::xProcessMessage(GetCurMsg(), 0, 0);

    HRESULT hr = SetCanDrop(true);
    UIASSERT(SUCCEEDED(hr));
    this->ModifyStyle(OBJECT_STYLE_RECEIVE_DRAGDROPEVENT, 0, false);

//     UI::IListBox* pListBox = (UI::IListBox*)FindChildObject(_T("test_listbox"));
//     if (pListBox)
//     {
//         for (int i = 0; i < 10; i++)
//         {
//             TCHAR szText[32] = _T("");
//             _stprintf(szText, _T("%d ---"), i);
//             UI::IStringListItem* pItem = NULL;
//             UI::IStringListItem::CreateInstance(GetUIApplication(), &pItem);
//             pListBox->AddItem(pItem);
//             pItem->SetText(szText);
// 
//             //-----------------------------
// 
//             _stprintf(szText, _T("%d +++++"), i);
//             UI::IIconStringListItem* pItem2 = NULL;
//             UI::IIconStringListItem::CreateInstance(GetUIApplication(), &pItem2);
//             pListBox->AddItem(pItem2);
//             pItem2->SetText(szText);
//         }
//     }

    UI::IListView* pListView = (UI::IListView*)FindChildObject(_T("test_listview"));
    if (pListView)
    {
        UI::IHeaderListCtrl*  pHeader = pListView->GetHeaderCtrl();
        if (pHeader)
        {
            pHeader->InsertColumn(0, _T("First"), 100);
            pHeader->InsertColumn(1, _T("Second"), 100);
        }
        for (int i = 0; i < 3; i++)
        {
            pListView->AddTextItem(_T("1111111111111111"));
            pListView->AddTextItem(_T("2222222222222222"));
            pListView->AddTextItem(_T("3333333333333333"));

            pListView->SetItemText(i*3,   1, _T("aaaaa"), false);
            pListView->SetItemText(i*3+1, 1, _T("bbbbb"), false);
            pListView->SetItemText(i*3+2, 1, _T("ccccc"), false);
        }
    }

    UI::IComboBox*  pCombo = (UI::IComboBox*)FindChildObject(_T("combo"));
    if (pCombo)
    {
        pCombo->AddString(_T("aaaaaa"));
        pCombo->AddString(_T("aaaaaa"));
        pCombo->AddString(_T("aaaaaa"));
        pCombo->AddString(_T("aaaaaa"));
        pCombo->AddString(_T("aaaaaa"));
        pCombo->AddString(_T("aaaaaa"));
        pCombo->AddString(_T("aaaaaa"));
        pCombo->AddString(_T("aaaaaa"));
        pCombo->AddString(_T("aaaaaa"));
    }

//     UI::IPropertyCtrl*  pPropertyCtrl = (UI::IPropertyCtrl*)FindChildObject(_T("test_propertyctrl"));
//     if (pPropertyCtrl)
//     {
//         for (int i = 0; i < 3; i++)
//         {
//             UI::IPropertyCtrlGroupItem* pParent = pPropertyCtrl->InsertGroupItem(_T("group1"), _T("TEST_Group"));
//             pPropertyCtrl->InsertEditProperty(_T("name"), _T("test"), _T("name"), pParent);
//             pPropertyCtrl->InsertEditProperty(_T("name"), _T("test"), _T("name"), pParent);
//             pPropertyCtrl->InsertEditProperty(_T("name"), _T("test"), _T("name"), pParent);
//             pPropertyCtrl->InsertEditProperty(_T("name"), _T("test"), _T("name"), pParent);
//             pPropertyCtrl->InsertEditProperty(_T("name"), _T("test"), _T("name"), pParent);
//             pPropertyCtrl->InsertEditProperty(_T("name"), _T("test"), _T("name"), pParent);
//         }
//     }
}

LRESULT  CControlDemoWindow::OnDropTargetEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    UI::DROPTARGETEVENT_DATA* pData = (UI::DROPTARGETEVENT_DATA*)lParam;
    UI::UI_LOG_DEBUG(_T("%s  type:%d"), FUNC_NAME, wParam);

    switch (wParam)
    {
    case UI::_DragEnter:
        break;

    case UI::_DragOver:
        break;

    case UI::_DragLeave:
        break;

    case UI::_Drop:
        break;
    }

    return 0;
}

void  CControlDemoWindow::OnPaint(UI::IRenderTarget* pRenderTarget)
{
#if 0
    CRect rcClient;
    this->GetClientRectAsWin32(&rcClient);
     rcClient.left += 20;
     rcClient.right = rcClient.left+600;
 
 //    rcClient.top += 30;
    rcClient.bottom = rcClient.top + 380;
    UI::IRenderFont*  pRenderFont = GetRenderFont();

    UI::DRAWTEXTPARAM  param;
    param.color.ReplaceRGB(RGB(255,255,255));
    param.nFormatFlag = 0;
    param.prc = &rcClient;
    param.szText = _T("工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n工工工工工工工工工工工工工工工工工xxx\n");
    param.nEffectFlag = UI::TEXT_EFFECT_HALO;
    param.wParam = 3;
    param.bkcolor.ReplaceRGB(RGB(0,0,0));

    int nTick1 = GetTickCount();
    pRenderTarget->DrawString(pRenderFont, &param);
    int nTick2 = GetTickCount();
    UI::UI_LOG_DEBUG(_T("%d"), nTick2-nTick1);
#endif
}

void  CControlDemoWindow::OnBnClicked(IMessage*  pMsgFrom)
{
    TCHAR szText[128] = _T("");
    _stprintf(szText, _T("clicked: 0x%08x"), pMsgFrom);
    ::MessageBox(GetHWND(), szText, _T("Test"), MB_OK|MB_ICONINFORMATION);
}