#ifndef EVENT_H_19FEF657_DBE3_47e2_99E1_C95DF678C41D
#define EVENT_H_19FEF657_DBE3_47e2_99E1_C95DF678C41D

namespace IM
{
    class UIEventBase
    {
    public:
        void  AddListener(IM::IUI*);
        void  RemoveListener(IM::IUI*);
        void  FireEvent();
    protected:
        int  GetCount();
        IM::IUI*  GetUI(int nIndex);

    private:
        vector<IM::IUI*>   m_vecListener;
    };

}

#endif