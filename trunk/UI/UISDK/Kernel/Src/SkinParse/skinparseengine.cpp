#include "stdafx.h"
#include "skinparseengine.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "msxmlwrap/msxmlwrap.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"

namespace UI
{

SkinParseEngine::SkinParseEngine(IUIApplication* pUIApp, SkinRes* pSkinRes)
{   
    m_pUIApplication = pUIApp;
    m_pSkinRes = pSkinRes;
}

SkinParseEngine::~SkinParseEngine()
{
}

bool  SkinParseEngine::ProcessFile(const TCHAR* szXmlFile)
{
#if 0
    1. ���� <skin> root element�����û���ҵ���ֱ�ӷ��أ���ʾ�ⲻ��һ���Ϸ��ļ�
        2. ���� <skin> ��child element

        2.1 ��ȡtagName������image
        2.2 ��ȡimage��ǩ��Ӧ�Ľ����� IImageParse
        2.3 ����IImageParse��NewElement
        2.4 IImageParse�Լ�ȥ��������ӽ��
#endif
    
    IUIDocument*  pUIDocument = UIMarkup::New();
    if (false == m_pSkinRes->GetDataSource()->Load_UIDocument(pUIDocument, szXmlFile))
    {
        SAFE_RELEASE(pUIDocument);
        return false;
    }

    IUIElement* pRootElem = NULL;
    IUIChildNodeEnum* pEnum = NULL;

    do 
    {
        pRootElem = pUIDocument->FindElem(XML_SKIN);
        if (NULL == pRootElem)
        {
            UI_LOG_ERROR(_T("%s Cannot find root element: %s"), FUNC_NAME, XML_SKIN);
            break;
        }

        m_pSkinRes->OnNewUIDocument(pUIDocument, szXmlFile);

        pEnum = pRootElem->EnumChild();
        if (NULL == pEnum)
            break;

        IUIElement* pChildElement = NULL;
        while (pChildElement = pEnum->NextElement())
        {
            this->NewChild(pChildElement);
            SAFE_RELEASE(pChildElement);
        }
    }
    while (0);

    SAFE_RELEASE(pRootElem);
    SAFE_RELEASE(pEnum);

    SAFE_RELEASE(pUIDocument);
    return true;
}

void  SkinParseEngine::NewChild(IUIElement* pElement)
{
    CComBSTR  bstrTagName;
    pElement->GetTagName(&bstrTagName);

    funcUIParseSkinElement func;
    if (FAILED(m_pUIApplication->GetSkinTagParseFunc((BSTR)bstrTagName, &func)))
        return;

    // ������Ԫ��
//     ISkinTagParseBase* pParse = iter->second;
//     pParse->NewElement(pElement);

    func(pElement, m_pSkinRes->GetISkinRes());
}


HRESULT  SkinParseEngine::UIParseIncludeElement(IUIElement* pElement, ISkinRes* pSkinRes)
{
    if (NULL == pElement || NULL == pSkinRes)
        return E_FAIL;

//  TODO: ������DataSource��Ӱ�죬���ٺ�����ж�·����
//     String  strLastPath;
//     if (false == GetIncludeFilePath(pElement, pSkinRes, strLastPath))
//         return E_FAIL;

    CComBSTR  bstrFilePath;
    if (FAILED(pElement->GetData(&bstrFilePath)))
        return E_FAIL;

    SkinParseEngine parse(pSkinRes->GetUIApplication(), pSkinRes->GetImpl());
    if (!parse.ProcessFile((BSTR)bstrFilePath))
        return E_FAIL;

    return S_OK;
}
}