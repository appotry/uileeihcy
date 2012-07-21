#pragma once

namespace UI { namespace Util { 

template< class T>
class CBufferT
{
	T*			m_pBuf;
	DWORD		m_dwSize;

public:
	CBufferT()
	{
		m_pBuf		= NULL;
		m_dwSize	= 0;
	}
	~CBufferT()
	{
		Release();
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

	operator T*()
	{
		return m_pBuf;
	}
	T& operator []( int i )
	{
		return m_pBuf[i];
	}

	DWORD	GetBufSize()
	{
		return this->m_dwSize;
	}
private:

	void Release()
	{
		if( m_pBuf != NULL )
		{
			delete []m_pBuf;
			m_pBuf		= NULL;
			m_dwSize	= 0;
		}
	}
};
#define CBuffer CBufferT<TCHAR>


//////////////////////////////////////////////////////////////////////////
//
//                       数据结构
//
//////////////////////////////////////////////////////////////////////////

#ifdef UTIL_DATA_STRUCTURE 

//////////////////////////////////////////////////////////////////////////
//
// 单链表（线性链表）(未完成...突然不需要了)

template<class T>
class LinkList_Node
{
public:
	T               data;
	LinkList_Node*  next;
};

vector
template<class T>
class CLinkList
{
public:
	T& operator[](int index)          //  不做任何的安全检查
	{
		LinkList_Node* p = head;

		int i = 0;
		while(i < index)
		{
			p = p->next;  j++;
		}
		return p->data;
	}
	bool GetAt(int index, T& out)     // 做安全检查
	{
		if( index < 0 )
			return false;

		LinkList_Node* p = head;

		int i = 0;
		while( p && i < index)
		{
			p = p->next;  j++;
		}

		if( NULL == p )
			return false;

		out = p->data;
		return true;
	}
	bool push_back( const T& val )
	{
				
	}


private:
	LinkList_Node*  head;
};


#endif

}}