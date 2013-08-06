#ifndef INC_H_737E850B_CEFB_4ca2_9EFD_67CFF7946317
#define INC_H_737E850B_CEFB_4ca2_9EFD_67CFF7946317

#ifdef PLAYER_CTRLS_EXPORTS
#define PLAYER_CTRLS_API __declspec(dllexport)
#else
#define PLAYER_CTRLS_API __declspec(dllimport)
#endif

// {D2500E23-38A9-48af-A7B8-FCC9235DD259}
static const GUID IID_UI_ITTPlayerPlaylistCtrl = 
{ 0xd2500e23, 0x38a9, 0x48af, { 0xa7, 0xb8, 0xfc, 0xc9, 0x23, 0x5d, 0xd2, 0x59 } };
// {CEB41E8F-F16F-44d9-A1D4-F1F099FB0348}
static const GUID CLSID_UI_TTPlayerPlaylistCtrl = 
{ 0xceb41e8f, 0xf16f, 0x44d9, { 0xa1, 0xd4, 0xf1, 0xf0, 0x99, 0xfb, 0x3, 0x48 } };

// {D13C467C-D7F7-4cd8-A92B-BDF0E3620685}
static const GUID IID_UI_IMultiLabel = 
{ 0xd13c467c, 0xd7f7, 0x4cd8, { 0xa9, 0x2b, 0xbd, 0xf0, 0xe3, 0x62, 0x6, 0x85 } };
// {5DB6AF5E-53E6-4abf-AC28-CD0572C387C5}
static const GUID CLSID_UI_MultiLabel = 
{ 0x5db6af5e, 0x53e6, 0x4abf, { 0xac, 0x28, 0xcd, 0x5, 0x72, 0xc3, 0x87, 0xc5 } };

// {683960CC-6B05-4c82-95A1-C354354A96B4}
static const GUID IID_UI_ILyricCtrl = 
{ 0x683960cc, 0x6b05, 0x4c82, { 0x95, 0xa1, 0xc3, 0x54, 0x35, 0x4a, 0x96, 0xb4 } };
// {1232C4FC-F8C3-414a-BE0F-B6B01335A2E3}
static const GUID CLSID_UI_LyricCtrl = 
{ 0x1232c4fc, 0xf8c3, 0x414a, { 0xbe, 0xf, 0xb6, 0xb0, 0x13, 0x35, 0xa2, 0xe3 } };


#include "App/Player/Player_Ctrls/Inc/ittplayerplaylistctrl.h"

extern "C"
PLAYER_CTRLS_API bool TTPlayer_Ctrl_RegisterUIObject(UI::IUIApplication* p);

#endif // INC_H_737E850B_CEFB_4ca2_9EFD_67CFF7946317