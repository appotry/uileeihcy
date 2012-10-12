

#ifdef PLAYER_UIP_EXPORTS
#define PLAYER_UIP_API __declspec(dllexport)
#else
#define PLAYER_UIP_API __declspec(dllimport)
#endif


// 全局方法


class IMp3EventCallback
{
public:
	virtual void  on_mp3_stop() = 0;
	virtual void  on_mp3_progress_ind(double dSeconds, double dPercent) = 0;
	virtual void  on_mp3_volume_ind(long lVolumn) = 0;  // 0 到 -10000
};

PLAYER_UIP_API bool  mp3_init(HWND hMainWnd);
PLAYER_UIP_API bool  mp3_play();
PLAYER_UIP_API bool  mp3_pause();
PLAYER_UIP_API bool  mp3_stop();
PLAYER_UIP_API bool  mp3_set_file(const String& strPath);
PLAYER_UIP_API bool  mp3_release();
PLAYER_UIP_API bool  mp3_add_event_callback(IMp3EventCallback* p);
PLAYER_UIP_API bool  mp3_set_cur_pos( double percent );
PLAYER_UIP_API bool  mp3_set_volumn( long lPercent );
PLAYER_UIP_API bool  mp3_mute( bool bMute );