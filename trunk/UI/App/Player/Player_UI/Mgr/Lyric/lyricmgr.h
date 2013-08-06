#pragma once
#include "App\Player\Player_UI\Mgr\player.h"
class  CLyricDownloadMgr;
class  CLyricDlg;
class  CPlayerLyricData;

class CLyricMgr : public IMgr
{
public:
    CLyricMgr(void);
    ~CLyricMgr(void);

public:
    bool   Initialize();
    bool   Release();

    CLyricDlg*  GetLyricDlg() { return m_pLyricDlg; }

    void   ToggleShowLyricDlg();
    void   OnLyricDlgHide();

    virtual  void  HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

protected:
    void  OnPlay(IPlayerListItemInfo*  pItem);
    

private:
    CLyricDlg*   m_pLyricDlg;
    CLyricDownloadMgr*  m_pLyricDownloadMgr;
    CPlayerLyricData*   m_pPlayerLyricData;
};
