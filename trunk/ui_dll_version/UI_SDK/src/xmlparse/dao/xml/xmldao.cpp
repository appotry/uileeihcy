#include "StdAfx.h"
#include <atlconv.h>


#if 0

project.uiproj <- ProjectXmlParse
<?xml version="1.0" encoding="utf-8" ?>
<ui>
	<proj  select="mediaplayer">                                        // ��ǰ����ʹ���ĸ�����Ƥ��
		<skin  name="mediaplayer" path="skin\mediaplayer\ui.xml"/>      // һ��Ƥ��������Ϣ������+·��
		<skin  name="orange" path="skin\orange\ui.xml"/>
	</proj>
	
	<log  swtich="on/off" >                                             // ��ǰ�����е���־������Ϣ, ȫ����־���أ�Ĭ��Ϊoff
		<item 
			name=""                                                     // ��־����
			path=""                                                     // ��־�ļ����·��
			flag="0x "                                                  // ��־��ӡ��ʽ
			fileflag="0x "                                              // �ļ����ɷ�ʽ
			level=""                                                    // ��־�������
			switch="on/off"                                             // ������־���أ�Ĭ��Ϊoff
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
		<!-- ͼƬ��Դ�����ļ�·�� -->
		<img>img.xml</img>

		<!-- ��ɫ��Դ�����ļ�·�� -->
		<color>col.xml</color>

		<!-- ������Դ�����ļ�·�� -->
		<font>font.xml</font>

		<!-- ���ʻ������ļ�·�� -->
		<i18n></i18n>

		<!-- �ؼ���ʽ�����ļ�·�� -->
		<style></style>

		<!-- �ؼ�λ�������ļ�·�� -->
		<pos></pos>
	</xml>
</ui>



img.xml <- ImgXmlParse 

<?xml version="1.0" encoding="utf-8" ?>
<...>
	<img>
		<item id="btn_4.png">img\btn.png</item>	   // id: ͼƬid������ΪͼƬ����    node data:ͼƬ·����������Լ�ת���ɾ���·��
	</img>
</...>



font.xml <- FontXmlParse 

<?xml version="1.0" encoding="utf-8" ?>
<...>
	<font>
		<item id="songti"         // id
			fontface="����"       // ��������       (Ĭ��SYSTEM)
			height="9"            // �����С       (Ĭ��9)
			orientation="0"       // ����Ƕ�       (Ĭ��0)
			bold="false"          // �Ƿ������ʾ    0/1(Ĭ��0)
			italic="false"        // �Ƿ�б����ʾ    0/1(Ĭ��0)
			underline="false"     // �Ƿ��»�����ʾ  0/1(Ĭ��0)
			strikeout="false"     // �Ƿ�ɾ������ʾ  0/1(Ĭ��0)
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
		 <classname                         // object��Ӧ��xml����
			 id=""                          // object��id��ȫ��Ψһ���Զ�ƥ��style�е�id selector
			 styleclass=""                  // object����ʽ���ͣ��Զ�ƥ��style�е�class selector�����֮��ʹ��,�ָ�
		 >
		 </classname>
	 </layout>
 </...>


 style.xml <- CXmlStyleParse
 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <style>
		 <class                         // CLASSѡ����������ͨ����
			 id=""                      // CLASSѡ����ID�������ⲿ����ʱָ��
			 inherit="a,b,c"            // �̳����ԣ�����̳й�ϵ��,�ֿ�
			 key1="value1"              // ����1
			 key2="value2"              // ����2
		 />
		 <id                            // IDѡ����������Ψһ��
			 id=""                      // IDѡ����ID�������ⲿ����ʱָ��
			 inherit="a,b,c"            // �̳����ԣ�����̳й�ϵ��,�ֿ�
			 key1="value1"              // ����1
			 key2="value2"              // ����2
		 /> 
		 <~tag~                         // ��ǩѡ���������Զ�ƥ���Ӧ���͵ı�ǩ
			 inherit="a,b,c"            // �̳����ԣ�����̳й�ϵ��,�ֿ�
			 key1="value1"              // ����1
			 key2="value2"              // ����2
		 />

	 </style>
 </...>

 style.xml <- StyleXmlParse
 <?xml version="1.0" encoding="utf-8" ?>
 <...>
	 <layout>
		 <classname                         // object��Ӧ��xml���ƣ������ִ�Сд

			 /* object ������Ҫ�������б� */

			 id=""                      // ����id
			 width=""                   // �����ȣ�����Ϊ auto | ��ֵ����Ĭ�� auto��
			 height=""                  // ����ĸ߶ȣ�����Ϊ auto | ��ֵ����Ĭ�� auto��
			 margin=""                  // �������߾࣬��Ĭ��Ϊȫ0)
			 padding=""                 // ������ڱ߾࣬��Ĭ��Ϊȫ0)
			 minwidht=""                // �������С��ȣ���δʵ��)
			 minheight=""               // �������С�߶ȣ���δʵ��)
			 maxwidth=""                // ���������ȣ���δʵ��)
			 maxheight=""               // ��������߶ȣ���δʵ��)

			 erasebkgnd.type=""         // ���󱳾����Ʒ�ʽ��color | gradienth | gradientv | imagesimple | 
			 erasebkgnd.color=""
			 erasebkgnd.colorfrom=""
			 erasebkgnd.colorto=""
			 erasebkgnd.image=""
			 erasebkgnd.bkcolor=""

			 cursor=""
			 font=""

			 /* panel ������Ҫ�������б� */

			 layout=""                  // �������ͣ���ȡֵ��stack��grid��dock��canvas��card��Ĭ��Ϊcanvas

			 /* ���ֲ���������Ҫ�������б� */

			 /* stack���� */
			 layout.direction=""        // ���򣬿���Ϊlefttoright��righttoleft��leftandright��toptobottom��bottomtotop��topandbottom��Ĭ��Ϊlefttoright
			 layout.dock=""             // ���directionȡΪleftandright/topandbottomʱ���������ıߣ�����Ϊ left|right|top|bottom

			 /* canvas���� */
			 layout.left=""             // �������Ե�Ĵ�С����Ĭ��Ϊ0��
			 layout.right=""            // �����ұ�Ե�Ĵ�С��������layout.leftʱ������ȡlayout.left
			 layout.top=""              // �����ϱ�Ե�Ĵ�С����Ĭ��Ϊ0��
			 layout.bottom=""           // �����±�Ե�Ĵ�С��������layout.topʱ������ȡlayout.top

			 /* grid���� */
			 layout.width=""            // grid���ֵĸ��еĿ�ȶ��壬��ȡΪ��100,*,*,auto�����ŵ�����������������Ĭ��Ϊ*��
			 layout.height=""           // grid���ֵĸ��еĸ߶ȶ��壬��ȡΪ��100,*,*,auto�����ŵ�����������������Ĭ��Ϊ*��
			 layout.row=""              // ���������У���Ĭ��Ϊ0��
			 layout.col=""              // ���������У���Ĭ��Ϊ0��
			 layout.rowspan=""          // ����ռ�ݵ�����������Ĭ��Ϊ1��
			 layout.colspan=""          // ����ռ�ݵ�����������Ĭ��Ϊ1��

			 /* dock���� */
			 layout.dock=""             // ����ͣ���ķ��򣬿�ȡΪ��left,right,top,bottom,center
		 >
		 </classname>
	 </layout>
 </...>


 ������
 <classname
	
	 /* window������Ҫ�������б� */

	 font=""                    // ����ʹ�õ�����

	 /*  ����͸�������б�*/

	 transparentmaskcolor=""    // FrameWindowBackgroundRender���У�����͸��ɫ��Ĭ�Ϸ�ɫ255,0,255) [Ĭ��]
	 transparentmaskalpha=""    // FrameWindowBackgroundRender���У�ָ����С�ڶ���alphaֵ������ٳ��� 
	>
</classname

 �ؼ�����control����
 <classname
	 transparentbk="0/1"               // �ؼ��ı����Ƿ���͸���ģ�͸���Ŀؼ�ˢ��ʱ��Ҫ����ˢ����������
 </classname>

 windows�ؼ���װ
 <HwndHost 
     controlid=""                       // Ҫ��װ�Ŀؼ�IDֵ��10����
 >
 </HwndHost>

 �ؼ����ԣ�������������object����
 <Button 
	 btnbkrender=""                     // ��ť��Ⱦ���� color,theme,image
	 drawfocus=""                       // �Ƿ���ƽ���״̬ 0,1
	 autosize=""                        // ��ť��С�Զ����� content(Ĭ��),bkimage,bkimagehcontentw
	
	 bknormalcol=""                     // color bkrender���͵ı���ɫ -- color id
	 bkhovercol=""
	 bkpresscol=""
	 bkdisablecol=""
	 bordernormalcol=""                 // color bkrender���͵ı߿�ɫ -- color id
	 borderpresscol=""
	 borderhovercol=""
	 borderdisablecol=""
	 bkrenderflag=""                    // ��Ҫ������Щ״̬��color 1|2|4|8��Ĭ��Ϊ6������hover��press״̬

	 bkimage=""                         // ��ť��״̬ͼƬ
	 imagedirection=""                  // bkimage�ķ��� x,y

	 icon=""                            // ��ťͼ��--image id
	 icontype=""                        // ͼ��������ͣ�Ŀǰ��֧�� static��8state��û����չ gif, 4state

	 text=""                            // ��ť����
	 texttype=""                        // �ı��������� singleline��Ĭ�ϣ�
	 >
 </Button>

 ��̬�ı��ؼ�
 <Lable>
	 text=""                            // �ı�����
	 font=""                            // �ı�ʹ�õ�����
	 color=""                           // �ı�ʹ�õ���ɫ
	 halign=""                          // ������뷽ʽ left right center
	 valign=""                          // ������뷽ʽ top bottom center
 </Lable>

 <ImageProgressCtrl
	 fore_image=""
	 bkgnd_image=""
	 btn.*=""
 >
 </ImageProgressCtrl>

 groupbox
 <GroupBox
	 text=""                            // �ı�����
	 font=""                            // �ı�ʹ�õ�����
	 color=""                           // �ı�ʹ�õ���ɫ
	 halign=""                          // ������뷽ʽ left right center
	 valign=""                          // ������뷽ʽ top bottom center
	 borderimage=""                     // �߿�ͼƬ
 >
 </GroupBox>

 // �༭��
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
//	ȫ�ֺ�������

//
// ����һ��xml��(��������ȫ��ת����Сд[����])
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
	// ͳһ�л�ΪСд��ĸ����ֹ���ô�Сд���� ע�������漰��һЩӢ���ı��������ã���text,tooltip
	// ��˲�������������ȫ��ת��Сд

	//String content = pXml->GetDoc();
	//transform(content.begin(),content.end(),content.begin(),tolower);
	//return pXml->SetDoc(content);
	return bRet;
}

//
//	��ȡһ��·����Ӧ����ʱ�ļ�·��
//
String Get_TempFile( const String& strDataSource )
{
	return strDataSource + TEMP_FILE_EXT;
}

// ��CMarkup�е����ݱ��浽һ��temp�ļ��У���������繲��
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
// ��һ��temp�ļ������¼������ݵ�CMarkup��
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
		if( false == xml.FindElem( ) )                      break;    // ����root��������
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_PROJECT) )            
		{
			xml.RemoveElem() ;
		}
		
		if( false == xml.InsertElem( XML_PROJECT ))         break;
		if( false == xml.SetAttrib( XML_PROJECT_NAME, pProject->GetProjectName()))  break;
		if( false == xml.SetAttrib( XML_PROJECT_SELECT, pProject->GetActiveSkinName() ) )  break;

		if( false == xml.IntoElem() )                       break;
		
		// �����skin item ��Ϣ
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

			// ����ui.xml����� .uiproj�ļ������·��
			TCHAR szRelative[MAX_PATH] = _T("");
			Util::CalcRelativePathToFile( m_strDataSource.c_str(), pSkinItem->GetSkinXmlPath().c_str(), szRelative );
			if( false == xml.SetAttrib( XML_PROJECT_SKIN_PATH, szRelative ))  { bLoopRet = false;  break; }
		}

		if( !bLoopRet ) break;

		// �����ļ�
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

		if( false == xml.FindElem( ) )                      break;    // ����root��������
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

			// ת���ɾ���·��
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
		if( false == xml.FindElem( ) )                      break;    // ����root��������
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_XML) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_XML ))             break;
		if( false == xml.IntoElem() )                       break;

		// �����skin elem��Ϣ
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
		
		// �����ļ�
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

		if( false == xml.FindElem( ) )                      break;    // ����root��������
		if( false == xml.IntoElem() )                       break;

		if( false == xml.FindElem(XML_XML) )                break;
		if( false == xml.IntoElem() )                       break;
		xml.SavePos();    //  CMarkup�Ľ����Ǵ������µģ���xml�еĸ��ڵ��˳���ǹ̶���

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
	if( g_pUIApplication->IsDesignMode() )  // ��designģʽ�£�����Ҫ����xml������
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
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
		}
	}
	return bRet;

}

// ��temp�ļ�
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
//	����ԭ��xml���ݽ�����д(��ʱû���õ�)
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
		if( false == xml.FindElem( ) )                      break;    // ����root��������
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_IMG) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_IMG ))             break;
		if( false == xml.IntoElem() )                       break;

		// ���Ԫ��
		int  nCount   = (int)pImageInfo->GetImageCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			CPojo_ImageItem*  pItem = pImageInfo->GetImageItem(i);

			if( false == insert_image_2_xml(pItem) ){ bLoopRet = false; break; }
		}
		if( !bLoopRet )  break;

		// �����ļ�
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
		if( false == pXml->FindElem( ) )                      break;    // ����root��������
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
		if( false == xml.FindElem( ) )                      break;    // ����root��������
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_COLOR) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_COLOR ))             break;
		if( false == xml.IntoElem() )                       break;

		// ���Ԫ��
		int  nCount   = (int)pColorInfo->GetColorCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			CPojo_ColorItem* pItem = pColorInfo->GetColorItem(i);
			if( false == insert_color_2_xml(pItem) ){ bLoopRet = false; break; }
			
		}
		if( !bLoopRet )  break;

		// �����ļ�
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
	if( g_pUIApplication->IsDesignMode() )  // ��designģʽ�£�����Ҫ����xml������
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
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
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
		if( false == pXml->FindElem( ) )                      break;    // ����root��������
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
		if( false == xml.FindElem( ) )                      break;    // ����root��������
		if( false == xml.IntoElem() )                       break;

		if( xml.FindElem(XML_FONT) )            
		{
			xml.RemoveElem() ;
		}

		if( false == xml.InsertElem( XML_FONT ))             break;
		if( false == xml.IntoElem() )                       break;

		// ���Ԫ��
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

		// �����ļ�
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
	if( g_pUIApplication->IsDesignMode() )  // ��designģʽ�£�����Ҫ����xml������
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
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
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
		if( false == pXml->FindElem( ) )                      break;    // ����root��������
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
	if( g_pUIApplication->IsDesignMode() )  // ��designģʽ�£�����Ҫ����xml������
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
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
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
//	�ݹ����һ��CPojo_StyleItem�ļ̳�����
//
//	Parameter
//		pTreeItem
//			[in]	Ҫ������CPojo_StyleItem��pTreeItem�������ļ̳���������������Ҷ�ӽ�㡣
//					����Լ��ظ�������������У���ʾ�����������̳У��޷�������
//
//		pPojoStyle
//			[in]	��ǰ���е�StyleItem�б�
//
//	Return
//		�����ɹ�����TRUE��ʧ�ܷ���FALSE��ֻҪ������һ������FALSE�������ȫ������FALSE��
//
bool CXmlStyleParse::parse_inherit( tree<CPojo_StyleItem*>* pTreeItem, CPojo_Style* pPojoStyle )
{
	if( NULL == pTreeItem || NULL == pTreeItem->data )
		return false;

	CPojo_StyleItem* pStyleItem = pTreeItem->data;

	// �ж�pTreeItem�Ƿ��м̳й�ϵ
	int nSize = pStyleItem->GetInheritCount();
	if( 0 == nSize )
		return true;

	// �жϵ�ǰ���pTreeItem�����������ǲ����ظ������ˣ�����ظ����������ʾ��ѭ����
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

		// �����ɹ����̳���������
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
		if( false == pXml->FindElem( ) )                      break;    // ����root��������
		if( false == pXml->IntoElem() )                       break;

		if( false == pXml->FindElem( XML_STYLE ))             break;
		if( false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if( false == pXml->FindElem( ) )                  { break; }
			
			CPojo_StyleItem* pStyleItem = new CPojo_StyleItem;

			// ������������
			for( int j = 0; ; j++ )
			{
				String key = pXml->GetAttribName(j);
				if( _T("") == key )
					break;

				String value = pXml->GetAttrib( key );
				pStyleItem->SetAttribute(key,value);
			}

			// ����  SELECTOR_TPYE, ID
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

			// ���� inherit ����
			String strInherit;
			if( pStyleItem->GetAttribute(XML_STYLE_INHERIT, strInherit) )
			{
				pStyleItem->RemoveAttribute(XML_STYLE_INHERIT);  // ����һ�����ԣ�������Ҫ����չ��
				pStyleItem->SetInherits( strInherit );
			}

			if( false == pStyleInfo->InsertStyle(pStyleItem) )
			{
				delete pStyleItem;
				pStyleItem = NULL;
			}
		}
		if( !bLoopRet )  break;

		// �����е�ITEM������󣬿�ʼ����inherit��ϵ
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
//	����ָ����style item���ڵ�xmlλ�ã����ӵ�ǰ����λ�ã�ȫ������
//
bool CXmlStyleParse::find_elem_in_xml( STYLE_SELECTOR_TYPE type, const String& strID )
{
	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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
//	����ָ����style item���ڵ�xmlλ�ã�Ĭ�ϵ�ǰλ�ô���<style>�µ��ʼ
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
		if( false == m_xml.FindElem( ) )                      break;    // ����root��������
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

	UIASSERT( false && _T("TODO: û���ҵ�RemoveAttrib����") );
	m_xml.SetAttrib(strKey,_T(""));  // TODO: û���ҵ�RemoveAttrib����
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
//	if( g_pUIApplication->IsDesignMode() )  // layout.xml�Ƚ����⣬��Ҫȫ��ʹ�ã���ʱ����
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
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
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
//	����ָ���Ķ���Ĳ������ݣ���xml�н�����pRootObj��object name��id�����Ҷ�������
//
bool CXmlLayoutParse::LoadLayout(Object* pRootObj)
{
	Object*  pObj = NULL;
	bool     bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. ��xml�ж�λ��pRootOb����������ݶ���xml���ƺͶ���id���ԣ�
		//
		if( false == m_xml.FindElem( ) )             break;     // ����root��������
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
			UI_LOG_FATAL( _T("LayoutXmlParse::Load, δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),
				pRootObj->GetObjectName().c_str(), pRootObj->m_strID.c_str() );
			break;
		}

		bRet = false;      // reset

		//
		//  2. ����ָ������pRootOb������
		//
		this->loadAttributeForCurrentObjectInXml( pRootObj );

		bRet = true;
	}
	while(false);

	//
	//	3. ���ڿ�ʼ�ݹ����������ڵĿؼ�������
	//
	if( true == bRet )
	{
		this->loadObjects( pRootObj );
	}

	return bRet;
}

//
//	-- ����
//
bool CXmlLayoutParse::ReLoadLayout( Object* pRootObj, list<Object*>& listAllChild )
{
	Object*  pObj = NULL;
	bool     bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. ��xml�ж�λ��pRootOb����������ݶ���xml���ƺͶ���id���ԣ�
		//
		if( false == m_xml.FindElem( ) )             break;     // ����root��������
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
			UI_LOG_FATAL( _T("LayoutXmlParse::Load, δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),
				pRootObj->GetObjectName().c_str(), pRootObj->m_strID.c_str() );
			break;
		}

		bRet = false;      // reset

		//
		//  2. ����ָ������pRootOb������
		//
		this->loadAttributeForCurrentObjectInXml( pRootObj, true );

		bRet = true;
	}
	while(false);

	//
	//	3. ���ڿ�ʼ�ݹ����������ڵĿؼ�������
	//
	if( true == bRet )
	{
		this->reloadObjects( pRootObj, listAllChild );
	}

	return bRet;
}

//
//	ͨ��xml�еı�ǩ����ʵ����һ������
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
		UI_LOG_ERROR( _T("LayoutXmlParse::mapXmlToClass��δ�ܽ����Ķ������ƣ�%s"), strXmlName.c_str() ); \
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
**	Ϊm_mxl��ǰָ��Ķ��������������
*/
bool CXmlLayoutParse::loadAttributeForCurrentObjectInXml( Object* pObj, bool bReload/*=false*/ )
{
	map<String, String> mapAttrib;

	//
	//	������������
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
	// ���⴦����չstyle
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
	//	�����key��value�������ɶ����Լ�ȥ����
	//
	pObj->ResetAttribute();
	pObj->SetAttribute( mapAttrib,bReload );

	return true;
 
}

//
// �ݹ飬���������Ӷ����Ӷ��������
//
bool CXmlLayoutParse::loadObjects( Object* pObjParent )
{
	bool bRet = false;

	if( false == m_xml.IntoElem() )
		return false;

	// ���������Ӷ���
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

		// ����ǩ��ת��Ϊ��
		pObj = this->mapXmlToClass( tagName );
		if( NULL == pObj )
		{
			continue;
		}

		// ����
		pObjParent->AddChild( pObj );

		// ��������
		this->loadAttributeForCurrentObjectInXml(pObj);

		// Ĭ�Ͻ��ÿؼ������һ��notify objectΪ���ڶ��󣬲���msgmapid����Ϊ0
		pObj->AddNotify( pObj->GetWindowObject(), 0 );
		

		// �ݹ飬��������Ӷ�����Ӷ���
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

	// ���������Ӷ���
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
			// ���� tagName + id ��listAllChild�в��Ҹö���
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
			// û���ҵ�
			// ֱ������������
			// ����ǩ��ת��Ϊ��
			pObj = this->mapXmlToClass( tagName );
			if( NULL == pObj )
			{
				continue;
			}

			// ����
			pObjParent->AddChild( pObj );

			// ��������
			this->loadAttributeForCurrentObjectInXml(pObj, false);

			// Ĭ�Ͻ��ÿؼ������һ��notify objectΪ���ڶ��󣬲���msgmapid����Ϊ0
			pObj->AddNotify( pObj->GetWindowObject(), 0 );

			// �ݹ飬��������Ӷ�����Ӷ���
			this->reloadObjects( pObj, listAllChild );
		}
		else
		{
			// ����
			pObjParent->AddChild( pObj );

			// ��������
			this->loadAttributeForCurrentObjectInXml(pObj,true);

			// �ݹ飬��������Ӷ�����Ӷ���
			this->reloadObjects( pObj, listAllChild );

			// ���б����Ƴ�
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