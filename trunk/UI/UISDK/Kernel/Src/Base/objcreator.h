#pragma once


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
		InitialRelease();
		FinalRelease();
	}

	// ÿ��UI Object �Ĵ������������ᱻ���·�ʽ���ã�
	// (s_UICreateInstancePtr)className::_CreatorClass::UICreateInstance
	static HRESULT UICreateInstance(T** pOut, IUIApplication* pUIApp)
	{
		return ::UICreateInstance(pOut, pUIApp);
	}

	virtual void ObjectMustCreateByUIObjCreator(){};
};

template<class T>
HRESULT UICreateInstance(T** pOut, IUIApplication* pUIApp)
{
	if (NULL == pOut || NULL == pUIApp)
		return E_INVALIDARG;

	UIObjCreator<T>* p = new UIObjCreator<T>();
	p->SetUIApplication(pUIApp);

	HRESULT hr = S_OK;
	hr = p->InitialConstruct();
	if (SUCCEEDED(hr))
		hr = p->FinalConstruct();

	if (FAILED(hr))
	{
		delete p;
	}
	else
	{
		*pOut = static_cast<T*>(p);
	}
	return hr;
}

}