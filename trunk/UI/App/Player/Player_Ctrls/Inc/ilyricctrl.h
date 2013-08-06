#ifndef ILYRICCTRL_H_B0A49B72_B97E_46e8_9BF2_4F6A8A589CE0
#define ILYRICCTRL_H_B0A49B72_B97E_46e8_9BF2_4F6A8A589CE0

class LyricCtrl;
interface PLAYER_CTRLS_API ILyricCtrl : public UI::IControl
{
    UI_DECLARE_Ixxx_INTERFACE(ILyricCtrl, LyricCtrl);

    bool  LoadLyric(const TCHAR*  szBuffer);
    void  Start();
    void  Pause();
    void  Stop();
    void  OnProgressInd(int ms);
};

#endif // ILYRICCTRL_H_B0A49B72_B97E_46e8_9BF2_4F6A8A589CE0