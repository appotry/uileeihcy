#pragma once

namespace UI
{
interface IMapAttribute;
interface IUIElement;

interface IUIChildNodeEnum
{
    virtual void  Release() = 0;
    virtual IUIElement*  NextElement() = 0;  // 注：需要Release
};

interface IUIElement
{
    virtual long  AddRef() = 0;
    virtual long  Release() = 0;

    virtual void  GetTagName(BSTR* pbstr) = 0;
    virtual HRESULT  GetDocument(IUIDocument** pp) = 0;     // 注：需要Release

    virtual bool  GetData(BSTR* pbstr) = 0;
    virtual bool  SetData(const TCHAR* szText) = 0;

    virtual bool  AddChild(const TCHAR* szNodeName, IUIElement** pp) = 0;
    virtual void  RemoveChild(IUIElement*) = 0;
    virtual IUIElement*  FindChild(BSTR bstrChildName) = 0;
    virtual IUIElement*  FirstChild() = 0;
    virtual IUIChildNodeEnum*  EnumChild() = 0;  // 注：需要Release

    virtual void  GetAttribList(IMapAttribute** ppMapAttrib) = 0;
    virtual void  GetAttribList2(IListAttribute** ppListAttrib) = 0;
    virtual bool  GetAttrib(const TCHAR* szKey, BSTR* pbstrValue) = 0;
    virtual bool  HasAttrib(const TCHAR* szKey) = 0;
    virtual bool  AddAttrib(const TCHAR* szKey, const TCHAR* szValue) = 0;
    virtual bool  RemoveAttrib(const TCHAR* szKey) = 0;
    virtual bool  ModifyAttrib(const TCHAR* szKey, const TCHAR* szValue) = 0;
    virtual bool  ClearAttrib() = 0;
};

interface IUIDocument
{
    virtual long  AddRef() = 0;
    virtual long  Release() = 0;

    virtual bool  LoadFile(const TCHAR* szFile) = 0;
    virtual bool  LoadContent(BSTR bstrContent) = 0;
    virtual IUIElement*  FindElem(const TCHAR* szText) = 0;
    virtual bool  Save() = 0;
    virtual bool  SaveAs(const TCHAR* szPath) = 0;
    virtual const TCHAR*  GetPath() = 0;

    virtual void  SetDirty(bool b) = 0;
    virtual bool  IsDirty() = 0;
};
}