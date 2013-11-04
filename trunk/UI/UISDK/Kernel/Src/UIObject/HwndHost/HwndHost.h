#pragma  once
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Inc\Interface\ihwndhost.h"

namespace UI
{

//
//  ��װһ��ԭʼ��HWND����
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

    virtual bool  DrawObject(IRenderTarget* p, RenderContext roc) {return false;} // ������
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
	CWndProcThunk  m_thunk;   // ATL�е�THUNK�����ڽ�һ�����ڹ��������Լ��ĳ�Ա����
	WNDPROC  m_oldWndProc;    // �ô��ڵ�ԭʼ���ڹ���
};

}