#pragma once

#include "..\UI_SDK\include\3rd\markup\markup.h"

// ����
class IParseBase
{
protected:
	IParseBase(const String& strDataSource):m_strDataSource(strDataSource){ }
	virtual ~IParseBase()=0{};

	String   m_strDataSource;    // ����Դ����xml�ļ�·��
public:

	void  Release(){ delete this; }
	const String& GetDataSourceStringRef() { return m_strDataSource; }
};

class ILogParse : public IParseBase
{
protected:
	ILogParse( const String& strDataSource ):IParseBase(strDataSource){}
	virtual ~ILogParse() {};

public:
	virtual  bool Create() = 0;
	virtual  bool Save( CPojo_Log*  pLogItem ) = 0;
	virtual  bool Load( CPojo_Log*  pLogItem ) = 0;
};

class CLogXmlParse : public ILogParse
{
public:
	CLogXmlParse(const String& strDataSource);
	~CLogXmlParse(void);

public:
	virtual  bool Create();
	virtual  bool Save( CPojo_Log*  pLog );
	virtual  bool Load( CPojo_Log*  pLog );

private:
#if 0 // -- ����
	bool  ParseLogItems( HMODULE hModule, const String&  strXmlPath,  vector<LogItem*>& vLogItems );
	bool  _makeLogItem( HMODULE hModule, const String& strName, const String& strSwitch, const String& strPath, const String& strLevel, const String& strFlag, const String& strFileFlag, LogItem* pItem );
#endif
};


//
//	�๤��
//
//		������Щparse��������service�����������������Լ��ͷŵ�
//		��˽���Щ�����������ʽ��new/delete�ĳ� create/release��
//		����ʹ��ʱ�������Գƣ�ֻ��new���Ҳ���delete
//		��һ����Ҳ�����Ժ��ʵ���������滻
//
class IParseClassFactory
{
public:
	static ILogParse*  CreateLogParseInstance(const String& strDataSource){ return new CLogXmlParse(strDataSource); }
};