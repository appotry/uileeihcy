#pragma  once

#ifdef PLAYER_UIP_EXPORTS
#define PLAYER_UIP_API __declspec(dllexport)
#else
#define PLAYER_UIP_API __declspec(dllimport)
#endif

enum E_EQ_FREQ
{
	EQ_FREQ_31,
	EQ_FREQ_63,
	EQ_FREQ_125,
	EQ_FREQ_250,
	EQ_FREQ_500,
	EQ_FREQ_1K,
	EQ_FREQ_2K,
	EQ_FREQ_4K,
	EQ_FREQ_8K,
	EQ_FREQ_16K,
	EQ_FREQ_PREAMP,
};
#define EQ_FREQ_COUNT 11

enum E_VISUALIZATION_TYPE
{
	VISUALIZATION_NONE,       // 不显示
	VISUALIZATION_SPECTRUM,   // 频谱图
	VISUALIZATION_WAVE,       // 波形图
	VISUALIZATION_XXX,        
};
#define VI_MASK_HWND   0x0001
#define VI_MASK_RECT   0x0002
#define VI_MASK_TYPE   0x0004
#define VI_MASK_FPS    0x0008
#define VI_MASK_BKGND_BMP    0x0080
#define VI_MASK_FOREGND_BMP  0x0100

#define VI_MASK_SPECTRUM_BAND_COUNT  0x0010
#define VI_MASK_SPECTRUM_BAND_WIDTH  0x0020
#define VI_MASK_SPECTRUM_GAP_WIDTH   0x0040

struct VisualizationInfo
{
	VisualizationInfo() { memset(this, 0, sizeof(VisualizationInfo)); }

	UINT     nMask;

	HWND     hWnd;
	RECT     rcRender;
	HBITMAP  hBkgndBmp;
	HBITMAP  hForegndBmp;
	E_VISUALIZATION_TYPE eType;
	int      nFps;

	int      nSpectrumBandCount;
	int      nSpectrumBandWidth;
	int      nSpectrumGapWidth;
};


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
PLAYER_UIP_API bool  mp3_set_pan(long lPanPercent);
PLAYER_UIP_API bool  mp3_enable_eq(bool bEnable);
PLAYER_UIP_API bool  mp3_reset_eq();
PLAYER_UIP_API bool  mp3_set_eq(E_EQ_FREQ eFreq, int nValue);
PLAYER_UIP_API bool  mp3_set_visualization(VisualizationInfo* pInfo);
PLAYER_UIP_API HBITMAP mp3_get_visual_snapshot();
PLAYER_UIP_API void    mp3_release_visual_snapshot();