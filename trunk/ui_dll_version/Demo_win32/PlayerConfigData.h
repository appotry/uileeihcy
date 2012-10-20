#pragma once

// ≈‰÷√ ˝æ›
class CConfigData
{
public:
	CConfigData();

	struct Visual
	{
		int  m_nType;
		int  m_nFps;

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