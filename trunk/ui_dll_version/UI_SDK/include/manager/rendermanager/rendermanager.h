#pragma once

namespace UI
{


// HRDC �������л��ƺ���Я���ĵ�һ������
#define HRDC     IRenderTarget*  
#define HRBITMAP IRenderBitmap*
#define HRFONT   IRenderFont*
#define HRIMAGELISTBITMAP  IImageListRenderBitmap*

typedef enum 
{
	GRAPHICS_RENDER_TYPE_AUTO = 0,   // <- ���ݴ������;������ֲ㴰��ʹ��GDIPLUS,��ͨ����ʹ��GDI
	GRAPHICS_RENDER_TYPE_GDI,
	GRAPHICS_RENDER_TYPE_GDIPLUS,
	GRAPHICS_RENDER_TYPE_DIRECT2D
}
GRAPHICS_RENDER_TYPE;

// ����ͼƬ��ͳһ������������Ҫ��д���DrawBitmap����
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
	int    wDest;  // Ŀ����Ƶķ�Χ��������Ҫ����ʱ����ʹ��
	int    hDest;  // Ŀ����Ƶķ�Χ��������Ҫ����ʱ����ʹ��
	int    xSrc;
	int    ySrc;
	int    wSrc;
	int    hSrc;
	Image9Region* pRegion;   // ����Ҫ����ʱ����ʹ��

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
	IRenderResource**   m_ppOutRef;  // �ⲿ���ã����ڴ���������release�Զ��ͷ�
									 // Ϊ�˽����Ҫ��CPojo_ImageItem�б���һ��UIImage*ָ�룬Ҫ�ⲿGetʱ��ֵ������������UIImage*�ֲ��ܸ����������á�
									 // �������ⲿ��UIImage* Release������CPojo_ImageItem::UIImage*ȴ�޷��ÿգ�������������һ��m_pOutRef��ָ�����
									 // UIImage*����UIImage�����������н����ÿ�
};

class UIAPI IRenderBitmap : public IRenderResource
{
protected:
	IRenderBitmap(IRenderResource**  ppOutRef);   // ֻ��ͨ��Create������

public:

//  ��������ȥʵ��CreateInstance new self.
//  CreateInstance���� AddRef����Ҫ�ⲿ������Ҫ���� AddRef��Ϊ������ CPojo_ImageItem::m_pImage������Ref��ԭ��
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

	// ��memdc�����õ�
	virtual bool     BeginDraw(HDC hDC, RECT* prc, RECT* prc2=NULL, bool bClear=false) = 0;
	virtual void     EndDraw() = 0;
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ) = 0;
	virtual BYTE*    LockBits() = 0;
	virtual void     UnlockBits() = 0;
	virtual void     Clear() = 0;
	virtual void     Save( const String& strPath ) = 0;
	virtual HBITMAP  CopyRect(RECT *prc) = 0;

protected:
	HWND    m_hWnd;   // ����ReleaseDC(m_hWnd, m_hDC);
};


//////////////////////////////////////////////////////////////////////////

class RenderOffsetClipHelper
{
public:
	RenderOffsetClipHelper(RECT* pInitRect);

	// ����falseʱ����ʾ�����������ȫ������ǰ���������ˣ�����Ҫ�ٻ���
	bool  DrawChild(Object* pChild, IRenderTarget* pRenderTarget);
	void  Scroll(IRenderTarget* pRenderTarget, Object* pObjScroll, bool bUpdate);
	void  DrawClient(IRenderTarget* pRenderTarget, Object* pObject, bool bUpdate);
	void  Update(IRenderTarget* pRenderTarget);
	void  Reset(IRenderTarget* pRenderTarget);

public:
	bool      m_bUpdateClip;   // �Ƿ���Ҫά��CLIP����

	CRect     m_rcClip;    // �����豸���꣬���������Ҫ���ƵĶ�������Ͻǣ����ļ�������
	POINT     m_ptOffset;  // ��ǰҪ���ƵĶ���ƫ��������ʵ�����豸����
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

// ��ȡHRDC��Ӧ��һ��HDC����Image���ƣ�GetHDC������ReleaseHDC����ʹ��
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