#pragma once
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"

namespace UI
{

class UIMarkupElement;
class UIMarkupChildNodeEnum : public IUIChildNodeEnum
{
protected:
    UIMarkupChildNodeEnum(IXMLDOMNodeList*, IUIDocument*);
public:
    static UIMarkupChildNodeEnum*  New(IXMLDOMNodeList* pNode, IUIDocument* pDoc);
    void  Release();
    IUIElement*  NextElement();

public:
    CComPtr<IXMLDOMNodeList>  m_pNodeList; 
    IUIDocument*  m_pDocument;   // 元素所在文档指针。仅引用指针，未保持计数
};

class UIMarkupElement : public IUIElement
{
protected:
    UIMarkupElement(IXMLDOMElement* pNode, IUIDocument* pDoc);
public:
    static UIMarkupElement*  New(IXMLDOMNode* pNode, IUIDocument*);
    long  Release();
    long  AddRef();

    HRESULT  GetDocument(IUIDocument** pp);
    void  GetTagName(BSTR* pbstr);
    void  GetAttribList(IMapAttribute** ppMapAttrib);
    void  GetAttribList2(IListAttribute** ppListAttrib);

    bool  GetData(BSTR* pbstr);
    bool  SetData(const TCHAR* szText);

    bool  HasAttrib(const TCHAR* szKey);
    bool  GetAttrib(const TCHAR* szKey, BSTR* pbstrValue);
    bool  AddAttrib(const TCHAR* szKey, const TCHAR* szValue);
    bool  ModifyAttrib(const TCHAR* szKey, const TCHAR* szValue);
    bool  RemoveAttrib(const TCHAR* szKey);
    bool  ClearAttrib();
    
    IUIElement*  FirstChild();
    IUIElement*  FindChild(BSTR bstrChildName);
    IUIChildNodeEnum*  EnumChild();
    void  RemoveChild(IUIElement*);
    bool  AddChild(const TCHAR* szNodeName, IUIElement** pp);

public:
    long  m_lRef;
    CComPtr<IXMLDOMElement>  m_pElement; 

    IUIDocument*  m_pDocument;   // 元素所在文档指针。仅引用指针，未保持计数
};

class UIMarkup : public IUIDocument
{
public:
    static UIMarkup*  New();
    long  Release();
    long  AddRef();

    UIMarkup();
    bool  LoadFile(const TCHAR* szFile);
    bool  LoadContent(BSTR bstrContent);

    IUIElement*  FindElem(const TCHAR* szText);
    bool  SaveAs(const TCHAR* szPath);
    bool  Save();
    const TCHAR*  GetPath() { return m_strPath.c_str(); }
    void  SetDirty(bool b);
    bool  IsDirty();

private:
    bool  Load(const TCHAR* szFile, BSTR bstrContent, bool bFileOrContent);

public:
    String  m_strPath;
    long    m_lRef;
    bool    m_bDirty;

    CComPtr<IXMLDOMDocument>  m_pDoc;
};

}