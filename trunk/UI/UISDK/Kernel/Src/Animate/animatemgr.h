#ifndef ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E
#define ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E

#include "UISDK\Kernel/Inc/Interface/ianimate.h"

namespace UI
{
	// http://msdn.microsoft.com/zh-cn/magazine/cc163397.aspx  ��չ WPF ������
	// http://www.cnblogs.com/Jax/archive/2008/03/26/1123778.html ��Programming WPF������ ��8�� ����

	// http://msdn.microsoft.com/en-us/library/system.windows.media.animation.doubleanimationbase_members(v=vs.85).aspx  DoubleAnimationBase Members

	// �˽�ʲô�������Ի򲻿���ʵ�ֵĹؼ��ǣ�������ľ۽����Ե�����
	
// 	Inheritance Hierarchy (WPF ��̳��б�)
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

	// ��ν��Delegate���Ǿͻػص��ӿڡ���MVC�е� M->C ��ͨѶ��ʽ������ôʵ�ֵ�
	// M�ͻ���ûص��ӿڣ�Ȼ��C����ʵ������ص��ӿڣ�����MC����ͨѶ�ˣ�����MҲ��
	// ���˽�C���κ���Ϣ.
	// Delegate��һ��һ�ģ���Notification��һ�Զ�Ĺ㲥��Ϣ��Target/Action�ǽ�Ӳ����Ϣ
	// ������������̰�ť֮�ࣩת���ɳ�����Ϣ��
	
	// Target-Action?

	// http://gamebabyrocksun.blog.163.com/blog/static/571534632010219544464/  ��Ϸ����ѭ������
	
	// ��WPF����ԭ��
	class ObjectStoryboard  // һ���������ͬʱ��Ӷ��Storyboard��Ϊ��ʵ�ָ��ö���һ��tickֻ����һ��֪ͨ�����ö������е�Storyboard����������
	{
	public:
		ObjectStoryboard(IMessage* p);
		~ObjectStoryboard();

		void  AddStoryboard(IStoryboard* p);
		void  RemoveStoryboard(IStoryboard* p);
		void  CheckFinishFlag();
		int   FindStoryboard(int nID);

		IMessage*      m_pNotifyObject;  // �п���ΪNULL
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


#define ANIMATE_TIME_LINE_FLAG_FINISH       0x0004   // ָʾһ��timeline�Ƿ��Ѿ����ڽ�����
#define ANIMATE_TIME_LINE_FLAG_NEED_RESET   0x0008   // ��ʾһ��timelineһ��ѭ���������´ο�ʼǰ��Ҫ����

// #define ANIMATE_TIME_LINE_FLAG_S_CONFIGED   0x0010   // ָʾ��ЩS(·��) T(ʱ��) V(���ٶ�) A(���ٶ�)����Щ������
// #define ANIMATE_TIME_LINE_FLAG_V_CONFIGED   0x0020   // û�б����õı�������Ҫͨ����ʵ����������
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
            m_eTimeType = TLT_BY_MS;  // Ĭ���Ժ���Ϊ��λ

			m_nRepeatTimes = 1;  // Ĭ��ֻ����һ��
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

		// virtual void GetCurrentValue(void** ppOut) = 0;  // �ɸ�������ȥʵ��
		// virtual void x_OnTick(bool* pbFinish) = 0;       // �ɸ�������ȥʵ��

		// TODO: �ú�������Ӧ����Ϊ�ӿڱ�¶���ⲿ�ġ�����������AnimateManager��Ҳֻ���õ�ITimelineָ�롣
		// ����������취�ܷ���ģ������ʵ�֣�ͬʱ���ܱ���IIntTimeline�̳���ITimeline
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

		int      m_nBeginTime;   // ��¼������ʼʱ��time tick����ʱ�䵽��m_nBeginTime+m_nDuretionʱ����������
		int      m_nDuration;    // ��������ʱ�䣬���ⲿ����

		int      m_nRepeatTimes; // 1��ʾ����һ�Σ�-1��ʾһֱ���ţ���--m_nRepeatTimes==0ʱֹͣ
//		int      m_nRepeatBehavior;
//		bool     m_bAutoReverse;

		int      m_nFlags;
        TIMELINE_TIME_TYPE  m_eTimeType;

		int      m_nID;
		LPARAM   m_pParam;
	};

	
}

#endif  // ANIMATEMGR_H_D4894A37_DD24_41d4_9699_E31C1CE1421E