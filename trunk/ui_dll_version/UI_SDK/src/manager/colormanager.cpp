#include "StdAfx.h"


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             颜色                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////



ColorManager::ColorManager(SkinManager* pSkinMgr): m_pojoColor(pSkinMgr)
{
	m_pColorParse = NULL;
	m_pSkinMgr = pSkinMgr;
}

ColorManager::~ColorManager(void)
{
	this->Clear();
	if( NULL != m_pColorParse )
	{
		m_pColorParse->Release();
		m_pColorParse = NULL;
	}
}

//
//	新建一个颜色管理文件
//
bool ColorManager::Create(const String& strXmlPath )
{
	if( NULL != m_pColorParse )
	{
		UI_LOG_WARN(_T("ColorManager::Create color parse already exist."));
		return false;
	}

	m_pColorParse = IParseClassFactory::CreateColorParseInstance( strXmlPath );
	if( false == m_pColorParse->Create() )
	{
		UI_LOG_ERROR(_T("ColorManager::Create create color parse failed. path=%s"), strXmlPath.c_str());
		return false;
	}

	return true;
}
bool ColorManager::Load( const String& strXmlPath )
{
	if( NULL != m_pColorParse )
	{
		UI_LOG_WARN(_T("ColorManager::Load color parse already exist."));
		return false;
	}

	m_pColorParse = IParseClassFactory::CreateColorParseInstance( strXmlPath );
	if( false == m_pColorParse->Load(&m_pojoColor) )
		return false;

	return true;
}

bool ColorManager::Reload()
{
	if( NULL == m_pColorParse )
	{
		UI_LOG_ERROR(_T("ColorManager::Reload m_pColorParse == NULL."));
		return false;
	}

	this->Clear();
	return  m_pColorParse->Reload( &m_pojoColor );
}

bool ColorManager::Save()
{
	assert( NULL != m_pColorParse );
	if( NULL == m_pColorParse )
		return false;

	return m_pColorParse->Save(&m_pojoColor);
}
/*
**	清除所有col资源
*/
void ColorManager::Clear()
{
	m_pojoColor.Clear();
}

int ColorManager::GetColorCount()
{
	return m_pojoColor.GetColorCount();
}
bool ColorManager::GetColor( const String& strName, UIColor** pCol )
{
	return m_pojoColor.GetColor(strName, pCol );
}

bool ColorManager::GetColorItemInfo( int nIndex, IColorItemInfo** ppColorItemInfo )
{
	CPojo_ColorItem* pItem = this->m_pojoColor.GetColorItem(nIndex);
	if( NULL == pItem )
		return false;

	*ppColorItemInfo = (IColorItemInfo*)pItem;
	return true;
}

bool ColorManager::InsertColorItem( const String& strID, const String& strColor )
{
	if( NULL == m_pColorParse )
		return false;

	if( false == m_pojoColor.InsertColor(strID,strColor,NULL) )
	{
		UI_LOG_ERROR(_T("ColorManager::InsertColorItem  m_pojoColor.InsertColorItem strID=%s"), strID.c_str());
		return false;
	}

	return m_pColorParse->InsertColor( this->m_pojoColor.GetColorItem(strID) );
}
bool ColorManager::ModifyColorItem( const String& strID, const String& strColor )
{
	if( NULL == m_pColorParse )
		return false;

	if( false == m_pojoColor.ModifyColor(strID,strColor) )
	{
		UI_LOG_ERROR(_T("ColorManager::ModifyColorItem  m_pojoColor.ModifyColorItem strID=%s Failed. "), strID.c_str() );
		return false;
	}

	return m_pColorParse->ModifyColor( this->m_pojoColor.GetColorItem(strID) );
}
bool ColorManager::RemoveColorItem( const String& strID )
{
	if( NULL == m_pColorParse )
		return false;

	if( false == m_pojoColor.RemoveColor(strID) )
	{
		UI_LOG_ERROR(_T("ColorManager::RemoveColorItem  m_pojoColor.RemoveColor strID=%s Failed. "), strID.c_str() );
		return false;
	}

	return m_pColorParse->RemoveColor( strID );
}

bool ColorManager::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	return m_pojoColor.ChangeSkinHLS(h,l,s,nFlag);
}