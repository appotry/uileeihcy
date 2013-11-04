#ifndef GDIBRUSH_H_703CBE70_0F92_463d_B1FA_16E761E5BCCE
#define GDIBRUSH_H_703CBE70_0F92_463d_B1FA_16E761E5BCCE

#include "UISDK\Kernel\Src\Renderlibrary\renderresourceimpl.h"

namespace UI
{
class GdiBrush : public IRenderResourceImpl<IRenderBrush>
{
protected:
	GdiBrush();
public:
	~GdiBrush();
	static   void  CreateInstance(IRenderBrush** ppOutRef);

public:
	virtual  GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }
	virtual  bool  CreateSolidBrush(Color* pColor);

protected:
	void     DeleteObject();

public:
	HBRUSH   m_hBrush;
};
}

#endif  // GDIBRUSH_H_703CBE70_0F92_463d_B1FA_16E761E5BCCE