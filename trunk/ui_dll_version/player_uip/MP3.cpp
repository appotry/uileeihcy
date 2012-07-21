#include "StdAfx.h"
#include "MP3.h"
#include "player_uip.h"

CMessageOnlyWindow::CMessageOnlyWindow(CMP3* p)
{
	m_pThis = p;
}

CMP3::CMP3(void):m_hWndEvent(this)
{
	m_pGraphBuilder = NULL;
	m_pMediaControl = NULL;
	m_pMediaEventEx = NULL;
	m_pMediaSeeking = NULL;
	m_pMediaPosition = NULL;
	m_pBasicAudio = NULL;

	m_nVolumn = 0;
	m_bMute = false;
}

CMP3::~CMP3(void)
{
	this->Release();
}

bool CMP3::Init()
{
	if( NULL != m_pGraphBuilder )
		return false;

	bool bRet = false;
	do 
	{
		HRESULT hr = m_pGraphBuilder.CoCreateInstance(CLSID_FilterGraph);
		if( FAILED(hr) )
			break;

		m_pGraphBuilder->QueryInterface(&m_pMediaPosition);
		if( NULL == m_pMediaPosition )
			break;

		m_pGraphBuilder->QueryInterface(&m_pMediaSeeking);
		if( NULL == m_pMediaSeeking )
			break;

		m_pGraphBuilder->QueryInterface(&m_pMediaEventEx);
		if( NULL == m_pMediaEventEx )
			break;

		m_pGraphBuilder->QueryInterface(&m_pMediaControl);
		if( NULL == m_pMediaControl )
			break;

		m_pGraphBuilder->QueryInterface(&m_pBasicAudio);
		if( NULL == m_pBasicAudio )
			break;

		if( NULL == m_hWndEvent.Create(HWND_MESSAGE) )
			break;

		hr = m_pMediaEventEx->SetNotifyWindow((OAHWND)m_hWndEvent.m_hWnd, EVENTEX_NOTIFY_MSG, (LONG_PTR)this );
		if( FAILED(hr) )
			break;

		bRet = true;
	} while (0);
	
	if( false == bRet )
	{
		this->Release();
	}
		
	return bRet;
}


bool CMP3::Release()
{
	SAFE_RELEASE2(m_pBasicAudio);
	SAFE_RELEASE2(m_pMediaPosition);
	SAFE_RELEASE2(m_pMediaSeeking);
	SAFE_RELEASE2(m_pMediaEventEx);
	SAFE_RELEASE2(m_pMediaControl);
	SAFE_RELEASE2(m_pGraphBuilder);
	if( m_hWndEvent.IsWindow() )
	{
		m_hWndEvent.DestroyWindow();

	}

	return true;
}

bool CMP3::RenderFile( const String& strFile )
{
	if( NULL == m_pGraphBuilder )
		return false;

	IEnumFilters* pEnum = NULL;
	IBaseFilter*  pFilter = NULL;

	m_pGraphBuilder->EnumFilters(&pEnum);
	while( S_OK == pEnum->Next(1,&pFilter,NULL) )
	{
		m_pGraphBuilder->RemoveFilter(pFilter);
	}

	HRESULT hr = m_pGraphBuilder->RenderFile( strFile.c_str(), NULL );   // render file不会删除以前的filter
	if( FAILED(hr) )
		return false;

	if( NULL != m_pBasicAudio )
	{
		long lVolumn = 0;
		HRESULT hr = m_pBasicAudio->get_Volume(&lVolumn); // 在RenderFile之前调用get_Volumn会返回E_NOTIMPL
		m_nVolumn = lVolumn;
		
		list<IMp3EventCallback*>::iterator iter = this->m_listEventCallback.begin();
		list<IMp3EventCallback*>::iterator iterEnd = this->m_listEventCallback.end();

		for(; iter != iterEnd; iter++)
		{
			(*iter)->on_mp3_volume_ind(lVolumn);
		}
	}	

	return false;
}

bool CMP3::Play()
{
	if( NULL == m_pMediaControl || NULL == m_pMediaPosition )
		return false;

	LONG lState = 0;
	HRESULT hr = this->m_pMediaControl->GetState(0, &lState);
	if( State_Stopped == lState )
	{
		m_pMediaPosition->put_CurrentPosition(0);
	}

	hr = m_pMediaControl->Run();
	m_hWndEvent.StartTimer(true);

	if( FAILED(hr) )
		return false;
	else
		return true;
}
bool CMP3::Pause()
{
	if( NULL == m_pMediaControl )
		return false;

	HRESULT hr = m_pMediaControl->Pause();
	m_hWndEvent.EndTimer();
	
	if( FAILED(hr) )
		return false;
	else
		return true;
}
bool CMP3::Stop()
{
	if( NULL == m_pMediaControl )
		return false;

	HRESULT hr = m_pMediaControl->Stop();
	m_hWndEvent.EndTimer();

	if( FAILED(hr) )
		return false;
	else
		return true;
}

bool CMP3::SetCurPos(double percent)
{
	if( NULL == m_pMediaPosition )
		return false;

	REFTIME len;
	this->m_pMediaPosition->get_Duration(&len);
	
	REFTIME cur = len * percent / 100.0;
	HRESULT hr = this->m_pMediaPosition->put_CurrentPosition(cur);

	if( SUCCEEDED(hr) )
		return true;

	return false;
}

int g_volumes[] = 
	{-10000,-6418,-6147,-6000,
	-5892,-4826,-4647,-4540
	-4477, -4162,-3876, -3614, -3500,
	-3492,-3374,-3261,-3100,-3153,-3048,-2947,-2849,-2755,-2700,
	-2663,-2575,-2520,-2489,-2406,-2325,-2280,-2246,-2170,-2095,-2050,
	-2023,-1952,-1900, -1884,-1834, -1820, -1800,-1780, -1757,-1695,-1636,-1579,
	-1521,-1500,-1464,-1436,-1420, -1408,-1353,-1299,-1246,-1195,-1144,
	-1096,-1060, -1049,-1020,-1003,-957,-912,-868, -800, -774,-784, -760, -744,
	-705,-667,-630,-610,-594,-570 ,-558,-525,-493,-462,-432,-403,
	-375,-348,-322,-297,-285, -273,-250,-228,-207,-187,-176, -168,
	-150,-102,-75,-19,-10,0,0};


bool CMP3::SetVolume(double percent)
{
	if( NULL == m_pBasicAudio )
		return false;


//	long lVolumn = (long)( percent*100.0 - 10000 );
	if( percent < 0 )
		percent = 0;
	if( percent >= sizeof(g_volumes)/sizeof(int) )
		percent = sizeof(g_volumes)/sizeof(int) -1;

	long lVolumn = (long)( g_volumes[(int)percent] );
	if( m_bMute )   // 当前处于静音状态，不能调用put_Volumn
	{
		m_nVolumn = lVolumn;
		return true;
	}

	HRESULT hr = this->m_pBasicAudio->put_Volume(lVolumn);

	if( SUCCEEDED(hr) )
	{
		m_nVolumn = lVolumn;
		return true;
	}

	return false;
}

bool CMP3::Mute(bool bMute)
{
	if( NULL == m_pBasicAudio )
		return false;

	m_bMute = bMute;

	if( bMute )
		m_pBasicAudio->put_Volume(-10000);
	else
		m_pBasicAudio->put_Volume(m_nVolumn);

	return true;
}

bool CMP3::AddEventCallback(IMp3EventCallback* p)
{
	if( NULL == p )
		return false;

	// TODO: 检测是否存在
	m_listEventCallback.push_back(p);

	return true;
}

HRESULT CMessageOnlyWindow::OnNotifyMsg(UINT msg, WPARAM w, LPARAM l)
{
	CMP3* pThis = (CMP3*)l;
	if( NULL == pThis )
		return 0;

	if( NULL == pThis->m_pMediaEventEx )
		return 0;

	long  lEventCode = 0;
	long  lParam = 0;
	long  lParam2 = 0;
	long  msTimeout = 0;

	HRESULT hr = pThis->m_pMediaEventEx->GetEvent( &lEventCode, &lParam, &lParam2, msTimeout );
	if( FAILED(hr) )
		return 0;

	switch( lEventCode )
	{
	case EC_COMPLETE:  // 播放结束
		{
			list<IMp3EventCallback*>::iterator iter = pThis->m_listEventCallback.begin();
			list<IMp3EventCallback*>::iterator iterEnd = pThis->m_listEventCallback.end();

			for(; iter != iterEnd; iter++)
			{
				(*iter)->on_mp3_stop();
			}
		}
		break;
	}

	m_pThis->m_pMediaEventEx->FreeEventParams(lEventCode, lParam, lParam2 );

	return 0;
}

void CMessageOnlyWindow::OnTimer(UINT_PTR nIDEvent)
{
	if( TIMER_ID_PROGRESS == nIDEvent )
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
#else
		if( NULL != m_pThis->m_pMediaSeeking )
		{
			LONGLONG llDuration = 0, llCur = 0;
			HRESULT hr = m_pThis->m_pMediaSeeking->GetDuration(&llDuration);
			if( FAILED(hr) )
				return;

			hr = m_pThis->m_pMediaSeeking->GetCurrentPosition(&llCur);
			if( FAILED(hr) )
				return;

			list<IMp3EventCallback*>::iterator iter = m_pThis->m_listEventCallback.begin();
			list<IMp3EventCallback*>::iterator iterEnd = m_pThis->m_listEventCallback.end();

			for(; iter != iterEnd; iter++)
			{
				(*iter)->on_mp3_progress_ind(llCur,llDuration);
			}
		}
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