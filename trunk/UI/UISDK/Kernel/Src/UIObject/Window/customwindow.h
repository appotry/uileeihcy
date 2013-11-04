#ifndef CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24
#define CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24

#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"


namespace UI
{

class LayeredWindowWrap;

//
// 自定义窗口类，无非客户区域
//
class CustomWindow : public Window
{
public:
	CustomWindow();
	~CustomWindow(void);

	UI_DECLARE_OBJECT3(CustomWindow, OBJ_WINDOW, _T("Kernel/Window"))

	VIRTUAL_BEGIN_MSG_MAP(CustomWindow)
		MESSAGE_HANDLER(WM_NCPAINT, _OnNcPaint)
		MESSAGE_HANDLER(WM_NCACTIVATE, _OnNcActivate)
		MESSAGE_HANDLER(WM_NCDESTROY,  _OnNcDestroy)
        MESSAGE_HANDLER(WM_SIZE, _OnSize )
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGING, _OnWindowPosChanging)
		MESSAGE_HANDLER(WM_WINDOWPOSCHANGED, _OnWindowPosChanged)
		MESSAGE_HANDLER(WM_CANCELMODE, _OnCancelMode)
#if(_WIN32_WINNT >= 0x0600)
        MESSAGE_HANDLER(WM_DWMCOMPOSITIONCHANGED, _OnDwmCompositionChanged)
#endif
		CHAIN_MSG_MAP(Window)
	END_MSG_MAP()

	UI_BEGIN_MSG_MAP
		UIMSG_WM_SETCURSOR(OnSetCursor)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_LBUTTONUP(OnLButtonUp)
		UIMSG_WM_MOUSEMOVE(OnMouseMove)
		UIMSG_WM_HITTEST(OnHitTest)
		
//		UIMSG_WM_NCHITTEST(OnNcHitTest)
// 		UIMSG_WM_NCMOUSEMOVE    (OnNcMouseMove)
// 		UIMSG_WM_NCMOUSEHOVER   (OnNcMouseHover)
// 		UIMSG_WM_NCMOUSELEAVE   (OnNcMouseLeave)
// 		UIMSG_WM_NCLBUTTONDOWN  (OnNcLButtonDown)
// 		UIMSG_WM_NCLBUTTONUP    (OnNcLButtonUp)
// 		UIMSG_WM_NCLBUTTONDBLCLK(OnNcLButtonDblClk)
// 		UIMSG_WM_NCRBUTTONDOWN  (OnNcRButtonDown)
// 		UIMSG_WM_NCRBUTTONUP    (OnNcRButtonUp)
// 		UIMSG_WM_NCRBUTTONDBLCLK(OnNcRButtonDblClk)
// 		UIMSG_WM_NCMBUTTONDOWN  (OnNcMButtonDown)
// 		UIMSG_WM_NCMBUTTONUP    (OnNcMButtonUp)
// 		UIMSG_WM_NCMBUTTONDBLCLK(OnNcMButtonDblClk)

        UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_OBJECTLOADED(OnObjectLoaded)
        UIMSG_WM_PRECREATEWINDOW(PreCreateWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(Window)

    ICustomWindow* GetICustomWindow() { return m_pICustomWindow; }
    void SetICustomWindow(ICustomWindow* p) { m_pICustomWindow = p; }

	// 公共方法
public:
	void  SetWindowResizeType(UINT nType);
	bool  IsWindowLayered();
	void  SetWindowLayered(bool b);
    void  SetWindowAreo(bool b);
	void  SetWindowTransparentMaskType(int type);
    int   GetWindowTransparentMaskType();
	void  SetWindowTransparentColMask(COLORREF col);
	void  SetWindowTransparentColMask(const String& strColdID );
	void  SetWindowTransparentAlphaMask(int nAlpha);
    Image9Region*  GetWindowTransparentRgn9Region() { return &m_TransparentRgn9Region; }
    void  SetWindowTransparentRgn9Regoin(Image9Region* p) { m_TransparentRgn9Region = *p; }

public:
    void  UpdateWindowRgn();

protected:
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload=false);
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

	BOOL  PreCreateWindow(CREATESTRUCT* pcs);
    void  OnObjectLoaded(); 
    GRAPHICS_RENDER_LIBRARY_TYPE OnGetGraphicsRenderType();

    virtual void  OnInnerInitWindow();
	virtual void  CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount=1);
    virtual bool  DrawObject(IRenderTarget* p, RenderContext roc);

	// 消息响应
protected:
	LRESULT  _OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnNcDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnWindowPosChanging(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnWindowPosChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT  _OnCancelMode(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT  _OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	void     OnEraseBkgnd(IRenderTarget* hDC);
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
	void    UpdateWindowRgn(BYTE* pBits);
	HRGN    GetExcludeRgn( BYTE* pBits, const RECT& rc, bool bOffsetToOrigin );
	bool    TestResizeBit( int nBit );

protected:
    ICustomWindow*  m_pICustomWindow;

    //
	// 窗口透明处理相关数据
	//
	int       m_nWindowTransparentMaskType; // <-- 注：可以同时使用颜色+透明度的方式来设置窗口形状
	Color*    m_pColMask;    // 需要进行透明处理的颜色
	int       m_nAlphaMask;   // 需要进行透明处理的alpha值，小于该alpha值的区域都将被抠掉
	
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

	BOOL      PreCreateWindow(CREATESTRUCT* pcs);
	void      InitLayeredWindow();
	void      ReleaseLayeredWindow();
	void      OnWindowPosChanging(LPWINDOWPOS lpWndPos);
	void      OnWindowPosChanged(LPWINDOWPOS lpWndPos);

	void      OnLButtonDown(UINT nHitTest);
	void      OnLButtonUp();
	void      OnMouseMove();
	void      OnEnterSizeMove(UINT nHitTest);
	void      OnExitSizeMove();
	void      OnCancelMode();

protected:
	void      Commit2LayeredWindow();
    bool      IsMinimized();

protected:
	friend    class CustomWindow;
	CustomWindow*   m_pWindow;     // 对应的窗口指针

	POINT     m_ptWindow;          // 分层窗口的坐标
	SIZE      m_sizeWindow;        // 分层窗口的大小

	// 分层窗口拉伸时使用的中间参数
	UINT      m_nHitTestFlag;      // 拉伸标识
	POINT     m_ptStartSizeMove;   // 开始拉伸时，鼠标的位置，用于计算鼠标偏移
	POINT     m_ptWindowOld;       // 开始拉伸时的窗口坐标，用于和偏移一起计算新位置
	SIZE      m_sizeWindowOld;     // 开始拉伸时的窗口大小，用于和偏移一起计算新大小

};

}
#endif // CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24