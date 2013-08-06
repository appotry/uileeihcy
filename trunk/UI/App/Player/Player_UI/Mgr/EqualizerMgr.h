#pragma once
#include "player.h"
#include "App\Player\Player_UI\UI\EqualizerDlg.h"

class CEqualizerMgr : public IMgr
{
public:
	CEqualizerMgr(void);
	~CEqualizerMgr(void);

public:
	bool   Initialize();
	bool   Release();

	void   ToggleShowEqualizerDlg();
	void   OnEqualizerDlgHide();

	virtual  void  HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

	bool   SetBalance(long lPercent, bool bFinish);
	bool   EnableEq(bool bEnable);
	bool   SetEq(E_EQ_FREQ e, int nValue);
	bool   ReSetEq();

    CEqualizerDlg*  GetEqualizerDlg() { return m_pEqualizerDlg; }

private:
	CEqualizerDlg*   m_pEqualizerDlg;
};
