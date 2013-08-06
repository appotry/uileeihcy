#ifndef EDIT_H_8AD460A9_25E8_4651_8280_ACFE31EF11DB
#define EDIT_H_8AD460A9_25E8_4651_8280_ACFE31EF11DB
#include "UISDK\Control\Inc\Interface\iedit.h"
#include "UISDK\Control\Src\Control\Caret\caret.h"

//////////////////////////////////////////////////////////////////////////
//
//                             编辑框控件
//
//////////////////////////////////////////////////////////////////////////

//
//	2012.6.2
//		libo  重写
//  2012.9.13
//      libo  修改分层窗口下光标无法显示的问题
//
//
//	TODO:
//		a. 增加数据模式（整数、小数、负数）
//		b. 增加密码模式
//		c. 增加CTRL+<- CTRL+-> 识别中文功能
//		d. 双击选中的功能
//

#include <usp10.h>
#pragma comment(lib, "usp10.lib")

#ifndef _UNICODE 
	#error "usp10 requirs a unicode build."
#endif
namespace UI
{
class Edit;
#define EDIT_DRAW_FLAG_CARET_HEIGHT_CONFIGED  1   // 在属性中已经指定了光标高度，不再需要从字体中进行判断了

//
//	封装编辑框的数据
//
class EditData
{
public:
	EditData();
	~EditData();

public:
	void            BindToEdit(Edit* pEdit);

	void            SetText(const TCHAR*, bool& bUpdate);
	void            ReplaceChar(const TCHAR& c, bool& bUpdate);
	void            ReplaceString(const String& str, bool& bUpdate);
	void            Delete(bool& bUpdate);
	void            BackSpace(bool& bUpdate);
	void            SetCaret(int nCaret, bool bSetSelStart, bool& bUpdate);

	void            CutToClipboard();
	void            CopyToClipboard();
	void            PasteFromClipboard();
	void            GetPriorItemPos(int nCP, int* pPrior);
	void            GetNextItemPos(int nCP, int* pNext);

	void            SetMaxChar(int nMax);
	void            SetMaxChar(int nMax, bool bUpdate);
	void            SetInsertMode(bool bInsertOrOverride);
	bool            GetInsertMode() { return m_bInsertMode; }

	void            GetText(String& str) { str = m_strText; }
	const String&   GetText()       { return m_strText; }
	int             GetTextLength() { return (int)m_strText.length(); }
	int             GetCaretIndex() { return m_nCaret; }
	int             GetSelectionLength() { return abs(m_nCaret - m_nSelStart); }
	int             GetTextWidth()  { return m_nTextWidth; }
	void            GetSelectionInfo(int& nLeft, int& nRight) const;
	void            SetSelectionInfo(int nStart, int nEnd, bool& bUpdate);
	void            GetSelectionText(String& str);
	bool            IsSelectionExist();

	bool            CP2X(int nCP, int* pX);
	bool            X2CP(int nX, int* pnCP, int* pbTrailOrLead);
	void            DestroyStringAnalysis();

protected:
	void            DeleteSelectionText();   // 该函数仅用于内部调用,不对数据进行处理，仅删除文本

	void            Fire_Text_Changed(BOOL bSetText=FALSE);
	bool            FilterString(const TCHAR* szSrc, String& strDest);
	bool            FilterChar(const TCHAR& cSrc, TCHAR& cDest);

	bool            StringAnalysis();

private:
	String          m_strText;              // 编辑框的内容
	int             m_nMaxChar;             // 允许输入的最大字符数，-1表示无限制

	int	            m_nSelStart;            // 选择的字符起点，当没有选区时	m_nSelStart==m_nCaret	
	int             m_nCaret;               // 当前光标的位置，也标志着选区的End pos

	bool            m_bInsertMode;          // 插入/重写模式

	// UniScribe相关变量
	SCRIPT_CONTROL	m_ScriptControl;		// For uniscribe
	SCRIPT_STATE	m_ScriptState;			// For uniscribe

	SCRIPT_STRING_ANALYSIS	m_Analysis;     // For cp2x, x2cp...
	int             m_nTextWidth;           // 当前字符的总长度

	Edit*           m_pEdit;
};

class Edit : public MessageProxy
{
public:
	Edit();
	~Edit();

    UI_DECLARE_OBJECT3(Edit, OBJ_CONTROL|CONTROL_EDIT, _T("UICtrl/Control"))

	UI_BEGIN_MSG_MAP
 		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
 		UIMSG_WM_SETCURSOR(OnSetCursor)
		UIMSG_WM_MOUSEMOVE(OnMouseMove)
 		UIMSG_WM_CHAR(OnChar)
		UIMSG_WM_KEYDOWN(OnKeyDown)
//		UIMSG_WM_KEYUP(OnKeyUp)
		UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
		UIMSG_WM_LBUTTONUP(OnLButtonUp)
		UIMSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)
		UIMSG_WM_RBUTTONDOWN(OnRButtonDown)
 		UIMSG_WM_SETFOCUS(OnSetFocus)
 		UIMSG_WM_KILLFOCUS(OnKillFocus)
		UIMSG_WM_WINDOWPOSCHANGED(OnWindowPosChanged)
//		UIMSG_WM_FONTMODIFIED(OnFontModifyed)
		UIMSG_WM_VISIBLE_CHANGED(OnVisibleChanged)
		UIMSG_WM_SHOWWINDOW(OnShowWindow)
        UIMSG_WM_GETDLGCODE(OnGetDlgCode)
        UIMESSAGE_HANDLER_EX(UI_WM_WINDOWLAYEREDCHANGED, OnWindowLayeredChanged)
        UIMESSAGE_HANDLER_EX(WM_IME_NOTIFY, OnImeNotify)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
	UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(Edit, IControl)

    void  SetIEdit(IEdit* p) { m_pIEdit = p; SetIMessageProxy(static_cast<IMessage*>(p)); }
    IEdit*  GetIEdit() { return m_pIEdit; }

	// 消息处理
protected:
	void  OnPaint(IRenderTarget* hDC);
	void  OnEraseBkgnd(IRenderTarget* pRenderTarget);
	BOOL  OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
	void  OnSetFocus(IObject*);
	void  OnKillFocus(IObject*);
	void  OnWindowPosChanged(LPWINDOWPOS);
	void  OnFontModifyed(IRenderFont* pFont);
	void  OnShowWindow(BOOL bShow, UINT nStatus);
    UINT  OnGetDlgCode(LPMSG lpMsg);
	void  OnVisibleChanged(BOOL bVisible, IObject* pParent);
    LRESULT  OnWindowLayeredChanged(UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT  OnImeNotify(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void  OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnLButtonDown(UINT nFlags, POINT point);
	void  OnLButtonUp(UINT nFlags, POINT point);
	void  OnMouseMove(UINT nFlags, POINT point);
	void  OnLButtonDblClk(UINT nFlags, POINT point);
	void  OnRButtonDown(UINT nFlags, CPoint point);

	void  OnInputChar(UINT nChar);
	void  OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	void  OnKeyDown_Ctrl_A();
	void  OnKeyDown_Ctrl_X();
	void  OnKeyDown_Ctrl_C();
	void  OnKeyDown_Ctrl_V();
	void  OnKeyDown_Ctrl_Z();
	void  OnKeyDown_Ctrl_Y();
	void  OnKeyDown_Backspace(bool bCtrlDown);
	void  OnKeyDown_Delete(bool bCtrlDown);
	void  OnKeyDown_Left_Top(bool bCtrlDown);
	void  OnKeyDown_Right_Down(bool bCtrlDown);
	void  OnKeyDown_Home(bool bCtrlDown);
	void  OnKeyDown_End(bool bCtrlDown);
	void  OnKeyDown_Insert(bool bCtrlDown);
    void  OnKeyDown_Return();
    void  OnKeydown_Esc();
    void  OnKeydown_Tab();
   
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMatAttrib, bool bReload);
    void  GetDesiredSize(SIZE* pSize); 

public:
	// 自己定义的虚函数
	virtual bool  FilterInputChar(UINT nChar);

public:
	int   GetTextLength();
//	int   GetText(TCHAR* szBuffer, int nBufferSize);
    const TCHAR*  GetText();
	void  SetText(const TCHAR* szText);
	int   GetEditStyle();
	void  SetEditStyle(int n);

	void  SetSel(int nStartChar, int nEndChar);
	void  GetSel(int& nStartChar,int& nEndChar) const;

	// 私有方法
private:
	void  DrawNormal(IRenderTarget* pRenderTarget);
	void  DrawFocus(IRenderTarget* pRenderTarget);

	void  CalcCaretPos(int nCaretIndex, bool& bUpdate);
	void  UpdateCaretByPos();

	// 属性
protected:
	EditData    m_EditData;

    IEdit*      m_pIEdit;

	// 数据
private:
	bool		m_bMouseDrag;				// 是否正在用鼠标进行拖动选择
	bool        m_bNeedUpdateCaretPos;      // 是否需要在下次刷新的时候更新光标位置
    bool        m_bNeedFixGdiAlpha;         // 如果是在分层窗口下，则需要带上alpha通道

	int			m_nXScroll;	                // 字符滚动的长度
	int			m_nCaretHeight;				// 光标的高度

	int			m_nXSelStart;				// 被选择的字符起点的x坐标
	int			m_nXSelEnd;					// 被选择的字符终点的x坐标
	int			m_nXCaretPos;				// 光标的位置，基于padding.left

	int         m_nDrawFlags;               // 绘制的一些参数设置

	friend class EditData;

	// 绘制
private:
	Color*   m_pColor;
	Color*   m_pColorSelect;
	Color*   m_pColorSelectBk;
	CCaret   m_caret;     // 用于实现分层窗口上的光标显示
};

}

#endif // EDIT_H_8AD460A9_25E8_4651_8280_ACFE31EF11DB