#include "stdafx.h"
#include "direct2drender.h"

#ifdef UI_D2D_REDNER
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
Direct2DRenderFont::Direct2DRenderFont()
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

void  Direct2DRenderFont::CreateInstance(IRenderFont** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return;

	Direct2DRenderFont* p = new Direct2DRenderFont();
    p->AddRef();
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
	if (false == bRet)
	{
		UI_LOG_WARN(_T("%s Load failed."), FUNC_NAME);
	}
	else
	{
		__super::NotifyListener();
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
	if (m_pTextFormat)
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
// Direct2DRenderTarget::Direct2DRenderTarget()
// {
// 	g_D2DGlobalData.AddRef();
// 	m_hWnd = NULL;
// 	m_pRenderTarget = NULL;
// 	UIASSERT(0);
// }
// Direct2DRenderTarget::Direct2DRenderTarget(HDC hDC)
// {
// 	g_D2DGlobalData.AddRef();
// 	m_hWnd = NULL;
// 	m_pRenderTarget = NULL;
// 	UIASSERT(0);
// }
Direct2DRenderTarget::Direct2DRenderTarget(HWND hWnd):IRenderTarget(hWnd)
{
	g_D2DGlobalData.AddRef();
	m_pRenderTarget = NULL;
}
// Direct2DRenderTarget::Direct2DRenderTarget(HWND hWnd, int nWidth, int nHeight)
// {
// 	g_D2DGlobalData.AddRef();
// 
// 	m_pRenderTarget = NULL;
// 	m_hWnd = hWnd;
// }
Direct2DRenderTarget::~Direct2DRenderTarget()
{
	SAFE_RELEASE(m_pRenderTarget);
	m_hWnd = NULL;

	g_D2DGlobalData.Release();
}

#include <comdef.h>
bool  Direct2DRenderTarget::BeginDraw(HDC hDC, RECT* prcArray, int rcCount, bool bClear)
{
	if (m_pRenderTarget)
	{
		return false;
	}

	if (NULL == g_D2DGlobalData.m_pD2DFactory)
		g_D2DGlobalData.CreateD2D();
	if (NULL == g_D2DGlobalData.m_pD2DFactory)
		return false;

	// Create a DC render target.
	D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties(
		D2D1_RENDER_TARGET_TYPE_DEFAULT,
		D2D1::PixelFormat(
		DXGI_FORMAT_B8G8R8A8_UNORM,
		D2D1_ALPHA_MODE_IGNORE/*D2D1_ALPHA_MODE_PREMULTIPLIED*/),  // 使用预乘模式，启用alpha channel
		0,
		0,
		D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE,
		D2D1_FEATURE_LEVEL_DEFAULT
		);

	HRESULT hr = g_D2DGlobalData.m_pD2DFactory->CreateDCRenderTarget(&props,&m_pRenderTarget);
	UIASSERT(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	RECT rc;
	::GetClientRect(m_hWnd, &rc);
	m_pRenderTarget->BindDC(hDC, &rc);

	m_pRenderTarget->BeginDraw();
	if (bClear)
	{
		D2D1_COLOR_F color = {0};
		m_pRenderTarget->Clear(color);
	}
	return true;
}
void Direct2DRenderTarget::EndDraw()
{
	m_pRenderTarget->EndDraw();
	SAFE_RELEASE(m_pRenderTarget);
}

void Direct2DRenderTarget::EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish )
{
	if (!bFinish)
		return;

	m_pRenderTarget->EndDraw();
	SAFE_RELEASE(m_pRenderTarget);
}
int Direct2DRenderTarget::DrawString(const TCHAR* szText, const CRect* lpRect, UINT nFormat, IRenderFont* pFont, COLORREF col )
{
	if (NULL == lpRect || lpRect->Width() <= 0 || lpRect->Height() <= 0)
		return 0;

	if (NULL == m_pRenderTarget)
		return 0;

	if (NULL == pFont)
	{
		UI_LOG_WARN(_T("%s hRFont == NULL"), FUNC_NAME);
		return -1;
	}

	if (pFont->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D)
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
void Direct2DRenderTarget::DrawBitmap(IRenderBitmap* pBitmap, DRAWBITMAPPARAM* pParam)
{
	if (NULL == pBitmap || NULL == pParam)
		return;

	if (pBitmap->GetGraphicsRenderLibraryType() != GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D)
		return;

	ID2D1Bitmap* pD2DBitmap = ((Direct2DRenderBitmap*)pBitmap)->GetBitmap(m_pRenderTarget);
	if (NULL == pD2DBitmap)
	{
		UI_LOG_WARN(_T("%s GetBitmap failed."), FUNC_NAME);
		return;
	}

	if (pParam->nFlag & DRAW_BITMAP_DISABLE)
	{
		UIASSERT(0);
	}

	if (pParam->nFlag & DRAW_BITMAP_BITBLT)
	{
		m_pRenderTarget->DrawBitmap(
			pD2DBitmap,
			D2D1::RectF(
				(FLOAT)pParam->xDest,
				(FLOAT)pParam->yDest,
				(FLOAT)pParam->xDest+pParam->wDest,
				(FLOAT)pParam->yDest+pParam->hDest)
			);
	}
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH)
	{
		m_pRenderTarget->DrawBitmap(
			pD2DBitmap,
			D2D1::RectF(
				(FLOAT)pParam->xDest,
				(FLOAT)pParam->yDest,
				(FLOAT)(pParam->xDest+pParam->wDest),
				(FLOAT)(pParam->yDest+pParam->hDest)),
			1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
			D2D1::RectF(
				(FLOAT)pParam->xSrc,
				(FLOAT)pParam->ySrc,
				(FLOAT)(pParam->xSrc+pParam->wSrc),
				(FLOAT)(pParam->ySrc+pParam->hSrc)));

//		UIASSERT(NULL == pParam->pRegion);
	}
	else if (pParam->nFlag & DRAW_BITMAP_STRETCH_BORDER)
	{

	}
	else if (pParam->nFlag & DRAW_BITMAP_TILE)
	{
		UIASSERT(0);
	}
	else if (pParam->nFlag & DRAW_BITMAP_CENTER)
	{
		UIASSERT(0);
	}
	else if (pParam->nFlag & DRAW_BITMAP_ADAPT)
	{
		UIASSERT(0);
	}
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

#endif