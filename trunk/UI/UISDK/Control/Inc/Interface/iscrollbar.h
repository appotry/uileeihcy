#ifndef ISCROLLBAR_H_67A3D018_3031_499a_9925_B5FB47CF3691
#define ISCROLLBAR_H_67A3D018_3031_499a_9925_B5FB47CF3691
#include "UISDK\Kernel\Inc\Interface\icontrol.h"


namespace UI
{

interface IScrollBarManager;
class SystemScrollBar;
interface ISystemScrollBar : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(ISystemScrollBar, SystemScrollBar);

    void  SetIScrollBarMgr(IScrollBarManager* p);
};


class SystemHScrollBar;
interface ISystemHScrollBar : public ISystemScrollBar
{
    UI_DECLARE_Ixxx_INTERFACE(ISystemHScrollBar, SystemHScrollBar);
};
class SystemVScrollBar;
interface ISystemVScrollBar : public ISystemScrollBar
{
    UI_DECLARE_Ixxx_INTERFACE(ISystemVScrollBar, SystemVScrollBar);
};

}

#endif // ISCROLLBAR_H_67A3D018_3031_499a_9925_B5FB47CF3691