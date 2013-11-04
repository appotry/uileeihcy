#include "stdafx.h"
#include "panel.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"
#include "UISDK\Kernel\Inc\Interface\imapattr.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"

namespace UI
{

Panel::Panel()
{
    m_pIPanel = NULL;
	this->m_pLayout = NULL;

    m_pMaterialRender = NULL;
    m_rcMaterialRenderRegion.SetRectEmpty();

//     m_pMaskRender = NULL;
//     m_rcMaskRenderRegion.SetRectEmpty();

	SetRectEmpty(&m_rcBkgndRenderRegion);
	SetRectEmpty(&m_rcForegndRenderRegion);
}
Panel::~Panel()
{
	SAFE_RELEASE(m_pLayout);
    SAFE_RELEASE(m_pMaterialRender);
}

ILayout* Panel::GetLayout()
{
	return this->m_pLayout; 
}

void  Panel::SetLayoutType(LAYOUTTYPE eLayoutType)
{
    SAFE_RELEASE(m_pLayout);
 
    IUIApplication*  pUIApp = GetUIApplication();
    if (pUIApp)
        pUIApp->CreateLayout(eLayoutType, m_pIPanel, &m_pLayout);
}

LRESULT Panel::OnGetLayoutPtr(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return (LRESULT)m_pLayout;
}

void  Panel::OnSize(UINT nType, int cx, int cy)
{
    SetMsgHandled(FALSE);
    if (this->GetObjectType() == OBJ_WINDOW)  // 窗口的大小改变由_OnSize处理过了
        return;

    if (m_pLayout)
        m_pLayout->Arrange(NULL);
}

void  Panel::OnGetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;

    if (this->m_pLayout)
        *pSize= this->m_pLayout->Measure();

    pSize->cx += GetNonClientW();
    pSize->cy += GetNonClientH();
}

void  Panel::ResetAttribute()
{
	__super::ResetAttribute();
	SAFE_RELEASE(m_pLayout);

	this->ModifyStyle(OBJECT_STYLE_TRANSPARENT|OBJECT_STYLE_REJECT_MOUSE_MSG_SELF, 0, 0);
	SetRectEmpty(&m_rcBkgndRenderRegion);
	SetRectEmpty(&m_rcForegndRenderRegion);

    SAFE_RELEASE(m_pMaterialRender);
    m_rcMaterialRenderRegion.SetRectEmpty();
//     SAFE_RELEASE(m_pMaskRender);
//     m_rcMaskRenderRegion.SetRectEmpty();
}
void  Panel::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	__super::SetAttribute(pMapAttrib, bReload);

	SAFE_RELEASE(m_pLayout);
    const TCHAR* szText = pMapAttrib->GetAttr(XML_LAYOUT, true);
    GetUIApplication()->CreateLayoutByName((BSTR)szText, m_pIPanel, true, &m_pLayout);

    if (m_pLayout)
        m_pLayout->SetAttribute(pMapAttrib);

    // 加载皮肤属性
    SAFE_RELEASE(m_pMaterialRender);
    pMapAttrib->GetAttr_RenderBase(XML_MATERIAL_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, m_pIObject, &m_pMaterialRender);
    pMapAttrib->GetAttr_CRegion4(XML_MATERIAL_RENDER_PREFIX XML_PANEL_RENDER_REGION, true, &m_rcMaterialRenderRegion);

//     SAFE_RELEASE(m_pMaskRender);
//     pMapAttrib->GetAttr_RenderBase(XML_MASK_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, m_pIObject, &m_pMaskRender);
//     pMapAttrib->GetAttr_CRegion4(XML_MASK_RENDER_PREFIX XML_PANEL_RENDER_REGION, true, &m_rcMaskRenderRegion);

    pMapAttrib->GetAttr_CRegion4(XML_BACKGND_RENDER_PREFIX XML_PANEL_RENDER_REGION, true, &m_rcBkgndRenderRegion);
    pMapAttrib->GetAttr_CRegion4(XML_FOREGND_RENDER_PREFIX XML_PANEL_RENDER_REGION, true, &m_rcForegndRenderRegion);
}

void  Panel::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
	__super::OnEditorGetAttrList(pData);
    IUIEditor* pEditor = pData->pEditor;

	IUIEditorGroupAttribute*  pPanelGroup = pEditor->CreateGroupAttribute(pData->pGroupAttr, Panel::GetXmlName(), NULL);

    IUIEditorGroupAttribute*  pLayoutGroup = pEditor->CreateGroupAttribute(pPanelGroup, XML_LAYOUT, NULL);
	pEditor->CreateTextAttribute(pLayoutGroup, XML_LAYOUT, pData->szPrefix);
    if (m_pLayout)
    {
        EDITORGETOBJECTATTRLISTDATA data = {0};
        data.pEditor = pEditor;
        data.pGroupAttr = pLayoutGroup;
        data.szPrefix = pData->szPrefix;
        m_pLayout->OnEditorGetAttrList(&data);
    }

	pEditor->CreateTextAttribute(pPanelGroup, XML_MATERIAL_RENDER_PREFIX XML_RENDER_TYPE, pData->szPrefix);
	pEditor->CreateTextAttribute(pPanelGroup, XML_MATERIAL_RENDER_PREFIX XML_PANEL_RENDER_REGION, pData->szPrefix);
	pEditor->CreateTextAttribute(pPanelGroup, XML_BACKGND_RENDER_PREFIX XML_PANEL_RENDER_REGION, pData->szPrefix);
	pEditor->CreateTextAttribute(pPanelGroup, XML_FOREGND_RENDER_PREFIX XML_PANEL_RENDER_REGION, pData->szPrefix);

    IUIEditorGroupAttribute* pMaterialRenderGroup = pEditor->CreateGroupAttribute(pPanelGroup, XML_MATERIAL_RENDER_PREFIX, NULL);
    pEditor->CreateTextAttribute(pMaterialRenderGroup, XML_RENDER_TYPE, pData->szPrefix, XML_MATERIAL_RENDER_PREFIX);
    if (m_pMaterialRender)
    {
        String strPrefix;
        if (pData->szPrefix)
            strPrefix.append(pData->szPrefix);
        strPrefix.append(XML_MATERIAL_RENDER_PREFIX);

        EDITORGETOBJECTATTRLISTDATA data;
        data.pEditor = pData->pEditor;
        data.pGroupAttr = pMaterialRenderGroup;
        data.szPrefix = strPrefix.c_str();
        UISendMessage(m_pMaterialRender, UI_EDITOR_GETOBJECTATTRLIST, (WPARAM)&data);
    }
}

void  Panel::RealDrawObject(IRenderTarget* pRenderTarget, RenderContext roc)
{
    __super::RealDrawObject(pRenderTarget, roc);

    // 放在最后绘制遮罩层  // TODO: 实现不了遮罩层。单个控件刷新时无法处理
//     if (m_pMaskRender)
//     {
//         CRect rcMaskRegion(0,0, this->GetWidth(), this->GetHeight());
//         rcMaskRegion.DeflateRect(&m_rcMaskRenderRegion);
// 
//         roc.Update(pRenderTarget);   // 还原属性
//         m_pMaskRender->DrawState(pRenderTarget, &rcMaskRegion, 0);
//     }
}

// Panel作为一个容器，不应该有自己的前景，否则上面的子对象无常获取到正确的背景了
// 但为了适应两个Render的绘制，将foregnd render也在onerasebkgnd中绘制
//
// TODO: 为了将用于换肤的图片能够绘制在Frame图片的阴影之内，这里加了m_rcBkgndRenderRegion的配置
//       
void Panel::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	CRect rc(0,0, this->GetWidth(), this->GetHeight());
	
	if (m_pBkgndRender)
	{
		CRect rcBkgnd(&rc);
		rcBkgnd.DeflateRect(&m_rcBkgndRenderRegion);
		m_pBkgndRender->DrawState(pRenderTarget, &rcBkgnd, 0);
	}
	
    if (m_pMaterialRender)
    {
        CRect rcTextureRegion(&rc);
        rcTextureRegion.DeflateRect(&m_rcMaterialRenderRegion);
        m_pMaterialRender->DrawState(pRenderTarget, &rcTextureRegion, 0);
    }

	if (m_pForegndRender)
	{
		CRect rcForegnd(&rc);
		rcForegnd.DeflateRect(&m_rcForegndRenderRegion);
		m_pForegndRender->DrawState(pRenderTarget, &rcForegnd, 0);
	}
}


void Panel::SetMaterialRender(IRenderBase* p)
{
    SAFE_RELEASE(m_pMaterialRender);
    m_pMaterialRender = p;

    if (m_pMaterialRender)
        m_pMaterialRender->AddRef();
}
IRenderBase*  Panel::GetMaterialRender()
{
    return m_pMaterialRender;
}

}