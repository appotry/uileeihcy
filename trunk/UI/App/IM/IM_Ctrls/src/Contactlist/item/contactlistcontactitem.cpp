#include "stdafx.h"
#include "contactlistcontactitem.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Control\Inc\Interface\ilabel.h"


#include <shobjidl.h>  // IDragSourceHelper
#include <ShlObj.h>    // CFSTR_DROPDESCRIPTION
#include <ShlGuid.h>

const RECT g_rcDragImgPadding = {10,10,10,10};
//////////////////////////////////////////////////////////////////////////

ContactListContactItemShareData::ContactListContactItemShareData()
{
    m_pTextRender1 = NULL;
    m_pTextRender2 = NULL;
    m_pIContactListContactItemShareData = NULL;
}
ContactListContactItemShareData::~ContactListContactItemShareData()
{
    SAFE_RELEASE(m_pTextRender1);
    SAFE_RELEASE(m_pTextRender2);
}


//////////////////////////////////////////////////////////////////////////

ContactListContactItem::ContactListContactItem()
{
    m_pIContactListContactItem = NULL;
    m_lSort = 0;
    m_pTreeView = NULL;
    m_pShareData = NULL;
    m_bBigFace = true;

    m_pPanelLeft = NULL;
    m_pPanelHigh = NULL;
    m_pPanelLow = NULL;
    m_pImage_Portrait = NULL;
    m_pString_NickName = NULL;
    m_pString_Moodphrase = NULL;
    m_pPanelPlugin = NULL;
    m_pPanelRight = NULL;
    m_pString_Remark = NULL;
    m_ptLastLButtonDown.x = m_ptLastLButtonDown.y = 0;
}


HRESULT  ContactListContactItem::FinalConstruct(UI::IUIApplication* p)
{
    DO_PARENT_PROCESS(IContactListContactItem, UI::IListItemBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pIContactListContactItem->SetItemType(CONTACTLIST_ITEM_TYPE_CONTACT);
    return S_OK;
}

void  ContactListContactItem::OnInitialize()
{
    UI::IListCtrlBase*  pCtrlBase = m_pIContactListContactItem->GetIListCtrlBase();
    m_pTreeView = static_cast<UI::ITreeView*>(pCtrlBase);

    IContactListContactItemShareData* pData = (IContactListContactItemShareData*)m_pTreeView->GetItemTypeShareData(CONTACTLIST_ITEM_TYPE_CONTACT);
    if (!pData)
    {
        UI::IUIApplication*  pUIApp = pCtrlBase->GetUIApplication();
        IContactListContactItemShareData::CreateInstance(pUIApp, &pData);
        m_pShareData = pData->GetImpl();
        m_pTreeView->SetItemTypeShareData(CONTACTLIST_ITEM_TYPE_CONTACT, pData);

        pUIApp->CreateTextRenderBase(UI::TEXTRENDER_TYPE_NORMAL, NULL, &m_pShareData->m_pTextRender1);
        pUIApp->CreateTextRenderBase(UI::TEXTRENDER_TYPE_NORMAL, NULL, &m_pShareData->m_pTextRender2);

        UI::IRenderFont* pFont = m_pTreeView->GetRenderFont();
        if (pFont)
        {
            UI::INormalTextRender*  pNormal = (UI::INormalTextRender*)m_pShareData->m_pTextRender1->QueryInterface(UI::uiiidof(INormalTextRender));
            if (pNormal)
            {
                pNormal->SetRenderFont(pFont);
            }

            pNormal = (UI::INormalTextRender*)m_pShareData->m_pTextRender2->QueryInterface(UI::uiiidof(INormalTextRender));
            if (pNormal)
            {
                pNormal->SetRenderFont(pFont);
                UI::Color c(127,127,127,255);
                pNormal->SetColor(&c);
            }
        }
    }
    else
    {
        m_pShareData = pData->GetImpl();
    }

    // 内部 item 初始化 
    InitInnerCtrl();
}

void  ContactListContactItem::InitInnerCtrl()
{
    m_pIContactListContactItem->CreateRootPanel();
    UI::IPanel*  pRootPanel = m_pIContactListContactItem->GetRootPanel();
    if (!pRootPanel)
        return;

    // 1. Create
    UI::IUIApplication*  pUIApp = m_pTreeView->GetUIApplication();

    UI::IPanel::CreateInstance(pUIApp, &m_pPanelLeft);
    UI::IPanel::CreateInstance(pUIApp, &m_pPanelRight);
    UI::IPanel::CreateInstance(pUIApp, &m_pPanelHigh);
    UI::IPanel::CreateInstance(pUIApp, &m_pPanelLow);

    m_pPanelLeft->SetID(L"panel_left");
    m_pPanelRight->SetID(L"panel_right");
    m_pPanelHigh->SetID(L"panel_high");
    m_pPanelLow->SetID(L"panel_low");

    pRootPanel->AddChild(m_pPanelLeft);
    pRootPanel->AddChild(m_pPanelRight);
    m_pPanelRight->AddChild(m_pPanelHigh);
    m_pPanelRight->AddChild(m_pPanelLow);

    UI::IPictureCtrl::CreateInstance(pUIApp, &m_pImage_Portrait);
    UI::ILabel::CreateInstance(pUIApp, &m_pString_NickName);
    UI::ILabel::CreateInstance(pUIApp, &m_pString_Remark);
    UI::ILabel::CreateInstance(pUIApp, &m_pString_Moodphrase);

    m_pPanelLeft->AddChild(m_pImage_Portrait);
    m_pPanelHigh->AddChild(m_pString_NickName);
    m_pPanelHigh->AddChild(m_pString_Remark);
    m_pPanelLow->AddChild(m_pString_Moodphrase);

    // 2. Layout
    m_pImage_Portrait->SetConfigLeft(0);
    m_pImage_Portrait->SetConfigTop(0);
    m_pImage_Portrait->SetConfigRight(0);
    m_pImage_Portrait->SetConfigBottom(0);

    // 需要根据大小头像进行区分的控件布局
    UpdadteInnerCtrlLayout();

    // 3. Attribute
    UI::ITextRenderBase*  pTextRender = m_pTreeView->GetTextRender();
    if (pTextRender)
    {
        m_pString_NickName->SetTextRender(m_pShareData->m_pTextRender1);
        m_pString_Remark->SetTextRender(m_pShareData->m_pTextRender2);
        m_pString_Moodphrase->SetTextRender(m_pShareData->m_pTextRender2);
    }

    m_pString_Moodphrase->ModifyStyle(0, OBJECT_STYLE_REJEST_MOUSE_MSG_ALL, false); // 接收鼠标消息用于显示提示条

}

void  ContactListContactItem::UpdadteBigFaceLayout()
{
    m_pPanelLeft->SetConfigWidth(40);
    m_pPanelLeft->SetConfigHeight(40);
    m_pPanelLeft->SetConfigLeft(7);
    m_pPanelLeft->SetConfigTop(7);

    m_pPanelRight->SetConfigLeft(50);
    m_pPanelRight->SetConfigRight(0);
    m_pPanelRight->SetConfigTop(0);
    m_pPanelRight->SetConfigBottom(0);
    m_pPanelRight->SetLayoutType(LAYOUT_TYPE_CANVAS);

    m_pPanelHigh->SetConfigLeft(0);
    m_pPanelHigh->SetConfigTop(7);
    m_pPanelHigh->SetConfigHeight(20);
    m_pPanelHigh->SetConfigRight(-1);
    m_pPanelHigh->SetLayoutType(LAYOUT_TYPE_STACK);

    m_pPanelLow->SetConfigLeft(0);
    m_pPanelLow->SetConfigTop(30);
    m_pPanelLow->SetConfigHeight(20);
    m_pPanelLow->SetConfigRight(-1);
    m_pPanelLow->SetLayoutType(LAYOUT_TYPE_STACK);
}
void  ContactListContactItem::UpdadteSmallFaceLayout()
{
    m_pPanelLeft->SetConfigWidth(20);
    m_pPanelLeft->SetConfigHeight(20);
    m_pPanelLeft->SetConfigLeft(5);
    m_pPanelLeft->SetConfigTop(5);
}

void  ContactListContactItem::UpdadteInnerCtrlLayout()
{
    if (m_bBigFace)
    {
        UpdadteBigFaceLayout();
    }
    else
    {
        UpdadteSmallFaceLayout();
    }
}

void  ContactListContactItem::OnPaint(UI::IRenderTarget* pRenderTarget)
{
    CRect  rcItem;
    m_pIContactListContactItem->GetParentRect(&rcItem);

    UINT nState = m_pIContactListContactItem->GetItemRenderState();
    UI::IRenderBase* pForegndRender = m_pTreeView->GetForeRender();
    if (pForegndRender)
    {
        pForegndRender->DrawState(pRenderTarget, &rcItem, nState);
    }
}

void  ContactListContactItem::Update(CONTACTLIST_CONTACTITEM_INFO* pInfo)
{
    if (NULL == pInfo)
        return;

    if (pInfo->nMask & CLCI_MASK_NICKNAME && m_pString_NickName)
    {
        m_pString_NickName->SetText(pInfo->pszNickName);
    }
    if (pInfo->nMask & CLCI_MASK_MOODPHRASE && m_pString_Moodphrase)
    {
        m_pString_Moodphrase->SetText(pInfo->pszMoodphrase);
        m_pString_Moodphrase->AddAttribute(XML_TOOLTIP, pInfo->pszMoodphrase);
    }
}

void  ContactListContactItem::OnMouseMove(UINT nFlags, POINT point)
{
    if (nFlags & MK_LBUTTON)
    {
        if (point.x != m_ptLastLButtonDown.x || 
            point.y != m_ptLastLButtonDown.y)
        {
            DoDrag(point);
        }
    }
    SetMsgHandled(FALSE);
}
void  ContactListContactItem::OnLButtonDown(UINT nFlags, POINT point)
{
    m_ptLastLButtonDown.x = point.x;
    m_ptLastLButtonDown.y = point.y;

    SetMsgHandled(FALSE);  // SELECTED/FOCUS
}

void  ContactListContactItem::DoDrag(POINT point)
{
    IDataObject*  pDataObject = NULL;
    IDropSource*  pDropSource = NULL;
    IDragSourceHelper2*  pDragSourceHelper = NULL;

    UI::CreateDataObjectInstance(&pDataObject);
    UI::CreateDropSourceInstance(&pDropSource);
    ::CoCreateInstance(CLSID_DragDropHelper, NULL, CLSCTX_INPROC, IID_IDragSourceHelper2, (void**)&pDragSourceHelper);

#pragma region // cf_text for drop test
    FORMATETC  format = {0};
    format.dwAspect = DVASPECT_CONTENT;
    format.cfFormat = CF_TEXT;
    format.tymed = TYMED_HGLOBAL;

    char* pTextDesc = "ui_dragtest_contactitem";
    HGLOBAL hGlobal = GlobalAlloc(0, strlen(pTextDesc)+1);
    LPSTR lpstr = (LPSTR)::GlobalLock(hGlobal);
    strcpy(lpstr, pTextDesc);
    ::GlobalUnlock(hGlobal);

    STGMEDIUM medium = {0};
    medium.tymed = TYMED_HGLOBAL;
    medium.hGlobal = hGlobal;
    pDataObject->SetData(&format, &medium, TRUE);
#pragma endregion

    HBITMAP hDragBitmap = NULL;
    if (pDragSourceHelper)
    {
        SHDRAGIMAGE image = {0};
        hDragBitmap = CreateDragBitmap((int*)&image.sizeDragImage.cx, (int*)&image.sizeDragImage.cy);
        image.hbmpDragImage = hDragBitmap;

        CRect rcParent;
        CRect rcWindow;
        m_pIContactListContactItem->GetParentRect(&rcParent);
        m_pIContactListContactItem->GetIListCtrlBase()->ItemRect2WindowRect(&rcParent, &rcWindow);

        image.ptOffset.x = point.x - rcWindow.left + g_rcDragImgPadding.left;
        image.ptOffset.y = point.y - rcWindow.top + g_rcDragImgPadding.top;

        pDragSourceHelper->SetFlags(DSH_ALLOWDROPDESCRIPTIONTEXT);

        HRESULT hr = pDragSourceHelper->InitializeFromBitmap(&image, pDataObject);
        UIASSERT(SUCCEEDED(hr));
    }

    DWORD dwEffect = 0;
    //HRESULT hr = ::DoDragDrop(pDataObject, pDropSource, DROPEFFECT_COPY, &dwEffect);
    HRESULT hr = SHDoDragDrop(NULL, pDataObject, pDropSource, DROPEFFECT_COPY|DROPEFFECT_MOVE, &dwEffect);
    SAFE_RELEASE(pDataObject);
    SAFE_RELEASE(pDropSource);
    SAFE_RELEASE(pDragSourceHelper);
    // SAFE_DELETE_GDIOBJECT(hDragBitmap); -- 自己手动删除会失败。是不是自动被释放了？

    return;
}


void  ContactListContactItem::OnLButtonDblClk(UINT nFlags, POINT point)
{
    // 判断鼠标没有位于按钮类控件上面
    UI::IObject* pCurObj = m_pTreeView->GetPressObject();
    if (!pCurObj)
        pCurObj = m_pTreeView->GetHoverObject();

    if (pCurObj)
    {
        UI::OBJ_TYPE nObjType = pCurObj->GetObjectType();
        UI::CONTROL_TYPE nObjTypeEx = pCurObj->GetObjectExtentType();

        if (nObjType == UI::OBJ_PANEL ||
            nObjType == UI::OBJ_CONTROL && (nObjType != UI::CONTROL_LABEL || nObjType != UI::CONTROL_PICTURE))
        {

        }
        else
        {
            return;
        }
    }

    UI::UIMSG  msg;
    msg.message = UI_WM_NOTIFY;
    msg.pMsgFrom = m_pTreeView;
    msg.nCode = UI_CONTACTLIST_NOTIFY_CONTACTITEM_DBCLICK;
    msg.wParam = (WPARAM)m_pIContactListContactItem;
    m_pTreeView->DoNotify(&msg);
}

void  ContactListContactItem::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = -1;
    pSize->cy = 50;
}

#include "UISDK\Kernel\Inc\Util\iimage.h"
// 注：如果直接将Item画在缓存的(0,0)处，结果拖拽时左侧有若干像素居然显示到右边去了，
//     具体也不知道windows内部是怎么处理的。因此将缓存图片增大g_rcDragImgPadding，
//     这样拖拽时显示的图片就能正常了
HBITMAP  ContactListContactItem::CreateDragBitmap(int* pWidth, int* pHeight)
{
    CRect  rcParent;
    m_pIContactListContactItem->GetParentRect(&rcParent);

    int nHeight = rcParent.Height() + g_rcDragImgPadding.left + g_rcDragImgPadding.right;
    int nWidth = rcParent.Width() + g_rcDragImgPadding.top + g_rcDragImgPadding.bottom;

    // 在Win7无主题的情况下，经N次验证得到这两个数。如果小于它，则拖拽时无法显示图片
    if (nWidth < 295)
        nWidth = 295;
    if (nHeight < 86)
        nHeight = 86;

    // 创建位图
    UI::IImage  image;
    image.Create(nWidth, nHeight, 32, UI::IImage::createAlphaChannel);
    HDC hDC = image.BeginDrawToMyself();
    
    // 创建rendertarget
    HWND hWnd = m_pIContactListContactItem->GetIListCtrlBase()->GetHWND();
    UI::IRenderTarget*  pRenderTarget = UI::UICreateRenderTarget(hWnd);
    pRenderTarget->BindHDC(hDC);

    // 绘制
    pRenderTarget->BeginDraw(NULL, 0);
    pRenderTarget->SetViewportOrgEx(
        -rcParent.left+g_rcDragImgPadding.left, 
        -rcParent.top+g_rcDragImgPadding.top);  // 抵消DrawItemInnerControl中的偏移

    OnPaint(pRenderTarget);
    m_pIContactListContactItem->DrawItemInnerControl(pRenderTarget);
    pRenderTarget->EndDraw();

    SAFE_RELEASE(pRenderTarget);
    image.EndDrawToMyself();

    if (pWidth)
        *pWidth = image.GetWidth();
    if (pHeight)
        *pHeight = image.GetHeight();

//   image.SaveAsPng(L"C:\\aaa.png");
    return image.Detach();
}
