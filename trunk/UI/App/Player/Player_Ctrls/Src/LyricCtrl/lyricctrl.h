#pragma once
#include "App\Player\Player_Ctrls\Inc\ilyricctrl.h"
#include "lyricanalyze.h"

class LyricCtrl : public UI::MessageProxy
{
public:
    LyricCtrl();
    ~LyricCtrl();

    UI_DECLARE_OBJECT3(LyricCtrl, UI::OBJ_CONTROL, _T("TTPlayerCtrl"));

    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(LyricCtrl, UI::IControl)

    void   SetILyricCtrl(ILyricCtrl* p) { m_pILyricCtrl = p; SetIMessageProxy(static_cast<UI::IMessage*>(p)); }
    ILyricCtrl*  GetILyricCtrl() { return m_pILyricCtrl; }

public:
    bool  LoadLyric(const TCHAR*  szBuffer);
    void  ShowLyric(bool bShow);

    void  Start();
    void  Pause();
    void  Stop();
    void  OnProgressInd(int ms);

protected:
    void  OnPaint(UI::IRenderTarget* pRenderTarget);
    void  SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload);
    void  ResetAttribute();

private:
    void  CalcCurItemByTime(int ms);
    int   _CalcCurItemByTime(int nCur, int ms, bool bForward=true);
private:
    ILyricCtrl*  m_pILyricCtrl;
    CLyricItemData**  m_pLyricItemArray;  // 将m_lyricData中的map数组转换成一个array数组，便于快速访问
    int  m_nCurItemIndex;

    int  m_nLyricLineArraySize;
    int  m_nItemHeight;
    int  m_nVScroll;

    CLyricData  m_lyricData;

    UI::Color*  m_pColor;
    UI::IRenderFont*   m_pRenderFont;
};