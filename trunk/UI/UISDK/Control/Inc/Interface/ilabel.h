#ifndef ILABEL_H_B087DBCB_BEE5_48c9_91CE_628F025E1BE0
#define ILABEL_H_B087DBCB_BEE5_48c9_91CE_628F025E1BE0

#include "UISDK\Kernel\Inc\Interface\icontrol.h"
namespace UI
{

class Label;
interface UICTRLAPI ILabel : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(ILabel, Label);

    const TCHAR*  GetText();
    void  SetText(const TCHAR* pszText, bool bRedraw=true);
};

class PictureCtrl;
interface UICTRLAPI IPictureCtrl : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IPictureCtrl, PictureCtrl);
};

class GifCtrl;
interface UICTRLAPI IGifCtrl : public IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IGifCtrl, GifCtrl);

    bool  Start();
    bool  Pause();
    bool  Stop();
};

}

#endif // ILABEL_H_B087DBCB_BEE5_48c9_91CE_628F025E1BE0