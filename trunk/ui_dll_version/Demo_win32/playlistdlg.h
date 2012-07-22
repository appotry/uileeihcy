#pragma once
class CPlayListDlg : public UI::CustomWindow
{
public:
	CPlayListDlg(void);
	~CPlayListDlg(void);

	UI_BEGIN_MSG_MAP
		UIMSG_WM_CREATE( OnCreate )
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_BN_CLICKED_EX(_T("playlist_add"), OnBtnClickAdd )
		UICHAIN_MSG_MAP( CustomWindow )
	UI_END_MSG_MAP

 
public:
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs, DWORD& dwStyleEx );
	virtual void OnInitWindow( );

	int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	void OnClose();
	void OnBtnClickAdd(Object* pBtnObj, POINT* pt);

public:
	void AddFile(const String& strFile);
	void AddDirectory(const String& strDir);

private:
	TTPlayerPlaylistCtrl*   m_plistctrl;
};

class CLyricDlg : public UI::CustomWindow
{
public:
	CLyricDlg(){}

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_GETRENDERTYPE(OnGetRenderType)
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_BN_CLICKED( _T("switch_layered"), OnSwitchLayered )
		UICHAIN_MSG_MAP(CustomWindow)
	UI_END_MSG_MAP

public:
	void    OnPaint( HRDC );
	void    OnClose();
	void    OnSwitchLayered();
	LRESULT OnGetRenderType() { return GRAPHICS_RENDER_TYPE_GDIPLUS; }
};

class CEqualizerDlg : public UI::CustomWindow
{
public:
	UI_BEGIN_MSG_MAP
		UIMSG_WM_CLOSE( OnClose )
		UIMSG_TRBN_POSCHANGED( _T("progress_hue1"), OnHueChanged1 )
		UIMSG_TRBN_POSCHANGED( _T("progress_hue2"), OnHueChanged2 )
		UICHAIN_MSG_MAP(CustomWindow)
	UI_END_MSG_MAP

	virtual void OnInitWindow( );

	void    OnClose();
	void    OnHueChanged1( int nPos, int nScrollType );
	void    OnHueChanged2( int nPos, int nScrollType );
	
};