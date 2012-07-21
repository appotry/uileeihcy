// Log.cpp : CLog µÄÊµÏÖ

#include "stdafx.h"
#include "Log.h"


// CLog

STDMETHODIMP CLog::Create(BSTR strXmlPath,LONG* pRetVal)
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	bool bRet = m_pLogManager->Create( strXmlPath );

	if( bRet )
		*pRetVal = 1;
	else
		*pRetVal = 0;

	return S_OK;
}
STDMETHODIMP CLog::Load(BSTR strXmlPath,LONG* pRetVal)
{
	if( NULL == pRetVal )
		return E_INVALIDARG;

	if( NULL == m_pLogManager )
		return E_FAIL;

	bool bRet = m_pLogManager->Load(strXmlPath );

	if( bRet )
		*pRetVal = 1;
	else
		*pRetVal = 0;

	return S_OK;
}
STDMETHODIMP CLog::Save( LONG* pRetVal )
{
	if( NULL == pRetVal )
		return E_INVALIDARG;

	if( NULL == m_pLogManager )
		return E_FAIL;

	bool bRet = m_pLogManager->Save();
	if( bRet )
		*pRetVal = 1;
	else
		*pRetVal = 0;

	return S_OK;
}
STDMETHODIMP CLog::IsDirty(LONG* pRetVal)
{
	if( NULL == pRetVal )
		return E_INVALIDARG;

	if( NULL == m_pLogManager )
		return E_FAIL;

	bool bRet = m_pLogManager->IsDirty();
	if( bRet )
		*pRetVal = 1;
	else
		*pRetVal = 0;

	return S_OK;
}
STDMETHODIMP CLog::Log(BSTR bstrLogItem, LONG level, BSTR bstrContent)
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	m_pLogManager->Log( bstrLogItem, (LOG_LEVEL)level, bstrContent );

	return S_OK;
}

STDMETHODIMP CLog::TestCanLog(BSTR bstrLogItem, LONG level, LONG* lRet)
{
	if( NULL == m_pLogManager || NULL == lRet )
		return E_FAIL;

	*lRet = (LONG)m_pLogManager->TestCanLog( bstrLogItem, (LOG_LEVEL)level );

	return S_OK;
}
STDMETHODIMP CLog::EnableLog( LONG lSwitch ,LONG* pRetVal)
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	if (NULL==pRetVal)
	{
		return E_POINTER;
	}

	if( 0 == lSwitch )
		m_pLogManager->EnableLog(false);
	else
		m_pLogManager->EnableLog(true);

	*pRetVal = 1;
	return S_OK;
}

STDMETHODIMP CLog::IsEnable(LONG* pRetVal)
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	if (NULL==pRetVal)
		return E_POINTER;

	bool b = m_pLogManager->IsEnable();
	if( b)
		*pRetVal = 1;
	else
		*pRetVal = 0;

	return S_OK;
}
STDMETHODIMP CLog::AddLogItem(BSTR bstrLogItem, LONG bSwitch,  LONG level, BSTR bstrLogPath, LONG lLogFlag, LONG lFileLog,LONG* pRetVal )
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	bool bOnOrOff = bSwitch==0?false:true;
	bool bRet = m_pLogManager->AddLogItem( bstrLogItem, bOnOrOff, level, bstrLogPath, lLogFlag, lFileLog );
	
	if( bRet )
		*pRetVal = 1;
	else
		*pRetVal = 0;

	return S_OK;
}
STDMETHODIMP CLog::ModifyLogItem(BSTR bstrOldLogItemName, BSTR bstrNewLogItemName, LONG bSwitch, LONG level, BSTR bstrLogPath, LONG lLogFlag, LONG lFileLog,LONG* pRetVal )
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	bool bOnOrOff = bSwitch==0?false:true;
	bool bRet = m_pLogManager->ModifyLogItem(bstrOldLogItemName, bstrNewLogItemName, bOnOrOff, level, bstrLogPath, lLogFlag, lFileLog );

	if( bRet )
		*pRetVal = 1;
	else
		*pRetVal = 0;

	return S_OK;
}

STDMETHODIMP CLog::RemoveLogItem( BSTR bstrLogItemName, LONG* pRetVal )
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	bool bRet = m_pLogManager->RemoveLogItem(bstrLogItemName);

	if( bRet )
		*pRetVal = 1;
	else
		*pRetVal = 0;

	return S_OK;
}

STDMETHODIMP CLog::GetLogItemCount( LONG* pRetVal )
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	if( NULL == pRetVal )
		return E_INVALIDARG;

	int nCount = m_pLogManager->GetLogItemCount();
	*pRetVal = nCount;

	return S_OK;
}

STDMETHODIMP CLog::GetLogItemByIndex( LONG lIndex, ILogItem** ppRetVal )
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	m_pLogManager->GetLogItem( (int)lIndex, ppRetVal );
	return S_OK;
}

STDMETHODIMP CLog::GetLogItemByName( BSTR bstrLogItemName, ILogItem** ppRetVal )
{
	if( NULL == m_pLogManager )
		return E_FAIL;

	m_pLogManager->GetLogItem( String(bstrLogItemName), ppRetVal );
	return S_OK;
}