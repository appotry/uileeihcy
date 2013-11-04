#include "stdafx.h"
#include "uiresource.h"
#include "UISDK\Kernel\Src\Resource\skinres.h"

OutCtrlDllItem::OutCtrlDllItem() 
{
    hModule = NULL;
}

OutCtrlDllItem::~OutCtrlDllItem()
{
	if (hModule)
	{
		FreeLibrary(hModule);
		hModule = NULL;
	}
}


bool SkinBuilderRes::Register(OutCtrlDllItem* pItem)
{
	if (NULL == pItem)
		return false;

	if (pItem->hModule)
		return false;

	pItem->hModule = ::LoadLibrary(pItem->strDllPath.c_str());
	if (NULL == pItem->hModule)
	{
		UI_LOG_ERROR(_T("%s LoadLibrary Failed. DllPath=%s"), FUNC_NAME, pItem->strDllPath.c_str());
		return false;
	}
	UI_LOG_DEBUG(_T("%s LoadLibrary To Register OutCtrl CreateInstanceFun: %s"), FUNC_NAME, pItem->strDllPath.c_str());

	funRegisterUIObjectPtr pFunc = (funRegisterUIObjectPtr)::GetProcAddress(pItem->hModule, FUN_RegisterUIObject_NAME);
	if (NULL == pFunc)
	{
		UI_LOG_ERROR(_T("%s GetProcAddress Failed. DllPath=%s"), FUNC_NAME, pItem->strDllPath.c_str());
		return false;
	}

	pFunc(m_pUIApp);
	return true;
}
void SkinBuilderRes::AddOuterCtrlsDllPath(const String& strDll)
{
	if (FindOuterCtrlsDll(strDll))
		return;

	OutCtrlDllItem* pItem = new OutCtrlDllItem;
	pItem->strDllPath = strDll;
	m_vecOuterCtrlsDll.push_back(pItem);

	this->Register(pItem);
}
OutCtrlDllItem*  SkinBuilderRes::FindOuterCtrlsDll(const String& strDll)
{
	vector<OutCtrlDllItem*>::iterator  iter = m_vecOuterCtrlsDll.begin();
	vector<OutCtrlDllItem*>::iterator  iterEnd = m_vecOuterCtrlsDll.end();
	for (; iter != iterEnd; iter++)
	{
		OutCtrlDllItem* p = *iter;
		if (p->strDllPath == strDll)
			return p;
	}
	return NULL;
}
OutCtrlDllItem*  SkinBuilderRes::FindOuterCtrlsDll(HMODULE hModule)
{
	vector<OutCtrlDllItem*>::iterator  iter = m_vecOuterCtrlsDll.begin();
	vector<OutCtrlDllItem*>::iterator  iterEnd = m_vecOuterCtrlsDll.end();
	for (; iter != iterEnd; iter++)
	{
		OutCtrlDllItem* p = *iter;
		if (p->hModule == hModule)
			return p;
	}
	return NULL;
}
void  SkinBuilderRes::Clear()
{
	vector<OutCtrlDllItem*>::iterator  iter = m_vecOuterCtrlsDll.begin();
	vector<OutCtrlDllItem*>::iterator  iterEnd = m_vecOuterCtrlsDll.end();
	for (; iter != iterEnd; iter++)
	{
		OutCtrlDllItem* p = *iter;
		delete p;
	}
	m_vecOuterCtrlsDll.clear();
}

SkinConfigRes::SkinConfigRes()
{
	memset(&m_sHlsInfo, 0, sizeof(SKIN_HLS_INFO)); 
}
void SkinConfigRes::SetHLS(char h, char l, char s, int nFlag)
{
	if (nFlag & CHANGE_SKIN_HLS_FLAG_H)
	{
		m_sHlsInfo.h = h;
	}

	if (nFlag & CHANGE_SKIN_HLS_FLAG_L)
	{
		m_sHlsInfo.l = l;
	}

	if (nFlag & CHANGE_SKIN_HLS_FLAG_S)
	{
		m_sHlsInfo.s = s;
	}

	m_sHlsInfo.nFlag |= nFlag;
}

//////////////////////////////////////////////////////////////////////////
// Layout
#if 0
LayoutResItem::LayoutResItem() 
{ 
    m_pObject = NULL; 
    m_pMapSelfAttr = NULL;
    m_pMapStyleAttr = NULL;
    m_pMapLastAttr = NULL;
    m_pILayoutResItem = NULL;
}
LayoutResItem::~LayoutResItem()
{
    SAFE_RELEASE(m_pMapSelfAttr);
    SAFE_RELEASE(m_pMapStyleAttr);
    SAFE_RELEASE(m_pMapLastAttr);
    SAFE_DELETE(m_pILayoutResItem);
}

ILayoutResItem*  LayoutResItem::GetILayoutResItem()
{
    if (NULL == m_pILayoutResItem)
        m_pILayoutResItem = new ILayoutResItem(this);

    return m_pILayoutResItem;
}

HRESULT LayoutResItem::GetSelfAttr(IMapAttribute** ppMapAttribute)
{
	if (NULL == ppMapAttribute)
		return E_INVALIDARG;

	*ppMapAttribute = m_pMapSelfAttr;
	return S_OK;
}
HRESULT LayoutResItem::GetStyleAttr(IMapAttribute** ppMapAttribute)
{
	if (NULL == ppMapAttribute)
		return E_INVALIDARG;

	*ppMapAttribute = m_pMapStyleAttr;
	return S_OK;
}
HRESULT LayoutResItem::GetLastAttr(IMapAttribute** ppMapAttribute)
{
	if (NULL == ppMapAttribute)
		return E_INVALIDARG;

	*ppMapAttribute = m_pMapLastAttr;
	return S_OK;
}

void  LayoutResItem::SetSelfAttr(IMapAttribute*  pattr)
{
    SAFE_RELEASE(m_pMapSelfAttr);
    if (pattr)
    {
        pattr->CreateCopy(&m_pMapSelfAttr);
    }
}
void  LayoutResItem::SetStyleAttr(IMapAttribute* pattr)
{
    SAFE_RELEASE(m_pMapStyleAttr);
    if (pattr)
    {
        pattr->CreateCopy(&m_pMapStyleAttr);
    }
}
void  LayoutResItem::SetLastAttr(IMapAttribute* pattr)
{
    SAFE_RELEASE(m_pMapLastAttr);
    if (pattr)
    {
        pattr->CreateCopy(&m_pMapLastAttr);
    }
}
#endif

LayoutRes::LayoutRes()
{
#if 0
    m_pILayoutRes = NULL;
#endif
}
LayoutRes::~LayoutRes()
{
#if 0
    SAFE_DELETE(m_pILayoutRes);
#endif
// 	_MyMapIter iter = m_mapResItem.begin();
// 	_MyMapIter iterEnd = m_mapResItem.end();
// 
// 	for (; iter != iterEnd; iter++)
// 	{
// 		LayoutResItem* pItem = iter->second;
// 		SAFE_DELETE(pItem);
// 	}
// 	m_mapResItem.clear();
}

ILayoutRes*  LayoutRes::GetILayoutRes()
{
    if (NULL == m_pILayoutRes)
        m_pILayoutRes = new ILayoutRes(this);

    return m_pILayoutRes;
}
// 
// LayoutResItem*  LayoutRes::AddResItem(Object* pObj)
// {
// 	if (NULL == pObj)
// 		return NULL;
// 
// 	_MyMapIter iter = m_mapResItem.find(pObj);
// 	if (iter != m_mapResItem.end())
// 		return iter->second;
// 
// 	LayoutResItem* pItem = new LayoutResItem;
// 	m_mapResItem[pObj] = pItem;
// 	return pItem;
// }
// LayoutResItem*  LayoutRes::GetResItem(Object* pObj)
// {
// 	if (NULL == pObj)
// 		return NULL;
// 
// 	_MyMapIter iter = m_mapResItem.find(pObj);
// 	if (iter != m_mapResItem.end())
// 		return iter->second;
// 
// 	return NULL;
// }