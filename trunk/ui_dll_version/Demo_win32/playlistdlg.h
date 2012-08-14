#pragma once


//////////////////////////////////////////////////////////////////////////
// 模仿千千静听播放列表的自绘控件

class TTPlayerPlaylistItem : public ListItemBase
{
public:
	TTPlayerPlaylistItem(ListCtrlBase* pCtrl):ListItemBase(pCtrl){};

public:
	String   m_strFilePath;
	String   m_strFileName;
	String   m_strFileTime;
};
class TTPlayerPlaylistCtrl : public ListCtrlBase
{
public:
	UI_DECLARE_OBJECT( TTPlayerPlaylistCtrl, OBJ_CONTROL )

public:
	virtual  void ResetAttribute();
	virtual  bool SetAttribute(ATTRMAP& mapAttrib, bool bReload);

public:
	void    AddFileItem(const String& strFilePath, bool bUpdate=true);

	virtual  void OnDrawItem(HRDC hRDC, ListItemBase* p);
	virtual  SIZE OnMeasureItem( ListItemBase* p);
	virtual  void OnDeleteItem( ListItemBase* p );
};

////////////////////////////////////////////////////////////////////////// 
// 列表窗口

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
	virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
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

