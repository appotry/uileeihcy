#include "StdAfx.h"
#include "stylemanager.h"

#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"

StyleManager::StyleManager(SkinRes* p)
{
    m_pUIElement = NULL;
    m_pIStyleManager = NULL;
#if 0
	m_pStyleParse = NULL;
#endif
	m_pSkinRes = p;
}

StyleManager::~StyleManager(void)
{
	this->Clear();
#if 0
	if (NULL!=m_pStyleParse)
	{
		m_pStyleParse->Release();
		m_pStyleParse = NULL;
	}
#endif
    SAFE_RELEASE(m_pUIElement);
    SAFE_DELETE(m_pIStyleManager);
}

IStyleManager*  StyleManager::GetIStyleManager()
{
    if (NULL == m_pIStyleManager)
        m_pIStyleManager = new IStyleManager(this);

    return m_pIStyleManager;
}

bool StyleManager::Create( const String& strXmlPath )
{
#if 0
	if (NULL == m_pSkinRes)
		return false;

	if (m_pStyleParse)
	{
		UI_LOG_WARN(_T("StyleManager::Create style parse already exist."));
		return false;
	}

	m_pStyleParse = IParseClassFactory::CreateStyleParseInstance(m_pSkinRes->GetUIApplication(), strXmlPath );
	if (false == m_pStyleParse->Create())
	{
		UI_LOG_ERROR(_T("StyleManager::Create create style parse failed. path=%s"), strXmlPath.c_str());
		return false;
	}
#endif

	return true;
}
bool StyleManager::Load( const String& strXmlPath )
{
#if 0    
	if (m_pStyleParse )
	{
		UI_LOG_WARN(_T("StyleManager::Load style parse already exist."));
		return false;
	}
	m_pStyleParse = IParseClassFactory::CreateStyleParseInstance(m_pSkinRes->GetUIApplication(), strXmlPath );

	if (false == m_pStyleParse->Load(&m_resStyle) )
		return false;
#endif
	return true;
}

bool StyleManager::Reload()
{
#if 0
	if (NULL == m_pStyleParse )
	{
		UI_LOG_ERROR(_T("StyleManager::Reload m_pStyleParse == NULL."));
		return false;
	}

	this->Clear();
	return  m_pStyleParse->Reload( &m_resStyle );
#endif
    return false;
}

bool StyleManager::Save()
{
#if 0
	assert( NULL != m_pStyleParse );
	if (NULL == m_pStyleParse )
		return false;

	return m_pStyleParse->Save(&m_resStyle);
#endif
    return false;
}

void StyleManager::Clear()
{
	m_resStyle.Clear();
}

int StyleManager::GetStyleCount( )
{
	return m_resStyle.GetStyleCount();
}
// bool StyleManager::GetStyleItemInfo( int nIndex, IStyleResItem** ppStyleItemInfo )
// {
// 	StyleResItem* pItem = this->m_resStyle.GetStyleItem(nIndex);
// 	if (NULL == pItem )
// 		return false;
// 
// 	*ppStyleItemInfo = (IStyleResItem*)pItem;
// 	return true;
// }
// 
// bool StyleManager::GetStyleItemInfo(STYLE_SELECTOR_TYPE eType, const TCHAR* szSelectorID, IStyleResItem** ppStyleItemInfo )
// {
// 	if (NULL == ppStyleItemInfo)
// 		return false;
// 
// 	StyleResItem* pItem = this->m_resStyle.GetStyleItem(eType, szSelectorID);
// 	if (NULL == pItem)
// 		return false;
// 
// 	*ppStyleItemInfo = (IStyleResItem*)pItem;
// 	return true;
// }

HRESULT StyleManager::InsertStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szInherit)
{
#if 0
	if (NULL == bstrID || NULL == bstrInherit)
		return E_INVALIDARG;

	if (NULL == m_pStyleParse)
		return E_FAIL;

	if (false == m_resStyle.InsertStyle(type, bstrID, bstrInherit))
	{
		UI_LOG_ERROR(_T("%s m_resStyle.InsertStyleItem strID=%s"), FUNC_NAME, bstrID);
		return E_FAIL;
	}

	if (false == m_pStyleParse->InsertStyle( this->m_resStyle.GetStyleItem(type, bstrID)))
	{
		UI_LOG_ERROR(_T("%s m_pStyleParse.InsertStyleItem strID=%s"), FUNC_NAME, bstrID);
		return E_FAIL;
	}
#endif
	return S_OK;
}
HRESULT StyleManager::ModifyStyleItem(STYLE_SELECTOR_TYPE  type, const TCHAR* szID, const TCHAR* szInherit)
{
#if 0
	if (NULL == bstrID || NULL == bstrInherit)
		return E_INVALIDARG;

	if (NULL == m_pStyleParse)
		return E_FAIL;

	if (false == m_resStyle.ModifyStyle(type, bstrID, bstrInherit))
	{
		UI_LOG_ERROR(_T("%s m_resStyle.ModifyStyle strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}

	if (false == m_pStyleParse->ModifyStyle( this->m_resStyle.GetStyleItem(type, bstrID)))
	{
		UI_LOG_ERROR(_T("%s m_pStyleParse.ModifyStyle strID=%s Failed. "),FUNC_NAME, bstrID);
		return E_FAIL;
	}
#endif
	return S_OK;
}
HRESULT StyleManager::RemoveStyleItem(STYLE_SELECTOR_TYPE  type, const TCHAR* szID)
{
#if 0
	if (NULL == bstrID)
		return E_INVALIDARG;

	if (NULL == m_pStyleParse)
		return E_FAIL;

	if (false == m_resStyle.RemoveStyle(type, bstrID) )
	{
		UI_LOG_ERROR(_T("%s m_resStyle.RemoveColor strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}

	if (false == m_pStyleParse->RemoveStyle(type, bstrID))
	{
		UI_LOG_ERROR(_T("%s m_pStyleParse.RemoveColor strID=%s Failed. "), FUNC_NAME, bstrID);
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT StyleManager::InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey, const TCHAR* szValue)
{
#if 0
	if (NULL == bstrID || NULL == bstrKey || NULL == bstrValue)
		return E_INVALIDARG;

	if (NULL == m_pStyleParse)
		return E_FAIL;

	if (false == m_resStyle.InsertStyleAttribute(type, bstrID, bstrKey, bstrValue))
	{
		UI_LOG_ERROR(_T("%s m_resStyle.InsertStyleAttribute strID=%s key=%s value=%s Failed. "), FUNC_NAME, bstrID, bstrKey, bstrValue);
		return E_FAIL;
	}

	if (false == m_pStyleParse->InsertStyleAttribute(type, bstrID, bstrKey, bstrValue))
	{
		UI_LOG_ERROR(_T("%s m_pStyleParse.InsertStyleAttribute strID=%s key=%s value=%s Failed. "), FUNC_NAME, bstrID, bstrKey, bstrValue);
		return E_FAIL;
	}
#endif
	return S_OK;
}
HRESULT StyleManager::ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey, const TCHAR* szValue)
{
#if 0
	if (NULL == bstrID || NULL == bstrKey || NULL == bstrValue)
		return E_INVALIDARG;

	if (NULL == m_pStyleParse)
		return E_FAIL;

	if (false == m_resStyle.ModifyStyleAttribute(type, bstrID, bstrKey, bstrValue))
	{
		return E_FAIL;
	}

	if (false == m_pStyleParse->ModifyStyleAttribute(type, bstrID, bstrKey, bstrValue))
	{
		return E_FAIL;
	}
#endif
	return S_OK;
}

HRESULT StyleManager::RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey)
{
#if 0
	if (NULL == bstrID || NULL == bstrKey)
		return E_INVALIDARG;

	if (NULL == m_pStyleParse )
		return E_FAIL;

	if (false == m_resStyle.RemoveStyleAttribute(type, bstrID, bstrKey))
		return E_FAIL;

	if (false == m_pStyleParse->RemoveStyleAttribute(type, bstrID, bstrKey))
		return E_FAIL;

#endif
	return S_OK;
}


HRESULT StyleManager::LoadStyle(const TCHAR* szTagName, const TCHAR* szStyleClass, const TCHAR* szID, IMapAttribute* pMapStyle)
{
	bool bRet = m_resStyle.LoadStyle(szTagName, szStyleClass, szID, pMapStyle);
	if (false == bRet)
		return E_FAIL;

    return S_OK;
}

IStyleRes*  StyleManager::GetStyleRes()
{
	return m_resStyle.GetIStyleRes();
}

// 从xml中将一个结点的所有属性读取出来后，需要解析其styleclass属性
HRESULT  StyleManager::ParseStyle(const TCHAR* szObjName, IMapAttribute* pMapAttrib)
{
    if (NULL == szObjName || NULL == pMapAttrib)
        return E_INVALIDARG;

    String strID, strStyleClass;  // 避免pMapAttrib->GetAttr返回临时变量。
    {
        const TCHAR*  szID = pMapAttrib->GetAttr(XML_ID, false);
        if (szID)
            strID = szID;

        const TCHAR*  szStyleClass = pMapAttrib->GetAttr(XML_STYLECLASS, true);
        if (szStyleClass)
            strStyleClass = szStyleClass;
    }
    return this->LoadStyle(szObjName, strStyleClass.c_str(), strID.c_str(), pMapAttrib);  // 非编辑模式下，直接使用该mapattrib
}

//
// 解析一个继承字符串所属的样式类型，如将#button解析为 id选择类型，id=button
//
bool  StyleManager::ParseInheritString(const String& strInherit, STYLE_SELECTOR_TYPE& eStyletype, TCHAR* szStyleName)
{
    if (strInherit.length() <= 0 || NULL == szStyleName )
        return false;

    if (strInherit[0] == STYLE_ID_PREFIX )
    {
        eStyletype = STYLE_SELECTOR_TYPE_ID;
        _tcsncpy( szStyleName, strInherit.substr( 1, strInherit.length()-1 ).c_str(), MAX_STRING_LEN-1 );
    }
    else if (strInherit[0] == STYLE_CLASS_PREFIX )
    {
        eStyletype = STYLE_SELECTOR_TYPE_CLASS;
        _tcsncpy( szStyleName, strInherit.substr( 1, strInherit.length()-1 ).c_str(), MAX_STRING_LEN-1 );
    }
    else
    {
        eStyletype = STYLE_SELECTOR_TYPE_TAG;
        _tcsncpy( szStyleName, strInherit.c_str(), MAX_STRING_LEN-1 );
    }
    return true;
}
bool  StyleManager::MakeInheritString(const STYLE_SELECTOR_TYPE& eStyletype, const String& strStypeName, TCHAR* szInherit)
{
    if (strStypeName.length() <= 0 || NULL == szInherit )
        return false;

    if (eStyletype == STYLE_SELECTOR_TYPE_ID )
    {
        _tcscpy( szInherit, _T(" ") );
        szInherit[0] = STYLE_ID_PREFIX ;
        _tcscat( szInherit, strStypeName.c_str() );
    }
    else if (eStyletype == STYLE_SELECTOR_TYPE_CLASS )
    {
        _tcscpy( szInherit, _T(" ") );
        szInherit[0] = STYLE_CLASS_PREFIX ;
        _tcscat( szInherit, strStypeName.c_str() );
    }
    else
    {
        _tcscpy( szInherit, strStypeName.c_str() );
    }
    return true;
}

//
//	递归解析一个CPojo_StyleItem的继承数据
//
//	Parameter
//		pTreeItem
//			[in]	要解析的CPojo_StyleItem，pTreeItem代表它的继承依赖树，是树的叶子结点。
//					如果自己重复出现在这个树中，表示出现了死锁继承，无法解析。
//
//		pStyleRes
//			[in]	当前所有的StyleItem列表
//
//	Return
//		解析成功返回TRUE，失败返回FALSE。只要树中有一个返回FALSE，这棵树全部返回FALSE。
//
bool  StyleManager::parse_inherit(tree<StyleResItem*>* pTreeItem, StyleRes* pStyleRes)
{
    if (NULL == pTreeItem || NULL == pTreeItem->data )
        return false;

    StyleResItem* pStyleItem = pTreeItem->data;

    // 判断pTreeItem是否有继承关系
    int nSize = pStyleItem->GetInheritCount();
    if (0 == nSize )
        return true;

    // 判断当前这个pTreeItem中整棵树中是不是重复出现了，如果重复出现了则表示死循环了
    {
        tree<StyleResItem*>* pParentItem = pTreeItem;
        do
        {
            pParentItem = pParentItem->parent;
            if (pParentItem && pParentItem->data == pStyleItem )
                return false;
        }
        while(pParentItem != NULL);
    }


    for( int i = nSize-1; i >= 0; i-- )
    {
        String strInherit;
        if (false == pStyleItem->GetInheritItem(i, strInherit) )
            continue;

        STYLE_SELECTOR_TYPE type = STYLE_SELECTOR_TYPE_TAG;
        TCHAR szStyleName[MAX_STRING_LEN] = _T("");
        ParseInheritString(strInherit, type, szStyleName);

        StyleResItem* pInheritItem = pStyleRes->GetStyleItem(type, szStyleName);
        if (NULL == pInheritItem )
            continue;

        tree<StyleResItem*> t;
        t.parent = pTreeItem;
        t.data   = pInheritItem;

        if (false == this->parse_inherit( &t, pStyleRes) )
        {
            return false;
        }

        // 解析成功，继承它的属性
        pInheritItem->InheritMyAttributesToAnother(pStyleItem);
        pStyleItem->RemoveInheritItem( strInherit );
    }

    return true;
}

HRESULT  StyleManager::UIParseSkinElement(IUIElement* pElem, ISkinRes* pSkinRes)
{
    IStyleManager*  pStyleMgr = pSkinRes->GetStyleManager();
    if (NULL == pStyleMgr)
        return E_FAIL;

    return pStyleMgr->GetImpl()->ParseNewElement(pElem);
}

HRESULT  StyleManager::ParseNewElement(IUIElement* pElem)
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

    // 在所有的ITEM加载完后，开始解析inherit关系
    int nSize = m_resStyle.GetStyleCount();
    for (int i = 0; i < nSize; i++)
    {
        StyleResItem* pItem = m_resStyle.GetStyleItem(i);
        tree<StyleResItem*>  t;
        t.data = pItem;

        if (false == this->parse_inherit(&t, &m_resStyle) )
        {
            UI_LOG_WARN(_T("CXmlStyleParse::load_from_file parse_inherit failed. style item=%s"), pItem->GetIDRef().c_str() );
        }
    }

    return S_OK;
}

void  StyleManager::OnNewChild(IUIElement* pElem)
{
    CComBSTR bstrTagName;
    pElem->GetTagName(&bstrTagName);

    //	加载所有属性
    IMapAttribute* pMapAttrib = NULL;
    pElem->GetAttribList(&pMapAttrib);

    StyleResItem* pStyleItem = new StyleResItem;
    pStyleItem->SetAttributeMap(pMapAttrib);

    do 
    {
        // 解析  SELECTOR_TPYE, ID
        if (bstrTagName == XML_STYLE_SELECTOR_TYPE_ID)
        {
            pStyleItem->SetSelectorType(STYLE_SELECTOR_TYPE_ID);

            String strID;
            const TCHAR* szText = pMapAttrib->GetAttr(XML_ID, true);
            if (NULL == szText)
            {
                UI_LOG_WARN( _T("Can't find the %s attribute of %s"), XML_ID, (BSTR)bstrTagName);
                delete pStyleItem;
                break;
            }
            strID = szText;
            pStyleItem->SetID( strID );
        }
        else if (bstrTagName == XML_STYLE_SELECTOR_TYPE_CLASS)
        {
            pStyleItem->SetSelectorType(STYLE_SELECTOR_TYPE_CLASS);

            String strID;
            const TCHAR* szText = pMapAttrib->GetAttr(XML_ID, true);
            if (NULL == szText)
            {
                UI_LOG_WARN( _T("Can't find the %s attribute of %s"), XML_ID, (BSTR)bstrTagName);
                delete pStyleItem;
                break;
            }
            strID = szText;
            pStyleItem->SetID(strID);
        }
        else
        {
            pStyleItem->SetSelectorType(STYLE_SELECTOR_TYPE_TAG);
            pStyleItem->SetID((BSTR)bstrTagName);
        }

        // 解析 inherit 属性
        const TCHAR* szText = pMapAttrib->GetAttr(XML_STYLE_INHERIT, true);  // 不是一个属性，它是需要被扩展的，删除它
        if (szText)
            pStyleItem->SetInherits(szText);

        if (false == m_resStyle.InsertStyle(pStyleItem) )
        {
            delete pStyleItem;
            pStyleItem = NULL;
        }
    }
    while (0);

    SAFE_RELEASE(pMapAttrib);
}
