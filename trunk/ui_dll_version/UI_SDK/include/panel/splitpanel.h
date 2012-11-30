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
		Picture*  m_pPicObj;      // �ָ����ı���ͼ
		Button*   m_pBtnLeftUp;   // ������ʾ/���ذ�ť����Ҫʱ�Ŵ�����
		Button*   m_pBtnRightDown;// ������ʾ/���ذ�ť����Ҫʱ�Ŵ�����
	};

	// �ָ�����ֻ��������Ԫ�ؽ����Ű档
	class SplitLayout : public Layout
	{
	public:
		SplitLayout();
		virtual SIZE  MeasureChildObject();
		virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false);
		virtual bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);
	public:
		LAYOUT_SPLIT_DIRECTION  m_dir;    // �Ų��ķ���
	};
}