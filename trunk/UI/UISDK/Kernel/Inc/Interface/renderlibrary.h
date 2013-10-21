#ifndef RENDERLIBRARY_H_599A2143_1D66_44b0_9A19_DE010366346F
#define RENDERLIBRARY_H_599A2143_1D66_44b0_9A19_DE010366346F

namespace UI
{
interface IRenderFont;
interface IRenderPen;
class IRenderTarget;
class ImageData;
interface IRenderFont;
    
    
// ����ͼƬ��ͳһ������������Ҫ��д���DrawBitmap����
enum DRAW_BITMAP_FLAG{
    // ״̬λ
	DRAW_BITMAP_DISABLE = 0x0001,

    // ����λ
	DRAW_BITMAP_BITBLT  = 0x00010000,
	DRAW_BITMAP_STRETCH = 0x00020000,
	DRAW_BITMAP_TILE    = 0x00040000,
	DRAW_BITMAP_CENTER  = 0x00080000,
	DRAW_BITMAP_ADAPT   = 0x00100000,
	DRAW_BITMAP_STRETCH_BORDER     = 0x00200000,  // ֻ����߿򣬲������м�
    DRAW_BITMAP_BITBLT_RIGHTTOP    = 0x00400000,  // ��ͼƬ���������ϽǶ���
    DRAW_BITMAP_BITBLT_LEFTBOTTOM  = 0x00800000,  // ��ͼƬ���������½Ƕ���
    DRAW_BITMAP_BITBLT_RIGHTBOTTOM = 0x01000000,  // ��ͼƬ���������½Ƕ���
    DRAW_BITMAP_STRETCH_ROUNDCORNER = 0x02000000, // Բ������
};


typedef struct tagDRAWBITMAPPARAM
{
	tagDRAWBITMAPPARAM() { memset(this, 0, sizeof(tagDRAWBITMAPPARAM)); nAlpha = 255; nFlag = DRAW_BITMAP_BITBLT;}

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
	byte   nAlpha;

    // out param
    CRect*  prcRealDraw;   // ͼƬ�������Ƶ����򡣵�prcRealDraw��Ϊ��ʱ��ʾ��Ҫ��ȡ

}DRAWBITMAPPARAM, *LPDRAWBITMAPPARAM;

typedef struct tagDRAWTEXTPARAM
{
    tagDRAWTEXTPARAM() {
        nFormatFlag = 0;
        nEffectFlag = 0;
        color.a = 255;
        szText = NULL;
        prc = NULL;
        bkcolor.a = 255;
        wParam = lParam = 0;
    }

    int  nFormatFlag;    // �����־��ͬDrawText flag
    int  nEffectFlag;    // ��Ч��־

    Color  color;
    const TCHAR*  szText;
    const CRect*  prc;

    // ��Ч�п��ܻ�ʹ�õ��Ĳ���
    Color   bkcolor;
    WPARAM  wParam;  
    LPARAM  lParam; 
}DRAWTEXTPARAM, *LPDRAWTEXTPARAM;

enum TEXT_EFFECT
{
    TEXT_EFFECT_NONE = 0,
    TEXT_EFFECT_HALO = 1,   // ��ȦЧ�� bkcolorָ��������ɫ��wParam��˹ģ���뾶ϵͳ
};

interface IRenderResource
{
    virtual ~IRenderResource() = 0 {};
    virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() = 0;
    virtual void  SetOutRef(IRenderResource** ppOutRef) = 0;
    virtual long  AddRef() = 0;
    virtual long  Release() = 0;
};

interface IRenderBitmap : public IRenderResource
{

//  ��������ȥʵ��CreateInstance new self.
//  CreateInstance���� AddRef����Ҫ�ⲿ������Ҫ���� AddRef��Ϊ������ CPojo_ImageItem::m_pImage������Ref��ԭ��
//	static void  CreateInstance( IRenderBitmap** pOutRef );

	virtual bool  LoadFromFile(const TCHAR* szPath, bool bCreateAlphaChannel) = 0;
    virtual bool  LoadFromData(byte* pData, int nSize, bool bCreateAlphaChannel) = 0;
	virtual bool  Create(int nWidth, int nHeight) = 0;
	virtual int   GetWidth() = 0;
	virtual int   GetHeight() = 0;
	virtual int   GetBPP() = 0;
	virtual COLORREF GetAverageColor() { return 0; }
    virtual void  Attach(HBITMAP hBitmap, bool bDelete) {};
    virtual HBITMAP  Detach() { return NULL; }

	virtual BYTE* LockBits() = 0;
	virtual void  UnlockBits() = 0;

	virtual bool  SaveBits(ImageData* pImageData) = 0;
	virtual bool  ChangeHLS(const ImageData* pOriginImageData, short h, short l, short s, int nFlag) = 0;
	virtual IMAGE_ITEM_TYPE  GetImageType() = 0;

	virtual void  SetAttribute(IMapAttribute* pMapAttrib) = 0;
};

interface IImageListRenderBitmap : public IRenderBitmap
{
public:
	virtual int  GetItemWidth() = 0;
	virtual int  GetItemHeight() = 0;
	virtual int  GetItemCount() = 0;
	virtual IMAGELIST_LAYOUT_TYPE GetLayoutType() = 0;
	virtual bool GetIndexPos(int nIndex, POINT* pPoint) = 0;
};

class RenderBitmapFactory
{
public:
	static void CreateInstance(GRAPHICS_RENDER_LIBRARY_TYPE eGraphicsRenderType, IMAGE_ITEM_TYPE eType, IRenderBitmap** ppOut);
};

#define FONTITEM_FLAG_UNDERLINE 0x01
#define FONTITEM_FLAG_BOLD      0x02
#define	FONTITEM_FLAG_ITALIC    0x04
#define FONTITEM_FLAG_STRIKEOUT 0X08

interface IRenderFont : public IRenderResource
{
public:
	virtual bool  Load( LOGFONT* plogfont ) = 0;
	virtual bool  ModifyFont(LOGFONT* plogfont) = 0;

	virtual void  Attach(HFONT hFont) = 0;
	virtual HFONT Detach() = 0;

	virtual UINT  GetTextMetricsHeight( ) = 0;
	virtual SIZE  MeasureString( const TCHAR* szText, int nLimitWidth = -1 ) = 0;

	virtual HFONT GetHFONT() = 0;
	virtual bool  GetLogFont(LOGFONT* plf) = 0;

// 	virtual void  AddModifyListener(Object* p) = 0;
// 	virtual void  RemoveModifyListener(Object* p) = 0;
// 	virtual void  NotifyListener() = 0;
};

// class IRenderFontListenerImpl : public IRenderFont
// {
// public:
// 	virtual ~IRenderFontListenerImpl();
// 
// 	virtual void  AddModifyListener(Object* p);
// 	virtual void  RemoveModifyListener(Object* p);
// 	virtual void  NotifyListener();
// 
// protected:
// 	list<Object*>  m_listFontModifyListener;  // �����޸ĵļ�����
// };

interface IRenderPen : public IRenderResource
{
public:
	virtual  bool  CreateSolidPen(int nWidth, Color* pColor) = 0;
	virtual  bool  CreateDotPen(int nWidth, Color* pColor) = 0;
};

interface IRenderBrush : public IRenderResource
{
public:
	virtual  bool  CreateSolidBrush(Color* pColor) = 0;
};

class IRenderTarget
{
public:
	IRenderTarget(HWND hWnd);
	virtual ~IRenderTarget() =0 {};
	virtual void     Release();

	virtual GRAPHICS_RENDER_LIBRARY_TYPE GetGraphicsRenderLibraryType() = 0;

	virtual void     BindHDC(HDC hDC) = 0;
	virtual HDC      GetBindHDC() = 0;
	virtual HDC      GetHDC() = 0;
	virtual void     ReleaseHDC( HDC hDC ) = 0;
	
	virtual HRGN     GetClipRgn() = 0;
	virtual int      SelectClipRgn( HRGN hRgn, int nMode = RGN_COPY) = 0;
	virtual BOOL     GetViewportOrgEx( LPPOINT lpPoint ) = 0;
	virtual BOOL     SetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL) = 0;
	virtual BOOL     OffsetViewportOrgEx( int x, int y, LPPOINT lpPoint = NULL) = 0;
	
	virtual void     FillRgn(HRGN hRgn, UI::Color* pColor) = 0;
	virtual void     FillRect(const CRect* lprc, UI::Color* pColor) = 0;
	virtual void     TileRect(const CRect* lprc, IRenderBitmap*) = 0;
	virtual void     Rectangle(const CRect* lprc, UI::Color* pColBorder, UI::Color* pColBack, int nBorder, bool bNullBack) = 0;
	virtual void     DrawFocusRect(const CRect* lprc) = 0;
	virtual void     DrawLine(int x1, int y1, int x2, int y2, IRenderPen*) = 0;
	virtual void     DrawPolyline(POINT* lppt, int nCount, IRenderPen*) = 0;
	virtual void     GradientFillH(const CRect* lprc, COLORREF colFrom, COLORREF colTo) = 0;
	virtual void     GradientFillV(const CRect* lprc, COLORREF colFrom, COLORREF colTo) = 0;
	virtual void     BitBlt(int xDest, int yDest, int wDest, int hDest, IRenderTarget* pSrcHDC, int xSrc, int ySrc, DWORD dwRop ) = 0;
	virtual void     ImageList_Draw(IRenderBitmap* , int x, int y, int col, int row, int cx, int cy) = 0;
	virtual void     DrawBitmap(IRenderBitmap* , DRAWBITMAPPARAM* pParam) = 0;
	virtual void     DrawRotateBitmap(IRenderBitmap* pBitmap, int nDegres, DRAWBITMAPPARAM* pParam) = 0;
    virtual void     DrawString(IRenderFont* pFont, DRAWTEXTPARAM* pParam) = 0;

	//////////////////////////////////////////////////////////////////////////
	//  resource object

	virtual IRenderPen*    CreateSolidPen(int nWidth, Color* pColor) = 0;
	virtual IRenderPen*    CreateDotPen(int nWidth, Color* pColor) = 0; 
	virtual IRenderBrush*  CreateSolidBrush(Color* pColor) = 0;

	virtual bool     BeginDraw(RECT* prcArray, int rcCount, bool bClear=false) = 0;
	virtual void     EndDraw() = 0;
	virtual void     ResizeRenderTarget( int nWidth, int nHeight ) = 0;
	virtual BYTE*    LockBits() = 0;
	virtual void     UnlockBits() = 0;
	virtual void     Clear() = 0;
	virtual void     Save( const String& strPath ) = 0;
	virtual HBITMAP  CopyRect(RECT *prc) = 0;

protected:
	HWND    m_hWnd; 
};

interface IUICursor
{
    virtual long  AddRef() = 0;
    virtual long  Release() = 0;

    virtual HCURSOR  GetCursor() = 0;
};

}

#endif // RENDERLIBRARY_H_599A2143_1D66_44b0_9A19_DE010366346F