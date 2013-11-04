#pragma once

#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"

namespace UI
{


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             颜色                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

class ColorManager
{
public:
    ColorManager(SkinRes* pSkinRes);
    ~ColorManager(void);

    IColorRes*  GetColorRes();
    HRESULT  InsertColorItem(const TCHAR* bstrID, const TCHAR* bstrColor);
    HRESULT  ModifyColorItem(const TCHAR* bstrID, const TCHAR* bstrColor);
    HRESULT  RemoveColorItem(const TCHAR* bstrID);
    HRESULT  ParseNewElement(IUIElement* pElem);
    IColorManager*  GetIColorManager();

public:
    bool  Create(const String& strXmlPath );
    bool  Load( const String& strXmlPath );
    bool  Save();
    bool  Reload();
    void  Clear();
    int   GetColorCount();
    IColorResItem*  GetColorItemInfo(int nIndex);

    bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

    static HRESULT  UIParseSkinElement(IUIElement*, ISkinRes* pSkinRes);
#if 0
    IColorParse*  GetColorParse() { return m_pColorParse; }
#endif

private:
    void  OnNewChild(IUIElement* pElem);

private:
#if 0
    IColorParse*    m_pColorParse;  // TODO: 废弃，使用新架构
#endif
    IColorManager*  m_pIColorManager;
    IUIElement*  m_pUIElement;

    ColorRes     m_resColor;              
    SkinRes*     m_pSkinRes;
};
}