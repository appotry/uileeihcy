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

		// ÿ�δ�С�����֡����뷽ʽ�ı����Ҫ���¼�����������ֵ
		bool          m_bNeedCalcRect;
		CRect         m_rcBorder;  // ����groupbox�ĸ��߿������.top һ�������ֵ�һ��߶ȣ�
		CRect         m_rcText; 
		CRect         m_rcClip;   // ����CLIENT,ʹ��ʱ��Ҫת���ɻ���window
	};
}