#ifndef GDIPEN_H_5B58777E_D320_4d25_AB38_8E4BB216999B
#define GDIPEN_H_5B58777E_D320_4d25_AB38_8E4BB216999B

#include "UISDK\Kernel\Src\Renderlibrary\renderresourceimpl.h"

namespace UI
{
class GdiPen : public IRenderResourceImpl<IRenderPen>
{
protected:
	GdiPen();
public:
	~GdiPen();
	static   void  CreateInstance(IRenderPen** ppOutRef);

public:
	virtual  GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }
	virtual  bool  CreateSolidPen(int nWidth, Color* pColor);
	virtual  bool  CreateDotPen(int nWidth, Color* pColor);

protected:
	void   DeleteObject();

public:
	HPEN   m_hPen;
};
}

#endif // GDIPEN_H_5B58777E_D320_4d25_AB38_8E4BB216999B