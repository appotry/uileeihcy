#include "stdafx.h"
#include "qqloginaccountlistbox.h"
#include "qqloginaccountlistboxlayout.h"
#include "UISDK\Control\Inc\Interface\ibutton.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"

const UINT  QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_NORMAL  = RENDER_STATE_NORMAL|0;
const UINT  QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_HOVER   = RENDER_STATE_HOVER|1;
const UINT  QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_PRESS   = RENDER_STATE_PRESS|2;
const UINT  QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_DISABLE = RENDER_STATE_DISABLE|3;
const UINT  QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_SELECTED_NORMAL = RENDER_STATE_NORMAL|RENDER_STATE_SELECTED|4;
const UINT  QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_SELECTED_HOVER  = RENDER_STATE_HOVER|RENDER_STATE_SELECTED|5;
const UINT  QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_SELECTED_PRESS  = RENDER_STATE_PRESS|RENDER_STATE_SELECTED|6;
const UINT  QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_SELECTED_DISABLE = RENDER_STATE_DISABLE|RENDER_STATE_SELECTED|7;
const UINT  QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_SELECTED_NOT_FOCUS = RENDER_STATE_SELECTED|RENDER_STATE_NOTFOCUS|8;

QQLoginAccountListBox::QQLoginAccountListBox()
{
    m_pIQQLoginAccountListBox = NULL;
	m_pNameTextRender = NULL;
	m_pEraseBtn = NULL;
    m_pBindObj = NULL;
}
QQLoginAccountListBox::~QQLoginAccountListBox()
{
	SAFE_RELEASE(m_pNameTextRender);
	SAFE_DELETE_Ixxx(m_pEraseBtn);
}

HRESULT  QQLoginAccountListBox::FinalConstruct(UI::IUIApplication* p)
{
    DO_PARENT_PROCESS(IQQLoginAccountListBox, UI::IListCtrlBase);
    if (FAILED(GetCurMsg()->lRet))
        return GetCurMsg()->lRet;

    m_pIQQLoginAccountListBox->SetLayout(new QQLoginAccountListBoxLayout());
    return S_OK;
}

void  QQLoginAccountListBox::ResetAttribute()
{
    DO_PARENT_PROCESS(IQQLoginAccountListBox, UI::IListCtrlBase);

	SAFE_RELEASE(m_pNameTextRender);
}
void  QQLoginAccountListBox::SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload)
{
	// 设置listbox默认字体
    UI::IUIApplication*  pUIApp = m_pIQQLoginAccountListBox->GetUIApplication();
    const TCHAR* szText = pMapAttrib->GetAttr(XML_TEXTRENDER_TYPE, false);
	if (NULL == szText && NULL == m_pIQQLoginAccountListBox->GetTextRender())
	{
        UI::IRenderFont* pRenderFont = m_pIQQLoginAccountListBox->GetRenderFont();
		UI::ITextRenderBase* pTextRender = NULL;
        pUIApp->CreateTextRenderBase(UI::TEXTRENDER_TYPE_COLORLIST, m_pIQQLoginAccountListBox, &pTextRender);
		if (pTextRender)
		{
            UI::IColorListTextRender* p = (UI::IColorListTextRender*)pTextRender->QueryInterface(UI::uiiidof(IColorListTextRender));
            if (p)
            {
			    p->SetCount(8);
			    p->SetColor(0, RGBA(0,0,0,255));
			    p->SetColor(1, RGBA(255,255,255,255));
			    p->SetColor(2, RGBA(255,255,255,255));
			    p->SetColor(3, RGBA(255,255,255,255));

			    p->SetColor(4, RGBA(255,255,255,255));
			    p->SetColor(5, RGBA(255,255,255,255));
			    p->SetColor(6, RGBA(255,255,255,255));
			    p->SetColor(7, RGBA(255,255,255,255));
			    p->SetRenderFont(pRenderFont);

			    p->SetTextAlignment(DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS|DT_NOPREFIX);
            }

			pTextRender->SetAttribute(pMapAttrib, XML_COMBOBOX_LISTBOX_PRIFIX, true);
			m_pIQQLoginAccountListBox->SetTextRender(pTextRender);

            SAFE_RELEASE(pTextRender);
		}
	}

    DO_PARENT_PROCESS(IQQLoginAccountListBox, UI::IListCtrlBase);

	szText = pMapAttrib->GetAttr(XML_QQLoginAccountList_NAMETEXT_RENDER_PREFIX XML_TEXTRENDER_TYPE, true);
	if (szText)
	{
        pUIApp->CreateTextRenderBaseByName(szText, m_pIQQLoginAccountListBox, &m_pNameTextRender);
		if (m_pNameTextRender)
		{
			m_pNameTextRender->SetAttribute(pMapAttrib, XML_QQLoginAccountList_NAMETEXT_RENDER_PREFIX, true);
			m_pNameTextRender->SetTextAlignment(DT_SINGLELINE|DT_VCENTER|DT_LEFT|DT_END_ELLIPSIS|DT_NOPREFIX);
		}
	}

	if (NULL == m_pEraseBtn)
	{
        UI::IButton::CreateInstance(pUIApp, &m_pEraseBtn);
		m_pEraseBtn->SetOutRef((void**)&m_pEraseBtn);  // 这样就能放心的在析构函数中去释放m_pEraseBtn

		//pNewItem->m_pPanelRoot->AddChild(pEraseBtn);

// 		m_pEraseBtn->SetConfigWidth(16);
// 		m_pEraseBtn->SetConfigHeight(16);
		m_pEraseBtn->SetConfigRight(2);
		m_pEraseBtn->SetNotify(m_pIQQLoginAccountListBox, 1);
        m_pEraseBtn->SetDrawFocusType(UI::BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE);
		m_pEraseBtn->SetConfigLayoutFlags(UI::LAYOUT_ITEM_ALIGN_CENTER);

        m_pEraseBtn->SetAttributeByPrefix(XML_QQLoginAccountList_ERASEBTN_PREFIX, pMapAttrib, bReload, true);
	}
}

LRESULT QQLoginAccountListBox::OnDrawItem(WPARAM w, LPARAM l)
{
    UI::IRenderTarget* pRenderTarget = (UI::IRenderTarget*)w;
	IQQLoginAccountListItem* p = (IQQLoginAccountListItem*)l;;

	CRect rcItem;
	p->GetParentRect(&rcItem);
    bool  bHilight = false;  // 高亮时才显示姓名结点

    UI::IRenderBase*  pForegndRender = m_pIQQLoginAccountListBox->GetForeRender();
	UINT nRenderState = 0;
	if (p->IsHover())
	{
        bHilight = true;
		nRenderState = QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_HOVER;

		if (m_pIQQLoginAccountListBox->GetForeRender())
		{
			m_pIQQLoginAccountListBox->GetForeRender()->DrawState(pRenderTarget, &rcItem, 0);
		}
	}
	else if(NULL == m_pIQQLoginAccountListBox->GetHoverItem() && p->IsSelected())
	{
        bHilight = true;
		nRenderState = QQACCOUNTLISTBOXITEM_FOREGND_RENDER_STATE_SELECTED_HOVER;

		if (pForegndRender)
			pForegndRender->DrawState(pRenderTarget, &rcItem, 0);
	}

	int nHeight = rcItem.Height();
	int nImgSize = nHeight-4;
    CRect rcFace;

    UI::ITextRenderBase*  pTextRender = m_pIQQLoginAccountListBox->GetTextRender();
    if (pTextRender)
    {
	    rcFace.SetRect(rcItem.left+1, rcItem.top+2, rcItem.left+1+nImgSize, rcItem.bottom-2);
	    CRect rcText (rcItem);
	    rcText.left = rcFace.right + 6;
	    rcText.top += 2;
	    rcText.bottom -= 2;
    	
	    if (bHilight)//p->GetHeight() > ITEM_NEAR_HEIGHT)
	    {
		    CRect rcTextName (rcText);
		    rcTextName.bottom = rcText.top + rcTextName.Height()/2;

		    if (m_pNameTextRender)
			    m_pNameTextRender->DrawState(pRenderTarget, &rcTextName, nRenderState, p->GetName());

		    rcText.top = rcTextName.bottom;
		    pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, p->GetAccount());
	    }
	    else
	    {
		    pTextRender->DrawState(pRenderTarget, &rcText, nRenderState, p->GetAccount());
	    }
    }

    UI::IRenderBitmap*  pBitmap = p->GetFaceBitmap();
	if (pBitmap)
	{
        UI::DRAWBITMAPPARAM param;
		param.xDest = rcFace.left;
		param.yDest = rcFace.top;
		param.wDest = rcFace.Width();
		param.hDest = rcFace.Height();
		param.wSrc  = pBitmap->GetWidth();
		param.hSrc  = pBitmap->GetHeight();
        param.nFlag = UI::DRAW_BITMAP_STRETCH_ROUNDCORNER;

		pRenderTarget->DrawBitmap(pBitmap, &param);
	}

    return 0;
}

IQQLoginAccountListItem* QQLoginAccountListBox::AddAccount(const TCHAR* szId, const TCHAR* szName, const TCHAR* szFaceImgPath)
{
    IQQLoginAccountListItem* pItem = NULL;
    IQQLoginAccountListItem::CreateInstance(m_pIQQLoginAccountListBox->GetUIApplication(), &pItem);

    if (NULL == pItem)
        return NULL;
    
    m_pIQQLoginAccountListBox->AddItem(pItem, 0);
    pItem->SetAccount(szId);
    pItem->SetName(szName);
    pItem->SetFaceImgFilePath(szFaceImgPath);

    pItem->CreateRootPanel();

    return pItem;

}

void AddInvalidItem(IQQLoginAccountListItem** pInvalidArray, int nMaxSize, IQQLoginAccountListItem* p)
{
	if (NULL == p)
		return;

	for (int i = 0; i < nMaxSize; i++)
	{
		if (pInvalidArray[i] == p)
			return;
		if (pInvalidArray[i] == NULL)
		{
			pInvalidArray[i] = p;
			return;
		}
	}
}

LRESULT  QQLoginAccountListBox::OnItemChanged(WPARAM w, LPARAM l)
{
    HandleItemChanged(
        (IQQLoginAccountListItem*)w, 
        (IQQLoginAccountListItem*)l);

    return 0;
}

void QQLoginAccountListBox::HandleItemChanged(
        IQQLoginAccountListItem* pOldItem, IQQLoginAccountListItem* pNewItem, bool bAnimate)
{
	if (NULL == pOldItem && NULL == pNewItem)
		return;

    UI::UI_LOG_DEBUG(_T("%s old: %s,  new: %s"), FUNC_NAME, pOldItem?pOldItem->GetAccount():_T("null"), pNewItem?pNewItem->GetAccount():_T("null"));

	if (NULL == pNewItem)  
	{
		// 当鼠标移到P上面（P不是当前选中项）时，再直接关闭下拉框。然后再次打开下拉框，
		// 这时我们应该是期望看到选中项是被hover的，并且显示为最大高度

		pNewItem = (IQQLoginAccountListItem*)m_pIQQLoginAccountListBox->GetFirstSelectItem();
	}
	if (NULL == pOldItem)
	{
		// 当鼠标刚移进列表框时，当前高亮项是当前被选中项
		pOldItem = (IQQLoginAccountListItem*)m_pIQQLoginAccountListBox->GetFirstSelectItem();
	}

	if (pNewItem)
	{
        UI::IPanel* pPanel = pNewItem->GetRootPanel();
        UI::IObject* pOldParent = m_pEraseBtn->GetParentObject();
		if (pOldParent != pPanel)
		{
			if (pOldParent)
				pOldParent->RemoveChildInTree(m_pEraseBtn);
			pPanel->AddChild(m_pEraseBtn);
		}
	}

    UI::IAnimateManager* pAnimateMgr = m_pIQQLoginAccountListBox->GetUIApplication()->GetAnimateMgr();
	IQQLoginAccountListItem* pInvalidArray[6] = {NULL};
	if (pOldItem)
	{
		AddInvalidItem(pInvalidArray, 6, (IQQLoginAccountListItem*)pOldItem->GetPrevItem());
		AddInvalidItem(pInvalidArray, 6, pOldItem);
		AddInvalidItem(pInvalidArray, 6, (IQQLoginAccountListItem*)pOldItem->GetNextItem());
	}
	if (pNewItem)
	{
		AddInvalidItem(pInvalidArray, 6, (IQQLoginAccountListItem*)pNewItem->GetPrevItem());
		AddInvalidItem(pInvalidArray, 6, pNewItem);
		AddInvalidItem(pInvalidArray, 6, (IQQLoginAccountListItem*)pNewItem->GetNextItem());
	}

	for (int i = 0; i < 6; i++)
	{
		IQQLoginAccountListItem* pItem = pInvalidArray[i];
		if (NULL == pItem)
			break;

		int nItemLastHeight = ITEM_NORMAL_HEIGHT;
		if (NULL == pNewItem)
		{

		}
		else if (pItem == pNewItem)
		{
			nItemLastHeight = ITEM_HILIGHT_HEIGHT;
		}
		else if (pNewItem->GetPrevItem() == pItem || pNewItem->GetNextItem() == pItem)
		{
			nItemLastHeight = ITEM_NEAR_HEIGHT;
		}

// 		if (pItem->m_nCurItemHeight == nItemLastHeight)  // 不能添加该判断，因为有可能该item还存在动画，需要再添加一次timeline 
// 			continue;

		//if (bAnimate)
		{
            UI::IStoryboard*  pStoryboard = pAnimateMgr->CreateStoryboard();

            UI::IIntLinearMove* pMoveAlgo = NULL;
            UI::IIntTimeline* pTimeline = (UI::IIntTimeline*)pStoryboard->CreateTimeline(UI::TLV_INT, 0, UI::ITMA_Linear, (UI::IMoveAlgorithm**)&pMoveAlgo);
            pMoveAlgo->SetParam1(pItem->GetHeight(), nItemLastHeight, 150);
            int* pnHeight = NULL;
            pItem->GetHeightRef(&pnHeight);
            pTimeline->SetOutRef(pnHeight);

            pStoryboard->SetWParam((WPARAM)pItem);
            pStoryboard->SetNotifyObj(static_cast<UI::IMessage*>(m_pIQQLoginAccountListBox));
            pStoryboard->SetId((int)_ttoi(pItem->GetAccount()));
            pStoryboard->Begin();
		}
	}
}

void QQLoginAccountListBox::OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
{
	m_pIQQLoginAccountListBox->UpdateItemRect();
}


void  QQLoginAccountListBox::OnInitPopupWindow()
{
    if (m_pBindObj)
        m_pIQQLoginAccountListBox->SetMinWidth(m_pBindObj->GetWidth());  // 限制宽度最小与COMBOBOX一致

    if (NDEF == m_pIQQLoginAccountListBox->GetMaxHeight())
        m_pIQQLoginAccountListBox->SetMaxHeight(300);

    UI::IAnimateManager* pAnimateMgr = m_pIQQLoginAccountListBox->GetUIApplication()->GetAnimateMgr();
    pAnimateMgr->SetFps(60);   // 提升列表动画的流畅性
}

void  QQLoginAccountListBox::OnUnInitPopupControlWindow()
{
    UI::IAnimateManager* pAnimateMgr = m_pIQQLoginAccountListBox->GetUIApplication()->GetAnimateMgr();
    pAnimateMgr->SetFps(30);  // 还原
}

void  QQLoginAccountListBox::OnEraseBtnClick()
{
    UI::IAnimateManager* pAnimateMgr = m_pIQQLoginAccountListBox->GetUIApplication()->GetAnimateMgr();
    pAnimateMgr->ClearStoryboardOfNotify(m_pIQQLoginAccountListBox);

    m_pEraseBtn->RemoveMeInTheTree();
    m_pIQQLoginAccountListBox->DelayRemoveItem(m_pIQQLoginAccountListBox->GetHoverItem(), LISTITEM_OPFLAG_NOALL);
}

LRESULT  QQLoginAccountListBox::OnItemRemoved(WPARAM, LPARAM)
{
    if (m_pIQQLoginAccountListBox->TestStyleEx(LISTCTRLBASE_STYLE_SELECT_AS_HOVER_MODE))
    {
        SIZE s = m_pIQQLoginAccountListBox->GetDesiredSize();
        ::SetWindowPos(m_pIQQLoginAccountListBox->GetHWND(), 0, 0, 0, s.cx, s.cy, SWP_NOZORDER|SWP_NOMOVE);
    }

    // 如果删除的不是当前selection item（最后一个ITEM)，并且鼠标有可能位于列表框外面了，则需要重新让selection item变成最高项
    UI::IListItemBase*  p = m_pIQQLoginAccountListBox->GetHoverItem();
    if (p)
    {
        HandleItemChanged(NULL, (IQQLoginAccountListItem*)p);
    }
    else
    {
        HandleItemChanged(NULL, (IQQLoginAccountListItem*)(m_pIQQLoginAccountListBox->GetFirstSelectItem()));
    }

//     HWND hWnd = m_pIQQLoginAccountListBox->GetHWND();
//     POINT pt = {0};
//     ::GetCursorPos(&pt);
//     ::ScreenToClient(hWnd, &pt);
//     ::SendMessage(hWnd, WM_MOUSEMOVE, 0, MAKELPARAM(pt.x, pt.y));
    return 0;
}