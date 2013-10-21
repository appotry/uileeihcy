#pragma once
#include "3rd\markup\markup.h"

// 配置数据
class CConfigData
{
public:
	CConfigData();

    struct Skin
    {
        String  m_strActiveSkinName;  // 当前使用的皮肤包名称

        bool  m_bDirty;
    }skin;

	struct Player
	{
		bool  m_bMute;         // 是否静音
		byte  m_byteVolumn;    // 当前音量  0 - 100 ，百分制
		char  m_byteBalance;   // 均衡器设置--平衡， -100 - 100，百分制
		byte  m_bytePlayMode;  // 播放列表播放模式，参见PLAY_MODE
        bool  m_bWndTopMost;   // 窗口置顶显示
		String  m_strPlayingFileName;  // 最后播放的文件路径
		int   m_nPlayingTime;  // 当前曲目播放位置

		bool  m_bDirty;
	}player;
	struct Visual
	{
		int  m_nType;         // 频谱图类型，参见E_VISUALIZATION_TYPE
		int  m_nFps;          // 频谱图的分析速度

		bool m_bDirty;
	}visual;


    void  SetSkinActiveName(const TCHAR* szName);
    void  SetPlayerMute(bool b);
};

//////////////////////////////////////////////////////////////////////////


class CPlayerConfigData
{
public:
	~CPlayerConfigData();

	bool  Load(CConfigData* pData);
	bool  Save(CConfigData* pData);

private:
	void  GetConfigXmlPath(String& str);
	void  SaveVisual(CConfigData* pData);

private:
	CMarkup  m_xml;
};