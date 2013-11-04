#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

namespace UI
{

class Object;

class StackLayoutParam : public LayoutParamBase
{
public:
    StackLayoutParam();
    ~StackLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_STACK; }
    virtual SIZE  CalcDesiredSize();

public:
    DECLARE_INT_SETGET(ConfigWidth)
    DECLARE_INT_SETGET(ConfigHeight)
    DECLARE_INT_SETGET(ConfigLayoutFlags)

private:
    int  m_nConfigWidth;  
    int  m_nConfigHeight; 
    int  m_nConfigLayoutFlags;
};
class StackLayout : public LayoutBase<StackLayout, StackLayoutParam, LAYOUT_TYPE_STACK>
{
public:
	StackLayout();
    ~StackLayout();

    virtual SIZE  Measure();
	virtual void  Arrange(IObject* pObjToArrage = NULL, bool bUpdate=false);
	virtual void  SetAttribute(IMapAttribute* pMapAttrib);
    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

protected:
    void  ArrangeObject_Left(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size);
    void  ArrangeObject_Right(Object* pChildObj, int& nLeftCustom, int& nRightCustom, SIZE size);
    void  ArrangeObject_Top(Object* pChildObj, int& nTopCustom, int& nBottomCustom, SIZE size);
    void  ArrangeObject_Bottom(Object* pChildObj, int& nTopCustom, int& nBottomCustom, SIZE size);

    bool  IsHorz();
    bool  IsVert();

public:
	LAYOUT_STACK_DIRECTION  direction;    // 堆栈排布的方向
    int   m_nGap;  // 两个子对象之间间隙
};



}