#include "stdafx.h"
#include "colorres.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Src\Atl\image.h"

namespace UI
{

//////////////////////////////////////////////////////////////////////////
//
//	ColorResItem
//

ColorResItem::ColorResItem()
{
    m_pColor = NULL;
	m_bUseSkinHLS = true;
	m_pOriginColorValue = NULL;
    m_pIColorResItem = NULL;
}
ColorResItem::~ColorResItem()
{
	SAFE_DELETE(m_pOriginColorValue);
    SAFE_RELEASE(m_pColor);
    SAFE_DELETE(m_pIColorResItem);
}
IColorResItem*  ColorResItem::GetIColorResItem()
{
    if (NULL == m_pIColorResItem)
        m_pIColorResItem = new IColorResItem(this);

    return m_pIColorResItem;
}
const TCHAR* ColorResItem::GetColorString()
{
	return m_strColor.c_str();
}
const String& ColorResItem::GetColorStringRef()
{ 
	return m_strColor; 
}

bool ColorResItem::GetColor(Color** ppColor, bool* pbFirstTimeCreate) 
{
	if (NULL == ppColor)
		return false;

    if (NULL == m_pColor)
    {
        m_pColor = Color::CreateInstance(Util::TranslateRGB(m_strColor.c_str(), XML_SEPARATOR));
        
        if (pbFirstTimeCreate)
            *pbFirstTimeCreate = true;
        
    }

    m_pColor->AddRef();
	*ppColor = m_pColor;
	return  true;
}

void ColorResItem::SetColor(const String& strColor) 
{
	m_strColor = strColor;

	if (m_pColor)
		m_pColor->ReplaceRGB(Util::TranslateRGB(m_strColor.c_str(), XML_SEPARATOR));
}

void ColorResItem::SetAttribute(IMapAttribute* pMapAttrib)
{
	bool bUseSkinHLS = true;
    pMapAttrib->GetAttr_bool(XML_COLOR_USESKINHLS, true, &bUseSkinHLS);
	this->SetUseSkinHLS(bUseSkinHLS);
}

bool ColorResItem::ModifyHLS( short h, short l, short s, int nFlag)
{
	if (false == m_bUseSkinHLS)
		return true;

	if (m_pColor)
	{
		if (NULL == m_pOriginColorValue )
		{
			m_pOriginColorValue = new COLORREF;
			*m_pOriginColorValue = m_pColor->m_col;
		}

		BYTE R = GetRValue(*m_pOriginColorValue);
		BYTE G = GetGValue(*m_pOriginColorValue);
		BYTE B = GetBValue(*m_pOriginColorValue);

		if (ChangeColorHLS(R,G,B,h,l,s,nFlag))
			m_pColor->ReplaceRGB(RGB(R,G,B));
	}
	return true;
}

ColorRes::ColorRes(SkinRes* p)
{
    m_pSkinRes = p; 
    m_pIColorRes = NULL; 
}

ColorRes::~ColorRes()
{
	this->Clear();
    SAFE_DELETE(m_pIColorRes);
}

IColorRes*  ColorRes::GetIColorRes()
{
    if (NULL == m_pIColorRes)
        m_pIColorRes = new IColorRes(this);

    return m_pIColorRes;
}
long ColorRes::GetColorCount() 
{
	return (long)m_vColors.size();
}

IColorResItem*  ColorRes::GetColorResItem(long lIndex)
{
	ColorResItem* pItem = GetColorItem(lIndex);
	if (NULL == pItem)
		return NULL;

	return pItem->GetIColorResItem();
}
ColorResItem* ColorRes::GetColorItem(int nIndex)
{
	if (nIndex < 0)
		return NULL;
	if (nIndex >= (int)m_vColors.size() )
		return NULL;

	return m_vColors[nIndex];
}

ColorResItem* ColorRes::GetColorItem( const String& strID )
{
	vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

	for (; iter != iterEnd; iter++)
	{
		ColorResItem* p = *iter;
		if (p->GetIDRef() == strID )
		{
			return p;
		}
	}
	return NULL;
}

void ColorRes::GetColor(const TCHAR* szColorId, Color** pp)
{
	if (NULL == szColorId)
		return;

	ColorResItem* pItem = this->GetColorItem(szColorId);
	if (NULL == pItem)
	{
		UI_LOG_ERROR( _T("%s failed, id=%s"), FUNC_NAME, szColorId);
		return;
	}

	bool bFirstTimeCreate = false;
    pItem->GetColor(pp, &bFirstTimeCreate);
	if (NULL == pp)
		return;

	if (bFirstTimeCreate && pItem->GetUseSkinHLS())
	{
        // 检查当前皮肤的HLS
        if (m_pSkinRes && m_pSkinRes->GetHLSInfo())
        {
            SKIN_HLS_INFO* pHLSInfo = m_pSkinRes->GetHLSInfo();
            pItem->ModifyHLS(pHLSInfo->h, pHLSInfo->l, pHLSInfo->s, pHLSInfo->nFlag);
        }
	}
}


//
// 从文件中加载一项(由CXmlImageParse::load_from_file中调用)
//
bool ColorRes::LoadItem(IMapAttribute* pMapAttrib, const TCHAR* szValue)
{
	String strID;

    const TCHAR* szText = pMapAttrib->GetAttr(XML_ID, true);
	if (szText)
		strID = szText;

	ColorResItem* pItem = NULL;
	if (this->InsertColor(strID, szValue, &pItem))
	{
		pItem->SetAttribute(pMapAttrib);
		return true;
	}
	else
	{
		UI_LOG_WARN( _T("%s insert image m_strID=%s, path=%s failed."), FUNC_NAME, strID.c_str(), szValue);
		return false;
	}
}
bool ColorRes::InsertColor(const String& strID, const TCHAR* szColor, ColorResItem** pItem )
{
    if (NULL == szColor)
        return false;

	ColorResItem* p = this->GetColorItem(strID);
	if (p)
	{
		UI_LOG_WARN(_T("ColorRes::InsertColor failed, insert item=%s, color=%s"), strID.c_str(), szColor);
		return false;
	}

	ColorResItem*  pColorItem = new ColorResItem;
	pColorItem->SetID(strID);
	pColorItem->SetColor(szColor);

	this->m_vColors.push_back(pColorItem); 

	if (pItem)
		*pItem = pColorItem;
	
	return true;
}

bool ColorRes::ModifyColor( const String& strID, const String& strColor )
{
	ColorResItem* p = this->GetColorItem(strID);
	if (p)
	{
		p->SetColor(strColor);
		return true;
	}

	return false;
}

bool ColorRes::RemoveColor(const String& strID )
{
	vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		ColorResItem* p = *iter;
		if (p->GetIDRef() == strID )
		{
			delete p;
			p = NULL;
			m_vColors.erase(iter);
			return true;
		}
	}

	return false;
}
void ColorRes::Clear()
{
	vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		ColorResItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vColors.clear();
}

bool ColorRes::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	vector<ColorResItem*>::iterator  iter = m_vColors.begin();
	vector<ColorResItem*>::iterator  iterEnd = m_vColors.end();

	for( ; iter != iterEnd; iter++ )
	{
		ColorResItem* p = *iter;
		p->ModifyHLS(h,l,s,nFlag);
	}

	return true;
}

}