#include "StdAfx.h"
#include "colormanager.h"

#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             颜色                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////



ColorManager::ColorManager(SkinRes* pSkinRes): m_resColor(pSkinRes)
{
    m_pIColorManager = NULL;
    m_pUIElement = NULL;
#if 0
	m_pColorParse = NULL;
#endif
	m_pSkinRes = pSkinRes;
}

ColorManager::~ColorManager(void)
{
	this->Clear();
    SAFE_RELEASE(m_pUIElement);
#if 0
    SAFE_RELEASE(m_pColorParse);
#endif
    SAFE_DELETE(m_pIColorManager);
}

IColorManager*  ColorManager::GetIColorManager()
{
    if (NULL == m_pIColorManager)
        m_pIColorManager = new IColorManager(this);

    return m_pIColorManager;
}
//
//	新建一个颜色管理文件
//
bool ColorManager::Create(const String& strXmlPath )
{
#if 0
	if( NULL != m_pColorParse )
	{
		UI_LOG_WARN(_T("%s color parse already exist."), FUNC_NAME);
		return false;
	}
	if (NULL == m_pSkinRes)
		return false;

	m_pColorParse = IParseClassFactory::CreateColorParseInstance(m_pSkinRes->GetUIApplication(), strXmlPath);

	if (false == m_pColorParse->Create())
	{
		UI_LOG_ERROR(_T("%s create color parse failed. path=%s"), FUNC_NAME, strXmlPath.c_str());
		return false;
	}
#endif
	return true;
}
bool ColorManager::Load( const String& strXmlPath )
{
#if 0
	if (m_pColorParse)
	{
		UI_LOG_WARN(_T("ColorManager::Load color parse already exist."));
		return false;
	}

	if (NULL == m_pSkinRes)
		return false;

	m_pColorParse = IParseClassFactory::CreateColorParseInstance(m_pSkinRes->GetUIApplication(), strXmlPath);

	if (false == m_pColorParse->Load(&m_resColor))
		return false;
#endif
	return true;
}

bool ColorManager::Reload()
{
#if 0
	if( NULL == m_pColorParse )
	{
		UI_LOG_ERROR(_T("ColorManager::Reload m_pColorParse == NULL."));
		return false;
	}

	this->Clear();
	return  m_pColorParse->Reload( &m_resColor );
#endif
    return false;
}

bool ColorManager::Save()
{
#if 0
	assert( NULL != m_pColorParse );
	if( NULL == m_pColorParse )
		return false;

	return m_pColorParse->Save(&m_resColor);
#endif
    return false;
}
/*
**	清除所有col资源
*/
void ColorManager::Clear()
{
	m_resColor.Clear();
}

int ColorManager::GetColorCount()
{
	return m_resColor.GetColorCount();
}

IColorResItem* ColorManager::GetColorItemInfo(int nIndex)
{
	ColorResItem* pItem = this->m_resColor.GetColorItem(nIndex);
	if (NULL == pItem)
		return NULL;

    return pItem->GetIColorResItem();
}

HRESULT  ColorManager::InsertColorItem(const TCHAR* bstrID, const TCHAR* bstrColor)
{
#if 0
	if (NULL == bstrID || NULL == bstrColor)
		return E_INVALIDARG;

	if (NULL == m_pColorParse)
		return E_FAIL;

	if (false == m_resColor.InsertColor(bstrID, bstrColor, NULL))
	{
		UI_LOG_ERROR(_T("%s m_resColor.InsertColorItem strID=%s"), bstrID);
		return E_FAIL;
	}

	if (false == m_pColorParse->InsertColor(this->m_resColor.GetColorItem(bstrID)))
	{
		UI_LOG_ERROR(_T("%s m_pColorParse.InsertColorItem strID=%s"), bstrID);
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT  ColorManager::ModifyColorItem(const TCHAR* bstrID, const TCHAR* bstrColor)
{
#if 0
	if (NULL == bstrID || NULL == bstrColor)
		return E_INVALIDARG;

	if (NULL == m_pColorParse)
		return E_FAIL;

	if (false == m_resColor.ModifyColor(bstrID, bstrColor))
	{
		UI_LOG_ERROR(_T("%s m_resColor.ModifyColorItem strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}

	if (false == m_pColorParse->ModifyColor(this->m_resColor.GetColorItem(bstrID)))
	{
		UI_LOG_ERROR(_T("%s m_pColorParse.ModifyColorItem strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}
#endif
	return S_OK;
}
HRESULT ColorManager::RemoveColorItem(const TCHAR* bstrID)
{
#if 0
	if (NULL == bstrID)
		return E_INVALIDARG;

	if (NULL == m_pColorParse )
		return E_FAIL;

	if (false == m_resColor.RemoveColor(bstrID))
	{
		UI_LOG_ERROR(_T("%s m_resColor.RemoveColor strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}

	if (false == m_pColorParse->RemoveColor(bstrID))
	{
		UI_LOG_ERROR(_T("%s m_pColorParse.RemoveColor strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}
#endif
	return S_OK;
}

bool ColorManager::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	return m_resColor.ChangeSkinHLS(h,l,s,nFlag);
}

IColorRes*  ColorManager::GetColorRes()
{
	return m_resColor.GetIColorRes();
}


//////////////////////////////////////////////////////////////////////////

HRESULT  ColorManager::UIParseSkinElement(IUIElement* pElem, ISkinRes* pSkinRes)
{
    IColorManager*  pColorMgr = pSkinRes->GetColorManager();
    if (NULL == pColorMgr)
        return E_FAIL;

    return pColorMgr->GetImpl()->ParseNewElement(pElem);
}

HRESULT  ColorManager::ParseNewElement(IUIElement* pElem)
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
void  ColorManager::OnNewChild(IUIElement* pElem)
{
    CComBSTR bstrTagName;
    pElem->GetTagName(&bstrTagName);

    //	加载所有属性
    IMapAttribute* pMapAttrib = NULL;
    pElem->GetAttribList(&pMapAttrib);

    CComBSTR  bstrValue;
    pElem->GetData(&bstrValue);

    if (false == m_resColor.LoadItem(pMapAttrib, bstrValue))
    {
        UI_LOG_WARN(_T("%s insert color failed."), FUNC_NAME);
    }

    SAFE_RELEASE(pMapAttrib);
}