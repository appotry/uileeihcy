#include "stdafx.h"
#include "menuseparatoritem.h"

namespace UI
{
//////////////////////////////////////////////////////////////////////////

MenuSeparatorItemShareData::MenuSeparatorItemShareData()
{
    m_pListCtrl = NULL;
    m_pIMenuSeparatorItemShareData = NULL;

    m_pSeparatorRender = NULL;
    nIconGutterWidth = 0;
    nHeight = 0;
}

MenuSeparatorItemShareData::~MenuSeparatorItemShareData()
{
    SAFE_RELEASE(m_pSeparatorRender);
}

void  MenuSeparatorItemShareData::ResetAttribute()
{
    nHeight = 3;
    SAFE_RELEASE(m_pSeparatorRender);
}
void  MenuSeparatorItemShareData::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    if (!m_pListCtrl)
        return;
    if (!pMapAttrib)
        return;

    const TCHAR* szText = pMapAttrib->GetAttr(XML_MENU_SEPARATOR_RENDEER_PREFIX XML_RENDER_TYPE, true);
    if (szText)
    {
        SAFE_RELEASE(m_pSeparatorRender);
        m_pListCtrl->GetUIApplication()->CreateRenderBaseByName((BSTR)szText, m_pListCtrl, &m_pSeparatorRender);
        if (m_pSeparatorRender)
        {
            m_pSeparatorRender->SetAttribute(pMapAttrib, XML_MENU_SEPARATOR_RENDEER_PREFIX, true);
        }
        else
        {
            UI_LOG_WARN( _T("%s Invalid %s=\"%s\""), FUNC_NAME, XML_MENU_SEPARATOR_RENDEER_PREFIX XML_RENDER_TYPE, szText);
        }
    }

    if (NULL == m_pSeparatorRender)
    {
        m_pListCtrl->GetUIApplication()->CreateRenderBase(RENDER_TYPE_THEME_MENUSEPERATOR, m_pListCtrl, &m_pSeparatorRender);
    }
}

//////////////////////////////////////////////////////////////////////////


MenuSeparatorItem::MenuSeparatorItem()
{
    m_pIMenuSeparatorItem = NULL;
    m_pShareData = NULL;
}

void  MenuSeparatorItem::GetDesiredSize(SIZE* pSize)
{
    pSize->cx = 0;
    pSize->cy = m_pShareData->nHeight;
}

void  MenuSeparatorItem::OnInitialize()
{
    DO_PARENT_PROCESS(IMenuSeparatorItem, IListItemBase);
    m_pIMenuSeparatorItem->SetItemType(UI_LISTITEM_TYPE_MENU_SEPARATOR);

    IListCtrlBase*  pListCtrl = m_pIMenuSeparatorItem->GetIListCtrlBase();
    IMenuSeparatorItemShareData* p = static_cast<IMenuSeparatorItemShareData*>(
        pListCtrl->GetItemTypeShareData(UI_LISTITEM_TYPE_MENU_SEPARATOR));
    if (NULL == p)
    {
        IMenuSeparatorItemShareData::CreateInstance(pListCtrl->GetUIApplication(), &p);
        pListCtrl->SetItemTypeShareData(UI_LISTITEM_TYPE_MENU_SEPARATOR, p);
         
        m_pShareData = p->GetImpl();

        m_pShareData->m_pListCtrl = m_pIMenuSeparatorItem->GetIListCtrlBase();
        m_pShareData->nIconGutterWidth = UISendMessage(pListCtrl, UI_WM_MENU_GETICONGUTTERWIDTH);

        IMapAttribute*  pMapAttrib = NULL;
        pListCtrl->GetMapAttribute(&pMapAttrib);

		SERIALIZEDATA data = {0};
		data.pMapAttrib = pMapAttrib;
		data.nFlag = SERIALIZEFLAG_LOAD;
		UISendMessage(m_pShareData->GetIIMenuSeparatorItemShareData(), UI_WM_SERIALIZE, (WPARAM)&data);
//        UISendMessage(m_pShareData->GetIIMenuSeparatorItemShareData(), UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, 0);
        SAFE_RELEASE(pMapAttrib);
    }
    else
    {
        m_pShareData = p->GetImpl();
    }
}

void  MenuSeparatorItem::OnPaint(IRenderTarget* pRenderTarget)
{
    IListCtrlBase*  pListCtrl = m_pIMenuSeparatorItem->GetIListCtrlBase();

    CRect  rcParent;
    m_pIMenuSeparatorItem->GetParentRect(&rcParent);

    if (m_pShareData->m_pSeparatorRender)
    {
        rcParent.left += m_pShareData->nIconGutterWidth;
        m_pShareData->m_pSeparatorRender->DrawState(pRenderTarget, &rcParent, 0);
    }
}

}