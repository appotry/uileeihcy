#ifndef CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24
#define CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24

#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\window.h"


namespace UI
{

class LayeredWindowWrap;

//
// �Զ��崰���࣬�޷ǿͻ�����
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

	// ��������
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

	// ��Ϣ��Ӧ
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
	// ����͸�������������
	//
	int       m_nWindowTransparentMaskType; // <-- ע������ͬʱʹ����ɫ+͸���ȵķ�ʽ�����ô�����״
	Color*    m_pColMask;    // ��Ҫ����͸���������ɫ
	int       m_nAlphaMask;   // ��Ҫ����͸�������alphaֵ��С�ڸ�alphaֵ�����򶼽����ٵ�
	
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
	CustomWindow*   m_pWindow;     // ��Ӧ�Ĵ���ָ��

	POINT     m_ptWindow;          // �ֲ㴰�ڵ�����
	SIZE      m_sizeWindow;        // �ֲ㴰�ڵĴ�С

	// �ֲ㴰������ʱʹ�õ��м����
	UINT      m_nHitTestFlag;      // �����ʶ
	POINT     m_ptStartSizeMove;   // ��ʼ����ʱ������λ�ã����ڼ������ƫ��
	POINT     m_ptWindowOld;       // ��ʼ����ʱ�Ĵ������꣬���ں�ƫ��һ�������λ��
	SIZE      m_sizeWindowOld;     // ��ʼ����ʱ�Ĵ��ڴ�С�����ں�ƫ��һ������´�С

};

}
#endif // CUSTOMWINDOW_H_E436450D_B466_4889_A8AA_CA6FCABCEB24