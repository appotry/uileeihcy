#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

namespace UI{

class Object;

class CardLayoutParam : public LayoutParamBase
{
public:
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_CARD; }
};

class CardLayout : public LayoutBase<CardLayout, CardLayoutParam, LAYOUT_TYPE_CARD>
{
public:
    CardLayout();

    virtual void  SetAttribute(IMapAttribute* pMapAttrib) {};
    virtual SIZE  Measure();
    virtual void  Arrange(IObject* pObjToArrage=NULL, bool bUpdate=false);
};


}