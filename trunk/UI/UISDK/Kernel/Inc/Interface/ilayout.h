#ifndef ILAYOUT_H_1599DE50_CAFA_45cb_BC50_5E689AE0B184
#define ILAYOUT_H_1599DE50_CAFA_45cb_BC50_5E689AE0B184

namespace UI
{
class Object;
interface IMapAttribute;


interface ILayoutParam : public IRootInterface
{
    virtual void  Release() = 0;

    //  在布局前，计算控件期望的大小
    virtual SIZE  CalcDesiredSize() = 0;

    // 由当前坐标RECT反推出布局参数
    virtual void  UpdateByRect() = 0;

    // 序列化
    virtual void  Serialize(SERIALIZEDATA* pData) = 0;

    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData) = 0;

    // 接口类型判断
    virtual LAYOUTTYPE  GetLayoutType() = 0;
};


interface ILayout : public IRootInterface
{
    virtual void  Release() = 0;
    
    virtual LAYOUTTYPE  GetLayoutType() = 0;
    virtual SIZE  Measure() = 0;
    virtual void  Arrange(IObject* pObjToArrage, bool bReDraw=false) = 0;
    virtual void  SetAttribute(IMapAttribute* pMapAttrib) = 0;
    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData) = 0;
    virtual ILayoutParam*  CreateLayoutParam(IObject* pObj) = 0;

    
};

}
#endif