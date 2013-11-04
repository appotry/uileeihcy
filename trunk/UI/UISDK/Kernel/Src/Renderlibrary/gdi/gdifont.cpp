#include "stdafx.h"
#include "gdifont.h"

#include "UISDK\Kernel\Src\atl\image.h"

GDIRenderFont::GDIRenderFont()
{
	m_hFont = NULL;
	m_bCreateOrAttach = true;
}
GDIRenderFont::~GDIRenderFont()
{
	UI_LOG_DEBUG(_T("GDIRenderFont Delete. ptr=0x%08X"), this);
	this->DestroyFont();
}
//
//	根据FONT的创建方式来释放字体
//
void  GDIRenderFont::DestroyFont()
{
	if(m_bCreateOrAttach)
	{
		SAFE_DELETE_GDIOBJECT(m_hFont);
	}
	m_hFont = NULL;
	m_bCreateOrAttach = true;
}
void  GDIRenderFont::CreateInstance(IRenderFont** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return;

	GDIRenderFont* p = new GDIRenderFont();
    p->AddRef();
	*ppOutRef = p;
}

bool GDIRenderFont::Load( LOGFONT* plogfont )
{
	if (NULL == plogfont)
	{
		UI_LOG_WARN(_T("%s plogfont == NULL"), FUNC_NAME);
		return false;
	}

	this->DestroyFont();
	m_hFont = ::CreateFontIndirect(plogfont);
	if (NULL == m_hFont)
	{
		UI_LOG_WARN(_T("%s CreateFontIndirect failed, facename=%s"), FUNC_NAME, plogfont->lfFaceName );
		return false;
	}
	m_bCreateOrAttach = true;
	return true;
}
bool GDIRenderFont::ModifyFont(LOGFONT* plogfont)
{
	bool bRet = this->Load(plogfont);
	if (false == bRet)
	{
		UI_LOG_WARN(_T("%s Load failed."), FUNC_NAME);
	}
	else
	{
	//	__super::NotifyListener();
	}
	return bRet;
}

void  GDIRenderFont::Attach(HFONT hFont)
{
	this->DestroyFont();
	m_hFont = hFont;
	m_bCreateOrAttach = false;
}
HFONT GDIRenderFont::Detach()
{
	HFONT hSave = m_hFont;
	m_hFont = NULL;
	m_bCreateOrAttach = true;

	return hSave;
}
SIZE GDIRenderFont::MeasureString( const TCHAR* szText, int nLimitWidth)
{
	CSize sizeText(0, 0);

	HDC hDC = Image::GetCacheDC();
	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_hFont);

	if (-1 == nLimitWidth && NULL == _tcsrchr(szText,_T('\n')))
	{
		::GetTextExtentPoint32(hDC, szText, _tcslen(szText), &sizeText);
	}
	else
	{
		RECT rcText = {0,0,0,0};
		if (-1 != nLimitWidth )
		{
			rcText.right = nLimitWidth;
		}
		::DrawText(hDC, szText, _tcslen(szText), &rcText, DT_CALCRECT|DT_EDITCONTROL|DT_WORDBREAK);
		sizeText.cx = rcText.right;
		sizeText.cy = rcText.bottom;
	}
	::SelectObject(hDC, hOldFont);
	Image::ReleaseCacheDC(hDC);

	return sizeText;
}
UINT GDIRenderFont::GetTextMetricsHeight()
{
	HDC hDC = Image::GetCacheDC();
	HFONT hOldFont = (HFONT)::SelectObject(hDC, m_hFont);

	TEXTMETRIC  t;
	GetTextMetrics( hDC, &t );

	::SelectObject(hDC, hOldFont);
	Image::ReleaseCacheDC(hDC);
	return t.tmHeight;
}

HFONT GDIRenderFont::GetHFONT()
{
	return m_hFont;
}

bool GDIRenderFont::GetLogFont(LOGFONT* plf)
{
	if (NULL == plf || NULL == m_hFont)
		return false;

	GetObject(m_hFont, sizeof(LOGFONT), plf);
	return true;
}