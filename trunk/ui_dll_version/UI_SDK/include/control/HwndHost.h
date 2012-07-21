#pragma  once

namespace UI
{

//
//  包装一个原始的HWND对象
//
class UIAPI HwndHost : public Object
{
public:
	UI_DECLARE_OBJECT(HwndHost, OBJ_HWNDHOST);

	HwndHost();
	~HwndHost();

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP(Object)
	UI_END_MSG_MAP

protected:

public:
	bool              SubclassWindow();
	bool              UnSubclassWindow();
	void              OnSubclassWindow();
	void              OnUnsubclassWindow();

	virtual   bool    SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	virtual   SIZE    GetDesiredSize( HRDC hDC );  

protected:
	static LRESULT    CALLBACK _WndProc( HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT		      WndProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	LRESULT           DefWindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
	HWND              m_hWnd;

protected:
	CWndProcThunk     m_thunk;               // ATL中的THUNK，用于将一个窗口过程作成自己的成员函数
	WNDPROC           m_oldWndProc;          // 该窗口的原始窗口过程

};

}