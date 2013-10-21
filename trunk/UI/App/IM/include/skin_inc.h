#ifndef SKIN_INC_H_178E6479_8CBF_4d1c_88A8_3C426E11D0BF
#define SKIN_INC_H_178E6479_8CBF_4d1c_88A8_3C426E11D0BF
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"

#ifdef SKIN_EXPORTS
#define IMSKINAPI __declspec(dllexport)
#else
#define IMSKINAPI __declspec(dllimport)
#endif

namespace IM
{
    struct ISkinUI : public IUI
    {
        virtual UI::IImageRender*  GetShareSkinRender() = 0;
        virtual int   GetTextureAlpha() = 0;
        virtual void  Show() = 0;
    };

    struct ISkinBiz : public IBiz
    {

    };

#define EVENT_SKIN_TEXTURE_ALPHA_CHANGED  _T("skin_texture_alpha_changed")
}

#endif // SKIN_INC_H_178E6479_8CBF_4d1c_88A8_3C426E11D0BF