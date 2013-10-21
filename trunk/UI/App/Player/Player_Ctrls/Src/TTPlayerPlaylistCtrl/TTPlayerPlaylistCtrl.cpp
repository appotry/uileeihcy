#include "stdafx.h"
#include "TTPlayerPlaylistCtrl.h"
#include "UISDK\Control\Inc\Interface\iscrollbar.h"

TTPlayerPlaylistItem::TTPlayerPlaylistItem()
{
    m_pItemInfo = NULL;
    m_pITTPlayerPlaylistItem = NULL;
}

void  TTPlayerPlaylistItem::OnPaint(UI::IRenderTarget* pRenderTarget)
{
    
}

void  TTPlayerPlaylistItem::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = 0;
    pSize->cy = 20;
}

//////////////////////////////////////////////////////////////////////////
TTPlayerPlaylistCtrl::TTPlayerPlaylistCtrl()
{
	m_pPlayingItem = NULL;
	m_pBkColor1 = NULL;
	m_pBkColor2 = NULL;
	m_pIndexTextColor = NULL;
	m_pIndexTextColorSelect = NULL;
	m_pIndexTextColorPlaying = NULL;
	m_pTextColor = NULL;
	m_pTextColorSelect = NULL;
	m_pTextColorPlaying = NULL;
	m_pTimeTextColor = NULL;
	m_pTimeTextColorSelect = NULL;
	m_pTimeTextColorPlaying = NULL;
	m_pSelectBorderColor = NULL;
	m_pSelectColor1 = NULL;
	m_pSelectColor2 = NULL;

    m_pITTPlayerPlaylistCtrl = NULL;
	//	this->ModifyStyle(0, LISTCTRLBASE_SIZE_2_CONTENT|LISTCTRLBASE_CONTENT_2_SIZE);
}

TTPlayerPlaylistCtrl::~TTPlayerPlaylistCtrl()
{
	SAFE_RELEASE(m_pBkColor1);
	SAFE_RELEASE(m_pBkColor2);
	SAFE_RELEASE(m_pIndexTextColor);
	SAFE_RELEASE(m_pIndexTextColorSelect);
	SAFE_RELEASE(m_pIndexTextColorPlaying);
	SAFE_RELEASE(m_pTextColor);
	SAFE_RELEASE(m_pTextColorSelect);
	SAFE_RELEASE(m_pTextColorPlaying);
	SAFE_RELEASE(m_pTimeTextColor);
	SAFE_RELEASE(m_pTimeTextColorSelect);
	SAFE_RELEASE(m_pTimeTextColorPlaying);
	SAFE_RELEASE(m_pSelectBorderColor);
	SAFE_RELEASE(m_pSelectColor1);
	SAFE_RELEASE(m_pSelectColor2);
}

void TTPlayerPlaylistCtrl::ResetAttribute()
{
    DO_PARENT_PROCESS(ITTPlayerPlaylistCtrl, UI::IListCtrlBase);

	SAFE_RELEASE(m_pBkColor1);
	SAFE_RELEASE(m_pBkColor2);
	SAFE_RELEASE(m_pIndexTextColor);
	SAFE_RELEASE(m_pIndexTextColorSelect);
	SAFE_RELEASE(m_pIndexTextColorPlaying);
	SAFE_RELEASE(m_pTextColor);
	SAFE_RELEASE(m_pTextColorSelect);
	SAFE_RELEASE(m_pTextColorPlaying);
	SAFE_RELEASE(m_pTimeTextColor);
	SAFE_RELEASE(m_pTimeTextColorSelect);
	SAFE_RELEASE(m_pTimeTextColorPlaying);
	SAFE_RELEASE(m_pSelectBorderColor);
	SAFE_RELEASE(m_pSelectColor1);
	SAFE_RELEASE(m_pSelectColor2);
}

void  TTPlayerPlaylistCtrl::SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload)
{
    DO_PARENT_PROCESS(ITTPlayerPlaylistCtrl, UI::IListBox);

#define GET_COLOR(xml, p) \
	szText = pMapAttrib->GetAttr(xml, true); \
	if (szText) \
	{ \
    	pColorRes->GetColor((BSTR)szText, &p); \
	}

	UI::IColorRes* pColorRes = m_pITTPlayerPlaylistCtrl->GetUIApplication()->GetActiveSkinColorRes();
	if (NULL == pColorRes)
		return;

    const TCHAR* szText = NULL;

	GET_COLOR(XML_TTPLAYER_PLAYLIST_BKCOLOR1, m_pBkColor1);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_BKCOLOR2, m_pBkColor2);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_INDEX_TEXT_COLOR, m_pIndexTextColor);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_INDEX_TEXT_COLOR_SELECT, m_pIndexTextColorSelect);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_INDEX_TEXT_COLOR_PLAYING, m_pIndexTextColorPlaying);

	GET_COLOR(XML_TTPLAYER_PLAYLIST_TEXT_COLOR, m_pTextColor);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_TEXT_COLOR_SELECT, m_pTextColorSelect);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_TEXT_COLOR_PLAYING, m_pTextColorPlaying);

	GET_COLOR(XML_TTPLAYER_PLAYLIST_TIME_COLOR, m_pTimeTextColor);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_TIME_COLOR_SELECT, m_pTimeTextColorSelect);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_TIME_COLOR_PLAYING, m_pTimeTextColorPlaying);

	GET_COLOR(XML_TTPLAYER_PLAYLIST_SELECT_BORDER_COLOR, m_pSelectBorderColor);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_SELECT_COLOR1, m_pSelectColor1);
	GET_COLOR(XML_TTPLAYER_PLAYLIST_SELECT_COLOR2, m_pSelectColor2);

}

void TTPlayerPlaylistCtrl::SetPlayingItem(TTPlayerPlaylistItem* pCtrlItem)
{
	bool bNeedUpdate1 = false;
    bool bNeedUpdate2 = false;
	if (m_pPlayingItem != pCtrlItem)
	{
		bNeedUpdate1 = true;
	}

	m_pPlayingItem = pCtrlItem;
    if (pCtrlItem)
    {
	    m_pITTPlayerPlaylistCtrl->MakeItemVisible(pCtrlItem->GetITTPlayerPlaylistItem(), &bNeedUpdate2);
    }

    if (bNeedUpdate1 || bNeedUpdate2)
    {
        m_pITTPlayerPlaylistCtrl->UpdateObject();
    }
}

LRESULT TTPlayerPlaylistCtrl::OnDrawItem(WPARAM wParam, LPARAM lParam)
{
    UI::IRenderTarget*  pRenderTarget = (UI::IRenderTarget*)wParam;
    ITTPlayerPlaylistItem*  p = (ITTPlayerPlaylistItem*)lParam;

	if (NULL == pRenderTarget || NULL == p)
		return 0;

	TTPlayerPlaylistItem* pItem = p->GetImpl();

	CRect rcItem;
	p->GetParentRect(&rcItem);

	if (0 == p->GetLineIndex()%2)
	{
		if (m_pBkColor1)
			pRenderTarget->FillRect(&rcItem, m_pBkColor1);
	}
	else
	{
		if (m_pBkColor2)
        {
            pRenderTarget->FillRect(&rcItem, m_pBkColor2);
        }
        else
        {
            UI::Color c(32,32,32,255);
            pRenderTarget->FillRect(&rcItem, &c);
        }
	}

	COLORREF rgbText = m_pTextColor? m_pTextColor->m_col:RGB(0,128,255);

	// 	if (p->IsDisable())
	// 	{
	// 
	// 	}
	// 	else
	if (p->IsSelected())
	{
        UI::Color colBorder(255,255,255,255);
        if (m_pSelectBorderColor)
            colBorder = *m_pSelectBorderColor;

		COLORREF colSelect1 = m_pSelectColor1?m_pSelectColor1->m_col:RGB(47,100,190);
		COLORREF colSelect2 = m_pSelectColor2?m_pSelectColor2->m_col:RGB(4,10,19);

		pRenderTarget->GradientFillV(&rcItem, colSelect1, colSelect2);
		pRenderTarget->Rectangle(&rcItem, &colBorder, NULL, 1, true);

		rgbText = m_pTextColorSelect? m_pTextColorSelect->m_col:RGB(255,255,255);
	}
	else if (m_pPlayingItem == pItem)  // 正在播放的歌曲
	{
		rgbText = m_pTextColorPlaying? m_pTextColorPlaying->m_col : RGB(0,255,0);
	}
	else if (p->IsPress())
	{
	}
	else if (NULL == m_pITTPlayerPlaylistCtrl->GetPressItem() && p->IsHover())
	{
	}

	if (pItem && NULL != pItem->m_pItemInfo)
	{
		CRect rcNum = rcItem;
		CRect rcTime = rcItem;
		CRect rcText = rcItem;

        if (m_pITTPlayerPlaylistCtrl->GetItemCount() > 99)  // 三位数
		    rcText.left = rcNum.right = 28;
        else
            rcText.left = rcNum.right = 20;

		rcText.right = rcTime.left = rcItem.right - 25;
		rcTime.right--;

        UI::IRenderFont*  pRenderFont = m_pITTPlayerPlaylistCtrl->GetRenderFont();

		TCHAR szNum[16] = _T("");
		_stprintf(szNum, _T("%d."), p->GetLineIndex()+1);

        UI::DRAWTEXTPARAM  param1;
        param1.szText = szNum;
        param1.prc = &rcNum;
        param1.color.ReplaceRGB(rgbText);
        param1.nFormatFlag = DT_SINGLELINE|DT_RIGHT|DT_VCENTER;
		pRenderTarget->DrawString(pRenderFont, &param1);

        UI::DRAWTEXTPARAM  param2;
        param2.szText = pItem->m_pItemInfo->GetDisplayText();
        param2.prc = &rcText;
        param2.color.ReplaceRGB(rgbText);
        param2.nFormatFlag = DT_SINGLELINE|DT_END_ELLIPSIS|DT_LEFT|DT_VCENTER;
		pRenderTarget->DrawString(pRenderFont, &param2);

        UI::DRAWTEXTPARAM  param3;
        param2.szText = pItem->m_pItemInfo->GetFileTime();
        param2.prc = &rcTime;
        param2.color.ReplaceRGB(rgbText);
        param2.nFormatFlag = DT_SINGLELINE|DT_RIGHT|DT_VCENTER;
		pRenderTarget->DrawString(pRenderFont, &param3);
	}

    return 0;
}

ITTPlayerPlaylistItem* TTPlayerPlaylistCtrl::AddFileItem(IPlayerListItemInfo* pItemInfo, bool bUpdate)
{
	//	int nPos = strFilePath.rfind(_T('\\'), strFilePath.length());

    ITTPlayerPlaylistItem* pItem = NULL;
    ITTPlayerPlaylistItem::CreateInstance(m_pITTPlayerPlaylistCtrl->GetUIApplication(), &pItem);

	pItem->GetImpl()->m_pItemInfo = pItemInfo;
	m_pITTPlayerPlaylistCtrl->AddItem(pItem, bUpdate?0:LISTITEM_OPFLAG_NOALL);

    return pItem;
}
void TTPlayerPlaylistCtrl::RemoveFileItem(IPlayerListItemInfo* pItemInfo, bool bUpdate)
{
    UI::IListItemBase* pItem = m_pITTPlayerPlaylistCtrl->GetFirstItem();
	while (pItem)
	{
		if (((ITTPlayerPlaylistItem*)pItem)->GetImpl()->m_pItemInfo == pItemInfo)
		{
			m_pITTPlayerPlaylistCtrl->RemoveItem(pItem);
			if (bUpdate)
				m_pITTPlayerPlaylistCtrl->UpdateObject();
			
			break;
		}
		pItem = pItem->GetNextItem();
	}
}
