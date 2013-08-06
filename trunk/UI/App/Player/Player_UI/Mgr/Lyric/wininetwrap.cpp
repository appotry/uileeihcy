#include "stdafx.h"
#include "wininetwrap.h"

#include <Wininet.h>
#pragma comment(lib, "Wininet.lib")

class CBuffer : public IBuffer
{
public:
    CBuffer()
    {   
        m_pData = NULL;
        m_nCapacity = 0;
        m_nSize = 0;
    }
    ~CBuffer()
    {
        Free();
    }
    virtual void   Alloc(DWORD dwSize)
    {
        if (0 == dwSize)
            return;

        m_pData = new byte[dwSize];
        memset(m_pData, 0, dwSize);

        m_nCapacity = dwSize;
    }
    virtual void  Free()
    {
        if (m_pData)
        {
            delete [] m_pData;
            m_pData = NULL;
            m_nCapacity = 0;
            m_nSize = 0;
        }
    }
    virtual void  Release()
    {
        delete this;
    }

    virtual byte*  Get() { return m_pData; }
    virtual void  SetSize(int nSize)
    {
        m_nSize = nSize;
    }
    virtual DWORD  GetSize() { return m_nSize; }
    virtual DWORD  GetCapacity() { return m_nCapacity; }
    virtual void  ReallocMore(DWORD newSize)
    {
        if (NULL == m_pData)
        {
            Alloc(newSize);
            return;
        }
        if (newSize < m_nCapacity)
            return;

        byte*  pNewData = new byte[newSize];
        memset(pNewData, 0, newSize);

        memcpy(pNewData, m_pData, min(newSize, m_nCapacity));
        SAFE_ARRAY_DELETE(m_pData);

        m_pData = pNewData;
        m_nCapacity = newSize;
    }

    byte*  m_pData;
    DWORD  m_nCapacity;   // 所分配的内存
    DWORD  m_nSize;       // 数据大小
};

HRESULT  CreateBuffer(IBuffer** pp)
{
    if (NULL == pp)
        return E_INVALIDARG;

    *pp = new CBuffer(); 
    return S_OK;
}

int  GetHttpData(const TCHAR* szWebAddr, int nPort, const TCHAR* szArg, __in __out IBuffer* pBuffer)
{
    if (NULL == szWebAddr || NULL == pBuffer)
        return -1;

    DWORD  dwReadTotal = 0;
    DWORD  dwRead = 0;

    HINTERNET  hInternet = NULL;
    HINTERNET  hSession = NULL;
    HINTERNET  hRequest = NULL;

    bool bRet = false;
    do 
    {
        hInternet = InternetOpen(NULL, 0, NULL, NULL, 0);
        if (NULL == hInternet)
            break;

        hSession = InternetConnect(hInternet, szWebAddr, nPort, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 0);
        if (NULL == hSession)
            break;

        HINTERNET hRequest = HttpOpenRequest(hSession, _T("GET"), szArg, NULL, NULL, NULL, 0, 0);
        if (NULL == hRequest)
            break;

        if (FALSE == HttpSendRequest(hRequest, NULL, 0, 0, 0))
            break;

        pBuffer->Alloc(128);

        while (1)
        {
            DWORD  dwSize = 0;
            if (!InternetQueryDataAvailable(hRequest, &dwSize, 0, 0))
                break;

            if (0 == dwSize)
                break;

            if (pBuffer->GetSize() + dwSize > pBuffer->GetCapacity())
            {
                pBuffer->ReallocMore(2* (pBuffer->GetSize() + dwSize));
            }

            InternetReadFile(hRequest, pBuffer->Get()+pBuffer->GetSize(), dwSize, &dwRead);
            dwReadTotal += dwRead;
            pBuffer->SetSize(dwReadTotal);

            if (0 == dwRead)
                break;
        }

        bRet = true;
    } while (0);

    if (hRequest)
    {
        InternetCloseHandle(hRequest);
        hRequest = NULL;
    }
    if (hSession)
    {
        hSession = NULL;
        InternetCloseHandle(hSession);
    }
    if (hInternet)
    {
        InternetCloseHandle(hInternet);
        hInternet = NULL;
    }

    if (false == bRet)
        return -1;

    return dwReadTotal;
}