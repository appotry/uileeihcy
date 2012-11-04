#include "StdAfx.h"


ProjectManager::ProjectManager()
{
	m_pProjectParse = NULL;
	m_pCurActiveSkinMgr = NULL;
	m_bDirty = false;
}
ProjectManager::~ProjectManager()
{
	if (m_bDirty)
	{
		SaveProjectInfo();
	}

	if( m_pProjectParse != NULL )
	{
		m_pProjectParse->Release(); 
		m_pProjectParse = NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// �ͷŸ�Ƥ�������ڴ�

	vector<SkinManager*>::iterator  iter = m_vSkinMgrs.begin();
	vector<SkinManager*>::iterator  iterEnd = m_vSkinMgrs.end();

	for ( ; iter != iterEnd; iter++ )
	{
		SkinManager* p = (*iter);
		delete p; 
	}
	m_vSkinMgrs.clear();
}
void ProjectManager::SetProjectParse(IProjectParse* p)
{
	if( m_pProjectParse != NULL )
	{
		m_pProjectParse->Release(); 
		m_pProjectParse = NULL;
	}

	m_pProjectParse = p;
}

//
//	����һ������
//
//	Parameter
//		strProjDir
//			[in]	�½���������Ŀ¼
//	
//		strProjName
//			[in]	�½���������
//
bool ProjectManager::CreateProject( const String& strProjDir, const String& strProjName )
{
	if( NULL != m_pProjectParse )
		return false;

	// 1. �����ļ�
	String  str = strProjDir + _T("\\") + strProjName + XML_PROJECT_XMLFILE_EXT;

	IProjectParse*  pParse = IParseClassFactory::CreateProjectParseInstance( str );
	if( false == pParse->Create() )
	{
		UI_LOG_ERROR(_T("ProjectManager::CreateProject  Create ProjectParse failed. path=%s"), str.c_str() );
		return false;
	}
	this->SetProjectParse(pParse);
	m_pojoProject.SetProjectName( strProjName );

	return true;
}

//
//	�򿪡�����һ�������ļ�
//
//		����ProjectManager::Initialize, bOnlyLoadActiveSkin = true����ʾ��Ӧ�ó�����õģ�ֻ��Ҫ���ص�ǰƤ��
//		����UI_OpenProjectʱ��bOnlyLoadActiveSkin = false����ʾ��UIBuilder���õģ��������е�Ƥ��
//
bool ProjectManager::OpenProject( const String& strProjFilePath, bool bOnlyLoadActiveSkin )
{
	if( false == bOnlyLoadActiveSkin )
	{
		UIASSERT( g_pUIApplication->IsDesignMode() );
	}

	if( NULL != m_pProjectParse )
		return false;

	m_pProjectParse = IParseClassFactory::CreateProjectParseInstance( strProjFilePath );

	// 1. ����Ƥ���б�
	if (false == m_pProjectParse->Load(&m_pojoProject))
	{
		UI_LOG_ERROR(_T("%s failed. project file path=%s."), FUNC_NAME, strProjFilePath.c_str() );
		m_pProjectParse->Release();
		m_pProjectParse = NULL;
		return false;
	}

	// 2. ����Ƥ����Դ
	int nSkinCount = m_pojoProject.GetSkinItemCount();
	if (0 == nSkinCount)
	{
		m_pProjectParse->Release();
		m_pProjectParse = NULL;
		UI_LOG_ERROR(_T("%s skin count is 0"), FUNC_NAME);
		return false;
	}

	int nActiveIndex = m_pojoProject.GetActiveSkinItemIndex();
	if (bOnlyLoadActiveSkin)
	{
		if (-1 == nActiveIndex)
			nActiveIndex = 0;

		m_pCurActiveSkinMgr = this->LoadSkin(m_pojoProject.GetSkinItem(0));
		if( NULL == m_pCurActiveSkinMgr )
		{
			UI_LOG_ERROR(_T("ProjectManager::OpenProject LoadSkin failed."));
		}
	}
	else
	{
		for (int i = 0; i < nSkinCount; i++)
		{
			SkinManager* pSkinMgr = this->LoadSkin(m_pojoProject.GetSkinItem(i));
			if( NULL == pSkinMgr )
			{
				UI_LOG_ERROR(_T("ProjectManager::OpenProject LoadSkin failed. index=%d"), i );
				continue;
			}

			if (nActiveIndex == i)
				m_pCurActiveSkinMgr = pSkinMgr;
		}
	}

	return true;
}


bool ProjectManager::Initialize( const String& strUIProjXmlPath )
{
	return this->OpenProject( strUIProjXmlPath, true );
}

//
//	Ӧ�ó����޸�Ƥ��
//
bool ProjectManager::ChangeSkin( int nIndex )
{
	CPojo_ProjectSkinItem* pSkinItem = m_pojoProject.GetSkinItem(nIndex);
	if( NULL == pSkinItem )
	{
		UI_LOG_ERROR(_T("ProjectManager::LoadSkin GetSkinItem Failed. skin index=%d"), nIndex );
		return false;
	}

	return this->ChangeSkin(pSkinItem);
}
bool ProjectManager::ChangeSkin( const String& strSkinName )
{
	CPojo_ProjectSkinItem* pSkinItem = m_pojoProject.GetSkinItem(strSkinName);
	if( NULL == pSkinItem )
	{
		UI_LOG_ERROR(_T("ProjectManager::LoadSkin GetSkinItem Failed. skin name=%s"), strSkinName.c_str() );
		return false;
	}

	return this->ChangeSkin(pSkinItem);
}

bool ProjectManager::ChangeSkin( CPojo_ProjectSkinItem* pSkinItem )
{
	if( NULL == pSkinItem )
		return false;

	SkinManager* pOldSkinMgr = m_pCurActiveSkinMgr;
	m_pCurActiveSkinMgr = NULL;
	m_vSkinMgrs.clear();

	SkinManager* pSkinMgr = this->LoadSkin(pSkinItem);
	if( NULL == pSkinMgr )
		return false;

	m_pCurActiveSkinMgr = pSkinMgr;
	g_pUIApplication->m_TopWindowMgr.ChangeSkin(pSkinMgr);
	m_pojoProject.SetActiveSkinName(pSkinMgr->GetSkinName());

	if( NULL != pOldSkinMgr )
	{
		delete pOldSkinMgr;
		pOldSkinMgr = NULL;
	}
	
	m_bDirty = true;
	return true;
}
bool ProjectManager::ChangeSkinHLS( short h, short l, short s, int nFlag )
{
	bool bRet = m_pCurActiveSkinMgr->ChangeSkinHLS(h,l,s,nFlag);
	if( false == bRet )
		return false;

	// ֪ͨ����ˢ��
	g_pUIApplication->m_TopWindowMgr.OnSkinHLSChanged(); //InvalidateWindow();

	// ����� HLS ����
	IProjectSkinItemInfo* pSkinItem = NULL;
	UI_GetProjectSkinItemInfo(m_pCurActiveSkinMgr, &pSkinItem);
	if (NULL != pSkinItem)
	{
		pSkinItem->SetHLS((char)h,(char)l,(char)s,nFlag);
	}
	m_bDirty = true;
	return true;
}

//
//	����Ƥ������
//
SkinManager* ProjectManager::LoadSkin(CPojo_ProjectSkinItem*  pSkinItem)
{
	if( NULL == pSkinItem )
		return false;

	SkinManager*   pSkinMgr  = NULL;
	ImageManager*  pImageMgr = NULL;
	ColorManager*  pColorMgr = NULL;
	FontManager*   pFontMgr  = NULL;
	StyleManager*  pStyleMgr = NULL;
	LayoutManager* pLayoutMgr = NULL;

	bool bRet = false;
	do 
	{
		pSkinMgr = new SkinManager(pSkinItem);

		//
		// ������Ƥ������Դ·��
		//

		if( false == pSkinMgr->LoadSkinInfo() )
		{
			UI_LOG_ERROR(_T("ProjectManager::LoadSkin skinmgr load skininfo failed."));
			break;
		}

		// ��ȡ������Դ����
		CPojo_SkinInfo* pSkinInfo = NULL;
		if( false == pSkinMgr->GetSkinInfoManager()->GetSkinInfo(&pSkinInfo) || NULL==pSkinInfo )
		{
			UI_LOG_ERROR(_T("ProjectManager::LoadSkin GetSkinInfo failed."));
			break;
		}

		//
		// ����image��Դ
		//

		pImageMgr = new ImageManager(pSkinMgr);
		if( false == pImageMgr->Load( pSkinInfo->GetImageXmlPath()) )
		{
			UI_LOG_ERROR( _T("ProjectManager::OpenProject  create imagemgr failed") );
			break;
		}
		pSkinMgr->SetImageManager(pImageMgr);

		//
		// ����color��Դ
		//

		pColorMgr = new ColorManager(pSkinMgr);
		if( false == pColorMgr->Load( pSkinInfo->GetColorXmlPath()) )
		{
			UI_LOG_ERROR( _T("ProjectManager::OpenProject  create color manager failed") );
			break;
		}
		pSkinMgr->SetColorManager(pColorMgr);

		//
		// ����font��Դ
		//

		pFontMgr = new FontManager;
		if( false == pFontMgr->Load( pSkinInfo->GetFontXmlPath()) )
		{
			UI_LOG_ERROR( _T("ProjectManager::OpenProject  create font manager failed") );
			break;
		}
		pSkinMgr->SetFontManager(pFontMgr);

		//
		//	����style��Դ
		//
		
		pStyleMgr = new StyleManager;
		if( false == pStyleMgr->Load( pSkinInfo->GetStyleXmlPath()) )
		{
			UI_LOG_ERROR( _T("ProjectManager::OpenProject  create style manager failed") );
			break;
		}
		pSkinMgr->SetStyleManager(pStyleMgr);
	
		pLayoutMgr = new LayoutManager;
		if( false == pLayoutMgr->Load( pSkinInfo->GetLayoutXmlPath()) )
		{
			UI_LOG_ERROR( _T("ProjectManager::OpenProject  create layout manager failed") );
			break;
		}
		pSkinMgr->SetLayoutManager(pLayoutMgr);

		bRet = true;
	}
	while (0);

	if( false == bRet )
	{
		// ��ΪSkinMgr���ͷŵ�ʱ�򣬻��Զ��ͷ�����������������Դmgr
		// ���������� GetxxxManager��Ϊ��ʱ�������ͷŸ���Դ�ڴ棬������ɶ����ͷ�

		if( NULL != pLayoutMgr && 
			(  NULL==pSkinMgr || (pSkinMgr!=NULL && NULL==pSkinMgr->GetLayoutManager()) ))
		{
			delete pLayoutMgr;
			pLayoutMgr = NULL;
		}

		if( NULL != pStyleMgr && 
			(  NULL==pSkinMgr || (pSkinMgr!=NULL && NULL==pSkinMgr->GetStyleManager()) ))
		{
			delete pStyleMgr;
			pStyleMgr = NULL;
		}

		if( NULL != pFontMgr && 
		   (  NULL==pSkinMgr || (pSkinMgr!=NULL && NULL==pSkinMgr->GetFontManager()) ))
		{
			delete pFontMgr;
			pFontMgr = NULL;
		}
		if( NULL != pColorMgr && 
			(  NULL==pSkinMgr || (pSkinMgr!=NULL && NULL==pSkinMgr->GetColorManager()) ))
		{
			delete pColorMgr;
			pColorMgr = NULL;
		}
		if( NULL != pImageMgr && 
			(  NULL==pSkinMgr || (pSkinMgr!=NULL && NULL==pSkinMgr->GetImageManager()) ))
		{
			delete pImageMgr;
			pImageMgr = NULL;
		}
		if( NULL != pSkinMgr )
		{
			delete pSkinMgr;
			pSkinMgr = NULL;
		}
		return false;
	}

	this->m_vSkinMgrs.push_back(pSkinMgr);
	return pSkinMgr;
}

//
//	����һ��Ƥ�����ڹ��������£�����һ��Ƥ���ļ��У�Ȼ����Ƥ���ļ�������
//	����Ƥ�����xml
//
//	Parameter
//		strSkinName
//			[in]	Ƥ������
//
//		strImageXmlPath
//			[in]	ͼƬ�����ļ�·��
//
//		strColorXmlPath
//			[in]	��ɫ�����ļ�·��
//
//		strFontXmlPath
//			[in]	���������ļ�·��
//
//	Return
//		�ɹ�����Ƥ�������ʧ�ܷ���NULL
//
HSKIN ProjectManager::CreateSkin( const String& strSkinName,      const String& strImageXmlPath, 
								  const String& strColorXmlPath,  const String& strFontXmlPath,
								  const String& strLayoutXmlPath, const String& strStyleXmlPath )
{
	if( NULL == m_pProjectParse )
	{
		UI_LOG_ERROR(_T("ProjectManager::CreateSkin failed, please create proejct first"));
		return NULL;
	}

	HSKIN hSkin = NULL; 

	//////////////////////////////////////////////////////////////////////////
	// 1. ����Ƥ��Ŀ¼
	
	String strProjDir;
	if( false == this->GetProjectDirectory(strProjDir) )
	{
		return NULL;
	}

	String strSkinDir = strProjDir + _T("\\") + strSkinName;
	if( FALSE == PathIsDirectory(strSkinDir.c_str()))
	{
		BOOL bRet = CreateDirectory( strSkinDir.c_str(), NULL );
		if( FALSE == bRet )
		{
			UI_LOG_FATAL(_T("UIApplication::CreateSkin  Create directory failed. path=%s"), strSkinDir.c_str() );
			return hSkin;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	// 2. ��Ƥ����Ϣ��ӵ�������

	String  str = strSkinDir + _T("\\ui.xml");
	hSkin = this->AddSkinMgrItem( strSkinName, str );
	if( NULL == hSkin )
	{
		UI_LOG_ERROR(_T("ProjectManager::CreateSkin  AddSkinMgrItem failed") );
		return NULL;
	}

	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkin failed") );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 3. ����ͼƬmanager

	if( false == this->CreateSkinImageMgr( pSkinManager, strImageXmlPath ) )
	{
		UI_LOG_ERROR(_T("ProjectManager::CreateSkin  CreateSkinImageMgr failed") );
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// 4. ������ɫmanager

	if( false == this->CreateSkinColorMgr( pSkinManager, strColorXmlPath ) )
	{
		UI_LOG_ERROR(_T("ProjectManager::CreateSkin  CreateSkinColorMgr failed") );
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// 5. ��������manager

	if( false == this->CreateSkinFontMgr( pSkinManager, strFontXmlPath ) )
	{
		UI_LOG_ERROR(_T("ProjectManager::CreateSkin  CreateSkinFontMgr failed") );
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// 6. ������ʽmanager

	if( false == this->CreateSkinStyleMgr( pSkinManager, strStyleXmlPath ) )
	{
		UI_LOG_ERROR(_T("ProjectManager::CreateSkin  CreateSkinStyleMgr failed") );
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// 7. ����layout manager

	if( false == this->CreateSkinLayoutMgr( pSkinManager, strLayoutXmlPath ) )
	{
		UI_LOG_ERROR(_T("ProjectManager::CreateSkin  CreateSkinLayoutMgr failed") );
		return NULL;
	}

	// �ύ���ݵ��ļ���
	UI_Save(hSkin);

	return hSkin;
}

// HSKIN ProjectManager::GetNextProjectSkin( HSKIN hSkin /*= NULL*/ )
// {
// 	assert(0);
// 	return NULL;
// }

// ���´���ʱ�ļ��ж�ȡ�µ�����
bool ProjectManager::ReloadResource( HSKIN hSkin, UI_RESOURCE_TYPE eResType )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if( NULL == pSkinMgr )
		return false;

	return pSkinMgr->ReloadResource(eResType);
}

//
//	���һ��Ƥ�������̵��У����ظ�Ƥ�����
//
//	Parameter
//		strSkinName
//			[in]	Ƥ������
//
//		strSkinXmlFullPath
//			[in]	Ƥ������Ӧ��ui.xml����·��
//
HSKIN ProjectManager::AddSkinMgrItem( const String& strSkinName, const String& strSkinXmlFullPath )
{
	HSKIN hSkin = NULL;

	//////////////////////////////////////////////////////////////////////////
	// 1. ����ui.xml

	ISkinInfoParse* pSkinInfoParse = IParseClassFactory::CreateSkinInfoParseInstance(strSkinXmlFullPath);
	if( false == pSkinInfoParse->Create() )
	{
		UI_LOG_ERROR(_T("ProjectManager::AddSkinMgrItem create skininfo failed."));
		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	// 2. ��ӵ��־��Զ���������

	CPojo_ProjectSkinItem* pSkinItem = new CPojo_ProjectSkinItem;
	pSkinItem->SetSkinName(strSkinName);
	pSkinItem->SetSkinXmlPath(strSkinXmlFullPath);

	m_pojoProject.AddSkinItem(pSkinItem);


	//////////////////////////////////////////////////////////////////////////
	// 3. ��ʼ��Ƥ������

	SkinManager* pSkinMgr = new SkinManager(pSkinItem);
	pSkinMgr->GetSkinInfoManager()->SetSkinInfoParse(pSkinInfoParse);


	//////////////////////////////////////////////////////////////////////////
	// 4. ��ӵ�����ʱ����������
	this->m_vSkinMgrs.push_back(pSkinMgr);

	hSkin = (HSKIN)pSkinMgr;
	return hSkin;
}

//
//	����Ƥ����ͼƬ����
//
bool ProjectManager::CreateSkinImageMgr( SkinManager* pSkinManager, const String&  strXmlPath )
{
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinImageMgr failed") );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���浽�־ö���������

	CPojo_SkinInfo* pSkininfo = NULL;
	pSkinManager->GetSkinInfoManager()->GetSkinInfo(&pSkininfo);
	if( NULL == pSkininfo )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinFontMgr failed. NULL == pSkininfo") );
		return false;
	}
	pSkininfo->SetImageXmlPath(strXmlPath);

	//////////////////////////////////////////////////////////////////////////
	//  ���ص�����ʱ����������

	ImageManager* pImageMgr = new ImageManager(pSkinManager);
	if( false == pImageMgr->Create(strXmlPath) )
	{
		delete pImageMgr;
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinImageMgr  create imagemgr failed") );
		return false;
	}

	if( false == pSkinManager->SetImageManager( pImageMgr ) )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinImageMgr  SetImageManager failed") );
		return false;
	}

	return true;
}

//
//	����Ƥ������ɫ����
//
bool ProjectManager::CreateSkinColorMgr( SkinManager* pSkinManager, const String&  strXmlPath )
{
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinColorMgr failed") );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���浽�־ö���������

	CPojo_SkinInfo* pSkininfo = NULL;
	pSkinManager->GetSkinInfoManager()->GetSkinInfo(&pSkininfo);
	if( NULL == pSkininfo )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinFontMgr failed. NULL == pSkininfo") );
		return false;
	}
	pSkininfo->SetColorXmlPath(strXmlPath);

	//////////////////////////////////////////////////////////////////////////
	//  ���ص�����ʱ����������

	
	ColorManager* pColorMgr = new ColorManager(pSkinManager);
	if( false == pColorMgr->Create( strXmlPath ) )
	{
		delete pColorMgr;
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinColorMgr  Create color mgr failed, path=%s"), strXmlPath.c_str() );
		return false;
	}

	if( false == pSkinManager->SetColorManager( pColorMgr ) )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinColorMgr  SetColorManager Failed") );
		return false;
	}

	return true;
}

//
//	����Ƥ�����������
//
bool ProjectManager::CreateSkinFontMgr( SkinManager* pSkinManager, const String&  strXmlPath )
{
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinFontMgr failed") );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���浽�־ö���������

	CPojo_SkinInfo* pSkininfo = NULL;
	pSkinManager->GetSkinInfoManager()->GetSkinInfo(&pSkininfo);
	if( NULL == pSkininfo )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinFontMgr failed. NULL == pSkininfo") );
		return false;
	}
	pSkininfo->SetFontXmlPath(strXmlPath);

	//////////////////////////////////////////////////////////////////////////
	//  ���ص�����ʱ����������

	FontManager* pFontMgr = new FontManager;
	if( false == pFontMgr->Create(strXmlPath) )
	{
		delete pFontMgr;
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinFontMgr  create font mgr failed, path=%s"), strXmlPath.c_str() );
		return false;
	}

	if( false == pSkinManager->SetFontManager( pFontMgr ) )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinFontMgr  SetFontManager Failed") );
		return false;
	}

	return true;
}

//
//	����Ƥ������ʽ����
//
bool ProjectManager::CreateSkinStyleMgr( SkinManager* pSkinManager, const String&  strXmlPath )
{
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinStyleMgr failed") );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���浽�־ö���������

	CPojo_SkinInfo* pSkininfo = NULL;
	pSkinManager->GetSkinInfoManager()->GetSkinInfo(&pSkininfo);
	if( NULL == pSkininfo )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinStyleMgr failed. NULL == pSkininfo") );
		return false;
	}
	pSkininfo->SetStyleXmlPath(strXmlPath);

	//////////////////////////////////////////////////////////////////////////
	//  ���ص�����ʱ����������

	StyleManager* pStyleMgr = new StyleManager;
	if( false == pStyleMgr->Create(strXmlPath) )
	{
		delete pStyleMgr;
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinStyleMgr  create style mgr failed, path=%s"), strXmlPath.c_str() );
		return false;
	}

	if( false == pSkinManager->SetStyleManager( pStyleMgr ) )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinStyleMgr  SetStyleManager Failed") );
		return false;
	}

	return true;
}


//
//	����Ƥ������ʽ����
//
bool ProjectManager::CreateSkinLayoutMgr( SkinManager* pSkinManager, const String&  strXmlPath )
{
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinLayoutMgr failed") );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���浽�־ö���������

	CPojo_SkinInfo* pSkininfo = NULL;
	pSkinManager->GetSkinInfoManager()->GetSkinInfo(&pSkininfo);
	if( NULL == pSkininfo )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinLayoutMgr failed. NULL == pSkininfo") );
		return false;
	}
	pSkininfo->SetLayoutXmlPath(strXmlPath);

	//////////////////////////////////////////////////////////////////////////
	//  ���ص�����ʱ����������

	LayoutManager* pLayoutMgr = new LayoutManager;
	if( false == pLayoutMgr->Create(strXmlPath) )
	{
		delete pLayoutMgr;
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinLayoutMgr  create layout mgr failed, path=%s"), strXmlPath.c_str() );
		return false;
	}

	if( false == pSkinManager->SetLayoutManager( pLayoutMgr ) )
	{
		UI_LOG_ERROR( _T("ProjectManager::CreateSkinLayoutMgr  SetLayoutManager Failed") );
		return false;
	}

	return true;
}

//
//	����ָ���Ķ���Ĳ������ݣ���xml�н�����pRootObj��object name��id�����Ҷ�������
//
bool ProjectManager::LoadLayout(Object* pRootObj)
{
	if( NULL == m_pCurActiveSkinMgr )
	{
		UI_LOG_ERROR( _T("%s m_pCurActiveSkinMgr == NULL."), _T(__FUNCTION__) );
		return false;
	}

	return this->m_pCurActiveSkinMgr->LoadLayout(pRootObj);
}

bool ProjectManager::LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle )
{
	if( NULL == m_pCurActiveSkinMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::LoadStyle m_pCurActiveSkinMgr == NULL.") );
		return false;
	}

	return this->m_pCurActiveSkinMgr->LoadStyle(strTagName, strStyleClass, strID, mapStyle );
}
Menu* ProjectManager::LoadMenu( const String& strMenuId )
{
	if( NULL == m_pCurActiveSkinMgr )
	{
		UI_LOG_ERROR( _T("%s m_pCurActiveSkinMgr == NULL."), _T(__FUNCTION__) );
		return NULL;
	}

	return this->m_pCurActiveSkinMgr->LoadMenu(strMenuId);
}

//
//	��ָ��Ƥ���в���һ��ͼƬ
//
bool ProjectManager::InsertImageItem( HSKIN hSkin, const String& strID, const String& strPath )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertImageItem failed") );
		return false;
	}

	ImageManager* pImgMgr = pSkinManager->GetImageManager();
	if( NULL == pImgMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertImageItem failed") );
		return false;
	}

	return pImgMgr->InsertImageItem( strID, strPath );
}
//
//	��ָ��Ƥ���в���һ����ɫ��Ϣ
//
bool ProjectManager::InsertColorItem( HSKIN hSkin, const String& strID, const String& strColor )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertColorItem failed") );
		return false;
	}

	ColorManager* pColorMgr = pSkinManager->GetColorManager();
	if( NULL == pColorMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertColorItem failed") );
		return false;
	}

	return pColorMgr->InsertColorItem( strID, strColor );
}


//
//	��ָ��Ƥ���в���һ��������Ϣ
//
bool ProjectManager::InsertFontItem ( HSKIN hSkin, const String& strID, LOGFONT* pLogFont )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertFontItem failed") );
		return false;
	}

	FontManager* pFontMgr = pSkinManager->GetFontManager();
	if( NULL == pFontMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertFontItem failed") );
		return false;
	}

	return pFontMgr->InsertFontItem( strID, pLogFont );
}
bool ProjectManager::InsertStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertStyleItem failed") );
		return false;
	}

	StyleManager* pStyleMgr = pSkinManager->GetStyleManager();
	if( NULL == pStyleMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertStyleItem failed") );
		return false;
	}

	return pStyleMgr->InsertStyleItem( type, strID, strInherit );
}
bool ProjectManager::InsertStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertStyleAttribute failed") );
		return false;
	}

	StyleManager* pStyleMgr = pSkinManager->GetStyleManager();
	if( NULL == pStyleMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::InsertStyleAttribute failed") );
		return false;
	}

	return pStyleMgr->InsertStyleAttribute( type, strID, strKey, strValue );
}
bool ProjectManager::ModifyImageItem( HSKIN hSkin, const String& strID, const String& strPath )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyImageItem failed._1") );
		return false;
	}

	ImageManager* pImageMgr = pSkinManager->GetImageManager();
	if( NULL == pImageMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyImageItem failed._2") );
		return false;
	}

	return pImageMgr->ModifyImageItem( strID, strPath );
}
bool ProjectManager::ModifyColorItem( HSKIN hSkin, const String& strID, const String& strColor )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyColorItem failed._1") );
		return false;
	}

	ColorManager* pColorMgr = pSkinManager->GetColorManager();
	if( NULL == pColorMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyColorItem failed._2") );
		return false;
	}

	return pColorMgr->ModifyColorItem( strID, strColor );
}

//
//	��ָ��Ƥ�����޸�һ��������Ϣ
//
bool ProjectManager::ModifyFontItem ( HSKIN hSkin, const String& strID, LOGFONT* pLogFont )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyFontItem failed._1") );
		return false;
	}

	FontManager* pFontMgr = pSkinManager->GetFontManager();
	if( NULL == pFontMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyFontItem failed._2") );
		return false;
	}

	return pFontMgr->ModifyFontItem( strID, pLogFont );
}

bool ProjectManager::ModifyStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyStyleItem failed") );
		return false;
	}

	StyleManager* pStyleMgr = pSkinManager->GetStyleManager();
	if( NULL == pStyleMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyStyleItem failed") );
		return false;
	}

	return pStyleMgr->ModifyStyleItem( type, strID, strInherit );
}

bool ProjectManager::ModifyStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyStyleAttribute failed") );
		return false;
	}

	StyleManager* pStyleMgr = pSkinManager->GetStyleManager();
	if( NULL == pStyleMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::ModifyStyleAttribute failed") );
		return false;
	}

	return pStyleMgr->ModifyStyleAttribute( type, strID, strKey, strValue );
}
//
//	ɾ��ָ��Ƥ���е�ָ��ͼƬ
//
bool ProjectManager::RemoveImageItem( HSKIN hSkin, const String& strID )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveImageItem failed._1") );
		return false;
	}
	ImageManager* pImageManager = pSkinManager->GetImageManager();
	if( NULL == pImageManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveImageItem failed._2") );
		return false;
	}

	return pImageManager->RemoveImageItem( strID );
}
//
//	ɾ��ָ��Ƥ���е�ָ������
//
bool ProjectManager::RemoveFontItem ( HSKIN hSkin, const String& strID )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveFontItem failed._1") );
		return false;
	}
	FontManager* pFontManager = pSkinManager->GetFontManager();
	if( NULL == pFontManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveFontItem failed._2") );
		return false;
	}

	return pFontManager->RemoveFontItem( strID );
}

//
//	ɾ��ָ��Ƥ���е�ָ����ɫ
//
bool ProjectManager::RemoveColorItem( HSKIN hSkin, const String& strID )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveColorItem failed._1") );
		return false;
	}
	ColorManager* pColorManager = pSkinManager->GetColorManager();
	if( NULL == pColorManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveColorItem failed._2") );
		return false;
	}

	return pColorManager->RemoveColorItem( strID );
}
bool ProjectManager::RemoveStyleItem( HSKIN hSkin, STYLE_SELECTOR_TYPE  type, const String& strID )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveStyleItem failed") );
		return false;
	}

	StyleManager* pStyleMgr = pSkinManager->GetStyleManager();
	if( NULL == pStyleMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveStyleItem failed") );
		return false;
	}

	return pStyleMgr->RemoveStyleItem( type, strID );
}
bool ProjectManager::RemoveStyleAttribute( HSKIN hSkin, STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveStyleAttribute failed") );
		return false;
	}

	StyleManager* pStyleMgr = pSkinManager->GetStyleManager();
	if( NULL == pStyleMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::RemoveStyleAttribute failed") );
		return false;
	}

	return pStyleMgr->RemoveStyleAttribute( type, strID, strKey);
}

//
//	���浽�ļ���
//
//		NULL - NULL ȫ������
//		NULL -  xx  ��ǰƤ����ָ����Դ
//		 xx  - NULL ָ��Ƥ����������Դ
//
bool ProjectManager::Save(HSKIN hSkin, UI_RESOURCE_TYPE eResType)
{
	// ���湤������
	bool bRet = SaveProjectInfo();

	//////////////////////////////////////////////////////////////////////////
	// �����Ƥ����Ϣ

	int nSkinCount = (int)m_vSkinMgrs.size();
	for (int i = 0; i < nSkinCount; i++ )
	{
		if( NULL != hSkin )
		{
			SkinManager* pSkinMgr = this->_GetSkinManagerByHSKIN(hSkin);
			if( NULL == pSkinMgr || pSkinMgr != m_vSkinMgrs[i] )
				continue;

			pSkinMgr->Save(eResType);
		}
		else
			m_vSkinMgrs[i]->Save(eResType);
	}

	return bRet;
}

// ���湤����Ϣ�ļ�
bool ProjectManager::SaveProjectInfo()
{
	assert( NULL != m_pProjectParse );
	if( NULL == m_pProjectParse )
		return false;

	//////////////////////////////////////////////////////////////////////////
	// ���湤����Ϣ

	bool bRet = m_pProjectParse->Save(&m_pojoProject);

	if (bRet)
		m_bDirty = false;

	return bRet;
}

//
//	��ȡһ��HSKIN��Ӧ����m_vSkinMgrs�е�����
//
//	ʧ�ܷ���-1
//
int ProjectManager::_HSKIN_2_Index( HSKIN h )
{
	if( NULL == h )
		return -1;

	SkinManager* pSkinMgr = this->_GetSkinManagerByHSKIN(h);
	if( NULL == pSkinMgr )
		return -1;

	int nSize = (int)m_vSkinMgrs.size();
	if( 0 == nSize )
		return -1;

	for ( int i = 0; i < nSize; i++ )
	{
		if( m_vSkinMgrs[i] == pSkinMgr ) 
			return i;
	}

	return -1;
}


//
//	��ȡƤ������
//
bool  ProjectManager::GetSkinName( HSKIN hSkin, String& str )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if ( NULL == pSkinMgr )
		return false;

	TCHAR szName[MAX_STRING_LEN] = _T("");
	if( false == pSkinMgr->GetSkinName(szName) )
		return false;

	str = szName;
	return true;
}

bool  ProjectManager::GetSkinName( int i, String& str )
{
	if( i < 0 || i >= this->GetSkinCount() )
		return false;

	CPojo_ProjectSkinItem* pItem = m_pojoProject.GetSkinItem(i);
	if( NULL == pItem )
		return false;

	str = pItem->GetSkinNameRef();
	return true;
}

//
//	ͨ�������ȡ����
//
SkinManager* ProjectManager::_GetSkinManagerByHSKIN( HSKIN hSkin )
{
	// ���ֻ��һ������ת��
	return (SkinManager*)hSkin;  
}

//
//	ö�ٵ�ǰ�����е�Ƥ��
//
int ProjectManager::GetSkinCount()
{
	return m_pojoProject.GetSkinItemCount();

	//return (int)m_vSkinMgrs.size();  <-- �ڷ�designģʽ�£�ֻ�����active skin�����¸�ֵһֱΪ1
}
//
//	���÷��غ��HSKIN����Ҫ����ReleaseHSkin�����ͷ�	
//
HSKIN ProjectManager::GetSkinHandle( int nIndex )
{
	int nSize = (int)m_vSkinMgrs.size();
	if (nIndex < 0 || nIndex >= nSize )
	{
		return NULL;
	}
	HSKIN hSkin = (HSKIN)m_vSkinMgrs[nIndex];
	return hSkin;
}

//
//	��ȡƤ����һЩ������Ϣ�����ظ��ⲿʹ��
//
bool ProjectManager::GetSkinItemInfo( int nIndex, IProjectSkinItemInfo** pSkinItem )
{
	if( NULL == pSkinItem )
		return false;

	CPojo_ProjectSkinItem* pItem = m_pojoProject.GetSkinItem( nIndex );
	if( NULL == pItem )
		return false;

	*pSkinItem = pItem;
	return true;
}

bool ProjectManager::GetSkinItemInfo( HSKIN hSkin, IProjectSkinItemInfo** ppSkinItem )
{
	if( NULL == ppSkinItem )
		return false;

	if (NULL == hSkin)
	{
		hSkin = m_pCurActiveSkinMgr;
	}

	CPojo_ProjectSkinItem* pItem = m_pojoProject.GetSkinItem( (SkinManager*)hSkin );
	if( NULL == pItem )
		return false;

	*ppSkinItem = pItem;
	return true;
}

int ProjectManager::GetSkinImageCount( HSKIN hSkin )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if( NULL == pSkinMgr )
		return -1;

	return pSkinMgr->GetImageCount();
}
bool ProjectManager::GetSkinImageItemInfo( HSKIN hSkin, int nIndex, IImageItemInfo** ppImageItemInfo )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if( NULL == pSkinMgr )
		return false;

	return pSkinMgr->GetImageItemInfo( nIndex, ppImageItemInfo );
}
int ProjectManager::GetSkinColorCount( HSKIN hSkin )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if( NULL == pSkinMgr )
		return false;

	return pSkinMgr->GetColorCount();
}
bool ProjectManager::GetSkinColorItemInfo(  HSKIN hSkin, int nIndex, IColorItemInfo** ppColorItemInfo )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if( NULL == pSkinMgr )
		return false;

	return pSkinMgr->GetColorItemInfo( nIndex, ppColorItemInfo );
}
int ProjectManager::GetSkinFontCount(  HSKIN hSkin )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if( NULL == pSkinMgr )
		return false;

	return pSkinMgr->GetFontCount();
}
bool ProjectManager::GetSkinFontItemInfo(  HSKIN hSkin, int nIndex, IFontItemInfo** ppFontItemInfo )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if( NULL == pSkinMgr )
		return false;

	return pSkinMgr->GetFontItemInfo( nIndex, ppFontItemInfo );
}
int ProjectManager::GetSkinStyleCount( HSKIN hSkin )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if( NULL == pSkinMgr )
		return false;

	return pSkinMgr->GetStyleCount();
}
bool ProjectManager::GetSkinStyleItemInfo( HSKIN hSkin, int nIndex, IStyleItemInfo** ppStyleItemInfo )
{
	SkinManager* pSkinMgr = _GetSkinManagerByHSKIN(hSkin);
	if( NULL == pSkinMgr )
		return false;

	return pSkinMgr->GetStyleItemInfo( nIndex, ppStyleItemInfo );
}


//
//	��ȡͼƬ.xml����·�������ڽ������ͼƬ��������Ŀ¼֮��
//
bool ProjectManager::GetResourceDataSourcePath( HSKIN hSkin, UI_RESOURCE_TYPE eResType, String& strRet )
{
	SkinManager* pSkinManager = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinManager )
	{
		UI_LOG_ERROR( _T("ProjectManager::GetImageDataSourcePath failed") );
		return false;
	}

	return pSkinManager->GetResourceDataSourcePath(eResType, strRet);
}

//
// ���ڣ�ʹ��HRBITMAP����
//
// bool ProjectManager::GetImage( const String& strImageID, UIImage** ppImage, HSKIN hSkin/* = NULL*/ )
// {
// 	// ��ȡ��ǰ��ʹ�õ�Ƥ��
// 	SkinManager*  pSkinMgr = (NULL == hSkin)? m_pCurActiveSkinMgr:this->_GetSkinManagerByHSKIN( hSkin );
// 	if( NULL == pSkinMgr )
// 	{
// 		UI_LOG_ERROR( _T("ProjectManager::GetImage  Failed. pSkinMgr == NULL") );
// 		return false;
// 	}
// 
// 	return pSkinMgr->GetImage( strImageID, ppImage );
// }

bool ProjectManager::GetColor( const String& strColorID, UIColor** pCol, HSKIN hSkin/* = NULL*/ )
{
	// ��ȡ��ǰ��ʹ�õ�Ƥ��
	SkinManager*  pSkinMgr = (NULL == hSkin)? m_pCurActiveSkinMgr:this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::GetImage  Failed. pSkinMgr == NULL") );
		return false;
	}

	return pSkinMgr->GetColor( strColorID, pCol );
}
//
//	���ڣ�ʹ��HRFONT����
// 
// bool ProjectManager::GetFont( const String& strFontID, UIFont** ppFont, HSKIN hSkin/* = NULL*/ )
// {
// 	// ��ȡ��ǰ��ʹ�õ�Ƥ��
// 	SkinManager*  pSkinMgr = (NULL == hSkin)? m_pCurActiveSkinMgr:this->_GetSkinManagerByHSKIN( hSkin );
// 	if( NULL == pSkinMgr )
// 	{
// 		UI_LOG_ERROR( _T("ProjectManager::GetFont  Failed. pSkinMgr == NULL") );
// 		return false;
// 	}
// 
// 	return pSkinMgr->GetFont( strFontID, ppFont );
// }

bool ProjectManager::GetCursor( const String& strCursorID, UICursor** ppCursor, HSKIN hSkin /*= NULL*/ )
{
	// ��ȡ��ǰ��ʹ�õ�Ƥ��
	SkinManager*  pSkinMgr = (NULL == hSkin)? m_pCurActiveSkinMgr:this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::GetCursor  Failed. pSkinMgr == NULL") );
		return false;
	}

	return pSkinMgr->GetCursor( strCursorID, ppCursor );
}

HRBITMAP ProjectManager::GetImage( const String& strImageID, GRAPHICS_RENDER_TYPE eRenderType, HSKIN hSkin )
{
	// ��ȡ��ǰ��ʹ�õ�Ƥ��
	SkinManager*  pSkinMgr = (NULL == hSkin)? m_pCurActiveSkinMgr:this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::GetImage  Failed. pSkinMgr == NULL") );
		return false;
	}

	return pSkinMgr->GetImage( strImageID, eRenderType );
}

HRFONT ProjectManager::GetFontEx( HRFONT hRFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_TYPE eRenderType, HSKIN hSkin )
{
	SkinManager*  pSkinMgr = (NULL == hSkin)? m_pCurActiveSkinMgr:this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::GetFontEx  Failed. pSkinMgr == NULL") );
		return NULL;
	}

	return pSkinMgr->GetFontEx( hRFont, w, l, eRenderType );
}
HRFONT ProjectManager::GetFont( const String& strFontID, GRAPHICS_RENDER_TYPE eRenderType , HSKIN hSkin )
{
 	// ��ȡ��ǰ��ʹ�õ�Ƥ��
 	SkinManager*  pSkinMgr = (NULL == hSkin)? m_pCurActiveSkinMgr:this->_GetSkinManagerByHSKIN( hSkin );
 	if( NULL == pSkinMgr )
 	{
 		UI_LOG_ERROR( _T("ProjectManager::GetFont  Failed. pSkinMgr == NULL") );
 		return NULL;
 	}
 
 	return pSkinMgr->GetFont( strFontID, eRenderType );
}

HRFONT ProjectManager::GetDefaultFont( GRAPHICS_RENDER_TYPE eRenderType , HSKIN hSkin )
{
	// ��ȡ��ǰ��ʹ�õ�Ƥ��
	SkinManager*  pSkinMgr = (NULL == hSkin)? m_pCurActiveSkinMgr:this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinMgr )
	{
		UI_LOG_ERROR( _T("ProjectManager::GetDefaultFont  Failed. pSkinMgr == NULL") );
		return NULL;
	}

	return pSkinMgr->GetDefaultFont( eRenderType );
}
//
//	���õ�ǰ�ĻƤ��������Ƥ���༭�������ڱ༭��Ƥ����
//
//	һЩGET��������Ĭ������ڵ�ǰƤ�����Ե�
//
bool ProjectManager::SetActiveSkin( HSKIN hSkin )
{
	SkinManager*  pSkinMgr = this->_GetSkinManagerByHSKIN( hSkin );
	if( NULL == pSkinMgr )
	{
		UI_LOG_WARN(_T("ProjectManager::SetActiveSkin failed"));
		return false;
	}

	const String strSkinName = pSkinMgr->GetSkinName();
	if( strSkinName.empty() )
	{
		return false;
	}
	
	m_pojoProject.SetActiveSkinName( strSkinName  );
	m_pCurActiveSkinMgr = pSkinMgr;

	return true;
}

//
//	��ȡ��ǰƤ��������ֵ
//
int ProjectManager::GetActiveSkinIndex()
{
	int nActiveIndex = m_pojoProject.GetActiveSkinItemIndex();	
	return nActiveIndex;
}
//
//	��ȡ����·��
//
bool ProjectManager::GetProjectDirectory( TCHAR* szRetDir )
{
	if ( NULL == szRetDir )
		return false;

	String strDir;
	if( false == this->GetProjectDirectory(strDir) )
		return false;

	_tcscpy( szRetDir, strDir.c_str() );

	return true;
}
bool ProjectManager::GetProjectDirectory(String& strDir)
{
	if ( NULL == m_pProjectParse )
		return false;

	const String& str = m_pProjectParse->GetDataSourceStringRef();
	String::size_type nPos = str.find_last_of( _T('\\') );
	if( String::npos == nPos )
		return false;

	strDir = str.substr( 0, nPos );
	return true;
}

bool ProjectManager::GetProjectPath( TCHAR* szRetPath )
{
	if ( NULL == szRetPath )
		return false;

	if ( NULL == m_pProjectParse )
		return false;

	const String& str = m_pProjectParse->GetDataSourceStringRef();
	_tcscpy( szRetPath, str.c_str() );

	return true;
}

//
//	��ȡ�������ƣ�pnSizeָszName��������С��ʧ��ʱ��pnSize��������Ҫ�Ĵ�С
//
bool ProjectManager::GetProjectName( TCHAR* szName, int* pnSize )
{
	if ( NULL == pnSize )
		return false;

	const String& str = m_pojoProject.GetProjectNameRef();
	if( *pnSize <= (int)str.length() )
	{
		*pnSize = str.length()+1;
		return false;
	}

	if( NULL == szName )
		return false;

	_tcscpy( szName, str.c_str() );
	return true;
}