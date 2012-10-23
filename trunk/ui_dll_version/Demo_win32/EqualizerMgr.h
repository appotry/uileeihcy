#pragma once
#include "player.h"
#include "EqualizerDlg.h"

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

private:
	CEqualizerDlg*   m_pEqualizerDlg;
};
