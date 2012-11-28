#pragma once

namespace UI
{


// HRDC 将是所有绘制函数携带的第一个参数
#define HRDC     IRenderTarget*  
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

// 绘制图片的统一参数，避免需要重写多个DrawBitmap函数
enum DRAW_BITMAP_FLAG{
	DRAW_BITMAP_DISABLE = 0x0001,

	DRAW_BITMAP_BITBLT  = 0x00010000,
	DRAW_BITMAP_STRETCH = 0x00020000,
	DRAW_BITMAP_TILE    = 0x00040000,
	DRAW_BITMAP_CENTER  = 0x00080000,
	DRAW_BITMAP_ADAPT   = 0x00100000,
};
typedef struct tagDRAWBITMAPPARAM
{
	tagDRAWBITMAPPARAM() { memset(this, 0, sizeof(tagDRAWBITMAPPARAM));}

	int    nFlag;

	int    xDest;
	int    yDest;
	int    wDest;  // 目标绘制的范围，当不需要拉伸时，不使用
	int    hDest;  // 目标绘制的范围，当不需要拉伸时，不使用
	int    xSrc;
	int    ySrc;
	int    wSrc;
	int    hSrc;
	Image9Region* pRegion;   // 不需要拉伸时，不使用

}DRAWBITMAPPARAM, *LPDRAWBITMAPPARAM;

class UIAPI IRenderResource
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

class UIAPI IRenderBitmap : public IRenderResource
{
protected:
	IRenderBitmap(IRenderResource**  ppOutRef);   // 只能通过Create来创建

public:

//  各个子类去实现CreateInstance new self.
//  CreateInstance不做 AddRef，需要外部根据需要自行 AddRef（为了满足 CPojo_ImageItem::m_pImage不计算Ref的原因）
//	static void  CreateInstance( IRenderBitmap** pOutRef );

public:
	virtual bool  LoadFromFile( const String& strPath, bool bCreateAlphaChannel, const ATTRMAP& mapAttrib ) = 0;
	virtual bool  Modify(const String& strFilePath, bool bCreateAlphaChannel) = 0;
	virtual bool  Create(int nWidth, int nHeight) = 0;
	virtual int   GetWidth() = 0;
	virtual int   GetHeight() = 0;
	virtual COLORREF GetAverageColor() { return 0; }

	virtual BYTE* LockBits() = 0;
	virtual void  UnlockBits() = 0;

	virtual bool  SaveBits( ImageData* pImageData ) = 0;
	virtual bool  ChangeHLS( const ImageData* pOriginImageData, short h, short l, short s, int nFlag ) = 0;
	
protected:
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
	static void CreateInstance(IRenderBitmap** ppOut, GRAPHICS_RENDER_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType);
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
	virtual bool  GetLogFont(LOGFONT* plf) = 0;
};


class IRenderTarget
{
public:
//	IRenderTarget();
//	IRenderTarget(HDC hDC);
	IRenderTarget(HWND hWnd);
	virtual ~IRenderTarget() =0 {};
	virtual void     Release();

	virtual GRAPHICS_RENDER_TYPE GetRenderType() = 0;

	virtual HDC      GetHDC() = 0;
	virtual void     ReleaseHDC( HDC hDC ) = 0;
	virtual HDC      GetOriginHDC() = 0;

	virtual HRGN     GetClipRgn() = 0;
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY ) = 0;
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint ) = 0;
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) = 0;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL ) = 0;
	
	virtual int      DrawString( const TCHAR* szText, const CRect* lpRect, UINT nFormat, HRFONT hRFont, COLORREF col ) = 0;
	virtual void     FillRgn( HRGN hRgn, COLORREF col ) = 0;
	virtual void     FillRect( const CRect* lprc, COLORREF col) = 0;
	virtual void     TileRect( const CRect* lprc, HRBITMAP hBitmap ) = 0;
	virtual void     Rectangle( const CRect* lprc, COLORREF colBorder, COLORREF colBack, int nBorder, bool bNullBack ) = 0;
	virtual void     DrawFocusRect( const CRect* lprc ) = 0;
	virtual void     GradientFillH( const CRect* lprc, COLORREF colFrom, COLORREF colTo ) = 0;
	virtual void     GradientFillV( const CRect* lprc, COLORREF colFrom, COLORREF colTo ) = 0;
	virtual void     BitBlt( int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop ) = 0;
	virtual void     DrawBitmap( HRBITMAP hBitmap, int x, int y) = 0;
	virtual void     DrawBitmap( IRenderBitmap* pBitmap, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc)=0;
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
		                        int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight ) = 0;
	virtual void     DrawBitmap( HRBITMAP hBitmap, int xDest, int yDest, int nDestWidth, 
								int nDestHeight, int xSrc, int ySrc, int nSrcWidth, int nSrcHeight,
								Image9Region* p9Region ) = 0;
	virtual void     ImageList_Draw( HRBITMAP hBitmap, int x, int y, int col, int row, int cx, int cy ) = 0;
	virtual void     DrawBitmap( HRBITMAP hBitmap, DRAWBITMAPPARAM* pParam ) = 0;

	//////////////////////////////////////////////////////////////////////////
	//  bitmap

	virtual bool     LoadBitmapFromFile( String strPath ){return false;};
	virtual bool     LoadBitmapFromResource(){return false;};
	virtual bool     LoadBitmapFromZip() {return false;};

	// 仅memdc才有用的
	virtual bool     BeginDraw(HDC hDC, RECT* prc, RECT* prc2=NULL, bool bClear=false) = 0;
	virtual void     EndDraw() = 0;
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ) = 0;
	virtual BYTE*    LockBits() = 0;
	virtual void     UnlockBits() = 0;
	virtual void     Clear() = 0;
	virtual void     Save( const String& strPath ) = 0;
	virtual HBITMAP  CopyRect(RECT *prc) = 0;

protected:
	HWND    m_hWnd;   // 用于ReleaseDC(m_hWnd, m_hDC);
};


//////////////////////////////////////////////////////////////////////////

class RenderOffsetClipHelper
{
public:
	RenderOffsetClipHelper(RECT* pInitRect);

	// 返回false时，表示这个对象已完全超出当前剪裁区域了，不需要再绘制
	bool  DrawChild(Object* pChild, IRenderTarget* pRenderTarget);
	void  Scroll(IRenderTarget* pRenderTarget, Object* pObjScroll, bool bUpdate);
	void  DrawClient(IRenderTarget* pRenderTarget, Object* pObject, bool bUpdate);
	void  Update(IRenderTarget* pRenderTarget);
	void  Reset(IRenderTarget* pRenderTarget);

public:
	bool      m_bUpdateClip;   // 是否需要维护CLIP参数

	CRect     m_rcClip;    // 不是设备坐标，而是相对于要绘制的对象的左上角，它的剪裁区域
	POINT     m_ptOffset;  // 当前要绘制的对象偏移量，其实就是设备坐标
};

}
UIAPI HRDC     GetHRDC(HDC hDC, HWND hWnd);
UIAPI HRDC     GetHRDC(HWND hWnd);
UIAPI void     ReleaseHRDC(HRDC hRDC);
// UIAPI HRDC     CreateRenderTarget( HWND hWnd, int nWidth, int nHeight );
// UIAPI bool     BeginDraw(HRDC hRDC, HDC hDC);
// UIAPI void     EndDraw( HRDC hRDC );
// UIAPI void     EndDraw( HRDC hRDC, int xDest, int yDest, int wDest, int hDest, int xSrc, int ySrc, bool bFinish );
UIAPI void     ResizeRenderTarget( HRDC hRDC, int nWidth, int nHeight );

// 获取HRDC对应的一个HDC用于Image绘制，GetHDC必须与ReleaseHDC配套使用
UIAPI HDC      GetHDC(HRDC hRDC);
UIAPI void     ReleaseHDC( HRDC hRDC, HDC hDC );

UIAPI HRGN     GetClipRgn( HRDC hRDC );
UIAPI int      SelectClipRgn( HRDC hRDC, HRGN hRgn, int nMode = RGN_COPY );
UIAPI BOOL     GetViewportOrgEx( HRDC hRDC, LPPOINT lpPoint );
UIAPI BOOL     SetViewportOrgEx( HRDC hRDC, int x, int y, LPPOINT lpPoint );
UIAPI BOOL     OffsetViewportOrgEx( HRDC hRDC, int x, int y, LPPOINT lpPoint );
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

//////////////////////////////////////////////////////////////////////////

UIAPI GRAPHICS_RENDER_TYPE GetGraphicsRenderType(Object* pObj);
UIAPI GRAPHICS_RENDER_TYPE GetGraphicsRenderType(HWND hWnd);

UIAPI IRenderTarget*   CreateRenderTarget(HWND hWnd);