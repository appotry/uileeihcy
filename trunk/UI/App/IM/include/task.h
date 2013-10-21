#ifndef TASK_H_A3C2C02B_D040_443f_B0B4_B90BEC8FC9C8
#define TASK_H_A3C2C02B_D040_443f_B0B4_B90BEC8FC9C8


namespace IM
{
    struct IUiBizBase;

    enum TASK_TYPE
    {
        TASK_TYPE_DEFAULT,
        TASK_TYPE_PENDING,
        TASK_TYPE_CALLBACK,
    };

    struct ITask
    {
        virtual ~ITask() = 0 {};
        virtual TASK_TYPE GetTaskType() = 0; 
        virtual void  Release() = 0;
        virtual void  AddRef() = 0;

        virtual IUiBizBase*  GetDest() = 0;
        virtual long  GetCommandId() = 0;
        virtual WPARAM  GetwParam() = 0;
        virtual LPARAM  GetlParam() = 0;
    };

    template <class T>
    class TaskImpl : public T
    {
    public:
        TaskImpl()
        {
            m_pDest = 0;
            m_nCommand = 0;
            m_wParam = m_lParam = 0;
            m_lRef = 1;
        }
        virtual ~TaskImpl()
        {
        }
        virtual TASK_TYPE GetTaskType()
        {
            return TASK_TYPE_DEFAULT;
        }

        virtual void  Release()
        {
            --m_lRef;
            if (0 == m_lRef)
            {
                delete this;
            }
        }
        virtual void AddRef()
        {
            ++m_lRef;
        }
        virtual IUiBizBase*  GetDest() 
        {
            return m_pDest;
        }
        virtual long  GetCommandId()
        {
            return m_nCommand;
        }
        virtual WPARAM  GetwParam()
        {
            return m_wParam;
        }
        virtual LPARAM  GetlParam()
        {
            return m_lParam;
        }
    public:
        IUiBizBase*  m_pDest;
        long  m_nCommand;
        WPARAM  m_wParam;
        LPARAM  m_lParam;

        long  m_lRef;
    };

    struct IPendingTask : public ITask
    {
    };

    template<class T>
    class PendingTaskImpl : public TaskImpl<T>
    {
    public:
        PendingTaskImpl()
        {
        }
        ~PendingTaskImpl()
        {
        }
        virtual TASK_TYPE GetTaskType()
        {
            return TASK_TYPE_PENDING;
        }
    };

    struct ICallbackTask : public ITask
    {
        
    };
    template<class T>
    class CallbackTaskImpl : public TaskImpl<T>
    {
    public:
        CallbackTaskImpl()
        {
            m_nCallbackId = 0;
            m_nCommandCallback = 0;
            m_wParamCallback = m_lParamCallback = 0;
        }
        virtual TASK_TYPE GetTaskType()
        {
            return TASK_TYPE_CALLBACK;
        }

    public:
        long  m_nCallbackId;
        long  m_nCommandCallback;
        WPARAM  m_wParamCallback;
        LPARAM  m_lParamCallback;
    };


    inline ITask*  CreateTask(IUiBizBase* pDest, long nCommand, WPARAM wParam, LPARAM lParam)
    {
        TaskImpl<ITask>* p = new TaskImpl<ITask>;
        p->m_pDest = pDest;
        p->m_nCommand = nCommand;
        p->m_wParam = wParam;
        p->m_lParam = lParam;
        return p;
    }
    inline ITask*  CreatePendingTask(IUiBizBase* pDest, long nCommand, WPARAM wParam, LPARAM lParam)
    {
        PendingTaskImpl<ITask>* p = new PendingTaskImpl<ITask>;
        p->m_pDest = pDest;
        p->m_nCommand = nCommand;
        p->m_wParam = wParam;
        p->m_lParam = lParam;
        return p;
    }
    inline ITask*  CreateCallbackTask(IUiBizBase* pDest, long nCommand, WPARAM wParam, LPARAM lParam)
    {
        CallbackTaskImpl<ITask>* p = new CallbackTaskImpl<ITask>;
        p->m_pDest = pDest;
        p->m_nCommand = nCommand;
        p->m_wParam = wParam;
        p->m_lParam = lParam;
        return p;
    }
}


#endif // TASK_H_A3C2C02B_D040_443f_B0B4_B90BEC8FC9C8