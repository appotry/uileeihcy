#pragma once
#include "LogItem.h"
#include "LogXmlParse.h"

class LogManager
{
public:
	LogManager(void);
	~LogManager(void);

public:
	bool    Create(BSTR bstrXmlPathh);
	bool    Load(BSTR bstrXmlPath);
	bool    TestCanLog(long nCookie, LOG_LEVEL logLevel);
	void    Log(long nCookie, LOG_LEVEL logLevel, BSTR bstrContent );
	void    EnableLog(bool bEnable);
	bool    IsEnable();
	bool    Save();
	bool    IsDirty() { return m_bDirty; }

	int     AddLogItem(LOGITEM* pLogItem);
	bool    ModifyLogItem(long lCookie, LOGITEM* pLogItem);
	bool    DeleteLogItem(long lCookie);

	int     GetLogItemCount();
	ILogItem*  GetLogItemByIndex(int nIndex);
	ILogItem*  GetLogItemByName(BSTR bstrLogItemName);
	ILogItem*  GetLogItemByCookie(long lCookie);

private:
	CRITICAL_SECTION   m_CriticalSection;   // ������־ģ��ͬ�������ڵ�����ģ�鹲ͬ�����ͬһ���ļ������
	ILogParse*         m_pLogParse;
	LogRes             m_resLog;
	
	bool               m_bDirty;    // �Ƿ���Ҫ����
};
