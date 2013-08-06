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
	virtual void  on_mp3_finish() = 0;
};

PLAYER_UIP_API bool  mp3_init(HWND hMainWnd);
PLAYER_UIP_API bool  mp3_play();
PLAYER_UIP_API bool  mp3_pause();
PLAYER_UIP_API bool  mp3_stop();
PLAYER_UIP_API bool  mp3_set_file(const TCHAR* szPath);
PLAYER_UIP_API bool  mp3_release();
PLAYER_UIP_API bool  mp3_add_event_callback(IMp3EventCallback* p);
PLAYER_UIP_API bool  mp3_set_cur_pos(double percent);
PLAYER_UIP_API bool  mp3_get_cur_pos(double* pdSeconds, double* pdPercent);
PLAYER_UIP_API bool  mp3_set_volumn(long lPercent);
PLAYER_UIP_API bool  mp3_mute(bool bMute);
PLAYER_UIP_API bool  mp3_set_pan(long lPanPercent);
PLAYER_UIP_API bool  mp3_enable_eq(bool bEnable);
PLAYER_UIP_API bool  mp3_reset_eq();
PLAYER_UIP_API bool  mp3_set_eq(E_EQ_FREQ eFreq, int nValue);
PLAYER_UIP_API bool  mp3_set_visualization(VisualizationInfo* pInfo);
PLAYER_UIP_API HBITMAP mp3_get_visual_snapshot();
PLAYER_UIP_API void  mp3_release_visual_snapshot();


enum mp3_mpeg_audio_version
{
    MPEG_VERSION_2_5,
    MPEG_VERSION_Reserved,
    MPEG_VERSION_2,
    MPEG_VERSION_1,
};
enum mp3_layer_type
{
    LAYER_Reserved,
    LAYER_3,
    LAYER_2,
    LAYER_1
};

enum mp3_channel_mode
{
    CHANNEL_MODE_Stereo,       // 立体声
    CHANNEL_MODE_JointStereo,  // 联合立体声(立体声)
    CHANNEL_MODE_DualChannel,  // 双声道(立体声)
    CHANNEL_MODE_Single,       // 单声道
};

interface ITagParseCallback
{
	virtual HRESULT  OnAddAttribute(WCHAR* pName, byte* pValue, int nValueSize, int nDataType) = 0;

	virtual HRESULT  SetTitle(const WCHAR*) = 0;
	virtual HRESULT  SetArtist(const WCHAR*) = 0;
	virtual HRESULT  SetAlbum(const WCHAR*) = 0;
	virtual HRESULT  SetYear(const WCHAR*) = 0;
	virtual HRESULT  SetComment(const WCHAR*) = 0;
	virtual HRESULT  SetGenreString(const WCHAR*) = 0;
	virtual HRESULT  SetGenre(int) = 0;    // 类型
	virtual HRESULT  SetTrack(int) = 0;    // 音轨
    virtual HRESULT  SetBitrate(int nBitrate) = 0;
    virtual HRESULT  SetSapmleRate(int nSample) = 0;
    virtual HRESULT  SetDuration(int) = 0;

	virtual HRESULT  SetFormat(int nMpegVersion, int nLayerType, int nChannelMode) = 0;
};

PLAYER_UIP_API bool  mp3_get_tag(const TCHAR* szPath, ITagParseCallback* pCallback);
PLAYER_UIP_API const wchar_t*  mp3_parse_genre(int n);
