#include "stdafx.h"

Direct2DRenderGlobalData  g_D2DGlobalData;

#pragma region //Global data
Direct2DRenderGlobalData::Direct2DRenderGlobalData()
{
	m_dwRef = 0;
	m_pD2DFactory = NULL;
	m_pDWriteFactory = NULL;
	m_pWICFactory = NULL;
}

//
// Remark;
//	不要尝试在静态/全局类的析构函数中释放COM对象。
//  因为此时COM库可能已经卸载了
//
Direct2DRenderGlobalData::~Direct2DRenderGlobalData()
{
	UIASSERT(0 == m_dwRef);
	UIASSERT(NULL == m_pD2DFactory);
	m_pD2DFactory = NULL;
	m_dwRef = 0;
	m_pDWriteFactory = NULL;
	m_pWICFactory = NULL;
}
long Direct2DRenderGlobalData::AddRef()
{
	m_dwRef ++;
	return m_dwRef;
}
long Direct2DRenderGlobalData::Release()
{
	if (0 == -- m_dwRef)
	{
		SAFE_RELEASE(m_pWICFactory);
		SAFE_RELEASE(m_pDWriteFactory);
		SAFE_RELEASE(m_pD2DFactory);
		UI_LOG_INFO(_T("%s"), FUNC_NAME);
		return 0;
	}
	return m_dwRef;
}

HRESULT Direct2DRenderGlobalData::CreateD2D()
{
	UI_LOG_INFO(_T("%s"), FUNC_NAME);

	HRESULT hr = S_OK;
	if (NULL == m_pD2DFactory)
	{
		// Create a Direct2D factory.
		hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &m_pD2DFactory);
	}

		
	if (NULL == m_pDWriteFactory && SUCCEEDED(hr))
	{
		// Create a DirectWrite factory.
		hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(m_pDWriteFactory),
					reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	}

	if (NULL == m_pWICFactory && SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
					CLSID_WICImagingFactory,
					NULL,
					CLSCTX_INPROC_SERVER,
					IID_IWICImagingFactory,
					reinterpret_cast<void **>(&m_pWICFactory)
					);
	}

	if (FAILED(hr))
	{
		UI_LOG_ERROR(_T("%s failed. hr=0x%08x"), FUNC_NAME, hr);
	}

	return hr;
}
#pragma  endregion

#pragma  region // D2D Font
Direct2DRenderFont::Direct2DRenderFont(IRenderFont** ppOutRef) : IRenderFont((IRenderResource**)ppOutRef)
{
	g_D2DGlobalData.AddRef();

	m_pTextFormat = NULL;
	m_hFont = NULL;
	m_bCreateOrAttach = true;
	m_pLogFont = NULL;
}
Direct2DRenderFont::~Direct2DRenderFont()
{
	UI_LOG_DEBUG(_T("Direct2DRenderFont Delete. ptr=0x%08X"), this);
	this->DestroyFont();

	g_D2DGlobalData.Release();
}

void  Direct2DRenderFont::DestroyFont()
{
	SAFE_RELEASE(m_pTextFormat);
	m_hFont = NULL;
	m_bCreateOrAttach = true;
	m_pLogFont = NULL; 
}

void  Direct2DRenderFont::CreateInstance( IRenderFont** ppOutRef )
{
	UIASSERT(NULL != ppOutRef);
	if( NULL == ppOutRef )
		return;

	Direct2DRenderFont* p = new Direct2DRenderFont(ppOutRef);
	*ppOutRef = p;
}


bool Direct2DRenderFont::Load( LOGFONT* plogfont )
{
	if( NULL == plogfont )
	{
		UI_LOG_WARN(_T("%s plogfont == NULL"), FUNC_NAME);
		return false;
	}
	this->DestroyFont();

	m_pLogFont = plogfont;  // 该plogfont应该是pojo_fontitem中的成员变量
	
	HRESULT hr = E_FAIL;
	if (NULL == g_D2DGlobalData.m_pDWriteFactory)
		g_D2DGlobalData.CreateD2D();
	if (NULL == g_D2DGlobalData.m_pDWriteFactory)
		return false;

	// TODO: 下划线要怎么搞
	UIASSERT(0 == plogfont->lfUnderline);

	// Create a DirectWrite text format object.
	hr = g_D2DGlobalData.m_pDWriteFactory->CreateTextFormat(
			plogfont->lfFaceName,
			NULL,
			plogfont->lfWeight>= FW_BOLD?DWRITE_FONT_WEIGHT_BOLD:DWRITE_FONT_WEIGHT_NORMAL,
			plogfont->lfItalic?DWRITE_FONT_STYLE_ITALIC:DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			(FLOAT)abs(plogfont->lfHeight),
			L"", //locale
			&m_pTextFormat
		);
	if (FAILED(hr))
	{
		UI_LOG_WARN(_T("%s CreateTextFormat failed. hr=0x%08x"), FUNC_NAME, hr);
	}
	

	m_bCreateOrAttach = true;
	return true;
}
bool Direct2DRenderFont::ModifyFont(LOGFONT* plogfont)
{
	bool bRet = this->Load(plogfont);
	if( false == bRet )
	{
		UI_LOG_WARN(_T("%s Load failed."), FUNC_NAME);
	}
	return bRet;
}


void  Direct2DRenderFont::Attach(HFONT hFont)
{
	this->DestroyFont();
	m_hFont = hFont;
	m_bCreateOrAttach = false;
}
HFONT Direct2DRenderFont::Detach()
{
	HFONT hSave = m_hFont;
	m_hFont = NULL;
	m_bCreateOrAttach = true;

	return hSave;
}

SIZE Direct2DRenderFont::MeasureString( const TCHAR* szText, int nLimitWidth)
{
	CSize sizeText(0, 0);
	if (NULL != m_pTextFormat)
	{
		const FLOAT MAX_LAYOUT_SIZE = 65535;

		if (-1 == nLimitWidth)
			nLimitWidth = (int)MAX_LAYOUT_SIZE;

		IDWriteTextLayout* pLayout = NULL;
		HRESULT hr = g_D2DGlobalData.m_pDWriteFactory->CreateTextLayout(
			szText, _tcslen(szText), m_pTextFormat, (FLOAT)nLimitWidth, MAX_LAYOUT_SIZE, &pLayout);
		
		if (SUCCEEDED(hr) && NULL != pLayout)
		{
			DWRITE_TEXT_METRICS m;
			hr = pLayout->GetMetrics(&m);
			if (FAILED(hr))
			{
				UI_LOG_WARN(_T("%s GetMetrics Failed. hr=0x%08X"), FUNC_NAME, hr);
			}
			else
			{
				sizeText.cx = (LONG) m.width+1;
				sizeText.cy = (LONG) m.height+1;
			}

		}
		else
		{
			UI_LOG_WARN(_T("%s CreateTextLayout Failed. hr=0x%08X"), FUNC_NAME, hr);
		}
		SAFE_RELEASE(pLayout);
	}

	return sizeText;
}

UINT Direct2DRenderFont::GetTextMetricsHeight()
{
	UIASSERT(0);
	return 0;
}

HFONT Direct2DRenderFont::GetHFONT()
{
	UIASSERT(0);
	if (m_bCreateOrAttach)
	{
		UIASSERT(0);
	}
	else
	{
		return m_hFont;
	}
	return NULL;
}


bool Direct2DRenderFont::GetLogFont(LOGFONT* plf)
{
	if (NULL == plf)
		return false;

	if (m_bCreateOrAttach)
	{
		memcpy(plf, m_pLogFont, sizeof(LOGFONT));
	}
	else
	{
		if (NULL == m_hFont)
			return false;

		GetObject(m_hFont, sizeof(LOGFONT), plf);
	}
	return true;
}

#pragma endregion

#pragma region // D2D DC
//////////////////////////////////////////////////////////////////////////
Direct2DRenderDC::Direct2DRenderDC()
{
	g_D2DGlobalData.AddRef();
	m_hWnd = NULL;
	m_pRenderTarget = NULL;
	UIASSERT(0);
}
Direct2DRenderDC::Direct2DRenderDC(HDC hDC)
{
	g_D2DGlobalData.AddRef();
	m_hWnd = NULL;
	m_pRenderTarget = NULL;
	UIASSERT(0);
}
Direct2DRenderDC::Direct2DRenderDC(HWND hWnd)
{
	g_D2DGlobalData.AddRef();
	m_hWnd = hWnd;
	m_pRenderTarget = NULL;
	UIASSERT(0);
}
Direct2DRenderDC::Direct2DRenderDC(HWND hWnd, int nWidth, int nHeight)
{
	g_D2DGlobalData.AddRef();

	m_pRenderTarget = NULL;
	m_hWnd = hWnd;

	if (NULL == g_D2DGlobalData.m_pD2DFactory)
		g_D2DGlobalData.CreateD2D();
	if (NULL != g_D2DGlobalData.m_pD2DFactory)
	{
		D2D1_SIZE_U size = D2D1::SizeU(nWidth, nHeight);

		// Create a Direct2D render target.
		HRESULT hr = g_D2DGlobalData.m_pD2DFactory->CreateHwndRenderTarget(
				D2D1::RenderTargetProperties(),
				D2D1::HwndRenderTargetProperties(m_hWnd, size),
				&m_pRenderTarget
			);
		UIASSERT(SUCCEEDED(hr));
	}
}
Direct2DRenderDC::~Direct2DRenderDC()
{
	SAFE_RELEASE(m_pRenderTarget);
	m_hWnd = NULL;

	g_D2DGlobalData.Release();
}

bool Direct2DRenderDC::BeginDraw(HDC hDC)
{
	if (NULL == m_pRenderTarget)
		return false;

	m_pRenderTarget->BeginDraw();
	return true;
}
void Direct2DRenderDC::EndDraw()
{
	m_pRenderTarget->EndDraw();
}

int Direct2DRenderDC::DrawString(const TCHAR* szText, const CRect* lpRect, UINT nFormat, IRenderFont* pFont, COLORREF col )
{
	if (NULL == m_pRenderTarget)
		return 0;

	if (NULL == pFont)
	{
		UI_LOG_WARN(_T("%s hRFont == NULL"), FUNC_NAME);
		return -1;
	}

	if (pFont->GetRenderType() != GRAPHICS_RENDER_TYPE_DIRECT2D)
	{
		UI_LOG_WARN(_T("%s hRFont render type != GRAPHICS_RENDER_TYPE_GDIPLUS"), FUNC_NAME);
		return -1;
	}

	IDWriteTextFormat* pTextFormat = ((Direct2DRenderFont*)pFont)->GetFont();
	if (NULL == pTextFormat)
	{
		UI_LOG_WARN(_T("%s NULL == pTextFormat"), FUNC_NAME);
		return -1;
	}

	ID2D1SolidColorBrush* pTextBrush = NULL;
	m_pRenderTarget->CreateSolidColorBrush(
			D2D1::ColorF(col),
			&pTextBrush
		);
	if (NULL == pTextBrush)
	{
		UI_LOG_WARN(_T("%s CreateSolidColorBrush failed."), FUNC_NAME);
		return -1;
	}

	if (nFormat & DT_CENTER)
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	else if (nFormat & DT_RIGHT)
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	else
		pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);

	if (nFormat & DT_VCENTER)
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	else if (nFormat & DT_BOTTOM)
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_FAR);
	else
		pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);


	m_pRenderTarget->DrawText(
		szText,
		_tcslen(szText),
		pTextFormat,
		D2D1::RectF(
			(FLOAT)lpRect->left, 
			(FLOAT)lpRect->top, 
			(FLOAT)lpRect->Width(), 
			(FLOAT)lpRect->Height()),
		pTextBrush
		);

	SAFE_RELEASE(pTextBrush);

	return 0;
}
void Direct2DRenderDC::DrawBitmap(IRenderBitmap* pBitmap, DRAWBITMAPPARAM* pParam)
{
	if (NULL == pBitmap || NULL == pParam)
		return;

	if (pBitmap->GetRenderType() != GRAPHICS_RENDER_TYPE_DIRECT2D)
		return;

	ID2D1Bitmap* pD2DBitmap = ((Direct2DRenderBitmap*)pBitmap)->GetBitmap(m_pRenderTarget);
	if (NULL == pD2DBitmap)
	{
		UI_LOG_WARN(_T("%s GetBitmap failed."), FUNC_NAME);
		return;
	}

	m_pRenderTarget->DrawBitmap(
		pD2DBitmap,
		D2D1::RectF(
			(FLOAT)pParam->xDest,
			(FLOAT)pParam->yDest,
			(FLOAT)pParam->xDest+pParam->wDest,
			(FLOAT)pParam->yDest+pParam->hDest)
		);
}
#pragma  endregion

// Direct2DMemRenderDC::Direct2DMemRenderDC(HWND hWnd, int nWidth, int nHeight)
// {
// 	
// }
// Direct2DMemRenderDC::~Direct2DMemRenderDC()
// {
// 
// }