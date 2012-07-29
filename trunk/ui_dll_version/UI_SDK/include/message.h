#pragma once


//
//	����ϵͳ�ؼ���WM_COMMAND��UI�ؼ���WM_COMAAND����Ϣ�����ǲ�һ�µ�
//  Ϊ���Է����߻�����������±��������������Ϣ��code���������¶���
//

enum
{
	UI_WM_NOTIFY = WM_USER+WM_USER,

	//
	//	������ʾ��֪ͨ
	//		message:UI_WM_SHOW_TOOLTIP
	//		code:   0
	//		wparam:  
	//		lParam: 
	//		pObjMsgFrom: pWindow
	//
	//	������󲻴��������Ϣ�򷵻�FALSE���򽫵���Ĭ�ϵ���ʾ������ʾtooltip�������ݡ�
	//
	UI_WM_SHOW_TOOLTIP,

	//
	//	��ť����¼�
	//
	//		message:UI_WM_NOTIFY
	//		code:   UI_BN_CLICKED
	//		wparam: POINT*������window
	//		lParam: 
	//		pObjMsgFrom: this
	//
	UI_BN_CLICKED,

	//
	//	������λ�øı�֪ͨ
	//
	//		message:UI_WM_NOTIFY
	//		code:   UI_TRBN_POSCHANGED
	//		wparam: new Pos
	//		lParam: scroll type, SB_LEFT SB_RIGHT SB_THUMBTRACK SB_ENDSCROLL ...
	//		pObjMsgFrom: this
	//
	UI_TRBN_POSCHANGED,
	
	//
	//	������λ�øı�֪ͨ
	//
	//		message:UI_WM_HSCROLL/UI_WM_VSCROLL
	//		code:   0
	//		wparam: Pos Contains the current scroll-box position if the scroll-bar code is SB_THUMBPOSITION or SB_THUMBTRACK; otherwise not used.
	//		lParam: scroll type, SB_LEFT SB_RIGHT SB_THUMBTRACK SB_ENDSCROLL ...
	//		pObjMsgFrom: this
	//
	UI_WM_HSCROLL,
	UI_WM_VSCROLL,

	//
	//	����һ��������ָ���Ķ��������λ��
	//
	//		message: UI_WM_HITTEST
	//		code:
	//		wparam:  x (in window)
	//		lparam:  y (in window)
	//		pObjMsgFrom: 
	//
	//	Return
	//
	//		HTNOWHERE��ʾ���ڸö�������
	//
	UI_WM_HITTEST,

	//
	//	��ȡһ�����ڵĻ�������
	//
	//		message:  UI_WM_GETRENDERTYPE
	//		code:
	//		wparam:
	//		lparam:
	//
	//	Return
	//		
	//		RENDER_TYPE_GDI,RENDER_TYPE_GDIPLUS����RENDER_TYPE_AUTO;
	//
	UI_WM_GETRENDERTYPE,

	//
	//	MouseManager֪ͨһ���������ĵ�ǰ״̬�����ı�(hover,press...)
	//
	//		message:  UI_WM_STATECHANGED
	//		code:
	//		wparam:   old state bits
	//		lparam:   new state bits
	//
	//	Return
	//
	UI_WM_STATECHANGED,

	//
	//	��ȡ��ǰ���Ƶ�ƫ����������OnDraw
	//
	//		message: UI_WM_GETSCROLLOFFSET
	//		code:
	//		wparam:  [out] int* xOffset
	//		lparam:  [out] int* yOffset 
	//
	//	Return
	//
	UI_WM_GETSCROLLOFFSET,


	//
	//	��ȡ���󸽴��Ĺ���������
	//
	//		message: UI_WM_GETSCROLLBAROBJECT
	//		code:
	//		wparam:  HSCROLLBAR/VSCROLLBAR
	//		lparam:  
	//
	//	Return
	//		Object*�����û�и÷����ϵĹ��������򷵻�NULL
	//
	//UI_WM_GETSCROLLBAROBJECT,


	UI_WM_DESTROYPOPUPWINDOW,
};

namespace UI
{

class UIMSG;
class Message;

// 
// ��Ϣ֪ͨ����
long  UIAPI UISendMessage( UIMSG* pMsg, int nMsgMapID=0 );
long  UIAPI UISendMessage( Message* pObjMsgTo, 
						   UINT     message, 
						   WPARAM   wParam=0,  
						   LPARAM   lParam=0, 
						   UINT     code=0,    
						   Message* pObjMsgFrom=NULL,
						   int      nMsgMapID = 0);


//
// ��Ϣ�ṹ����
//   ϵͳ�ؼ���Ȼʹ��MSG����UI�ؼ�ʹ��UIMsg���д���
//
class UIMSG : public MSG
{
public:
	UIMSG()
	{
		hwnd    = NULL;
		message = 0;
		wParam  = lParam = NULL;

		pObjMsgFrom = pObjMsgTo = NULL;
		code = 0;
		lRet = 0;
		bHandled = FALSE;      // Ĭ�ϸ���Ϣû�����
	}

	Message*   pObjMsgFrom;    // ��Ϣ������
	Message*   pObjMsgTo;      // ��Ϣ������ 

	UINT       code;           // ��� WM_COMMAND,WM_NOTIFY

	long       lRet;           // ��Ϣ���������ķ���ֵ
	BOOL       bHandled;       // ����Ϣ�Ƿ��ѱ������
};

//
// �����������������ҵ���Ϣ
//
class MsgHook
{
public:
	MsgHook()
	{
		pObj = NULL;
		nMsgMapIDToHook = 0;
		nMsgMapIDToNotify = 0;
	}

	Message* pObj;                 // ��¼˭Ҫ�����ҵ���Ϣ
	int      nMsgMapIDToHook;      // pObjҪHOOK��map id����Ϣ
	int      nMsgMapIDToNotify;    // HOOK������Ϣ��pObj�Լ�ʹ�ø�map idȥ��Ӧ
};
//
//	����notify messsage������Ҫ֪ͨʲô����
//
class MsgNotify
{
public:
	MsgNotify()
	{
		pObj = NULL;
		nMsgMapIDToNotify = 0;
	}

	Message*  pObj;
	int       nMsgMapIDToNotify;    // ������Ϣ֪ͨpObjʱ��pObjʹ�ø�idȥ��Ӧ
};

//
//	��Ϣ����,object�Ӹ���̳дӶ�ӵ������Ϣ����
//
class UIAPI Message
{

protected:
	list< MsgHook* >     m_lHookMsgMap;  // ����ComboBoxҪhook Combobox�е�������ť���¼�
	list< MsgNotify* >   m_lNotifyMsgMap;// ���簴ť�ĵ���¼���Ҫ֪ͨ����  

	UIMSG                *m_pCurMsg;     // ��¼��ǰ���ڴ������Ϣ TODO���Ƿ���Ҫһ��m_pCurMsg

public:
	Message();
	~Message();

	BOOL IsMsgHandled() const;
	void SetMsgHandled(BOOL);

	void AddNotify( Message* pObj, int nMsgMapID );
	void RemoveNotify( Message* pObj, int nMsgMapID );
	void RemoveNotify( Message* pObj );
	void ClearNotify( );

	void AddHook( Message* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
	void RemoveHook( Message* pObj, int nMsgMapIDToHook, int nMsgMapIDToNotify );
	void RemoveHook( Message* pObj );
	void ClearHook( );
	 

	// ����TRUE����ʾ����Ϣ�ѱ�����FALSE��ʾ����Ϣû������
	virtual BOOL ProcessMessage( UIMSG* pMsg, int nMsgMapID=0 ) = 0 ;

protected:
	void DoNotify( UIMSG* pMsg );
	BOOL DoHook( UIMSG* pMsg, int nMsgMapID );
};


/*Ϊ�˿�������Ϣ��Ӧ������ֱ�ӷ��ʵ�ǰ��Ϣ�ṹ����m_pCurMsg��Ϊ��Ա�������з��ʻ�����*/
/*Ϊ�˷�ֹ�ڴ���һ����ϢA�Ĺ����У������ٷ�������һ����ϢB������A��m_pCurMsg��B�۸ģ�*/
/*����������һ����ʱ�������浱ǰ��Ϣm_pCurMsg��������B�������m_pCurMsg�ָ���A*/
/*Ϊ��ʵ�ֻ�ԭm_pCurMsg�����е���Ϣ����ֱ�ӷ��أ����ǽ�m_pCurMsg��ԭ֮����return*/
#define UI_BEGIN_MSG_MAP                              \
	BOOL ProcessMessage( UIMSG* pMsg, int nMsgMapID = 0 ) \
	{                                                 \
		assert( pMsg != NULL );                       \
		if( NULL == pMsg )                            \
			return FALSE;                             \
			                                          \
		UIMSG*  pOldMsg  = m_pCurMsg;                 \
		BOOL bRet = this->_ProcessMessage(pMsg, nMsgMapID); \
		m_pCurMsg = pOldMsg;                          \
		                                              \
		return bRet;                                  \
	}                                                 \
	BOOL _ProcessMessage( UIMSG* pMsg, int nMsgMapID=0 ) \
	{                                                 \
		/*ȡ��wParam,lParam���Ա����Window��Ϣ�����е�wParam,lParam����*/\
		WPARAM wParam = pMsg->wParam;                 \
		LPARAM lParam = pMsg->lParam;                 \
		UINT   uMsg   = pMsg->message;                \
		UINT   code   = pMsg->code;                   \
		Message* pObjMsgFrom = pMsg->pObjMsgFrom;      \
		long&  lResult = pMsg->lRet; /* ����wtl */    \
		                                              \
		m_pCurMsg = pMsg;                             \
		                                              \
		/*��ϢHOOK����*/                              \
	    if( DoHook(pMsg, nMsgMapID) )                 \
			return TRUE;                              \
			                                          \
		switch( nMsgMapID )                           \
		{                                             \
		case 0:                                       


#define UI_END_MSG_MAP                                \
		    break;                                    \
		}                                             \
		return FALSE;                                 \
	}

//
// ��Ϣ������
//
//	�ú��и�ȱ�㣬�����ܽ��յ���ǰ�ķ�֧��Ϣ�����ܴ���������֧��Ϣ����ʹ�ô�_EX ��׺�ĸĽ�������滻

#define UICHAIN_MSG_MAP( theChainClass )           \
 	if( theChainClass::ProcessMessage( pMsg, nMsgMapID ) ) \
 		return TRUE;                     

#define UICHAIN_MSG_MAP_MEMBER( theChainMember )     \
 	if( theChainMember.ProcessMessage( pMsg, nMsgMapID ) ) \
 		return TRUE;

#define UICHAIN_MSG_MAP_POINT_MEMBER( pTheChainMember )  \
	if( pTheChainMember != NULL )                     \
	{                                                 \
		if( pTheChainMember->ProcessMessage( pMsg, nMsgMapID ) ) \
		{ \
			return TRUE;                              \
		} \
	}

// 
//  ǿ�н�����ǰ��Ϣ��֧��������Ϣȫ�����ݸ���Ҫ�Ķ���
//
//	TODO: �ú�ȱ�㣬ǿ�н����󣬲�֪�����ĸ���֧�ٽ��п�ʼ������Ĭ��ȡΪ0�����齫�ú������ǰ����������
//
#define UI_BEGIN_CHAIN_ALL_MSG_MAP			          \
			break;                                    \
		}

#define UI_END_CHAIN_ALL_MSG_MAP                      \
		switch( nMsgMapID )                           \
		{                                             \
		case 0: 

#define UIALT_MSG_MAP(msgMapID)                       \
		break;                                        \
	case msgMapID:                                    

// void OnCommandHandlerEX(UINT uNotifyCode, int nID, HWND wndCtl)
#define UICOMMAND_HANDLER_EX  COMMAND_HANDLER_EX

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//                               UI��Ϣ����                               //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
// ����Ҫ��������������
//	1. ����Ϣ�Ƿ񱻴������
//	2. ����Ϣ�ķ���ֵ
//	3. ����Ϣ�Ĳ�������


// int OnCreate()
// remark
//		WM_CREATE��Ϣ����ԭʼ��Windows��Ϣ������UI SDK�Լ�ģ�����Ϣ��
//		���Ҳ����Ĳ�������μ���Window::Create
#define UIMSG_WM_CREATE(func)                          \
	if (uMsg == WM_CREATE)                             \
	{                                                  \
		SetMsgHandled(TRUE);                           \
		pMsg->lRet = (long)func((LPCREATESTRUCT)lParam);\
		if(IsMsgHandled())                             \
			return TRUE;                               \
	}

// BOOL OnInitDialog(HWND hWnd, LPARAM lInitParam)
#define UIMSG_WM_INITDIALOG  MSG_WM_INITDIALOG

#define UIMSG_WM_CLOSE   MSG_WM_CLOSE
// void OnClose()
// #define UIMSG_WM_CLOSE(fun)                           \
// 	if( uMsg == WM_CLOSE )                            \
// 	{                                                 \
// 		SetMsgHandled(TRUE);                           \
// 		fun();                                        \
// 		if(IsMsgHandled())                         \
//			return TRUE;                              \
// 	}

// void OnNcDestroy()
#define UIMSG_WM_NCDESTROY MSG_WM_NCDESTROY

#define UIMSG_WM_DESTROY MSG_WM_DESTROY
// void OnDestroy()
// #define UIMSG_WM_DESTROY(fun)                         \
// 	if( uMsg == WM_DESTROY )                          \
// 	{                                                 \
// 		SetMsgHandled(TRUE);                           \
// 		fun();                                        \
// 		if(IsMsgHandled()) return TRUE;            \
// 	}

// void OnEraseBkgnd( HDC hDC )
//	remark
//		����HDC�����Ѿ�������˫���崦���ˣ�����ֱ��ʹ�á�ԭʼ��WM_ERASEBKGND��Ϣ�ѱ�������
//		����ȥ���˷���ֵ���Է��������Լ�Ҳ��֪�����׸÷���ʲô����ʵ����ķ���ֵû����˼��
#define UIMSG_WM_ERASEBKGND(func)                     \
	if ( uMsg == WM_ERASEBKGND )                      \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		/*pMsg->lRet = (long)*/func((HRDC)wParam);     \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnPaint( HDC hDC )	
//	remark
//		�����WM_PAINT��ϢҲ�Ǳ��������hDCΪmemdc���Ѿ�����˫���崦��
#define UIMSG_WM_PAINT(func)                          \
	if ( uMsg == WM_PAINT )                           \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func((HRDC)wParam);                            \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

#define UIMSG_WM_NCPAINT  MSG_WM_NCPAINT
// void OnNcPaint( void )
// remark
//		����Ϣ�е�wParam=(HRGN)updateRgn������
// #define UIMSG_WM_NCPAINT(func)                        \
// 	if ( uMsg == WM_NCPAINT )                         \
// 	{                                                 \
// 		SetMsgHandled(TRUE);                           \
// 		func( );                                      \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCACTIVATE  MSG_WM_NCACTIVATE
// BOOL OnNcActivate( BOOL bActive )
// #define UIMSG_WM_NCACTIVATE(func)                     \
// 	if( uMsg == WM_NCACTIVATE )                       \
// 	{                                                 \
// 		SetMsgHandled(TRUE);                           \
// 		pMsg->lRet = (long)func((BOOL)wParam);        \
// 		if(IsMsgHandled())                         \
//			return TRUE;                              \
// 	}


// LRESULT OnNcHitTest( POINT pt )
// remark
//		����pt�Ѵ�screen����ת����client����
#define UIMSG_WM_NCHITTEST(func)                      \
	if( uMsg == WM_NCHITTEST )                        \
	{                                                 \
		POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };          \
		::ScreenToClient( pMsg->pObjMsgTo->GetWindowObject()->m_hWnd, &pt );\
		SetMsgHandled(TRUE);                          \
		pMsg->lRet = (long)func( pt );                \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// UINT OnHitTest( POINT* p )
#define UIMSG_WM_HITTEST(func)                        \
	if( uMsg == UI_WM_HITTEST )                       \
	{                                                 \
		POINT pt = { (LONG)wParam, (LONG)lParam };    \
		SetMsgHandled(TRUE);                          \
		pMsg->lRet = (long)func( &pt );               \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// BOOL OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
#define UIMSG_WM_SETCURSOR  MSG_WM_SETCURSOR
// #define UIMSG_WM_SETCURSOR(func)                      \
// 	if( uMsg == WM_SETCURSOR )                        \
// 	{                                                 \
// 		HWND hWnd = (HWND)wParam;                     \
// 		UINT nHitTest = (UINT)LOWORD(lParam);         \
// 		UINT message  = (UINT)HIWORD(lParam);         \
// 		SetMsgHandled(TRUE);                           \
// 		pMsg->lRet =(long)func(hWnd,nHitTest,message);\
// 		if(IsMsgHandled())                            \
//			return TRUE;                              \
// 	}

//	void OnSize( UINT nType, int cx, int cy );
#define UIMSG_WM_SIZE(func)                           \
	if( uMsg == WM_SIZE )                             \
	{                                                 \
		UINT nType = (UINT)wParam;                    \
		int  cx    = LOWORD(lParam);                  \
		int  cy    = HIWORD(lParam);                  \
		SetMsgHandled(TRUE);                          \
		func( nType, cx, cy);                         \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

#define UIMSG_WM_SIZING MSG_WM_SIZING
//	void OnSizing( UINT nSize, LPRECT lpRect);
// #define UIMSG_WM_SIZING(func)                      \
// 	if( uMsg == WM_SIZING )                           \
// 	{                                                 \
// 		UINT nType = wParam;                          \
// 		LPRECT lpRect = (LPRECT)lParam;               \
// 		SetMsgHandled(TRUE);                           \
// 		func( nType, lpRect );                        \
// 		if(IsMsgHandled())                            \
//			return TRUE;                              \
// 	}

#define UIMSG_WM_MOVE MSG_WM_MOVE
// void OnMove(POINT ptPos)
// #define UIMSG_WM_MOVE(func)                        \
// 	if (uMsg == WM_MOVE)                              \
// 	{                                                 \
// 		SetMsgHandled(TRUE);                          \
// 		POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)}; \
// 		func( pt );                                   \
// 		if(IsMsgHandled())                            \
//			return TRUE;                              \
// 	}

#define UIMSG_WM_MOVING MSG_WM_MOVING
// void OnMoving(UINT fwSide, LPRECT pRect)
// #define UIMSG_WM_MOVING(func)                         \
// 	if (uMsg == WM_MOVING)                            \
// 	{                                                 \
// 		SetMsgHandled(TRUE);                          \
// 		func((UINT)wParam, (LPRECT)lParam);           \
// 		if(IsMsgHandled())                            \
//			return TRUE;                              \
// }

#define UIMSG_WM_NCCALCSIZE MSG_WM_NCCALCSIZE
// void OnNcCalcSize( BOOL bCalcValidRects,  NCCALCSIZE_PARAMS* lpncsp );
// #define UIMSG_WM_NCCALCSIZE(func)                     \
// 	if( uMsg == WM_NCCALCSIZE )                       \
// 	{                                                 \
// 		BOOL bCalcValidRects = (BOOL)wParam;          \
// 		NCCALCSIZE_PARAMS* lpncsp = (NCCALCSIZE_PARAMS*)lParam;\
// 		SetMsgHandled(TRUE);                           \
// 		func( bCalcValidRects, lpncsp );              \
// 		if(IsMsgHandled())                         \
//			return TRUE;                              \
// 	}

#define UIMSG_WM_GETMINMAXINFO MSG_WM_GETMINMAXINFO
// void OnGetMinMaxInfo(LPMINMAXINFO lpMMI);
// #define UIMSG_WM_GETMINMAXINFO(func)                  \
// 	if( uMsg == WM_GETMINMAXINFO )                    \
// 	{                                                 \
// 		LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;    \
// 		SetMsgHandled(TRUE);                           \
// 		func( lpMMI );                                \
// 		if(IsMsgHandled())                         \
//			return TRUE;                              \
// 	}

// void OnSetFocus( Object* pOldFocusObj )
#define UIMSG_WM_SETFOCUS(func)                       \
	if( uMsg == WM_SETFOCUS )                         \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func( (Object*)wParam );                      \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnKillFocus( Object* pNewFocusObj )
#define UIMSG_WM_KILLFOCUS(func)                      \
	if( uMsg == WM_KILLFOCUS )                        \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func( (Object*)wParam );                      \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnCancelMode()
#define UIMSG_WM_CANCELMODE  MSG_WM_CANCELMODE
//
// ��ȡRender Font
//
// HRFONT OnGetFont()
#define UIMSG_WM_GETFONT(func) \
	if (uMsg == WM_GETFONT) \
	{ \
		SetMsgHandled(TRUE); \
		lResult = (long)(HRFONT)func(); \
		if(IsMsgHandled()) \
			return TRUE; \
	}

// LRESULT OnGetRenderType()
#define UIMSG_WM_GETRENDERTYPE(func)                      \
	if( uMsg == UI_WM_GETRENDERTYPE )                     \
	{                                                     \
		SetMsgHandled(TRUE);                              \
		pMsg->lRet = (long)func();                        \
		if(IsMsgHandled())                                \
			return TRUE;                                  \
	} 

#define UIMSG_WM_MOUSEMOVE MSG_WM_MOUSEMOVE
// point�����ʵ�ʴ���
//void OnMouseMove(UINT nFlags, POINT point)
// #define UIMSG_WM_MOUSEMOVE(func)                      \
// 	if ( uMsg == WM_MOUSEMOVE )                       \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func((UINT)wParam, pt);                       \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_MOUSELEAVE MSG_WM_MOUSELEAVE
// void OnMouseLeave( )
// #define UIMSG_WM_MOUSELEAVE(func)                     \
// 	if ( uMsg == WM_MOUSELEAVE )                      \
// 	{                                                 \
// 		SetMsgHandled(TRUE);                           \
// 		func();                                       \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

// BOOL OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
#define UIMSG_WM_MOUSEWHEEL MSG_WM_MOUSEWHEEL

// void OnLButtonDown(UINT nFlags, POINT point)
#define UIMSG_WM_LBUTTONDOWN MSG_WM_LBUTTONDOWN
// point�����ʵ�ʴ���
//void OnLButtonDown(UINT nFlags, POINT point)
// #define UIMSG_WM_LBUTTONDOWN(func)                    \
// 	if ( uMsg == WM_LBUTTONDOWN )                     \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 	 		return TRUE;                              \
// 	}

// void OnRButtonDown(UINT nFlags, CPoint point)
#define UIMSG_WM_RBUTTONDOWN  MSG_WM_RBUTTONDOWN

// void OnRButtonUp(UINT nFlags, CPoint point)
#define UIMSG_WM_RBUTTONUP    MSG_WM_RBUTTONUP

// void OnLButtonDblClk(UINT nFlags, POINT point)
#define UIMSG_WM_LBUTTONDBLCLK MSG_WM_LBUTTONDBLCLK

// void OnLButtonUp(UINT nFlags, POINT point)
#define UIMSG_WM_LBUTTONUP MSG_WM_LBUTTONUP
// point�����ʵ�ʴ���
// void OnLButtonUp(UINT nFlags, POINT point)
// #define UIMSG_WM_LBUTTONUP(func)                      \
// 	if ( uMsg == WM_LBUTTONUP )                       \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCMOUSEMOVE MSG_WM_NCMOUSEMOVE
//void OnNcMouseMove( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCMOUSEMOVE(func)                    \
// 	if ( uMsg == WM_NCMOUSEMOVE )                     \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

// void OnNcMouseHover( UINT nHitTest, POINT point ); 
#define UIMSG_WM_NCMOUSEHOVER(func)                   \
	if ( uMsg == WM_NCMOUSEHOVER )                    \
	{                                                 \
		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
		SetMsgHandled(TRUE);                          \
		func( (UINT)wParam, pt );                     \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnNcMouseLeave();   
#define UIMSG_WM_NCMOUSELEAVE(func)                     \
	if ( uMsg == WM_NCMOUSELEAVE )                    \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func( );                                      \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

#define UIMSG_WM_NCLBUTTONDOWN MSG_WM_NCLBUTTONDOWN
// void OnNcLButtonDown( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCLBUTTONDOWN(func)                  \
// 	if ( uMsg == WM_NCLBUTTONDOWN )                   \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCLBUTTONUP MSG_WM_NCLBUTTONUP
// void OnNcLButtonUp( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCLBUTTONUP(func)                    \
// 	if ( uMsg == WM_NCLBUTTONUP )                     \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCLBUTTONDBLCLK MSG_WM_NCLBUTTONDBLCLK
// void OnNcLButtonDblClk( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCLBUTTONDBLCLK(func)                \
// 	if ( uMsg == WM_NCLBUTTONDBLCLK )                 \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCRBUTTONDOWN MSG_WM_NCRBUTTONDOWN
// void OnNcRButtonDown( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCRBUTTONDOWN(func)                  \
// 	if ( uMsg == WM_NCRBUTTONDOWN )                   \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCRBUTTONUP MSG_WM_NCRBUTTONUP
// void OnNcRButtonUp( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCRBUTTONUP(func)                    \
// 	if ( uMsg == WM_NCRBUTTONUP )                     \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCRBUTTONDBLCLK MSG_WM_NCRBUTTONDBLCLK
// void OnNcRButtonDblClk( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCRBUTTONDBLCLK(func)                \
// 	if ( uMsg == WM_NCRBUTTONDBLCLK )                 \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCMBUTTONDOWN MSG_WM_NCMBUTTONDOWN
// void OnNcMButtonDown( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCMBUTTONDOWN(func)                  \
// 	if ( uMsg == WM_NCMBUTTONDOWN )                   \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCMBUTTONUP MSG_WM_NCMBUTTONUP
// void OnNcMButtonUp( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCMBUTTONUP(func)                    \
// 	if ( uMsg == WM_NCMBUTTONUP )                     \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}

#define UIMSG_WM_NCMBUTTONDBLCLK MSG_WM_NCMBUTTONDBLCLK
// void OnNcMButtonDblClk( UINT nHitTest, POINT point );
// #define UIMSG_WM_NCMBUTTONDBLCLK(func)                \
// 	if ( uMsg == WM_NCMBUTTONDBLCLK )                 \
// 	{                                                 \
// 		POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
// 		SetMsgHandled(TRUE);                           \
// 		func( (UINT)wParam, pt );                     \
// 		if(IsMsgHandled())                         \
// 			return TRUE;                              \
// 	}


#define UIMSG_WM_CHAR MSG_WM_CHAR
//  void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
// #define UIMSG_WM_CHAR(func)                           \
// 	if ( uMsg == WM_CHAR )                            \
// 	{                                                 \
// 		UINT nChar   = (UINT)wParam;                  \
// 		UINT nRepCnt = (UINT)LOWORD(lParam);          \
// 		UINT nFlags  = (UINT)HIWORD(lParam);          \
// 		SetMsgHandled(TRUE);                           \
// 		func( nChar, nRepCnt, nFlags );               \
// 		if(IsMsgHandled())                         \
//			return TRUE;                              \
// 	}

#define UIMSG_WM_KEYDOWN MSG_WM_KEYDOWN
//  void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
// #define UIMSG_WM_KEYDOWN(func)                        \
// 	if ( uMsg == WM_KEYDOWN )                         \
// 	{                                                 \
// 		UINT nChar   = (UINT)wParam;                  \
// 		UINT nRepCnt = (UINT)LOWORD(lParam);          \
// 		UINT nFlags  = (UINT)HIWORD(lParam);          \
// 		SetMsgHandled(TRUE);                           \
// 		func( nChar, nRepCnt, nFlags );               \
// 		if(IsMsgHandled())                         \
//			return TRUE;                              \
// 	}

#define UIMSG_WM_KEYUP MSG_WM_KEYUP
//  void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
// #define UIMSG_WM_KEYUP(func)                          \
// 	if ( uMsg == WM_KEYUP )                           \
// 	{                                                 \
// 		UINT nChar   = (UINT)wParam;                  \
// 		UINT nRepCnt = (UINT)LOWORD(lParam);          \
// 		UINT nFlags  = (UINT)HIWORD(lParam);          \
// 		SetMsgHandled(TRUE);                           \
// 		func( nChar, nRepCnt, nFlags );               \
// 		if(IsMsgHandled())                         \
//			return TRUE;                              \
// 	}

#if(_WIN32_WINNT >= 0x0501)
#define UIMSG_WM_THEMECHANGED MSG_WM_THEMECHANGED
#else
#define UIMSG_WM_THEMECHANGED(fun)
#endif

// void OnTimer(UINT_PTR nIDEvent)
#define UIMSG_WM_TIMER        MSG_WM_TIMER

#define UIMSG_WM_CONTEXTMENU  MSG_WM_CONTEXTMENU
//
// UI_WM_NOTIFY
//

// TODO: 
//	warn �����objIDû��ʹ��ȫ·��ʽ����window/panel/loginbtn��������ֱ��ʹ����loginbtn��
//       �����������ͬ��ID������ô�������Լ�ȥ�ж�
// void OnBtnClicked( );
#define UIMSG_BN_CLICKED( OBJID, func )               \
	if( uMsg == UI_WM_NOTIFY  &&                      \
		code == UI_BN_CLICKED &&                      \
		NULL != pObjMsgFrom   &&                      \
		((Object*)pObjMsgFrom)->m_strID == OBJID)     \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func( );                                      \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

#define UIMSG_BN_CLICKED2( ptr, func )                \
	if( uMsg == UI_WM_NOTIFY  &&                      \
		code == UI_BN_CLICKED &&                      \
		(Object*)pObjMsgFrom == ptr)                  \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func( );                                      \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnBtnClicked( Object* pBtnObj, POINT* pt );
#define UIMSG_BN_CLICKED_EX( OBJID, func )            \
	if( uMsg == UI_WM_NOTIFY &&                       \
		code == UI_BN_CLICKED &&                      \
        NULL != pObjMsgFrom &&                        \
		((Object*)pObjMsgFrom)->m_strID == OBJID)     \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func( (Object*)pObjMsgFrom, (POINT*)wParam ); \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnTrbnPosChanged( int nPos, int nScrollType );
#define UIMSG_TRBN_POSCHANGED( OBJID, func )          \
	if( uMsg == UI_WM_NOTIFY  &&                      \
		code == UI_TRBN_POSCHANGED &&                 \
		NULL != pObjMsgFrom &&                        \
		((ProgressCtrlBase*)pObjMsgFrom)->m_strID == OBJID) \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func( (int)wParam, (int)lParam );             \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnVScroll(int nSBCode, int nPos, Message* pMsgFrom)
#define UIMSG_WM_HSCROLL(func)                        \
	if( uMsg == WM_HSCROLL )                          \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func( (int)wParam, (int)lParam, pObjMsgFrom );\
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnVScroll(int nSBCode, int nPos, Message* pMsgFrom)
#define UIMSG_WM_VSCROLL(func)                        \
	if( uMsg == WM_VSCROLL )                          \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func( (int)wParam, (int)lParam, pObjMsgFrom );\
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnStateChanged(int nOld, int nNew);
#define UIMSG_WM_STATECHANGED(func)                   \
	if( uMsg == UI_WM_STATECHANGED )                  \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func((int)wParam, (int)lParam);               \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnGetScrollOffsetInfo(int* pxOffset, int* pyOffset);
#define UIMSG_WM_GETSCROLLOFFSETINFO(func)            \
	if(uMsg == UI_WM_GETSCROLLOFFSET)             \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func((int*)wParam, (int*)lParam);             \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

#define UIMSG_WM_SETSCROLLOFFSETINFO(func)            \
	if(uMsg == UI_WM_SETSCROLLOFFSET)             \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func((int)wParam, (int)lParam);               \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

// void OnGetScrollClientRect(RECT* prc)
#define UIMSG_WM_GETSCROLLCLIENTRECT(func)            \
	if(uMsg == UI_WM_GETSCROLLCLIENTRECT)             \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func((RECT*)wParam);                          \
		if(IsMsgHandled())                            \
			return TRUE;                              \
	}

//////////////////////////////////////////////////////////////////////////
//
//                    ATL�е���Ϣ����
//
//////////////////////////////////////////////////////////////////////////

#ifndef MESSAGE_HANDLER
#define MESSAGE_HANDLER(msg, func) \
	if(uMsg == msg) \
	{ \
		bHandled = TRUE; \
		lResult = func(uMsg, wParam, lParam, bHandled); \
		if(bHandled) \
			return TRUE; \
	}
#endif

#ifndef BEGIN_MESSAGE_MAP
#define BEGIN_MSG_MAP(theClass) \
public: \
	BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) \
	{ \
		BOOL bHandled = TRUE; \
		(hWnd); \
		(uMsg); \
		(wParam); \
		(lParam); \
		(lResult); \
		(bHandled); \
		switch(dwMsgMapID) \
			{ \
		case 0:
#endif

#ifndef ALT_MSG_MAP
#define ALT_MSG_MAP(msgMapID) \
	break; \
		case msgMapID:
#endif

#ifndef END_MSG_MAP
#define END_MSG_MAP() \
	break; \
		default: \
		break; \
		} \
		return FALSE; \
	}
#endif

}// namespace