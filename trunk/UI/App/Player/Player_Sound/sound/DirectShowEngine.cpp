#include "StdAfx.h"
#include "DirectShowEngine.h"

CDirectShowEngine::CDirectShowEngine(void)
{
	m_pGraphBuilder = NULL;
	m_pMediaControl = NULL;
	m_pMediaEventEx = NULL;
	m_pMediaSeeking = NULL;
	m_pMediaPosition = NULL;
	m_pBasicAudio = NULL;

	m_nVolumn = 0;
	m_bMute = false;

	m_pMgr = NULL;
	m_pWnd = NULL;
}

CDirectShowEngine::~CDirectShowEngine(void)
{
	this->Release();
}

HRESULT CDirectShowEngine::Init(CMP3* pMgr, CMessageOnlyWindow* pWndEvent)
{
	if (NULL == pWndEvent || NULL == pMgr)
		return E_INVALIDARG;

	m_pMgr = pMgr;
	m_pWnd = pWndEvent;

	if (m_pGraphBuilder)
		return S_FALSE;

	HRESULT hr = E_FAIL;
	do 
	{
		hr = m_pGraphBuilder.CoCreateInstance(CLSID_FilterGraph);
		if( FAILED(hr) )
			break;

		hr = m_pGraphBuilder->QueryInterface(&m_pMediaPosition);
		if( NULL == m_pMediaPosition )
			break;

		hr = m_pGraphBuilder->QueryInterface(&m_pMediaSeeking);
		if( NULL == m_pMediaSeeking )
			break;

		hr = m_pGraphBuilder->QueryInterface(&m_pMediaEventEx);
		if( NULL == m_pMediaEventEx )
			break;

		hr = m_pGraphBuilder->QueryInterface(&m_pMediaControl);
		if( NULL == m_pMediaControl )
			break;

		hr = m_pGraphBuilder->QueryInterface(&m_pBasicAudio);
		if( NULL == m_pBasicAudio )
			break;

		hr = m_pMediaEventEx->SetNotifyWindow((OAHWND)m_pWnd->m_hWnd, EVENTEX_NOTIFY_MSG, (LONG_PTR)this );
		if( FAILED(hr) )
			break;

		hr = S_OK;
	} while (0);

	if (FAILED(hr))
	{
		this->Release();
	}

	return hr;
}


HRESULT CDirectShowEngine::Release()
{
	SAFE_RELEASE2(m_pBasicAudio);
	SAFE_RELEASE2(m_pMediaPosition);
	SAFE_RELEASE2(m_pMediaSeeking);
	SAFE_RELEASE2(m_pMediaEventEx);
	SAFE_RELEASE2(m_pMediaControl);
	SAFE_RELEASE2(m_pGraphBuilder);

	return S_OK;
}

HRESULT CDirectShowEngine::RenderFile( const TCHAR* szFile, eMp3Type eType)
{
	if (NULL == szFile)
		return E_INVALIDARG;

	this->ClearRender();

	// ע. �����.wma�ļ���Ϊ.mp3��׺��RenderFileҲ�޷�ʶ����������Ǹ����磡
	HRESULT hr = m_pGraphBuilder->RenderFile(szFile, NULL);   // render file����ɾ����ǰ��filter
	if (FAILED(hr))
		return hr;

// 	if (m_pBasicAudio)
// 	{
// 		long lVolumn = 0;
// 		HRESULT hr = m_pBasicAudio->get_Volume(&lVolumn); // ��RenderFile֮ǰ����get_Volumn�᷵��E_NOTIMPL
// 		m_nVolumn = lVolumn;
// 
// 		if (m_pMgr)
// 		{
// 			m_pMgr->Fire_on_mp3_volume_ind(lVolumn);
// 		}
// 	}	

	return hr;
}

HRESULT CDirectShowEngine::ClearRender()
{
	if (NULL == m_pGraphBuilder)
		return E_FAIL;

	IEnumFilters* pEnum = NULL;
	IBaseFilter*  pFilter = NULL;
    IBaseFilter*  pPrevFilter = NULL;  // ��¼��һ��Filter����ֹ��ѭ��.TODO: ��ȷ��Ӧ����ôд��Ϊʲô��һַö�ٵ�ͬһ��

	HRESULT hr = m_pGraphBuilder->EnumFilters(&pEnum);
	while (S_OK == pEnum->Next(1,&pFilter,NULL))
	{
        if (pFilter == pPrevFilter)
        {
            pFilter->Release();
            break;
        }

		m_pGraphBuilder->RemoveFilter(pFilter);
        pPrevFilter = pFilter;
		pFilter->Release();
		pFilter = NULL;

		//Update the enumerator by calling the IEnumFilters::Reset method. You can then call the Next method safely.
		pEnum->Reset();    // ע�⣺�����reset���ᵼ�²�����wma���ٲ���mp3��ʧ�ܡ���Ϊû��ɾ���ɾ�
	}
	pEnum->Release();
	return S_OK;
}

HRESULT CDirectShowEngine::Play()
{
	if( NULL == m_pMediaControl || NULL == m_pMediaPosition )
		return E_FAIL;

	LONG lState = 0;
	HRESULT hr = this->m_pMediaControl->GetState(0, &lState);
	if( State_Stopped == lState )
	{
		hr = m_pMediaPosition->put_CurrentPosition(0);
	}

	hr = m_pMediaControl->Run();
//	m_pWnd->StartTimer(true);

	return hr;
}
HRESULT CDirectShowEngine::Pause()
{
	if( NULL == m_pMediaControl )
		return E_FAIL;

	HRESULT hr = m_pMediaControl->Pause();
//	m_pWnd->EndTimer();

	return hr;
}
HRESULT CDirectShowEngine::Stop()
{
	if( NULL == m_pMediaControl )
		return E_FAIL;

	HRESULT hr = m_pMediaControl->Stop();
//	m_pWnd->EndTimer();

	return hr;
}

HRESULT CDirectShowEngine::SetCurPos(double percent)
{
	if( NULL == m_pMediaPosition )
		return E_FAIL;

	REFTIME len;
	this->m_pMediaPosition->get_Duration(&len);

	REFTIME cur = len * percent;
	HRESULT hr = this->m_pMediaPosition->put_CurrentPosition(cur);

// 	// ֪ͨ�����������µ�ǰ����
// 	if (m_pWnd)
// 		::PostMessage(m_pWnd->m_hWnd, WM_TIMER, TIMER_ID_PROGRESS, 0);

	return hr;
}

HRESULT CDirectShowEngine::GetCurPos(double* pdSeconds, double* pdPercent)
{
	if( NULL == m_pMediaSeeking )
		return E_FAIL;

	if (NULL == pdSeconds || NULL == pdPercent)
		return E_INVALIDARG;

	LONGLONG llDuration = 0, llCur = 0;
	HRESULT hr = m_pMediaSeeking->GetDuration(&llDuration);
	if (FAILED(hr))
		return hr;

	hr = m_pMediaSeeking->GetCurrentPosition(&llCur);
	if (FAILED(hr))
		return hr;

	if (0 == llDuration)
		return E_FAIL;

	*pdPercent = llCur * 1.0 / llDuration;
	*pdSeconds = (double)(llCur/10000000);

	return S_OK;
}


HRESULT CDirectShowEngine::SetVolume(long lVolumn)
{
	HRESULT hr = this->m_pBasicAudio->put_Volume(lVolumn);
	return hr;
}


//
// ��MessageOnlyWndת����������Ϣ�����ڻ�ȡ���Ž�������Ϣ
//
void CDirectShowEngine::OnNoitfy(UINT uMsg, WPARAM w, LPARAM l)
{
	if( NULL == m_pMediaEventEx )
		return ;

	long  lEventCode = 0;
	long  lParam = 0;
	long  lParam2 = 0;
	long  msTimeout = 0;

	HRESULT hr = m_pMediaEventEx->GetEvent( &lEventCode, &lParam, &lParam2, msTimeout );
	if( FAILED(hr) )
		return;

	switch (lEventCode)
	{
	case EC_COMPLETE:  // ���Ž���
		{
//			this->m_pWnd->EndTimer();
			m_pMgr->Fire_on_mp3_finish();
		}
		break;
	}

	m_pMediaEventEx->FreeEventParams(lEventCode, lParam, lParam2 );

	return;
}