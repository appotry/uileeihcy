#include "StdAfx.h"
#include "layoutmanager.h"

#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\Resource\stylemanager.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Inc\Interface\iuieditor.h"

LayoutManager::LayoutManager(SkinRes* p)
{
	m_pSkinRes = p;
    m_pILayoutManager = NULL;
}

LayoutManager::~LayoutManager(void)
{
	this->Clear();
    SAFE_DELETE(m_pILayoutManager);
}

ILayoutManager*  LayoutManager::GetILayoutManager()
{
    if (NULL == m_pILayoutManager)
        m_pILayoutManager = new ILayoutManager(this);

    return m_pILayoutManager;
}

void  LayoutManager::Clear()
{
    m_listUIElement.RELEASE_ALL();
}

// ֻ���ע����layout parse func�Ķ�����Menu
// ���磺����ָ��Id�Ĳ˵������ز˵�����
HRESULT  LayoutManager::LoadRootLayout(const TCHAR* szObjName, const TCHAR* szId, IObject** ppOut)
{
    IUIElement*  pObjUIElement = NULL;
    this->FindRootUIElement(szObjName, szId, &pObjUIElement);
    if (NULL == pObjUIElement)
    {
        UI_LOG_FATAL(_T("%s, δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),  FUNC_NAME, szObjName, szId);
        return E_FAIL;
    }    
    this->LoadLayout(pObjUIElement, NULL, ppOut);

    SAFE_RELEASE(pObjUIElement);
    return S_OK;
}
//
//	����ָ���Ķ���Ĳ������ݣ���xml�н�����pRootObj��object name��id�����Ҷ�������
//
HRESULT  LayoutManager::LoadRootLayout(IObject* pRootObj, const TCHAR* szId)
{
    if (NULL == pRootObj)
        return E_INVALIDARG;

    IUIElement*  pObjUIElement = NULL;  // <<-- �ҵ��ö���󱣴��ڸñ�����

    //  1. ��xml�ж�λ��pRootOb����������ݶ���xml���ƺͶ���id���ԣ�
    const TCHAR*  szObjName = pRootObj->GetObjectName();
    this->FindRootUIElement(pRootObj->GetObjectName(), pRootObj->GetId(), &pObjUIElement);
    if (NULL == pObjUIElement)
    {
        UI_LOG_FATAL(_T("%s, δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),  FUNC_NAME, szObjName, szId);
        return E_FAIL;
    }    
    
    // 2. ���ض�������
    IUIApplication* pUIApp = m_pSkinRes->GetUIApplication();
    funcUIParseLayoutElement  func = NULL;

    // ����������Ѿ������˶��󣬾Ͳ��ٵ���ParseFunc
//     pUIApp->GetLayoutTagParseFunc(szObjName, &func);
//     if (func)
//     {
//         func(pObjUIElement, m_pSkinRes->GetUIApplication(), NULL, NULL);
//     }
//     else
    {
        this->LoadObjAttrAndChild(pRootObj, pObjUIElement);
    }

    SAFE_RELEASE(pObjUIElement);
	return S_OK;
}

// ��<layout>�������ָ����tag + id������IUIElement
HRESULT  LayoutManager::FindRootUIElement(const TCHAR* szTagName, const TCHAR* szId, IUIElement** ppElem)
{
    if (NULL == szTagName || NULL == szId || NULL == ppElem)
        return E_INVALIDARG;

    *ppElem = NULL;

    UIListItem<IUIElement*>* pUIElementItem = m_listUIElement.m_pFirst;
    while (pUIElementItem)
    {
        IUIChildNodeEnum*  pEnum = pUIElementItem->m_pData->EnumChild();
        if (NULL == pEnum)
            return E_FAIL;

        IUIElement*  pChildElem = NULL;
        CComBSTR  bstrTagName;
        CComBSTR  bstrId;

        while (pChildElem = pEnum->NextElement())
        {
            pChildElem->GetTagName(&bstrTagName);
            if (bstrTagName != szTagName)
            {
                SAFE_RELEASE(pChildElem);
                continue;
            }

            pChildElem->GetAttrib(XML_ID, &bstrId);
            if (bstrId != szId)
            {
                SAFE_RELEASE(pChildElem);
                continue;
            }
            else
            {
                *ppElem = pChildElem;
                break;
            }
        }
        SAFE_RELEASE(pEnum);

        if (*ppElem)
            return S_OK;;

        pUIElementItem = pUIElementItem->m_pNext;
    }
    return S_OK;
}



// �õ�һ��Ԫ��ָ�룬������Ԫ�ؼ����ӽ�㣬�����ض���ָ��
HRESULT  LayoutManager::LoadLayout(IUIElement* pUIElement, IObject* pParent, IObject** ppObj)
{
    IUIApplication*  pUIApp = m_pSkinRes->GetUIApplication();

    Object*  pObj = NULL;
    CComBSTR  bstrTagName;
    pUIElement->GetTagName(&bstrTagName);

    // ����ǩ��ת��Ϊ��
    IObject*  pIObject = NULL;
    pUIApp->CreateInstanceByName(bstrTagName, &pIObject);
    if (NULL == pIObject)
    {
        // ����Ѱ�Ҹ�Tag�Ƿ�ע����
        funcUIParseLayoutElement  func = NULL;
        pUIApp->GetLayoutTagParseFunc(bstrTagName, &func);
        if (func)
        {
            if (FAILED(func(pUIElement, pUIApp, pParent, &pIObject)))
            {
                UI_LOG_ERROR(_T("%s Parse Object:  %s Failed."), FUNC_NAME,  (BSTR)bstrTagName);    
            }
        }
        else
        {
            UI_LOG_ERROR(_T("%s CreateObject Failed. name=%s"), FUNC_NAME, (BSTR)bstrTagName);
        }
    }

    if (NULL == pIObject)
        return E_FAIL;

    *ppObj = pIObject;
    pObj = pIObject->GetImpl();

    if (pParent)
        pParent->AddChild(pIObject);

    LoadObjAttrAndChild(pIObject, pUIElement);

    // Ĭ�Ͻ��ÿؼ������һ��notify objectΪ���ڶ��󣬲���msgmapid����Ϊ0
    WindowBase* pWindowBase = pObj->GetWindowObject();
    if (pObj != pWindowBase && pWindowBase)
        pObj->SetNotify(pWindowBase->GetIMessage(), 0);

    return S_OK;
}

// �Ѿ���λ���Լ���λ�ã���Ҫ�����Լ������Ժ��Ӷ���ķ���
HRESULT  LayoutManager::LoadObjAttrAndChild(IObject* pObj, IUIElement* pUIElement)
{
    // �Լ�������
    LoadObjectAttribute(pObj->GetImpl(), pUIElement, false);

    // �����Ӷ���
    LoadChildObjects(pObj, pUIElement);

    // ����һ��֪ͨ��Ϣ���ڴ��������ڲ�����
    UISendMessage(pObj, UI_WM_OBJECTLOADED);

    // ֪ͨ�༭��
    IUIEditor* pUIEditor = m_pSkinRes->GetUIApplication()->GetUIEditorPtr();
    if (pUIEditor)
        pUIEditor->OnObjectAttributeLoad(pObj, pUIElement);
    
    return S_OK;
}

bool  LayoutManager::LoadObjectAttribute(Object* pObj, IUIElement* pUIElement, bool bReload)
{
    IUIApplication*  pUIApp = m_pSkinRes->GetUIApplication();
    StyleManager*  pStyleMgr = m_pSkinRes->GetStyleManager();
    if (NULL == pStyleMgr)
        return false;

    //	������������
    IMapAttribute*  pMapAttrib = NULL;
    pUIElement->GetAttribList(&pMapAttrib);

    const TCHAR*  szTagName = pObj->GetObjectName();
    pStyleMgr->ParseStyle(szTagName, pMapAttrib);

	// �°汾
	SERIALIZEDATA sd = {0};
	sd.nFlag = SERIALIZEFLAG_LOAD;
	if (bReload)
    {
        UISendMessage(pObj, UI_WM_RESETATTRIBUTE);
		sd.nFlag |= SERIALIZEFLAG_RELOAD;
    }

	sd.pMapAttrib = pMapAttrib;
	UISendMessage(pObj, UI_WM_SERIALIZE, (WPARAM)&sd, 0);

	// �ϰ汾
//     if (bReload)
//     {
//         UISendMessage(pObj, UI_WM_RESETATTRIBUTE);
//     }
//     UISendMessage(pObj, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)bReload);

    SAFE_RELEASE(pMapAttrib);

    return true;
}

// �ݹ飬���������Ӷ����Ӷ��������
void  LayoutManager::LoadChildObjects(IObject* pIObjParent, IUIElement* pUIElement)
{
    if (NULL == pIObjParent || NULL == pUIElement)
        return ;

    IUIElement*  pChildElem = NULL;
    IUIChildNodeEnum*  pEnum = pUIElement->EnumChild();

    while (pChildElem = pEnum->NextElement())
    {
        IObject* pChildObj = NULL;
        LoadLayout(pChildElem, pIObjParent, &pChildObj);        
        SAFE_RELEASE(pChildElem);
    }
    SAFE_RELEASE(pEnum);
}

//	����ָ���Ķ���Ĳ������ݣ���xml�н�����pRootObj��object name��id������ԭ�ж���������ԺͲ���
bool  LayoutManager::ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild )
{
    IUIElement*  pObjUIElement = NULL;  // <<-- �ҵ��ö���󱣴��ڸñ�����

    const TCHAR*  szObjName = pRootObj->GetObjectName();
    this->FindRootUIElement(pRootObj->GetObjectName(), pRootObj->GetId(), &pObjUIElement);

    if (NULL == pObjUIElement)
    {
        UI_LOG_FATAL(_T("%s, δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),  FUNC_NAME, szObjName, pRootObj->GetId());
        return false;
    }    

    LoadObjectAttribute(pRootObj, pObjUIElement, true);
    ReloadChildObjects(pRootObj, pObjUIElement, listAllChild);

    SAFE_RELEASE(pObjUIElement);
    return true;
}

void  LayoutManager::ReloadChildObjects(Object* pObjParent, IUIElement* pObjElement, list<Object*>& listAllChild)
{
    IUIApplication*  pUIApp = m_pSkinRes->GetUIApplication();
    IUIChildNodeEnum*  pEnum = pObjElement->EnumChild();
    IUIElement*  pChildElem = NULL;

    CComBSTR  bstrChildElemTagName;
    CComBSTR  bstrId;

    // ���������Ӷ���
    while (pChildElem = pEnum->NextElement())
    {
        Object*  pObj = NULL;
        pChildElem->GetTagName(&bstrChildElemTagName);
        if (0 == bstrChildElemTagName.Length())
        {
            UI_LOG_WARN( _T("%s, xml invalid tag name."), FUNC_NAME);
            SAFE_RELEASE(pChildElem);
            continue;
        }

        pChildElem->GetAttrib(XML_ID, &bstrId);
        if (0 != bstrId.Length())
        {
            // ���� tagName + id ��listAllChild�в��Ҹö���
            list<Object*>::iterator  iter = listAllChild.begin();
            list<Object*>::iterator  iterEnd = listAllChild.end();

            for (; iter != iterEnd; iter++)
            {
                Object* pTempObj = *iter;
                if (0 == _tcscmp((BSTR)bstrChildElemTagName, pTempObj->GetObjectName()) &&
                    0 == _tcscmp((BSTR)bstrId, pTempObj->GetId()))
                {
                    pObj = pTempObj;
                    break;
                }
            }
        }

        if (NULL == pObj)
        {
            // û���ҵ�
            // ֱ������������
            // ����ǩ��ת��Ϊ��

            IObject* pIObject = NULL;
            pUIApp->CreateInstanceByName(bstrChildElemTagName, &pIObject);
            if (NULL == pIObject)
            {
                UI_LOG_ERROR(_T("%s CreateObject Failed. name=%s"), FUNC_NAME, (BSTR)bstrChildElemTagName);
                SAFE_RELEASE(pChildElem);
                continue;
            }
            pObj = pIObject->GetImpl();

            // ����
            pObjParent->AddChild(pObj);

            // ��������
            this->LoadObjectAttribute(pObj, pChildElem, false);

            // Ĭ�Ͻ��ÿؼ������һ��notify objectΪ���ڶ��󣬲���msgmapid����Ϊ0
            WindowBase* pWindowBase = pObj->GetWindowObject();
            if (pWindowBase)
            {
                pObj->SetNotify(pWindowBase->GetIMessage(), 0);
            }

            // �ݹ飬��������Ӷ�����Ӷ���
            this->ReloadChildObjects(pObj, pChildElem, listAllChild);
        }
        else
        { 
            // ����
            pObjParent->AddChild(pObj);

            // ��������
            this->LoadObjectAttribute(pObj, pChildElem, true);

            // �ݹ飬��������Ӷ�����Ӷ���
            this->ReloadChildObjects(pObj, pChildElem, listAllChild);

            // ���б����Ƴ�
            list<Object*>::iterator iter = listAllChild.begin();
            list<Object*>::iterator iterEnd = listAllChild.end();
            for (; iter != iterEnd; iter++)
            {
                if (pObj == (*iter))
                {
                    listAllChild.erase(iter);
                    break;
                }
            }

        }

        SAFE_RELEASE(pChildElem);
    }
    SAFE_RELEASE(pEnum);
}

bool  LayoutManager::LoadAllWindowNodeInfo(LayoutWindowNodeInfo* pInfoArray, int* pArraySize)
{
    int nIndex = 0;
    UIListItem<IUIElement*>* pUIElementItem = m_listUIElement.m_pFirst;
    while (pUIElementItem)
    {
        IUIChildNodeEnum*  pEnum = pUIElementItem->m_pData->EnumChild();
        if (NULL == pEnum)
        {
            pUIElementItem = pUIElementItem->m_pNext;
            continue;
        }

        IUIElement*  pChildElem = NULL;
        CComBSTR  bstrTagName;
        CComBSTR  bstrId;

        while (pChildElem = pEnum->NextElement())
        {
            if (NULL == pInfoArray)  // ����ȡ��С
            {
                if (pArraySize)
                    (*pArraySize)++;

                SAFE_RELEASE(pChildElem);
                continue;
            }

            pChildElem->GetTagName(&bstrTagName);
            pChildElem->GetAttrib(XML_ID, &bstrId);

            _tcsncpy(pInfoArray[nIndex].szNodeID, (BSTR)bstrId, MAX_STRING_LEN-1);
            _tcsncpy(pInfoArray[nIndex].szNodeName, (BSTR)bstrTagName, MAX_STRING_LEN-1);

            nIndex++;
            SAFE_RELEASE(pChildElem);
        }
        SAFE_RELEASE(pEnum);
        pUIElementItem = pUIElementItem->m_pNext;
    }
    return false;
}

ILayoutRes*  LayoutManager::GetLayoutRes()
{
	return m_resLayout.GetILayoutRes();
}

//////////////////////////////////////////////////////////////////////////

HRESULT  LayoutManager::UIParseSkinElement(IUIElement* pElem, ISkinRes* pSkinRes)
{
    ILayoutManager*  pLayoutMgr = pSkinRes->GetLayoutManager();
    if (NULL == pLayoutMgr)
        return E_FAIL;

    return pLayoutMgr->GetImpl()->ParseNewElement(pElem);
}

HRESULT  LayoutManager::ParseNewElement(IUIElement* pElem)
{
    pElem->AddRef();
    m_listUIElement.Add(pElem);

    // �ɴ����Լ�����LoadLayout�ٽ���
    return S_OK;
}