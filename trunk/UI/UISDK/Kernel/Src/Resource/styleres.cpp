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
// 仅从m_vInheritList中删除，不从m_strInherit中删除
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
    // 继承一个属性值。如果自己已经定义了这个属性，则使用自身的值
    if (NULL == m_pMapAttrib->GetAttr(key.c_str(), false))
    {
        m_pMapAttrib->AddAttr(key.c_str(), value.c_str());
        return true;
    }
	
	return false;
}

//
// 将自己的属性继承给另一个item
//	
//	如果pChild已经有这个属性了，则使用pChild的属性
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
//	将自己的属性应用到一个已有的map中，如果map中已有这个属性，则使用该map的属性
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
//	返回匹配的样式
//
//	Parameter
//		strTagName
//			[in]	要匹配的标签名
//		strStyleClass
//			[in]	要匹配的class样式
//		strID
//			[in]	要匹配的id样式
//
//		mapStyle
//			[in/out] 将结果返回到该map中
//
//	Remark
//		优化级定义：
//			级别1. 在layout.xml中直接指定的属性
//			级别2. 通过ID匹配的属性
//			级别3. 通过CLASS匹配的属性
//			级别4. CLASS列表中，排后面的样式匹配的属性 > 前面的样式匹配的属性
//			级别5. 通过TAG匹配的属性
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

	// strStyleClass可能有多个
	if (szStyleClass && _tcslen(szStyleClass) > 0)
	{
		vector<String>  vStrArray;
		UI_Split(szStyleClass, XML_SEPARATOR, vStrArray);   // TODO: 这里的第一个参数还得转换为String，多了一次。建议修改UI_Split参数
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