#pragma once


//////////////////////////////////////////////////////////////////////////
//
//    ���󴴽��ߣ�Ϊ��֧���ڶ��󴴽������ٵ�ʱ���ܵ����麯����Ҫ�����ж���
//    ����UICreateInstance���д���������ʼ��������ͷŴ������FinalConstruct
//    /FinalRelease����
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

