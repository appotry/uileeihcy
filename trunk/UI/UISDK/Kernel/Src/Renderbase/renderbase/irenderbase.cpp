#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\irenderbase.h"
#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{
UI_IMPLEMENT_Ixxx_INTERFACE_ProcessMessage(IRenderBase, RenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(IRenderBase, RenderBase, Message)
UI_IMPLEMENT_Ixxx_INTERFACE(IThemeRenderBase, ThemeRenderBase, RenderBase)
UI_IMPLEMENT_Ixxx_INTERFACE(INullRender, NullRender, RenderBase)

IRenderBase::IRenderBase()
{
    m_bCreateRenderBaseImpl = NULL;
    m_pRenderBaseImpl = NULL;
    m_lRef = 0;
}
IRenderBase::~IRenderBase()                    
{                                              
    if (m_bCreateRenderBaseImpl)            
        SAFE_DELETE(m_pRenderBaseImpl);     
}      

long  IRenderBase::AddRef() 
{ 
    m_lRef++;  
    return m_lRef; 
}
long  IRenderBase::Release()
{ 
    m_lRef--; 
    if (0 >= m_lRef) 
    {
        this->delete_this(); 
        return 0;
    }
    return m_lRef; 
}

void  IRenderBase::CheckThemeChanged()
{
    return m_pRenderBaseImpl->CheckThemeChanged();
}

bool  IRenderBase::IsThemeRender()
{
    return m_pRenderBaseImpl->IsThemeRender();
}
void  IRenderBase::SetObject(IUIApplication*  pUIApp, IObject* pObject)
{
    Object* pObj = NULL;
    if (pObject)
    {
        pObj = pObject->GetImpl();
    }
    return m_pRenderBaseImpl->SetObject(pUIApp, pObj);
}
IObject*  IRenderBase::GetObject()
{
    Object* pObj = m_pRenderBaseImpl->GetObject();
    if (NULL == pObj)
        return NULL;

    return pObj->GetIObject();
}
void  IRenderBase::SetRenderType(const RENDER_TYPE& nType)
{
    m_pRenderBaseImpl->SetRenderType(nType);
}
RENDER_TYPE  IRenderBase::GetRenderType()
{
    return m_pRenderBaseImpl->GetRenderType();
}
IColorRes*  IRenderBase::GetActiveSkinColorRes()
{
    return m_pRenderBaseImpl->GetActiveSkinColorRes();
}
IImageRes*  IRenderBase::GetActiveSkinImageRes()
{
    return m_pRenderBaseImpl->GetActiveSkinImageRes();
}

void  IRenderBase::SetAttribute(IMapAttribute* pMapAttrib, const TCHAR* szPrefix, bool bErase)
{
	SERIALIZEDATA data;
	data.pMapAttrib = pMapAttrib;
	data.szPrefix = szPrefix;
	data.nFlag = SERIALIZEFLAG_LOAD;
	if (bErase)
		data.nFlag |= SERIALIZEFLAG_LOAD_ERASEATTR;

	UISendMessage(static_cast<IMessage*>(this), UI_WM_SERIALIZE, (WPARAM)&data);
}

void  IRenderBase::DrawState(IRenderTarget* p, const CRect* prc, int nState) 
{
    RENDERBASE_DRAWSTATE drawstate = {0};
    drawstate.pRenderTarget = p;
    if (prc)
        CopyRect(&drawstate.rc, prc);
    drawstate.nState = nState;

    UISendMessage(static_cast<IMessage*>(this), UI_WM_RENDERBASE_DRAWSTATE, (WPARAM)&drawstate, 0);
};
SIZE  IRenderBase::GetDesiredSize() 
{ 
    SIZE s = {0,0}; 
    UISendMessage(static_cast<IMessage*>(this), UI_WM_GETDESIREDSIZE, (WPARAM)&s, 0);

    return s; 
}
void  IRenderBase::Init()
{
    UISendMessage(static_cast<IMessage*>(this), UI_WM_INITIALIZE);
}
//////////////////////////////////////////////////////////////////////////


const TCHAR*  IThemeRenderBase::GetThemeName()
{
    const TCHAR* pszName = NULL;
    UISendMessage(static_cast<IMessage*>(this), UI_WM_RENDERBASE_GETTHEMENAME, (WPARAM)&pszName, 0);
    return pszName;
}

HTHEME  IThemeRenderBase::GetTHEME()
{
    return m_pThemeRenderBaseImpl->GetTHEME();
}

}