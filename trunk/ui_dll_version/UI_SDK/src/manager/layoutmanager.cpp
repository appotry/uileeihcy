#include "StdAfx.h"



//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             ����                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////




LayoutManager::LayoutManager(void)
{
	m_pLayoutParse = NULL;
}

LayoutManager::~LayoutManager(void)
{
	this->Clear();
	if( NULL!=m_pLayoutParse)
	{
		m_pLayoutParse->Release();
		m_pLayoutParse = NULL;
	}
}

bool LayoutManager::Create( const String& strXmlPath )
{
	if( NULL != m_pLayoutParse )
	{
		UI_LOG_WARN(_T("LayoutManager::Create layout parse already exist."));
		return false;
	}

	m_pLayoutParse = IParseClassFactory::CreateLayoutParseInstance( strXmlPath );
	if( false == m_pLayoutParse->Create() )
	{
		UI_LOG_ERROR(_T("LayoutManager::Create create layout parse failed. path=%s"), strXmlPath.c_str());
		return false;
	}

	return true;
}
bool LayoutManager::Load( const String& strXmlPath )
{
	if( NULL != m_pLayoutParse )
	{
		UI_LOG_WARN(_T("LayoutManager::Load layout parse already exist."));
		return false;
	}
	m_pLayoutParse = IParseClassFactory::CreateLayoutParseInstance( strXmlPath );

	if( false == m_pLayoutParse->Load(&m_pojoLayout) )
		return false;

	return true;
}

bool LayoutManager::Save()
{
	assert( NULL != m_pLayoutParse );
	if( NULL == m_pLayoutParse )
		return false;

	return m_pLayoutParse->Save(&m_pojoLayout);
}

void LayoutManager::Clear()
{

}
//
//	����ָ���Ķ���Ĳ������ݣ���xml�н�����pRootObj��object name��id�����Ҷ�������
//
bool LayoutManager::LoadLayout(Object* pRootObj)
{
	if( NULL == m_pLayoutParse )
	{
		UI_LOG_ERROR(_T("%s m_pLayoutParse == NULL"),_T(__FUNCTION__));
		return false;
	}
	
	return m_pLayoutParse->LoadLayout(pRootObj);
}

//
//	����ָ���Ķ���Ĳ������ݣ���xml�н�����pRootObj��object name��id������ԭ�ж���������ԺͲ���
//
bool LayoutManager::ReLoadLayout( Object* pRootObj, list<Object*>& listAllChild )
{
	if( NULL == m_pLayoutParse )
	{
		UI_LOG_ERROR(_T("LayoutManager::ReLoadLayout m_pLayoutParse == NULL"));
		return false;
	}

	return m_pLayoutParse->ReLoadLayout(pRootObj, listAllChild);
}

Menu* LayoutManager::LoadMenu( const String& strMenuId )
{

	if( NULL == m_pLayoutParse )
	{
		UI_LOG_ERROR(_T("%s m_pLayoutParse == NULL"),_T(__FUNCTION__));
		return NULL;
	}

	return m_pLayoutParse->LoadMenu(strMenuId);
}