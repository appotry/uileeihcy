#pragma once

#include "..\UI_SDK\include\3rd\markup\markup.h"

// 基类
class IParseBase
{
protected:
	IParseBase(const String& strDataSource):m_strDataSource(strDataSource){ }
	virtual ~IParseBase()=0{};

	String   m_strDataSource;    // 数据源，如xml文件路径
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
#if 0 // -- 过期
	bool  ParseLogItems( HMODULE hModule, const String&  strXmlPath,  vector<LogItem*>& vLogItems );
	bool  _makeLogItem( HMODULE hModule, const String& strName, const String& strSwitch, const String& strPath, const String& strLevel, const String& strFlag, const String& strFileFlag, LogItem* pItem );
#endif
};


//
//	类工厂
//
//		由于这些parse对象都是由service对象在析构函数中自己释放的
//		因此将这些对象的生成形式由new/delete改成 create/release，
//		避免使用时产生不对称，只有new而找不到delete
//		另一方面也便于以后的实现类整体替换
//
class IParseClassFactory
{
public:
	static ILogParse*  CreateLogParseInstance(const String& strDataSource){ return new CLogXmlParse(strDataSource); }
};