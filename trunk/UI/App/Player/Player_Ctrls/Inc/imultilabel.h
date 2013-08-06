#ifndef IMULTILABEL_H_12A84F0A_373C_41a3_A29D_AF147E2B05CE
#define IMULTILABEL_H_12A84F0A_373C_41a3_A29D_AF147E2B05CE

class MultiLabel;
interface PLAYER_CTRLS_API IMultiLabel : public UI::IControl
{
    UI_DECLARE_Ixxx_INTERFACE(IMultiLabel, MultiLabel);

    void  SetContent(const TCHAR* szContent, const TCHAR szSeparator=_T('\n'));
    void  StartAnimateSwitch(int nTime);
    void  StopAnimateSwitch();
};

#endif // IMULTILABEL_H_12A84F0A_373C_41a3_A29D_AF147E2B05CE