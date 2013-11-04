#pragma once
#include "animatemgr.h"
namespace UI
{

// 什么也不做，只是为了维持动画计时器
class NoneAnimateTimeline : public ITimelineImpl<INoneTimeline>
{
public:
    virtual void  GetCurrentValue(void* ppOut){}
    virtual void  x_OnTick(){}
    virtual void  Init(){}
};

}