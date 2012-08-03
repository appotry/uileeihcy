#pragma once


//////////////////////////////////////////////////////////////////////////
//
//    对象创建者
//
//////////////////////////////////////////////////////////////////////////
typedef HRESULT (*_CreateInstanceFunc)(void** p);

template<class T>
class UIObjectCreator : public T
{
public:
	static HRESULT CreateInstance(void** pOut)
	{
		if (NULL == pOut)
			return E_INVALIDARG;

		T* p = new T();

		HRESULT hr = S_OK;
// 		hr = p->FinalConstruct();
// 
// 		if (FAILED(hr))
// 			delete p;
// 		else
// 			*pOut = p;

		return hr;
	}
};


struct _UI_OBJMAP_ENTRY
{
	String name;
	_CreateInstanceFunc pfunc;
};

#define UI_BEGIN_OBJECT_MAP(x)     static _UI_OBJMAP_ENTRY x[] = {
#define UI_END_OBJECT_MAP          { _T("")/*, {0,0,0,0} */,NULL}};
#define UI_OBJECT_MAP_ENTRY(class) { class::XmlName(), UIObjectCreator<##class##>::CreateInstance },

UI_BEGIN_OBJECT_MAP(aaa)
	UI_OBJECT_MAP_ENTRY(ListBox)
UI_END_OBJECT_MAP

