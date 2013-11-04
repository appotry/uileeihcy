#ifndef GDIFONT_H_E499B610_1CBA_48b0_8DDB_116BA61ED7DB
#define GDIFONT_H_E499B610_1CBA_48b0_8DDB_116BA61ED7DB

#include "UISDK\Kernel\Src\Renderlibrary\renderresourceimpl.h"

namespace UI
{

class  GDIRenderFont : public IRenderResourceImpl<IRenderFont>
{
protected:
	GDIRenderFont();

public:
	~GDIRenderFont();
	static  void  CreateInstance( IRenderFont** ppOutRef );
	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() { return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; }

public:
	virtual bool  Load( LOGFONT* plogfont );
	virtual bool  ModifyFont(LOGFONT* plogfont);

	virtual void  Attach(HFONT hFont);
	virtual HFONT Detach();

	virtual SIZE  MeasureString( const TCHAR* szText, int nLimitWidth = -1 );
	virtual UINT  GetTextMetricsHeight( );

	virtual HFONT GetHFONT();
	virtual bool  GetLogFont(LOGFONT* plf);

protected:
	void    DestroyFont();

private:
	HFONT   m_hFont;
	bool    m_bCreateOrAttach;  // 标明该m_hFont的来源，用于区分如何释放
};
}
#endif // GDIFONT_H_E499B610_1CBA_48b0_8DDB_116BA61ED7DB