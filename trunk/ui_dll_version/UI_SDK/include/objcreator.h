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


//
//	通过xml中的标签名来实例化一个对象
//
#define BEGIN_XML2CLASS \
	Object* pObj = NULL;

#define XML2CLASS( CLASS )  \
	if( CLASS::XmlName() == strXmlName )  \
	{ \
		pObj = new UIObjCreator<CLASS>; \
	} \
	else 

#define END_XML2CLASS \
	{ \
		UI_LOG_ERROR( _T("LayoutXmlParse::mapXmlToClass，未能解析的对象名称：%s"), strXmlName.c_str() ); \
	}


