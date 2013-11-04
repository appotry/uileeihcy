#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

// ƽ�����䲼��
namespace UI
{

class Object;

class AverageLayoutParam : public LayoutParamBase
{
public:
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_AVERAGE; }
};


class AverageLayout : public LayoutBase<AverageLayout, AverageLayoutParam, LAYOUT_TYPE_AVERAGE>
{
public:
	AverageLayout();
    ~AverageLayout();

    virtual SIZE  Measure();
	virtual void  Arrange(IObject* pObjToArrage = NULL, bool bUpdate=false);
	virtual void  SetAttribute(IMapAttribute* pMapAttrib);
    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

    void  ArrangeObject_H(Object* pChildObj, CRect* prc);
    void  ArrangeObject_V(Object* pChildObj, CRect* prc);

public:
	LAYOUT_AVERAGE_DIRECTION  direction;    // ��ջ�Ų��ķ���
    Object*   m_pPanel;   // ��ò��ֹ�����panel
    int   m_nGap;  // �����Ӷ���֮���϶,
};



}