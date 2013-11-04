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

// 只针对注册了layout parse func的对象，如Menu
// 例如：加载指定Id的菜单，返回菜单对象
HRESULT  LayoutManager::LoadRootLayout(const TCHAR* szObjName, const TCHAR* szId, IObject** ppOut)
{
    IUIElement*  pObjUIElement = NULL;
    this->FindRootUIElement(szObjName, szId, &pObjUIElement);
    if (NULL == pObjUIElement)
    {
        UI_LOG_FATAL(_T("%s, 未找到要加载的对象：name=%s, id=%s"),  FUNC_NAME, szObjName, szId);
        return E_FAIL;
    }    
    this->LoadLayout(pObjUIElement, NULL, ppOut);

    SAFE_RELEASE(pObjUIElement);
    return S_OK;
}
//
//	加载指定的对象的布局数据，在xml中将根据pRootObj的object name和id来查找对象及数据
//
HRESULT  LayoutManager::LoadRootLayout(IObject* pRootObj, const TCHAR* szId)
{
    if (NULL == pRootObj)
        return E_INVALIDARG;

    IUIElement*  pObjUIElement = NULL;  // <<-- 找到该对象后保存在该变量中

    //  1. 在xml中定位到pRootOb这个对象（依据对象xml名称和对象id属性）
    const TCHAR*  szObjName = pRootObj->GetObjectName();
    this->FindRootUIElement(pRootObj->GetObjectName(), pRootObj->GetId(), &pObjUIElement);
    if (NULL == pObjUIElement)
    {
        UI_LOG_FATAL(_T("%s, 未找到要加载的对象：name=%s, id=%s"),  FUNC_NAME, szObjName, szId);
        return E_FAIL;
    }    
    
    // 2. 加载对象属性
    IUIApplication* pUIApp = m_pSkinRes->GetUIApplication();
    funcUIParseLayoutElement  func = NULL;

    // 这种情况下已经创建了对象，就不再调用ParseFunc
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

// 在<layout>下面查找指定的tag + id，返回IUIElement
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



// 得到一个元素指针，创建该元素及其子结点，并返回对象指针
HRESULT  LayoutManager::LoadLayout(IUIElement* pUIElement, IObject* pParent, IObject** ppObj)
{
    IUIApplication*  pUIApp = m_pSkinRes->GetUIApplication();

    Object*  pObj = NULL;
    CComBSTR  bstrTagName;
    pUIElement->GetTagName(&bstrTagName);

    // 将标签名转化为类
    IObject*  pIObject = NULL;
    pUIApp->CreateInstanceByName(bstrTagName, &pIObject);
    if (NULL == pIObject)
    {
        // 尝试寻找该Tag是否被注册了
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

    // 默认将该控件的添加一个notify object为窗口对象，并将msgmapid设置为0
    WindowBase* pWindowBase = pObj->GetWindowObject();
    if (pObj != pWindowBase && pWindowBase)
        pObj->SetNotify(pWindowBase->GetIMessage(), 0);

    return S_OK;
}

// 已经定位到自己的位置，需要加载自己的属性和子对象的方法
HRESULT  LayoutManager::LoadObjAttrAndChild(IObject* pObj, IUIElement* pUIElement)
{
    // 自己的属性
    LoadObjectAttribute(pObj->GetImpl(), pUIElement, false);

    // 遍历子对象
    LoadChildObjects(pObj, pUIElement);

    // 发送一个通知消息用于创建其它内部对象
    UISendMessage(pObj, UI_WM_OBJECTLOADED);

    // 通知编辑器
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

    //	加载所有属性
    IMapAttribute*  pMapAttrib = NULL;
    pUIElement->GetAttribList(&pMapAttrib);

    const TCHAR*  szTagName = pObj->GetObjectName();
    pStyleMgr->ParseStyle(szTagName, pMapAttrib);

	// 新版本
	SERIALIZEDATA sd = {0};
	sd.nFlag = SERIALIZEFLAG_LOAD;
	if (bReload)
    {
        UISendMessage(pObj, UI_WM_RESETATTRIBUTE);
		sd.nFlag |= SERIALIZEFLAG_RELOAD;
    }

	sd.pMapAttrib = pMapAttrib;
	UISendMessage(pObj, UI_WM_SERIALIZE, (WPARAM)&sd, 0);

	// 老版本
//     if (bReload)
//     {
//         UISendMessage(pObj, UI_WM_RESETATTRIBUTE);
//     }
//     UISendMessage(pObj, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)bReload);

    SAFE_RELEASE(pMapAttrib);

    return true;
}

// 递归，加载所有子对象及子对象的属性
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

//	更新指定的对象的布局数据，在xml中将根据pRootObj的object name和id来查找原有对象的新属性和布局
bool  LayoutManager::ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild )
{
    IUIElement*  pObjUIElement = NULL;  // <<-- 找到该对象后保存在该变量中

    const TCHAR*  szObjName = pRootObj->GetObjectName();
    this->FindRootUIElement(pRootObj->GetObjectName(), pRootObj->GetId(), &pObjUIElement);

    if (NULL == pObjUIElement)
    {
        UI_LOG_FATAL(_T("%s, 未找到要加载的对象：name=%s, id=%s"),  FUNC_NAME, szObjName, pRootObj->GetId());
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

    // 遍历所有子对象
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
            // 根据 tagName + id 从listAllChild中查找该对象
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
            // 没有找到
            // 直接添加这个对象
            // 将标签名转化为类

            IObject* pIObject = NULL;
            pUIApp->CreateInstanceByName(bstrChildElemTagName, &pIObject);
            if (NULL == pIObject)
            {
                UI_LOG_ERROR(_T("%s CreateObject Failed. name=%s"), FUNC_NAME, (BSTR)bstrChildElemTagName);
                SAFE_RELEASE(pChildElem);
                continue;
            }
            pObj = pIObject->GetImpl();

            // 关联
            pObjParent->AddChild(pObj);

            // 加载属性
            this->LoadObjectAttribute(pObj, pChildElem, false);

            // 默认将该控件的添加一个notify object为窗口对象，并将msgmapid设置为0
            WindowBase* pWindowBase = pObj->GetWindowObject();
            if (pWindowBase)
            {
                pObj->SetNotify(pWindowBase->GetIMessage(), 0);
            }

            // 递归，加载这个子对象的子对象
            this->ReloadChildObjects(pObj, pChildElem, listAllChild);
        }
        else
        { 
            // 关联
            pObjParent->AddChild(pObj);

            // 加载属性
            this->LoadObjectAttribute(pObj, pChildElem, true);

            // 递归，加载这个子对象的子对象
            this->ReloadChildObjects(pObj, pChildElem, listAllChild);

            // 从列表中移除
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
            if (NULL == pInfoArray)  // 仅获取大小
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

    // 由窗口自己调用LoadLayout再解析
    return S_OK;
}