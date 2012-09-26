#pragma  once

//////////////////////////////////////////////////////////////////////////
//
//                             滚动条控件
//
//////////////////////////////////////////////////////////////////////////

namespace UI
{
	class HScrollBar;
	class VScrollBar;
	class IScrollBarRender;
	class ScrollBarBase;

#define UISIF_RANGE           0x0001
#define UISIF_PAGE            0x0002
#define UISIF_POS             0x0004
#define UISIF_DISABLENOSCROLL 0x0008
#define UISIF_BUTTONLINE      0x0010
#define UISIF_WHEELLINE       0x0020
#define UISIF_ALL            (UISIF_RANGE | UISIF_PAGE | UISIF_POS | UISIF_KEYBOARDLINE | UISIF_WHEELLINE)

	typedef struct tagUISCROLLINFO
	{
		tagUISCROLLINFO(){ nMask = nRange = nPage = nPos = nButtonLine = nWheelLine = 0; }
		int  nMask;
		int  nRange;
		int  nPos;
		int  nPage;
		int  nButtonLine;
		int  nWheelLine;
	}UISCROLLINFO, *LPUISCROLLINFO;

	class UIAPI ScrollBarMgr : public Message
	{
	public:
		ScrollBarMgr();
		~ScrollBarMgr();

	public:
		virtual void   ResetAttribute();
		virtual bool   SetAttribute(ATTRMAP& mapAttrib, bool bReload);
		virtual BOOL   ProcessMessage(UIMSG* pMsg, int nMsgMapID=0);

	public:
		void     SetBindObject(Object* pBindObj);
		void     SetScrollBarVisibleType(
					SCROLLBAR_DIRECTION_TYPE eDirType, 
					SCROLLBAR_VISIBLE_TYPE eVisType );

		Object*  GetBindObject(){ return m_pBindObject; }
		SCROLLBAR_VISIBLE_TYPE GetScrollBarVisibleType(SCROLLBAR_DIRECTION_TYPE eType);

//		UINT     OnHitTest( POINT* pt );
		void     UpdateBindObjectNonClientRect();

		void     GetScrollPos(int* pnxOffset, int* pnyOffset);
		int      GetHScrollPos();
		int      GetVScrollPos();
		void     SetScrollPos(int nxPos, int nyPos);
		void     SetHScrollPos(int nX);
		void     SetVScrollPos(int nY);
		
		bool     SetScrollRange(int nX, int nY);
		void     SetHScrollRange(int nX);
		void     SetVScrollRange(int nY);
		int      GetHScrollRange();
		int      GetVScrollRange();
		
		void     SetVScrollLine(int nLine);
		void     SetHScrollLine(int nLine);

		bool     SetScrollPage(int nxPage, int nyPage);
		void     SetVScrollPage(int nPage);
		void     SetHScrollPage(int nPage);
		int      GetVScrollPage();
		int      GetHScrollPage();

		void     ScrollToBottom();
		void     ScrollToRightMost();

		HScrollBar*    GetHScrollBar() { return m_pHScrollBar; }
		VScrollBar*    GetVScrollBar() { return m_pVScrollBar; }
		void     OnScrollBarRelease(ScrollBarBase* p);

	protected:
		Object*        m_pBindObject;
		HScrollBar*    m_pHScrollBar;
		VScrollBar*    m_pVScrollBar;

		SCROLLBAR_VISIBLE_TYPE  m_evScrollbarVisibleType;
		SCROLLBAR_VISIBLE_TYPE  m_ehScrollbarVisibleType;
	};


	class IScrollBarRender : public Message
	{
	public:
		virtual ~IScrollBarRender() = 0{} ;
		IScrollBarRender(ScrollBarBase* p){ m_pScrollBar = p; }
		virtual SCROLLBAR_DIRECTION_TYPE GetScrollBarDirType() = 0;
		virtual void  ResetAttribute() = 0;
		virtual bool  SetAttribute(ATTRMAP& mapAttrib) = 0;
		virtual SIZE  GetAutoSize() = 0;
		virtual void  UpdateScrollBarVisible() = 0;

	protected:
		ScrollBarBase*   m_pScrollBar;
	};


	class UIAPI ScrollBarBase : public Control
	{
	public:
		ScrollBarBase();
		~ScrollBarBase();

		UI_BEGIN_MSG_MAP
			UI_BEGIN_CHAIN_ALL_MSG_MAP
				UICHAIN_MSG_MAP_POINT_MEMBER(m_pScrollBarRender)
			UI_END_CHAIN_ALL_MSG_MAP
		UI_END_MSG_MAP_CHAIN_PARENT(Control)

	protected:
		
	public:
		virtual bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload );   // 从Object继承到的
		virtual void  ResetAttribute();
		virtual SIZE  GetAutoSize(HRDC hRDC);

		void    Init(ScrollBarMgr* pMgr);
		int     GetScrollPos();
		void    SetScrollPage(int nPage);
		void    SetScrollRange(int nRange);

	public:
		int     GetScrollRange();
		int     GetScrollPage();

		void    ScrollLineDownRight();
		void    ScrollLineUpLeft();
		void    ScrollWheelLineUp();
		void    ScrollWheelLineDown();
		void    ScrollPageDownRight();
		void    ScrollPageUpLeft();
		void    ScrollToEnd();
		void    ScrollToBegin();

		bool    SetScrollInfo(LPUISCROLLINFO lpsi, bool bUpdate=true);
		bool    SetScrollPos(int nPos/*, bool bUpdate=true*/);
		void    SetScrollButtonLine(int nLine);
		void    SetScrollWheelLine(int nLine);

		void    FireScrollMessage(int nSBCode, int nThackPos=0);

		SCROLLBAR_DIRECTION_TYPE GetScrollBarDirection() { return m_eScrollDirection; }
		ScrollBarMgr*  GetScrollMgr() { return m_pScrollBarMgr; }
		Object*        GetBindObject(){ return m_pScrollBarMgr->GetBindObject(); }

	protected:
		ScrollBarMgr*            m_pScrollBarMgr;
		SCROLLBAR_DIRECTION_TYPE m_eScrollDirection;
		IScrollBarRender*        m_pScrollBarRender;

	protected:
		int    m_nRange;
		int    m_nPos;
		int    m_nPage;
		int    m_nButtonLine;
		int    m_nWheelLine;
	};


	class UIAPI HScrollBar : public ScrollBarBase
	{
	public:
		HScrollBar();
		~HScrollBar();
		UI_DECLARE_OBJECT(HScrollBar, OBJ_CONTROL)

		virtual void  ResetAttribute();
	};

	class UIAPI VScrollBar : public ScrollBarBase
	{
	public:
		VScrollBar();
		~VScrollBar();
		UI_DECLARE_OBJECT(VScrollBar, OBJ_CONTROL)

		virtual void  ResetAttribute();
	};
}