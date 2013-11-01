#ifndef ILAYOUT_H_1599DE50_CAFA_45cb_BC50_5E689AE0B184
#define ILAYOUT_H_1599DE50_CAFA_45cb_BC50_5E689AE0B184

namespace UI
{
class Object;
interface IMapAttribute;


interface ILayoutParam : public IRootInterface
{
    virtual void  Release() = 0;

    //  �ڲ���ǰ������ؼ������Ĵ�С
    virtual SIZE  CalcDesiredSize() = 0;

    // �ɵ�ǰ����RECT���Ƴ����ֲ���
    virtual void  UpdateByRect() = 0;

    // ���л�
    virtual void  Serialize(SERIALIZEDATA* pData) = 0;

    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData) = 0;

    // �ӿ������ж�
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