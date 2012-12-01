#pragma once

namespace UI
{
//
// 	1. ESC 取消drag
//  3. panel最大拉伸限制
//  5. drag到某一位置时，会显示出窗口0,0位置的图像，很可疑，要查清楚
//
// 	2. 窗口大小改变时，保持一个panel的大小
// 	4. 初始drag位置
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

	// 分隔布局只接受两个元素进行排版。
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
		SplitDragPanel*  m_pPanelDragBar;  // 分隔条的背景图。因为要放置两个button在上面，所以这里使用PANEL，而不用Picture控件
		Button*   m_pBtnLeftUp;     // 快速显示/隐藏按钮（需要时才创建）
		Button*   m_pBtnRightDown;  // 快速显示/隐藏按钮（需要时才创建）

		LAYOUT_SPLIT_DIRECTION  m_eDirection;    // 排布的方向
		LAYOUT_SPLIT_DRAGBAR_ALIGN  m_eDragbarAlign;    // 拖拽条对齐方向 
	};
}