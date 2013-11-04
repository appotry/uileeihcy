#ifndef UIRESOURCE_H_D9795415_7B29_43b5_8FAD_262F5DA40D30
#define UIRESOURCE_H_D9795415_7B29_43b5_8FAD_262F5DA40D30

#include "UISDK\Kernel\Src\SkinParse\mapattr\mapattr.h"
#include "UISDK\Kernel\Src\Resource\imageres.h"
#include "UISDK\Kernel\Src\Resource\cursorres.h"
#include "UISDK\Kernel\Src\Resource\gifres.h"
#include "UISDK\Kernel\Src\Resource\fontres.h"
#include "UISDK\Kernel\Src\Resource\colorres.h"
#include "UISDK\Kernel\Src\Resource\styleres.h"

namespace UI
{
class SkinRes;
class Object;

class OutCtrlDllItem
{
public:
	OutCtrlDllItem();
	~OutCtrlDllItem();

public:
	HMODULE hModule;
	String  strDllPath;
};
class SkinBuilderRes
{
public:
	SkinBuilderRes(){m_pUIApp = NULL;};
	~SkinBuilderRes() {Clear();}

	void  SetUIApplication(IUIApplication* pUIApp){ m_pUIApp = pUIApp; }
	void  AddOuterCtrlsDllPath(const String&  strDll);
	OutCtrlDllItem*  FindOuterCtrlsDll(const String& strDll);
	OutCtrlDllItem*  FindOuterCtrlsDll(HMODULE hModule);
	void  Clear();

	bool  Register(OutCtrlDllItem*);

private:
	IUIApplication*  m_pUIApp;
	vector<OutCtrlDllItem*>   m_vecOuterCtrlsDll;   // �ⲿ�ؼ����ڵ�DLL������ע���ⲿ��������
};

// ��ǰƤ����ɫ������
struct SKIN_HLS_INFO
{
	short  h;       // -120 ~ 120
	short  l;       // -100 ~ 100
	short  s;       // -100 ~ 100
	int   nFlag;
};


interface ISkinConfigRes : public IRootInterface
{
	virtual SKIN_HLS_INFO* GetSkinHlsInfo()= 0;
	virtual void SetHLS(char h, char l, char s, int nFlag) = 0;
};
//
//	Ƥ����Ϣ��������Ԫ��xmlλ�� 
//
class SkinConfigRes : public ISkinConfigRes
{
public:
	SkinConfigRes();

private:
	String   m_strImageXmlPath;       // ȫ·�������浽�ļ��еĲ������·�� 
	String   m_strFontXmlPath;
	String   m_strColorXmlPath;
	String   m_strStyleXmlPath;
	String   m_strLayoutXmlPath;
	String   m_strI18nXmlPath;

	SKIN_HLS_INFO  m_sHlsInfo;
public:

	DECLARE_STRING_SETGET( ImageXmlPath );
	DECLARE_STRING_SETGET( FontXmlPath );
	DECLARE_STRING_SETGET( ColorXmlPath );
	DECLARE_STRING_SETGET( StyleXmlPath );
	DECLARE_STRING_SETGET( LayoutXmlPath );
	DECLARE_STRING_SETGET( I18nXmlPath );

	void    SetHLS(char h, char l, char s, int nFlag);
	virtual SKIN_HLS_INFO* GetSkinHlsInfo() { return &m_sHlsInfo; }
};




#if 0 // ��������UIEditorȥ�������
class LayoutResItem 
{
public:
	LayoutResItem();
    ~LayoutResItem();
    ILayoutResItem*  GetILayoutResItem();

	HRESULT  GetSelfAttr(IMapAttribute** ppMapAttribute);
	HRESULT  GetStyleAttr(IMapAttribute** ppMapAttribute);
	HRESULT  GetLastAttr(IMapAttribute** ppMapAttribute);

public:
	void  SetSelfAttr(IMapAttribute*  pattr);
	void  SetStyleAttr(IMapAttribute* pattr);
	void  SetLastAttr(IMapAttribute* pattr);

private:
    ILayoutResItem*  m_pILayoutResItem;
	IObject* m_pObject;

    IMapAttribute*   m_pMapSelfAttr;
    IMapAttribute*   m_pMapStyleAttr;
    IMapAttribute*   m_pMapLastAttr; 

//	ATTRMAP  m_mapSelfAttr;  // �ö��������õ�״̬
//	ATTRMAP  m_mapStyleAttr; // �ö������õ�����ʽ
//	ATTRMAP  m_mapLastAttr;  // �ö�����������
};
#endif
class LayoutRes 
{
public:
// 	typedef map<Object*, LayoutResItem*> _MyMap;
// 	typedef map<Object*, LayoutResItem*>::iterator _MyMapIter;

    LayoutRes();
	~LayoutRes();
    ILayoutRes*  GetILayoutRes();

// 	LayoutResItem*   AddResItem(Object* pObj);
// 	LayoutResItem*   GetResItem(Object* pObj);
	
protected:
    ILayoutRes*  m_pILayoutRes;
//	map<Object*, LayoutResItem*>  m_mapResItem;
};


}

#endif  // UIRESOURCE_H_D9795415_7B29_43b5_8FAD_262F5DA40D30