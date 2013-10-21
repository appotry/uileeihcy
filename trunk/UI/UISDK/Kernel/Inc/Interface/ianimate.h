#pragma once
namespace UI
{
	//////////////////////////////////////////////////////////////////////////
	//
	//     动画接口
	//
	//////////////////////////////////////////////////////////////////////////
    enum E_TIMELINE_VALUE_TYPE
    {
        TLV_NONE,
        TLV_INT,
        TLV_RECT,
    };

    enum TIMELINE_TIME_TYPE
    {
        TLT_BY_MS,
        TLT_BY_SECOND,
        TLT_BY_FRAME,
    };

	interface ITimeline : public IRootInterface
	{
		virtual void     Destroy() = 0;
//		virtual void     SetDuration(int n) = 0;
		virtual void     SetRepeateCount(int n) = 0;
        virtual void     SetTimeType(TIMELINE_TIME_TYPE eType) = 0;
		virtual bool     IsFinish() = 0;
		virtual void     GetCurrentValue(void* ppOut) = 0;
		virtual void     SetId(int nID) = 0;
		virtual int      GetId() = 0;
		virtual void     SetUserData(LPARAM lParam) = 0;
		virtual LPARAM   GetUserData() = 0;
		

		// 只能内部使用的接口，外面不要调用
		virtual void     x_SetFinishFlag() = 0;
		virtual void     x_OnAnimateStart() = 0;  
		virtual void     x_OnTick() = 0;
	};



    enum E_INTTIMELINE_MOVE_ALGORITHM
    {
        ITMA_Linear,
        ITMA_Accelerate,
    };
    interface IMoveAlgorithm 
    {
        virtual ~IMoveAlgorithm() = 0 {};
    };

    interface IIntMoveAlgorithm : public IMoveAlgorithm
    {
        // t表示从动画开始到现在经过的时间(frame/ms/s)
        virtual bool OnTick(int t, int* pCurrentValue) = 0;
    };

    // S = VT
    interface IIntLinearMove : public IIntMoveAlgorithm
    {
        virtual void SetParam1(int from, int to, int t) = 0;
        virtual void SetParam2(int from, int to, float v) = 0;
        virtual void SetParam3(int from, float v, int t) = 0;
    };

    // Vt^2 - Vo^2 = 2as
    // s = Vot + 1/2at^2
    // 匀加速或匀减速运动
    interface IIntAccelerateMove : public IIntMoveAlgorithm
    {
        virtual void SetParam1(int from, int to, int t, float Vo) = 0;
        //virtual void SetParam2(int from, int t, int a, int Vo, int Vt) = 0;
    };

	interface IIntTimeline : public ITimeline
	{
		virtual void   SetOutRef(int* pRef) = 0;
	};

    interface INoneTimeline : public ITimeline
    {

    };

	interface IRectTimeline : public ITimeline
	{
// 		virtual void   SetFromTo(LPRECT lprcFrom, LPRECT lprcTo) = 0;
// 		virtual void   SetFromBy(LPRECT lprcFrom, LPRECT lprcBy) = 0;
	};


    class Storyboard;
    interface UISDKAPI IStoryboard : public IRootInterface
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IStoryboard, Storyboard);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IStoryboard, Storyboard);

        IMessage*  GetNotifyObj();
        void  SetNotifyObj(IMessage* pNotify);
        void  SetId(int nID);
        int   GetId();
        void    SetWParam(WPARAM wParam);
        WPARAM  GetWParam();
        void    SetLParam(LPARAM lParam);
        LPARAM  GetLParam();

        ITimeline*  CreateTimeline(E_TIMELINE_VALUE_TYPE eType, int nTimelineId, int nMoveAlgo, IMoveAlgorithm** ppMoveAlgo);
        ITimeline*  FindTimeline(int nTimelineId);
//        bool  AddTimeline(ITimeline* p);
//        bool  DestroyTimeline(int nTimelineId);

        bool  IsFinish();
        void  Begin();
        void  End();
    };

    class WindowAnimateBase;
    interface UISDKAPI IWindowAnimateBase : public IStoryboard
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IWindowAnimateBase, WindowAnimateBase);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IWindowAnimateBase, WindowAnimateBase);

        void   UpdateWindowSourceImage();
    };

    // 窗口3D旋转动画
    class Window3DAnimate;
	interface UISDKAPI IWindow3DAnimate : public IWindowAnimateBase
	{
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IWindow3DAnimate, Window3DAnimate);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IWindow3DAnimate, Window3DAnimate);

		int*   GetRotateXPtr();
		int*   GetRotateYPtr();
		int*   GetRotateZPtr();

		int*   GetOffsetXPtr();
		int*   GetOffsetYPtr();
		int*   GetOffsetZPtr();
	};

    // 窗口从上到下渐变显示/隐藏动画
    class WindowUpDownAlphaShowAnimate;
    interface UISDKAPI IWindowUpDownAlphaShowAnimate : public IWindowAnimateBase
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IWindowUpDownAlphaShowAnimate, WindowUpDownAlphaShowAnimate);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IWindowUpDownAlphaShowAnimate, WindowUpDownAlphaShowAnimate);

        bool  Show(int nId, int nDuration);
        bool  Hide(int nId, int nDuration);
    };

    // 窗口渐显渐隐动画 

    class WindowAlphaShowAnimate;
    interface UISDKAPI IWindowAlphaShowAnimate : public IWindowAnimateBase
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IWindowAlphaShowAnimate, WindowAlphaShowAnimate);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IWindowAlphaShowAnimate, WindowAlphaShowAnimate);

        bool  Show(int nId, int nDuration);
        bool  Hide(int nId, int nDuration);
    };


    class ControlAnimateBase;
    interface UISDKAPI IControlAnimateBase : public IStoryboard
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IControlAnimateBase, ControlAnimateBase);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IControlAnimateBase, ControlAnimateBase);
    };

    enum SLIDE_ANIMATE_FLAG
    {
        SLIDE_LEFT2RIGHT,
        SLIDE_TOP2BOTTOM,
        SLIDE_RIGHT2LEFT,
        SLIDE_BOTTOM2TOP,
    };

    // 两个控件滑动
    class SlideAnimate;
    interface UISDKAPI ISlideAnimate : public IControlAnimateBase
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(ISlideAnimate, SlideAnimate);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(ISlideAnimate, SlideAnimate);

        bool  Slide(IObject* pObj1Left, IObject* pObj2Right, CRect* prcCommitInWindow, int nFlags);
    };

	enum E_WINDOW_ANIMATE_TYPE
	{
		E_WINDOW_ANIMATE_3D_ROTATE,
        E_WINDOW_ANIMATE_UPDOWNALPHASHOW,
        E_WINDOW_ANIMATE_ALPHASHOW,
	};
    enum E_CONTROL_ANIMATE_TYPE
    {
        E_CONTROL_ANIMATE_SLIDE,
    };

    class AnimateManager;
	interface UISDKAPI IAnimateManager : public IRootInterface
	{
        IAnimateManager(AnimateManager* p);
        AnimateManager*  GetImpl();

        int   SetFps(int n);

        // void  AddStoryboard(IStoryboard* p); -- 不提供该接口，应该是由Storyboard::Begin内部调用

        void  ClearStoryboardOfNotify(IMessage* pMsg);
        void  RemoveStoryboard(IStoryboard* p);

        IStoryboard*  CreateStoryboard();
        IWindowAnimateBase*  CreateWindowAnimateInstance(E_WINDOW_ANIMATE_TYPE eType, IWindowBase* pWindow);
        IControlAnimateBase*  CreateControlAnimateInstance(E_CONTROL_ANIMATE_TYPE eType);

    private:
        AnimateManager*  m_pImpl;
	}; 
}