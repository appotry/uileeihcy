#pragma once
#include "App\Player\Player_Ctrls\Inc\imultilabel.h"
namespace UI
{
    interface IStoryboard;
}

class MultiLabel : public UI::MessageProxy
{
public:
    MultiLabel();
    ~MultiLabel();

    UI_DECLARE_OBJECT3(MultiLabel, (UI::OBJ_CONTROL|UI::CONTROL_LABEL), _T("TTPlayerCtrl"));

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_TIMER(OnTimer)
        UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(OnGetGraphicsRenderType)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(MultiLabel, UI::IControl)

    void   SetIMultiLabel(IMultiLabel* p) { m_pIMultiLabel = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }
    IMultiLabel*  GetIMultiLabel() { return m_pIMultiLabel; }

public:
    void  SetContent(const TCHAR* szContent, const TCHAR szSeparator=_T('\n'));
    void  ShowNextLabelNow();
    void  ShowPrevLabelNow();

    void  StartAnimateSwitch(int nTime);
    void  StopAnimateSwitch();

protected:
    void  OnPaint(UI::IRenderTarget* pRenderTarget);
    void  SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload);
    void  ResetAttribute();
    void  OnLButtonUp(UINT nFlags, POINT point);
    UI::GRAPHICS_RENDER_LIBRARY_TYPE  OnGetGraphicsRenderType() { return UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS; }  // 渐变消失，需要带alpha的文字绘制
    void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam);
    void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray);

private:
    const TCHAR*  GetCurShowText();
    const TCHAR*  GetNextShowText();
    const TCHAR*  GetPrevShowText();

    void  OnClicked(POINT* pt);
private:
    IMultiLabel*  m_pIMultiLabel;
    
    UINT_PTR  m_nSwitchTimerId;

    vector<String>  m_vecContent;
    int  m_nCurShowIndex;
    int  m_nTextAlpha;
    UI::Color*  m_pColor;
    UI::IRenderFont*   m_pRenderFont;
};