#include "stdafx.h"
#include "colorrender.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               ColorRender                                            //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


void  ColorBorderRender_DrawState(RENDERBASE_DRAWSTATE* pDrawStruct, Object* pObj, COLORREF color, const CRegion4& rBorder)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
    if (NULL == pRenderTarget)
        return ;

    // 绘制边框，由于直接调用Rectangle创建指定宽度的PEN后进行绘制，会导致PEN的
    // 一半区域位于控件剪裁区域外面。因此在这里用FillRect来实现边框的绘制。
    // 同时也支持了当各个边框大小不一致时的绘制

#if 0
    if (NULL == pObj)
        return;

    CRegion4 rBorder;
    pObj->GetBorderRegion(&rBorder);
#ifdef _DEBUG
    if (rBorder.left==0 && rBorder.right==0 && rBorder.top==0 && rBorder.bottom==0)
    {
        UI_LOG_WARN(_T("%s 指定了border color，但没有配置border"), FUNC_NAME);
    }
#endif
#endif

    Color c(color);
    c.a = 255;
    if (0 != rBorder.left)
    {
        CRect rcLeft = pDrawStruct->rc;
        rcLeft.right = rBorder.left;
        
        pRenderTarget->FillRect(&rcLeft, &c);
    }
    if (0 != rBorder.top)
    {
        CRect rcTop = pDrawStruct->rc;
        rcTop.bottom = rBorder.top;
        pRenderTarget->FillRect(&rcTop, &c);
    }
    if (0 != rBorder.right)
    {
        CRect rcRight = pDrawStruct->rc;
        rcRight.left = rcRight.right-rBorder.right;
        pRenderTarget->FillRect(&rcRight, &c);
    }
    if (0 != rBorder.bottom)
    {
        CRect rcBottom = pDrawStruct->rc;
        rcBottom.top = rcBottom.bottom - rBorder.bottom;
        pRenderTarget->FillRect(&rcBottom, &c);
    }
}

//////////////////////////////////////////////////////////////////////////

ColorRender::ColorRender()
{
    m_pIColorRender = NULL;
	m_pBkColor = NULL;
    m_pBorderColor = NULL;
    m_rcBorder.SetRectEmpty();
}
ColorRender::~ColorRender()
{
	SAFE_RELEASE(m_pBkColor);
    SAFE_RELEASE(m_pBorderColor);
}

void ColorRender::SetBkColor( COLORREF col )
{
	SAFE_RELEASE(m_pBkColor);
    m_pBkColor = Color::CreateInstance(col);
}
void ColorRender::SetBorderColor(COLORREF col)
{
    SAFE_RELEASE(m_pBorderColor);
    m_pBorderColor = Color::CreateInstance(col);
}
void  ColorRender::SetBorderRegion(const CRegion4* prc)
{
    if (NULL == prc)
    {
        m_rcBorder.SetRectEmpty();
    }
    else
    {
        m_rcBorder.CopyRect(prc);
    }
}

void ColorRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    m_pIRenderBase->nvProcessMessage(GetCurMsg(),0,false);

	IColorRes* pColorRes = GetActiveSkinColorRes();
	if (NULL == pColorRes)
		return;

    String strPrefix;
    if (szPrefix)
        strPrefix = szPrefix;

	String strAttrib;
    strAttrib = strPrefix;
	strAttrib.append(XML_RENDER_COLOR);

	const TCHAR* szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
		pColorRes->GetColor((BSTR)szText, &m_pBkColor);

    strAttrib.clear();
    if (szPrefix)
        strAttrib = szPrefix;
    strAttrib.append(XML_RENDER_BORDERCOLOR);
    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
    if (szText)
        pColorRes->GetColor((BSTR)szText, &m_pBorderColor);

    strAttrib.clear();
    if (szPrefix)
        strAttrib = szPrefix;
    strAttrib.append(XML_RENDER_BORDER);
    pMapAttrib->GetAttr_CRegion4(strAttrib.c_str(), pData->bErase, &m_rcBorder);
} 

void  ColorRender::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_COLOR, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_BORDERCOLOR, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_BORDER, szPrefix);
}


void ColorRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
	if (NULL == pRenderTarget)
		return ;

	if (m_pBkColor)
    {
		pRenderTarget->FillRect(&pDrawStruct->rc, m_pBkColor);
    }

    if (m_pBorderColor)
    {
        ColorBorderRender_DrawState(pDrawStruct, m_pObject, m_pBorderColor->m_col, &m_rcBorder);
    }
}



//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               SysColorRender                                         //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

SysColorRender::SysColorRender()
{
    m_pISysColorRender = NULL;

	m_nBkColorIndex = -1;
	m_nBorderColorIndex = -1;
    m_rcBorder.SetRectEmpty();

	// 
	// 	m_bkColor = 0;
	// 	m_borderColor = 0;
}
SysColorRender::~SysColorRender()
{

}

void SysColorRender::SetBkColor(int nColorIndex)
{
	m_nBkColorIndex = nColorIndex;
	// 	if (m_nBkColorIndex != -1)
	// 	{
	// 		m_bkColor = ::GetSysColor(m_nBkColorIndex);
	// 	}
}
void SysColorRender::SetBorderColor(int nColorIndex)
{
	m_nBorderColorIndex = nColorIndex;
	// 	if (m_nBorderColorIndex != -1)
	// 	{
	// 		m_borderColor = ::GetSysColor(m_nBorderColorIndex);
	// 	}
}

// void SysColorRender::OnThemeChanged()
// {
// 	UI_LOG_DEBUG(_T("%s,1. %d"), FUNC_NAME, m_bkColor);
// 	if (m_nBkColorIndex != -1)
// 	{
// 		UI_LOG_DEBUG(_T("XXXX"));
// 		Sleep(100); // 1250, OK, 100, 50 10 BAD
// 		m_bkColor = ::GetSysColor(m_nBkColorIndex);
// 	}
// 	if (m_nBorderColorIndex != -1)
// 	{
// 		UI_LOG_DEBUG(_T("DDDDD"));
// 		m_borderColor = ::GetSysColor(m_nBorderColorIndex);
// 	}
// 	UI_LOG_DEBUG(_T("%s,2. %d"), FUNC_NAME, m_bkColor);
// }

void SysColorRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    m_pIRenderBase->nvProcessMessage(GetCurMsg(),0,false);

    String strAttrib;

    strAttrib = szPrefix ? szPrefix : _T("");
    strAttrib.append(XML_RENDER_COLOR);
    pMapAttrib->GetAttr_int(strAttrib.c_str(), pData->bErase, &m_nBkColorIndex);

    strAttrib = szPrefix ? szPrefix : _T("");
    strAttrib.append(XML_RENDER_BORDERCOLOR);
    pMapAttrib->GetAttr_int(strAttrib.c_str(), pData->bErase, &m_nBorderColorIndex);

    strAttrib = szPrefix ? szPrefix : _T("");
    strAttrib.append(XML_RENDER_BORDER);
    pMapAttrib->GetAttr_CRegion4(strAttrib.c_str(), pData->bErase, &m_rcBorder);
}

void  SysColorRender::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_COLOR, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_BORDERCOLOR, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_BORDER, szPrefix);
}


void SysColorRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
	if (NULL == pRenderTarget)
		return ;

	if (-1 != m_nBkColorIndex)
    {
        Color c(GetSysColor(m_nBkColorIndex));
        c.a = 255;

		pRenderTarget->FillRect(&pDrawStruct->rc, &c);
    }

	// 绘制边框，由于直接调用Rectangle创建指定宽度的PEN后进行绘制，会导致PEN的
	// 一半区域位于控件剪裁区域外面。因此在这里用FillRect来实现边框的绘制。
	// 同时也支持了当各个边框大小不一致时的绘制

	if (-1 == m_nBorderColorIndex)
		return;

	COLORREF colorborder = GetSysColor(m_nBorderColorIndex)|0xff000000;
    ColorBorderRender_DrawState(pDrawStruct, m_pObject, colorborder, &m_rcBorder);

// 	CRegion4 rBorder;
// 	m_pObject->GetBorderRegion(&rBorder);
// 	if (0 != rBorder.left)
// 	{
// 		CRect rcLeft = *prc;
// 		rcLeft.right = rBorder.left;
// 		pRenderTarget->FillRect(&rcLeft, colorborder);
// 	}
// 	if (0 != rBorder.top)
// 	{
// 		CRect rcTop = *prc;
// 		rcTop.bottom = rBorder.top;
// 		pRenderTarget->FillRect(&rcTop, colorborder);
// 	}
// 	if (0 != rBorder.right)
// 	{
// 		CRect rcRight = *prc;
// 		rcRight.left = rcRight.right-rBorder.right;
// 		pRenderTarget->FillRect(&rcRight, colorborder);
// 	}
// 	if (0 != rBorder.bottom)
// 	{
// 		CRect rcBottom = *prc;
// 		rcBottom.top = rcBottom.bottom - rBorder.bottom;
// 		pRenderTarget->FillRect(&rcBottom, colorborder);
// 	}
}

//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               GradientRender                                  //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////


GradientRender::GradientRender()
{
    m_pIGradientRender = NULL;
    m_pBorderColor = NULL;
    m_rcBorder.SetRectEmpty();

	m_pColorFrom = NULL;
	m_pColorTo = NULL;
    m_nGradientDir = RENDER_COLOR_GRADIENT_DIR_H;
}
GradientRender::~GradientRender()
{
	SAFE_RELEASE(m_pColorFrom);
	SAFE_RELEASE(m_pColorTo);
    SAFE_RELEASE(m_pBorderColor);
}

void GradientRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    m_pIRenderBase->nvProcessMessage(GetCurMsg(),0,false);

	IColorRes* pColorRes = GetActiveSkinColorRes();
	if (NULL == pColorRes)
		return;

	String strAttrib; 

    strAttrib = szPrefix ? szPrefix:_T("");
	strAttrib.append(XML_RENDER_COLORFROM);
	const TCHAR* szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
		pColorRes->GetColor((BSTR)szText, &m_pColorFrom);

	strAttrib = szPrefix ? szPrefix:_T("");
	strAttrib.append(XML_RENDER_COLORTO);
	szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
		pColorRes->GetColor((BSTR)szText, &m_pColorTo);

    strAttrib = szPrefix ? szPrefix:_T("");
    strAttrib.append(XML_RENDER_COLOR_GRADIENT_DIR);
    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
    if (szText)
    {
        if (0 == _tcscmp(szText, XML_RENDER_COLOR_GRADIENT_DIR_H))
            m_nGradientDir = RENDER_COLOR_GRADIENT_DIR_H;
        else if (0 == _tcscmp(szText, XML_RENDER_COLOR_GRADIENT_DIR_V))
            m_nGradientDir = RENDER_COLOR_GRADIENT_DIR_V;
    }

    strAttrib = szPrefix ? szPrefix : _T("");
    strAttrib.append(XML_RENDER_BORDERCOLOR);
    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
    if (szText)
        pColorRes->GetColor((BSTR)szText, &m_pBorderColor);

    strAttrib = szPrefix ? szPrefix : _T("");
    strAttrib.append(XML_RENDER_BORDER);
    pMapAttrib->GetAttr_CRegion4(strAttrib.c_str(), pData->bErase, &m_rcBorder);
} 

void  GradientRender::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_COLORFROM, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_COLORTO, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_COLOR_GRADIENT_DIR, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_BORDERCOLOR, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_BORDER, szPrefix);
}

void GradientRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
	if(NULL == pRenderTarget || NULL == m_pColorFrom || NULL == m_pColorTo)
		return;

	CRect rc(0,0, this->m_pObject->GetWidth(), this->m_pObject->GetHeight());

	if (m_nGradientDir == RENDER_COLOR_GRADIENT_DIR_H)
	{
		pRenderTarget->GradientFillH(&pDrawStruct->rc, m_pColorFrom->m_col, m_pColorTo->m_col);
	}
	else if (m_nGradientDir == RENDER_COLOR_GRADIENT_DIR_V)
	{
		pRenderTarget->GradientFillV(&pDrawStruct->rc, m_pColorFrom->m_col, m_pColorTo->m_col);
	}

    if (m_pBorderColor)
    {
        ColorBorderRender_DrawState(pDrawStruct, m_pObject, m_pBorderColor->m_col, &m_rcBorder);
    }
}



//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               ColorListRender                                        //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

ColorListRender::ColorListRender( )
{
    m_pIColorListRender = NULL;
	m_nCount = 0;
}

ColorListRender::~ColorListRender()
{
	this->Clear();
}
void  ColorListRender::Clear()
{
	for (int i = 0; i < m_nCount; i++)
	{
		SAFE_RELEASE(m_vBkColor[i]);
		SAFE_RELEASE(m_vBorderColor[i]);
	}
	m_vBkColor.clear();
	m_vBorderColor.clear();
}

void ColorListRender::SetStateColor(int nState, COLORREF colorBk, bool bSetBk, COLORREF colBorder, bool bSetBorder)
{
	nState = LOWORD(nState);
	if (m_nCount <= nState)
	{
		return;
	}

	if (bSetBk)
	{
		SAFE_RELEASE(m_vBkColor[nState]);
        m_vBkColor[nState] = Color::CreateInstance(colorBk);
	}
	if (bSetBorder)
	{
		SAFE_RELEASE(m_vBorderColor[nState]);
        m_vBorderColor[nState] = Color::CreateInstance(colBorder);
	}
}

void  ColorListRender::SetCount(int n)
{
	if (n < 0)
		return;

	this->Clear();

	for(int i = 0; i < n; i++ )
	{
		m_vBkColor.push_back((Color*)NULL);
		m_vBorderColor.push_back((Color*)NULL);
	}
	m_nCount = n;
}
void ColorListRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    m_pIRenderBase->nvProcessMessage(GetCurMsg(),0,false);

	IColorRes* pColorRes = GetActiveSkinColorRes();
	if (NULL == pColorRes)
		return;

	String strAttrib; 

    strAttrib = szPrefix ? szPrefix:_T("");
    strAttrib.append(XML_RENDER_COLORLIST_COUNT);

    int nCount = 0;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_int(strAttrib.c_str(), pData->bErase, &nCount))
	    this->SetCount(nCount);

    strAttrib = szPrefix ? szPrefix:_T("");
    strAttrib.append(XML_RENDER_COLOR);
    const TCHAR* szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
	{
		vector<String> vColors;
		UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
		int nCount = (int)vColors.size();

        if (0 == m_nCount)
            this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

		for (int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if (! vColors[i].empty())
			{
				pColorRes->GetColor((BSTR)vColors[i].c_str(), &m_vBkColor[i]);
			}
		}
	}

    strAttrib = szPrefix ? szPrefix:_T("");
    strAttrib.append(XML_RENDER_BORDERCOLOR);
    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
	{
		vector<String> vColors;
		UI_Split(szText, XML_MULTI_SEPARATOR, vColors);
		int nCount = (int)vColors.size();

        if (0 == m_nCount)
            this->SetCount(nCount); //  如果未显示指定count，则自动取这里的大小

		for (int i = 0; i < m_nCount && i < nCount; i++ )
		{
			if (! vColors[i].empty())
			{
				pColorRes->GetColor((BSTR)vColors[i].c_str(), &m_vBorderColor[i]);
			}
		}
	}
}

void  ColorListRender::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_COLORLIST_COUNT, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_COLOR, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_BORDERCOLOR, szPrefix);
}

void ColorListRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    IRenderTarget* pRenderTarget = pDrawStruct->pRenderTarget;
	if (NULL == pRenderTarget)
		return;
    
    if (0 == m_nCount)
        return;

	int nRealState = LOWORD(pDrawStruct->nState);
	if (nRealState >= m_nCount)
		nRealState = 0;

	if (NULL == m_vBorderColor[nRealState])   // 不绘制边框
	{
		if (m_vBkColor[nRealState])
		{
			pRenderTarget->FillRect(&pDrawStruct->rc, m_vBkColor[nRealState]);
		}
	}
	else                           // 绘制边框
	{
		if (m_vBkColor[nRealState])
			pRenderTarget->Rectangle(&pDrawStruct->rc, m_vBorderColor[nRealState], m_vBkColor[nRealState], 1, false);
		else
			pRenderTarget->Rectangle(&pDrawStruct->rc, m_vBorderColor[nRealState], 0,1,true);
	}
}
