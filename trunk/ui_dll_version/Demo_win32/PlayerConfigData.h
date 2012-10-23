#pragma once

// 配置数据
class CConfigData
{
public:
	CConfigData();

	struct Player
	{
		bool m_bMute;         // 是否静音
		byte m_byteVolumn;    // 当前音量  0 - 100 ，百分制
		char m_byteBalance;   // 均衡器设置--平衡， -100 - 100，百分制

		bool m_bDirty;
	}player;
	struct Visual
	{
		int  m_nType;         // 频谱图类型，参见E_VISUALIZATION_TYPE
		int  m_nFps;          // 频谱图的分析速度

		bool m_bDirty;
	}visual;
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