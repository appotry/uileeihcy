#pragma once
namespace UI
{
	// ����ؼ�(����ť�Ĺ�����)
	class UIAPI SliderCtrlBase : public ProgressCtrlBase
	{
	public:
		SliderCtrlBase();

		UI_BEGIN_MSG_MAP
			UIMSG_WM_PAINT(OnPaint)
			UIMSG_WM_SIZE(OnSize)
			UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
			//		UIMSG_WM_SETCURSOR(OnSetCursor)
			UIMSG_WM_KEYDOWN(OnKeyDown)
			UIMSG_WM_KEYUP(OnKeyUp)

			// ��ť���¼�
			UIALT_MSG_MAP(1)   
			UIMSG_WM_LBUTTONDOWN(OnBtnLButtonDown)	
			UIMSG_WM_MOUSEMOVE(OnBtnMouseMove)	
			UIMSG_WM_SETCURSOR(OnSetCursor)
			UIMSG_WM_LBUTTONUP(OnBtnLButtonUp)
			UIMSG_WM_KEYDOWN(OnKeyDown)
			UIMSG_WM_KEYUP(OnKeyUp)

		UI_END_MSG_MAP_CHAIN_PARENT(ProgressCtrlBase)

	public:
		virtual       void ResetAttribute();
		virtual       bool SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);

		virtual      int    SetPos(int nPos, bool bUpdate=true);
		virtual      int    SetPage(int nPage, bool bUpdate=true);
		virtual      void   SetRange(int nLower, int nUpper, bool bUpdate=true);
		virtual      void   SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

		void         UpdateButtonRect();

	protected:
		void         OnPaint( HRDC hDC );
		void		 OnLButtonDown(UINT nFlags, POINT point);
		void		 OnBtnLButtonDown(UINT nFlags, POINT point);
		void		 OnBtnMouseMove(UINT nFlags, POINT point);
		void         OnBtnLButtonUp(UINT nFlags, POINT point);
		void         OnMove( POINT pt );
		void         OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
		void         OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
		void         OnSize( UINT nType, int cx, int cy );

		void         UpdateUIData(bool bCalBeginEnd=false, bool bUpdateButtonRect=true);
		void         CalcCurPosByUITrackPos(int nWindowPos);

		int          WindowPoint2UITrackPos(const POINT* ptWindow);
	protected:
		// ������ʾ����
		int          m_nBegin;     // ��Ӧm_nMin����������ʼ���Ƶ����꣬PADDING + half of button size
		int          m_nEnd;       // ��Ӧm_nMax���������������Ƶĵط���Control Size - ( PADDING + half of button size )
		int          m_nTrack;     // ��Ӧm_nCur����ǰ��ť���ڵ����꣬middle of button

		// show
		ButtonBase*    m_pButton;     // ��ק�õİ�ť�ؼ������ฺ��ʵ����
		int            m_nDiffFix;    // ����������϶�ʱ�������core m_nTrack��ƫ��
	};

	class UIAPI SliderCtrl : public SliderCtrlBase
	{
	public:
		SliderCtrl();
		~SliderCtrl();

		UI_DECLARE_OBJECT( SliderCtrl, OBJ_CONTROL )

	};
}