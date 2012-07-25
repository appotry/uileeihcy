#pragma  once

namespace UI
{

//
//    按钮控件的抽象基类，在CONTROL的基础上再封装了按状态绘制虚函数、CLICK事件响应
//
class UIAPI ButtonBase : public Control
{
public:
	ButtonBase();

	UI_DECLARE_OBJECT( ButtonBase, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UIMSG_WM_STATECHANGED(OnStateChanged)
//		UIMSG_WM_MOUSEMOVE( OnMouseMove )
//		UIMSG_WM_MOUSELEAVE( OnMouseLeave )
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
		UIMSG_WM_PAINT(OnPaint)
//		UIMSG_WM_LBUTTONDOWN( OnLButtonDown )
//		UIMSG_WM_LBUTTONDBLCLK( OnLButtonDblClk )
		UIMSG_WM_LBUTTONUP( OnLButtonUp )
		UIMSG_WM_KEYDOWN( OnKeyDown )
		UIMSG_WM_KEYUP( OnKeyUp )
		UIMSG_WM_KILLFOCUS( OnKillFocus )
		UIMSG_WM_SETFOCUS( OnSetFocus )
//		UICHAIN_MSG_MAP_MEMBER(m_render)
		UICHAIN_MSG_MAP(Control)
	UI_END_MSG_MAP


	// 消息处理
protected:
	void OnStateChanged(int nOld, int nNew);
	void OnPaint(HRDC);
	void OnEraseBkgnd(HRDC);
// 	void OnMouseMove(UINT nFlags, POINT point);
// 	void OnMouseLeave();
// 	void OnLButtonDown(UINT nFlags, POINT point);
	void OnLButtonUp(UINT nFlags, POINT point);
//	void OnLButtonDblClk(UINT nFlags, POINT point);
	void OnSetFocus( Object* );
	void OnKillFocus( Object* );
	void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
	
	void analyzeIconTextRect( HRDC hRDC, CRect& rcIcon, CRect& rcText);
	void DrawFocus( HRDC hRDC, CRect* prcIcon, CRect* prcText );

public:
	// object virtual function
	virtual void ResetAttribute();
	virtual bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	virtual SIZE GetAutoSize(HRDC hRDC);

	// my virtual function
	virtual void OnClicked(){};         // 自己被点击了（可在子类中用于设置当前check状态）

public:
	// 外部接口
	void    SetCheck(int nCheckState);
	void    SetChecked();
	void    SetUnChecked();
	int     GetCheck();
	bool    IsChecked();
	int     GetButtonStyle();
	void    SetButtonStyle(int n);
	void    SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType);
	void    SetAutoSizeType(BUTTON_RENDER_AUTO_SIZE_TYPE eType);

protected:
//	ButtonRender   m_render;

protected:
	String         m_strText;
	int            m_nCheckState;       // BST_UNCHECKED, BST_CHECKED
	int            m_nAutoSizeType;     // 按钮自动调整大小类型
	int            m_nIconTextFlag;     // 图标与文本的位置关系
	int            m_nIconMarginText;   // 图标距离文本的大小
	int            m_nDrawFocusType;
	bool           m_bSpaceKeyDown;
};
// 
// class IButtonView : public Message
// {
// public:
// 
// private:
// 	IIconTextView*  m_pIconTextView;
// };
// 
// 
// 
// 
// class ThemeButtonView : public IButtonView
// {
// public:
// 	
// };
// class ImageButtonView : public IButtonView
// {
// public:
// };
// class ColorButtonView : public IButtonView
// {
// public:
// };
// class AlhpaAnimateButtonView : public ImageButtonView
// {
// public:
// };


class UIAPI Button : public ButtonBase
{
public:
	Button();
	~Button();

	UI_DECLARE_OBJECT( Button, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP(ButtonBase)
	UI_END_MSG_MAP
};
 
//
//	复选按钮抽象基类
//
class UIAPI CheckButtonBase : public ButtonBase
{
public:
	CheckButtonBase();

	UI_DECLARE_OBJECT( CheckButtonBase, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP( ButtonBase );
	UI_END_MSG_MAP

public:
	virtual bool SetAttribute( map<String,String>& mapAttrib, bool bReload );
	virtual void OnClicked();
};


//
//	比ImageButton多一个check状态，并默认使用press状态作为check的图片
//	TODO: 未来可考虑扩展为使用第五张图片项作为check状态，并加一个标识符
//
class UIAPI CheckButton : public CheckButtonBase
{
public:
	CheckButton();
	~CheckButton();

	UI_DECLARE_OBJECT( CheckButton, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP(CheckButtonBase)
	UI_END_MSG_MAP

};

//
//	单选按钮抽象基类
//
class UIAPI RadioButtonBase : public ButtonBase
{
public:
	RadioButtonBase();

	UI_DECLARE_OBJECT( RadioButtonBase, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP( ButtonBase );
	UI_END_MSG_MAP

public:
	virtual bool SetAttribute( map<String,String>& mapAttrib, bool bReload );
	virtual void OnClicked();
};

class UIAPI RadioButton : public RadioButtonBase
{
public:

	UI_DECLARE_OBJECT( RadioButton, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP(RadioButtonBase)
	UI_END_MSG_MAP

};

class UIAPI HyperLink : public ButtonBase
{
public:
	HyperLink();

	UI_DECLARE_OBJECT( HyperLink, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UIMSG_WM_SETCURSOR(OnSetCursor)
		UICHAIN_MSG_MAP(ButtonBase);
	UI_END_MSG_MAP

public:
	virtual bool SetAttribute( map<String,String>& mapAttrib, bool bReload );
	virtual void OnClicked();
protected:
	BOOL    OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );

};

//
//	点击后弹出菜单的按钮
//
// class MenuButton : public ButtonBase
// {
// public:
// 	MenuButton();
// 	~MenuButton();
// 
// 	UI_DECLARE_OBJECT( MenuButton, OBJ_CONTROL )
// 
// 	UI_BEGIN_MSG_MAP
// 		MSG_WM_LBUTTONDOWN(OnLButtonDown)
// 		UI_CHAIN_MSG_MAP(ButtonBase)
// 	UI_END_MSG_MAP
// 
// private:
// 	void OnLButtonDown(UINT nFlags, POINT point);	
// 	
// public:
// 	bool    SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
// 	SIZE    GetAutoSize( HDC hDC );
// 
// 	virtual void DrawDisable( HDC hDC );
// 	virtual void DrawNormal( HDC hDC ) ;
// 	virtual void DrawHover( HDC hDC ) ;
// 	virtual void DrawPress( HDC hDC ) ;
// 	virtual void DrawFocus( HDC hDC ) {};  // 不绘制focus
// 
// private:
// 	UIImage*   m_pImageBk;               // 背景图片
// 	UIImage*   m_pIcon;                  // 图标图片
// 
// 	int        m_nImageBkDirection;      // 背景图片的方向
// 	int        m_nImageBkItemWidth;      // 背景图片单项宽度 
// 	int        m_nImageBkItemHeight;     // 背景图片单项高度
// 
// };
// 
// //
// //	既可用于按钮点击，也可用于弹出菜单的按钮
// //
// class ButtonAndMenu : public ButtonBase
// {
// public:
// 	UI_DECLARE_OBJECT( ButtonAndMenu, OBJ_CONTROL )
// 
// 	UI_BEGIN_MSG_MAP
// 		UI_CHAIN_MSG_MAP(ButtonBase)
// 	UI_END_MSG_MAP
// 
// public:
// };
}