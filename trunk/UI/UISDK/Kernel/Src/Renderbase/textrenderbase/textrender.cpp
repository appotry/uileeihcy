#include "stdafx.h"
#include "textrender.h"

#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Inc\Interface\imapattr.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\Atl\image.h"

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                                  TextRender                                          //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


TextRenderBase::TextRenderBase()
{
    m_pITextRenderBase = NULL;
    m_pUIApplication = NULL;
	m_pObject = NULL;
	m_nTextRenderType = TEXTRENDER_TYPE_NULL;
	m_nDrawTextFlag = DT_SINGLELINE|DT_END_ELLIPSIS|DT_CENTER|DT_VCENTER|DT_NOPREFIX;
    m_eDrawTextEffect = TEXT_EFFECT_NONE;
    m_pColorTextBkgnd = NULL;
    m_wparamDrawText = m_lparamDrawText = 0;
}
TextRenderBase::~TextRenderBase()
{
    SAFE_RELEASE(m_pColorTextBkgnd);
}
void TextRenderBase::SetTextAlignment(int nDrawFlag)
{
	m_nDrawTextFlag = nDrawFlag;
}

void TextRenderBase::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

	String strAttr;
    String strPrefix;
    if (szPrefix)
        strPrefix = szPrefix;

    strAttr = strPrefix;
	strAttr.append(XML_TEXTRENDER_ALIGN);

    const TCHAR* szText = pMapAttrib->GetAttr(strAttr.c_str(), pData->bErase);
	if (szText)
	{
		vector<String> vecAlign;
		UI_Split(szText, XML_FLAG_SEPARATOR, vecAlign);

		m_nDrawTextFlag = 0;

		int nSize = (int)vecAlign.size();
		for (int i = 0; i < nSize; i++)
		{
			const String& strType = vecAlign[i];

			if (strType == XML_TEXTRENDER_ALIGN_LEFT)
			{
				m_nDrawTextFlag |= DT_LEFT;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_RIGHT )
			{
				m_nDrawTextFlag |= DT_RIGHT;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_CENTER )
			{
				m_nDrawTextFlag |= DT_CENTER;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_TOP )
			{
				m_nDrawTextFlag |= DT_TOP;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_BOTTOM )
			{
				m_nDrawTextFlag |= DT_BOTTOM;
			}
			else if( strType == XML_TEXTRENDER_ALIGN_VCENTER )
			{
				m_nDrawTextFlag |= DT_VCENTER|DT_SINGLELINE;
			}
			else 
			{
				UI_LOG_WARN(_T("%s unknow align type:%s"), FUNC_NAME, strType.c_str());
			}
		}
	}

    strAttr = strPrefix;
    strAttr.append(XML_TEXTRENDER_EFFECT);
    szText = pMapAttrib->GetAttr(strAttr.c_str(), pData->bErase);
    if (szText)
    {
        if (0 == _tcscmp(szText, XML_TEXTRENDER_EFFECT_NONE))
        {
            m_eDrawTextEffect = TEXT_EFFECT_NONE;
        }
        else if (0 == _tcscmp(szText, XML_TEXTRENDER_EFFECT_HALO))
        {
            m_eDrawTextEffect = TEXT_EFFECT_HALO;

            strAttr = strPrefix;
            strAttr.append(XML_TEXTRENDER_EFFECT_HALO_SHADOW_COLOR);
            szText = pMapAttrib->GetAttr(strAttr.c_str(), pData->bErase);
            if (m_pObject && szText)
            {
                IUIApplication* pUIApp = m_pObject->GetUIApplication();
                if (pUIApp)
                {
                    SAFE_RELEASE(m_pColorTextBkgnd);
                    pUIApp->GetActiveSkinColorRes()->GetColor(szText, &m_pColorTextBkgnd);
                }
            }
            m_wparamDrawText = 3; // 默认模糊半径
        }
    }
}

void  TextRenderBase::OnGetDesiredSize(SIZE* pSize, GETDESIREDSIZEINFO* pInfo)
{
    *pSize = GetDesiredSize(pInfo->szText, pInfo->nLimitWidth);
}

SIZE TextRenderBase::GetDesiredSize(const TCHAR* szText, int nLimitWidth)
{
    SIZE s = {0,0};
    IRenderFont* pFont = m_pITextRenderBase->GetRenderFont();
    if (pFont)
        s = pFont->MeasureString(szText, nLimitWidth);

    return s;
}

IColorRes*  TextRenderBase::GetActiveSkinColorRes()
{
    if (NULL == m_pUIApplication)
        return NULL;

	IColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
	UIASSERT(NULL != pColorRes);
	return pColorRes;
}

IFontRes*  TextRenderBase::GetActiveSkinFontRes()
{
    if (NULL == m_pUIApplication)
        return NULL;

	IFontRes* pFontRes = m_pUIApplication->GetActiveSkinFontRes();
	UIASSERT(NULL != pFontRes);
	return pFontRes;
}

bool  TextRenderBase::IsThemeRender() 
{ 
    return  m_nTextRenderType > TEXTRENDER_TYPE_THEME_FIRST && 
            m_nTextRenderType < TEXTRENDER_TYPE_THEME_LAST; 
}

void  TextRenderBase::CheckSkinMaterialChanged()
{
    if (m_nTextRenderType > TEXTRENDER_TYPE_CONTRAST_FIRST && m_nTextRenderType < TEXTRENDER_TYPE_CONTRAST_LAST)
    {
        UISendMessage(static_cast<IMessage*>(m_pITextRenderBase), UI_WM_SKINMATERIALCHANGED);
    }
}


NormalTextRender::NormalTextRender()
{
    m_pINormalTextRender = NULL;

	m_pColorText = NULL;
	m_hFont = NULL;
}
NormalTextRender::~NormalTextRender()
{
	SAFE_RELEASE(m_pColorText);
	SAFE_RELEASE(m_hFont);
}

// 如果字体指针为空，则取object对象的配置字体。
void NormalTextRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

	TextRenderBase::SetAttribute(pData);

	IFontRes*  pFontRes = GetActiveSkinFontRes();
	IColorRes* pColorRes = GetActiveSkinColorRes();
	
	if (NULL == pFontRes || NULL == pColorRes)
		return;

	// 颜色
    String strAttr;
    if (szPrefix)
        strAttr = szPrefix;
    strAttr.append(XML_TEXTRENDER_COLOR);

	const TCHAR* szText = pMapAttrib->GetAttr(strAttr.c_str(), pData->bErase);
	if (szText)
	{
		pColorRes->GetColor(szText, &this->m_pColorText);
	}

    if (szPrefix)
	    strAttr = szPrefix;
    else
        strAttr.clear();
	strAttr.append(XML_TEXTRENDER_FONT);

	szText = pMapAttrib->GetAttr(strAttr.c_str(), pData->bErase);
	if (szText)
	{
		pFontRes->GetFont((BSTR)szText, ::GetRenderLibraryType(m_pObject->GetIObject()), &m_hFont);
	}
	if (NULL == m_hFont)
	{
		IRenderFont* pFont = m_pObject->GetRenderFont();
		if (pFont)
		{
			m_hFont = m_pObject->GetRenderFont();
			m_hFont->AddRef();
		}
		else
		{
			// 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
			pFontRes->GetDefaultFont(GetRenderLibraryType(m_pObject->GetIObject()), &m_hFont);
		}
	}
}

void NormalTextRender::SetRenderFont(IRenderFont* pFont)
{
	SAFE_RELEASE(m_hFont);

	m_hFont = pFont;

	if (pFont)
		pFont->AddRef();
}

void NormalTextRender::SetColor(Color*  pColText)
{
	SAFE_RELEASE(m_pColorText);
    if (pColText)
        m_pColorText = Color::CreateInstance(pColText->m_col);
}

void NormalTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->ds_renderbase.pRenderTarget;
	if (NULL == pRenderTarget || NULL == pDrawStruct->szText)
		return ;

	if (m_hFont && _tcslen(pDrawStruct->szText) > 0)
	{
        DRAWTEXTPARAM  param;

        if (m_pColorText)
			param.color = m_pColorText->m_col;
        param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;
        param.prc = &pDrawStruct->ds_renderbase.rc;
        param.szText = pDrawStruct->szText;

        param.nEffectFlag = m_eDrawTextEffect;
        if (m_pColorTextBkgnd)
            param.bkcolor = *m_pColorTextBkgnd;
        param.wParam = m_wparamDrawText;
        param.lParam = m_lparamDrawText;
		pRenderTarget->DrawString(m_hFont, &param);
	}
}

//////////////////////////////////////////////////////////////////////////


ContrastColorTextRender::ContrastColorTextRender()
{
    m_pIContrastColorTextRender = NULL;

    m_pColorText = NULL;
    m_hFont = NULL;
    m_bNeedCalcColor = false;
}
ContrastColorTextRender::~ContrastColorTextRender()
{
    SAFE_RELEASE(m_hFont);
    SAFE_RELEASE(m_pColorText);
}

// 如果字体指针为空，则取object对象的配置字体。
void ContrastColorTextRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    TextRenderBase::SetAttribute(pData);

    IFontRes*  pFontRes = GetActiveSkinFontRes();
    IColorRes* pColorRes = GetActiveSkinColorRes();
    if (!pFontRes || !pColorRes)
        return;

    String  strAttr;
    if (szPrefix)
        strAttr = szPrefix;
    else
        strAttr.clear();
    strAttr.append(XML_TEXTRENDER_FONT);

    const TCHAR*  szText = pMapAttrib->GetAttr(strAttr.c_str(), pData->bErase);
    if (szText)
    {
        pFontRes->GetFont((BSTR)szText, ::GetRenderLibraryType(m_pObject->GetIObject()), &m_hFont);
    }
    if (NULL == m_hFont)
    {
        IRenderFont* pFont = m_pObject->GetRenderFont();
        if (pFont)
        {
            m_hFont = m_pObject->GetRenderFont();
            m_hFont->AddRef();
        }
        else
        {
            // 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
            pFontRes->GetDefaultFont(GetRenderLibraryType(m_pObject->GetIObject()), &m_hFont);
        }
    }

    // 颜色
    strAttr.clear();
    if (szPrefix)
        strAttr = szPrefix;
    strAttr.append(XML_TEXTRENDER_COLOR);

    szText = pMapAttrib->GetAttr(strAttr.c_str(), pData->bErase);
    if (szText)
    {
        pColorRes->GetColor(szText, &m_pColorText);
        // 如果没有配置背景色，则默认设置一个
        if (m_pColorText && !m_pColorTextBkgnd && m_eDrawTextEffect == TEXT_EFFECT_HALO)
        {
            if (m_pColorText->m_col == 0xFFFFFFFF)
            {
                m_pColorTextBkgnd = Color::CreateInstance(0xFF000000);
            }
            else
            {
                m_pColorTextBkgnd = Color::CreateInstance(0xFFFFFFFF);
            }
        }
    }

}

void ContrastColorTextRender::SetRenderFont(IRenderFont* pFont)
{
    SAFE_RELEASE(m_hFont);

    m_hFont = pFont;

    if (pFont)
        pFont->AddRef();
}

void ContrastColorTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (NULL == m_pObject)
        return;

    if (NULL == m_pColorText)
    {
        DWORD dwColor = m_pObject->CalcContrastTextColor();
        m_pColorText = Color::CreateInstance(dwColor);

        if (m_eDrawTextEffect == TEXT_EFFECT_HALO)
        {
            SAFE_RELEASE(m_pColorTextBkgnd);
            if (dwColor == 0xFFFFFFFF)
            {
                m_pColorTextBkgnd = Color::CreateInstance(0xFF000000);
            }
            else
            {
                m_pColorTextBkgnd = Color::CreateInstance(0xFFFFFFFF);
            }
        }
    }

    IRenderTarget* pRenderTarget = pDrawStruct->ds_renderbase.pRenderTarget;
    if (NULL == pRenderTarget || NULL == pDrawStruct->szText)
        return ;

    if (m_hFont && _tcslen(pDrawStruct->szText) > 0)
    {
        DRAWTEXTPARAM  param;
        param.szText = pDrawStruct->szText;
        param.prc = &pDrawStruct->ds_renderbase.rc;
        param.color = m_pColorText->m_col;
        param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;

        param.nEffectFlag = m_eDrawTextEffect;
        if (m_pColorTextBkgnd)
            param.bkcolor = *m_pColorTextBkgnd;
        param.wParam = m_wparamDrawText;
        param.lParam = m_lparamDrawText;
        pRenderTarget->DrawString(m_hFont, &param);
    }
}
LRESULT  ContrastColorTextRender::OnSkinMaterialChanged(UINT, WPARAM, LPARAM)
{
    SAFE_RELEASE(m_pColorText);
    return 0;
}

//////////////////////////////////////////////////////////////////////////

ContrastColorListTextRender::ContrastColorListTextRender()
{
    m_nCount = 0;
    m_hFont = NULL;
    m_pIContrastColorListTextRender = NULL;
}
ContrastColorListTextRender::~ContrastColorListTextRender()
{
    this->Clear();
    SAFE_RELEASE(m_hFont);
}
void  ContrastColorListTextRender::Clear()
{
    for (int i = 0; i < m_nCount; i++)
    {
        SAFE_RELEASE(m_vTextColor[i]);
    }

    m_vTextColor.clear();
    m_nCount = 0;
}


LRESULT  ContrastColorListTextRender::OnSkinMaterialChanged(UINT, WPARAM, LPARAM)
{
    for (int i = 0; i < m_nCount; i++)
    {
        SAFE_RELEASE(m_vTextColor[i]);
        m_vTextColor[i] = NULL;
    }

    return 0;
}
void ContrastColorListTextRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    TextRenderBase::SetAttribute(pData);

    IFontRes*  pFontRes = GetActiveSkinFontRes();
    if (NULL == pFontRes)
        return;

    String strAttrib;
    strAttrib = szPrefix ? szPrefix:_T("");
    strAttrib.append(XML_TEXTRENDER_COLORLIST_COUNT);

    const TCHAR* szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
    if (szText)
        this->SetCount( _ttoi(szText) );

    strAttrib = szPrefix ? szPrefix:_T("");
    strAttrib.append(XML_TEXTRENDER_FONT);

    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
    if (szText)
    {
        pFontRes->GetFont((BSTR)szText, ::GetRenderLibraryType(m_pObject->GetHWND()), &m_hFont);
    }

    if (NULL == m_hFont)
    {
        IRenderFont* pFont = m_pObject->GetRenderFont();
        if (pFont)
        {
            m_hFont = m_pObject->GetRenderFont();
            if (m_hFont)
                m_hFont->AddRef();
        }
        else
        {
            // 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
            pFontRes->GetDefaultFont(GetRenderLibraryType(m_pObject->GetIObject()), &m_hFont);
        }
    }
}
void ContrastColorListTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (0 == m_nCount || NULL == m_pObject)
        return;

    int nRealState = LOWORD(pDrawStruct->ds_renderbase.nState);
    if (nRealState >= m_nCount)
        nRealState = 0;


    if (NULL  == m_vTextColor[nRealState])
        m_vTextColor[nRealState] = Color::CreateInstance(m_pObject->CalcContrastTextColor());

    COLORREF col = RGB(0,0,0);
    

    DRAWTEXTPARAM  param;

    param.prc = &pDrawStruct->ds_renderbase.rc;
    param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;
    if (m_vTextColor[nRealState])
        param.color = m_vTextColor[nRealState]->m_col;
    param.szText = pDrawStruct->szText;
    pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_hFont, &param);
}
IRenderFont* ContrastColorListTextRender::GetRenderFont()
{
    return m_hFont;
}

void ContrastColorListTextRender::SetRenderFont(IRenderFont* pFont)
{
    SAFE_RELEASE(m_hFont);
    m_hFont = pFont;

    if (pFont)
        pFont->AddRef();
}

void ContrastColorListTextRender::SetCount( int nCount )
{
    this->Clear();
    m_nCount = nCount;

    for (int i = 0; i < m_nCount; i++)
    {
        m_vTextColor.push_back(NULL);
    }
}

//////////////////////////////////////////////////////////////////////////
ColorListTextRender::ColorListTextRender()
{
	m_nCount = 0;
	m_hFont = NULL;
    m_pIColorListTextRender = NULL;
}
ColorListTextRender::~ColorListTextRender()
{
	this->Clear();
	SAFE_RELEASE(m_hFont);
}
void ColorListTextRender::Clear()
{
	for (int i = 0; i < m_nCount; i++)
	{
		SAFE_RELEASE(m_vTextColor[i]);
	}
	m_vTextColor.clear();
	m_nCount = 0;
}


void ColorListTextRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

	TextRenderBase::SetAttribute(pData);

	IColorRes* pColorRes = GetActiveSkinColorRes();
	IFontRes*  pFontRes = GetActiveSkinFontRes();

	if (NULL == pColorRes || NULL == pFontRes)
		return;

	String strAttrib;
    strAttrib = szPrefix ? szPrefix:_T("");
	strAttrib.append(XML_TEXTRENDER_COLORLIST_COUNT);

	const TCHAR* szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
		this->SetCount( _ttoi(szText) );

    strAttrib = szPrefix ? szPrefix:_T("");
	strAttrib.append(XML_TEXTRENDER_COLOR);

	szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
	{
		vector<String> vColors;
		UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
		int nCount = (int)vColors.size();

		if (0 == m_nCount)
		{
			this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小
		}

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if (!vColors[i].empty())
			{
				SAFE_RELEASE(m_vTextColor[i]);
				pColorRes->GetColor((BSTR)vColors[i].c_str(), &m_vTextColor[i]);
			}
		}
	}

    strAttrib = szPrefix ? szPrefix:_T("");
    strAttrib.append(XML_TEXTRENDER_FONT);

    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
	{
		pFontRes->GetFont((BSTR)szText, ::GetRenderLibraryType(m_pObject->GetHWND()), &m_hFont);
	}

	if (NULL == m_hFont)
	{
		IRenderFont* pFont = m_pObject->GetRenderFont();
		if (pFont)
		{
			m_hFont = m_pObject->GetRenderFont();
			if (m_hFont)
				m_hFont->AddRef();
		}
		else
		{
			// 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
			pFontRes->GetDefaultFont(GetRenderLibraryType(m_pObject->GetIObject()), &m_hFont);
		}
	}
}
void ColorListTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (0 == m_nCount)
        return;

	int nRealState = LOWORD(pDrawStruct->ds_renderbase.nState);
	if (nRealState >= m_nCount)
		nRealState = 0;

    DRAWTEXTPARAM  param;
	if (m_vTextColor[nRealState])
		param.color = m_vTextColor[nRealState]->m_col;
	param.prc = &pDrawStruct->ds_renderbase.rc;
	param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;
    param.szText = pDrawStruct->szText;
	pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_hFont, &param);
}
IRenderFont* ColorListTextRender::GetRenderFont()
{
	return m_hFont;
}

void ColorListTextRender::SetRenderFont(IRenderFont* pFont)
{
	SAFE_RELEASE(m_hFont);
	m_hFont = pFont;

	if (pFont)
		pFont->AddRef();
}

void ColorListTextRender::SetCount( int nCount )
{
	this->Clear();
	m_nCount = nCount;

	for (int i = 0; i < m_nCount; i++)
	{
		m_vTextColor.push_back(NULL);
	}
}
void ColorListTextRender::SetColor(int nIndex, COLORREF col)
{
	nIndex = LOWORD(nIndex);
	if (nIndex >= m_nCount)
		return;

	if (m_vTextColor[nIndex])
	{
		SAFE_RELEASE(m_vTextColor[nIndex]);
	}
    m_vTextColor[nIndex] = Color::CreateInstance(col);
}

FontColorListTextRender::FontColorListTextRender()
{
	m_nCount = 0;
    m_pIFontColorListTextRender = NULL;
}
FontColorListTextRender::~FontColorListTextRender()
{
	this->Clear();
}
void FontColorListTextRender::Clear()
{
	for (int i = 0; i < m_nCount; i++)
	{
		SAFE_RELEASE(m_vTextColor[i]);
		SAFE_RELEASE(m_vTextFont[i]);
	}
	m_vTextFont.clear();
	m_vTextColor.clear();
	m_nCount = 0;
}

void FontColorListTextRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

	__super::SetAttribute(pData);

	IColorRes* pColorRes = GetActiveSkinColorRes();
	IFontRes*  pFontRes = GetActiveSkinFontRes();

	if (NULL == pColorRes || NULL == pFontRes)
		return;

	String strAttrib;
    strAttrib = szPrefix ? szPrefix:_T("");
	strAttrib.append(XML_TEXTRENDER_FONTCOLORLIST_COUNT);

	const TCHAR* szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
		this->SetCount( _ttoi(szText) );

    strAttrib = szPrefix ? szPrefix:_T("");
	strAttrib.append(XML_TEXTRENDER_COLOR);

	szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
	{
		vector<String> vColors;
		UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
		int nCount = (int)vColors.size();

        if (0 == m_nCount)
            this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vColors[i].empty() )
			{
				pColorRes->GetColor((BSTR)vColors[i].c_str(), &m_vTextColor[i]);
			}
		}
	}

    strAttrib = szPrefix ? szPrefix:_T("");
	strAttrib.append(XML_TEXTRENDER_FONT);

    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
	{
		vector<String> vFont;
		UI_Split(szText, XML_MULTI_SEPARATOR, vFont);
		int nCount = (int)vFont.size();

        if (0 == m_nCount)
            this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

		for(int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if(! vFont[i].empty() )
			{
				pFontRes->GetFont((BSTR)vFont[i].c_str(), ::GetRenderLibraryType(m_pObject->GetHWND()), &m_vTextFont[i]);
			}
		}
	}

	for(int i = 0; i < m_nCount; i++ )
	{
		if(NULL == m_vTextFont[i])
		{
			m_vTextFont[i] = m_pObject->GetRenderFont();
			if (m_vTextFont[i])
				m_vTextFont[i]->AddRef();
		}
	}
}
void FontColorListTextRender::DrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct)
{
    if (0 == m_nCount)
        return;

	int nRealState = LOWORD(pDrawStruct->ds_renderbase.nState);
	if (nRealState >= m_nCount)
		nRealState = 0;

    DRAWTEXTPARAM  param;
	if (m_vTextColor[nRealState])
		param.color = *m_vTextColor[nRealState];

	param.nFormatFlag = pDrawStruct->nDrawTextFlag==-1 ? m_nDrawTextFlag:pDrawStruct->nDrawTextFlag;
    param.szText = pDrawStruct->szText;
    param.prc = &(pDrawStruct->ds_renderbase.rc);
	pDrawStruct->ds_renderbase.pRenderTarget->DrawString(m_vTextFont[nRealState], &param);
}
IRenderFont* FontColorListTextRender::GetRenderFont()
{
	if (m_vTextFont.size() > 0)
		return m_vTextFont[0];
	
	return NULL;
}

void FontColorListTextRender::SetRenderFont(IRenderFont* pRenderFont)
{
	if (m_vTextFont.size() > 0)
	{
		SAFE_RELEASE(m_vTextFont[0]);
		m_vTextFont[0] = pRenderFont;

		if (pRenderFont)
			pRenderFont->AddRef();
	}
}

void FontColorListTextRender::SetCount(int nCount)
{
	this->Clear();
	m_nCount = nCount;

	for (int i = 0; i < m_nCount; i++)
	{
		m_vTextColor.push_back(NULL);
		m_vTextFont.push_back(NULL);
	}
}
void FontColorListTextRender::SetColor(int nIndex, DWORD col)
{
	nIndex = LOWORD(nIndex);
	if (nIndex >= m_nCount)
		return;

	if (m_vTextColor[nIndex])
	{
		SAFE_RELEASE(m_vTextColor[nIndex]);
	}
    m_vTextColor[nIndex] = Color::CreateInstance(col);
}
void FontColorListTextRender::SetFont(int nIndex, IRenderFont* pFont)
{
	if (nIndex >= m_nCount)
		return;

	SAFE_RELEASE(m_vTextFont[nIndex]);

	m_vTextFont[nIndex] = pFont;

	if (pFont)
		pFont->AddRef();
}
//////////////////////////////////////////////////////////////////////////

ThemeTextRenderBase::ThemeTextRenderBase()
{
    m_pIThemeTextRenderBase = NULL;
	m_hTheme = NULL;
    m_bNoTheme = false;
}
ThemeTextRenderBase::~ThemeTextRenderBase()
{
	if (m_hTheme)
	{
		::CloseThemeData( m_hTheme );
		m_hTheme = NULL;
	}
}
void ThemeTextRenderBase::OnInit()
{
	this->CreateTheme();
}
void ThemeTextRenderBase::OnThemeChanged()
{
	this->CreateTheme();
}
void ThemeTextRenderBase::CreateTheme()
{
    if (m_hTheme)
    {
        ::CloseThemeData(m_hTheme);
        m_hTheme = NULL;
    }
    if (!m_bNoTheme)
    {
        const TCHAR* pThemeName = m_pIThemeTextRenderBase->GetThemeName();
        if (pThemeName)
        {
            m_hTheme = ::OpenThemeData(m_pObject->GetHWND(), pThemeName);
        }
    }
}

void ThemeTextRenderBase::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    m_pITextRenderBase->nvProcessMessage(GetCurMsg(),0,false);

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
