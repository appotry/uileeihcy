#ifndef EVENT_H_19FEF657_DBE3_47e2_99E1_C95DF678C41D
#define EVENT_H_19FEF657_DBE3_47e2_99E1_C95DF678C41D
#include "App\IM\include\framework_inc.h"

class CFramework;

    class UIEvent
    {
    public:
        UIEvent(CFramework* pFramework, const TCHAR* szName);
        ~UIEvent();

        void  AddListener(IM::IUI*);
        void  RemoveListener(IM::IUI*);
        void  FireEvent(LPARAM lParam);
        int   GetCount();
        IM::EventType  GetName();

    protected:
        bool  IsListenExist(IM::IUI*);

    private:
        vector<IM::IUI*>   m_vecListener;
        CFramework*  m_pFramework;

        CRITICAL_SECTION  m_cs;
        String  m_strName;
    };



//     class BizEvent
//     {
//     public:
//         void  AddListener(IM::IBiz*);
//         void  RemoveListener(IM::IBiz*);
//         void  FireEvent();
//         int   GetCount();
// 
//     private:
//         vector<IM::IBiz*>   m_vecListener;
//     };


#endif