#include "StdAfx.h"
#include "skinres.h"
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Resource\imagemanager.h"
#include "UISDK\Kernel\Src\Resource\colormanager.h"
#include "UISDK\Kernel\Src\Resource\stylemanager.h"
#include "UISDK\Kernel\Src\Resource\layoutmanager.h"
#include "UISDK\Kernel\Src\Resource\fontmanager.h"
#include "UISDK\Kernel\Src\Resource\skinmanager.h"
#include "UISDK\Kernel\Src\SkinParse\datasource\skindatasource.h"

//////////////////////////////////////////////////////////////////////////
//
//                             Skin Manager
//
//////////////////////////////////////////////////////////////////////////


//
//	pSkinItem中记录了该皮肤的名称以及该皮肤的路径
//
SkinRes::SkinRes()
{	
    m_pISkinRes = NULL;
    m_eLoadState = SKIN_RES_LOAD_STATE_NONE;
	m_pImageMgr = NULL;
	m_pColorMgr = NULL;
	m_pFontMgr  = NULL;
	m_pStyleMgr = NULL;
	m_pLayoutMgr= NULL;
	m_pSkinMgr = NULL;
    m_pDataSource = NULL;
    m_pHLSInfo = NULL;
}
SkinRes::~SkinRes()
{
    Unload();
    SAFE_DELETE(m_pISkinRes);
    SAFE_DELETE(m_pHLSInfo);
    SAFE_RELEASE(m_pDataSource);
}

ISkinRes*  SkinRes::GetISkinRes()
{
    if (NULL == m_pISkinRes)
    {
        m_pISkinRes = new ISkinRes(this);
    }
    return m_pISkinRes; 
}

void  SkinRes::SetParam(SKIN_PACKET_TYPE eType, const TCHAR* szName, const TCHAR* szPath)
{
    SAFE_DELETE(m_pDataSource);
    CreateDataSourceInstance(eType, &m_pDataSource);
    m_pDataSource->SetDirection(szPath);
    m_pDataSource->SetName(szName);
}
HRESULT  SkinRes::SetSkinMgr(SkinManager* p)
{
	m_pSkinMgr = p;
	return S_OK;
}

const TCHAR*  SkinRes::GetName()
{ 
    if (NULL == m_pDataSource)
        return NULL;

    return m_pDataSource->GetName(); 
}
const TCHAR*  SkinRes::GetPath() 
{ 
    if (NULL == m_pDataSource)
        return NULL;

    return m_pDataSource->GetDirection(); 
}

IUIApplication* SkinRes::GetUIApplication()
{
	if (NULL == m_pSkinMgr)
		return NULL;

	return m_pSkinMgr->GetUIApplication();
}
ISkinManager* SkinRes::GetISkinManager()
{
	if (NULL == m_pSkinMgr)
		return NULL;

	return m_pSkinMgr->GetISkinManager();;
}


IImageManager* SkinRes::GetIImageManager()
{
	if (NULL == m_pImageMgr)
		return NULL;

	return m_pImageMgr->GetIImageManager();
}

IImageRes*  SkinRes::GetIImageRes()
{
	if (NULL == m_pImageMgr)
		return NULL;

	return m_pImageMgr->GetImageRes();
}
IColorManager* SkinRes::GetIColorManager()
{
	if (NULL == m_pColorMgr)
		return NULL;

	return m_pColorMgr->GetIColorManager();
}
IColorRes* SkinRes::GetIColorRes()
{
	if (NULL == m_pColorMgr)
		return NULL;

	return m_pColorMgr->GetColorRes();
}
IFontManager* SkinRes::GetIFontManager()
{
	if (NULL == m_pFontMgr)
		return NULL;

	return m_pFontMgr->GetIFontManager();
}
IFontRes*  SkinRes::GetIFontRes()
{
	if (NULL == m_pFontMgr)
		return NULL;

	return m_pFontMgr->GetFontRes();
}
IStyleManager* SkinRes::GetIStyleManager()
{
	if (NULL == m_pStyleMgr)
		return NULL;

	return m_pStyleMgr->GetIStyleManager();;
}
IStyleRes* SkinRes::GetIStyleRes()
{
	if (NULL == m_pStyleMgr)
		return NULL;

	return m_pStyleMgr->GetStyleRes();
}

ILayoutManager* SkinRes::GetILayoutManager()
{
	if (NULL == m_pLayoutMgr)
		return NULL;

	return m_pLayoutMgr->GetILayoutManager();
}


// 重新从Temp文件中读取数据
bool SkinRes::ReloadResource(UI_RESOURCE_TYPE eResType)
{
	switch(eResType)
	{
	case UI_RESOURCE_IMAGE:
		return m_pImageMgr->Reload();
		break;
	case UI_RESOURCE_COLOR:
		return m_pColorMgr->Reload();
		break;
	case UI_RESOURCE_FONT:
		return m_pFontMgr->Reload();
		break;
	case  UI_RESOURCE_STYLE:
		return m_pStyleMgr->Reload();
		break;
	case UI_RESOURCE_LAYOUT:
		break;
	}

	return false;
}

bool  SkinRes::Load()
{
    if (m_eLoadState == SKIN_RES_LOAD_STATE_LOADED)
        return true;

    IUIApplication* pUIApp = GetUIApplication();
    SkinParseEngine parse(pUIApp, this);

    if (NULL == m_pImageMgr)
        m_pImageMgr = new ImageManager(this, pUIApp);

    if (NULL == m_pColorMgr)
        m_pColorMgr = new ColorManager(this);

    if (NULL == m_pFontMgr)
        m_pFontMgr = new FontManager(this);

    if (NULL == m_pStyleMgr)
        m_pStyleMgr = new StyleManager(this);

    if (NULL == m_pLayoutMgr)
        m_pLayoutMgr = new LayoutManager(this);

    if (m_pDataSource)
        m_pDataSource->Init();

    if (parse.ProcessFile(XML_SKIN_XML))
    {
        m_eLoadState = SKIN_RES_LOAD_STATE_LOADED;
    }   

#if 0
    switch (m_ePacketType)
    {
    case SKIN_PACKET_TYPE_DIR:
        {
            String  strXmlFile = m_strPath;
            strXmlFile.append(_T("\\") XML_SKIN_XML);
            _Load_Dir(strXmlFile.c_str());
        }
		break;

	case SKIN_PACKET_TYPE_ZIP:
		{
     		_Load_Zip(m_strPath.c_str());
		}
		break;
    }
   
    m_eLoadState = SKIN_RES_LOAD_STATE_LOADED;
#endif
    return true;
}


bool  SkinRes::_Load_Dir(const TCHAR* szXml)
{
	IUIApplication* pUIApp = GetUIApplication();
    SkinParseEngine parse(pUIApp, this);

    if (NULL == m_pImageMgr)
        m_pImageMgr = new ImageManager(this, pUIApp);

    if (NULL == m_pColorMgr)
        m_pColorMgr = new ColorManager(this);

    if (NULL == m_pFontMgr)
        m_pFontMgr = new FontManager(this);

    if (NULL == m_pStyleMgr)
        m_pStyleMgr = new StyleManager(this);

    if (NULL == m_pLayoutMgr)
        m_pLayoutMgr = new LayoutManager(this);

    if (parse.ProcessFile(szXml))
    {
        m_eLoadState = SKIN_RES_LOAD_STATE_LOADED;
    }   
    return false;
}

bool  SkinRes::_Load_Zip(const TCHAR* szZip)
{

	return false;
}

bool  SkinRes::Unload()
{
    if (m_eLoadState != SKIN_RES_LOAD_STATE_LOADED)
        return false;

    SAFE_DELETE(m_pImageMgr);
    SAFE_DELETE(m_pColorMgr);
    SAFE_DELETE(m_pFontMgr);
    SAFE_DELETE(m_pStyleMgr);
    SAFE_DELETE(m_pLayoutMgr);

    _DocList::iterator iter = m_listDoc.begin();
    for (; iter != m_listDoc.end(); iter++)
    {
        UISkinResDocInfo*  p = *iter;
        SAFE_RELEASE(p->m_pDoc);
        SAFE_DELETE(p);
    }
    m_listDoc.clear();

    m_eLoadState = SKIN_RES_LOAD_STATE_NONE;
    return true;
}

// parse.ProcessFile在解析配置文件过程中，如果发现一个新的文档，
// 则会回调该函数进行通知
void  SkinRes::OnNewUIDocument(IUIDocument* pDoc, const TCHAR* szPath)
{
    UISkinResDocInfo* p = new UISkinResDocInfo;
    p->m_pDoc = pDoc;
    p->m_pDoc->AddRef();
    p->m_strDocPath = szPath;

    m_listDoc.push_back(p);
}

bool SkinRes::Save()
{
    _DocList::iterator iter = m_listDoc.begin();
    for (; iter != m_listDoc.end(); iter++)
    {
        UISkinResDocInfo*  p = *iter;
        if (!p->m_pDoc->IsDirty())
            continue;

        if (!p->m_pDoc->Save())
        {
            UI_LOG_ERROR(_T("%s Save Xml Doc Failed. path=%d"), p->m_strDocPath.c_str());
        }
    }

	return true;
}

bool SkinRes::SetImageManager(ImageManager* pImageManager)
{
	if (NULL == pImageManager || NULL != m_pImageMgr)
	{
		return false;
	}

	m_pImageMgr = pImageManager;
	return true;
}

bool SkinRes::SetColorManager(ColorManager* pColorManager)
{
	if (NULL == pColorManager || NULL != m_pColorMgr)
	{
		return false;
	}

	m_pColorMgr = pColorManager;
	return true;
}

bool SkinRes::SetFontManager(FontManager* pFontManager)
{
	if (NULL == pFontManager || NULL != m_pFontMgr)
	{
		return false;
	}

	m_pFontMgr = pFontManager;
	return true;
}

bool SkinRes::SetStyleManager(StyleManager* pStyleManager)
{
	if (NULL == pStyleManager || NULL != m_pStyleMgr)
	{
		return false;
	}

	m_pStyleMgr = pStyleManager;
	return true;
}

bool SkinRes::SetLayoutManager(LayoutManager* pLayoutManager)
{
	if (NULL == pLayoutManager || NULL != m_pLayoutMgr)
	{
		return false;
	}

	m_pLayoutMgr = pLayoutManager;
	return true;
}

//
//	更新指定的对象的布局数据，在xml中将根据pRootObj的object name和id来查找原有对象的新属性和布局
//
bool SkinRes::ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild )
{
	if (NULL == m_pLayoutMgr)
	{
		UI_LOG_ERROR(_T("SkinRes::ReLoadLayout m_pLayoutMgr == NULL."));
		return false;
	}

	return m_pLayoutMgr->ReLoadLayout(pRootObj, listAllChild);
}

bool SkinRes::LoadAllWindowNodeInfo(LayoutWindowNodeInfo* pInfoArray, int* pArraySize)
{
	if (NULL == m_pLayoutMgr)
	{
		UI_LOG_ERROR(_T("%s m_pLayoutMgr == NULL."), FUNC_NAME);
		return false;
	}

	return m_pLayoutMgr->LoadAllWindowNodeInfo(pInfoArray, pArraySize);
}

int SkinRes::GetImageCount( )
{
	if (NULL == m_pImageMgr)
		return -1;

	return m_pImageMgr->GetImageCount();
}
IImageResItem* SkinRes::GetImageItemInfo(int nIndex)
{
	if (NULL == m_pImageMgr)
		return NULL;

	return m_pImageMgr->GetImageItemInfo(nIndex);
}

int SkinRes::GetColorCount( )
{
	if (NULL == m_pColorMgr)
		return -1;

	return m_pColorMgr->GetColorCount();
}
IColorResItem* SkinRes::GetColorItemInfo(int nIndex)
{
	if (NULL == m_pColorMgr)
		return NULL;

	return m_pColorMgr->GetColorItemInfo(nIndex);
}

int SkinRes::GetFontCount( )
{
	if (NULL == m_pFontMgr)
		return -1;

	return m_pFontMgr->GetFontCount();
}
IFontResItem* SkinRes::GetFontItemInfo(int nIndex)
{
	if (NULL == m_pFontMgr)
		return false;

	return m_pFontMgr->GetFontItemInfo(nIndex);
}

int SkinRes::GetStyleCount()
{
	if (NULL == m_pStyleMgr)
		return false;

	return m_pStyleMgr->GetStyleCount();
}

bool SkinRes::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	if (NULL == m_pImageMgr || NULL == m_pColorMgr)
		return false;

    if (NULL == m_pHLSInfo)
    {
        m_pHLSInfo = new SKIN_HLS_INFO;
    }
    m_pHLSInfo->h = h;
    m_pHLSInfo->l = l;
    m_pHLSInfo->s = s;
    m_pHLSInfo->nFlag = nFlag;

	if (false == m_pImageMgr->ChangeSkinHLS(h,l,s,nFlag))
		return false;
	if (false == m_pColorMgr->ChangeSkinHLS(h,l,s,nFlag))
		return false;

#if 0 // -- 架构改造	
	// 保存该 HLS 数据
	SkinConfigRes* pSkinConfigRes = m_pSkinConfigMgr->GetSkinConfigRes();
	if (NULL == pSkinConfigRes)
		return false;

	pSkinConfigRes->SetHLS((char)h,(char)l,(char)s,nFlag);
#endif
	return true;
}


UINT  SkinRes::GetXmlDocCount()
{
    return (int)m_listDoc.size();
}
const TCHAR*  SkinRes::GetXmlDoc(UINT nIndex, IUIDocument** pp)
{
    if (nIndex >= GetXmlDocCount())
        return NULL;

    UISkinResDocInfo*  pInfo = m_listDoc[nIndex];
    if (pp)
    {
        *pp = pInfo->m_pDoc;
        pInfo->m_pDoc->AddRef();
    }

    return pInfo->m_strDocPath.c_str();
}