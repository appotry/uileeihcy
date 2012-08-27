#pragma once


//////////////////////////////////////////////////////////////////////////
//
//    对象创建者，为了支持在对象创建和销毁的时候能调用虚函数，要求所有对象
//    调用UICreateInstance进行创建，将初始化代码和释放代码放在FinalConstruct
//    /FinalRelease里面
//
//////////////////////////////////////////////////////////////////////////

template<class T>
class UIObjCreator : public T
{
public:
	virtual ~UIObjCreator()
	{
		InitialRelease();
		FinalRelease();
	}
	static HRESULT UICreateInstance(T** pOut)
	{
		return ::UICreateInstance(pOut);
	}

	virtual void ObjectMustCreateByUIObjCreator(){};
};

HRESULT UICreateInstance(const String& strXmlName, Object** pOut);

template<class T>
HRESULT UICreateInstance(T** pOut)
{
	if (NULL == pOut)
		return E_INVALIDARG;

	UIObjCreator<T>* p = new UIObjCreator<T>();

	HRESULT hr = S_OK;
	hr = p->InitialConstruct();
	if (SUCCEEDED(hr))
		hr = p->FinalConstruct();

	if (FAILED(hr))
		delete p;
	else
		*pOut = static_cast<T*>(p);

	return hr;
}

