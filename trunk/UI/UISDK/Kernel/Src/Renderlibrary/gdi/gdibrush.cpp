#include "stdafx.h"
#include "gdibrush.h"

GdiBrush::GdiBrush()
{
	m_hBrush = NULL;
}
GdiBrush::~GdiBrush()
{
	DeleteObject();
}

void  GdiBrush::CreateInstance(IRenderBrush** ppOutRef)
{
	UIASSERT(NULL != ppOutRef);
	if (NULL == ppOutRef)
		return;

	GdiBrush* p = new GdiBrush();
	*ppOutRef = static_cast<IRenderBrush*>(p);
}

void GdiBrush::DeleteObject()
{
	SAFE_DELETE_GDIOBJECT(m_hBrush);
}

bool GdiBrush::CreateSolidBrush(Color* pColor)
{
	if (NULL == pColor)
		return false;
	
	DeleteObject();

	m_hBrush = ::CreateSolidBrush(RGB(pColor->r, pColor->g, pColor->b));
	return m_hBrush!=NULL;
}