#pragma once


namespace UI
{
interface IUIElement;
interface IUIDocument;

class SkinParseEngine
{
public:
    SkinParseEngine(IUIApplication* pUIApp, SkinRes* pSkinRes);
    ~SkinParseEngine();

    bool  ProcessFile(const TCHAR* szXmlFile);
    void  NewChild(IUIElement* pElement);

    static HRESULT  UIParseIncludeElement(IUIElement*, ISkinRes* pSkinRes);

    IUIApplication*  m_pUIApplication;
    SkinRes*  m_pSkinRes;
};

}