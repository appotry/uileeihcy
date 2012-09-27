namespace UI
{

#define WRSB_TOP              0x8000     // ��
#define WRSB_LEFT             0x4000     // ��
#define WRSB_BOTTOM           0x2000     // ��
#define WRSB_RIGHT            0x1000     // ��
#define WRSB_TOPLEFT          0x0800     // �I
#define WRSB_TOPRIGHT         0x0400     // �J
#define WRSB_BOTTOMLEFT       0x0200     // �L
#define WRSB_BOTTOMRIGHT      0x0100     // �K
#define WRSB_ALL              WRSB_TOP | WRSB_LEFT | WRSB_BOTTOM | WRSB_RIGHT | WRSB_TOPLEFT | WRSB_TOPRIGHT | WRSB_BOTTOMLEFT | WRSB_BOTTOMRIGHT
#define WRSB_NONE             0x0

class LayeredWindowWrap;

//
// �Զ��崰���࣬�޷ǿͻ�����
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


	// ��������
public:
	void      SetWindowResizeType( UINT nType );
	bool      IsWindowLayered();
	void      SetWindowLayered(bool b);
	void      SetWindowTransparentMaskType( int type );
	void      SetWindowTransparentColMask( COLORREF col );
	void      SetWindowTransparentColMask( const String& strColdID );
	void      SetWindowTransparentAlphaMask( int nAlpha );

	// ���ⷽ��
protected:
	virtual   BOOL PreCreateWindow( CREATESTRUCT& cs );
	virtual   void OnInitWindow( );
	virtual   void InvalidateObject( Object* pInvalidateObj, RECT* prc, bool bUpdateNow );
	virtual   HRDC BeginDrawObject( Object* pInvalidateObj );
	virtual   void EndDrawObject( CRect* prcWindow, bool bFinish );

public:
	// object �麯��
	virtual   void ResetAttribute();
	virtual   bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	// 18105
	// ��Ϣ��Ӧ
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
	// ����͸�������������
	//

	int       m_nWindowTransparentMaskType; // <-- ע������ͬʱʹ����ɫ+͸���ȵķ�ʽ�����ô�����״
	UIColor*  m_pColMask;    // ��Ҫ����͸���������ɫ
	int       m_nAlphaMask;  // ��Ҫ����͸�������alphaֵ��С�ڸ�alphaֵ�����򶼽����ٵ�
	
	
	bool     m_bNeedToSetWindowRgn;   // �Ƿ���Ҫ�������ô�����������
	WINDOW_TRANSPARENT_PART_TYPE   m_eTransparentRgnType;
	Image9Region    m_TransparentRgn9Region;
	HRGN     m_hRgn_topleft;
	HRGN     m_hRgn_topright;
	HRGN     m_hRgn_bottomleft;
	HRGN     m_hRgn_bottomright;

	//
	// ��������������ʶλ
	//
	int      m_nResizeBit;         // ���ڱ�Ե������־λ

	//
	// �ֲ㴰�ڹ���
	//
	LayeredWindowWrap*  m_pLayeredWindowWrap;
	friend class LayeredWindowWrap;
};

//
//	�ֲ㴰��ʵ�ִ���
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
	CustomWindow*   m_pWindow;     // ��Ӧ�Ĵ���ָ��

	HDC       m_hLayeredMemDC;     // �ֲ㴰����Ҫ�Լ�ά��һ��ͼƬ,UpdateLayeredWindow�ĵڶ���HDC
	HBITMAP   m_hLayeredBitmap;    // �ֲ㴰����Ҫ�Լ�ά��һ��ͼƬ

	POINT     m_ptWindow;          // �ֲ㴰�ڵ�����
	SIZE      m_sizeWindow;        // �ֲ㴰�ڵĴ�С

	// �ֲ㴰������ʱʹ�õ��м����
	UINT      m_nHitTestFlag;      // �����ʶ
	POINT     m_ptStartSizeMove;   // ��ʼ����ʱ������λ�ã����ڼ������ƫ��
	POINT     m_ptWindowOld;       // ��ʼ����ʱ�Ĵ������꣬���ں�ƫ��һ�������λ��
	SIZE      m_sizeWindowOld;     // ��ʼ����ʱ�Ĵ��ڴ�С�����ں�ƫ��һ������´�С

	
	
};

}