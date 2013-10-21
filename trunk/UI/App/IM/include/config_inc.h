#ifndef CONFIGBIZ_H_42721BD8_369A_4d29_92BD_8A09089BD738
#define CONFIGBIZ_H_42721BD8_369A_4d29_92BD_8A09089BD738
#include "App\IM\include\framework_inc.h"

namespace IM
{

    class IConfigBiz : public IM::IBiz
    {
    public:
        virtual  void  GetCurSkinPath(TCHAR* szText) = 0;
        virtual  void  SetCurSkinPath(const TCHAR* szText) = 0;
        virtual  byte  GetSkinAlpha() = 0;
        virtual  void  SetSkinAlpha(byte b) = 0;
        virtual  byte  GetSkinTextureAlpha() = 0;
        virtual  void  SetSkinTextureAlpha(byte b) = 0;
    };


    class IConfigUI : public IM::IUI
    {

    };
}

#endif // CONFIGBIZ_H_42721BD8_369A_4d29_92BD_8A09089BD738