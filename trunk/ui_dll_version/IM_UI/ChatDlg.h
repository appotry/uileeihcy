#pragma once

class CChatDlg : public CustomWindow
{
public:
	CChatDlg(void);
	~CChatDlg(void);

	UI_BEGIN_MSG_MAP
//		UIMSG_WM_GETGRAPHICSRENDERTYPE(OnGetGraphicsRenderType)
		UIMSG_BN_CLICKED2(m_pChangeBkImgBtn, OnBnClickChangeBkImg)
	UI_END_MSG_MAP_CHAIN_PARENT(CustomWindow)

//	LRESULT OnGetGraphicsRenderType(){ return GRAPHICS_RENDER_TYPE_GDIPLUS; }
	virtual void OnInitWindow();
	void  OnBnClickChangeBkImg();

private:
	Button*   m_pChangeBkImgBtn;
};
