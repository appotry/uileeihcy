#pragma once
#include "UISDK\Kernel\Inc\Interface\icustomwindow.h"
interface ILyricCtrl;
class CLyricMgr;

class CLyricDlg : public UI::ICustomWindow
{
public:
	CLyricDlg();

	UI_BEGIN_MSG_MAP_Ixxx(CLyricDlg)
		UIMSG_WM_PAINT(OnPaint)
		UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
		UIMSG_WM_CLOSE(OnClose)
		UIMSG_BN_CLICKED(_T("switch_layered"), OnSwitchLayered)
        UIMSG_WM_SKINCHANGED(OnSkinChanged)
        UIMSG_WM_DESTROY(OnDestroy)
        UIMSG_WM_INITIALIZE(OnInitWindow)
	UI_END_MSG_MAP_CHAIN_PARENT(ICustomWindow)

public:
    // 逻辑模块通知上来的事件
    void  OnLyricLoad(IPlayerListItemInfo*  pItem, IBuffer* pBuffer);
    void  OnMp3Start(IPlayerListItemInfo*  pItem);
    void  OnMp3Stop();
    void  OnMp3ProgressInd(double dSeconds, double dPercent);

    void  SetLyricMgr(CLyricMgr* p)
    {
        m_pLyricMgr = p;
    }
protected:
    void  OnInitWindow();
    void  OnPaint(UI::IRenderTarget* pRenderTarget);
    void  OnClose();
    void  OnSwitchLayered();
    LRESULT  OnGetGraphicsRenderType() { return UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS; }
    void  OnSkinChanged();
    void  OnDestroy();

private:
    void  LoadCtrls();

private:
    CLyricMgr*   m_pLyricMgr;
    ILyricCtrl*  m_pILyricCtrl;
};

#if 0
// 色调换肤测试代码，由于架构重写原因，这部分代码可能运行不了了

UIMSG_TRBN_POSCHANGED_ID( _T("progress_hls_h"), OnHLSChanged_H )
UIMSG_TRBN_POSCHANGED_ID( _T("progress_hls_l"), OnHLSChanged_L )
UIMSG_TRBN_POSCHANGED_ID( _T("progress_hls_s"), OnHLSChanged_S )
UIMSG_WM_LBUTTONDBLCLK(OnLButtonDblClk)

void    OnHLSChanged_H( int nPos, int nScrollType );
void    OnHLSChanged_L( int nPos, int nScrollType );
void    OnHLSChanged_S( int nPos, int nScrollType );
void    OnLButtonDblClk(UINT nFlags, POINT point);

UI::ISliderCtrl* m_pH;
UI::ISliderCtrl* m_pL;
UI::ISliderCtrl* m_pS;
#endif