#ifndef ILAYOUT_H_1599DE50_CAFA_45cb_BC50_5E689AE0B184
#define ILAYOUT_H_1599DE50_CAFA_45cb_BC50_5E689AE0B184

namespace UI
{
class Object;
interface IMapAttribute;
interface ILayout
{
    virtual void  Release() = 0;

    virtual SIZE  Measure() = 0;
    virtual void  Arrange(IObject* pObjToArrage, bool bReDraw=false) = 0;
    virtual void  SetAttribute(IMapAttribute* pMapAttrib) = 0;
    virtual void  UpdateObjectLayoutPos(IObject* pObj) = 0;
};

}
#endif