#ifndef OBJCREATOR_H_6A6E5AFC_D1B4_4e7b_8C57_4F8ECD9296B0
#define OBJCREATOR_H_6A6E5AFC_D1B4_4e7b_8C57_4F8ECD9296B0
#include "UISDK\Kernel\Inc\Base\leakdetect.h"  // ���ͷ�ļ�����Ҫ����uiobject��������leakdetect���

//////////////////////////////////////////////////////////////////////////
//
//    ���󴴽��ߣ�Ϊ��֧���ڶ��󴴽������ٵ�ʱ���ܵ����麯����Ҫ�����ж���
//    ����UICreateInstance���д���������ʼ��������ͷŴ������FinalConstruct
//    /FinalRelease����
//
//////////////////////////////////////////////////////////////////////////
namespace UI
{
interface IUIApplication;

template<class T>
class UIObjCreator : public T
{
public:
    virtual ~UIObjCreator()
    {
        UISendMessage(this, UI_WM_FINALRELEASE);
    }
public:

	// ÿ��UI Object �Ĵ������������ᱻ���·�ʽ���ã�
	// (s_UICreateInstancePtr)className::_CreatorClass::UICreateInstance
	static HRESULT UICreateInstance(IUIApplication* pUIApp, T** pOut)
    {
        UIObjCreator<T>* p = NULL; 
        HRESULT hr = S_OK;
        bool bRet = false;

        do 
        {
            // ���������һ��IUIApplication�Ĳ�����Ϊ����IControlImpl�ܹ��ڹ��캯�����õ�pAppȥCreateInstance
            p = new UIObjCreator<T>;
            p->CreateImpl();

            UISendMessage(p, UI_WM_RESETATTRIBUTE);  // ��Щ����ĳ�ʼ������������

            if (FAILED(UISendMessage(p, UI_WM_FINALCONSTRUCT, (WPARAM)pUIApp)))
                break;

            *pOut = static_cast<T*>(p);
            bRet = true;
        } while (0);

        if (!bRet)
        {
            delete p;
            return hr;
        }

        return hr;
    }

    static HRESULT UICreateInstance2(T** ppOut)
    {
        if (NULL == ppOut)
            return E_INVALIDARG;

        UIObjCreator<T>* p = new UIObjCreator<T>;
        p->AddRef();
        p->CreateImpl();

        *ppOut = static_cast<T*>(p);
        return S_OK;
    };

protected:
    // ��delete����������Ϊ����new�Գƣ�����new,delete�ڲ�ͬ��ģ����
    void  do_delete_this()
    {
        delete this;
    }
};

}
#endif // OBJCREATOR_H_6A6E5AFC_D1B4_4e7b_8C57_4F8ECD9296B0