#include "stdafx.h"
#include "UISDK\Kernel\Inc\Interface\iuiapplication.h"
#include "UISDK\Kernel\Src\Resource\skinmanager.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Src\Helper\topwindow\topwindowmanager.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\Resource\colormanager.h"
#include "UISDK\Kernel\Src\Resource\imagemanager.h"
#include "UISDK\Kernel\Src\Resource\colormanager.h"
#include "UISDK\Kernel\Src\Resource\fontmanager.h"
#include "UISDK\Kernel\Src\Resource\stylemanager.h"
#include "UISDK\Kernel\Src\Resource\layoutmanager.h"

namespace UI
{
ISkinManager::ISkinManager(SkinManager* p)
{
    m_pImpl = p;
}
SkinManager*  ISkinManager::GetImpl()
{
    return m_pImpl;
}

IUIApplication*  ISkinManager::GetUIApplication() { return m_pImpl->GetUIApplication(); }
UINT  ISkinManager::GetSkinCount() { return m_pImpl->GetSkinCount(); }
void  ISkinManager::GetSkinDirection(TCHAR*  szOut) { m_pImpl->GetSkinDirection(szOut); }
ISkinRes*  ISkinManager::AddSkin(const TCHAR*  szName) { return m_pImpl->AddSkin(szName); }
HRESULT  ISkinManager::ChangeSkin(ISkinRes*  pSkinRes, bool bSync) { return m_pImpl->ChangeSkin(pSkinRes, bSync); }
HRESULT  ISkinManager::ChangeSkinHLS(short h, short l, short s, int nFlag) { return m_pImpl->ChangeSkinHLS(h,l,s,nFlag); }
HRESULT  ISkinManager::SetActiveSkin(ISkinRes* pSkin) { return m_pImpl->SetActiveSkin(pSkin); }
ISkinRes*  ISkinManager::GetActiveSkin() { return m_pImpl->GetActiveSkin(); }
ISkinRes*  ISkinManager::GetSkinResByIndex(long lIndex)
{
    SkinRes* p = m_pImpl->GetSkinResByIndex(lIndex); 
    if (p)
        return p->GetISkinRes();
    return NULL;
}
bool  ISkinManager::Save(ISkinRes* pSkinRes) 
{
    if (NULL == pSkinRes)
        return m_pImpl->Save(NULL);
    else
        return m_pImpl->Save(pSkinRes->GetImpl()); 
}

//////////////////////////////////////////////////////////////////////////

ISkinRes::ISkinRes(SkinRes* p)
{
    m_pImpl = p;
}
SkinRes*  ISkinRes::GetImpl()
{
    return m_pImpl;
}

IUIApplication*  ISkinRes::GetUIApplication()  { return m_pImpl->GetUIApplication(); }
ISkinManager*    ISkinRes::GetSkinManager()    { return m_pImpl->GetISkinManager(); }
IImageManager*   ISkinRes::GetImageManager()   { return m_pImpl->GetIImageManager(); }
IColorManager*   ISkinRes::GetColorManager()   { return m_pImpl->GetIColorManager(); }
IFontManager*    ISkinRes::GetFontManager()    { return m_pImpl->GetIFontManager(); }
IStyleManager*   ISkinRes::GetStyleManager()   { return m_pImpl->GetIStyleManager(); }
ILayoutManager*  ISkinRes::GetLayoutManager()  { return m_pImpl->GetILayoutManager(); }

IImageRes*  ISkinRes::GetImageRes()            { return m_pImpl->GetIImageRes(); }
IFontRes*   ISkinRes::GetFontRes()             { return m_pImpl->GetIFontRes();  }
IColorRes*  ISkinRes::GetColorRes()            { return m_pImpl->GetIColorRes(); }
IStyleRes*  ISkinRes::GetStyleRes()            { return m_pImpl->GetIStyleRes(); }

const TCHAR*  ISkinRes::GetName()              { return m_pImpl->GetName(); }
const TCHAR*  ISkinRes::GetPath()              { return m_pImpl->GetPath(); }
void  ISkinRes::OnNewUIDocument(IUIDocument* pDoc, const TCHAR* szPath)  { return m_pImpl->OnNewUIDocument(pDoc, szPath); }

bool  ISkinRes::Load()                         { return m_pImpl->Load(); }
bool  ISkinRes::ChangeSkinHLS(short h, short l, short s, int nFlag) { return m_pImpl->ChangeSkinHLS(h,l,s,nFlag); }
bool  ISkinRes::LoadAllWindowNodeInfo(LayoutWindowNodeInfo* pInfoArray, int* pArraySize)  { return m_pImpl->LoadAllWindowNodeInfo(pInfoArray, pArraySize); }

ISkinDataSource*  ISkinRes::GetDataSource()    { return m_pImpl->GetDataSource(); }
UINT  ISkinRes::GetXmlDocCount()               { return m_pImpl->GetXmlDocCount(); }
const TCHAR*  ISkinRes::GetXmlDoc(UINT nIndex, IUIDocument** pp)  { return m_pImpl->GetXmlDoc(nIndex, pp); }

//////////////////////////////////////////////////////////////////////////

ITopWindowManager::ITopWindowManager(TopWindowManager* p) 
{
    m_pImpl = p;
}
TopWindowManager* ITopWindowManager::GetImpl()
{
    return m_pImpl;
}

HRESULT  ITopWindowManager::AddTopWindowObject(IWindowBase* p)
{
    if (NULL == p)
        return E_INVALIDARG;

    return m_pImpl->AddTopWindowObject(p->GetImpl());
}
HRESULT  ITopWindowManager::RemoveTopWindowObject(IWindowBase* p)
{
    if (NULL == p)
        return E_INVALIDARG;

    return m_pImpl->RemoveTopWindowObject(p->GetImpl());
}
void  ITopWindowManager::ChangeSkin(ISkinRes* pNewSkinRes)
{
    if (NULL == pNewSkinRes)
        return;

    m_pImpl->ChangeSkin(pNewSkinRes->GetImpl());
}

bool  ITopWindowManager::UpdateAllWindow() { return m_pImpl->UpdateAllWindow(); }
void  ITopWindowManager::SendMessage2AllWnd(UIMSG*  pMsg) { return m_pImpl->SendMessage2AllWnd(pMsg); }
void  ITopWindowManager::PostMessage2AllWnd(UIMSG* pMsg)  { return m_pImpl->PostMessage2AllWnd(pMsg); }
void  ITopWindowManager::ForwardMessage2AllObj(UIMSG*  pMsg) { return m_pImpl->ForwardMessage2AllObj(pMsg); }

//////////////////////////////////////////////////////////////////////////

IColorManager::IColorManager(ColorManager* p) 
{
    m_pImpl = p;
}
ColorManager*  IColorManager::GetImpl()
{
    return m_pImpl;
}

IColorRes*  IColorManager::GetColorRes() { return m_pImpl->GetColorRes(); }

HRESULT  IColorManager::InsertColorItem(const TCHAR* szID, const TCHAR* szColor) { return m_pImpl->InsertColorItem(szID, szColor); }
HRESULT  IColorManager::ModifyColorItem(const TCHAR* szID, const TCHAR* szColor) { return m_pImpl->ModifyColorItem(szID, szColor); }
HRESULT  IColorManager::RemoveColorItem(const TCHAR* szID) { return m_pImpl->RemoveColorItem(szID); }


//////////////////////////////////////////////////////////////////////////


IImageManager::IImageManager(ImageManager* p) { m_pImpl = p; }
ImageManager*  IImageManager::GetImpl() { return m_pImpl; }

IImageRes*   IImageManager::GetImageRes() { return m_pImpl->GetImageRes(); }
ICursorRes*  IImageManager::GetCursorRes(){ return m_pImpl->GetCursorRes(); }
IGifRes*     IImageManager::GetGifRes() { return m_pImpl->GetGifRes(); }

IImageResItem*  IImageManager::InsertImageItem(const TCHAR* szID, const TCHAR* szPath) { return m_pImpl->InsertImageItem(szID, szPath); }
HRESULT  IImageManager::ModifyImageItem(const TCHAR* szID, const TCHAR* szPath) { return m_pImpl->ModifyImageItem(szID, szPath); }
HRESULT  IImageManager::RemoveImageItem(const TCHAR* szID) { return m_pImpl->RemoveImageItem(szID); }

HRESULT  IImageManager::ModifyImageItemInRunTime(const TCHAR* szID, const TCHAR* szPath) { return m_pImpl->ModifyImageItemInRunTime(szID, szPath); }
HRESULT  IImageManager::ModifyImageItemAlpha(const TCHAR* szID, int nAlphaPercent) { return m_pImpl->ModifyImageItemAlpha(szID, nAlphaPercent); }

//////////////////////////////////////////////////////////////////////////

IFontManager::IFontManager(FontManager* p) { m_pImpl = p; } 
FontManager*  IFontManager::GetImpl() { return m_pImpl; }

IFontRes*  IFontManager::GetFontRes() { return m_pImpl->GetFontRes(); }
HRESULT  IFontManager::InsertFontItem(const TCHAR* szID, LOGFONT* pLogFont) { return m_pImpl->InsertFontItem(szID, pLogFont); }
HRESULT  IFontManager::ModifyFontItem(const TCHAR* szID, LOGFONT* pLogFont) { return m_pImpl->ModifyFontItem(szID, pLogFont); }
HRESULT  IFontManager::RemoveFontItem(const TCHAR* szID) { return m_pImpl->RemoveFontItem(szID); }


//////////////////////////////////////////////////////////////////////////

IStyleManager::IStyleManager(StyleManager* p) { m_pImpl = p; }
StyleManager*  IStyleManager::GetImpl() { return m_pImpl; }

IStyleRes*  IStyleManager::GetStyleRes() { return m_pImpl->GetStyleRes(); }

HRESULT  IStyleManager::InsertStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szInherit) { return m_pImpl->InsertStyleItem(type, szID, szInherit); }
HRESULT  IStyleManager::ModifyStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szInherit) { return m_pImpl->ModifyStyleItem(type, szID, szInherit); }
HRESULT  IStyleManager::RemoveStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID) { return m_pImpl->RemoveStyleItem(type, szID); }

HRESULT  IStyleManager::InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey, const TCHAR* szValue) { return m_pImpl->InsertStyleAttribute(type, szID, szKey, szValue); }
HRESULT  IStyleManager::ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey, const TCHAR* szValue) { return m_pImpl->ModifyStyleAttribute(type, szID, szKey, szValue); }
HRESULT  IStyleManager::RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey) { return m_pImpl->RemoveStyleAttribute(type, szID, szKey); }

HRESULT  IStyleManager::LoadStyle(const TCHAR* szTagName, const TCHAR* szStyleClass, const TCHAR* szID, IMapAttribute* pMapStyle) { return m_pImpl->LoadStyle(szTagName, szStyleClass, szID, pMapStyle); }
HRESULT  IStyleManager::ParseStyle(const TCHAR* szObjName, IMapAttribute* pMapAttrib) { return m_pImpl->ParseStyle(szObjName, pMapAttrib); }

//////////////////////////////////////////////////////////////////////////

ILayoutManager::ILayoutManager(LayoutManager* p) { m_pImpl = p; }
LayoutManager*  ILayoutManager::GetImpl() { return m_pImpl; }

ILayoutRes*  ILayoutManager::GetLayoutRes() { return m_pImpl->GetLayoutRes(); }
HRESULT  ILayoutManager::LoadRootLayout(IObject* pRootObj, const TCHAR* szId) { return m_pImpl->LoadRootLayout(pRootObj, szId); }
HRESULT  ILayoutManager::LoadRootLayout(const TCHAR* szObjName, const TCHAR* szId, IObject** ppObj) { return m_pImpl->LoadRootLayout(szObjName, szId, ppObj); }
HRESULT  ILayoutManager::LoadLayout(IUIElement* pUIElement, IObject* pParent, IObject** ppObj) { return m_pImpl->LoadLayout(pUIElement, pParent, ppObj); }
HRESULT  ILayoutManager::LoadObjAttrAndChild(IObject* pRootObj, IUIElement* pUIElement) { return m_pImpl->LoadObjAttrAndChild(pRootObj, pUIElement); }


//////////////////////////////////////////////////////////////////////////

IImageRes::IImageRes(ImageRes* p) { m_pImpl = p; }
ImageRes*  IImageRes::GetImpl()   { return m_pImpl; }

long  IImageRes::GetImageCount() { return m_pImpl->GetImageCount(); }
IImageResItem*  IImageRes::GetImageResItem(long lIndex) { return m_pImpl->GetImageResItem(lIndex); }
IImageResItem*  IImageRes::GetImageResItem(const TCHAR* szID) { return m_pImpl->GetImageResItem(szID); }
bool  IImageRes::ModifyImage(const TCHAR* szId, const TCHAR* szPath) { return m_pImpl->ModifyImage(szId, szPath); }
HRESULT  IImageRes::GetBitmap(const TCHAR* szImageID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderBitmap** ppRenderBitmap)
{ return m_pImpl->GetBitmap(szImageID, eRenderType, ppRenderBitmap); }
HBITMAP  IImageRes::LoadBitmap(const TCHAR* szId)  { return m_pImpl->LoadBitmap(szId); }

//////////////////////////////////////////////////////////////////////////

IImageResItem::IImageResItem(ImageResItem* p) { m_pImpl = p; }
ImageResItem*  IImageResItem::GetImpl() { return m_pImpl; }

const TCHAR*  IImageResItem::GetID() { return m_pImpl->GetID(); }
const TCHAR*  IImageResItem::GetPath() { return m_pImpl->GetPath(); }
bool  IImageResItem::ModifyImage(const TCHAR*  szPath) { return m_pImpl->ModifyImage(szPath); }
bool  IImageResItem::GetUseSkinHLS() { return m_pImpl->GetUseSkinHLS(); }
bool  IImageResItem::GetNeedAntiAliasing() { return m_pImpl->GetNeedAntiAliasing(); }
void  IImageResItem::SetUseSkinHLS(bool b) { return m_pImpl->SetNeedAntiAliasing(b); }
void  IImageResItem::SetNeedAntiAliasing(bool b) { return m_pImpl->SetNeedAntiAliasing(b); }

//////////////////////////////////////////////////////////////////////////


ICursorRes::ICursorRes(CursorRes* p) { m_pImpl = p; }
CursorRes*  ICursorRes::GetImpl() { return m_pImpl; }

void ICursorRes::GetCursor(const TCHAR*  szCursorID, IUICursor** pp) { m_pImpl->GetCursor(szCursorID, pp); }

ICursorResItem::ICursorResItem(CursorResItem* p) { m_pImpl = p; }
CursorResItem*  ICursorResItem::GetImpl()  { return m_pImpl; }

//////////////////////////////////////////////////////////////////////////

IColorRes::IColorRes(ColorRes* p) { m_pImpl = p; }
ColorRes*  IColorRes::GetImpl() { return m_pImpl; }

long  IColorRes::GetColorCount() { return m_pImpl->GetColorCount(); }
IColorResItem*  IColorRes::GetColorResItem(long lIndex) { return m_pImpl->GetColorResItem(lIndex); }
void  IColorRes::GetColor(const TCHAR* szColorId, Color** pp) { return m_pImpl->GetColor(szColorId, pp); }

//////////////////////////////////////////////////////////////////////////

IColorResItem::IColorResItem(ColorResItem* p) { m_pImpl = p; }
ColorResItem*  IColorResItem::GetImpl() { return m_pImpl; }

const TCHAR* IColorResItem::GetID() { return m_pImpl->GetID(); }
const TCHAR* IColorResItem::GetColorString() { return m_pImpl->GetColorString(); }

//////////////////////////////////////////////////////////////////////////

IFontResItem::IFontResItem(FontResItem* p) { m_pImpl = p; }
FontResItem*  IFontResItem::GetImpl(){ return m_pImpl; }

const TCHAR*  IFontResItem::GetID()  { return m_pImpl->GetID(); }
LOGFONT*  IFontResItem::GetLogFont() { return m_pImpl->GetLogFont(); }

//////////////////////////////////////////////////////////////////////////

IGifRes::IGifRes(GifRes* p) { m_pImpl = p; }
GifRes*  IGifRes::GetImpl()  { return m_pImpl; }

HRESULT  IGifRes::GetGifImage(const TCHAR*  szId, __out IGifImage** ppGifImage) { return m_pImpl->GetGifImage(szId, ppGifImage); }

IGifResItem::IGifResItem(GifResItem* p) { m_pImpl = p; }
GifResItem*  IGifResItem::GetImpl()  { return m_pImpl; }

//////////////////////////////////////////////////////////////////////////


IFontRes::IFontRes(FontRes* p) { m_pImpl = p; }
FontRes*  IFontRes::GetImpl() { return m_pImpl; }

long     IFontRes::GetFontCount() { return m_pImpl->GetFontCount(); }
HRESULT  IFontRes::GetFontResItem(long lIndex, IFontResItem** ppResItem) { return m_pImpl->GetFontResItem(lIndex, ppResItem); }
HRESULT  IFontRes::GetFont(BSTR bstrFontID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut) { return m_pImpl->GetFont(bstrFontID, eRenderType, ppOut); }
HRESULT  IFontRes::GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont) { return m_pImpl->GetDefaultFont(eRenderType, ppFont); }
HRESULT  IFontRes::GetFontEx(IRenderFont* pFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont) { return m_pImpl->GetFontEx(pFont, w, l, eRenderType, ppFont); }

//////////////////////////////////////////////////////////////////////////

IStyleResItem::IStyleResItem(StyleResItem* p) { m_pImpl = p; }
StyleResItem*  IStyleResItem::GetImpl()    { return m_pImpl; }

STYLE_SELECTOR_TYPE  IStyleResItem::GetSelectorType() { return m_pImpl->GetSelectorType(); }
const TCHAR*  IStyleResItem::GetID()       { return m_pImpl->GetID(); }
const TCHAR*  IStyleResItem::GetInherits() { return m_pImpl->GetInherits(); }

void  IStyleResItem::GetAttributeMap(IMapAttribute** ppMapAttrib) { m_pImpl->GetAttributeMap(ppMapAttrib); }

//////////////////////////////////////////////////////////////////////////

IStyleRes::IStyleRes(StyleRes* p) { m_pImpl = p;}
StyleRes*  IStyleRes::GetImpl()   { return m_pImpl; }

long     IStyleRes::GetStyleCount() { return m_pImpl->GetStyleCount(); }
IStyleResItem*  IStyleRes::GetStyleResItem(long lIndex) { return m_pImpl->GetStyleResItem(lIndex); }
IStyleResItem*  IStyleRes::GetStyleResItem2(STYLE_SELECTOR_TYPE type, const TCHAR* szId) { return m_pImpl->GetStyleResItem2(type, szId); }


//////////////////////////////////////////////////////////////////////////
#if 0
ILayoutResItem::ILayoutResItem(LayoutResItem* p) { m_pImpl = p;}
LayoutResItem*  ILayoutResItem::GetImpl()        { return m_pImpl; }

HRESULT  ILayoutResItem::GetSelfAttr(IMapAttribute** ppMapAttribute) { return m_pImpl->GetSelfAttr(ppMapAttribute); }
HRESULT  ILayoutResItem::GetStyleAttr(IMapAttribute** ppMapAttribute){ return m_pImpl->GetStyleAttr(ppMapAttribute);  }
HRESULT  ILayoutResItem::GetLastAttr(IMapAttribute** ppMapAttribute) { return m_pImpl->GetLastAttr(ppMapAttribute); }
#endif
//////////////////////////////////////////////////////////////////////////

ILayoutRes::ILayoutRes(LayoutRes* p) { m_pImpl = p;}
LayoutRes*  ILayoutRes::GetImpl()    { return m_pImpl; }
}