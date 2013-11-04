#pragma once
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
#include "UISDK\Kernel\Src\Resource\uiresource.h"

namespace UI
{
class Object;
class LayoutManager
{
public:
	LayoutManager(SkinRes*);
	~LayoutManager(void);
	
	ILayoutRes*  GetLayoutRes();
    ILayoutManager*  GetILayoutManager();

	HRESULT  LoadRootLayout(IObject* pRootObj, const TCHAR* szId);
    HRESULT  LoadRootLayout(const TCHAR* szObjName, const TCHAR* szId, IObject** ppOut);
    HRESULT  LoadLayout(IUIElement* pUIElement, IObject* pParent, IObject** ppObj);
    HRESULT  LoadObjAttrAndChild(IObject* pObj, IUIElement* pUIElement);

public:
	void  Clear();
//--bool  LoadLayout(Object* pRootObj);
	bool  ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild );

	// 编辑器专用函数
	bool  LoadAllWindowNodeInfo(LayoutWindowNodeInfo* pInfoArray, int* pArraySize);

    bool  LoadObjectAttribute(Object* pObj, IUIElement* pObjElement, bool bReload);
    void  ReloadChildObjects(Object* pObjParent, IUIElement* pObjElement, list<Object*>& listAllChild);
    void  LoadChildObjects(IObject* pIObjParent, IUIElement* pUIElement);

    static HRESULT  UIParseSkinElement(IUIElement*, ISkinRes* pSkinRes);
private:
    HRESULT  ParseNewElement(IUIElement* pElem);
    virtual HRESULT  FindRootUIElement(const TCHAR* szTagName, const TCHAR* szId, IUIElement** ppElem);

private:
    ILayoutManager*  m_pILayoutManager;
    UIList<IUIElement*>  m_listUIElement;

	// 对象属性
	LayoutRes        m_resLayout;
	SkinRes*         m_pSkinRes;
};

}