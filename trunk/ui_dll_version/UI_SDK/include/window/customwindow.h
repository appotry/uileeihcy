namespace UI
{

#define WRSB_TOP              0x8000     // ↑
#define WRSB_LEFT             0x4000     // ←
#define WRSB_BOTTOM           0x2000     // ↓
#define WRSB_RIGHT            0x1000     // →
#define WRSB_TOPLEFT          0x0800     // ↖
#define WRSB_TOPRIGHT         0x0400     // ↗
#define WRSB_BOTTOMLEFT       0x0200     // ↙
#define WRSB_BOTTOMRIGHT      0x0100     // ↘
#define WRSB_ALL              WRSB_TOP | WRSB_LEFT | WRSB_BOTTOM | WRSB_RIGHT | WRSB_TOPLEFT | WRSB_TOPRIGHT | WRSB_BOTTOMLEFT | WRSB_BOTTOMRIGHT
#define WRSB_NONE             0x0

class LayeredWindowWrap;

//
// 自定义窗口类，无非客户区域
//

class UIAPI CustomWindow : public Window
{
public:
	CustomWindow(void);
	~CustomWindow(void);

	UI_DECLARE_OBJECT( CustomWindow, OBJ_WINDOW )

	BEGIN_MSG_MAP(CustomWindow)
		MESSAGE_HANDLER(WM_NCPAINT, _OnNcPaint)
		MESSAGE_HANDLER(WM_NCACTIVATE, _OnNcActivate)
		MESSAGE_HANDLER(WM_NCDESTROY,  _OnNcDestroy)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, _OnWindowPosChanged)
		MESSAGE_HANDLER(WM_CANCELMODE, _OnCancelMode)
		CHAIN_MSG_MAP(Window)
	END_MSG_MAP()

	UI_BEGIN_MSG_MAP
		UIMSG_WM_SETCURSOR( OnSetCursor )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
		UIMSG_WM_LBUTTONDOWN( OnLButtonDown )
		UIMSG_WM_LBUTTONUP( OnLButtonUp )
		UIMSG_WM_MOUSEMOVE( OnMouseMove )
		UIMSG_WM_SIZE( OnSize )
		UIMSG_WM_HITTEST( OnHitTest )
		

//		UIMSG_WM_NCHITTEST( OnNcHitTest )
// 		UIMSG_WM_NCMOUSEMOVE    ( OnNcMouseMove )
// 		UIMSG_WM_NCMOUSEHOVER   ( OnNcMouseHover )
// 		UIMSG_WM_NCMOUSELEAVE   ( OnNcMouseLeave )
// 		UIMSG_WM_NCLBUTTONDOWN  ( OnNcLButtonDown )
// 		UIMSG_WM_NCLBUTTONUP    ( OnNcLButtonUp )
// 		UIMSG_WM_NCLBUTTONDBLCLK( OnNcLButtonDblClk )
// 		UIMSG_WM_NCRBUTTONDOWN  ( OnNcRButtonDown )
// 		UIMSG_WM_NCRBUTTONUP    ( OnNcRButtonUp )
// 		UIMSG_WM_NCRBUTTONDBLCLK( OnNcRButtonDblClk )
// 		UIMSG_WM_NCMBUTTONDOWN  ( OnNcMButtonDown )
// 		UIMSG_WM_NCMBUTTONUP    ( OnNcMButtonUp )
// 		UIMSG_WM_NCMBUTTONDBLCLK( OnNcMButtonDblClk )

	UI_END_MSG_MAP_CHAIN_PARENT(Window)


	// 公共方法
public:
	void      SetWindowResizeType( UINT nType );
	bool      IsWindowLayered();
	void      SetWindowLayered(bool b);
	void      SetWindowTransparentMaskType( int type );
	void      SetWindowTransparentColMask( COLORREF col );
	void      SetWindowTransparentColMask( const String& strColdID );
	void      SetWindowTransparentAlphaMask( int nAlpha );

	// 虚拟方法
protected:
	virtual   BOOL PreCreateWindow( CREATESTRUCT& cs );
	virtual   void OnInitWindow( );
	virtual   void InvalidateObject( Object* pInvalidateObj, RECT* prc, bool bUpdateNow );
	virtual   HRDC BeginDrawObject( Object* pInvalidateObj );
	virtual   void EndDrawObject( CRect* prcWindow, bool bFinish );

public:
	// object 虚函数
	virtual   void ResetAttribute();
	virtual   bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	// 18105
	// 消息响应
protected:

	LRESULT  _OnNcPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnNcActivate( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnNcDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnWindowPosChanged( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	LRESULT  _OnCancelMode( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled );
	
	void     OnEraseBkgnd(HRDC hDC);
	void     OnSize(UINT nType, int cx, int cy);
	BOOL     OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void     OnLButtonDown(UINT nFlags, POINT point);
	void     OnLButtonUp(UINT nFlags, POINT point);
	void     OnMouseMove(UINT nFlags, POINT point);
	UINT     OnHitTest(POINT* p);

//	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
//	LRESULT  OnNcHitTest( POINT pt );
// 	void OnNcMouseMove( UINT nHitTest, POINT point );
// 	void OnNcMouseHover( UINT nHitTest, POINT point ); 
// 	void OnNcMouseLeave();
// 	void OnNcLButtonDown( UINT nHitTest, POINT point );
// 	void OnNcLButtonUp( UINT nHitTest, POINT point );
// 	void OnNcLButtonDblClk( UINT nHitTest, POINT point );
// 	void OnNcRButtonDown( UINT nHitTest, POINT point );
// 	void OnNcRButtonUp( UINT nHitTest, POINT point );
// 	void OnNcRButtonDblClk( UINT nHitTest, POINT point );
// 	void OnNcMButtonDown( UINT nHitTest, POINT point );
// 	void OnNcMButtonUp( UINT nHitTest, POINT point );
// 	void OnNcMButtonDblClk( UINT nHitTest, POINT point );

protected:
	void    UpdateWindowRgn( HRDC hRDC );
	void    UpdateWindowRgn( BYTE* pBits );
	HRGN    GetExcludeRgn( BYTE* pBits, const RECT& rc, bool bOffsetToOrigin );
	bool    TestResizeBit( int nBit );

protected:

	//
	// 窗口透明处理相关数据
	//

	int       m_nWindowTransparentMaskType; // <-- 注：可以同时使用颜色+透明度的方式来设置窗口形状
	UIColor*  m_pColMask;    // 需要进行透明处理的颜色
	int       m_nAlphaMask;  // 需要进行透明处理的alpha值，小于该alpha值的区域都将被抠掉
	
	
	bool     m_bNeedToSetWindowRgn;   // 是否需要重新设置窗口异形区域
	WINDOW_TRANSPARENT_PART_TYPE   m_eTransparentRgnType;
	Image9Region    m_TransparentRgn9Region;
	HRGN     m_hRgn_topleft;
	HRGN     m_hRgn_topright;
	HRGN     m_hRgn_bottomleft;
	HRGN     m_hRgn_bottomright;

	//
	// 窗口伸缩能力标识位
	//
	int      m_nResizeBit;         // 窗口边缘伸缩标志位

	//
	// 分层窗口功能
	//
	LayeredWindowWrap*  m_pLayeredWindowWrap;
	friend class LayeredWindowWrap;
};

//
//	分层窗口实现代码
//
class LayeredWindowWrap
{
public:
	LayeredWindowWrap(CustomWindow* pWindow);
	~LayeredWindowWrap();

	BOOL      PreCreateWindow( CREATESTRUCT& cs );
	void      InitLayeredWindow();
	void      ReleaseLayeredWindow();
	void      OnSize( UINT nType, int cx, int cy );
	void      OnWindowPosChanged(LPWINDOWPOS lpWndPos);
	void      InvalidateObject( Object* pInvalidateObj, bool bUpdateNow );
	HRDC      BeginDrawObject( Object* pInvalidateObj);

	void      OnLButtonDown(UINT nHitTest);
	void      OnLButtonUp();
	void      OnMouseMove();
	void      OnEnterSizeMove(UINT nHitTest);
	void      OnExitSizeMove();
	void      OnCancelMode();

protected:
	void      Commit2LayeredWindow();

protected:
	friend    class CustomWindow;
	CustomWindow*   m_pWindow;     // 对应的窗口指针

	HDC       m_hLayeredMemDC;     // 分层窗口需要自己维护一份图片,UpdateLayeredWindow的第二个HDC
	HBITMAP   m_hLayeredBitmap;    // 分层窗口需要自己维护一份图片

	POINT     m_ptWindow;          // 分层窗口的坐标
	SIZE      m_sizeWindow;        // 分层窗口的大小

	// 分层窗口拉伸时使用的中间参数
	UINT      m_nHitTestFlag;      // 拉伸标识
	POINT     m_ptStartSizeMove;   // 开始拉伸时，鼠标的位置，用于计算鼠标偏移
	POINT     m_ptWindowOld;       // 开始拉伸时的窗口坐标，用于和偏移一起计算新位置
	SIZE      m_sizeWindowOld;     // 开始拉伸时的窗口大小，用于和偏移一起计算新大小

	
	
};

}