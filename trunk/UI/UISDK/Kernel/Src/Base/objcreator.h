#pragma once


//////////////////////////////////////////////////////////////////////////
//
//    对象创建者，为了支持在对象创建和销毁的时候能调用虚函数，要求所有对象
//    调用UICreateInstance进行创建，将初始化代码和释放代码放在FinalConstruct
//    /FinalRelease里面
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

	// 每个UI Object 的创建函数，将会被如下方式调用：
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