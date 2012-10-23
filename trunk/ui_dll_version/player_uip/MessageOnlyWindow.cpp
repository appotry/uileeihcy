#include "StdAfx.h"
#include "MessageOnlyWindow.h"
#include "Sound\\DirectShowEngine.h"
#include "Sound\\DirectSoundEngine.h"

CMessageOnlyWindow::CMessageOnlyWindow()
{
	m_pMgr = NULL;
// 	m_pDirectShow = NULL;
// 	m_pDirectSound = NULL;
}
void CMessageOnlyWindow::SetEnginePtr(CMP3* pMgr, CDirectShowEngine* p1, CDirectSoundEngine* p2)
{
	m_pMgr = pMgr;
// 	m_pDirectShow = p1;
// 	m_pDirectSound = p2;
}


HRESULT CMessageOnlyWindow::OnNotifyMsg(UINT msg, WPARAM w, LPARAM l)
{
	ISoundEngine* pThis = (CDirectShowEngine*)l;
	if( NULL == pThis )
		return 0;

	pThis->OnNoitfy(msg, w, l);
	return 0;
}

void CMessageOnlyWindow::OnTimer(UINT_PTR nIDEvent)
{
	if (TIMER_ID_PROGRESS == nIDEvent)
	{
#if 0
		if( NULL != m_pThis->m_pMediaPosition )
		{
			REFTIME rt, len;
			m_pThis->m_pMediaPosition->get_CurrentPosition(&rt);
			m_pThis->m_pMediaPosition->get_Duration(&len);

			list<IMp3EventCallback*>::iterator iter = m_pThis->m_listEventCallback.begin();
			list<IMp3EventCallback*>::iterator iterEnd = m_pThis->m_listEventCallback.end();

			for(; iter != iterEnd; iter++)
			{
				(*iter)->on_mp3_progress_ind(rt,len);
			}
		}
#elif 0
		if( NULL != m_pDirectShow->m_pMediaSeeking )
		{
			LONGLONG llDuration = 0, llCur = 0;
			HRESULT hr = m_pDirectShow->m_pMediaSeeking->GetDuration(&llDuration);
			if( FAILED(hr) )
				return;

			hr = m_pDirectShow->m_pMediaSeeking->GetCurrentPosition(&llCur);
			if( FAILED(hr) )
				return;

			m_pDirectShow->m_pMgr->Fire_on_mp3_progress_ind(llCur, llDuration);
		}
#else
		if (NULL == m_pMgr->m_pCurrentEngine)
		{
			this->EndTimer();
			return;
		}
		double dSeconds = 0, dPercent = 0;
		HRESULT hr = m_pMgr->m_pCurrentEngine->GetCurPos(&dSeconds, &dPercent);
		if (FAILED(hr))
			return;

		m_pMgr->Fire_on_mp3_progress_ind(dSeconds, dPercent);
#endif
	}
}

void CMessageOnlyWindow::StartTimer(bool bStartNow)
{
	::SetTimer( m_hWnd, TIMER_ID_PROGRESS, 500, NULL );

	if( bStartNow )
		this->OnTimer(TIMER_ID_PROGRESS);
}
void CMessageOnlyWindow::EndTimer()
{
	::KillTimer( m_hWnd, TIMER_ID_PROGRESS );
}