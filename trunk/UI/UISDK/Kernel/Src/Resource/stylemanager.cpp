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

// ��xml�н�һ�������������Զ�ȡ��������Ҫ������styleclass����
HRESULT  StyleManager::ParseStyle(const TCHAR* szObjName, IMapAttribute* pMapAttrib)
{
    if (NULL == szObjName || NULL == pMapAttrib)
        return E_INVALIDARG;

    String strID, strStyleClass;  // ����pMapAttrib->GetAttr������ʱ������
    {
        const TCHAR*  szID = pMapAttrib->GetAttr(XML_ID, false);
        if (szID)
            strID = szID;

        const TCHAR*  szStyleClass = pMapAttrib->GetAttr(XML_STYLECLASS, true);
        if (szStyleClass)
            strStyleClass = szStyleClass;
    }
    return this->LoadStyle(szObjName, strStyleClass.c_str(), strID.c_str(), pMapAttrib);  // �Ǳ༭ģʽ�£�ֱ��ʹ�ø�mapattrib
}

//
// ����һ���̳��ַ�����������ʽ���ͣ��罫#button����Ϊ idѡ�����ͣ�id=button
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
//	�ݹ����һ��CPojo_StyleItem�ļ̳�����
//
//	Parameter
//		pTreeItem
//			[in]	Ҫ������CPojo_StyleItem��pTreeItem�������ļ̳���������������Ҷ�ӽ�㡣
//					����Լ��ظ�������������У���ʾ�����������̳У��޷�������
//
//		pStyleRes
//			[in]	��ǰ���е�StyleItem�б�
//
//	Return
//		�����ɹ�����TRUE��ʧ�ܷ���FALSE��ֻҪ������һ������FALSE�������ȫ������FALSE��
//
bool  StyleManager::parse_inherit(tree<StyleResItem*>* pTreeItem, StyleRes* pStyleRes)
{
    if (NULL == pTreeItem || NULL == pTreeItem->data )
        return false;

    StyleResItem* pStyleItem = pTreeItem->data;

    // �ж�pTreeItem�Ƿ��м̳й�ϵ
    int nSize = pStyleItem->GetInheritCount();
    if (0 == nSize )
        return true;

    // �жϵ�ǰ���pTreeItem�����������ǲ����ظ������ˣ�����ظ����������ʾ��ѭ����
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

        // �����ɹ����̳���������
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
    if (m_pUIElement)  // ��ݶ��廹δʵ�֣�Ҫ���ǵĶ����϶࣬ ��һ����ʵ�ְ�
        return E_NOTIMPL;

    m_pUIElement = pElem;
    m_pUIElement->AddRef();

    // ��������Ԫ��
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

    // �����е�ITEM������󣬿�ʼ����inherit��ϵ
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

    //	������������
    IMapAttribute* pMapAttrib = NULL;
    pElem->GetAttribList(&pMapAttrib);

    StyleResItem* pStyleItem = new StyleResItem;
    pStyleItem->SetAttributeMap(pMapAttrib);

    do 
    {
        // ����  SELECTOR_TPYE, ID
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

        // ���� inherit ����
        const TCHAR* szText = pMapAttrib->GetAttr(XML_STYLE_INHERIT, true);  // ����һ�����ԣ�������Ҫ����չ�ģ�ɾ����
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
