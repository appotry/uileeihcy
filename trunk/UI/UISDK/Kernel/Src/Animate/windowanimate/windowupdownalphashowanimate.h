#pragma  once


#include "UISDK\Kernel\Src\Animate\windowanimate\PerspectiveTransform.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowanimatebase.h"

namespace UI
{

class CustomWindow;

class WindowUpDownAlphaShowAnimate : public WindowAnimateBase
{
public:
    WindowUpDownAlphaShowAnimate();
    virtual ~WindowUpDownAlphaShowAnimate();

public:
    virtual void  OnTick(int nCount, IStoryboard** ppTimerArray);
    virtual void  Initialize();

public:
    void  SetIWindowUpDownAlphaShowAnimate(IWindowUpDownAlphaShowAnimate* p) { m_pIWindowUpDownAlphaShowAnimate = p; }
    IWindowUpDownAlphaShowAnimate*  GetIWindowUpDownAlphaShowAnimate() { return m_pIWindowUpDownAlphaShowAnimate; }

    bool  Show(int nId, int nDuration);
    bool  Hide(int nId, int nDuration);

private:
    bool  DoAction(int nId, int nDuration, bool bShow);

protected:
    IWindowUpDownAlphaShowAnimate*  m_pIWindowUpDownAlphaShowAnimate;

    int  m_nStartLine;
    int  m_nScanlineRange;

    bool  m_bShow;

    // 是否需要处理areo窗口
    bool  m_bAreo;   
    CustomWindow*  m_pCustomWindow;
};

}