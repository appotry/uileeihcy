#include "stdafx.h"
#include "styleres.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"

namespace UI
{
	

//////////////////////////////////////////////////////////////////////////
//
//	StyleResItem
//

StyleResItem::StyleResItem()
{
    m_pIStyleResItem = NULL;
    m_pMapAttrib = NULL;
	m_eSelectorType = STYLE_SELECTOR_TYPE_TAG;
}
StyleResItem::~StyleResItem()
{
    SAFE_RELEASE(m_pMapAttrib);
    SAFE_DELETE(m_pIStyleResItem);
}
IStyleResItem*  StyleResItem::GetIStyleResItem()
{
    if (NULL == m_pIStyleResItem)
        m_pIStyleResItem = new IStyleResItem(this);

    return m_pIStyleResItem;
}
void StyleResItem::SetInherits( const String& str )
{
	m_strInherits = str;
	m_vInheritList.clear();

	vector<String>  vStrArray;
	UI_Split(str, XML_SEPARATOR, vStrArray);
	int nSize = (int)vStrArray.size();
	if (0 != nSize )
	{
		for(int i = 0; i < nSize; i++ )
		{
			m_vInheritList.push_back(vStrArray[i]);
		}
	}
}
int StyleResItem::GetInheritCount() 
{
	return (int) m_vInheritList.size(); 
}

bool StyleResItem::GetInheritItem(int i, String& str)
{
	if (i < 0 || i >= GetInheritCount() )
		return false;

	str = m_vInheritList[i];
	return true;
}

//
// ����m_vInheritList��ɾ��������m_strInherit��ɾ��
//
bool StyleResItem::RemoveInheritItem( const String& str )
{
	vector<String>::iterator iter = m_vInheritList.begin();
	vector<String>::iterator iterEnd = m_vInheritList.end();
	for ( ; iter != iterEnd; iter++ )
	{
		if (*iter == str )
		{
			m_vInheritList.erase(iter);
			return true;
		}
	}

	return false;
}

void  StyleResItem::SetAttributeMap(IMapAttribute* pMapAttrib)
{
    SAFE_RELEASE(m_pMapAttrib);
    m_pMapAttrib = pMapAttrib;
    if (m_pMapAttrib)
        m_pMapAttrib->AddRef();
}

void StyleResItem::GetAttributeMap(IMapAttribute** ppMapAttrib)
{
	if (NULL == ppMapAttrib)
		return;

	*ppMapAttrib = m_pMapAttrib;
    if (m_pMapAttrib)
        m_pMapAttrib->AddRef();
}

void StyleResItem::SetAttribute(const String& key, const String& value)
{
    if (m_pMapAttrib)
        m_pMapAttrib->AddAttr(key.c_str(), value.c_str());
}

bool StyleResItem::GetAttribute(const String& key, String& retValue)
{
    if (NULL == m_pMapAttrib)
        return false;

    const TCHAR* szText = m_pMapAttrib->GetAttr(key.c_str(), false);
    if (NULL == szText)
        return false;

    retValue = szText;
    return true;
}

bool StyleResItem::InsertAttribute(const String& key, const String& value)
{
	SetAttribute(key, value);
    return true;
}

bool StyleResItem::ModifyAttribute(const String& key, const String& value)
{
    UIASSERT(0);
#if 0
	if (0 == m_mapAttributes.count(key))
		return false;

	m_mapAttributes[key] = value;
#endif
	return true;
}

bool StyleResItem::RemoveAttribute(const String& key)
{
    UIASSERT(0);
#if 0
	if (0 == m_mapAttributes.count(key))
		return false;

	m_mapAttributes.erase(key);
#endif

	return true;
}

bool StyleResItem::InheritAttribute(const String& key, const String& value)
{
    // �̳�һ������ֵ������Լ��Ѿ�������������ԣ���ʹ�������ֵ
    if (NULL == m_pMapAttrib->GetAttr(key.c_str(), false))
    {
        m_pMapAttrib->AddAttr(key.c_str(), value.c_str());
        return true;
    }
	
	return false;
}

//
// ���Լ������Լ̳и���һ��item
//	
//	���pChild�Ѿ�����������ˣ���ʹ��pChild������
//
bool StyleResItem::InheritMyAttributesToAnother(StyleResItem* pChild)
{
	if (NULL == pChild)
		return false;

    IMapAttribute*  pMapAttrib = NULL;
    pChild->GetAttributeMap(&pMapAttrib);
    if (pMapAttrib)
    {
        m_pMapAttrib->CopyTo(pMapAttrib, false);
        SAFE_RELEASE(pMapAttrib);
    }
   
	return true;
}

//
//	���Լ�������Ӧ�õ�һ�����е�map�У����map������������ԣ���ʹ�ø�map������
//
bool StyleResItem::ApplyMyAttributesToAnExistMap(IMapAttribute* pMapAttrib)
{
    if (NULL == pMapAttrib)
        return false;

    m_pMapAttrib->CopyTo(pMapAttrib, false);
	return true;
}

//////////////////////////////////////////////////////////////////////////
// 
//   StyleRes
//

StyleRes::StyleRes()
{
    m_pIStyleRes = NULL;
}
StyleRes::~StyleRes()
{
	this->Clear();
    SAFE_DELETE(m_pIStyleRes);
}
IStyleRes*  StyleRes::GetIStyleRes()
{
    if (NULL == m_pIStyleRes)
        m_pIStyleRes = new IStyleRes(this);

    return m_pIStyleRes;
}

void StyleRes::Clear()
{
	vector<StyleResItem*>::iterator  iter = m_vStyles.begin();
	vector<StyleResItem*>::iterator  iterEnd = m_vStyles.end();

	for ( ; iter != iterEnd; iter++ )
	{
		StyleResItem* p = *iter;
		delete p;
		p = NULL;
	}

	m_vStyles.clear();
}

IStyleResItem* StyleRes::GetStyleResItem(long lIndex)
{
	StyleResItem* pItem = this->GetStyleItem(lIndex);
	if (NULL == pItem)
		return NULL;

    return pItem->GetIStyleResItem();
}

IStyleResItem* StyleRes::GetStyleResItem2(STYLE_SELECTOR_TYPE type, const TCHAR* szId)
{
	if (NULL == szId)
		return NULL;

	StyleResItem* pItem = this->GetStyleItem(type, szId);
	if (NULL == pItem)
		return NULL;

	return pItem->GetIStyleResItem();
}

bool StyleRes::InsertStyle( STYLE_SELECTOR_TYPE type, const String& strID, const String& strInherit )
{
	StyleResItem* pStyleItem = new StyleResItem;
	pStyleItem->SetSelectorType(type);
	pStyleItem->SetID(strID);
	pStyleItem->SetInherits(strInherit);

	bool bRet = this->InsertStyle(pStyleItem);
	if (false == bRet)
	{
		delete pStyleItem;
		pStyleItem = NULL;
	}

	return bRet;
}

bool StyleRes::InsertStyle( StyleResItem* pNewItem )
{
	if (NULL == pNewItem)
		return false;

	StyleResItem* p = this->GetStyleItem(pNewItem->GetSelectorType(), pNewItem->GetIDRef());
	if (p)
	{
		UI_LOG_WARN(_T("StyleRes::InsertStyle failed, insert item=%s"), pNewItem->GetIDRef().c_str() );
		return false;
	}

	this->m_vStyles.push_back(pNewItem);  
	return true;
}

bool StyleRes::ModifyStyle( STYLE_SELECTOR_TYPE type,const String& strID, const String& strInherit )
{
	StyleResItem* p = this->GetStyleItem(type, strID);
	if (NULL == p)
	{
		UI_LOG_WARN(_T("StyleRes::ModifyStyle failed, modify item=%s"),strID.c_str() );
		return false;
	}

	p->SetInherits(strInherit);
	return true;
}

bool StyleRes::RemoveStyle( STYLE_SELECTOR_TYPE type, const String& strID )
{
	vector<StyleResItem*>::iterator  iter = m_vStyles.begin();
	vector<StyleResItem*>::iterator  iterEnd = m_vStyles.end();

	for( ; iter != iterEnd; iter++ )
	{
		StyleResItem* p = *iter;
		if (p->GetSelectorType() == type && p->GetIDRef() == strID)
		{
			delete p;
			p = NULL;
			m_vStyles.erase(iter);
			return true;
		}
	}
	return false;
}

long StyleRes::GetStyleCount() 
{
	return (long)m_vStyles.size();
}


StyleResItem* StyleRes::GetStyleItem( int nIndex )
{
	if (nIndex < 0)
		return NULL;
	if (nIndex >= (int)m_vStyles.size())
		return NULL;

	return m_vStyles[nIndex];
}

StyleResItem* StyleRes::GetStyleItem( STYLE_SELECTOR_TYPE type, const String& strID )
{
	return GetStyleItem(type, strID.c_str());
}
StyleResItem* StyleRes::GetStyleItem( STYLE_SELECTOR_TYPE type, const TCHAR* szID )
{
	if (NULL == szID)
		return NULL;

	vector<StyleResItem*>::iterator  iter = m_vStyles.begin();
	vector<StyleResItem*>::iterator  iterEnd = m_vStyles.end();

	for ( ; iter != iterEnd; iter++ )
	{
		StyleResItem* p = *iter;
		if (p->GetSelectorType() == type && 0 == _tcscmp(p->GetIDRef().c_str(), szID))
		{
			return p;
		}
	}
	return NULL;
}

bool StyleRes::InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	StyleResItem* pItem = this->GetStyleItem(type, strID);
	if (false == pItem)
		return false;

	return pItem->InsertAttribute( strKey, strValue );
}
bool StyleRes::ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	StyleResItem* pItem = this->GetStyleItem(type, strID);
	if (false == pItem)
		return false;

	return pItem->ModifyAttribute( strKey, strValue );
}
bool StyleRes::RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey )
{
	StyleResItem* pItem = this->GetStyleItem(type, strID);
	if (false == pItem)
		return false;

	return pItem->RemoveAttribute( strKey );
}

//
//	����ƥ�����ʽ
//
//	Parameter
//		strTagName
//			[in]	Ҫƥ��ı�ǩ��
//		strStyleClass
//			[in]	Ҫƥ���class��ʽ
//		strID
//			[in]	Ҫƥ���id��ʽ
//
//		mapStyle
//			[in/out] ��������ص���map��
//
//	Remark
//		�Ż������壺
//			����1. ��layout.xml��ֱ��ָ��������
//			����2. ͨ��IDƥ�������
//			����3. ͨ��CLASSƥ�������
//			����4. CLASS�б��У��ź������ʽƥ������� > ǰ�����ʽƥ�������
//			����5. ͨ��TAGƥ�������
//
bool StyleRes::LoadStyle(const TCHAR* szTagName, const TCHAR* szStyleClass, const TCHAR* szID, IMapAttribute* pMapStyle)
{
	if (szID && _tcslen(szID) > 0)
	{
		StyleResItem* pIDStyleItem = this->GetStyleItem(STYLE_SELECTOR_TYPE_ID, szID);
		if (pIDStyleItem)
		{
			pIDStyleItem->ApplyMyAttributesToAnExistMap(pMapStyle);
		}
	}

	// strStyleClass�����ж��
	if (szStyleClass && _tcslen(szStyleClass) > 0)
	{
		vector<String>  vStrArray;
		UI_Split(szStyleClass, XML_SEPARATOR, vStrArray);   // TODO: ����ĵ�һ����������ת��ΪString������һ�Ρ������޸�UI_Split����
		int nCount = (int)vStrArray.size();
		if (0 != nCount)
		{
			for (int i = 0; i < nCount; i++)
			{
				if (vStrArray[i].empty())
					continue;

				StyleResItem* pClassStyleItem = this->GetStyleItem(STYLE_SELECTOR_TYPE_CLASS, vStrArray[i]);
				if (pClassStyleItem)
				{
					pClassStyleItem->ApplyMyAttributesToAnExistMap(pMapStyle);
				}
			}
		}
	}

	if (szTagName && _tcslen(szTagName) > 0)
	{
		StyleResItem* pTagStyleItem = this->GetStyleItem(STYLE_SELECTOR_TYPE_TAG, szTagName);
		if (pTagStyleItem)
		{
			pTagStyleItem->ApplyMyAttributesToAnExistMap(pMapStyle);
		}
	}

	return true;
}
}