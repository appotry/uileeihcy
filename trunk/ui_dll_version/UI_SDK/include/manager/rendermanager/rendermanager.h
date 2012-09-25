#pragma once

namespace UI
{


// HRDC 将是所有绘制函数携带的第一个参数
#define HRDC     IRenderDC*  
#define HRBITMAP IRenderBitmap*
#define HRFONT   IRenderFont*
#define HRIMAGELISTBITMAP  IImageListRenderBitmap*

typedef enum 
{
	GRAPHICS_RENDER_TYPE_AUTO = 0,   // <- 依据窗口类型决定。分层窗口使用GDIPLUS,普通窗口使用GDI
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
	IRenderResource**   m_ppOutRef;  // 外部引用，用于创建单例和release自动释放
									 // 为了解决需要在CPojo_ImageItem中保存一份UIImage*指针，要外部Get时赋值，但这个保存的UIImage*又不能给它计算引用。
									 // 当所有外部的UIImage* Release结束后，CPojo_ImageItem::UIImage*却无法置空，因此在这里添加一个m_pOutRef，指向这个
									 // UIImage*，在UIImage的析构函数中将中置空
};

class IRenderBitmap : public IRenderResource
{
protected:
	IRenderBitmap(IRenderResource**  ppOutRef);   // 只能通过Create来创建

public:

//  各个子类去实现CreateInstance new self.
//  CreateInstance不做 AddRef，需要外部根据需要自行 AddRef（为了满足 CPojo_ImageItem::m_pImage不计算Ref的原因）
//	static void  CreateInstance( IRenderBitmap** pOutRef );

public:
	virtual bool  LoadFromFile( const String& strPath ) = 0;
	virtual int   GetWidth() = 0;
	virtual int   GetHeight() = 0;

	virtual BYTE* LockBits() = 0;
	virtual void  UnlockBits() = 0;

	virtual bool  SaveBits( ImageData* pImageData ) = 0;
	virtual bool  ChangeHLS( const ImageData* pOriginImageData, short h, short l, short s, int nFlag ) = 0;
	
	virtual void  SetAttribute( const ATTRMAP& mapAttrib ) = 0;
};

class IImageListRenderBitmap : public IRenderBitmap
{
public:
	IImageListRenderBitmap(IRenderResource**  ppOutRef);

	virtual int  GetItemWidth() = 0;
	virtual int  GetItemHeight() = 0;
	virtual IMAGELIST_LAYOUT_TYPE GetLayoutType() = 0;
	virtual bool GetIndexPos(int nIndex, POINT* pPoint) = 0;
};

class RenderBitmapFactory
{
public:
	static void CreateInstance(IRenderBitmap** ppOut, GRAPHICS_RENDER_TYPE eGraphicsRenderType, const ATTRMAP& mapAttrib);
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
	
	virtual bool     BeginDraw( HDC hDC ) = 0;
	virtual void     EndDraw( ) = 0;
	virtual void     EndDraw( int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish ) = 0;
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
	virtual void     DrawBitmap( IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)=0;
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
	HWND    m_hWnd;   // 用于ReleaseDC(m_hWnd, m_hDC);
};


//////////////////////////////////////////////////////////////////////////

// enum RENDER_MGR_POLICY
// {
// 	RENDER_MGR_POLICY_AUTO,            // 正常下使用GDI，在分层窗口下面使用GDIPLUS
// 	RENDER_MGR_POLICY_GDI,             // 强制全部使用GDI
// 	RENDER_MGR_POLICY_GDIPLUS          // 强制全部使用GDIPLUS
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

	// 返回false时，表示这个对象已完全超出当前剪裁区域了，不需要再绘制
	bool  DrawChild(Object* pChild, HRDC hRDC);
	void  Scroll(HRDC hRDC, Object* pObjScroll, bool bUpdate);
	void  DrawClient(HRDC hRDC, Object* pObject, bool bUpdate);
	void  Update(HRDC hRDC);
	void  Reset(HRDC hRDC);

public:
	CRect     m_rcClip;    // 不是设备坐标，而是相对于要绘制的对象的左上角，它的剪裁区域
	POINT     m_ptOffset;  // 当前要绘制的对象偏移量，其实就是设备坐标
};

}
UIAPI GRAPHICS_RENDER_TYPE GetGraphicsRenderType(Object* pObj);
UIAPI GRAPHICS_RENDER_TYPE GetGraphicsRenderType(HWND hWnd);

UIAPI HRDC     GetHRDC(HDC hDC, HWND hWnd);
UIAPI HRDC     GetHRDC(HWND hWnd);
UIAPI void     ReleaseHRDC(HRDC hRDC);
UIAPI HRDC     CreateCompatibleHRDC( HRDC hRDC, int nWidth, int nHeight );
UIAPI HRDC     CreateRenderTarget( HWND hWnd, int nWidth, int nHeight );
UIAPI bool     BeginDraw(HRDC hRDC, HDC hDC);
UIAPI void     EndDraw( HRDC hRDC );
UIAPI void     EndDraw( HRDC hRDC, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish );
UIAPI void     ResizeRenderTarget( HRDC hRDC, int nWidth, int nHeight );

// 获取HRDC对应的一个HDC用于Image绘制，GetHDC必须与ReleaseHDC配套使用
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