#ifndef LAYOUT_H_70402CBF_A4C2_4a35_AA14_829F26BA9A5C
#define LAYOUT_H_70402CBF_A4C2_4a35_AA14_829F26BA9A5C

#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{
class Panel;
class WindowBase;

#if 0

/*
**	ע�⣺DockLayoutֻ������һ��center���󣬵��ö��������һ��Panel��Object
**		  ����������ļ����ж��center������ֻȡ��һ������
*/
class DockLayout : public Layout
{
public:
	virtual SIZE  MeasureChildObject();
	virtual void  ArrangeChildObject(Object* pObjToArrage = NULL);
};


class LayoutManagerFactory 
{
public:
	static Layout* GetLayout(const TCHAR* szType, Object* pPanel);
	static Layout* GetLayoutFromAttribute(IMapAttribute* pMapAttrib, Object* pObject, bool bCreateDefaultCanvasLayout=true);
};
#endif

template <class TLayout, class TLayoutParam, LAYOUTTYPE tLayoutType>
class LayoutBase : public ILayout
{
public:
    LayoutBase()
    {
        m_pPanel = NULL;
    }
    virtual ~LayoutBase()
    {
        if (m_pPanel)
        {
            Object*  pChild = NULL;
            while (pChild = this->m_pPanel->EnumChildObject(pChild))
            {
                pChild->SetLayoutParam(NULL);
            }
        }
    }
    void  Release()
    {
        delete this;    
    }

    virtual LAYOUTTYPE  GetLayoutType() 
    { 
        return tLayoutType; 
    }

    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
    {

    }

    static HRESULT  CreateInstance(IObject* p, ILayout** ppOut)
    {
        if (NULL == ppOut || NULL == p)
            return E_INVALIDARG;

        TLayout*  pLayout = new TLayout;
        pLayout->m_pPanel = p->GetImpl();
        *ppOut = pLayout;
        return S_OK;
    }
    ILayoutParam*  CreateLayoutParam(IObject* pObj)
    {
        return s_CreateLayoutParam(pObj);
    }

    static ILayoutParam*  s_CreateLayoutParam(IObject* pObj)
    {
        if (!pObj)
            return NULL;

        TLayoutParam* p = new TLayoutParam();
        p->SetObject(pObj->GetImpl());
        IMapAttribute* pMapAttr = NULL;
        pObj->GetMapAttribute(&pMapAttr);

        SERIALIZEDATA data;
        data.nFlag = SERIALIZEFLAG_LOAD;
        data.pMapAttrib = pMapAttr;

        p->Serialize(&data);
        SAFE_RELEASE(pMapAttr);

        return static_cast<ILayoutParam*>(p);
    }

    // �Լ��ڲ��ֵ�ʱ�����
    static TLayoutParam*  s_GetObjectLayoutParam(Object* pObj)
    {
        if (!pObj)
            return NULL;

        ILayoutParam* pParam = pObj->GetLayoutParam();
        if (pParam && pParam->GetLayoutType() == tLayoutType)
        {
            return static_cast<TLayoutParam*>(pParam);
        }

        pParam = s_CreateLayoutParam(pObj->GetIObject());
        pObj->SetLayoutParam(pParam);
        return static_cast<TLayoutParam*>(pParam);
    }

protected:
    Object*   m_pPanel;   // ��ò��ֹ�����panel
};

class LayoutParamBase : public ILayoutParam
{
public:
    LayoutParamBase()
    {
        m_pObj = NULL;
    }
    virtual ~LayoutParamBase()
    {

    }
    virtual void  Release()
    {
        delete this;
    }

    void  SetObject(Object* p)
    {
        m_pObj = p;
    }
    virtual void  UpdateByRect()
    {

    }
    virtual void  Serialize(SERIALIZEDATA* pData)
    {

    }
    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
    {

    }
    virtual SIZE  CalcDesiredSize()
    {
        SIZE s = {0, 0};

        if (m_pObj)
        {
            // ���� margin �Ĵ�С
            s.cx += m_pObj->GetMarginW();
            s.cy += m_pObj->GetMarginH();
        }

        return s;
    }

protected:
    Object*  m_pObj;
};

// ���沼�ֹ�������ר���������ö��㴰������Ļ�еĴ�С��λ��
// ���沼����Щ���⣬��ֱ�ӹ����ڵ�λ�ã������ںͿؼ���Panel�кܶ಻һ���ĵط���
// �細�ڷֿͻ����ͷǿͻ���������DesktopLayoutֻ��Ҫ����һ�����󣬼�Window��û������
// �Ӷ���������ﲻ��DesktopLayout������LayoutManager
class DesktopLayout   
{
public:
    void  Arrange(WindowBase*  pWindow);
};

}

#endif // LAYOUT_H_70402CBF_A4C2_4a35_AA14_829F26BA9A5C