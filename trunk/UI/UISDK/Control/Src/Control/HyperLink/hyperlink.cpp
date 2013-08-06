#include "stdafx.h"
#include "UISDK\Control\Src\Control\HyperLink\hyperlink.h"

namespace UI
{

HyperLink::HyperLink()
{
    m_pIHyperLink = NULL;
}

HRESULT  HyperLink::FinalConstruct(IUIApplication* p)
{
    HRESULT hr = __super::FinalConstruct(p);
    if (FAILED(hr))
        return hr;

    m_pIHyperLink->SetStyleEx(MAKECONTROLSUBTYPE(m_pIHyperLink->GetStyleEx(), BUTTON_STYLE_HYPERLINK));
    return S_OK;
}

void HyperLink::SetAttribute(IMapAttribute* pMapAttrib, bool bReload )
{
    IUIApplication*  pUIApplication = m_pIHyperLink->GetUIApplication();

	IFontRes* pFontRes = pUIApplication->GetActiveSkinFontRes();
	if (NULL == pFontRes)
		return;

    const TCHAR* szText = pMapAttrib->GetAttr(XML_TEXTRENDER_TYPE, false);
	if (NULL == szText && NULL == m_pIHyperLink->GetTextRender())
	{
		IRenderFont* pFont = m_pIHyperLink->GetRenderFont();

        ITextRenderBase*  pTextRenderBase = NULL;
        pUIApplication->CreateTextRenderBase(TEXTRENDER_TYPE_FONTCOLORLIST, m_pIHyperLink, &pTextRenderBase);
		if (pTextRenderBase)
		{
            pTextRenderBase->SetAttribute(pMapAttrib, NULL, true);
			IFontColorListTextRender* p = (IFontColorListTextRender*)pTextRenderBase->QueryInterface(uiiidof(IFontColorListTextRender));
            if (p)
            {
			    p->SetCount(4);
			    p->SetColor(0, RGBA(0,0,255, 255));
			    p->SetColor(1, RGBA(255,0,0, 255));
			    p->SetColor(2, RGBA(255,0,0, 255));
			    p->SetColor(3, RGBA(192,192,192, 255));

			    IRenderFont* pUnderLineFont = NULL;
			    pFontRes->GetFontEx(pFont, FONTITEM_FLAG_UNDERLINE, 0, GetRenderLibraryType(m_pIHyperLink->GetHWND()), &pUnderLineFont);

			    p->SetFont(0, pFont);
			    p->SetFont(1, pUnderLineFont);
			    p->SetFont(2, pUnderLineFont);
			    p->SetFont(3, pFont);
			    SAFE_RELEASE(pUnderLineFont);
            }

            m_pIHyperLink->SetTextRender(pTextRenderBase);
            SAFE_RELEASE(pTextRenderBase);
		}
	}

	__super::SetAttribute(pMapAttrib, bReload);

    if (!bReload)
    {
        szText = pMapAttrib->GetAttr(XML_HYPERLINK_HREF, true);
        if (szText)
            m_strHref = szText;
    }
}


void  HyperLink::ResetAttribute()
{
    __super::ResetAttribute();

    m_pIHyperLink->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, false);   // Hyperlink默认是透明的

    CRegion4 r(0,0,0,0);
    m_pIHyperLink->SetPaddingRegion(&r);
}

BOOL HyperLink::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
	::SetCursor(::LoadCursor(NULL, IDC_HAND));
	return TRUE;
}
void HyperLink::OnClicked()
{
	UIMSG   msg;
	msg.message = UI_WM_NOTIFY;
	msg.nCode    = UI_BN_CLICKED;
	msg.wParam  = 0;
	msg.lParam  = NULL;
	msg.pMsgFrom = m_pIHyperLink;
	LRESULT lRet = m_pIHyperLink->DoNotify(&msg);
	if (0 != lRet) // 外部已经处理了
		return;

	if (m_strHref.length() > 0)
	{
		ShellExecute(NULL,_T("open"), m_strHref.c_str(), NULL, NULL, SW_SHOW );
	}
}

}