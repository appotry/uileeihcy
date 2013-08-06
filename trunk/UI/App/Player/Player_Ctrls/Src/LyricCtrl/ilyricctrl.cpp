#include "stdafx.h"
#include "App\Player\Player_Ctrls\Inc\ilyricctrl.h"
#include "App\Player\Player_Ctrls\Src\LyricCtrl\lyricctrl.h"

UI_IMPLEMENT_Ixxx_INTERFACE2(ILyricCtrl, LyricCtrl, IControl)

bool  ILyricCtrl::LoadLyric(const TCHAR*  szBuffer)     { return m_pLyricCtrlImpl->LoadLyric(szBuffer); }
void  ILyricCtrl::Start()                               { m_pLyricCtrlImpl->Start(); }
void  ILyricCtrl::Pause()                               { m_pLyricCtrlImpl->Pause(); }
void  ILyricCtrl::Stop()                                { m_pLyricCtrlImpl->Stop(); }
void  ILyricCtrl::OnProgressInd(int ms)                 { m_pLyricCtrlImpl->OnProgressInd(ms); }