#include "StdAfx.h"
#include "fontmanager.h"

#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"

FontManager::FontManager(SkinRes* p)
{
    m_pUIElement = NULL;
    m_pIFontManager = NULL;
#if 0
	m_pFontParse = NULL;
#endif
	m_pSkinRes = p;
}

FontManager::~FontManager(void)
{
	this->Clear();
#if 0
    SAFE_RELEASE(m_pFontParse);
#endif
    SAFE_RELEASE(m_pUIElement);
    SAFE_DELETE(m_pIFontManager);
}

IFontManager*  FontManager::GetIFontManager()
{
    if (NULL == m_pIFontManager)
        m_pIFontManager = new IFontManager(this);

    return m_pIFontManager;
}

bool FontManager::Create( const String& strXmlPath )
{
#if 0
	if (m_pFontParse)
	{
		UI_LOG_WARN(_T("FontManager::Create font parse already exist."));
		return false;
	}
	if (NULL == m_pSkinRes)
		return false;

	m_pFontParse = IParseClassFactory::CreateFontParseInstance(m_pSkinRes->GetUIApplication(), strXmlPath);
	if (false == m_pFontParse->Create())
	{
		UI_LOG_ERROR(_T("FontManager::Create create font parse failed. path=%s"), strXmlPath.c_str());
		return false;
	}
#endif
	return true;
}
bool FontManager::Load( const String& strXmlPath )
{
#if 0
	if (m_pFontParse)
	{
		UI_LOG_WARN(_T("FontManager::Load font parse already exist."));
		return false;
	}

	if (NULL == m_pSkinRes)
		return false;

	m_pFontParse = IParseClassFactory::CreateFontParseInstance(m_pSkinRes->GetUIApplication(), strXmlPath );

	if( false == m_pFontParse->Load(&m_resFont) )
		return false;

#if 0 // TODO unimple
	this->Clear();

	// 从xml中解析出所有的图片列表
	FontXmlParse  parse;
	if( false == parse.LoadXml( strXmlPath ) )
		return false;

	vector<FontItem>  vItems;

	if( false == parse.ParseFontItems( vItems ) )
		return false;

	// 解析xml数据,保存
	size_t  size = vItems.size();
	for ( size_t i = 0; i < size; i++ )
	{

		String  strID = vItems[i].strID;
		
		int nWeight = FW_NORMAL;
		if( vItems[i].bBold )
			nWeight = FW_BOLD;

		HFONT   hFont = ::CreateFont(
			vItems[i].nHeight,            // nHeight
			0,                            // nWidth
			0,                            // nEscapement
			vItems[i].nOrientation,       // nOrientation
			nWeight,                      // nWeight
			vItems[i].bItalic,            // bItalic
			vItems[i].bUnderline,         // bUnderline
			vItems[i].bStrikeout,         // cStrikeOut
			ANSI_CHARSET,                 // nCharSet
			OUT_DEFAULT_PRECIS,           // nOutPrecision
			CLIP_DEFAULT_PRECIS,          // nClipPrecision
			DEFAULT_QUALITY,              // nQuality
			DEFAULT_PITCH | FF_SWISS,     // nPitchAndFamily
			vItems[i].strFacename.c_str() // lpszFacename
			);                 
		if( NULL == hFont )
		{
			UI_LOG_ERROR( _T("FontManager::Load, CreateFont(%s) return NULL"), vItems[i].strFacename.c_str() );
			continue;
		}
		 
		this->m_map.insert( make_pair(strID, hFont) );
	}
#endif
#endif
	return true;
}

bool FontManager::Reload()
{
#if 0
	if( NULL == m_pFontParse )
	{
		UI_LOG_ERROR(_T("FontManager::Reload m_pFontParse == NULL."));
		return false;
	}

	this->Clear();
	return  m_pFontParse->Reload( &m_resFont );
#endif
    return false;
}
bool FontManager::Save()
{
#if 0
	assert( NULL != m_pFontParse );
	if( NULL == m_pFontParse )
		return false;

	return m_pFontParse->Save(&m_resFont);
#endif
    return false;
}

/*
**	清除所有图片资源
*/
void FontManager::Clear()
{
	m_resFont.Clear();
}

int FontManager::GetFontCount()
{
	return m_resFont.GetFontCount();
}
IFontResItem*  FontManager::GetFontItemInfo(int nIndex)
{
	FontResItem* pItem = m_resFont.GetFontItem(nIndex);
	if (NULL == pItem)
		return NULL;

    return pItem->GetIFontResItem();
}


HRESULT FontManager::InsertFontItem(const TCHAR* szID, LOGFONT* pLogFont)
{
#if 0
	if (NULL == bstrID || NULL == pLogFont)
		return E_INVALIDARG;

	if (NULL == m_pFontParse)
		return E_FAIL;

	if (false == m_resFont.InsertFont(bstrID, pLogFont))
	{
		UI_LOG_ERROR(_T("%s m_resFont.InsertFont strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}

	if (false == m_pFontParse->InsertFont(this->m_resFont.GetFontItem(bstrID)))
	{
		UI_LOG_ERROR(_T("%s m_pFontParse.InsertFont strID=%s Failed "), FUNC_NAME, bstrID);
		return E_FAIL;
	}
#endif
	return S_OK;
}
HRESULT FontManager::ModifyFontItem(const TCHAR* szID, LOGFONT* pLogFont)
{
#if 0
	if (NULL == bstrID || NULL == pLogFont)
		return E_INVALIDARG;

	if (NULL == m_pFontParse)
		return E_FAIL;

	if (false == m_resFont.ModifyFont(bstrID, pLogFont))
	{
		UI_LOG_ERROR(_T("%s m_resFont.ModifyFont strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}

	if (false == m_pFontParse->ModifyFont(this->m_resFont.GetFontItem(bstrID)))
	{
		UI_LOG_ERROR(_T("%s m_pFontParse.ModifyFont strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT FontManager::RemoveFontItem(const TCHAR* szID)
{
#if 0
	if (NULL == bstrID)
		return E_INVALIDARG;

	if (NULL == m_pFontParse)
		return E_FAIL;

	if (false == m_resFont.RemoveFont(bstrID))
	{
		UI_LOG_ERROR(_T("%s m_resFont.RemoveFont strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}

	if (false == m_pFontParse->RemoveFont(bstrID))
	{
		UI_LOG_ERROR(_T("%s m_pFontParse.RemoveFont strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}
#endif
	return S_OK;
}

IFontRes*  FontManager::GetFontRes()
{
	return m_resFont.GetIFontRes();
}


HRESULT  FontManager::UIParseSkinElement(IUIElement* pElem, ISkinRes* pSkinRes)
{
    IFontManager*  pFontMgr = pSkinRes->GetFontManager();
    if (NULL == pFontMgr)
        return E_FAIL;

    return pFontMgr->GetImpl()->ParseNewElement(pElem);
}

HRESULT  FontManager::ParseNewElement(IUIElement* pElem)
{
    if (m_pUIElement)  // 多份定义还未实现，要考虑的东西较多， 下一次再实现吧
        return E_NOTIMPL;

    m_pUIElement = pElem;
    m_pUIElement->AddRef();

    // 遍历其子元素
    IUIChildNodeEnum* pEnum = pElem->EnumChild();
    if (pEnum)
    {
        IUIElement* pChildElement = NULL;
        while (pChildElement = pEnum->NextElement())
        {
            this->OnNewChild(pChildElement);
            SAFE_RELEASE(pChildElement);
        }

        SAFE_RELEASE(pEnum);
    }

    return S_OK;
}

void  FontManager::OnNewChild(IUIElement* pElem)
{
    CComBSTR bstrTagName;
    pElem->GetTagName(&bstrTagName);

    //	加载所有属性
    IMapAttribute* pMapAttrib = NULL;
    pElem->GetAttribList(&pMapAttrib);

    String  strID;
    String  strFaceName;
    int   nFontHeight = 9;
    int   lfOrientation = 0;
    bool  bBold = false;
    bool  bItalic = false;
    bool  bUnderline = false;
    bool  bStrikeout = false;
    bool  bClearType = false;
    
    const TCHAR* szText = pMapAttrib->GetAttr(XML_ID, true);
    if (szText)
        strID = szText;

    szText = pMapAttrib->GetAttr(XML_FONT_FACENAME, true);
    if (szText)
        strFaceName = szText;

    if (!Util::IsFontExist(strFaceName.c_str()))
    {
        szText = pMapAttrib->GetAttr(XML_FONT_FACENAME2, true);
        if (szText)
            strFaceName = szText;
    }

    pMapAttrib->GetAttr_int(XML_FONT_HEIGHT, true, &nFontHeight);
    pMapAttrib->GetAttr_int(XML_FONT_ORIENTATION, true, &lfOrientation);
    pMapAttrib->GetAttr_bool(XML_FONT_BOLD, true, &bBold);
    pMapAttrib->GetAttr_bool(XML_FONT_ITALIC, true, &bItalic);
    pMapAttrib->GetAttr_bool(XML_FONT_UNDERLINE, true, &bUnderline);
    pMapAttrib->GetAttr_bool(XML_FONT_STRIKEOUT, true, &bStrikeout);
    pMapAttrib->GetAttr_bool(XML_FONT_CLEARTYPE, true, &bClearType);

    if (strFaceName.empty() )
        strFaceName = _T("SYSTEM");
    
    LOGFONT lf;
    ::ZeroMemory( &lf, sizeof(lf) );
    _tcsncpy(lf.lfFaceName, strFaceName.c_str(), 31);

    lf.lfHeight = Util::FontSize2Height(nFontHeight);
    lf.lfOrientation = lfOrientation;
    lf.lfEscapement = lf.lfOrientation;
    lf.lfWeight = bBold?FW_BOLD:FW_NORMAL;
    lf.lfItalic = bItalic?1:0;
    lf.lfUnderline = bUnderline?1:0;
    lf.lfStrikeOut = bStrikeout?1:0;
    lf.lfQuality = bClearType ? CLEARTYPE_QUALITY : DEFAULT_QUALITY;

    if (false == m_resFont.InsertFont(strID, &lf))
        UI_LOG_WARN( _T("insert font failed. m_strID=%s, facename=%s"), strID.c_str(), strFaceName.c_str() );

    SAFE_RELEASE(pMapAttrib);
}
