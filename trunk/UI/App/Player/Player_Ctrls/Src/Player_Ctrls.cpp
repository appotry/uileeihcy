// Player_Ctrls.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "APP\Player\Player_Ctrls\Src\TTPlayerPlaylistCtrl\TTPlayerPlaylistCtrl.h"
#include "APP\Player\Player_Ctrls\Src\MultiLabel\multilabel.h"
#include "App\Player\Player_Ctrls\Src\LyricCtrl\lyricctrl.h"

extern "C" 
{
    __declspec(dllexport) bool RegisterUIObject(UI::IUIApplication* p)
    {
        REGISTER_UI_OBJECT(p, TTPlayerPlaylistCtrl);
        REGISTER_UI_OBJECT(p, MultiLabel);
        REGISTER_UI_OBJECT(p, LyricCtrl);
        return true;
    }

    __declspec(dllexport) bool TTPlayer_Ctrl_RegisterUIObject(UI::IUIApplication* p)
    {
        return RegisterUIObject(p);
    }
}