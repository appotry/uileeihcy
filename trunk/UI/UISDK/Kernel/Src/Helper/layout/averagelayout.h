#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

// 平均分配布局
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
	LAYOUT_AVERAGE_DIRECTION  direction;    // 堆栈排布的方向
    Object*   m_pPanel;   // 与该布局关联的panel
    int   m_nGap;  // 两个子对象之间间隙,
};



}