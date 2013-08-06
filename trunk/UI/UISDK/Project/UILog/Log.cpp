// Log.cpp : CLog µÄÊµÏÖ

#include "stdafx.h"
#include "Log.h"


// CLog

STDMETHODIMP CLog::Create(BSTR bstrXmlPath)
{
	if (NULL == m_pLogManager)
		return E_FAIL;

	bool bRet = m_pLogManager->Create(bstrXmlPath);
	if (bRet)
		return S_OK;
	else
		return E_FAIL;
}
STDMETHODIMP CLog::Load(BSTR strXmlPath)
{
	if (NULL == m_pLogManager)
		return E_FAIL;

	bool bRet = m_pLogManager->Load(strXmlPath);

	if (bRet)
		return S_OK;
	else
		return E_FAIL;
}
STDMETHODIMP CLog::Save()
{
	if (NULL == m_pLogManager)
		return E_FAIL;

	bool bRet = m_pLogManager->Save();
	if (bRet)
		return S_OK;
	else
		return E_FAIL;
}
STDMETHODIMP CLog::IsDirty(VARIANT_BOOL* pbDirty)
{
	if (NULL == pbDirty)
		return E_INVALIDARG;

	if (NULL == m_pLogManager)
		return E_FAIL;

	bool bRet = m_pLogManager->IsDirty();
	if (bRet)
		*pbDirty = VARIANT_TRUE;
	else
		*pbDirty = VARIANT_FALSE;

	return S_OK;
}
STDMETHODIMP CLog::Log(long lCookie, LOG_LEVEL level, BSTR bstrContent)
{
	if (NULL == m_pLogManager)
		return E_FAIL;

	m_pLogManager->Log(lCookie, level, bstrContent);

	return S_OK;
}

STDMETHODIMP CLog::TestCanLog(long lCookie, LOG_LEVEL level, VARIANT_BOOL* pbRet)
{
	if (NULL == m_pLogManager || NULL == pbRet)
		return E_FAIL;

	bool bRet = (LONG)m_pLogManager->TestCanLog(lCookie, level);
	if (bRet)
		*pbRet = VARIANT_TRUE;
	else
		*pbRet = VARIANT_FALSE;
	

	return S_OK;
}
STDMETHODIMP CLog::EnableLog(VARIANT_BOOL bEnable)
{
	if (NULL == m_pLogManager)
		return E_FAIL;

	m_pLogManager->EnableLog(VARIANT_TRUE==bEnable?true:false);

	return S_OK;
}

STDMETHODIMP CLog::IsEnable(VARIANT_BOOL* pbEnable)
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	if (NULL == pbEnable)
		return E_INVALIDARG;

	bool b = m_pLogManager->IsEnable();
	if (b)
		*pbEnable = VARIANT_TRUE;
	else
		*pbEnable = VARIANT_FALSE;

	return S_OK;
}
STDMETHODIMP CLog::AddLogItem(LOGITEM* pLogItem, long* pOutCookie)
{
	if (NULL == pOutCookie || NULL == pLogItem)
		return E_INVALIDARG;

	if (NULL == m_pLogManager)
		return E_FAIL;

	*pOutCookie = m_pLogManager->AddLogItem(pLogItem);
	
	if (*pOutCookie != -1)
		return S_OK;
	else
		return E_FAIL;
}
STDMETHODIMP CLog::ModifyLogItem(long lCookie, LOGITEM* pLogItem)
{
	if (NULL == pLogItem)
		return E_INVALIDARG;

	if (NULL == m_pLogManager)
		return E_FAIL;

	bool bRet = m_pLogManager->ModifyLogItem(lCookie, pLogItem);

	if (bRet)
		return S_OK;
	else
		return E_FAIL;
}

STDMETHODIMP CLog::DeleteLogItem(long lCookie)
{
	if (NULL == m_pLogManager)
		return E_FAIL;

	bool bRet = m_pLogManager->DeleteLogItem(lCookie);

	if (bRet)
		return S_OK;
	else
		return E_FAIL;

	return S_OK;
}

STDMETHODIMP CLog::GetLogItemCount(long* pRetVal)
{
	if (NULL == pRetVal)
		return E_INVALIDARG;

	if (NULL == m_pLogManager)
		return E_FAIL;

	*pRetVal = m_pLogManager->GetLogItemCount();

	return S_OK;
}

STDMETHODIMP CLog::GetLogItemByIndex(long lIndex, ILogItem** ppRetVal)
{
	if (NULL == ppRetVal)
		return E_INVALIDARG;

	if (NULL == m_pLogManager)
		return E_FAIL;

	*ppRetVal = m_pLogManager->GetLogItemByIndex((int)lIndex);
	return S_OK;
}

STDMETHODIMP CLog::GetLogItemByName(BSTR bstrLogItemName, ILogItem** ppRetVal)
{
	if (NULL == ppRetVal)
		return E_INVALIDARG;

	if( NULL == m_pLogManager )
		return E_FAIL;

	*ppRetVal = m_pLogManager->GetLogItemByName(bstrLogItemName);
	return S_OK;
}

STDMETHODIMP CLog::GetLogItemByCookie(long lCookie, ILogItem **ppRetVal)
{
	if (NULL == ppRetVal)
		return E_INVALIDARG;

	if( NULL == m_pLogManager )
		return E_FAIL;

	*ppRetVal = m_pLogManager->GetLogItemByCookie(lCookie);
	return S_OK;
}