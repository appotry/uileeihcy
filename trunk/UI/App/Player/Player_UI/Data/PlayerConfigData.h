#pragma once
#include "3rd\markup\markup.h"

// ��������
class CConfigData
{
public:
	CConfigData();

    struct Skin
    {
        String  m_strActiveSkinName;  // ��ǰʹ�õ�Ƥ��������

        bool  m_bDirty;
    }skin;

	struct Player
	{
		bool  m_bMute;         // �Ƿ���
		byte  m_byteVolumn;    // ��ǰ����  0 - 100 ���ٷ���
		char  m_byteBalance;   // ����������--ƽ�⣬ -100 - 100���ٷ���
		byte  m_bytePlayMode;  // �����б���ģʽ���μ�PLAY_MODE
        bool  m_bWndTopMost;   // �����ö���ʾ
		String  m_strPlayingFileName;  // ��󲥷ŵ��ļ�·��
		int   m_nPlayingTime;  // ��ǰ��Ŀ����λ��

		bool  m_bDirty;
	}player;
	struct Visual
	{
		int  m_nType;         // Ƶ��ͼ���ͣ��μ�E_VISUALIZATION_TYPE
		int  m_nFps;          // Ƶ��ͼ�ķ����ٶ�

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