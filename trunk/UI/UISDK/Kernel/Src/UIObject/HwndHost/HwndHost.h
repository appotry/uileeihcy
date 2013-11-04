#pragma  once
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Inc\Interface\ihwndhost.h"

namespace UI
{

//
//  包装一个原始的HWND对象
//
class HwndHost : public Object
{
public:
	UI_DECLARE_OBJECT3(HwndHost, OBJ_HWNDHOST, _T("Kernel/Control"));

	HwndHost();
	~HwndHost();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_VISIBLE_CHANGED(OnVisibleChanged)
		UIMSG_WM_HITTEST(OnHitTest)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
	UI_END_MSG_MAP_CHAIN_PARENT(Object)

    void  SetIHwndHost(IHwndHost* p) { m_pIHwndHost = p; };

protected:
	UINT  OnHitTest(POINT* pt);
	void  OnVisibleChanged(BOOL bVisible, IObject* pObjChanged);
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload=false);
    void  GetDesiredSize(SIZE* pSize);

    virtual bool  DrawObject(IRenderTarget* p, RenderContext roc) {return false;} // 不绘制
public:
	bool  SubclassWindow();
	bool  UnSubclassWindow();
	void  OnSubclassWindow();
	void  OnUnsubclassWindow();

protected:
	static LRESULT CALLBACK  _WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT  WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT  DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

    void  UpdateTabStopStyle();
public:
    IHwndHost*  m_pIHwndHost;
	HWND  m_hWnd;
protected:
	CWndProcThunk  m_thunk;   // ATL中的THUNK，用于将一个窗口过程作成自己的成员函数
	WNDPROC  m_oldWndProc;    // 该窗口的原始窗口过程
};

}