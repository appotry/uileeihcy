#ifndef MAPATTR_H_7AE2A63B_631C_4dcf_9378_A72CCC795023
#define MAPATTR_H_7AE2A63B_631C_4dcf_9378_A72CCC795023
#include "UISDK\Kernel\Inc\Interface\imapattr.h"

namespace UI
{

class CMapAttribute : public IMapAttribute
{
public:
    CMapAttribute();
    CMapAttribute(const CMapAttribute& o);
    
    virtual bool  HasAttrib(const TCHAR* szKey);
    virtual const TCHAR* GetAttr(const TCHAR* szKey, bool bErase);
    virtual long  GetAttr_bool(const TCHAR* szKey, bool bErase, bool* pbGet);
    virtual long  GetAttr_int(const TCHAR* szKey, bool bErase, int* pnGet);
    virtual long  GetAttr_intarray(const TCHAR* szKey, bool bErase, int* pIntArray, unsigned int nSize);
    virtual long  GetAttr_CRegion4(const TCHAR* szKey, bool bErase, CRegion4* prcGet);
    virtual long  GetAttr_Image9Region(const TCHAR* szKey, bool bErase, Image9Region* pRegion);
    virtual long  GetAttr_RenderBase(const TCHAR* szPrefix, const TCHAR* szKey, bool bErase, IUIApplication* pUIApp, IObject* pBindObj, IRenderBase** ppGet);
    virtual long  GetAttr_TextRenderBase(const TCHAR* szKey, bool bErase, IUIApplication* pUIApp, IObject* pBindObj, ITextRenderBase** ppGet);

    virtual bool  AddAttr(const TCHAR* szKey, const TCHAR* szValue);
    virtual bool  ExtractMapAttrByPrefix(const TCHAR* szPrefix, bool bErase, /*out*/IMapAttribute** ppMapAttribute);
    virtual long  Release();
    virtual long  AddRef();
    virtual void  CreateCopy(IMapAttribute** ppNewCopy);
    virtual void  CopyTo(IMapAttribute* pDestMapAttrib, bool bOverride);

    virtual void  BeginEnum();
    virtual bool  EnumNext(const TCHAR** szKey, const TCHAR** szValue);
    virtual void  EndEnum();

public:
    const TCHAR* GetAttr(const String& strKey, bool bErase);
    void  AddAttr(const String& strKey, const String& strValue);

protected:
    ATTRMAP  m_mapAttr;
    String   m_strTemp;     // 用于删除要查找的对象时，能保存一份要删除的字符串引用
    int      m_lRef;

    ATTRMAP::iterator  m_iterEnum;
};


class ListAttrItem 
{
public:
    ListAttrItem () { pNext = NULL; }

    String  strKey;
    String  strValue;
    ListAttrItem*  pNext;
};
class CListAttribute : public  IListAttribute 
{
public:
    CListAttribute();
    ~CListAttribute();

    virtual bool  AddAttr(const TCHAR* szKey, const TCHAR* szValue);

    virtual void  BeginEnum();
    virtual bool  EnumNext(const TCHAR** szKey, const TCHAR** szValue);
    virtual void  EndEnum();

    virtual long  Release();
    virtual long  AddRef();

private:
    ListAttrItem*  m_pFirstItem;
    ListAttrItem*  m_pLastItem;

    ListAttrItem*  m_pEnum;  //  正在被Enum的对象
    int  m_lRef;
};

}

#endif MAPATTR_H_7AE2A63B_631C_4dcf_9378_A72CCC795023