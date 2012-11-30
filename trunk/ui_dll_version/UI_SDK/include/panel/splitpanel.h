#pragma once

namespace UI
{
	class Picture;
	class Button;

	class UIAPI SplitPanel : public Panel
	{
	public:
		SplitPanel();
		~SplitPanel();

		UI_DECLARE_OBJECT( SplitPanel, OBJ_PANEL );
	public:
		UI_BEGIN_MSG_MAP
		UI_END_MSG_MAP_CHAIN_PARENT(Panel)

	protected:
		virtual bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload);

	protected:
		Picture*  m_pPicObj;      // 分隔条的背景图
		Button*   m_pBtnLeftUp;   // 快速显示/隐藏按钮（需要时才创建）
		Button*   m_pBtnRightDown;// 快速显示/隐藏按钮（需要时才创建）
	};

	// 分隔布局只接受两个元素进行排版。
	class SplitLayout : public Layout
	{
	public:
		SplitLayout();
		virtual SIZE  MeasureChildObject();
		virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false);
		virtual bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);
	public:
		LAYOUT_SPLIT_DIRECTION  m_dir;    // 排布的方向
	};
}