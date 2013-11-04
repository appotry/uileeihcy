#pragma once
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
#include "UISDK\Kernel\Src\Resource\uiresource.h"

namespace UI
{

// 用于解析继承关系的数据结构，TODO: 抽象出来使用
template<class T>
class tree
{
public:
    tree()
    {
        parent = NULL;
        first_child = NULL;
        first_neighbour = NULL;
    };

    T         data;
    tree<T>*  parent;
    tree<T>*  first_child;
    tree<T>*  first_neighbour;
};

class StyleManager
{
public:
	StyleManager(SkinRes* p);
	~StyleManager(void);
	
	IStyleRes*  GetStyleRes();
    IStyleManager*  GetIStyleManager();

	HRESULT  InsertStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szInherit);
	HRESULT  ModifyStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szInherit);
	HRESULT  RemoveStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID );

	HRESULT  InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey, const TCHAR* szValue);
	HRESULT  ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey, const TCHAR* szValue);
	HRESULT  RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey);

	HRESULT  LoadStyle(const TCHAR* szTagName, const TCHAR* szStyleClass, const TCHAR* szID, IMapAttribute* pMapStyle);
    HRESULT  ParseStyle(const TCHAR* szObjName, IMapAttribute* pMapAttrib);

public:
	bool     Create(const String& strXmlPath);
	bool     Load(const String& strXmlPath);
	bool     Reload();
	bool     Save();
	void     Clear();
	int      GetStyleCount( );
//  bool     GetStyleItemInfo( int nIndex, IStyleResItem** ppStyleItemInfo );
//  bool     GetStyleItemInfo( STYLE_SELECTOR_TYPE eType, const TCHAR* szSelectorID, IStyleResItem** ppStyleItemInfo );

    static HRESULT  UIParseSkinElement(IUIElement*, ISkinRes* pSkinRes);
private:
    HRESULT  ParseNewElement(IUIElement* pElem);
    void  OnNewChild(IUIElement* pElem);

    bool  parse_inherit(tree<StyleResItem*>* pTreeItem, StyleRes* pStyleRes);
    bool  MakeInheritString(const STYLE_SELECTOR_TYPE& eStyletype, const String& strStypeName, TCHAR* szInherit);
    bool  ParseInheritString(const String& strInherit, STYLE_SELECTOR_TYPE& eStyletype, TCHAR* szStyleName);
    
private:
	// 数据持久层
#if 0
	IStyleParse*    m_pStyleParse;
#endif
    IStyleManager*  m_pIStyleManager;
    IUIElement*   m_pUIElement;

	// 对象属性
	StyleRes        m_resStyle;

	SkinRes*        m_pSkinRes;
};;

}