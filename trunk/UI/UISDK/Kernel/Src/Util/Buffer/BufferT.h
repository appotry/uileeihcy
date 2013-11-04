#pragma once
#include "UISDK\Kernel\Inc\Util\ibuffer.h"

namespace UI
{

template< class T>
class CBufferT : public IBuffer
{
	T*			m_pBuf;
	DWORD		m_dwSize;
    long   m_lRef;
public:
	CBufferT()
	{
		m_pBuf = NULL;
		m_dwSize = 0;
        m_lRef = 0;
        AddRef();
	}
	~CBufferT()
	{
        free();
	}

	T* malloc( unsigned int nSize )
	{
		Release();
		m_pBuf = new T[ nSize ];
		::ZeroMemory( m_pBuf, nSize );

		m_dwSize = nSize;
		return m_pBuf;
	}
	T* realloc( unsigned int nSize )
	{
		return malloc( nSize );
	}
    void free()
    {
        if(m_pBuf)
        {
            delete []m_pBuf;
            m_pBuf		= NULL;
            m_dwSize	= 0;
        }
    }

	operator T*()
	{
		return m_pBuf;
	}
	T& operator []( int i )
	{
		return m_pBuf[i];
	}

     virtual void  SetBuffer(void* pBuffer, ULONG nSize)
     {
        free();
        m_dwSize = nSize;
        m_pBuf = (T*)pBuffer;
     }

	virtual ULONG GetSize()
	{
		return this->m_dwSize;
	}

    virtual void*  GetBuffer()
    {
        return (void*)m_pBuf;
    }
	virtual void Release()
	{
        m_lRef --;
        
        if (0 == m_lRef)
        {
            delete this;
            return;
        }
	}
    virtual void  AddRef()
    {
        m_lRef++;
    }
};
#define CBuffer CBufferT<TCHAR>

}