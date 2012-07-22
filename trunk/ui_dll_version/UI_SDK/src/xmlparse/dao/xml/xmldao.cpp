#include "StdAfx.h"
#include <atlconv.h>


#if 0

project.uiproj <- ProjectXmlParse
<?xml version="1.0" encoding="utf-8" ?>
<ui>
	<proj  select="mediaplayer">                                        // 当前工程使用哪个界面皮肤
		<skin  name="mediaplayer" path="skin\mediaplayer\ui.xml"/>      // 一项皮肤配置信息，名称+路径
		<skin  name="orange" path="skin\orange\ui.xml"/>
	</proj>
	
	<log  swtich="on/off" >                                             // 当前工程中的日志配置信息, 全局日志开关，默认为off
		<item 
			name=""                                                     // 日志名称
			path=""                                                     // 日志文件输出路径
			flag="0x "                                                  // 日志打印方式
			fileflag="0x "                                              // 文件生成方式
			level=""                                                    // 日志输出级别
			switch="on/off"                                             // 该项日志开关，默认为off
		/>		
	</log>
</ui>

ui.xml  <- UIXmlParse

<?xml version="1.0" encoding="utf-8" ?>
<ui>
	<log>
		<item name="ui" path="log\ui.log" level="debug" flag="0x3" flag="0x3" fileflag="0x0A000008"/>
	</log>

	<xml>
		<!-- 图片资源配置文件路径 -->
		<img>img.xml</img>

		<!-- 颜色资源配置文件路径 -->
		<color>col.xml</color>

		<!-- 字体资源配置文件路径 -->
		<font>font.xml</font>

		<!-- 国际化配置文件路径 -->
		<i18n></i18n>

		<!-- 控件样式配置文件路径 -->
		<style></style>

		<!-- 控件位置配置文件路径 -->
		<pos></pos>
	</xml>
</ui>



img.xml <- ImgXmlParse 

<?xml version="1.0" encoding="utf-8" ?>
<...>
	<img>
		<item id="btn_4.png">img\btn.png</item>	   // id: 图片id，可起为图片名称    node data:图片路径，程序可自己转化成绝对路径
	</img>
</...>



font.xml <- FontXmlParse 

<?xml version="1.0" encoding="utf-8" ?>
<...>
	<font>
		<item id="songti"         // id
			fontface="宋体"       // 字体名称       (默认SYSTEM)
			height="9"            // 字体大小       (默认9)
			orientation="0"       // 字体角度       (默认0)
			bold="false"          // 是否粗体显示    0/1(默认0)
			italic="false"        // 是否斜体显示    0/1(默认0)
			underline="false"     // 是否下划线显示  0/1(默认0)
			strikeout="false"     // 是否删除线显示  0/1(默认0)
		></item>
	 </font>
 </...>


 color.xml <- ColorXmlParse

 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <color>
		<item id="black">255,255,255</item>			
	 </color>
 </...>


 layout.xml <- LayoutXmlParse
 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <layout>
		 <classname                         // object对应的xml名称
			 id=""                          // object的id，全局唯一，自动匹配style中的id selector
			 styleclass=""                  // object的样式类型，自动匹配style中的class selector，多个之间使用,分隔
		 >
		 </classname>
	 </layout>
 </...>


 style.xml <- CXmlStyleParse
 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <style>
		 <class                         // CLASS选择器，具有通用性
			 id=""                      // CLASS选择器ID，用于外部引用时指定
			 inherit="a,b,c"            // 继承属性，多个继承关系用,分开
			 key1="value1"              // 属性1
			 key2="value2"              // 属性2
		 />
		 <id                            // ID选择器，具有唯一性
			 id=""                      // ID选择器ID，用于外部引用时指定
			 inherit="a,b,c"            // 继承属性，多个继承关系用,分开
			 key1="value1"              // 属性1
			 key2="value2"              // 属性2
		 /> 
		 <~tag~                         // 标签选择器，可自动匹配对应类型的标签
			 inherit="a,b,c"            // 继承属性，多个继承关系用,分开
			 key1="value1"              // 属性1
			 key2="value2"              // 属性2
		 />

	 </style>
 </...>

 style.xml <- StyleXmlParse
 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <layout>
		 <classname                         // object对应的xml名称，不区分大小写

			 /* object 对象需要的属性列表 */

			 id=""                      // 对象id
			 width=""                   // 对象宽度，可以为 auto | 数值，（默认 auto）
			 height=""                  // 对象的高度，可以为 auto | 数值，（默认 auto）
			 margin=""                  // 对象的外边距，（默认为全0)
			 padding=""                 // 对象的内边距，（默认为全0)
			 minwidht=""                // 对象的最小宽度，（未实现)
			 minheight=""               // 对象的最小高度，（未实现)
			 maxwidth=""                // 对象的最大宽度，（未实现)
			 maxheight=""               // 对象的最大高度，（未实现)

			 erasebkgnd.type=""         // 对象背景绘制方式：color | gradienth | gradientv | imagesimple | 
			 erasebkgnd.color=""
			 erasebkgnd.colorfrom=""
			 erasebkgnd.colorto=""
			 erasebkgnd.image=""
			 erasebkgnd.bkcolor=""

			 cursor=""
			 font=""

			 /* panel 对象需要的属性列表 */

			 layout=""                  // 布局类型，可取值：stack，grid，dock，canvas，card，默认为canvas

			 /* 各种布局类型需要的属性列表 */

			 /* stack布局 */
			 layout.direction=""        // 方向，可以为lefttoright｜righttoleft｜leftandright｜toptobottom｜bottomtotop｜topandbottom，默认为lefttoright
			 layout.dock=""             // 如果direction取为leftandright/topandbottom时，依靠在哪边，可以为 left|right|top|bottom

			 /* canvas布局 */
			 layout.left=""             // 距离左边缘的大小，（默认为0）
			 layout.right=""            // 距离右边缘的大小，当存在layout.left时，优先取layout.left
			 layout.top=""              // 距离上边缘的大小，（默认为0）
			 layout.bottom=""           // 距离下边缘的大小，当存在layout.top时，优先取layout.top

			 /* grid布局 */
			 layout.width=""            // grid布局的各列的宽度定义，可取为：100,*,*,auto。逗号的数量决定了列数（默认为*）
			 layout.height=""           // grid布局的各列的高度定义，可取为：100,*,*,auto。逗号的数量决定了行数（默认为*）
			 layout.row=""              // 对象所在行，（默认为0）
			 layout.col=""              // 对象所在列，（默认为0）
			 layout.rowspan=""          // 对象占据的行数量，（默认为1）
			 layout.colspan=""          // 对象占据的列数量，（默认为1）

			 /* dock布局 */
			 layout.dock=""             // 对象停靠的方向，可取为：left,right,top,bottom,center
		 >
		 </classname>
	 </layout>
 </...>


 窗口类
 <classname
	
	 /* window对象需要的属性列表 */

	 font=""                    // 窗体使用的字体

	 /*  背景透明属性列表*/

	 transparentmaskcolor=""    // FrameWindowBackgroundRender特有，窗口透明色（默认粉色255,0,255) [默认]
	 transparentmaskalpha=""    // FrameWindowBackgroundRender特有，指定将小于多少alpha值的区域抠除掉 
	>
</classname

 控件基类control属性
 <classname
	 transparentbk="0/1"               // 控件的背景是否是透明的，透明的控件刷新时需要重新刷新整个窗口
 </classname>

 windows控件封装
 <HwndHost 
     controlid=""                       // 要封装的控件ID值，10进制
 >
 </HwndHost>

 控件属性，不包括基本的object属性
 <Button 
	 btnbkrender=""                     // 按钮渲染类型 color,theme,image
	 drawfocus=""                       // 是否绘制焦点状态 0,1
	 autosize=""                        // 按钮大小自动类型 content(默认),bkimage,bkimagehcontentw
	
	 bknormalcol=""                     // color bkrender类型的背景色 -- color id
	 bkhovercol=""
	 bkpresscol=""
	 bkdisablecol=""
	 bordernormalcol=""                 // color bkrender类型的边框色 -- color id
	 borderpresscol=""
	 borderhovercol=""
	 borderdisablecol=""
	 bkrenderflag=""                    // 需要绘制哪些状态的color 1|2|4|8，默认为6，绘制hover和press状态

	 bkimage=""                         // 按钮四状态图片
	 imagedirection=""                  // bkimage的方向 x,y

	 icon=""                            // 按钮图标--image id
	 icontype=""                        // 图标绘制类型，目前仅支持 static，8state还没有扩展 gif, 4state

	 text=""                            // 按钮标题
	 texttype=""                        // 文本绘制类型 singleline（默认）
	 >
 </Button>

 静态文本控件
 <Lable>
	 text=""                            // 文本内容
	 font=""                            // 文本使用的字体
	 color=""                           // 文本使用的颜色
	 halign=""                          // 横向对齐方式 left right center
	 valign=""                          // 纵向对齐方式 top bottom center
 </Lable>

 <ImageProgressCtrl
	 fore_image=""
	 bkgnd_image=""
	 btn.*=""
 >
 </ImageProgressCtrl>

 groupbox
 <GroupBox
	 text=""                            // 文本内容
	 font=""                            // 文本使用的字体
	 color=""                           // 文本使用的颜色
	 halign=""                          // 横向对齐方式 left right center
	 valign=""                          // 纵向对齐方式 top bottom center
	 borderimage=""                     // 边框图片
 >
 </GroupBox>

 // 编辑框
 <Edit
	color=""
	selectcolor=""
	selectbkcolor=""
	maxlength=""
 >
 </Edit>
#endif

//////////////////////////////////////////////////////////////////////////
//	
//	全局函数定义

//
// 加载一个xml，(并将内容全部转换成小写[废弃])
//
bool LoadXml_ToLower( CMarkup* pXml, const String& strDataSource )
{
	if( NULL == pXml )
		return false;

	bool bRet = pXml->Load( strDataSource );
	if( false == bRet )
	{
		UI_LOG_ERROR( _T("XmlParse::LoadXml, failed to load xml:%s"), strDataSource.c_str() );
		return false;
	}

	//////////////////////////////////////////////////////////////////////////
	// 统一切换为小写字母，防止配置大小写错误 注：由于涉及到一些英文文本内容配置，如text,tooltip
	// 因此不能随便就能内容全部转成小写

	//String content = pXml->GetDoc();
	//transform(content.begin(),content.end(),content.begin(),tolower);
	//return pXml->SetDoc(content);
	return bRet;
}

//
//	获取一个路径对应的临时文件路径
//
String Get_TempFile( const String& strDataSource )
{
	return strDataSource + TEMP_FILE_EXT;
}

// 将CMarkup中的内容保存到一个temp文件中，用于与外界共享
bool Commit_2_TempFile(CMarkup& markup, const String& strDataSource)
{
	String strTempFilePath = Get_TempFile(strDataSource);

// #ifdef _UNICODE
// 	wofstream f;
// 	//*setlocale(LC_ALL,"chs");*/
// 	locale loc("chs");
// 	f.imbue(loc);
// #else
// 	ofstream f;
// #endif
// 
// 	f.open(strTempFilePath.c_str(), ios_base::out );
// 	if( f.fail() )
// 		return false;
// 
// 	f << markup.GetDoc().c_str();
// 	UIASSERT(f.good());
// 
// 	f.close();

	markup.Save(strTempFilePath);
#ifdef _DEBUG
#else
	::SetFileAttributes(strTempFilePath.c_str(), FILE_ATTRIBUTE_HIDDEN );
#endif

	return true;
 }
// 从一个temp文件中重新加载数据到CMarkup中
bool Update_From_TempFile(CMarkup& markup, const String& strDataSource)
{
	return false;
}

bool Delete_TempFile(const String& strDataSource)
{
	String strTempFilePath = Get_TempFile(strDataSource);
	if( PathFileExists(strTempFilePath.c_str() ))
	{
		::DeleteFile(strTempFilePath.c_str());
	}
	return true;
}
//////////////////////////////////////////////////////////////////////////


CXmlProjectParse::CXmlProjectParse( const String& strDataSource ):IProjectParse(strDataSource)
{
	m_strDataSource = strDataSource;
}
CXmlProjectParse::~CXmlProjectParse()
{

}

bool CXmlProjectParse::Create()
{	
	::DeleteFile( m_strDataSource.c_str() );
	if( FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		return Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_PROJECT);
	}
	return true;
}
bool CXmlProjectParse::Save( CPojo_Project*  pProject )
{
	if( NULL == pProject )
		return false;

	CMarkup  xml;
	if( false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlProjectParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_PROJECT) )            
		{
			xml.RemoveElem() ;
		}
		
		if( false == xml.InsertElem( XML_PROJECT ))         break;
		if( false == xml.SetAttrib( XML_PROJECT_NAME, pProject->GetProjectName()))  break;
		if( false == xml.SetAttrib( XML_PROJECT_SELECT, pProject->GetActiveSkinName() ) )  break;

		if( false == xml.IntoElem() )                       break;
		
		// 插入各skin item 信息
		int   nCount   = (int)pProject->GetSkinItemCount();
		bool  bLoopRet = true;

		for ( int i = nCount-1; i >= 0; i-- )
		{
			CPojo_ProjectSkinItem* pSkinItem = pProject->GetSkinItem(i);
			if( NULL == pSkinItem )
			{
				UI_LOG_WARN(_T("CXmlProjectParse::Save  GetSkinItem return NULL, nIndex=%d"), i );
				continue;
			}

			if( false == xml.InsertElem( XML_PROJECT_SKIN) )  { bLoopRet = false;  break; }
			if( false == xml.SetAttrib( XML_PROJECT_SKIN_NAME, pSkinItem->GetSkinName() ))     { bLoopRet = false;  break; }

			// 计算ui.xml相对于 .uiproj文件的相对路径
			TCHAR szRelative[MAX_PATH] = _T("");
			Util::CalcRelativePathToFile( m_strDataSource.c_str(), pSkinItem->GetSkinXmlPath().c_str(), szRelative );
			if( false == xml.SetAttrib( XML_PROJECT_SKIN_PATH, szRelative ))  { bLoopRet = false;  break; }
		}

		if( !bLoopRet ) break;

		// 保存文件
		if( false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlProjectParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlProjectParse::Load( CPojo_Project*  pProject )
{
	if( NULL == pProject )
		return false;

	CMarkup  xml;
	if( false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlProjectParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		TCHAR  szText[MAX_STRING_LEN] = _T("");

		if( false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == xml.IntoElem() )                       break;

		if( false == xml.FindElem(XML_PROJECT) )            break;
		pProject->SetProjectName(xml.GetAttrib(XML_PROJECT_NAME));
		pProject->SetActiveSkinName (xml.GetAttrib( XML_PROJECT_SELECT ));

		if( false == xml.IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if( false == xml.FindElem( XML_PROJECT_SKIN ) ){ break; }

			CPojo_ProjectSkinItem*  pItem = new CPojo_ProjectSkinItem;
			pItem->SetSkinName( xml.GetAttrib( XML_PROJECT_SKIN_NAME ) );

			// 转换成绝对路径
			String strPath = xml.GetAttrib(XML_PROJECT_SKIN_PATH);
			if( strPath.empty() )                          { bLoopRet = false; break; }
			if( Util::IsFullPath(strPath.c_str()) )
			{
				pItem->SetSkinXmlPath(strPath);
			}
			else
			{
				TCHAR szFull[MAX_PATH] = _T("");
				TCHAR szProjDir[MAX_PATH] = _T("");
				
				Util::GetPathDir(m_strDataSource.c_str(), szProjDir);
				Util::CalcFullPathByRelative(szProjDir, strPath.c_str(), szFull );
				pItem->SetSkinXmlPath( szFull );
			}

			if( false == pProject->AddSkinItem(pItem) )    { bLoopRet = false; break; }
		}

		if( false == bLoopRet ) break;
		
		bRet = true;
	}
	while(0);

	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlProjectParse::Load  failed!"));
	}
	return bRet;

}

//////////////////////////////////////////////////////////////////////////


CXmlSkinInfoParse::CXmlSkinInfoParse( const String& strDataSource ) : ISkinInfoParse(strDataSource)
{
	m_strDataSource = strDataSource;
}
CXmlSkinInfoParse::~CXmlSkinInfoParse()
{

}

bool CXmlSkinInfoParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if( FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		return Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_XML);
	}
	return true;
}

bool CXmlSkinInfoParse::Save( CPojo_SkinInfo*  pSkinInfo )
{
	if( NULL == pSkinInfo )
		return false;

	CMarkup  xml;
	if( false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlSkinInfoParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_XML) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_XML ))             break;
		if( false == xml.IntoElem() )                       break;

		// 插入各skin elem信息
		TCHAR szRelative[MAX_PATH] = _T("");
		
		if( !pSkinInfo->GetImageXmlPath().empty() )
		{
			Util::CalcRelativePathToFile(m_strDataSource.c_str(), pSkinInfo->GetImageXmlPath().c_str(), szRelative);
			if( false == xml.InsertElem( XML_IMG,    szRelative))    break;
		}

		if( !pSkinInfo->GetColorXmlPath().empty() )
		{
			Util::CalcRelativePathToFile(m_strDataSource.c_str(), pSkinInfo->GetColorXmlPath().c_str(), szRelative);
			if( false == xml.InsertElem( XML_COLOR,    szRelative))    break;
		}
	
		if( !pSkinInfo->GetFontXmlPath().empty() )
		{
			Util::CalcRelativePathToFile(m_strDataSource.c_str(), pSkinInfo->GetFontXmlPath().c_str(), szRelative);
			if( false == xml.InsertElem( XML_FONT,   szRelative))    break;
		}

		if( !pSkinInfo->GetStyleXmlPath().empty() )
		{
			Util::CalcRelativePathToFile(m_strDataSource.c_str(), pSkinInfo->GetStyleXmlPath().c_str(), szRelative);
			if( false == xml.InsertElem( XML_STYLE,  szRelative))    break;
		}

		if( !pSkinInfo->GetLayoutXmlPath().empty() )
		{
			Util::CalcRelativePathToFile(m_strDataSource.c_str(), pSkinInfo->GetLayoutXmlPath().c_str(), szRelative);
			if( false == xml.InsertElem( XML_LAYOUT, szRelative))    break;
		}

		if( !pSkinInfo->GetI18nXmlPath().empty() )
		{
			Util::CalcRelativePathToFile(m_strDataSource.c_str(), pSkinInfo->GetI18nXmlPath().c_str(), szRelative);
			if( false == xml.InsertElem( XML_I18N,   szRelative))    break;
		}
		
		// 保存文件
		if( false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlSkinInfoParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlSkinInfoParse::Load( CPojo_SkinInfo*  pSkinInfo )
{
	if( NULL == pSkinInfo )
		return false;
	
	CMarkup  xml;
	if( false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlSkinInfoParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		String strPath;

		if( false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == xml.IntoElem() )                       break;

		if( false == xml.FindElem(XML_XML) )                break;
		if( false == xml.IntoElem() )                       break;
		xml.SavePos();    //  CMarkup的解析是从上至下的，而xml中的各节点的顺序不是固定的

		TCHAR szProjDir[MAX_PATH] = _T("");
		TCHAR szFullPath[MAX_PATH] = _T("");
		Util::GetPathDir(m_strDataSource.c_str(), szProjDir);

		if( false == xml.FindElem( XML_IMG ) )              break;
		strPath = xml.GetData();
		if( strPath.empty() )                               break;
		
		if( Util::IsFullPath(strPath.c_str()))
		{
			pSkinInfo->SetImageXmlPath(strPath);
		}
		else
		{
			Util::CalcFullPathByRelative( szProjDir, strPath.c_str(), szFullPath );
			pSkinInfo->SetImageXmlPath(szFullPath);
		}
		

		xml.RestorePos();
		if( false == xml.FindElem( XML_COLOR ) )              break;
		strPath = xml.GetData();
		if( strPath.empty() )                               break;

		if( Util::IsFullPath(strPath.c_str()))
		{
			pSkinInfo->SetColorXmlPath(strPath);
		}
		else
		{
			Util::CalcFullPathByRelative( szProjDir, strPath.c_str(), szFullPath );
			pSkinInfo->SetColorXmlPath(szFullPath);
		}

		xml.RestorePos();
		if( false == xml.FindElem( XML_FONT ) )             break;
		strPath = xml.GetData();
		if( strPath.empty() )                               break;

		if( Util::IsFullPath(strPath.c_str()))
		{
			pSkinInfo->SetFontXmlPath(strPath);
		}
		else
		{
			Util::CalcFullPathByRelative( szProjDir, strPath.c_str(), szFullPath );
			pSkinInfo->SetFontXmlPath(szFullPath);
		}


		xml.RestorePos();
		if( false == xml.FindElem( XML_STYLE ) )            break;
		strPath = xml.GetData();
		if( strPath.empty() )                               break;

		if( Util::IsFullPath(strPath.c_str()))
		{
			pSkinInfo->SetStyleXmlPath(strPath);
		}
		else
		{
			Util::CalcFullPathByRelative( szProjDir, strPath.c_str(), szFullPath );
			pSkinInfo->SetStyleXmlPath(szFullPath);
		}


		xml.RestorePos();
		if( false == xml.FindElem( XML_LAYOUT ) )            break;
		strPath = xml.GetData();
		if( strPath.empty() )                               break;

		if( Util::IsFullPath(strPath.c_str()))
		{
			pSkinInfo->SetLayoutXmlPath(strPath);
		}
		else
		{
			Util::CalcFullPathByRelative( szProjDir, strPath.c_str(), szFullPath );
			pSkinInfo->SetLayoutXmlPath(szFullPath);
		}

// 		xml.RestorePos();
// 		if( false == xml.FindElem( XML_I18N ) )             break;
// 		strPath = xml.GetData();
// 		if( strPath.empty() )                               break;
// 
// 		if( Util::IsFullPath(strPath.c_str()))
// 		{
// 			pSkinInfo->SetI18nXmlPath(strPath);
// 		}
// 		else
// 		{
// 			Util::CalcFullPathByRelative( szProjDir, strPath.c_str(), szFullPath );
// 			pSkinInfo->SetI18nXmlPath(szFullPath);
// 		}

		bRet = true;
	}
	while(0);
	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlSkinInfoParse::Load  failed!"));
	}
	return bRet;
}
//////////////////////////////////////////////////////////////////////////


CXmlImageParse::CXmlImageParse( const String& strDataSource ) : IImageParse(strDataSource)
{
}
CXmlImageParse::~CXmlImageParse()
{
	if( g_pUIApplication->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);;
}
bool CXmlImageParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if( FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if( false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_IMG) )
			return false;
	}

	if( false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}
bool CXmlImageParse::Save( CPojo_Image*  pImageInfo )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}


bool CXmlImageParse::Load( CPojo_Image*  pImageInfo )
{
	if( NULL == pImageInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if( g_pUIApplication->IsDesignMode() )  // 非design模式下，不需要保存xml的内容
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pImageInfo);
	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlImageParse::Load  failed!"));
	}
	else
	{
		if( g_pUIApplication->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;

}

// 从temp文件
bool CXmlImageParse::Reload( CPojo_Image* pImageInfo )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pImageInfo );
}
bool CXmlImageParse::InsertImage(  CPojo_ImageItem *pImageItemInfo )
{
	if( NULL == pImageItemInfo )
		return false;

	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_IMG) )            
		{
			if( false == m_xml.InsertElem( XML_IMG ))         break;
		}
		if( false == m_xml.IntoElem() )                       break;
		if( false == insert_image_2_xml(pImageItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}

bool CXmlImageParse::ModifyImage( CPojo_ImageItem *pImageItemInfo )
{
	if( NULL == pImageItemInfo )
		return false;

	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_IMG) )                break;
		if( false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if( false == m_xml.FindElem(XML_ITEM) )
				break;;
			if( m_xml.GetAttrib(XML_ID) == pImageItemInfo->GetIDRef() )
			{
				TCHAR szRelative[MAX_PATH] = _T("");
				Util::CalcRelativePathToFile(m_strDataSource.c_str(), pImageItemInfo->GetPathRef().c_str(), szRelative );
				if( m_xml.SetData( pImageItemInfo->GetPathRef()) )  
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if( !bLoopRet ) break;

		bRet = true;
	}
	while(0);


	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}

bool CXmlImageParse::RemoveImage( const String& strID )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_IMG) )                break;
		if( false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if( false == m_xml.FindElem(XML_ITEM) )
				break;;
			if( m_xml.GetAttrib(XML_ID) == strID )
			{
				if( m_xml.RemoveElem() )
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if( !bLoopRet ) break;

		bRet = true;
	}
	while(0);

	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}


//
//	忽略原有xml内容进行重写(暂时没有用到)
//
bool CXmlImageParse::clear_save( CPojo_Image*  pImageInfo )
{
	if( NULL == pImageInfo )
		return false;

	CMarkup  xml;
	if( false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlImageParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_IMG) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_IMG ))             break;
		if( false == xml.IntoElem() )                       break;

		// 添加元素
		int  nCount   = (int)pImageInfo->GetImageCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			CPojo_ImageItem*  pItem = pImageInfo->GetImageItem(i);

			if( false == insert_image_2_xml(pItem) ){ bLoopRet = false; break; }
		}
		if( !bLoopRet )  break;

		// 保存文件
		if( false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlImageParse::Save  failed!"));
	}
	return bRet;
}

bool  CXmlImageParse::load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Image *pImageInfo )
{
	if( NULL == pXml || NULL == pImageInfo )
		return false;

	if( false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlImageParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == pXml->FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == pXml->IntoElem() )                       break;

		if( false == pXml->FindElem( XML_IMG ))               break;
		if( false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if( false == pXml->FindElem( XML_ITEM ) )        { break; }

			String strID   = pXml->GetAttrib(XML_ID);
			String strPath = pXml->GetData();
			String strUseSkinHue = pXml->GetAttrib(XML_IMAGE_USESKINHUE);
			bool   bUseSkinHue = 1;
			if( !strUseSkinHue.empty() )
			{
				bUseSkinHue = _ttoi(strUseSkinHue.c_str())?true:false;;
			}

			String strFullPath;
			if( Util::IsFullPath(strPath.c_str()) )
			{
				strFullPath = strPath;
			}
			else
			{
				TCHAR szProjDir[MAX_PATH] = _T("");
				TCHAR szFullPath[MAX_PATH] = _T("");
				Util::GetPathDir(m_strDataSource.c_str(), szProjDir);
				Util::CalcFullPathByRelative( szProjDir, strPath.c_str(), szFullPath );

				strFullPath = szFullPath;
			}

			CPojo_ImageItem* pItem = NULL;
			if( false == pImageInfo->InsertImage( strID, strFullPath, &pItem) )
			{
				UI_LOG_WARN( _T("CXmlImageParse::load_from_file  insert image m_strID=%s, path=%s failed."), strID.c_str(), strFullPath.c_str() );
			}
			pItem->SetUseSkinHue(bUseSkinHue);
		}
		bRet = true;
	} 
	while (0);

	return bRet;
}
bool CXmlImageParse::insert_image_2_xml( CPojo_ImageItem *pImageItemInfo )
{
	TCHAR szRelative[MAX_PATH] = _T("");
	Util::CalcRelativePathToFile(m_strDataSource.c_str(), pImageItemInfo->GetPathRef().c_str(), szRelative );

	if( false == m_xml.AddElem( XML_ITEM, String(szRelative)) )    
		return false;
	if( false == m_xml.SetAttrib( XML_ID, pImageItemInfo->GetIDRef()) )  
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////////


CXmlColorParse::CXmlColorParse( const String& strDataSource ) : IColorParse(strDataSource)
{
	m_strDataSource = strDataSource;
}
CXmlColorParse::~CXmlColorParse()
{
	if( g_pUIApplication->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlColorParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if( FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if( false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_COLOR) )
			return false;
	}

	if( false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}
bool CXmlColorParse::Save( CPojo_Color*  pColorInfo )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlColorParse::clear_save(CPojo_Color* pColorInfo )
{
	if( NULL == pColorInfo )
		return false;

	CMarkup  xml;
	if( false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlColorParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_COLOR) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_COLOR ))             break;
		if( false == xml.IntoElem() )                       break;

		// 添加元素
		int  nCount   = (int)pColorInfo->GetColorCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			CPojo_ColorItem* pItem = pColorInfo->GetColorItem(i);
			if( false == insert_color_2_xml(pItem) ){ bLoopRet = false; break; }
			
		}
		if( !bLoopRet )  break;

		// 保存文件
		if( false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlColorParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlColorParse::Load( CPojo_Color*  pColorInfo )
{
	if( NULL == pColorInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if( g_pUIApplication->IsDesignMode() )  // 非design模式下，不需要保存xml的内容
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pColorInfo);
	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlColorParse::Load  failed!"));
	}
	else
	{
		if( g_pUIApplication->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;
}
bool CXmlColorParse::load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Color *pColorInfo )
{
	if(NULL == pXml || NULL == pColorInfo)
		return false;

	if( false == ::LoadXml_ToLower(pXml, strDataSource))
	{
		UI_LOG_ERROR( _T("CXmlColorParse::Load  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}
	pXml->ResetPos();

	bool bRet = false;
	do 
	{
		if( false == pXml->FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == pXml->IntoElem() )                       break;

		if( false == pXml->FindElem( XML_COLOR ))               break;
		if( false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if( false == pXml->FindElem(XML_ITEM) )       break;

			String strID = pXml->GetAttrib(XML_ID);
			String strValue = pXml->GetData();

			if( false == pColorInfo->InsertColor( strID, strValue ) )
			{
				UI_LOG_WARN(_T("CXmlColorParse::load_from_file insert color failed. m_strID=%s, value=%s"), strID.c_str(), strValue.c_str() );
			}
		}
		if( !bLoopRet )  break;

		bRet = true;
	} 
	while (0);

	if( false == bRet )
	{
		UI_LOG_ERROR(_T("CXmlColorParse::load_from_file  failed!"));
	}
	return bRet;
}

bool CXmlColorParse::Reload( CPojo_Color* pColorInfo )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pColorInfo );
}
bool CXmlColorParse::InsertColor( CPojo_ColorItem *pColorItemInfo )
{
	if( NULL == pColorItemInfo )
		return false;

	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_COLOR) )            
		{
			if( false == m_xml.InsertElem( XML_COLOR ))       break;
		}
		if( false == m_xml.IntoElem() )                       break;
		if( false == insert_color_2_xml(pColorItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlColorParse::ModifyColor( CPojo_ColorItem *pColorItemInfo )
{
	if( NULL == pColorItemInfo )
		return false;

	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_COLOR) )                break;
		if( false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if( false == m_xml.FindElem(XML_ITEM) )
				break;;
			if( m_xml.GetAttrib(XML_ID) == pColorItemInfo->GetIDRef() )
			{
				if( m_xml.SetData( pColorItemInfo->GetColorStringRef() ) )  
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if( !bLoopRet ) break;

		bRet = true;
	}
	while(0);


	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlColorParse::RemoveColor( const String& strID )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_COLOR) )              break;
		if( false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if( false == m_xml.FindElem(XML_ITEM) )
				break;;
			if( m_xml.GetAttrib(XML_ID) == strID )
			{
				if( m_xml.RemoveElem() )
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if( !bLoopRet ) break;

		bRet = true;
	}
	while(0);

	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlColorParse::insert_color_2_xml( CPojo_ColorItem *pColorItemInfo )
{
	if (NULL == pColorItemInfo)
		return false;

	if( false == m_xml.AddElem( XML_ITEM, pColorItemInfo->GetColorStringRef().c_str() ) )   
		return false;
	if( false == m_xml.SetAttrib( XML_ID, pColorItemInfo->GetIDRef().c_str())) 
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////////////


CXmlFontParse::CXmlFontParse( const String& strDataSource ) : IFontParse(strDataSource)
{
	m_strDataSource = strDataSource;
}
CXmlFontParse::~CXmlFontParse()
{
	if( g_pUIApplication->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlFontParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if( FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if( false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_FONT) )
			return false;
	}

	if( false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}
bool CXmlFontParse::Save( CPojo_Font*  pFontInfo )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlFontParse::clear_save( CPojo_Font*  pFontInfo )
{
	if( NULL == pFontInfo )
		return false;

	CMarkup  xml;
	if( false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlFontParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_FONT) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_FONT ))             break;
		if( false == xml.IntoElem() )                       break;

		// 添加元素
		int  nCount   = (int)pFontInfo->GetFontCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			CPojo_FontItem* pItem = NULL;
			pItem = pFontInfo->GetFontItem(i);
			if (NULL == pItem)
			{
				UI_LOG_WARN( _T("CXmlFontParse::Save GetFontItem failed, nIndex=%d"), i );
				continue;
			}

			if( false == this->insert_font_2_xml(pItem) ) 
			{ 
				bLoopRet = false; 
				break; 
			}

		}
		if( !bLoopRet )  break;

		// 保存文件
		if( false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlFontParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlFontParse::Reload( CPojo_Font* pFontInfo )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pFontInfo );	
}

bool CXmlFontParse::Load( CPojo_Font* pFontInfo )
{
	if( NULL == pFontInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if( g_pUIApplication->IsDesignMode() )  // 非design模式下，不需要保存xml的内容
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pFontInfo);
	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlFontParse::Load  failed!"));
	}
	else
	{
		if( g_pUIApplication->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;
}

bool CXmlFontParse::load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Font *pFontInfo )
{
	if(NULL == pXml || NULL == pFontInfo )
		return false;

	if( false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlFontParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}

	pXml->ResetPos();

	bool bRet = false;
	do 
	{
		if( false == pXml->FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == pXml->IntoElem() )                       break;

		if( false == pXml->FindElem( XML_FONT ))              break;
		if( false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if( false == pXml->FindElem( XML_ITEM ) )        { break; }

			String strID = pXml->GetAttrib( XML_ID );
			String strFaceName = pXml->GetAttrib( XML_FONT_FACENAME );
			String strFontHeight = pXml->GetAttrib( XML_FONT_HEIGHT );
			String strOrientation = pXml->GetAttrib( XML_FONT_ORIENTATION );
			String strBold = pXml->GetAttrib( XML_FONT_BOLD );
			String strItalic = pXml->GetAttrib( XML_FONT_ITALIC );
			String strUnderline = pXml->GetAttrib( XML_FONT_UNDERLINE );
			String strStrikeout = pXml->GetAttrib( XML_FONT_STRIKEOUT );

			if( strFaceName.empty() )
			{
				strFaceName = _T("SYSTEM");
			}
			int nFontHeight = 9;
			int lfOrientation = 0;
			int lfEscapement = 0;
			int lfWeight = FW_NORMAL;
			int lfItalic = 0;
			int lfUnderline = 0;
			int lfStrikeOut = 0;

			if(!strFontHeight.empty())
			{
				nFontHeight = _ttoi(strFontHeight.c_str());
			}
			if(!strOrientation.empty())
			{
				lfOrientation = _ttoi(strOrientation.c_str());
			}
			if(!strBold.empty())
			{
				lfWeight = (_ttoi(strBold.c_str())==1)?FW_BOLD:FW_NORMAL;
			}
			if( !strItalic.empty())
			{
				lfItalic = _ttoi(strItalic.c_str());
			}
			if( !strUnderline.empty())
			{
				lfUnderline = _ttoi(strUnderline.c_str());
			}
			if( !strStrikeout.empty())
			{
				lfStrikeOut = _ttoi(strStrikeout.c_str());
			}


			LOGFONT lf;
			::ZeroMemory( &lf, sizeof(lf) );
			_tcsncpy( lf.lfFaceName, strFaceName.c_str(), 31 );
			lf.lfHeight = Util::FontSize2Height( nFontHeight );
			lf.lfOrientation = lfOrientation;
			lf.lfEscapement = lf.lfOrientation;
			lf.lfWeight = lfWeight;
			lf.lfItalic = lfItalic;
			lf.lfUnderline = lfUnderline;
			lf.lfStrikeOut = lfStrikeOut;

			if( false == pFontInfo->InsertFont( strID, &lf ) )
			{
				UI_LOG_WARN( _T("CXmlFontParse::load_from_file insert font failed. m_strID=%s, facename=%s"), strID.c_str(), strFaceName.c_str() );
			}
		}
		if( !bLoopRet )  break;

		bRet = true;
	} 
	while (0);

	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlFontParse::load_from_file  failed!"));
	}
	return bRet;
}

bool CXmlFontParse::InsertFont( CPojo_FontItem *pFontItemInfo )
{
	if( NULL == pFontItemInfo )
		return false;

	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_FONT) )            
		{
			if( false == m_xml.InsertElem( XML_FONT ))        break;
		}
		if( false == m_xml.IntoElem() )                       break;
		if( false == insert_font_2_xml(pFontItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlFontParse::ModifyFont( CPojo_FontItem *pFontItemInfo )
{
	if( NULL == pFontItemInfo )
		return false;

	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_FONT) )                break;
		if( false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if( false == m_xml.FindElem(XML_ITEM) )
				break;;
			if( m_xml.GetAttrib(XML_ID) == pFontItemInfo->GetIDRef() )
			{
				TCHAR szFontFace[32] = _T("");
				pFontItemInfo->GetFaceName(szFontFace);
				if( false == m_xml.SetAttrib( XML_ID,    pFontItemInfo->GetIDRef() ))                      { bLoopRet = false; break; }
				if( false == m_xml.SetAttrib( XML_FONT_FACENAME,    szFontFace ))                          { bLoopRet = false; break; }
				if( false == m_xml.SetAttrib( XML_FONT_HEIGHT,      pFontItemInfo->GetFontSize() ))        { bLoopRet = false; break; }
				if( false == m_xml.SetAttrib( XML_FONT_ORIENTATION, pFontItemInfo->GetFontOrientation() )) { bLoopRet = false; break; }
				if( false == m_xml.SetAttrib( XML_FONT_BOLD,      (int)pFontItemInfo->GetFontBold() ))     { bLoopRet = false; break; }
				if( false == m_xml.SetAttrib( XML_FONT_ITALIC,    (int)pFontItemInfo->GetFontItalic() ))   { bLoopRet = false; break; }
				if( false == m_xml.SetAttrib( XML_FONT_UNDERLINE, (int)pFontItemInfo->GetFontUnderline() )){ bLoopRet = false; break; }
				if( false == m_xml.SetAttrib( XML_FONT_STRIKEOUT, (int)pFontItemInfo->GetFontStrikeout() )){ bLoopRet = false; break; }

				bLoopRet = true;
				break;
			}
		}
		if( !bLoopRet ) break;

		bRet = true;
	}
	while(0);


	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlFontParse::RemoveFont( const String& strID )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_FONT) )                break;
		if( false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if( false == m_xml.FindElem(XML_ITEM) )
				break;;
			if( m_xml.GetAttrib(XML_ID) == strID )
			{
				if( m_xml.RemoveElem() )
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if( !bLoopRet ) break;

		bRet = true;
	}
	while(0);

	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}

bool CXmlFontParse::insert_font_2_xml( CPojo_FontItem *pFontItemInfo )
{
	if (NULL == pFontItemInfo)
		return false;

	if( false == m_xml.AddElem( XML_ITEM ) )   { return false; }

	TCHAR szFontFace[32] = _T("");
	pFontItemInfo->GetFaceName(szFontFace);
	if( false == m_xml.SetAttrib( XML_ID,    pFontItemInfo->GetIDRef() ))                      { return false; }
	if( false == m_xml.SetAttrib( XML_FONT_FACENAME,    szFontFace ))                          { return false; }
	if( false == m_xml.SetAttrib( XML_FONT_HEIGHT,      pFontItemInfo->GetFontSize() ))        { return false; }
	if( false == m_xml.SetAttrib( XML_FONT_ORIENTATION, pFontItemInfo->GetFontOrientation() )) { return false; }
	if( false == m_xml.SetAttrib( XML_FONT_BOLD,      (int)pFontItemInfo->GetFontBold() ))     { return false; }
	if( false == m_xml.SetAttrib( XML_FONT_ITALIC,    (int)pFontItemInfo->GetFontItalic() ))   { return false; }
	if( false == m_xml.SetAttrib( XML_FONT_UNDERLINE, (int)pFontItemInfo->GetFontUnderline() )){ return false; }
	if( false == m_xml.SetAttrib( XML_FONT_STRIKEOUT, (int)pFontItemInfo->GetFontStrikeout() )){ return false; }

	return true;
}
//////////////////////////////////////////////////////////////////////////


CXmlStyleParse::CXmlStyleParse( const String& strDataSource ) : IStyleParse(strDataSource)
{
	m_strDataSource = strDataSource;
}
CXmlStyleParse::~CXmlStyleParse()
{
	if( g_pUIApplication->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlStyleParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if( FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if( false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_STYLE) )
			return false;
	}
	if( false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}

bool CXmlStyleParse::Save( CPojo_Style* pStyleInfo )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlStyleParse::Load( CPojo_Style* pStyleInfo )
{
	if( NULL == pStyleInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if( g_pUIApplication->IsDesignMode() )  // 非design模式下，不需要保存xml的内容
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pStyleInfo);
	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlStyleParse::Load  failed!"));
	}
	else
	{
		if( g_pUIApplication->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;
}
bool CXmlStyleParse::Reload( CPojo_Style* pStyleInfo )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pStyleInfo );
}

//
//	递归解析一个CPojo_StyleItem的继承数据
//
//	Parameter
//		pTreeItem
//			[in]	要解析的CPojo_StyleItem，pTreeItem代表它的继承依赖树，是树的叶子结点。
//					如果自己重复出现在这个树中，表示出现了死锁继承，无法解析。
//
//		pPojoStyle
//			[in]	当前所有的StyleItem列表
//
//	Return
//		解析成功返回TRUE，失败返回FALSE。只要树中有一个返回FALSE，这棵树全部返回FALSE。
//
bool CXmlStyleParse::parse_inherit( tree<CPojo_StyleItem*>* pTreeItem, CPojo_Style* pPojoStyle )
{
	if( NULL == pTreeItem || NULL == pTreeItem->data )
		return false;

	CPojo_StyleItem* pStyleItem = pTreeItem->data;

	// 判断pTreeItem是否有继承关系
	int nSize = pStyleItem->GetInheritCount();
	if( 0 == nSize )
		return true;

	// 判断当前这个pTreeItem中整棵树中是不是重复出现了，如果重复出现了则表示死循环了
	{
		tree<CPojo_StyleItem*>* pParentItem = pTreeItem;
		do
		{
			pParentItem = pParentItem->parent;
			if( NULL != pParentItem && pParentItem->data == pStyleItem )
				return false;
		}
		while(pParentItem != NULL);
	}


	for( int i = nSize-1; i >= 0; i-- )
	{
		String strInherit;
		if( false == pStyleItem->GetInheritItem(i, strInherit) )
			continue;

		STYLE_SELECTOR_TYPE type = STYLE_SELECTOR_TYPE_TAG;
		TCHAR szStyleName[MAX_STRING_LEN] = _T("");
		CPojo_StyleItem::ParseInheritString(strInherit, type, szStyleName);

		CPojo_StyleItem* pInheritItem = pPojoStyle->GetStyleItem(type, szStyleName);
		if( NULL == pInheritItem )
			continue;

		tree<CPojo_StyleItem*> t;
		t.parent = pTreeItem;
		t.data   = pInheritItem;

		if( false == this->parse_inherit( &t, pPojoStyle) )
		{
			return false;
		}

		// 解析成功，继承它的属性
		pInheritItem->InheritMyAttributesToAnother(pStyleItem);
		pStyleItem->RemoveInheritItem( strInherit );
	}

	return true;
}

bool CXmlStyleParse::load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Style *pStyleInfo )
{
	if( NULL == pXml || NULL == pStyleInfo )
		return false;

	if( false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlStyleParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}

	pXml->ResetPos();

	bool bRet = false;
	do 
	{
		if( false == pXml->FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == pXml->IntoElem() )                       break;

		if( false == pXml->FindElem( XML_STYLE ))             break;
		if( false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if( false == pXml->FindElem( ) )                  { break; }
			
			CPojo_StyleItem* pStyleItem = new CPojo_StyleItem;

			// 加载所有属性
			for( int j = 0; ; j++ )
			{
				String key = pXml->GetAttribName(j);
				if( _T("") == key )
					break;

				String value = pXml->GetAttrib( key );
				pStyleItem->SetAttribute(key,value);
			}

			// 解析  SELECTOR_TPYE, ID
			String strTagName = pXml->GetTagName();
			if( XML_STYLE_SELECTOR_TYPE_ID == strTagName )
			{
				pStyleItem->SetSelectorType( STYLE_SELECTOR_TYPE_ID );
				
				String strID;
				if( false == pStyleItem->GetAttribute( XML_ID, strID ) )
				{
					UI_LOG_WARN( _T("CXmlStyleParse::load_from_file Can't find the %s attribute of %s"), XML_ID, strTagName.c_str() );
					delete pStyleItem;
					continue;
				}
				pStyleItem->SetID( strID );
				pStyleItem->RemoveAttribute( XML_ID );
			}
			else if( XML_STYLE_SELECTOR_TYPE_CLASS == strTagName )
			{
				pStyleItem->SetSelectorType( STYLE_SELECTOR_TYPE_CLASS );

				String strID;
				if( false == pStyleItem->GetAttribute( XML_ID, strID ) )
				{
					UI_LOG_WARN( _T("CXmlStyleParse::load_from_file Can't find the %s attribute of %s"), XML_ID, strTagName.c_str() );
					delete pStyleItem;
					continue;
				}
				pStyleItem->SetID( strID );
				pStyleItem->RemoveAttribute( XML_ID );
			}
			else
			{
				pStyleItem->SetSelectorType( STYLE_SELECTOR_TYPE_TAG );
				pStyleItem->SetID(strTagName);
			}

			// 解析 inherit 属性
			String strInherit;
			if( pStyleItem->GetAttribute(XML_STYLE_INHERIT, strInherit) )
			{
				pStyleItem->RemoveAttribute(XML_STYLE_INHERIT);  // 不是一个属性，它是需要被扩展的
				pStyleItem->SetInherits( strInherit );
			}

			if( false == pStyleInfo->InsertStyle(pStyleItem) )
			{
				delete pStyleItem;
				pStyleItem = NULL;
			}
		}
		if( !bLoopRet )  break;

		// 在所有的ITEM加载完后，开始解析inherit关系
		int nSize = pStyleInfo->GetStyleCount();
		for ( int i = 0; i < nSize; i++ )
		{
			CPojo_StyleItem* pItem = pStyleInfo->GetStyleItem(i);
			tree<CPojo_StyleItem*>  t;
			t.data = pItem;

			if( false == this->parse_inherit(&t, pStyleInfo) )
			{
				UI_LOG_WARN(_T("CXmlStyleParse::load_from_file parse_inherit failed. style item=%s"), pItem->GetIDRef().c_str() );
			}
		}

		bRet = true;
	} 
	while (0);

	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlStyleParse::load_from_file  failed."));
	}
	return bRet;
}

bool CXmlStyleParse::insert_style_2_xml( CPojo_StyleItem *pStyleItemInfo )
{
	if (NULL == pStyleItemInfo)
		return false;

	STYLE_SELECTOR_TYPE type = pStyleItemInfo->GetSelectorType();
	const String& strID = pStyleItemInfo->GetIDRef();
	const String& strInherit = pStyleItemInfo->GetInheritsRef();

	switch( type )
	{
	case STYLE_SELECTOR_TYPE_ID:
		m_xml.AddElem( XML_STYLE_SELECTOR_TYPE_ID );
		m_xml.AddAttrib( XML_ID, strID );
		break;

	case STYLE_SELECTOR_TYPE_CLASS:
		m_xml.AddElem( XML_STYLE_SELECTOR_TYPE_CLASS );
		m_xml.AddAttrib( XML_ID, strID );
		break;

	default:
		m_xml.AddElem( strID );
		break;
	}

	if( ! strInherit.empty() )
	{
		m_xml.AddAttrib( XML_STYLE_INHERIT, strInherit );
	}
	return true;
}

//
//	查找指定的style item所在的xml位置，忽视当前所在位置，全文搜索
//
bool CXmlStyleParse::find_elem_in_xml( STYLE_SELECTOR_TYPE type, const String& strID )
{
	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_STYLE) )              break;
		if( false == m_xml.IntoElem() )                       break;

		if( false == this->find_elem_under_style(type,strID)) break;

		bRet = true;
	}
	while(0);

	return bRet;
}
//
//	查找指定的style item所在的xml位置，默认当前位置处于<style>下的最开始
//
bool CXmlStyleParse::find_elem_under_style( STYLE_SELECTOR_TYPE type, const String& strID )
{
	bool bLoopRet = false;
	switch( type )
	{
	case STYLE_SELECTOR_TYPE_ID:
		{
			while(1)    
			{
				if( false == m_xml.FindElem(XML_STYLE_SELECTOR_TYPE_ID) )
					break;
				if( m_xml.GetAttrib(XML_ID) == strID )
				{
					bLoopRet = true;
					break;
				}
			}
		}
		break;

	case STYLE_SELECTOR_TYPE_CLASS:
		{
			while(1)    
			{
				if( false == m_xml.FindElem(XML_STYLE_SELECTOR_TYPE_CLASS) )
					break;
				if( m_xml.GetAttrib(XML_ID) == strID)
				{
					bLoopRet = true;
					break;
				}
			}
		}	
		break;

	default:
		{
			if( m_xml.FindElem(strID) )
				bLoopRet = true;
		}
		break;
	}

	return bLoopRet;
}
bool CXmlStyleParse::InsertStyle( CPojo_StyleItem *pStyleItemInfo )
{
	if( NULL == pStyleItemInfo )
		return false;

	UIASSERT(g_pUIApplication->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // 忽略root结点的名称
		if( false == m_xml.IntoElem() )                       break;

		if( false == m_xml.FindElem(XML_STYLE) )            
		{
			if( false == m_xml.InsertElem( XML_STYLE ))       break;
		}
		if( false == m_xml.IntoElem() )                       break;
		if( false == insert_style_2_xml(pStyleItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if( bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlStyleParse::ModifyStyle( CPojo_StyleItem *pStyleItemInfo )
{
	if( NULL == pStyleItemInfo )
		return false;

	UIASSERT(g_pUIApplication->IsDesignMode());

	if( false == this->find_elem_in_xml(pStyleItemInfo->GetSelectorType(), pStyleItemInfo->GetIDRef()) )
	{
		UI_LOG_WARN( _T("CXmlStyleParse::ModifyStyle Cannot find element type=%d, m_strID=%s"), pStyleItemInfo->GetSelectorType(), pStyleItemInfo->GetIDRef().c_str());
		return false;
	}

	m_xml.SetAttrib( XML_STYLE_INHERIT, pStyleItemInfo->GetInheritsRef() ) ;
	Commit_2_TempFile(m_xml, m_strDataSource );
		
	return true;
}
bool CXmlStyleParse::RemoveStyle( STYLE_SELECTOR_TYPE type, const String& strID )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	if( false == this->find_elem_in_xml( type, strID ) )
	{
		UI_LOG_WARN(_T("CXmlStyleParse::InsertStyleAttribute Cannot find elem in xml. type=%d, m_strID=%s"), type, strID.c_str() );
		return false;
	}

	m_xml.RemoveElem();
	Commit_2_TempFile(m_xml, m_strDataSource );

	return true;
}

bool CXmlStyleParse::InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	UIASSERT( g_pUIApplication->IsDesignMode() );

	if( false == this->find_elem_in_xml(type, strID) )
	{
		UI_LOG_WARN(_T("CXmlStyleParse::InsertStyleAttribute Cannot find elem in xml. type=%d, m_strID=%s"), type, strID.c_str() );
		return false;
	}

	m_xml.AddAttrib(strKey, strValue);
	Commit_2_TempFile(m_xml, m_strDataSource );
	return true;
}
bool CXmlStyleParse::ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue )
{
	UIASSERT( g_pUIApplication->IsDesignMode() );

	if( false == this->find_elem_in_xml(type, strID) )
	{
		UI_LOG_WARN(_T("CXmlStyleParse::InsertStyleAttribute Cannot find elem in xml. type=%d, m_strID=%s"), type, strID.c_str() );
		return false;
	}

	m_xml.SetAttrib(strKey,strValue);
	Commit_2_TempFile(m_xml, m_strDataSource );
	return true;
}
bool CXmlStyleParse::RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey )
{
	UIASSERT( g_pUIApplication->IsDesignMode() );

	if( false == this->find_elem_in_xml(type, strID) )
	{
		UI_LOG_WARN(_T("CXmlStyleParse::InsertStyleAttribute Cannot find elem in xml. type=%d, m_strID=%s"), type, strID.c_str() );
		return false;
	}

	UIASSERT( false && _T("TODO: 没有找到RemoveAttrib方法") );
	m_xml.SetAttrib(strKey,_T(""));  // TODO: 没有找到RemoveAttrib方法
	Commit_2_TempFile(m_xml, m_strDataSource );

	return true;
}
//////////////////////////////////////////////////////////////////////////


CXmlLayoutParse::CXmlLayoutParse( const String& strDataSource ) : ILayoutParse(strDataSource)
{
	m_strDataSource = strDataSource;
}
CXmlLayoutParse::~CXmlLayoutParse()
{
	if( g_pUIApplication->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlLayoutParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if( FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if( false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_LAYOUT) )
			return false;
	}

	if( false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}

bool CXmlLayoutParse::Save( CPojo_Layout* pLayoutInfo )
{
	UIASSERT(g_pUIApplication->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlLayoutParse::Load( CPojo_Layout* pLayoutInfo )
{
	if( NULL == pLayoutInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
//	if( g_pUIApplication->IsDesignMode() )  // layout.xml比较特殊，需要全局使用，随时加载
		pXml = &m_xml;
//	else
//		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pLayoutInfo);
	if( false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlLayoutParse::Load  failed!"));
	}
	else
	{
		if( g_pUIApplication->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // 将读取到的数据写入temp文件，用于外部编辑
		}
	}
	return bRet;
}

bool CXmlLayoutParse::load_from_file( CMarkup* pXml, const String& strDataSource, CPojo_Layout* pLayoutInfo )
{
	if( NULL == pXml || NULL == pLayoutInfo )
		return false;

	if( false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlLayoutParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}
	
	return true;
}

//
//	加载指定的对象的布局数据，在xml中将根据pRootObj的object name和id来查找对象及数据
//
bool CXmlLayoutParse::LoadLayout(Object* pRootObj)
{
	Object*  pObj = NULL;
	bool     bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. 在xml中定位到pRootOb这个对象（依据对象xml名称和对象id属性）
		//
		if( false == m_xml.FindElem( ) )             break;     // 忽略root结点的名称
		if( false == m_xml.IntoElem() )              break;

		if( false == m_xml.FindElem( XML_LAYOUT) )   break;
		if( false == m_xml.IntoElem() )              break;

		while(true )
		{
			if( false == m_xml.FindElem( pRootObj->GetObjectName() ) )
			{
				bRet = false;
				break;
			}

			if( m_xml.GetAttrib(XML_ID) != pRootObj->m_strID )
			{
				continue;
			}
			else
			{
				bRet = true;
				break;
			}
		}
		if( !bRet )
		{
			UI_LOG_FATAL( _T("LayoutXmlParse::Load, 未找到要加载的对象：name=%s, id=%s"),
				pRootObj->GetObjectName().c_str(), pRootObj->m_strID.c_str() );
			break;
		}

		bRet = false;      // reset

		//
		//  2. 加载指定对象pRootOb的属性
		//
		this->loadAttributeForCurrentObjectInXml( pRootObj );

		bRet = true;
	}
	while(false);

	//
	//	3. 现在开始递归加载这个窗口的控件对象了
	//
	if( true == bRet )
	{
		this->loadObjects( pRootObj );
	}

	return bRet;
}

//
//	-- 换肤
//
bool CXmlLayoutParse::ReLoadLayout( Object* pRootObj, list<Object*>& listAllChild )
{
	Object*  pObj = NULL;
	bool     bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. 在xml中定位到pRootOb这个对象（依据对象xml名称和对象id属性）
		//
		if( false == m_xml.FindElem( ) )             break;     // 忽略root结点的名称
		if( false == m_xml.IntoElem() )              break;

		if( false == m_xml.FindElem( XML_LAYOUT) )   break;
		if( false == m_xml.IntoElem() )              break;

		while(true )
		{
			if( false == m_xml.FindElem( pRootObj->GetObjectName() ) )
			{
				bRet = false;
				break;
			}

			if( m_xml.GetAttrib(XML_ID) != pRootObj->m_strID )
			{
				continue;
			}
			else
			{
				bRet = true;
				break;
			}
		}
		if( !bRet )
		{
			UI_LOG_FATAL( _T("LayoutXmlParse::Load, 未找到要加载的对象：name=%s, id=%s"),
				pRootObj->GetObjectName().c_str(), pRootObj->m_strID.c_str() );
			break;
		}

		bRet = false;      // reset

		//
		//  2. 加载指定对象pRootOb的属性
		//
		this->loadAttributeForCurrentObjectInXml( pRootObj, true );

		bRet = true;
	}
	while(false);

	//
	//	3. 现在开始递归加载这个窗口的控件对象了
	//
	if( true == bRet )
	{
		this->reloadObjects( pRootObj, listAllChild );
	}

	return bRet;
}

//
//	通过xml中的标签名来实例化一个对象
//
#define BEGIN_XML2CLASS \
	Object* pObj = NULL;

#define XML2CLASS( CLASS )  \
	if( CLASS::XmlName() == strXmlName )  \
	{ \
		pObj = new CLASS; \
	} \
	else 
#define END_XML2CLASS \
	{ \
		UI_LOG_ERROR( _T("LayoutXmlParse::mapXmlToClass，未能解析的对象名称：%s"), strXmlName.c_str() ); \
	}

Object*  CXmlLayoutParse::mapXmlToClass( String strXmlName )
{
	BEGIN_XML2CLASS
		XML2CLASS( Panel )
		XML2CLASS( GroupBox )
		XML2CLASS( ScrollPanel )
		XML2CLASS( HwndHost )
		XML2CLASS( Button )
		XML2CLASS( CheckButton )
		XML2CLASS( RadioButton )
		XML2CLASS( HyperLink )
		XML2CLASS( Label )
		XML2CLASS( PictureCtrl )
//		XML2CLASS( Edit )
		XML2CLASS( SliderCtrl )
		XML2CLASS( ListBox )
		XML2CLASS( TTPlayerPlaylistCtrl )
		XML2CLASS( HScrollBar )
		XML2CLASS( VScrollBar )
	END_XML2CLASS
	return pObj;
}

/*
**	为m_mxl当前指向的对象加载它的属性
*/
bool CXmlLayoutParse::loadAttributeForCurrentObjectInXml( Object* pObj, bool bReload/*=false*/ )
{
	map<String, String> mapAttrib;

	//
	//	加载所有属性
	//
	for( int j = 0; ; j++ )
	{
		String key = m_xml.GetAttribName(j);
		if( _T("") == key )
			break;

		String value = m_xml.GetAttrib( key );
		mapAttrib[key]= value;
	}

	//
	// 特殊处理：扩展style
	//
	String strTagName, strStyleClass, strID;
	if( 0 != mapAttrib.count( XML_ID) )
	{
		strID = mapAttrib[XML_ID];
	}
	if( 0 != mapAttrib.count( XML_STYLECLASS ) )
	{
		strStyleClass = mapAttrib[XML_STYLECLASS];
	}
	strTagName = pObj->GetObjectName();

	::UI_LoadStyle( strTagName, strStyleClass, strID, mapAttrib );

	// 
	//	具体的key、value解析交由对象自己去处理
	//
	pObj->ResetAttribute();
	pObj->SetAttribute( mapAttrib,bReload );

	return true;
 
}

//
// 递归，加载所有子对象及子对象的属性
//
bool CXmlLayoutParse::loadObjects( Object* pObjParent )
{
	bool bRet = false;

	if( false == m_xml.IntoElem() )
		return false;

	// 遍历所有子对象
	while(true)
	{
		bool bRet = this->m_xml.FindElem();
		if( false == bRet )     break;

		Object*  pObj = NULL;
		String   tagName = this->m_xml.GetTagName();
		if( _T("") == tagName )
		{
			UI_LOG_WARN( _T("LayoutXmlParse::loadObjects, xml invalid tag name.") );
			return false;
		}

		// 将标签名转化为类
		pObj = this->mapXmlToClass( tagName );
		if( NULL == pObj )
		{
			continue;
		}

		// 关联
		pObjParent->AddChild( pObj );

		// 加载属性
		this->loadAttributeForCurrentObjectInXml(pObj);

		// 默认将该控件的添加一个notify object为窗口对象，并将msgmapid设置为0
		pObj->AddNotify( pObj->GetWindowObject(), 0 );
		

		// 递归，加载这个子对象的子对象
		this->loadObjects( pObj );
	}
	
	m_xml.OutOfElem();

	return bRet;
}


bool CXmlLayoutParse::reloadObjects(Object* pObjParent, list<Object*>& listAllChild)
{

	bool bRet = false;

	if( false == m_xml.IntoElem() )
		return false;

	// 遍历所有子对象
	while(true)
	{
		bool bRet = this->m_xml.FindElem();
		if( false == bRet )     break;

		Object*  pObj = NULL;
		String   tagName = this->m_xml.GetTagName();
		if( _T("") == tagName )
		{
			UI_LOG_WARN( _T("LayoutXmlParse::loadObjects, xml invalid tag name.") );
			continue;
		}
		String   strID = this->m_xml.GetAttrib( XML_ID );
		if( strID.empty() )
		{
		}
		else
		{
			// 根据 tagName + id 从listAllChild中查找该对象
			list<Object*>::iterator  iter = listAllChild.begin();
			list<Object*>::iterator  iterEnd = listAllChild.end();

			for( ; iter != iterEnd; iter++ )
			{
				Object* pTempObj = *iter;
				if( pTempObj->GetObjectName() == tagName && pTempObj->m_strID == strID )
				{
					pObj = pTempObj;
					break;
				}
			}
		}

		if( NULL == pObj )
		{
			// 没有找到
			// 直接添加这个对象
			// 将标签名转化为类
			pObj = this->mapXmlToClass( tagName );
			if( NULL == pObj )
			{
				continue;
			}

			// 关联
			pObjParent->AddChild( pObj );

			// 加载属性
			this->loadAttributeForCurrentObjectInXml(pObj, false);

			// 默认将该控件的添加一个notify object为窗口对象，并将msgmapid设置为0
			pObj->AddNotify( pObj->GetWindowObject(), 0 );

			// 递归，加载这个子对象的子对象
			this->reloadObjects( pObj, listAllChild );
		}
		else
		{
			// 关联
			pObjParent->AddChild( pObj );

			// 加载属性
			this->loadAttributeForCurrentObjectInXml(pObj,true);

			// 递归，加载这个子对象的子对象
			this->reloadObjects( pObj, listAllChild );

			// 从列表中移除
			list<Object*>::iterator iter = listAllChild.begin();
			list<Object*>::iterator iterEnd = listAllChild.end();
			for ( ; iter != iterEnd; iter++ )
			{
				if(pObj == (*iter))
				{
					listAllChild.erase(iter);
					break;
				}
			}

		}
	}

	m_xml.OutOfElem();

	return bRet;
}