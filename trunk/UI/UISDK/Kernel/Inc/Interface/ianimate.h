#pragma once
namespace UI
{
	//////////////////////////////////////////////////////////////////////////
	//
	//     动画接口
	//
	//////////////////////////////////////////////////////////////////////////

	interface ITimeline : public IRootInterface
	{
		virtual void     Destroy() = 0;
		virtual void     SetDuration(int n) = 0;
		virtual void     SetRepeateCount(int n) = 0;
		virtual bool     IsFinish() = 0;
		virtual void     GetCurrentValue(void* ppOut) = 0;
		virtual void     SetID(int nID) = 0;
		virtual int      GetID() = 0;
		virtual void     SetUserData(LPARAM lParam) = 0;
		virtual LPARAM   GetUserData() = 0;
		

		// 只能内部使用的接口，外面不要调用
		virtual void     x_SetFinishFlag() = 0;
		virtual void     x_OnAnimateStart() = 0;  
		virtual void     x_OnTick(bool* pbFinish) = 0;
	};

	interface IIntTimeline : public ITimeline
	{
		virtual void   SetFromTo(int nFrom, int nTo) = 0;
		virtual void   SetFromBy(int nFrom, double fSpeed) = 0;
		virtual void   SetOutRef(int* pRef) = 0;
		virtual void   SetSpeed(double fSpeed) = 0;
		virtual void   SetAcceleration(double fa) = 0;
	};

	interface IRectTimeline : public ITimeline
	{
		virtual void   SetFromTo(LPRECT lprcFrom, LPRECT lprcTo) = 0;
		virtual void   SetFromBy(LPRECT lprcFrom, LPRECT lprcBy) = 0;
	};


    class Storyboard;
    interface UISDKAPI IStoryboard : public IRootInterface
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IStoryboard, Storyboard);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IStoryboard, Storyboard);

        IMessage*  GetNotifyObj();
        void  SetNotifyObj(IMessage* pNotify);
        void  SetID(int nID);
        int   GetID();
        void    SetUserData(LPARAM lParam);
        LPARAM  GetUserData();

        bool  AddTimeline(ITimeline* p);
        bool  DestroyTimeline(int nTimelineId);
        ITimeline*  FindTimeline(int nTimelineId);
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


    // 两个控件滑动
    class ControlAnimateBase;
    interface UISDKAPI IControlAnimateBase : public IStoryboard
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(IControlAnimateBase, ControlAnimateBase);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(IControlAnimateBase, ControlAnimateBase);
    };

    class SlideAnimate;
    interface UISDKAPI ISlideAnimate : public IControlAnimateBase
    {
        UI_DECLARE_Ixxx_INTERFACE_Construct2(ISlideAnimate, SlideAnimate);
        UI_DECLARE_Ixxx_INTERFACE_CreateImpl(ISlideAnimate, SlideAnimate);
    };

	enum E_TIMELINE_TYPE
	{
		E_TIMELINE_INT,
		E_TIMELINE_RECT,
	};
	enum E_WINDOW_ANIMATE_TYPE
	{
		E_WINDOW_ANIMATE_3D_ROTATE,
        E_WINDOW_ANIMATE_UPDOWNALPHASHOW,
        E_WINDOW_ANIMATE_ALPHASHOW,
	};

    class AnimateManager;
	interface UISDKAPI IAnimateManager : public IRootInterface
	{
        IAnimateManager(AnimateManager* p);
        AnimateManager*  GetImpl();

        void  SetFps(int n);

        void  AddStoryboard(IStoryboard* p);
        void  ClearStoryboardOfNotify(IMessage* pMsg);
        void  RemoveStoryboard(IStoryboard* p);

        ITimeline*    CreateTimelineInstance(E_TIMELINE_TYPE eType);
        IStoryboard*  CreateStoryboard();
        IWindowAnimateBase*  CreateWindowAnimateInstance(E_WINDOW_ANIMATE_TYPE eType, IWindowBase* pWindow);

    private:
        AnimateManager*  m_pImpl;
	}; 
}