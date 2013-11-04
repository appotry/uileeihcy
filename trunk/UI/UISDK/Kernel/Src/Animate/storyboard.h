#pragma once
#include "UISDK\Kernel\Inc\Interface\ianimate.h"

namespace UI
{
interface ITimeline;
class  AnimateManager;

typedef list<ITimeline*>  TimelineList;
typedef list<ITimeline*>::iterator  TimelineIter;


// 用于管理一个动画中的几个timeline 
class Storyboard
{
public:
    Storyboard();
    ~Storyboard();

    bool  AddTimeline(ITimeline* p);
    bool  DestroyTimeline(int nTimelineId);
    ITimeline*  FindTimeline(int nTimelineId);
    ITimeline*  CreateTimeline(E_TIMELINE_VALUE_TYPE eType, int nTimelineId, int nMoveAlgo, IMoveAlgorithm** ppMoveAlgo);

    IMessage*  GetNotifyObj();
    void  SetNotifyObj(IMessage* pNotify);
    void  SetId(int nID) { m_nID = nID; }
    int   GetId() { return m_nID; }
    void    SetWParam(WPARAM wParam) { m_pWParam = wParam; }
    WPARAM  GetWParam() { return m_pWParam; }
    void    SetLParam(LPARAM lParam) { m_pLParam = lParam; }
    LPARAM  GetLParam() { return m_pLParam; }

    void  Begin();
    void  End();
    
    void  SetIStoryboard(IStoryboard* p) { m_pIStoryboard = p; };
    IStoryboard*  GetIStoryboard() { return m_pIStoryboard; }

public:
    void  SetAnimateMgr(AnimateManager*  p);
    void  OnAnimateStart();
    void  SetFinishFlag();
    bool  IsFinish();
    void  OnTick(bool* pbFinish);

protected:
    void  DestroyAllTimeline();
    
protected:
    AnimateManager*    m_pAnimateMgr;
    IStoryboard*       m_pIStoryboard;
    IMessage*          m_pNotify;
    int                m_nID;
    WPARAM             m_pWParam;
    LPARAM             m_pLParam;

    list<ITimeline*>   m_listTimeline;
};

}