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
//	ͨ��xml�еı�ǩ����ʵ����һ������
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
		UI_LOG_ERROR( _T("LayoutXmlParse::mapXmlToClass��δ�ܽ����Ķ������ƣ�%s"), strXmlName.c_str() ); \
	}


