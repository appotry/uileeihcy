#pragma once
#include "animatemgr.h"
namespace UI
{

// ʲôҲ������ֻ��Ϊ��ά�ֶ�����ʱ��
class NoneAnimateTimeline : public ITimelineImpl<INoneTimeline>
{
public:
    virtual void  GetCurrentValue(void* ppOut){}
    virtual void  x_OnTick(){}
    virtual void  Init(){}
};

}