#pragma once
#include "animatemgr.h"
namespace UI
{
    // ??
    // 1. 如何判断运动结束
    //    如果指定了时间，就按时间，否则按路程

    class IntLinearMove : public IIntLinearMove
    {
    public:
        IntLinearMove();
        ~IntLinearMove();

        virtual void SetParam1(int from, int to, int t);
        virtual void SetParam2(int from, int to, float v);
        virtual void SetParam3(int from, float v, int t);

        virtual bool OnTick(int t, int* pCurrentValue);

    public:
        int  m_from;
        int  m_to;
        float m_v;
        int  m_t;

        enum CHECK_FINISH_TYPE 
        {
            CHECK_FINISH_BY_S,
            CHECK_FINISH_BY_T,
        };
        CHECK_FINISH_TYPE m_eFinishtType;  
    };

    // Vt^2 - Vo^2 = 2as
    // s = Vot + 1/2at^2
    // a = (2s - 2Vot)/t^2
    // 匀加速或匀减速运动
    class IntAccelerateMove : public IIntAccelerateMove
    {
    public:
        IntAccelerateMove();
        ~IntAccelerateMove();

        virtual void SetParam1(int from, int to, int t, float Vo);
//        virtual void SetParam2(int from, int t, int a, int Vo, int Vt);

        virtual bool OnTick(int t, int* pCurrentValue);
    public:
        int    m_from;
        int    m_to;
        int    m_t;
        float  m_vo;
      //float  m_vt;
        float  m_a;

        enum CHECK_FINISH_TYPE 
        {
            CHECK_FINISH_BY_S,
            CHECK_FINISH_BY_T,
        };
        CHECK_FINISH_TYPE m_eFinishtType;  
    };
    
	class IntAnimateTimeline : public ITimelineImpl<IIntTimeline>
	{
	public:
		IntAnimateTimeline();
		~IntAnimateTimeline();

        IIntMoveAlgorithm*  CreateMoveAlgorithm(E_INTTIMELINE_MOVE_ALGORITHM eType);

        virtual void  GetCurrentValue(void* pOut);
        virtual void  SetOutRef(int* pRef);
		virtual void  x_OnTick();

	public:
		virtual  void Init();

	protected:
        IIntMoveAlgorithm*  m_pMoveAlgorithm;
		int     m_nCurValue;
		int*    m_pOutRef;
	};

}
