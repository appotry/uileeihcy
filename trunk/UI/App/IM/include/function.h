#ifndef FUNCTION_H_4960E35A_7C09_4658_986E_E221CDF122C3
#define FUNCTION_H_4960E35A_7C09_4658_986E_E221CDF122C3

namespace IM
{
    // 函数包基类，用于将函数、参数封装为一个指针传递参数传递
    class fun_pkg
    {
    public:
        long  Invoke()
        {
            return (*this)();
        }
        void  Release()
        {
            delete this;
        }
    protected:
        virtual ~fun_pkg() = 0 {};
        virtual long  operator()() = 0;
    };



#pragma region // 0个参数的成员函数封装

    // 函数对象
    template<class T, typename Return> 
    class mem_fun0
    {
    public:
        explicit mem_fun0(Return (T::*pfunc)())
        {
            m_pfunc = pfunc;
        }
        Return operator()(T* pThis)
        {
            return (pThis->*m_pfunc)();
        }

        Return (T::*m_pfunc)();
    };

    // 函数包
    template<class T, typename Return=long> 
    class mem_fun0_pkg : public fun_pkg
    {
    public:
        mem_fun0_pkg(mem_fun0<T, Return>& mf, T* pThis):m_memberfunction0(mf)
        {
            m_pThis = pThis;
        }
        Return operator()()
        {
            return m_memberfunction0(m_pThis);
        }
    private:
        mem_fun0<T, Return>   m_memberfunction0;
        T*    m_pThis;
    };

    // 构造成员函数对象
    template<class T, typename Return> 
    mem_fun0<T, Return> make_mem_fun0(Return (T::*pfunc)())
    {
        return mem_fun0<T, Return>(pfunc);
    }

    // 创建函数包对象
    template<class T, typename Return> 
    mem_fun0_pkg<T, Return> pkg_mem_fun0(Return (T::*pfunc)(), T* pThis)
    {
        return mem_fun0_pkg<T, Return>(make_mem_fun0(pfunc), pThis);
    }

    // 创建函数包指针
    template<class T, typename Return> 
    fun_pkg* create_mem_fun0_pkg(Return (T::*pfunc)(), T* pThis)
    {
        return new mem_fun0_pkg<T, Return>(make_mem_fun0(pfunc), pThis);
    }
#pragma endregion

#pragma region // 一个参数的成员函数封装

    // 函数对象
    template<class T, typename Arg1, typename Return> 
    class mem_fun1
    {
    public:
        explicit mem_fun1(Return (T::*pfunc)(Arg1))
        {
            m_pfunc = pfunc;
        }
        Return operator()(T* pThis, Arg1 _1)
        {
            return (pThis->*m_pfunc)(_1);
        }

        Return (T::*m_pfunc)(Arg1);
    };

    // 函数包
    template<class T, typename Arg1, typename Return=long> 
    class mem_fun1_pkg : public fun_pkg
    {
    public:
        mem_fun1_pkg(mem_fun1<T, Arg1, Return>& mf, T* pThis, Arg1 arg1):m_memberfunction1(mf)
        {
            m_pThis = pThis;
            m_Arg1 = arg1;
        }
        Return operator()()
        {
            return m_memberfunction1(m_pThis, m_Arg1);
        }
    private:
        mem_fun1<T, Arg1, Return>   m_memberfunction1;
        T*    m_pThis;
        Arg1  m_Arg1;
    };

    // 构造成员函数对象
    template<class T, typename Arg1, typename Return> 
    mem_fun1<T, Arg1, Return> make_mem_fun1(Return (T::*pfunc)(Arg1))
    {
        return mem_fun1<T, Arg1, Return>(pfunc);
    }

    // 创建函数包对象
    template<class T, typename Arg1, typename Return> 
    mem_fun1_pkg<T, Arg1, Return> pkg_mem_fun1(Return (T::*pfunc)(Arg1), T* pThis, Arg1 arg1)
    {
        return mem_fun1_pkg<T, Arg1, Return>(make_mem_fun1(pfunc), pThis, arg1);
    }

    // 创建函数包指针
    template<class T, typename Arg1, typename Return> 
    fun_pkg* create_mem_fun1_pkg(Return (T::*pfunc)(Arg1), T* pThis, Arg1 arg1)
    {
        return new mem_fun1_pkg<T, Arg1, Return>(make_mem_fun1(pfunc), pThis, arg1);
    }
#pragma endregion


#pragma region // 两个参数的成员函数封装

    // 函数对象
    template<class T, typename Arg1, typename Arg2, typename Return> 
    class mem_fun2
    {
    public:
        explicit mem_fun2(Return (T::*pfunc)(Arg1, Arg2))
        {
            m_pfunc = pfunc;
        }
        Return operator()(T* pThis, Arg1 _1, Arg2 _2)
        {
            return (pThis->*m_pfunc)(_1, _2);
        }

        Return (T::*m_pfunc)(Arg1, Arg2);
    };

    // 函数包
    template<class T, typename Arg1, typename Arg2, typename Return=long> 
    class mem_fun2_pkg : public fun_pkg
    {
    public:
        mem_fun2_pkg(mem_fun2<T, Arg1, Arg2, Return>& mf, T* pThis, Arg1 arg1, Arg2 arg2):m_memberfunction2(mf)
        {
            m_pThis = pThis;
            m_Arg1 = arg1;
            m_Arg2 = arg2;
        }
        Return operator()()
        {
            return m_memberfunction2(m_pThis, m_Arg1, m_Arg2);
        }
    private:
        mem_fun2<T, Arg1, Arg2, Return>   m_memberfunction2;
        T*    m_pThis;
        Arg1  m_Arg1;
        Arg2  m_Arg2;
    };

    // 构造成员函数对象
    template<class T, typename Arg1, typename Arg2, typename Return> 
    mem_fun2<T, Arg1, Arg2, Return> make_mem_fun2(Return (T::*pfunc)(Arg1, Arg2))
    {
        return mem_fun2<T, Arg1, Arg2, Return>(pfunc);
    }

    // 创建函数包对象
    template<class T, typename Arg1, typename Arg2, typename Return> 
    mem_fun2_pkg<T, Arg1, Arg2, Return> pkg_mem_fun2(Return (T::*pfunc)(Arg1, Arg2), T* pThis, Arg1 arg1, Arg2 arg2)
    {
        return mem_fun2_pkg<T, Arg1, Arg2, Return>(make_mem_fun2(pfunc), pThis, arg1, arg2);
    }

    // 创建函数包指针
    template<class T, typename Arg1, typename Arg2, typename Return> 
    fun_pkg* create_mem_fun2_pkg(Return (T::*pfunc)(Arg1, Arg2), T* pThis, Arg1 arg1, Arg2 arg2)
    {
        return new mem_fun2_pkg<T, Arg1, Arg2, Return>(make_mem_fun2(pfunc), pThis, arg1, arg2);
    }
#pragma endregion
}

#endif // FUNCTION_H_4960E35A_7C09_4658_986E_E221CDF122C3