#pragma  once

//////////////////////////////////////////////////////////////////////////
//
//                             �༭��ؼ�
//
//////////////////////////////////////////////////////////////////////////

//
//	2012.6.2
//		libo  ��д
//
//
//	TODO:
//		a. ��������ģʽ��������С����������
//		b. ��������ģʽ
//		c. ����CTRL+<- CTRL+-> ʶ�����Ĺ���
//		d. ˫��ѡ�еĹ���
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
//	��װ�༭�������
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
	void            DeleteSelectionText();   // �ú����������ڲ�����,�������ݽ��д�����ɾ���ı�

	void            Fire_Text_Changed();
	bool            FilterString(const String& strSrc, String& strDest);
	bool            FilterChar(const TCHAR& cSrc, TCHAR& cDest);

	bool            StringAnalysis();

private:
	String          m_strText;              // �༭�������
	int             m_nMaxChar;             // �������������ַ�����-1��ʾ������

	int	            m_nSelStart;            // ѡ����ַ����			
	int             m_nCaret;               // ��ǰ����λ�ã�Ҳ��־��ѡ����End pos

	bool            m_bInsertMode;          // ����/��дģʽ

	// UniScribe��ر���
	SCRIPT_CONTROL	m_ScriptControl;		// For uniscribe
	SCRIPT_STATE	m_ScriptState;			// For uniscribe

	SCRIPT_STRING_ANALYSIS	m_Analysis;     // For cp2x, x2cp...
	int             m_nTextWidth;           // ��ǰ�ַ����ܳ���

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


	// ��Ϣ����
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
	// ��Object�̳������麯��
	virtual      void        ResetAttribute();
	virtual      bool        SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	// ��Control�̳й������麯��
	virtual      SIZE        GetAutoSize( HRDC hDC ); 

	// �Լ�������麯��
	virtual      bool        FilterInputChar( UINT nChar );

public:
	String      GetText();
	void        SetText( const String& strText );

	// ˽�з���
private:
	void        DrawNormal( HRDC hRDC );
	void        DrawFocus( HRDC hRDC );

	void        CalcCaretPos(int nCaretIndex, bool& bUpdate);
	void        UpdateCaretByPos();

	// ����
protected:
	EditData    m_EditData;

	// ����
private:
	bool		m_bMouseDrag;				// �Ƿ��������������϶�ѡ��

	int			m_nXScroll;	                // �ַ������ĳ���
	int			m_nCaretHeight;				// ���ĸ߶�

	int			m_nXSelStart;				// ��ѡ����ַ�����x����
	int			m_nXSelEnd;					// ��ѡ����ַ��յ��x����
	int			m_nXCaretPos;				// ����λ�ã�����padding.left

	friend class EditData;

	// ����
private:
	UIColor*    m_pColor;
	UIColor*    m_pColorSelect;
	UIColor*    m_pColorSelectBk;
};

}
