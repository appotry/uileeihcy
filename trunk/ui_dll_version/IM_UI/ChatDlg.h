#pragma once

class CChatDlg : public CustomWindow
{
public:
	CChatDlg(void);
	~CChatDlg(void);

	UI_BEGIN_MSG_MAP
//		UIMSG_WM_GETGRAPHICSRENDERTYPE(OnGetGraphicsRenderType)
		UIMSG_BN_CLICKED2(m_pChangeBkImgBtn, OnBnClickChangeBkImg)
		UIMSG_TRBN_POSCHANGED(m_pAlphaSlider, OnAlphaChanged)
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

//	LRESULT OnGetGraphicsRenderType(){ return GRAPHICS_RENDER_TYPE_GDIPLUS; }
	virtual void OnInitWindow();
	void  OnBnClickChangeBkImg();
	void  OnAlphaChanged(int nPos, int nScrollType);

private:
	Button*   m_pChangeBkImgBtn;
	SliderCtrl*  m_pAlphaSlider;
};
