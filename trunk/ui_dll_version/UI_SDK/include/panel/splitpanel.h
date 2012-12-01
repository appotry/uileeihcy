#pragma once

namespace UI
{
//
// 	1. ESC ȡ��drag
//  3. panel�����������
//  5. drag��ĳһλ��ʱ������ʾ������0,0λ�õ�ͼ�񣬺ܿ��ɣ�Ҫ�����
//
// 	2. ���ڴ�С�ı�ʱ������һ��panel�Ĵ�С
// 	4. ��ʼdragλ��
//
	class Button;
	class SplitLayout;

	class SplitDragPanel : public Panel
	{
	public:
		SplitDragPanel();

		UI_DECLARE_OBJECT( SplitDragPanel, OBJ_PANEL ) 

		UI_BEGIN_MSG_MAP
			UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
			UIMSG_WM_LBUTTONUP(OnLButtonUp)
			UIMSG_WM_MOUSEMOVE(OnMouseMove)
			UIMSG_WM_CANCELMODE(OnCancelMode)
			UIMSG_WM_KEYDOWN(OnKeyDown)
		UI_END_MSG_MAP_CHAIN_PARENT(Panel)

		void   SetSplitLayout(SplitLayout* pLayout);
		bool   IsDraging() { return m_bButtonDown; }
	protected:
		void   OnLButtonDown(UINT nFlags, POINT point);
		void   OnLButtonUp(UINT nFlags, POINT point);
		void   OnCancelMode();
		void   OnMouseMove(UINT nFlags, POINT point);
		void   OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	protected:
		SplitLayout*  m_pLayout;
		bool   m_bButtonDown;

		POINT  m_ptLButtonDown;
		POINT  m_ptLastMouseMove;
	};

	// �ָ�����ֻ��������Ԫ�ؽ����Ű档
	class SplitLayout : public Layout
	{
	public:
		SplitLayout();
		~SplitLayout();

		virtual SIZE  MeasureChildObject();
		virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false);
		virtual bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);
	public:

		void    OnDragBegin(POINT point);
		void    OnDraging(POINT point);
		void    OnDragEnd(POINT point);

	protected:
		SplitDragPanel*  m_pPanelDragBar;  // �ָ����ı���ͼ����ΪҪ��������button�����棬��������ʹ��PANEL��������Picture�ؼ�
		Button*   m_pBtnLeftUp;     // ������ʾ/���ذ�ť����Ҫʱ�Ŵ�����
		Button*   m_pBtnRightDown;  // ������ʾ/���ذ�ť����Ҫʱ�Ŵ�����

		LAYOUT_SPLIT_DIRECTION  m_eDirection;    // �Ų��ķ���
		LAYOUT_SPLIT_DRAGBAR_ALIGN  m_eDragbarAlign;    // ��ק�����뷽�� 
	};
}