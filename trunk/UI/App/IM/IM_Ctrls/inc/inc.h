#pragma once

#ifdef IM_CTRLS_EXPORTS
#define IM_CTRLS_API __declspec(dllexport)
#else
#define IM_CTRLS_API __declspec(dllimport)
#endif

extern "C"
__declspec(dllexport) bool IM_Ctrl_RegisterUIObject(UI::IUIApplication* p);

#include "App\IM\IM_Ctrls\inc\iqqloginaccountlistbox.h"
