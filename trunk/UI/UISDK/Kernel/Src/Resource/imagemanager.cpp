#include "StdAfx.h"
#include "imagemanager.h"

#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Inc\Interface\iuieditor.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             ͼƬ                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


ImageManager::ImageManager(SkinRes* pSkinRes, IUIApplication* pUIApp) : m_resImage(pSkinRes), m_resGif(pSkinRes)
{
    m_pUIElement = NULL;
    m_pIImageManager = NULL;
	m_pSkinRes = pSkinRes;
	m_resGif.SetUIApplication(pUIApp);
}

ImageManager::~ImageManager(void)
{
	this->Clear();
    SAFE_RELEASE(m_pUIElement);
    SAFE_DELETE(m_pIImageManager);
}

IImageManager*  ImageManager::GetIImageManager()
{
    if (NULL == m_pIImageManager)
        m_pIImageManager = new IImageManager(this);

    return m_pIImageManager;
}


//
//	�½�һ��ͼƬ�����ļ�
//
bool ImageManager::Create( const String& strXmlPath )
{
#if 0
	if (m_pImageParse)
	{
		UI_LOG_WARN(_T("ImageManager::Create image parse already exist."));
		return false;
	}

	if (NULL == m_pSkinRes)
		return false;

	m_pImageParse = IParseClassFactory::CreateImageParseInstance(m_pSkinRes->GetUIApplication(), strXmlPath );
	if (false == m_pImageParse->Create())
	{
		UI_LOG_ERROR(_T("ImageManager::Create create image parse failed. path=%s"), strXmlPath.c_str());
		return false;
	}
#endif
	return true;
}

/*
**	[public]  bool Load( )
**
**	Parameters
**		NA
**
**	Return
**		�ɹ�����true��ʧ�ܷ���false
**
**	Remark
**		�����е�ͼƬ��Դ������m_map���С�
**
**	See Also
**		UIGlobal:: LoadImage
**
*/
bool ImageManager::Load( const String& strXmlPath )
{
#if 0
	this->Clear();

	if( NULL != m_pImageParse )
	{
		UI_LOG_WARN(_T("ImageManager::Load image parse already exist."));
		return false;
	}

	if (NULL == m_pSkinRes)
		return false;

	m_pImageParse = IParseClassFactory::CreateImageParseInstance(m_pSkinRes->GetUIApplication(), strXmlPath);

	if (false == m_pImageParse->Load(&m_resImage, &m_resCursor, &m_resGif))
		return false;

#endif
	return true;
}

//
//	Ϊ��ʵ���ڱ༭�����ܹ�ͬʱ��view/code����������б༭�����޸���code�����,
//  code�Ὣ����д�뵽һ��tmp�ļ��У�Ȼ��ImageManager��Ҫ���¶�ȡ������
//
bool ImageManager::Reload()
{
#if 0
	if( NULL == m_pImageParse )
	{
		UI_LOG_ERROR(_T("ImageManager::Reload m_pImageParse == NULL."));
		return false;
	}

	this->Clear();
	return  m_pImageParse->Reload(&m_resImage, &m_resCursor, &m_resGif);
#endif
    return false;
}

bool ImageManager::ChangeSkinHLS(short h, short l, short s, int nFlag)
{
	return m_resImage.ChangeSkinHLS(h,l,s,nFlag);
}

void ImageManager::Clear()
{
	m_resImage.Clear();
}

IImageResItem* ImageManager::InsertImageItem(const TCHAR* szId, const TCHAR* szPath)
{
	if (NULL == szId || NULL == szPath)
		return NULL;

	ImageResItem* p = m_resImage.InsertImage(szId, szPath);
    if (!p)
	{
		UI_LOG_ERROR(_T("%s m_resImage.InsertImage Id=%s, Path=%s Failed. "), FUNC_NAME, szId, szPath);
		return NULL;
	}

    IUIElement* pElem = NULL;
    m_pUIElement->AddChild(_T("image"), &pElem);
    if (pElem)
    {
        pElem->AddAttrib(XML_ID, szId);
        pElem->AddAttrib(XML_PATH, szPath);
        
        IUIEditor* pEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
        if (pEditor)
        {
            pEditor->OnImageItemLoad(p->GetIImageResItem(), pElem);
        }

        SAFE_RELEASE(pElem);
    }
	return p->GetIImageResItem();
}

HRESULT ImageManager::ModifyImageItem(const TCHAR* szId, const TCHAR* szPath)
{
	if (NULL == szId || NULL == szPath)
		return E_INVALIDARG;

	if (false == m_resImage.ModifyImage(szId, szPath))
	{
		UI_LOG_ERROR(_T("%s m_resImage.ModifyImage strID=%s, strPath=%s Failed."), FUNC_NAME, szId, szPath);
		return E_FAIL;
	}

	return S_OK;
}
HRESULT ImageManager::ModifyImageItemInRunTime(const TCHAR* szId, const TCHAR* szPath)
{
	if (NULL == szId || NULL == szPath)
		return E_INVALIDARG;

	if (false == m_resImage.ModifyImage(szId, szPath))
	{
		UI_LOG_ERROR(_T("%s m_resImage.ModifyImage strID=%s,strPath=%s Failed. "), FUNC_NAME, szId, szPath);
		return E_FAIL;
	}

	// TODO: ���浽�û������ļ���
	UI_LOG_DEBUG(_T("%s TODO: ���浽�û������ļ���"), FUNC_NAME);
	
	return S_OK;
}

HRESULT ImageManager::ModifyImageItemAlpha(const TCHAR* szId, int nAlphaPercent)
{
	if (NULL == szId || nAlphaPercent < 0 || nAlphaPercent > 100)
		return E_INVALIDARG;

	if (false == m_resImage.ModifyImageItemAlpha(szId, nAlphaPercent))
	{
		UI_LOG_ERROR(_T("%s m_resImage.ModifyImageItemAlpha strID=%s,nAlphaPercent=%d Failed. "), FUNC_NAME, szId, nAlphaPercent);
		return E_FAIL;
	}

	// TODO: ���浽�û������ļ���
	UI_LOG_DEBUG(_T("%s TODO: ���浽�û������ļ���"), FUNC_NAME);

	return S_OK;
}

HRESULT ImageManager::RemoveImageItem(const TCHAR* szId)
{
	if (NULL == szId)
		return E_INVALIDARG;

	if (false == m_resImage.RemoveImage(szId))
	{
		UI_LOG_ERROR(_T("%s m_resImage.RemoveImage strID=%s Failed. "), FUNC_NAME, szId);
		return E_FAIL;
	}

	return S_OK;
}

int ImageManager::GetImageCount( )
{
	return m_resImage.GetImageCount();
}
IImageResItem* ImageManager::GetImageItemInfo(int nIndex)
{
	ImageResItem* pItem = m_resImage.GetImageItem2(nIndex);
    if (NULL == pItem)
        return NULL;

	return pItem->GetIImageResItem();
}

IImageRes*  ImageManager::GetImageRes()
{
	return m_resImage.GetIImageRes();
}
ICursorRes*  ImageManager::GetCursorRes()
{
	return m_resCursor.GetICursorRes();
}
IGifRes*  ImageManager::GetGifRes()
{
	return m_resGif.GetIGifRes();
}


//////////////////////////////////////////////////////////////////////////

HRESULT  ImageManager::UIParseSkinElement(IUIElement* pElem, ISkinRes* pSkinRes)
{
    IImageManager*  pImageMgr = pSkinRes->GetImageManager();
    if (NULL == pImageMgr)
        return E_FAIL;

    return pImageMgr->GetImpl()->ParseNewElement(pElem);
}
HRESULT  ImageManager::ParseNewElement(IUIElement* pElem)
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

    return S_OK;
}

void  ImageManager::OnNewChild(IUIElement* pElem)
{
    CComBSTR bstrTagName;
    pElem->GetTagName(&bstrTagName);

    //	������������
    IMapAttribute* pMapAttrib = NULL;
    pElem->GetAttribList(&pMapAttrib);

    // ��ȡ·��
    CComBSTR  bstrPath;
    pElem->GetData(&bstrPath);

    if (0 == bstrPath.Length())
        bstrPath = pMapAttrib->GetAttr(XML_PATH, true);  // ���û�������������У��ͼ��һ���Ƿ������ó�������
    
    if (0 == bstrPath.Length())
    {
        SAFE_RELEASE(pMapAttrib);
        return;
    }

    // REMOVED 2013.5.29 ����datasourceȥ�ж�
//     String  strFullPath;
//     if (Util::IsFullPath((BSTR)bstrPath))
//     {
//         strFullPath = bstrPath;
//     }
//     else
//     {
//         TCHAR szFullPath[MAX_PATH] = _T("");
//         Util::CalcFullPathByRelative(m_pSkinRes->GetPath(), (BSTR)bstrPath, szFullPath);
// 
//         strFullPath = szFullPath;
//     }

    if (bstrTagName == XML_IMAGE_ITEM_CURSOR)
    {
        CursorResItem* p = m_resCursor.LoadItem(pMapAttrib, (BSTR)bstrPath);
        if (p)
        {
            IUIEditor* pEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
            if (pEditor)
            {
                pEditor->OnCursorItemLoad(p->GetICursorResItem(), pElem);
            }
        }
        else
        {
            UI_LOG_WARN(_T("%s insert cursor failed. path=%s"), FUNC_NAME, (BSTR)bstrPath);
        }
    }
    else if (bstrTagName == XML_IMAGE_ITEM_GIF)
    {
        GifResItem* p = m_resGif.LoadItem(pMapAttrib, (BSTR)bstrPath);
        if (p)
        {
            IUIEditor* pEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
            if (pEditor)
            {
                pEditor->OnGifItemLoad(p->GetIGifResItem(), pElem);
            }
        }
        else
        {
            UI_LOG_WARN(_T("%s insert gif failed. path=%s"), FUNC_NAME, (BSTR)bstrPath);
        }
    }
    else
    {
        ImageResItem* p = m_resImage.LoadItem((BSTR)bstrTagName, pMapAttrib, (BSTR)bstrPath);
        if (p)
        {
            IUIEditor* pEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
            if (pEditor)
            {
                pEditor->OnImageItemLoad(p->GetIImageResItem(), pElem);
            }
        }
        else
        {
            UI_LOG_WARN(_T("%s insert image failed. path=%s"), FUNC_NAME, (BSTR)bstrPath);
        }
    }

    SAFE_RELEASE(pMapAttrib);
}
