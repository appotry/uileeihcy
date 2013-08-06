#ifndef ICONTROL_H_12B21A73_15E1_4562_9D1D_B910CC43921D
#define ICONTROL_H_12B21A73_15E1_4562_9D1D_B910CC43921D

#include "UISDK\Kernel\Inc\Interface\iobject.h"

namespace UI
{

class Control;
interface UISDKAPI IControl : public IObject
{
   UI_DECLARE_Ixxx_INTERFACE(IControl, Control)
};

}

#endif  // ICONTROL_H_12B21A73_15E1_4562_9D1D_B910CC43921D