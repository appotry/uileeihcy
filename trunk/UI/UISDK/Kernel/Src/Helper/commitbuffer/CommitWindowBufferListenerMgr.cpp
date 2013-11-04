#include "stdafx.h"
#include "CommitWindowBufferListenerMgr.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"

CommitWindowBufferListenerMgr::CommitWindowBufferListenerMgr()
{	
	m_pWindow = NULL;
}
CommitWindowBufferListenerMgr::~CommitWindowBufferListenerMgr()
{
	m_list.clear();
}

void CommitWindowBufferListenerMgr::DoPre(HDC hDC, HDC hMemDC, RECT* prc, int nRectCount)
{
	if (0 == m_list.size())
		return;

	_MyIter iter = m_list.begin();
	_MyIter iterEnd = m_list.end();
	for (; iter != iterEnd; iter++)
	{
		ICommitWindowBufferListener* pItem = *iter;
		if (NULL == pItem)
			continue;

		pItem->PreCommitWindowBuffer(hDC, hMemDC, m_pWindow->GetIWindowBase(), prc, nRectCount);
	}
}
void CommitWindowBufferListenerMgr::DoPost(HDC hDC, HDC hMemDC, RECT* prc, int nRectCount)
{
	if (0 == m_list.size())
		return;

	_MyIter iter = m_list.begin();
	_MyIter iterEnd = m_list.end();

	for (; iter != iterEnd;)
	{
		ICommitWindowBufferListener* pItem = *iter;
		if (NULL == pItem)
			continue;

		pItem->PostCommitWindowBuffer(hDC, hMemDC, m_pWindow->GetIWindowBase(), prc, nRectCount);

		if (pItem->OnlyListenOnce())
			iter = m_list.erase(iter);
		else
			iter++;
	}
}

void  CommitWindowBufferListenerMgr::AddListener(ICommitWindowBufferListener* p)
{
	if (NULL == p)
		return;

	_MyIter iter = std::find(m_list.begin(), m_list.end(), p);
	if (iter != m_list.end())
		return;

	m_list.push_back(p);
}
void  CommitWindowBufferListenerMgr::RemoveListener(ICommitWindowBufferListener* p)
{
	if (NULL == p)
		return;

	_MyIter iter = std::find(m_list.begin(), m_list.end(), p);
	if (iter != m_list.end())
		m_list.erase(iter);
}
void  CommitWindowBufferListenerMgr::ClearListener()
{
	m_list.clear();
}
