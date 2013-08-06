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

// void CMessageOnlyWindow::OnTimer(UINT_PTR nIDEvent)
// {
// 	if (TIMER_ID_PROGRESS == nIDEvent)
// 	{
// 		if (NULL == m_pMgr->m_pCurrentEngine)
// 		{
// 			this->EndTimer();
// 			return;
// 		}
// 		double dSeconds = 0, dPercent = 0;
// 		HRESULT hr = m_pMgr->m_pCurrentEngine->GetCurPos(&dSeconds, &dPercent);
// 		if (FAILED(hr))
// 			return;
// 
// 		m_pMgr->Fire_on_mp3_progress_ind(dSeconds, dPercent);
// 	}
// }
// 
// void CMessageOnlyWindow::StartTimer(bool bStartNow)
// {
// 	::SetTimer(m_hWnd, TIMER_ID_PROGRESS, 300, NULL);  // 500ms会有两次变换不均匀的现象，设成1s内有3次变换机会
// 
// 	if( bStartNow )
// 		this->OnTimer(TIMER_ID_PROGRESS);
// }
// void CMessageOnlyWindow::EndTimer()
// {
// 	::KillTimer(m_hWnd, TIMER_ID_PROGRESS);
// }