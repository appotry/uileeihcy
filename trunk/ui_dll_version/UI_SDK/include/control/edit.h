#pragma  once

//////////////////////////////////////////////////////////////////////////
//
//                             编辑框控件
//
//////////////////////////////////////////////////////////////////////////

//
//	2012.6.2
//		libo  重写
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

	void            SetText(const String& str, bool& bUpdate);
	void            ReplaceChar(const TCHAR& c, bool& bUpdate);
	void            ReplaceString(const String& str, bool& bUpdate);
	void            Delete(bool& bUpdate);
	void            BackSpace(bool& bUpdate);
	void            SetCaret(int nCaret, bool bSetSelStart, bool& bUpdate );

	void            CutToClipboard();
	void            CopyToClipboard();
	void            PasteFromClipboard();
	void            GetPriorItemPos( int nCP, int* pPrior );
	void            GetNextItemPos( int nCP, int* pNext );

	void            SetMaxChar(int nMax);
	void            SetMaxChar(int nMax, bool bUpdate);
	void            SetInsertMode( bool bInsertOrOverride );
	bool            GetInsertMode() { return m_bInsertMode; }

	void            GetText(String& str) { str = m_strText; }
	const String&   GetText()       { return m_strText; }
	int             GetTextLength() { return (int)m_strText.length(); }
	int             GetCaretIndex() { return m_nCaret; }
	int             GetSelectionLength() { return abs(m_nCaret - m_nSelStart); }
	int             GetTextWidth()  { return m_nTextWidth; }
	void            GetSelectionInfo( int& nLeft, int& nRight );
	void            GetSelectionText( String& str );
	bool            IsSelectionExist();

	bool            CP2X(int nCP, int* pX);
	bool            X2CP(int nX, int* pnCP, int* pbTrailOrLead);

protected:
	void            DeleteSelectionText();   // 该函数仅用于内部调用,不对数据进行处理，仅删除文本

	void            Fire_Text_Changed();
	bool            FilterString(const String& strSrc, String& strDest);
	bool            FilterChar(const TCHAR& cSrc, TCHAR& cDest);

	bool            StringAnalysis();

private:
	String          m_strText;              // 编辑框的内容
	int             m_nMaxChar;             // 允许输入的最大字符数，-1表示无限制

	int	            m_nSelStart;            // 选择的字符起点			
	int             m_nCaret;               // 当前光标的位置，也标志着选区的End pos

	bool            m_bInsertMode;          // 插入/重写模式

	// UniScribe相关变量
	SCRIPT_CONTROL	m_ScriptControl;		// For uniscribe
	SCRIPT_STATE	m_ScriptState;			// For uniscribe

	SCRIPT_STRING_ANALYSIS	m_Analysis;     // For cp2x, x2cp...
	int             m_nTextWidth;           // 当前字符的总长度

	Edit*           m_pEdit;
};

class UIAPI Edit : public Control
{
public:
	Edit();
	~Edit();
	UI_DECLARE_OBJECT( Edit, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
 		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
 		UIMSG_WM_SETCURSOR( OnSetCursor )
		UIMSG_WM_MOUSEMOVE( OnMouseMove )
 		UIMSG_WM_CHAR( OnChar )
		UIMSG_WM_STATECHANGED( OnStateChanged )
		UIMSG_WM_KEYDOWN( OnKeyDown )
//		UIMSG_WM_KEYUP( OnKeyUp )
		UIMSG_WM_LBUTTONDOWN( OnLButtonDown )
		UIMSG_WM_LBUTTONUP( OnLButtonUp )
		UIMSG_WM_LBUTTONDBLCLK( OnLButtonDblClk )
 		UIMSG_WM_SETFOCUS( OnSetFocus )
 		UIMSG_WM_KILLFOCUS( OnKillFocus )
		UICHAIN_MSG_MAP(Control)
	UI_END_MSG_MAP


	// 消息处理
protected:
	void        OnPaint(HRDC hDC);
	void        OnEraseBkgnd(HRDC hRDC);
	BOOL        OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
	void        OnSetFocus( Object* );
	void        OnKillFocus( Object* );
	void        OnStateChanged(int nOld, int nNew);

	void        OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
	void        OnLButtonDown(UINT nFlags, POINT point);
	void        OnLButtonUp(UINT nFlags, POINT point);
	void        OnMouseMove(UINT nFlags, POINT point);
	void        OnLButtonDblClk(UINT nFlags, POINT point);

	void        OnInputChar( UINT nChar );
	void        OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	void        OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
	void        OnKeyDown_Ctrl_A();
	void        OnKeyDown_Ctrl_X();
	void        OnKeyDown_Ctrl_C();
	void        OnKeyDown_Ctrl_V();
	void        OnKeyDown_Ctrl_Z();
	void        OnKeyDown_Ctrl_Y();
	void        OnKeyDown_Backspace(bool bCtrlDown);
	void        OnKeyDown_Delete(bool bCtrlDown);
	void        OnKeyDown_Left_Top(bool bCtrlDown);
	void        OnKeyDown_Right_Down(bool bCtrlDown);
	void        OnKeyDown_Home(bool bCtrlDown);
	void        OnKeyDown_End(bool bCtrlDown);
	void        OnKeyDown_Insert(bool bCtrlDown);

public:
	// 从Object继承来的虚函数
	virtual      void        ResetAttribute();
	virtual      bool        SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	// 从Control继承过来的虚函数
	virtual      SIZE        GetAutoSize( HRDC hDC ); 

	// 自己定义的虚函数
	virtual      bool        FilterInputChar( UINT nChar );

public:
	String      GetText();
	void        SetText( const String& strText );

	// 私有方法
private:
	void        DrawNormal( HRDC hRDC );
	void        DrawFocus( HRDC hRDC );

	void        CalcCaretPos(int nCaretIndex, bool& bUpdate);
	void        UpdateCaretByPos();

	// 属性
protected:
	EditData    m_EditData;

	// 数据
private:
	bool		m_bMouseDrag;				// 是否正在用鼠标进行拖动选择

	int			m_nXScroll;	                // 字符滚动的长度
	int			m_nCaretHeight;				// 光标的高度

	int			m_nXSelStart;				// 被选择的字符起点的x坐标
	int			m_nXSelEnd;					// 被选择的字符终点的x坐标
	int			m_nXCaretPos;				// 光标的位置，基于padding.left

	friend class EditData;

	// 绘制
private:
	UIColor*    m_pColor;
	UIColor*    m_pColorSelect;
	UIColor*    m_pColorSelectBk;
};

}
