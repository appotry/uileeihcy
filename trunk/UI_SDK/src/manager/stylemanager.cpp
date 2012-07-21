#include "StdAfx.h"



//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             ×ÖÌå                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////




StyleManager::StyleManager(void)
{
	m_pStyleParse = NULL;
}

StyleManager::~StyleManager(void)
{
	this->Clear();
	if( NULL!=m_pStyleParse)
	{
		m_pStyleParse->Release();
		m_pStyleParse = NULL;
	}
}

bool StyleManager::Create( const String& strXmlPath )
{
	if( NULL != m_pStyleParse )
	{
		UI_LOG_WARN(_T("StyleManager::Create style parse already exist."));
		return false;
	}

	m_pStyleParse = IParseClassFactory::CreateStyleParseInstance( strXmlPath );
	if( false == m_pStyleParse->Create() )
	{
		UI_LOG_ERROR(_T("StyleManager::Create create style parse failed. path=%s"), strXmlPath.c_str());
		return false;
	}

	return true;
}
bool StyleManager::Load( const String& strXmlPath )
{
	if( NULL != m_pStyleParse )
	{
		UI_LOG_WARN(_T("StyleManager::Load style parse already exist."));
		return false;
	}
	m_pStyleParse = IParseClassFactory::CreateStyleParseInstance( strXmlPath );

	if( false == m_pStyleParse->Load(&m_pojoStyle) )
		return false;

	return true;
}

bool StyleManager::Reload()
{
	if( NULL == m_pStyleParse )
	{
		UI_LOG_ERROR(_T("StyleManager::Reload m_pStyleParse == NULL."));
		return false;
	}

	this->Clear();
	return  m_pStyleParse->Reload( &m_pojoStyle );
}

bool StyleManager::Save()
{
	assert( NULL != m_pStyleParse );
	if( NULL == m_pStyleParse )
		return false;

	return m_pStyleParse->Save(&m_pojoStyle);
}

void StyleManager::Clear()
{
	m_pojoStyle.Clear();
}

int StyleManager::GetStyleCount( )
{
	return m_pojoStyle.GetStyleCount();
}
bool StyleManager::GetStyleItemInfo( int nIndex, IStyleItemInfo** ppStyleItemInfo )
{
	CPojo_StyleItem* pItem = this->m_pojoStyle.GetStyleItem(nIndex);
	if( NULL == pItem )
		return false;

	*ppStyleItemInfo = (IStyleItemInfo*)pItem;
	return true;
}

bool StyleManager::InsertStyleItem( STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit )
{
	if( NULL == m_pStyleParse )
		return false;

	if( false == m_pojoStyle.InsertStyle( type, strID, strInherit) )
	{
		UI_LOG_ERROR(_T("StyleManager::InsertStyleItem m_pojoStyle.InsertStyleItem strID=%s"), strID.c_str());
		return false;
	}

	return m_pStyleParse->InsertStyle( this->m_pojoStyle.GetStyleItem(type, strID) );
}
bool StyleManager::ModifyStyleItem( STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit )
{
	if( NULL == m_pStyleParse )
		return false;

	if( false == m_pojoStyle.ModifyStyle(type,strID,strInherit) )
	{
		UI_LOG_ERROR(_T("ColorManager::ModifyColorItem  m_pojoStyle.ModifyStyle strID=%s Failed. "), strID.c_str() );
		return false;
	}

	return m_pStyleParse->ModifyStyle( this->m_pojoStyle.GetStyleItem(type, strID) );
}
bool StyleManager::RemoveStyleItem( STYLE_SELECTOR_TYPE  type, const String& strID )
{
	if( NULL == m_pStyleParse )
		return false;

	if( false == m_pojoStyle.RemoveStyle(type,strID) )
	{
		UI_LOG_ERROR(_T("ColorManager::RemoveColorItem  m_pojoStyle.RemoveColor strID=%s Failed. "), strID.c_str() );
		return false;
	}

	return m_pStyleParse->RemoveStyle( type,strID );
}

bool StyleManager::InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	if( NULL == m_pStyleParse )
		return false;

	if( false == m_pojoStyle.InsertStyleAttribute(type, strID, strKey, strValue ) )
		return false;

	return m_pStyleParse->InsertStyleAttribute(type, strID, strKey, strValue);
}
bool StyleManager::ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	if( NULL == m_pStyleParse )
		return false;

	if( false == m_pojoStyle.ModifyStyleAttribute(type, strID, strKey, strValue ) )
		return false;

	return m_pStyleParse->ModifyStyleAttribute(type, strID, strKey, strValue);
}

bool StyleManager::RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey )
{
	if( NULL == m_pStyleParse )
		return false;

	if( false == m_pojoStyle.RemoveStyleAttribute(type, strID, strKey ) )
		return false;

	return m_pStyleParse->RemoveStyleAttribute(type, strID, strKey);
}


bool StyleManager::LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle )
{
	return m_pojoStyle.LoadStyle( strTagName, strStyleClass, strID, mapStyle );
}