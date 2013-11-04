#ifndef ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E
#define ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E

#include "UISDK\Kernel/Inc/Interface/ianimate.h"

namespace UI
{
	// http://msdn.microsoft.com/zh-cn/magazine/cc163397.aspx  扩展 WPF 动画类
	// http://www.cnblogs.com/Jax/archive/2008/03/26/1123778.html 《Programming WPF》翻译 第8章 动画

	// http://msdn.microsoft.com/en-us/library/system.windows.media.animation.doubleanimationbase_members(v=vs.85).aspx  DoubleAnimationBase Members

	// 了解什么动画可以或不可以实现的关键是，理解它的聚焦属性的天性
	
// 	Inheritance Hierarchy (WPF 类继承列表)
// 		--------------------------------------------------------------------------------
// 
// 		System.Object
// 		System.Windows.Threading.DispatcherObject
// 		System.Windows.DependencyObject
// 		System.Windows.Freezable
// 		System.Windows.Media.Animation.Animatable
// 		System.Windows.Media.Animation.Timeline
// 		System.Windows.Media.Animation.AnimationTimeline
// 		System.Windows.Media.Animation.DoubleAnimationBase
// 		System.Windows.Media.Animation.DoubleAnimation
// 		System.Windows.Media.Animation.DoubleAnimationUsingKeyFrames
// 		System.Windows.Media.Animation.DoubleAnimationUsingPath

	// 所谓的Delegate其是就回回调接口。在MVC中的 M->C 的通讯方式就是这么实现的
	// M就会调用回调接口，然后C必须实现这个回调接口，这样MC就能通讯了，而且M也不
	// 用了解C的任何信息.
	// Delegate是一对一的，而Notification是一对多的广播消息，Target/Action是将硬件消息
	// （鼠标点击，键盘按钮之类）转换成程序消息。
	
	// Target-Action?

	// http://gamebabyrocksun.blog.163.com/blog/static/571534632010219544464/  游戏引擎循环机制
	
	// 仿WPF动画原理
	class ObjectStoryboard  // 一个对象可能同时添加多个Storyboard。为了实现给该对象一次tick只发送一个通知，将该对象所有的Storyboard串起来管理
	{
	public:
		ObjectStoryboard(IMessage* p);
		~ObjectStoryboard();

		void  AddStoryboard(IStoryboard* p);
		void  RemoveStoryboard(IStoryboard* p);
		void  CheckFinishFlag();
		int   FindStoryboard(int nID);

		IMessage*      m_pNotifyObject;  // 有可能为NULL
		IStoryboard**  m_pStoryboardList;
		int            m_nCount;
	};

	typedef list<ObjectStoryboard*>  ObjectStoryboardList;
	typedef list<ObjectStoryboard*>::iterator ObjectStoryboardIter;
    class AnimateManager : public IWaitForHandleCallback
	{
	public:
		AnimateManager();
		~AnimateManager();

		void  AddStoryboard(IStoryboard* p);
		void  ClearStoryboardOfNotify(IMessage* pMsg);
		void  RemoveStoryboard(IStoryboard* p);
		
        IStoryboard*  CreateStoryboard();
		IWindowAnimateBase*  CreateWindowAnimateInstance(E_WINDOW_ANIMATE_TYPE eType, IWindowBase* pWindow);
        IControlAnimateBase* CreateControlAnimateInstance(E_CONTROL_ANIMATE_TYPE eType);

        IAnimateManager*  GetIAnimateManager();
	public:
		void    SetUIApplication(UIApplication* p) 
        {
            m_pUIApplication = p; 
        }
		int     GetFps() { return m_nFps; }
		int     SetFps(int n);

        virtual void OnWaitForHandleObjectCallback(HANDLE, LPARAM);
	protected:
        void    Destroy();

		void    StartAnimate();
		void    EndAnimate();

		void    SetTimer();
		void    KillTimer();

		ObjectStoryboard*  FindObjectStoryboard(IMessage* pNotify);
		ObjectStoryboardIter  FindObjectStoryboardIter(IMessage* pNotify);

	protected:
        IAnimateManager*  m_pIAnimateManager;

		HANDLE m_hTimer;
		int    m_nFps;
		bool   m_bHandlingTimerCallback;


		ObjectStoryboardList  m_listObjStoryboard;
		UIApplication*  m_pUIApplication;
	};


#define ANIMATE_TIME_LINE_FLAG_FINISH       0x0004   // 指示一个timeline是否已经到期结束了
#define ANIMATE_TIME_LINE_FLAG_NEED_RESET   0x0008   // 表示一个timeline一次循环结束，下次开始前需要重置

// #define ANIMATE_TIME_LINE_FLAG_S_CONFIGED   0x0010   // 指示哪些S(路程) T(时间) V(初速度) A(加速度)中哪些配置了
// #define ANIMATE_TIME_LINE_FLAG_V_CONFIGED   0x0020   // 没有被配置的变量则需要通过其实变量来计算
// #define ANIMATE_TIME_LINE_FLAG_T_CONFIGED   0x0040
// #define ANIMATE_TIME_LINE_FLAG_A_CONFIGED   0x0080

	template <class T>
	class ITimelineImpl : public T //public ITimeline
	{
	protected:
		ITimelineImpl()
		{
			m_nBeginTime = 0;
//			m_nDuration = 0;
            m_eTimeType = TLT_BY_MS;  // 默认以毫秒为单位

			m_nRepeatTimes = 1;  // 默认只播放一次
// 			m_bAutoReverse = 0;
// 			m_nRepeatBehavior = 0;

			m_nFlags = 0;
			m_nID = 0;
			m_pParam = NULL;
		}

	public:
		virtual ~ITimelineImpl(){};
		virtual void  Destroy() 
        { 
            delete this; 
        }
		virtual void  SetRepeateCount(int n) 
        { 
            m_nRepeatTimes = n; 
        }
		virtual bool  IsFinish() 
        { 
            return m_nFlags & ANIMATE_TIME_LINE_FLAG_FINISH ? true:false; 
        }
		virtual void  SetUserData(LPARAM lParam)
        { 
            m_pParam = lParam;
        }
		virtual LPARAM  GetUserData() 
        { 
            return m_pParam; 
        }
		virtual void  SetId(int nID) 
        { 
            m_nID = nID; 
        }
		virtual int  GetId() 
        { 
            return m_nID; 
        }
        virtual void  SetTimeType(TIMELINE_TIME_TYPE eType) 
        {
            m_eTimeType = eType; 
        }

//         virtual void  SetDuration(int n) 
//         { 
//             m_nDuration = n; 
//             m_nFlags|=ANIMATE_TIME_LINE_FLAG_T_CONFIGED; 
//         }

		// virtual void GetCurrentValue(void** ppOut) = 0;  // 由各个子类去实现
		// virtual void x_OnTick(bool* pbFinish) = 0;       // 由各个子类去实现

		// TODO: 该函数本不应该作为接口暴露给外部的。但是无奈在AnimateManager中也只能拿到ITimeline指针。
		// 后面再想想办法能否不用模块类来实现，同时还能保存IIntTimeline继承于ITimeline
		virtual void x_OnAnimateStart()
		{
			this->Init();
			m_nBeginTime = GetTickCount();
		}
		virtual void x_SetFinishFlag() 
		{ 
			m_nFlags |= ANIMATE_TIME_LINE_FLAG_FINISH;
		}
		
	public:
		virtual void  Init() = 0;

		void  SetAnimateMgrPtr(AnimateManager* p)
		{ m_pAnimateMgr = p; }

	protected:
		AnimateManager*  m_pAnimateMgr;

		int      m_nBeginTime;   // 记录动画开始时的time tick，当时间到达m_nBeginTime+m_nDuretion时，动画结束
		int      m_nDuration;    // 动画持续时间，由外部设置

		int      m_nRepeatTimes; // 1表示播放一次，-1表示一直播放，当--m_nRepeatTimes==0时停止
//		int      m_nRepeatBehavior;
//		bool     m_bAutoReverse;

		int      m_nFlags;
        TIMELINE_TIME_TYPE  m_eTimeType;

		int      m_nID;
		LPARAM   m_pParam;
	};

	
}

#endif  // ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E