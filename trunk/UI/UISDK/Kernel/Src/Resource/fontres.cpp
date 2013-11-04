#include "stdafx.h"
#include "fontres.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdi\gdifont.h"
#include "UISDK\Kernel\Src\Renderlibrary\gdiplus\gdiplusfont.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"

namespace UI
{

#if 0

UIFont::UIFont(LOGFONT* plogfont, UIFont** ppOutRef)
{ 
	m_hFont = NULL; 
	m_dwRef = 0; 
	m_pLogFont = plogfont;

	m_pOutRef = ppOutRef;
	*ppOutRef = this;
}
UIFont::~UIFont() 
{ 
	if (-1 == m_dwRef )
	{
		m_hFont = NULL;
	}
	else
	{
		if (m_hFont)
		{
			::DeleteObject(m_hFont); 
			m_hFont = NULL;
		}
	}
	m_dwRef = 0; 
	m_pLogFont = NULL;

	if(m_pOutRef != NULL)
	{
		*m_pOutRef = NULL;
		m_pOutRef = NULL;
	}
}


/*static*/ void UIFont::CreateInstance(LOGFONT* plogfont, UIFont** ppOutRef) 
{ 
	UIFont* pFont = new UIFont(plogfont, ppOutRef); 
	
	if (plogfont)
	{
		pFont->Load(plogfont);
	}
	pFont->AddRef();
}

bool UIFont::Load( LOGFONT* plogfont )
{
	if (NULL == plogfont)
	{
		UI_LOG_WARN(_T("UIFont::Load plogfont == NULL"));
		return false;
	}

	m_pLogFont = plogfont;
	m_hFont = ::CreateFontIndirect(m_pLogFont);
	if (NULL == m_hFont )
	{
		UI_LOG_WARN(_T("UIFont::SetLogFont CreateFontIndirect failed, facename=%s"), m_pLogFont->lfFaceName );
		return false;
	}

	return true;
}

HFONT UIFont::GetFont()
{ 
	return m_hFont; 
}

void UIFont::ModifyFont(LOGFONT* plogfont)
{
	m_pLogFont = plogfont;
	if (m_hFont)
	{
		::DeleteObject(m_hFont);
		m_hFont = ::CreateFontIndirect(m_pLogFont);
	}
}

//
//	TODO: Attach/Detach怎么与GetFont/Release统一起来使用？
//
//	--> 目前 Detach全由Release来替代，Release自己判断是否需要释放FONT对象
//
void UIFont::Attach(HFONT hFont)
{	
	UIASSERT(hFont != NULL);
	m_hFont = hFont;
	m_pLogFont = NULL;
}
HFONT UIFont::Detach()
{
	UIASSERT(-1 == m_dwRef);
	UIASSERT(m_hFont != NULL);

	HFONT hOldFont = m_hFont;
	m_hFont = NULL;

	return hOldFont;
}
long UIFont::AddRef()
{
	if ( NULL == m_hFont && NULL != m_pLogFont )
	{
		m_hFont = ::CreateFontIndirect( m_pLogFont );
		if (NULL == m_hFont )
		{
			UI_LOG_WARN( _T("UIFont::AddRef, Create font failed, font facename=%s size=%d"), m_pLogFont->lfFaceName, m_pLogFont->lfHeight );
		}
	}

	m_dwRef ++;
	return m_dwRef;
}
long UIFont::Release()
{
	m_dwRef--;
	if (0 >= m_dwRef )
	{
		if (m_hFont && NULL != m_pLogFont)
		{
			::DeleteObject(m_hFont);
		}
		m_hFont = NULL;

		m_dwRef = 0;
		delete this;
		return 0;
	}

	return m_dwRef;
}
#endif

FontResItem::FontResItem() 
{ 
    m_pIFontResItem = NULL;
	m_pGdiFont = NULL;
	m_pGdiplusFont = NULL;
#ifdef UI_D2D_RENDER
	m_pD2DFont = NULL;
#endif
	m_wParam = 0;
	m_lParam = 0;
}
FontResItem::~FontResItem()
{ 
	SAFE_DELETE(m_pGdiFont);
	SAFE_DELETE(m_pGdiplusFont);
#ifdef UI_D2D_RENDER
	SAFE_DELETE(m_pD2DFont);
#endif
    SAFE_DELETE(m_pIFontResItem);
}

IFontResItem*  FontResItem::GetIFontResItem()
{
    if (NULL == m_pIFontResItem)
        m_pIFontResItem = new IFontResItem(this);

    return m_pIFontResItem;
}
//
// 通过结构体来快速赋值
//
void FontResItem::SetLogFont( LOGFONT* pLogFont )
{
	if ( NULL==pLogFont)
		return;

	memcpy(&m_lf, pLogFont, sizeof(LOGFONT));
	_tcscpy(m_lf.lfFaceName, pLogFont->lfFaceName);
	m_lf.lfEscapement = m_lf.lfOrientation;    // TODO: 实现字体角度的话，这两个值好像要相等
}

void FontResItem::ModifyFont(LOGFONT* pLogFont)
{
	this->SetLogFont(pLogFont);

	if (m_pGdiFont)
	{
		m_pGdiFont->ModifyFont(pLogFont);
	}
	if (m_pGdiplusFont)
	{
		m_pGdiplusFont->ModifyFont(pLogFont);
	}
#ifdef UI_D2D_RENDER
	if (m_pD2DFont)
	{
		m_pD2DFont->ModifyFont(pLogFont);
	}
#endif
}

IRenderFont* FontResItem::GetFont( GRAPHICS_RENDER_LIBRARY_TYPE eRenderType )
{
	switch(eRenderType)
	{
	case GRAPHICS_RENDER_LIBRARY_TYPE_GDI:
		{
			if (NULL == m_pGdiFont)
			{
				GDIRenderFont::CreateInstance((IRenderFont**)&m_pGdiFont);
				if (m_pGdiFont)
				{
					m_pGdiFont->SetOutRef((IRenderResource**)&m_pGdiFont);
					m_pGdiFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("%s gdi font create: %s, facename=%s, size=%d, Ptr=0x%08X"), FUNC_NAME, m_strID.c_str(), m_lf.lfFaceName, m_lf.lfHeight, m_pGdiFont );
				}
                return (IRenderFont*)m_pGdiFont;
			}
			if (m_pGdiFont )
			{
				m_pGdiFont->AddRef();
			}
			return (IRenderFont*)m_pGdiFont;
		}
		break;

	case GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS:
		{
			if (NULL == m_pGdiplusFont)
			{
				GdiplusRenderFont::CreateInstance((IRenderFont**)&m_pGdiplusFont);
				if (m_pGdiplusFont )
				{
					m_pGdiplusFont->SetOutRef((IRenderResource**)&m_pGdiplusFont);

					m_pGdiplusFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("%s gdiplus font create: %s \tPtr=0x%08X"), FUNC_NAME, m_strID.c_str(), m_pGdiplusFont );
				}
                return (IRenderFont*)m_pGdiplusFont;
			}
			if (m_pGdiplusFont)
			{
				m_pGdiplusFont->AddRef();
			}
			return (IRenderFont*)m_pGdiplusFont;
		}
		break;

#ifdef UI_D2D_RENDER
	case GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D:
		{
			if (NULL == m_pD2DFont)
			{
				Direct2DRenderFont::CreateInstance((IRenderFont**)&m_pD2DFont);
				if (m_pD2DFont )
				{
					m_pD2DFont->SetOutRef((IRenderResource**)&m_pD2DFont);
					m_pD2DFont->Load(&m_lf);
					UI_LOG_DEBUG(_T("%s direct2d font create: %s \tPtr=0x%08X"), FUNC_NAME, m_strID.c_str(), m_pGdiplusFont );
				}
                return (IRenderFont*)m_pD2DFont;
			}
			if (m_pD2DFont)
			{
				m_pD2DFont->AddRef();
			}
			return (IRenderFont*)m_pD2DFont;
		}
		break;;
#endif

	default:
		return NULL;
	}

	return NULL;
}

bool FontResItem::IsMyRenderFont(IRenderFont* pRenderFont)
{ 
    m_pIFontResItem = NULL;
    if (m_pGdiFont == pRenderFont || m_pGdiplusFont == pRenderFont) 
    {
        return true;
    }  
    return false;
}

FontRes::FontRes()
{
    m_pIFontRes = NULL;
}
FontRes::~FontRes()
{
	this->Clear();
    SAFE_DELETE(m_pIFontRes);
}
IFontRes*  FontRes::GetIFontRes()
{
    if (NULL == m_pIFontRes)
        m_pIFontRes = new IFontRes(this);

    return m_pIFontRes;
}
long FontRes::GetFontCount() 
{ 
	return (long) m_vFonts.size(); 
}
HRESULT FontRes::GetFontResItem(long lIndex, IFontResItem** ppResItem)
{
	if (NULL == ppResItem)
		return E_INVALIDARG;
	
	FontResItem* pItem = this->GetFontItem(lIndex);
	if (NULL == pItem)
		return E_FAIL;

    *ppResItem = pItem->GetIFontResItem();
	return S_OK;
}
FontResItem* FontRes::GetFontItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if (nIndex >= (int)m_vFonts.size() )
		return NULL;

	return m_vFonts[nIndex];
}
FontResItem* FontRes::GetFontItem( const String& strID )
{
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;
		if (p->GetIDRef() == strID )
			return p;
	}
	return NULL;
}
HRESULT FontRes::GetFont(BSTR bstrFontID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut)
{
	if (NULL == bstrFontID || NULL == ppOut)
		return E_INVALIDARG;

	vector<FontResItem*>::iterator iter = m_vFonts.begin();
	vector<FontResItem*>::iterator iterEnd = m_vFonts.end();

	FontResItem* pBaseFont = NULL;
	for (; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;
		if (0 == wcscmp(p->GetIDRef().c_str(), bstrFontID))
		{
			if (p->GetWParam() == 0 && p->GetLParam() == 0 )
			{
				*ppOut = p->GetFont(eRenderType);
				return S_OK;
			}
		}
	}
	return E_FAIL;
}

HRESULT  FontRes::GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont)
{
	if (NULL == ppFont)
		return E_INVALIDARG;

	if (0 == (int)m_vFonts.size())
		return E_FAIL;

	if (NULL == m_vFonts[0])
		return E_FAIL;

	 *ppFont = m_vFonts[0]->GetFont(eRenderType);
	return S_OK;
}
bool  FontRes::GetRenderFontID(IRenderFont* pFont, String& strID)
{
	vector<FontResItem*>::iterator iter = m_vFonts.begin();
	vector<FontResItem*>::iterator iterEnd = m_vFonts.end();

	FontResItem* pBaseFont = NULL;
	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;
		if (p->IsMyRenderFont(pFont) )
		{
			strID = p->GetIDRef();
			return true;
		}
	}
	return false;
}

HRESULT  FontRes::GetFontEx(IRenderFont* pFont, WPARAM wParam, LPARAM lParam, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont)
{
	if (NULL == pFont || NULL == ppFont)
		return E_INVALIDARG;

	String strFontID;
	if (false == GetRenderFontID(pFont, strFontID))
		return E_FAIL;
	
	vector<FontResItem*>::iterator iter = m_vFonts.begin();
	vector<FontResItem*>::iterator iterEnd = m_vFonts.end();

	FontResItem* pBaseFont = NULL;
	for( ; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;
		if (p->GetIDRef() == strFontID)
		{
			if (p->GetWParam() == wParam && p->GetLParam() == lParam)
			{
				*ppFont = p->GetFont(eRenderType);
				return S_OK;
			}
			else
			{
				if (p->GetWParam() == 0 && p->GetLParam() == 0)
				{
					pBaseFont = p;
				}
			}
		}
	}

	if (pBaseFont)  // 没有找到 strFontID+wparam+lparam对应的字体，但找到了strFontID对应的字体
	{
		LOGFONT lf;
		memcpy( &lf, pBaseFont->GetLogFont(), sizeof(LOGFONT) );
		if (wParam & FONTITEM_FLAG_UNDERLINE )
		{
			lf.lfUnderline = 1;
		}
		if (wParam & FONTITEM_FLAG_BOLD )
		{
			lf.lfWeight = FW_BOLD;
		}
		if (wParam & FONTITEM_FLAG_STRIKEOUT )
		{
			lf.lfStrikeOut = 1;
		}
		if ( wParam & FONTITEM_FLAG_ITALIC )
		{
			lf.lfItalic = 1;
		}
		FontResItem* pNewFontItem = this->InsertFont(strFontID, &lf, wParam, lParam );
		if (NULL == pNewFontItem )
		{
			UI_LOG_ERROR( _T("%s InsertFont failed, id=%s wParam=%d lParam=%d"), FUNC_NAME, strFontID.c_str(), wParam, lParam );
			return E_FAIL;
		}

		*ppFont = pNewFontItem->GetFont(eRenderType);
		return S_OK;
	}

	return E_FAIL;
}

bool FontRes::InsertFont( const String& strID, LOGFONT* pLogFont )
{
	FontResItem* pItem = this->InsertFont(strID, pLogFont, 0,0 );
	if (NULL == pItem )
		return false;
	
	return true;
}
FontResItem* FontRes::InsertFont( const String& strID, LOGFONT* pLogFont, WPARAM wParam, LPARAM lParam )
{
	if (NULL == pLogFont )
	{
		UI_LOG_ERROR(_T("FontRes::InsertFont failed."));
		return NULL;
	}
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;

		if (p->GetIDRef() == strID && p->GetWParam() == wParam && p->GetLParam() == lParam )
		{
			UI_LOG_WARN(_T("FontRes::InsertFont failed, insert item=%s"), strID.c_str() );
			return NULL;
		}
	}

	FontResItem*  pFontItem = new FontResItem;
	pFontItem->SetID( strID );
	pFontItem->SetLogFont( pLogFont );
	pFontItem->SetWParam(wParam);
	pFontItem->SetLParam(lParam);

	this->m_vFonts.push_back(pFontItem);  
	return pFontItem;
}
bool FontRes::ModifyFont( const String& strID, LOGFONT* pLogFont )
{
	if (NULL == pLogFont )
	{
		UI_LOG_ERROR(_T("FontRes::ModifyFont failed."));
		return NULL;
	}
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for (; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;

		if (p->GetIDRef() == strID)
		{
			p->ModifyFont(pLogFont);
			return true;
		}
	}
	UI_LOG_WARN(_T("FontRes::InsertFont failed, insert item=%s"), strID.c_str() );
	return false;
}
bool FontRes::RemoveFont( const String& strID )
{
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for( ; iter != iterEnd; iter++ )
	{
		FontResItem* p = *iter;

		if (p->GetIDRef() == strID )
		{
			delete p;
			p = NULL;
			m_vFonts.erase(iter);
			return true;
		}
	}
	return false;
}
void FontRes::Clear()
{
	vector<FontResItem*>::iterator  iter = m_vFonts.begin();
	vector<FontResItem*>::iterator  iterEnd = m_vFonts.end();

	for (; iter != iterEnd; iter++)
	{
		FontResItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vFonts.clear();
}


}