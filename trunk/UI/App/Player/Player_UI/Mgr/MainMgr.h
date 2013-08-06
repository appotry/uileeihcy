#pragma once
#include "App\Player\Player_UI\Mgr\player.h"
#include "App\Player\Player_UI\Mgr\PlayerListMgr.h"
class MainWindow;

class CMainMgr : public IMgr, public IMp3EventCallback
{
public:
	CMainMgr();
	~CMainMgr();

	bool  Initialize();
    void  Initialize2();
	bool  Release();

	HWND  GetMainWnd();

	void  Play(IPlayerListItemInfo* pItem);	
	void  Play();
	void  Stop();
	void  Pause();
	void  PlayNext();
	void  PlayPrev();

	bool  InitVisualization();
	bool  SetVisualizationType(int nType);
	bool  SetVisualizationFps(int nFps);
	bool  SetVisualizationBkgndBmp(HBITMAP hBitmap);
	bool  SetVisualizationBkgndBmpAndRect(HBITMAP hBitmap, RECT* prc);
	HBITMAP  GetVisualSnapshot() { return ::mp3_get_visual_snapshot(); }
	void  ReleaseVisualSnapshot() { ::mp3_release_visual_snapshot(); }

	void  HandleEvent(IMgr* pSource, int nEventType, int nEventId, WPARAM wParam, LPARAM lParam);

    bool  SetMute(bool bMute);
	bool  SetVolumn(long lPercent);
	bool  SetPlayProgressPercent(int nPercent);

	void  HandlePlayingFile();     
	bool  SavePlayingFileInfo();

    void  OnSkinChanged(const TCHAR* szNewSkinName);
	void  OnMainWindowDestroy();

protected:	
	virtual void  on_mp3_finish();
    friend  void  UITimerProc_Progress(UINT_PTR nTimerID, UI::TimerItem* pTimerItem);

protected:
	MainWindow*   m_pMainWindow;
	IPlayerListItemInfo*  m_pCurPlayingItem;  // ��ǰ���ڲ�ŵ��ļ�, TODO: �Ƿ�Ӧ�ÿ���ֻ����һ�����ݣ�����Mgrֱ�����ø����ݣ��������Լ�Ҳ����һ�ݡ�����Ұָ��������ݲ�ͬ��
    UINT_PTR      m_nProgressTimerId;
};