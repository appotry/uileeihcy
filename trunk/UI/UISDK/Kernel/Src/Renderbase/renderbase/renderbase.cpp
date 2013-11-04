#include "stdafx.h"
#include "renderbase.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#pragma comment(lib, "uxtheme.lib")

namespace UI
{

RenderBase::RenderBase()
{
    m_pIRenderBase = NULL;
	m_pObject = NULL;
	m_nRenderType = RENDER_TYPE_NULL;
    m_pUIApplication = NULL;
}

IColorRes*  RenderBase::GetActiveSkinColorRes()
{
    if (NULL == m_pUIApplication)
        return NULL;

	IColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
	UIASSERT(NULL != pColorRes);
	return pColorRes;
}

IImageRes*  RenderBase::GetActiveSkinImageRes()
{
	if (NULL == m_pUIApplication)
		return NULL;

	IImageRes* pImageRes = m_pUIApplication->GetActiveSkinImageRes();
	UIASSERT(NULL != pImageRes);
	return pImageRes;
}

void  RenderBase::CheckThemeChanged()
{
    if (this->IsThemeRender())
    {
        UISendMessage(static_cast<IMessage*>(m_pIRenderBase), WM_THEMECHANGED);
    }
}
//////////////////////////////////////////////////////////////////////////

ThemeRenderBase::ThemeRenderBase()
{
    m_bNoTheme = false;
    m_hTheme = NULL;
    m_pIThemeRenderBase = NULL;
}
ThemeRenderBase::~ThemeRenderBase()
{
    if (m_hTheme)
    {
        ::CloseThemeData( m_hTheme );
        m_hTheme = NULL;
    }
}
void ThemeRenderBase::OnInit()
{
    __super::nvProcessMessage(m_pIThemeRenderBase->GetCurMsg(), 0, false);
    this->CreateTheme(); // ���ʱ����ܻ�û�õ�m_bNoTheme����
}
void ThemeRenderBase::OnThemeChanged()
{
    this->CreateTheme();
}
void ThemeRenderBase::CreateTheme()
{
    if (m_hTheme)
    {
        ::CloseThemeData(m_hTheme);
        m_hTheme = NULL;
    }
    if (!m_bNoTheme)
    {
        const TCHAR* pThemeName = m_pIThemeRenderBase->GetThemeName();
        if (pThemeName)
        {
            // ps: ����ʱ����һ��������NULL���ɣ������ڻ���listview item/Treeview itemʱ��
            //     �����ȵ���SetWindowTheme(m_hWnd, L"explorer", NULL);
            ///    ͬʱ��OpenThemeData(m_hWnd)���С�
            m_hTheme = ::OpenThemeData(m_pObject->GetHWND(), pThemeName);
        }
    }
}
void ThemeRenderBase::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    m_pIRenderBase->nvProcessMessage(GetCurMsg(),0,false);
    
    String strAttr;
    if (szPrefix)
        strAttr = szPrefix;
    strAttr.append(XML_RENDER_THEME_DISABLE);
    pMapAttrib->GetAttr_bool(strAttr.c_str(), pData->bErase, &m_bNoTheme);

    if (m_bNoTheme && NULL != m_hTheme)
    {
        ::CloseThemeData(m_hTheme);
        m_hTheme = NULL;
    }
}
void  ThemeRenderBase::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
	pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_THEME_DISABLE, pData->szPrefix);
}

}