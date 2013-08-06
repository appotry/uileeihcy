#ifndef IPANEL_H_F960562B_443B_476a_BD3D_05ED9D463B28
#define IPANEL_H_F960562B_443B_476a_BD3D_05ED9D463B28

#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{
interface ILayout;
interface IObject;
class Panel;

interface UISDKAPI IPanel : public IObject
{
    UI_DECLARE_Ixxx_INTERFACE(IPanel, Panel)

    ILayout*   GetLayout();
    void  SetLayoutType(LAYOUTTYPE eLayoutType);

    void  SetMaterialRender(IRenderBase* p);
    IRenderBase*  GetMaterialRender();
};

}

#endif  // IPANEL_H_F960562B_443B_476a_BD3D_05ED9D463B28