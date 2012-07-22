#pragma once

namespace UI
{


// HRDC �������л��ƺ���Я���ĵ�һ������
#define HRDC     IRenderDC*  
#define HRBITMAP IRenderBitmap*
#define HRFONT   IRenderFont*

typedef enum 
{
	GRAPHICS_RENDER_TYPE_AUTO,   // <- ���ݴ������;������ֲ㴰��ʹ��GDIPLUS,��ͨ����ʹ��GDI
	GRAPHICS_RENDER_TYPE_GDI,
	GRAPHICS_RENDER_TYPE_GDIPLUS,
	GRAPHICS_RENDER_TYPE_DIRECT2D
}
GRAPHICS_RENDER_TYPE;


class IRenderResource
{
protected:
	IRenderResource(IRenderResource** ppOutRef);
public:
	virtual ~IRenderResource();
	virtual GRAPHICS_RENDER_TYPE GetRenderType() = 0;

	long   AddRef();
	long   Release();

protected:
	long       m_dwRef;
	IRenderResource**   m_ppOutRef;  // �ⲿ���ã����ڴ���������release�Զ��ͷ�
									 // Ϊ�˽����Ҫ��CPojo_ImageItem�б���һ��UIImage*ָ�룬Ҫ�ⲿGetʱ��ֵ������������UIImage*�ֲ��ܸ����������á�
									 // �������ⲿ��UIImage* Release������CPojo_ImageItem::UIImage*ȴ�޷��ÿգ��������������һ��m_pOutRef��ָ�����
									 // UIImage*����UIImage�����������н����ÿ�
};
class IRenderBitmap : public IRenderResource
{
protected:
	IRenderBitmap(IRenderResource**  ppOutRef);   // ֻ��ͨ��Create������

public:

//  ��������ȥʵ��CreateInstance new self.
//  CreateInstance���� AddRef����Ҫ�ⲿ������Ҫ���� AddRef��Ϊ������ CPojo_ImageItem::m_pImage������Ref��ԭ��
//	static void  CreateInstance( IRenderBitmap** pOutRef );

public:
	virtual bool  LoadFromFile( const String& strPath ) = 0;
	virtual int   GetWidth() = 0;
	virtual int   GetHeight() = 0;

	virtual BYTE* LockBits() = 0;
	virtual void  UnlockBits() = 0;

	virtual bool  SaveBits( ImageData* pImageData ) = 0;
	virtual bool  ChangeHue( const ImageData* pOriginImageData, WORD wNewHue ) = 0;
};

class  IRenderFont : public IRenderResource
{
protected:
	IRenderFont(IRenderResource** ppOutRef);

public:
	virtual bool  Load( LOGFONT* plogfont ) = 0;
	virtual bool  ModifyFont(LOGFONT* plogfont) = 0;

	virtual void  Attach(HFONT hFont) = 0;
	virtual HFONT Detach() = 0;

	virtual UINT  GetTextMetricsHeight( ) = 0;
	virtual SIZE  MeasureString( const TCHAR* szText, int nLimitWidth = -1 ) = 0;

	virtual HFONT GetHFONT() = 0;
};


class IRenderDC
{
public:
	IRenderDC();
	IRenderDC(HDC hDC);
	IRenderDC(HWND hWnd);
	virtual ~IRenderDC() =0 {};
	virtual GRAPHICS_RENDER_TYPE GetRenderType() = 0;

	virtual HRDC     CreateCompatibleHRDC( int nWidth, int nHeight ) = 0;
	virtual HDC      GetHDC() = 0;
	virtual void     ReleaseHDC( HDC hDC ) = 0;

// 	virtual HRFONT   SelectFont( HRFONT hFont ) = 0;
// 	virtual HRFONT   GetFont() = 0;
	virtual HRGN     GetClipRgn() = 0;
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY ) = 0;
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint ) = 0;
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) = 0;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) = 0;
// 	virtual COLORREF SetTextColor( COLORREF color, byte Alpha = 255 ) = 0;
// 	virtual COLORREF GetTextColor( ) = 0;
	
	virtual void     BeginDraw( HDC hDC ) = 0;
	virtual void     EndDraw( ) = 0;
	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc ) = 0;
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ) = 0;
	virtual BYTE*    LockBits() = 0;
	virtual void     UnlockBits() = 0;
	virtual void     Save( const String& strPath ) = 0;

	virtual int      DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col ) = 0;
	virtual void     FillRgn( HRGN hRgn, COLORREF col ) = 0;
	virtual void     FillRect( const CRect* lprc, COLORREF col) = 0;
	virtual void     TileRect( const CRect* lprc, HRBITMAP hBitmap ) = 0;
	virtual void     Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack ) = 0;
	virtual void     DrawFocusRect( const CRect* lprc ) = 0;
	virtual void     GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo ) = 0;
	virtual void     GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo ) = 0;
	virtual void     BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderDC* pSrcHDC, int xSrc, int ySrc, DWORD dwRop ) = 0;
	virtual void     DrawBitmap( HRBITMAP hBitmap, int x, int y) = 0;
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
		                        int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) = 0;
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
								Image9Region* p9Region ) = 0;
	virtual void     ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//  bitmap

	virtual bool     LoadBitmapFromFile( String strPath ){return false;};
	virtual bool     LoadBitmapFromResource(){return false;};
	virtual bool     LoadBitmapFromZip() {return false;};

protected:
	HWND    m_hWnd;   // ����ReleaseDC(m_hWnd, m_hDC);
};


//////////////////////////////////////////////////////////////////////////

// enum RENDER_MGR_POLICY
// {
// 	RENDER_MGR_POLICY_AUTO,            // ������ʹ��GDI���ڷֲ㴰������ʹ��GDIPLUS
// 	RENDER_MGR_POLICY_GDI,             // ǿ��ȫ��ʹ��GDI
// 	RENDER_MGR_POLICY_GDIPLUS          // ǿ��ȫ��ʹ��GDIPLUS
// };
class RenderManager
{
public:
	RenderManager();
	~RenderManager();

	HRDC GetHRDC( HWND hWnd );
	HRDC GetHRDC( HDC hDC, HWND hWnd );
	void ReleaseHRDC( HRDC hRDC );
	HRDC CreateRenderTarget( HWND hWnd, int nWidth, int nHeight );

private:
};

class RenderOffsetClipHelper
{
public:
	RenderOffsetClipHelper(Object* pObject);

	// ����falseʱ����ʾ�����������ȫ������ǰ���������ˣ�����Ҫ�ٻ���
	bool  DrawChild(Object* pChild, HRDC hRDC);
	void  Scroll(HRDC hRDC, Object* pObjScroll, bool bUpdate);
	void  DrawClient(HRDC hRDC, Object* pObject, bool bUpdate);
	void  Update(HRDC hRDC);
	void  Reset(HRDC hRDC);

public:
	CRect     m_rcClip;    // �����豸���꣬���������Ҫ���ƵĶ�������Ͻǣ����ļ�������
	POINT     m_ptOffset;  // ��ǰҪ���ƵĶ���ƫ��������ʵ�����豸����
};

}
UIAPI GRAPHICS_RENDER_TYPE GetGraphicsRenderType(Object* pObj);
UIAPI GRAPHICS_RENDER_TYPE GetGraphicsRenderType(HWND hWnd);

UIAPI HRDC     GetHRDC(HDC hDC, HWND hWnd);
UIAPI HRDC     GetHRDC(HWND hWnd);
UIAPI void     ReleaseHRDC(HRDC hRDC);
UIAPI HRDC     CreateCompatibleHRDC( HRDC hRDC, int nWidth, int nHeight );
UIAPI HRDC     CreateRenderTarget( HWND hWnd, int nWidth, int nHeight );
UIAPI void     BeginDraw(HRDC hRDC, HDC hDC);
UIAPI void     EndDraw( HRDC hRDC );
UIAPI void     EndDraw( HRDC hRDC, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc );
UIAPI void     ResizeRenderTarget( HRDC hRDC, int nWidth, int nHeight );

// ��ȡHRDC��Ӧ��һ��HDC����Image���ƣ�GetHDC������ReleaseHDC����ʹ��
UIAPI HDC      GetHDC(HRDC hRDC);
UIAPI void     ReleaseHDC( HRDC hRDC, HDC hDC );

// UIAPI HRFONT   SelectFont( HRDC hRDC, HRFONT hFont );
// UIAPI HRFONT   GetFont( HRDC hRDC );
UIAPI HRGN     GetClipRgn( HRDC hRDC );
UIAPI int      SelectClipRgn( HRDC hRDC, HRGN hRgn, int nMode = RGN_COPY );
UIAPI BOOL     GetViewportOrgEx( HRDC hRDC, LPPOINT lpPoint );
UIAPI BOOL     SetViewportOrgEx( HRDC hRDC, int x, int y, LPPOINT lpPoint );
UIAPI BOOL     OffsetViewportOrgEx( HRDC hRDC, int x, int y, LPPOINT lpPoint );
// UIAPI COLORREF SetTextColor( HRDC hRDC, COLORREF color, byte Alpha=255 );
// UIAPI COLORREF GetTextColor( HRDC hRDC );
UIAPI UINT     GetTextMetricsHeight( HRFONT hRFont );
UIAPI SIZE     MeasureString( HRFONT hRFont, const TCHAR* szText, int nLimitWidth = -1 );
UIAPI BYTE*    LockBits( HRDC hRDC );
UIAPI void     UnlockBits( HRDC hRDC );
UIAPI void     Save( HRDC hRDC, const String& strPath );

UIAPI int      DrawString( HRDC hRDC, const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col= 0 );
UIAPI void     FillRgn( HRDC hRDC, HRGN hRgn, COLORREF col );
UIAPI void     FillRect( HRDC hRDC, const CRect* lprc, COLORREF col);
UIAPI void     TileRect( HRDC hRDC, const CRect* lprc, HRBITMAP hBitmap );
UIAPI void     Rectangle( HRDC hRDC, const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder=1, bool bNullBack=false );
UIAPI void     DrawFocusRect( HRDC hRDC, const CRect* lprc );
UIAPI void     GradientFillH( HRDC hRDC, const CRect* lprc, COLORREF colFrom, COLORREF colTo );
UIAPI void     GradientFillV( HRDC hRDC, const CRect* lprc, COLORREF colFrom, COLORREF colTo );
UIAPI void     BitBlt( HRDC hRDC, int xDest, int yDest, int wDest, int hDest, HRDC hrdcSrc, int xSrc, int ySrc, DWORD dwRop );
UIAPI void     DrawBitmap( HRDC hRDC, HRBITMAP hBitmap, int x, int y );
UIAPI void     DrawBitmap( HRDC hRDC, HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
						 int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight );
UIAPI void     DrawBitmap( HRDC hRDC, HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
						  int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
						  Image9Region* p9Region );
UIAPI void     ImageList_Draw(HRDC hRDC, HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy );