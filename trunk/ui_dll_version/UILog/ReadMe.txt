/*
**	功能
**		1. 支持多个模块的日志输出
**		2. 支持输出到Console上实时查看日志
**      3. 支持循环输出日志
**      4. 支持添加日期名输出日志
**		5. xml的大小写无关
**  
**  未实现功能
**		1. 日志分析工具
**
**	Remark
**		由谁负责写日志？调用者还是实现者？
**		==》由实现者负责写日志，因为只有实现者才知道具体错在哪里
**			调用者只需要负责判断返回值即可
**
*/

调用方法：

   ILogPtr  m_pLogOK;
   
   if( NULL==m_pLogOK )
	{
		HRESULT hr = m_pLogOK.CreateInstance( __uuidof(Log) );
		m_pLogOK->Load( (LONG) _Module.m_hInst, L"E:\\编程\\workingpath\\ui\\project\\Debug\\test.xml");
	}
	    
	m_pLogOK->Log( L"test", 0, L"test");
	

配置文件

<root>                                                    // 根名称，可以任意名称。程序将去根结点下的<log>信息 
    <log  switch="on/off">                                // log，switch用来控制整个模块的开关状态
	        <item                                         //
			 name="moduleName"                            // 一个日志输出项，模块的名称
			 switch="on/off"                              // 该模块日志输出开关
             level="debug/info/warn/error/fator"          // 该日志输出项的输出级别  
             path="log.log"                               // 日志文件路径，当第二个字符为:时，默认为绝对路径，否则为相对路径
             flag="0x0"                                   // 日志flag
             fileflag="0x0"                               // 输出文件flag
        />        
    </log>
</root>