#pragma once
#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
namespace UI
{

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             字体                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

// 在CUIApplication中被声明

class FontManager
{
public:
	FontManager(SkinRes* p);
	~FontManager(void);
   
    IFontManager*  GetIFontManager();
	IFontRes*  GetFontRes();

	HRESULT  InsertFontItem(const TCHAR* szID, LOGFONT* pLogFont);
	HRESULT  ModifyFontItem(const TCHAR* szID, LOGFONT* pLogFont);
	HRESULT  RemoveFontItem(const TCHAR* szID);

public:
	bool       Create(const String& strXmlPath );
	bool       Load( const String& strXmlPath );
	bool       Reload();
	bool       Save();
	void       Clear();
	int        GetFontCount();
	IFontResItem*  GetFontItemInfo(int nIndex);
	
    static HRESULT  UIParseSkinElement(IUIElement*, ISkinRes* pSkinRes);
private:
    HRESULT  ParseNewElement(IUIElement* pElem);
    void  OnNewChild(IUIElement* pElem);

private:
	/*map<String, HFONT>  m_map;*/

	// 数据持久层
#if 0
	IFontParse*  m_pFontParse;
#endif
    IFontManager*  m_pIFontManager;
    IUIElement*  m_pUIElement;

	// 对象属性
	FontRes   m_resFont;
	SkinRes*  m_pSkinRes;
};

}