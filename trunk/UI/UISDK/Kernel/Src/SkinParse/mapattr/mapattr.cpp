#include "stdafx.h"
#include "mapattr.h"
#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Src\Renderbase\textrenderbase\textrender.h"

namespace UI
{

CMapAttribute::CMapAttribute()
{
    m_lRef = 0;
    m_iterEnum = m_mapAttr.end();
}
CMapAttribute::CMapAttribute(const CMapAttribute& o)
{
    m_mapAttr = o.m_mapAttr;
    m_strTemp.clear();
}

bool  CMapAttribute::HasAttrib(const TCHAR* szKey)
{
    if (NULL == szKey)
        return false;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return false;

    return true;
}
const TCHAR* CMapAttribute::GetAttr(const TCHAR* szKey, bool bErase)
{
    if (NULL == szKey)
        return NULL;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return NULL;

    if (bErase)
    {
        m_strTemp = iter->second;
        m_mapAttr.erase(iter);
        return m_strTemp.c_str();
    }
    return iter->second.c_str();
}

const TCHAR* CMapAttribute::GetAttr(const String& strKey, bool bErase)
{
    ATTRMAP::iterator iter = m_mapAttr.find(strKey);
    if (iter == m_mapAttr.end())
        return NULL;

    if (bErase)
    {
        m_strTemp = iter->second;
        m_mapAttr.erase(iter);
        return m_strTemp.c_str();
    }
    return iter->second.c_str();
}

LRESULT CMapAttribute::GetAttr_bool(const TCHAR* szKey, bool bErase, bool* pbGet)
{
    if (NULL == szKey || NULL == pbGet)
        return MAPATTR_RET_ERROR;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;
    
    LRESULT lRet = MAPATTR_RET_OK;

    const TCHAR* szValue = iter->second.c_str();
    if (0 == _tcscmp(szValue, XML_BOOL_VALUE_TRUE)  || 
        0 == _tcscmp(szValue, XML_BOOL_VALUE_1)     ||
        0 == _tcscmp(szValue, XML_BOOL_VALUE_YES))
    {
        *pbGet = true; 
    }
    else if (0 == _tcscmp(szValue, XML_BOOL_VALUE_FALSE) ||
        0 == _tcscmp(szValue, XML_BOOL_VALUE_0)      ||
        0 == _tcscmp(szValue, XML_BOOL_VALUE_NO))
    {
        *pbGet = false;
    }
    else 
    {
        lRet = MAPATTR_RET_INVLID_VALUE;
    }

    if (bErase)
        m_mapAttr.erase(iter);

    return lRet;
}

long CMapAttribute::GetAttr_int(const TCHAR* szKey, bool bErase, int* pnGet)
{
    if (NULL == szKey || NULL == pnGet)
        return MAPATTR_RET_ERROR;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    *pnGet = _ttoi(iter->second.c_str());

    if (bErase)
        m_mapAttr.erase(iter);

    return MAPATTR_RET_OK;
}

long  CMapAttribute::GetAttr_intarray(const TCHAR* szKey, bool bErase, int* pIntArray, unsigned int nSize)
{
    if (NULL == szKey || NULL == pIntArray)
        return MAPATTR_RET_ERROR;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    Util::ISplitStringEnum* pEnum = NULL;
    int nCount = Util::SplitString(iter->second.c_str(), XML_SEPARATOR, &pEnum);
    if (nCount != nSize)
    {
        SAFE_RELEASE(pEnum);
        return MAPATTR_RET_INVLID_VALUE;
    }
    for (int i = 0; i < nCount; i++)
    {
        pIntArray[i] = _ttoi(pEnum->GetText(i));
    }

    SAFE_RELEASE(pEnum);
    if (bErase)
        m_mapAttr.erase(iter);

    return MAPATTR_RET_OK;
}

long CMapAttribute::GetAttr_CRegion4(const TCHAR* szKey, bool bErase, CRegion4* prcGet)
{
    if (NULL == szKey || NULL == prcGet)
        return MAPATTR_RET_ERROR;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    LRESULT lRet = MAPATTR_RET_OK;
    if (false == Util::TranslateRECT(iter->second.c_str(), prcGet))
        lRet = MAPATTR_RET_INVLID_VALUE;

    if (bErase)
        m_mapAttr.erase(iter);

    return lRet;
}

long  CMapAttribute::GetAttr_Image9Region(const TCHAR* szKey, bool bErase, Image9Region* pRegion)
{
    if (NULL == szKey || NULL == pRegion)
        return MAPATTR_RET_ERROR;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    LRESULT lRet = MAPATTR_RET_OK;
    if (false == Util::TranslateImage9Region(iter->second.c_str(), pRegion))
        lRet = MAPATTR_RET_INVLID_VALUE;

    if (bErase)
        m_mapAttr.erase(iter);

    return lRet;
}

long CMapAttribute::GetAttr_RenderBase(const TCHAR* szPrefix, const TCHAR* szKey, bool bErase, IUIApplication* pUIApp, IObject* pBindObj, IRenderBase** ppGet)
{
    if (NULL == szKey || NULL == ppGet || NULL == pUIApp)
        return MAPATTR_RET_ERROR;

    String strKey;
    if (szPrefix)
        strKey = szPrefix;
    strKey.append(szKey);

    ATTRMAP::iterator iter = m_mapAttr.find(strKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    LRESULT lRet = MAPATTR_RET_OK;
    SAFE_RELEASE(*ppGet);

    pUIApp->CreateRenderBaseByName((BSTR)iter->second.c_str(), pBindObj, ppGet);
    if (*ppGet)
    {
        (*ppGet)->SetAttribute(static_cast<IMapAttribute*>(this), szPrefix, bErase);
    }
    else
    {
        lRet = MAPATTR_RET_INVLID_VALUE;
    }

    if (bErase)
        m_mapAttr.erase(iter);

    return lRet;
}

long CMapAttribute::GetAttr_TextRenderBase(const TCHAR* szKey, bool bErase, IUIApplication* pUIApp, IObject* pBindObj, ITextRenderBase** ppGet)
{
    if (NULL == szKey || NULL == ppGet)
        return MAPATTR_RET_ERROR;

    ATTRMAP::iterator iter = m_mapAttr.find(szKey);
    if (iter == m_mapAttr.end())
        return MAPATTR_RET_NOT_EXIST;

    LRESULT lRet = MAPATTR_RET_OK;
    SAFE_RELEASE(*ppGet);

    pUIApp->CreateTextRenderBaseByName((BSTR)iter->second.c_str(), pBindObj, ppGet);
    if (*ppGet)
    {
        (*ppGet)->SetAttribute(static_cast<IMapAttribute*>(this), NULL, bErase);
    }
    else
    {
        lRet = MAPATTR_RET_INVLID_VALUE;
    }

    if (bErase)
        m_mapAttr.erase(iter);

    return lRet;
}

bool CMapAttribute::AddAttr(const TCHAR* szKey, const TCHAR* szValue) 
{
    if (NULL == szKey || NULL == szValue)
        return FALSE;

    m_mapAttr[szKey] = szValue;
    return true;
}
void CMapAttribute::AddAttr(const String& strKey, const String& strValue)
{
    m_mapAttr[strKey] = strValue;
}

// 使用前缀Prefix，从当前属性中抽取出相应的属性集放在ppMapAttribute中返回给外部
// 外部调用完之后，需要使用ppMapAttribute->Release释放内存
bool  CMapAttribute::ExtractMapAttrByPrefix(const TCHAR* szPrefix, bool bErase, /*out*/IMapAttribute** ppMapAttribute)
{
    if (NULL == ppMapAttribute)
        return FALSE;

    if (NULL == szPrefix || 0 == _tcslen(szPrefix))
    {
//         *ppMapAttribute = static_cast<IMapAttribute*>(this);
//         this->AddRef();
//         return true;
        return false;
    }

    ATTRMAP::iterator  iter = m_mapAttr.begin();
    ATTRMAP::iterator  iterEnd = m_mapAttr.end();

    int nPrifixLength = _tcslen(szPrefix);

    IMapAttribute*  pSubMapAttrib = NULL;
    UICreateIMapAttribute(&pSubMapAttrib);

    for ( ; iter != iterEnd; )
    {
        TCHAR*  szKey = const_cast<TCHAR*>(iter->first.c_str());
        if (_tcsstr(szKey, szPrefix) == szKey)
        {
            pSubMapAttrib->AddAttr(szKey + nPrifixLength, iter->second.c_str());

            if (bErase)
                m_mapAttr.erase(iter++);
            else
                ++iter;
        }
        else
        {
            ++iter;
        }
    }

    *ppMapAttribute = pSubMapAttrib;
    return true;
}

long  CMapAttribute::Release()
{
    UIASSERT(m_lRef > 0);
    --m_lRef;
    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }
    return m_lRef;
}
long  CMapAttribute::AddRef()
{
    return ++ m_lRef;
}

void  CMapAttribute::CreateCopy(IMapAttribute** ppNewCopy)
{
    if (NULL == ppNewCopy)
        return;

    CMapAttribute* p = new CMapAttribute;
    p->AddRef();
    *ppNewCopy = static_cast<IMapAttribute*>(p);
    p->m_mapAttr = m_mapAttr;
}

// 将自己的属性拷贝给pDestMapAttrib，如果pDestMapAttrib中已经存在，则按钮bOverride参数判断是否覆盖
void  CMapAttribute::CopyTo(IMapAttribute* pDestMapAttrib, bool bOverride)
{
    if (NULL == pDestMapAttrib)
        return;

    ATTRMAP::iterator  iter = m_mapAttr.begin();
    if (bOverride)
    {
        for (; iter != m_mapAttr.end(); iter++)
        {
            pDestMapAttrib->AddAttr(iter->first.c_str(), iter->second.c_str());
        }
    }
    else
    {
        for (; iter != m_mapAttr.end(); iter++)
        {
            if (pDestMapAttrib->HasAttrib(iter->first.c_str()))
                continue;

            pDestMapAttrib->AddAttr(iter->first.c_str(), iter->second.c_str());
        }
    }
}


void  CMapAttribute::BeginEnum()
{
    m_iterEnum = m_mapAttr.begin();
}
bool  CMapAttribute::EnumNext(const TCHAR** szKey, const TCHAR** szValue)
{
    if (NULL == szKey || NULL == szValue)
        return false;

    if (m_iterEnum == m_mapAttr.end())
        return false;

    *szKey = m_iterEnum->first.c_str();
    *szValue = m_iterEnum->second.c_str();

    m_iterEnum++;
    return true;
}
void  CMapAttribute::EndEnum()
{
    m_iterEnum = m_mapAttr.end();
}

HRESULT  UICreateIMapAttribute(IMapAttribute** ppOut)
{
    if (NULL == ppOut)
        return E_INVALIDARG;

    CMapAttribute* p = new CMapAttribute;
    *ppOut = static_cast<IMapAttribute*>(p);
    p->AddRef();

    return S_OK;
}

HRESULT  UICreateIListAttribute(IListAttribute** ppOut)
{
    if (NULL == ppOut)
        return E_INVALIDARG;

    CListAttribute* p = new CListAttribute;
    *ppOut = static_cast<IListAttribute*>(p);
    p->AddRef();

    return S_OK;
}

//////////////////////////////////////////////////////////////////////////

CListAttribute::CListAttribute()
{
    m_lRef = 0;
    m_pFirstItem = NULL;
    m_pLastItem = NULL;
    m_pEnum = NULL;
}
CListAttribute::~CListAttribute()
{
    ListAttrItem* pItem = m_pFirstItem;
    while (pItem)
    {
        ListAttrItem*  pNext = pItem->pNext;
        SAFE_DELETE(pItem);
        pItem = pNext;
    }
    m_pFirstItem = NULL;
    m_pLastItem = NULL;
}

bool  CListAttribute::AddAttr(const TCHAR* szKey, const TCHAR* szValue)
{
    ListAttrItem* pItem = new ListAttrItem;
    if (szKey)
        pItem->strKey = szKey;
    if (szValue)
        pItem->strValue = szValue;

    if (NULL == m_pLastItem)
    {
        m_pFirstItem = m_pLastItem = pItem;
    }
    else
    {
        m_pLastItem->pNext = pItem;
        m_pLastItem = pItem;
    }

    return true;
}

void  CListAttribute::BeginEnum()
{
    m_pEnum = m_pFirstItem;
}
bool  CListAttribute::EnumNext(const TCHAR** szKey, const TCHAR** szValue)
{
    if (NULL == m_pEnum)
        return false;

    if (szKey)
        *szKey = m_pEnum->strKey.c_str();
    if (szValue)
        *szValue = m_pEnum->strValue.c_str();

    m_pEnum = m_pEnum->pNext;
    return true;
}
void  CListAttribute::EndEnum()
{
    m_pEnum = NULL;
}

long  CListAttribute::Release()
{
    UIASSERT(m_lRef > 0);
    --m_lRef;
    if (0 == m_lRef)
    {
        delete this;
        return 0;
    }
    return m_lRef;
}
long  CListAttribute::AddRef()
{
    return ++ m_lRef;
}

}


