#ifndef BUTTON_H_FD4B3D3C_D26B_4e80_B1BB_5070F9E02316
#define BUTTON_H_FD4B3D3C_D26B_4e80_B1BB_5070F9E02316

#include "UISDK\Control\Inc\Interface\ibutton.h"
#define BUTTON_VK_SPACE_MOUSECAPTURE_NOTIFY_ID  138162231
namespace UI
{

//
//    按钮控件的抽象基类，在CONTROL的基础上再封装了按状态绘制虚函数、CLICK事件响应
//
class ButtonBase : public MessageProxy
{
public:
	ButtonBase();

	UI_DECLARE_OBJECT3(ButtonBase, OBJ_CONTROL, _T("UICtrl/Control"))

	UI_BEGIN_MSG_MAP
		UIMSG_WM_STATECHANGED2( OnStateChanged2 )
//		UIMSG_WM_MOUSEMOVE( OnMouseMove )
//		UIMSG_WM_MOUSELEAVE( OnMouseLeave )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )

		UIMSG_WM_LBUTTONDOWN( OnLButtonDown )
		UIMSG_WM_LBUTTONDBLCLK( OnLButtonDblClk )
		UIMSG_WM_LBUTTONUP( OnLButtonUp )
		UIMSG_WM_RBUTTONUP( OnRButtonUp )
		UIMSG_WM_KEYDOWN( OnKeyDown )
		UIMSG_WM_KEYUP( OnKeyUp )
		UIMSG_WM_KILLFOCUS( OnKillFocus )
		UIMSG_WM_SETFOCUS( OnSetFocus )
        UIMSG_WM_QUERYINTERFACE( QueryInterface )
        UIMSG_WM_RESETATTRIBUTE( ResetAttribute )
        UIMSG_WM_SETATTRIBUTE( SetAttribute )
		UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
        UIMSG_WM_FINALCONSTRUCT( FinalConstruct )
    UIALT_MSG_MAP( BUTTON_VK_SPACE_MOUSECAPTURE_NOTIFY_ID )
        {
            return TRUE;
        }
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ButtonBase, IControl)   // 由当前消息入口类将消息传递给IControl处理

    void SetIButtonBase(IButtonBase* p) { m_pIButtonBase = p; SetIMessageProxy(m_pIButtonBase); }

	// 消息处理
protected:
    HRESULT  FinalConstruct(IUIApplication* p);

	void  OnStateChanged2(UINT nMask);
	void  OnEraseBkgnd(IRenderTarget*);
// 	void  OnMouseMove(UINT nFlags, POINT point);
// 	void  OnMouseLeave();
 	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);
	void  OnRButtonUp(UINT nFlags, POINT point);
	void  OnLButtonDblClk(UINT nFlags, POINT point);
	void  OnSetFocus(IObject*);
	void  OnKillFocus(IObject*);
	void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags );
	void  OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags );
	void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
	void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

	void  DrawFocus(IRenderTarget*);
	void  OnClicked(POINT* pt);         // 自己被点击了（可在子类中用于设置当前check状态）

public:
	// 外部接口
	void  SetCheck(int nCheckState);
	void  SetChecked();
	void  SetUnChecked();
	int   GetCheck();
	bool  IsChecked();
	int   GetButtonStyle();
	void  SetButtonStyle(int n);

protected:
    IButtonBase* m_pIButtonBase;
	int     m_nCheckState;       // BST_UNCHECKED, BST_CHECKED
	bool    m_bSpaceKeyDown;     // 是否按下了空格键

};


class Button : public ButtonBase
{
public:
    Button();
    ~Button();

    UI_DECLARE_OBJECT3(Button, OBJ_CONTROL|CONTROL_BUTTON, _T("UICtrl/Control"));

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_BN_CLICKED2(m_pIButton, OnClicked)
        UIMSG_WM_NOTIFY(UI_BN_DRAWFOCUS, DrawFocus)
        UIMSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_OBJECTLOADED(OnObjectLoaded)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
		UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT(ButtonBase)

public:
    void  SetIButton(IButton* p) { m_pIButton = p; }
    IButton*  GetIButton() { return m_pIButton; }

    void  SetText(const TCHAR*  szText, bool bUpdate=true);
    const TCHAR* GetText();

    void  SetIconFromFile(const TCHAR* szIconPath);
    void  SetIconFromHBITMAP(HBITMAP hBitmap);

protected:
    HRESULT FinalConstruct(IUIApplication* p);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
    void  OnObjectLoaded();
    void  OnClicked();

	void  GetDesiredSize(SIZE* pSize);
    UINT  OnGetDlgCode(LPMSG lpMsg);
    void  OnPaint(IRenderTarget*);
    LRESULT  DrawFocus(WPARAM, LPARAM);

public:
    void  SetDrawFocusType(BUTTON_RENDER_DRAW_FOCUS_TYPE eType);
    void  SetAutoSizeType(BUTTON_RENDER_AUTO_SIZE_TYPE eType);

protected:
    void    analyzeIconTextRect(IRenderTarget*, CRect& rcIcon, CRect& rcText);

protected:
    IButton*  m_pIButton;
    String    m_strText;

    int       m_nAutoSizeType;     // 按钮自动调整大小类型
    int       m_nIconAlignFlag;    // 图标与文本的位置关系
    int       m_nIconMarginText;   // 图标距离文本的大小. ps: icon距离按钮边缘的距离可通过padding来实现
    int       m_nDrawFocusType;    // FOCUS类型
};

} 

#endif