#pragma once

namespace UI
{
	class UIAPI GroupBox : public Panel
	{
	public:
		GroupBox();
		~GroupBox();

		UI_DECLARE_OBJECT( GroupBox, OBJ_PANEL );

	public:
		UI_BEGIN_MSG_MAP
			UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
			UIMSG_WM_SIZE(OnSize)
		UI_END_MSG_MAP_CHAIN_PARENT(Panel)

		void   OnEraseBkgnd( HRDC hRDC );

		virtual void ResetAttribute();
		virtual bool SetAttribute( map<String,String>& mapAttrib, bool bReload );

		void    GetBorderRect(RECT* prc);

	protected:
		void    CalcBorderRect();
		void    OnSize( UINT nType, int cx, int cy );

	private:
		String      m_strText;

		IRenderBitmap* m_pBorderBitmap;
		Image9Region   m_9RegionBorderBitmap;

		// 每次大小、文字、对齐方式改变后，需要重新计算以下两个值
		bool          m_bNeedCalcRect;
		CRect         m_rcBorder;  // 绘制groupbox四个边框的区域（.top 一般是文字的一半高度）
		CRect         m_rcText; 
		CRect         m_rcClip;   // 基于CLIENT,使用时，要转换成基于window
	};
}