#include "StdAfx.h"
#include "xmlparse.h"
#include <atlconv.h>

#include "UISDK\Kernel\Src\Atl\image.h"
#include "UISDK\Kernel\Src\Util\MapAttr\mapattr.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Src\Window\windowbase.h"

#if 0

project.ui <- ProjectXmlParse
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
		<item
			id="btn_4.png"     // id: ͼƬid������ΪͼƬ����
			type=""            // ͼƬ����:icon, imagelist, gif
			useskinhls=""      // �Ƿ����ɫ���ı�: 0/1
			count=""           // imagelist������ͼƬ������
			width="" height="" // icon���ͼ���ʱָ����ͼƬ��С
		>img\btn.png</item>	   // node data:ͼƬ·����������Լ�ת���ɾ���·��������д��path=""����
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
		<item id="black" useskinhls="">255,255,255</item>   // �Ƿ����ɫ���ı�: 0/1
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

 layout.xml <- LayoutXmlParse
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
#if 0
bool LoadXml_ToLower( CMarkup* pXml, const String& strDataSource )
{
	if (NULL == pXml )
		return false;

	bool bRet = pXml->Load( strDataSource );
	if (false == bRet )
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
// 	if (f.fail() )
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
	if (PathFileExists(strTempFilePath.c_str() ))
	{
		::DeleteFile(strTempFilePath.c_str());
	}
	return true;
}
#endif
//////////////////////////////////////////////////////////////////////////

#if 0
CXmlImageParse::CXmlImageParse()
{
}
CXmlImageParse::~CXmlImageParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);;
}
bool CXmlImageParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_IMG) )
			return false;
	}

	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}
bool CXmlImageParse::Save( ImageRes*  pImageInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}


bool CXmlImageParse::Load(ImageRes*  pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo)
{
	if (NULL == pImageInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if (m_pUIApp->IsDesignMode() )  // ��designģʽ�£�����Ҫ����xml������
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pImageInfo, pCursorInfo, pGifInfo);
	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlImageParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
		}
	}
	return bRet;

}

// ��temp�ļ�
bool CXmlImageParse::Reload(ImageRes* pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo)
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return this->load_from_file(&m_xml, m_strDataSource + TEMP_FILE_EXT, pImageInfo, pCursorInfo, pGifInfo);
}
bool CXmlImageParse::InsertImage(  ImageResItem *pImageItemInfo )
{
	if (NULL == pImageItemInfo )
		return false;

	if (false == m_pUIApp->IsDesignMode())
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_IMG) )            
		{
			if (false == m_xml.InsertElem( XML_IMG ))         break;
		}
		if (false == m_xml.IntoElem() )                       break;
		if (false == insert_image_2_xml(pImageItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}

bool CXmlImageParse::ModifyImage( ImageResItem *pImageItemInfo )
{
	if (NULL == pImageItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_IMG) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == pImageItemInfo->GetIDRef())
			{
				TCHAR szRelative[MAX_PATH] = _T("");
				Util::CalcRelativePathToFile(m_strDataSource.c_str(), pImageItemInfo->GetPathRef().c_str(), szRelative );
				if (m_xml.SetData( pImageItemInfo->GetPathRef()) )  
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);


	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}

bool CXmlImageParse::RemoveImage( const String& strID )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_IMG) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == strID )
			{
				if (m_xml.RemoveElem() )
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}


//
//	����ԭ��xml���ݽ�����д(��ʱû���õ�)
//
bool CXmlImageParse::clear_save( ImageRes*  pImageInfo )
{
	if (NULL == pImageInfo )
		return false;

	CMarkup  xml;
	if (false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlImageParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == xml.FindElem( ) )                      break;    // ����root��������
		if (false == xml.IntoElem() )                       break;

		if (xml.FindElem(XML_IMG) )            
		{
			xml.RemoveElem() ;
		}

		if (false == xml.InsertElem( XML_IMG ))             break;
		if (false == xml.IntoElem() )                       break;

		// ���Ԫ��
		int  nCount   = (int)pImageInfo->GetImageCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			ImageResItem*  pItem = pImageInfo->GetImageItem(i);

			if (false == insert_image_2_xml(pItem) ){ bLoopRet = false; break; }
		}
		if (!bLoopRet )  break;

		// �����ļ�
		if (false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlImageParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlImageParse::load_from_file( CMarkup* pXml, const String& strDataSource, ImageRes *pImageInfo, CursorRes* pCursorInfo, GifRes* pGifInfo )
{
#if 0 // TODO: ɾ�������¼ܹ���
	if (NULL == pXml || NULL == pImageInfo || NULL == pCursorInfo || NULL == pGifInfo)
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlImageParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == pXml->FindElem( ) )                      break;    // ����root��������
		if (false == pXml->IntoElem() )                       break;

		if (false == pXml->FindElem( XML_IMG ))               break;
		if (false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
//			if (false == pXml->FindElem( XML_ITEM ))        { break; }
			if (false == pXml->FindElem())                  { break; }

			String strTagName = pXml->GetTagName();

			//	������������
			ATTRMAP  mapAttrib;
			for( int j = 0; ; j++ )
			{
				String key = pXml->GetAttribName(j);
				if (_T("") == key )
					break;

				String value = pXml->GetAttrib( key );
				mapAttrib[key]= value;
			}

			String  strPath = pXml->GetData();
			if (strPath.empty())
			{
				strPath = pXml->GetAttrib(XML_PATH);   // ���û�������������У��ͼ��һ���Ƿ������ó�������
			}
			String  strFullPath;
			if (Util::IsFullPath(strPath.c_str()) )
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

			if (strTagName == XML_IMAGE_ITEM_CURSOR)
			{
				if (false == pCursorInfo->LoadItem(mapAttrib, strFullPath))
					UI_LOG_WARN(_T("%s insert cursor failed. path=%s"), FUNC_NAME, strFullPath.c_str());
			}
			else if (strTagName == XML_IMAGE_ITEM_GIF)
			{
				if (false == pGifInfo->LoadItem(mapAttrib, strFullPath))
					UI_LOG_WARN(_T("%s insert gif failed. path=%s"), FUNC_NAME, strFullPath.c_str());
			}
			else
			{
				if (false == pImageInfo->LoadItem(strTagName, mapAttrib, strFullPath))
					UI_LOG_WARN(_T("%s insert image failed. path=%s"), FUNC_NAME, strFullPath.c_str());
			}
		}
		bRet = true;
	} 
	while (0);

	return bRet;
#endif 
    return 0;
}
bool CXmlImageParse::insert_image_2_xml( ImageResItem *pImageItemInfo )
{
	TCHAR szRelative[MAX_PATH] = _T("");
	Util::CalcRelativePathToFile(m_strDataSource.c_str(), pImageItemInfo->GetPathRef().c_str(), szRelative );

	if (false == m_xml.AddElem( XML_ITEM, String(szRelative)) )    
		return false;
	if (false == m_xml.SetAttrib( XML_ID, pImageItemInfo->GetIDRef()) )  
		return false;

	return true;
}


//////////////////////////////////////////////////////////////////////////


CXmlColorParse::CXmlColorParse()
{
}
CXmlColorParse::~CXmlColorParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlColorParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_COLOR) )
			return false;
	}

	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}
bool CXmlColorParse::Save( ColorRes*  pColorInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlColorParse::clear_save(ColorRes* pColorInfo )
{
	if (NULL == pColorInfo )
		return false;

	CMarkup  xml;
	if (false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlColorParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == xml.FindElem( ) )                      break;    // ����root��������
		if (false == xml.IntoElem() )                       break;

		if (xml.FindElem(XML_COLOR) )            
		{
			xml.RemoveElem() ;
		}

		if (false == xml.InsertElem( XML_COLOR ))             break;
		if (false == xml.IntoElem() )                       break;

		// ���Ԫ��
		int  nCount   = (int)pColorInfo->GetColorCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			ColorResItem* pItem = pColorInfo->GetColorItem(i);
			if (false == insert_color_2_xml(pItem) ){ bLoopRet = false; break; }
			
		}
		if (!bLoopRet )  break;

		// �����ļ�
		if (false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlColorParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlColorParse::Load( ColorRes*  pColorInfo )
{
	if (NULL == pColorInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if (m_pUIApp->IsDesignMode() )  // ��designģʽ�£�����Ҫ����xml������
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pColorInfo);
	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlColorParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
		}
	}
	return bRet;
}
bool CXmlColorParse::load_from_file( CMarkup* pXml, const String& strDataSource, ColorRes *pColorInfo )
{
#if 0
	if(NULL == pXml || NULL == pColorInfo)
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource))
	{
		UI_LOG_ERROR( _T("CXmlColorParse::Load  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}
	pXml->ResetPos();

	bool bRet = false;
	do 
	{
		if (false == pXml->FindElem( ) )                      break;    // ����root��������
		if (false == pXml->IntoElem() )                       break;

		if (false == pXml->FindElem( XML_COLOR ))             break;
		if (false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if (false == pXml->FindElem(XML_ITEM) )           break;

			//	������������
			ATTRMAP  mapAttrib;
			for( int j = 0; ; j++ )
			{
				String key = pXml->GetAttribName(j);
				if (_T("") == key )
					break;

				String value = pXml->GetAttrib( key );
				mapAttrib[key]= value;
			}

			String strValue = pXml->GetData();
			if (false == pColorInfo->LoadItem(mapAttrib, strValue))
						UI_LOG_WARN(_T("%s insert color failed."), FUNC_NAME);
		}
		if (!bLoopRet )  break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_ERROR(_T("CXmlColorParse::load_from_file  failed!"));
	}
    return bRet;
#endif 
    return 0;
}

bool CXmlColorParse::Reload( ColorRes* pColorInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pColorInfo );
}
bool CXmlColorParse::InsertColor( ColorResItem *pColorItemInfo )
{
	if (NULL == pColorItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_COLOR) )            
		{
			if (false == m_xml.InsertElem( XML_COLOR ))       break;
		}
		if (false == m_xml.IntoElem() )                       break;
		if (false == insert_color_2_xml(pColorItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlColorParse::ModifyColor( ColorResItem *pColorItemInfo )
{
	if (NULL == pColorItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_COLOR) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == pColorItemInfo->GetIDRef() )
			{
				if (m_xml.SetData( pColorItemInfo->GetColorStringRef() ) )  
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);


	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlColorParse::RemoveColor( const String& strID )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_COLOR) )              break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == strID )
			{
				if (m_xml.RemoveElem() )
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlColorParse::insert_color_2_xml( ColorResItem *pColorItemInfo )
{
	if (NULL == pColorItemInfo)
		return false;

	if (false == m_xml.AddElem( XML_ITEM, pColorItemInfo->GetColorStringRef().c_str() ) )   
		return false;
	if (false == m_xml.SetAttrib( XML_ID, pColorItemInfo->GetIDRef().c_str())) 
		return false;

	return true;
}
//////////////////////////////////////////////////////////////////////////

CXmlFontParse::CXmlFontParse()
{
}
CXmlFontParse::~CXmlFontParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlFontParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_FONT) )
			return false;
	}

	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}
bool CXmlFontParse::Save( FontRes*  pFontInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlFontParse::clear_save( FontRes*  pFontInfo )
{
	if (NULL == pFontInfo )
		return false;

	CMarkup  xml;
	if (false == xml.Load(m_strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlFontParse::Save  load xml failed, datasource=%s"), m_strDataSource.c_str() );
		return false;
	}

	xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == xml.FindElem( ) )                      break;    // ����root��������
		if (false == xml.IntoElem() )                       break;

		if (xml.FindElem(XML_FONT) )            
		{
			xml.RemoveElem() ;
		}

		if (false == xml.InsertElem( XML_FONT ))             break;
		if (false == xml.IntoElem() )                       break;

		// ���Ԫ��
		int  nCount   = (int)pFontInfo->GetFontCount();
		bool bLoopRet = true;  

		for ( int i = nCount-1; i >= 0; i-- )
		{
			FontResItem* pItem = NULL;
			pItem = pFontInfo->GetFontItem(i);
			if (NULL == pItem)
			{
				UI_LOG_WARN( _T("CXmlFontParse::Save GetFontItem failed, nIndex=%d"), i );
				continue;
			}

			if (false == this->insert_font_2_xml(pItem) ) 
			{ 
				bLoopRet = false; 
				break; 
			}

		}
		if (!bLoopRet )  break;

		// �����ļ�
		if (false == xml.Save(m_strDataSource))              break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlFontParse::Save  failed!"));
	}
	return bRet;
}

bool CXmlFontParse::Reload( FontRes* pFontInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pFontInfo );	
}

bool CXmlFontParse::Load( FontRes* pFontInfo )
{
	if (NULL == pFontInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if (m_pUIApp->IsDesignMode() )  // ��designģʽ�£�����Ҫ����xml������
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pFontInfo);
	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlFontParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
		}
	}
	return bRet;
}

bool CXmlFontParse::load_from_file( CMarkup* pXml, const String& strDataSource, FontRes *pFontInfo )
{
	if(NULL == pXml || NULL == pFontInfo )
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlFontParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}

	pXml->ResetPos();

	bool bRet = false;
	do 
	{
		if (false == pXml->FindElem( ) )                      break;    // ����root��������
		if (false == pXml->IntoElem() )                       break;

		if (false == pXml->FindElem( XML_FONT ))              break;
		if (false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if (false == pXml->FindElem( XML_ITEM ) )        { break; }

			String strID = pXml->GetAttrib( XML_ID );
			String strFaceName = pXml->GetAttrib( XML_FONT_FACENAME );
			String strFontHeight = pXml->GetAttrib( XML_FONT_HEIGHT );
			String strOrientation = pXml->GetAttrib( XML_FONT_ORIENTATION );
			String strBold = pXml->GetAttrib( XML_FONT_BOLD );
			String strItalic = pXml->GetAttrib( XML_FONT_ITALIC );
			String strUnderline = pXml->GetAttrib( XML_FONT_UNDERLINE );
			String strStrikeout = pXml->GetAttrib( XML_FONT_STRIKEOUT );

			if (strFaceName.empty() )
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
			if (!strItalic.empty())
			{
				lfItalic = _ttoi(strItalic.c_str());
			}
			if (!strUnderline.empty())
			{
				lfUnderline = _ttoi(strUnderline.c_str());
			}
			if (!strStrikeout.empty())
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

			if (false == pFontInfo->InsertFont( strID, &lf ) )
			{
				UI_LOG_WARN( _T("CXmlFontParse::load_from_file insert font failed. m_strID=%s, facename=%s"), strID.c_str(), strFaceName.c_str() );
			}
		}
		if (!bLoopRet )  break;

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlFontParse::load_from_file  failed!"));
	}
	return bRet;
}

bool CXmlFontParse::InsertFont( FontResItem *pFontItemInfo )
{
	if (NULL == pFontItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_FONT) )            
		{
			if (false == m_xml.InsertElem( XML_FONT ))        break;
		}
		if (false == m_xml.IntoElem() )                       break;
		if (false == insert_font_2_xml(pFontItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlFontParse::ModifyFont( FontResItem *pFontItemInfo )
{
	if (NULL == pFontItemInfo)
		return false;

	if (!m_pUIApp->IsDesignMode())
		return true;

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_FONT) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == pFontItemInfo->GetIDRef() )
			{
				TCHAR szFontFace[32] = _T("");
				pFontItemInfo->GetFaceName(szFontFace);
				if (false == m_xml.SetAttrib( XML_ID,    pFontItemInfo->GetIDRef() ))                      { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_FACENAME,    szFontFace ))                          { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_HEIGHT,      pFontItemInfo->GetFontSize() ))        { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_ORIENTATION, pFontItemInfo->GetFontOrientation() )) { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_BOLD,      (int)pFontItemInfo->GetFontBold() ))     { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_ITALIC,    (int)pFontItemInfo->GetFontItalic() ))   { bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_UNDERLINE, (int)pFontItemInfo->GetFontUnderline() )){ bLoopRet = false; break; }
				if (false == m_xml.SetAttrib( XML_FONT_STRIKEOUT, (int)pFontItemInfo->GetFontStrikeout() )){ bLoopRet = false; break; }

				bLoopRet = true;
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);


	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlFontParse::RemoveFont( const String& strID )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_FONT) )                break;
		if (false == m_xml.IntoElem() )                       break;

		bool bLoopRet = false;
		while(1)    
		{
			if (false == m_xml.FindElem(XML_ITEM) )
				break;;
			if (m_xml.GetAttrib(XML_ID) == strID )
			{
				if (m_xml.RemoveElem() )
				{
					bLoopRet = true;
				}
				break;
			}
		}
		if (!bLoopRet ) break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}

bool CXmlFontParse::insert_font_2_xml( FontResItem *pFontItemInfo )
{
	if (NULL == pFontItemInfo)
		return false;

	if (false == m_xml.AddElem( XML_ITEM ) )   { return false; }

	TCHAR szFontFace[32] = _T("");
	pFontItemInfo->GetFaceName(szFontFace);
	if (false == m_xml.SetAttrib( XML_ID,    pFontItemInfo->GetIDRef() ))                      { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_FACENAME,    szFontFace ))                          { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_HEIGHT,      pFontItemInfo->GetFontSize() ))        { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_ORIENTATION, pFontItemInfo->GetFontOrientation() )) { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_BOLD,      (int)pFontItemInfo->GetFontBold() ))     { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_ITALIC,    (int)pFontItemInfo->GetFontItalic() ))   { return false; }
	if (false == m_xml.SetAttrib( XML_FONT_UNDERLINE, (int)pFontItemInfo->GetFontUnderline() )){ return false; }
	if (false == m_xml.SetAttrib( XML_FONT_STRIKEOUT, (int)pFontItemInfo->GetFontStrikeout() )){ return false; }

	return true;
}
//////////////////////////////////////////////////////////////////////////


CXmlStyleParse::CXmlStyleParse()
{
}
CXmlStyleParse::~CXmlStyleParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlStyleParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_STYLE) )
			return false;
	}
	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}

bool CXmlStyleParse::Save( StyleRes* pStyleInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlStyleParse::Load( StyleRes* pStyleInfo )
{
	if (NULL == pStyleInfo )
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
	if (m_pUIApp->IsDesignMode() )  // ��designģʽ�£�����Ҫ����xml������
		pXml = &m_xml;
	else
		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pStyleInfo);
	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlStyleParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode() )
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
		}
	}
	return bRet;
}
bool CXmlStyleParse::Reload( StyleRes* pStyleInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return this->load_from_file( &m_xml, m_strDataSource + TEMP_FILE_EXT, pStyleInfo );
}



//
// ����һ���̳��ַ�����������ʽ���ͣ��罫#button����Ϊ idѡ�����ͣ�id=button
//
/*static*/ bool ParseInheritString(const String& strInherit, STYLE_SELECTOR_TYPE& eStyletype, TCHAR* szStyleName )
{
	if (strInherit.length() <= 0 || NULL == szStyleName )
		return false;

	if (strInherit[0] == STYLE_ID_PREFIX )
	{
		eStyletype = STYLE_SELECTOR_TYPE_ID;
		_tcsncpy( szStyleName, strInherit.substr( 1, strInherit.length()-1 ).c_str(), MAX_STRING_LEN-1 );
	}
	else if (strInherit[0] == STYLE_CLASS_PREFIX )
	{
		eStyletype = STYLE_SELECTOR_TYPE_CLASS;
		_tcsncpy( szStyleName, strInherit.substr( 1, strInherit.length()-1 ).c_str(), MAX_STRING_LEN-1 );
	}
	else
	{
		eStyletype = STYLE_SELECTOR_TYPE_TAG;
		_tcsncpy( szStyleName, strInherit.c_str(), MAX_STRING_LEN-1 );
	}
	return true;
}
/*static*/ bool MakeInheritString(const STYLE_SELECTOR_TYPE& eStyletype, const String& strStypeName, TCHAR* szInherit )
{
	if (strStypeName.length() <= 0 || NULL == szInherit )
		return false;

	if (eStyletype == STYLE_SELECTOR_TYPE_ID )
	{
		_tcscpy( szInherit, _T(" ") );
		szInherit[0] = STYLE_ID_PREFIX ;
		_tcscat( szInherit, strStypeName.c_str() );
	}
	else if (eStyletype == STYLE_SELECTOR_TYPE_CLASS )
	{
		_tcscpy( szInherit, _T(" ") );
		szInherit[0] = STYLE_CLASS_PREFIX ;
		_tcscat( szInherit, strStypeName.c_str() );
	}
	else
	{
		_tcscpy( szInherit, strStypeName.c_str() );
	}
	return true;
}


//
//	�ݹ����һ��CPojo_StyleItem�ļ̳�����
//
//	Parameter
//		pTreeItem
//			[in]	Ҫ������CPojo_StyleItem��pTreeItem�������ļ̳���������������Ҷ�ӽ�㡣
//					����Լ��ظ�������������У���ʾ�����������̳У��޷�������
//
//		pStyleRes
//			[in]	��ǰ���е�StyleItem�б�
//
//	Return
//		�����ɹ�����TRUE��ʧ�ܷ���FALSE��ֻҪ������һ������FALSE�������ȫ������FALSE��
//
bool CXmlStyleParse::parse_inherit( tree<StyleResItem*>* pTreeItem, StyleRes* pStyleRes )
{
	if (NULL == pTreeItem || NULL == pTreeItem->data )
		return false;

	StyleResItem* pStyleItem = pTreeItem->data;

	// �ж�pTreeItem�Ƿ��м̳й�ϵ
	int nSize = pStyleItem->GetInheritCount();
	if (0 == nSize )
		return true;

	// �жϵ�ǰ���pTreeItem�����������ǲ����ظ������ˣ�����ظ����������ʾ��ѭ����
	{
		tree<StyleResItem*>* pParentItem = pTreeItem;
		do
		{
			pParentItem = pParentItem->parent;
			if (pParentItem && pParentItem->data == pStyleItem )
				return false;
		}
		while(pParentItem != NULL);
	}


	for( int i = nSize-1; i >= 0; i-- )
	{
		String strInherit;
		if (false == pStyleItem->GetInheritItem(i, strInherit) )
			continue;

		STYLE_SELECTOR_TYPE type = STYLE_SELECTOR_TYPE_TAG;
		TCHAR szStyleName[MAX_STRING_LEN] = _T("");
		ParseInheritString(strInherit, type, szStyleName);

		StyleResItem* pInheritItem = pStyleRes->GetStyleItem(type, szStyleName);
		if (NULL == pInheritItem )
			continue;

		tree<StyleResItem*> t;
		t.parent = pTreeItem;
		t.data   = pInheritItem;

		if (false == this->parse_inherit( &t, pStyleRes) )
		{
			return false;
		}

		// �����ɹ����̳���������
		pInheritItem->InheritMyAttributesToAnother(pStyleItem);
		pStyleItem->RemoveInheritItem( strInherit );
	}

	return true;
}

bool CXmlStyleParse::load_from_file( CMarkup* pXml, const String& strDataSource, StyleRes *pStyleInfo )
{
	if (NULL == pXml || NULL == pStyleInfo )
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource) )
	{
		UI_LOG_ERROR( _T("CXmlStyleParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}

	pXml->ResetPos();

	bool bRet = false;
	do 
	{
		if (false == pXml->FindElem( ) )                      break;    // ����root��������
		if (false == pXml->IntoElem() )                       break;

		if (false == pXml->FindElem( XML_STYLE ))             break;
		if (false == pXml->IntoElem() )                       break;

		bool bLoopRet = true;
		for ( ;; )
		{
			if (false == pXml->FindElem( ) )                  { break; }
			
			StyleResItem* pStyleItem = new StyleResItem;

			// ������������
			for( int j = 0; ; j++ )
			{
				String key = pXml->GetAttribName(j);
				if (_T("") == key )
					break;

				String value = pXml->GetAttrib( key );
				pStyleItem->SetAttribute(key,value);
			}

			// ����  SELECTOR_TPYE, ID
			String strTagName = pXml->GetTagName();
			if (XML_STYLE_SELECTOR_TYPE_ID == strTagName )
			{
				pStyleItem->SetSelectorType( STYLE_SELECTOR_TYPE_ID );
				
				String strID;
				if (false == pStyleItem->GetAttribute( XML_ID, strID ) )
				{
					UI_LOG_WARN( _T("CXmlStyleParse::load_from_file Can't find the %s attribute of %s"), XML_ID, strTagName.c_str() );
					delete pStyleItem;
					continue;
				}
				pStyleItem->SetID( strID );
				pStyleItem->RemoveAttribute( XML_ID );
			}
			else if (XML_STYLE_SELECTOR_TYPE_CLASS == strTagName )
			{
				pStyleItem->SetSelectorType( STYLE_SELECTOR_TYPE_CLASS );

				String strID;
				if (false == pStyleItem->GetAttribute( XML_ID, strID ) )
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
			if (pStyleItem->GetAttribute(XML_STYLE_INHERIT, strInherit) )
			{
				pStyleItem->RemoveAttribute(XML_STYLE_INHERIT);  // ����һ�����ԣ�������Ҫ����չ��
				pStyleItem->SetInherits( strInherit );
			}

			if (false == pStyleInfo->InsertStyle(pStyleItem) )
			{
				delete pStyleItem;
				pStyleItem = NULL;
			}
		}
		if (!bLoopRet )  break;

		// �����е�ITEM������󣬿�ʼ����inherit��ϵ
		int nSize = pStyleInfo->GetStyleCount();
		for ( int i = 0; i < nSize; i++ )
		{
			StyleResItem* pItem = pStyleInfo->GetStyleItem(i);
			tree<StyleResItem*>  t;
			t.data = pItem;

			if (false == this->parse_inherit(&t, pStyleInfo) )
			{
				UI_LOG_WARN(_T("CXmlStyleParse::load_from_file parse_inherit failed. style item=%s"), pItem->GetIDRef().c_str() );
			}
		}

		bRet = true;
	} 
	while (0);

	if (false == bRet )
	{
		UI_LOG_FATAL(_T("CXmlStyleParse::load_from_file  failed."));
	}
	return bRet;
}

bool CXmlStyleParse::insert_style_2_xml( StyleResItem *pStyleItemInfo )
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

	if (! strInherit.empty() )
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
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_STYLE) )              break;
		if (false == m_xml.IntoElem() )                       break;

		if (false == this->find_elem_under_style(type,strID)) break;

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
				if (false == m_xml.FindElem(XML_STYLE_SELECTOR_TYPE_ID) )
					break;
				if (m_xml.GetAttrib(XML_ID) == strID )
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
				if (false == m_xml.FindElem(XML_STYLE_SELECTOR_TYPE_CLASS) )
					break;
				if (m_xml.GetAttrib(XML_ID) == strID)
				{
					bLoopRet = true;
					break;
				}
			}
		}	
		break;

	default:
		{
			if (m_xml.FindElem(strID) )
				bLoopRet = true;
		}
		break;
	}

	return bLoopRet;
}
bool CXmlStyleParse::InsertStyle( StyleResItem *pStyleItemInfo )
{
	if (NULL == pStyleItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	m_xml.ResetPos();

	bool bRet = false;
	do 
	{
		if (false == m_xml.FindElem( ) )                      break;    // ����root��������
		if (false == m_xml.IntoElem() )                       break;

		if (false == m_xml.FindElem(XML_STYLE) )            
		{
			if (false == m_xml.InsertElem( XML_STYLE ))       break;
		}
		if (false == m_xml.IntoElem() )                       break;
		if (false == insert_style_2_xml(pStyleItemInfo) )     break;

		bRet = true;
	}
	while(0);

	if (bRet )
		Commit_2_TempFile(m_xml, m_strDataSource );
	return bRet;
}
bool CXmlStyleParse::ModifyStyle( StyleResItem *pStyleItemInfo )
{
	if (NULL == pStyleItemInfo )
		return false;

	UIASSERT(m_pUIApp->IsDesignMode());

	if (false == this->find_elem_in_xml(pStyleItemInfo->GetSelectorType(), pStyleItemInfo->GetIDRef()) )
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
	UIASSERT(m_pUIApp->IsDesignMode());

	if (false == this->find_elem_in_xml( type, strID ) )
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
	UIASSERT( m_pUIApp->IsDesignMode() );

	if (false == this->find_elem_in_xml(type, strID) )
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
	UIASSERT( m_pUIApp->IsDesignMode() );

	if (false == this->find_elem_in_xml(type, strID) )
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
	UIASSERT( m_pUIApp->IsDesignMode() );

	if (false == this->find_elem_in_xml(type, strID) )
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


CXmlLayoutParse::CXmlLayoutParse()
{
}
CXmlLayoutParse::~CXmlLayoutParse()
{
	if (m_pUIApp->IsDesignMode() )
		::Delete_TempFile(m_strDataSource);
}
bool CXmlLayoutParse::Create()
{
	::DeleteFile( m_strDataSource.c_str() );
	if (FALSE == PathFileExists(m_strDataSource.c_str()) )
	{
		if (false == Util::CreateEmptyXmlFile(m_strDataSource.c_str(), XML_UI, XML_LAYOUT) )
			return false;
	}

	if (false == m_xml.Load(m_strDataSource) )
		return false;

	::Commit_2_TempFile(m_xml, m_strDataSource);
	return true;
}

bool CXmlLayoutParse::Save( LayoutRes* pLayoutInfo )
{
	UIASSERT(m_pUIApp->IsDesignMode());

	return m_xml.Save(m_strDataSource);
}
bool CXmlLayoutParse::Load(LayoutRes* pLayoutInfo)
{
	if (NULL == pLayoutInfo)
		return false;

	CMarkup* pXml = NULL;
	CMarkup  tempXml;
//	if (m_pUIApp->IsDesignMode() )  // layout.xml�Ƚ����⣬��Ҫȫ��ʹ�ã���ʱ����
		pXml = &m_xml;
//	else
//		pXml = &tempXml;

	bool bRet = this->load_from_file(pXml, m_strDataSource, pLayoutInfo);
	if (false == bRet)
	{
		UI_LOG_FATAL(_T("CXmlLayoutParse::Load  failed!"));
	}
	else
	{
		if (m_pUIApp->IsDesignMode())
		{
			::Commit_2_TempFile(m_xml, m_strDataSource);  // ����ȡ��������д��temp�ļ��������ⲿ�༭
		}
	}
	return bRet;
}

bool CXmlLayoutParse::load_from_file( CMarkup* pXml, const String& strDataSource, LayoutRes* pLayoutInfo )
{
	if (NULL == pXml || NULL == pLayoutInfo)
		return false;

	if (false == ::LoadXml_ToLower(pXml, strDataSource))
	{
		UI_LOG_ERROR( _T("CXmlLayoutParse::load_from_file  load xml failed, datasource=%s"), strDataSource.c_str() );
		return false;
	}
	
	return true;
}

//
//	����ָ���Ķ���Ĳ������ݣ���xml�н�����pRootObj��object name��id�����Ҷ�������
//
bool CXmlLayoutParse::LoadLayout(Object* pRootObj, LayoutRes* pResLayout)
{
	Object* pObj = NULL;
	bool     bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. ��xml�ж�λ��pRootOb����������ݶ���xml���ƺͶ���id���ԣ�
		//
		if (false == m_xml.FindElem())               break;     // ����root��������
		if (false == m_xml.IntoElem())               break;

		if (false == m_xml.FindElem(XML_LAYOUT))     break;
		if (false == m_xml.IntoElem())               break;

		while (true)
		{
			if (false == m_xml.FindElem(pRootObj->GetObjectName()))
			{
				bRet = false;
				break;
			}

			if (m_xml.GetAttrib(XML_ID) != pRootObj->GetID())
			{
				continue;
			}
			else
			{
				bRet = true;
				break;
			}
		}
		if (!bRet)
		{
			UI_LOG_FATAL(_T("%s, δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),
			                FUNC_NAME, pRootObj->GetObjectName(), pRootObj->GetID());
			break;
		}

		bRet = false;      // reset

		//
		//  2. ����ָ������pRootOb������
		//
		this->loadAttributeForCurrentObjectInXml(pRootObj, pResLayout);

		bRet = true;
	}
	while (false);

	//
	//	3. ���ڿ�ʼ�ݹ����������ڵĿؼ�������
	//
	if (true == bRet)
	{
		this->loadObjects(pRootObj, pResLayout);

		// ����һ��֪ͨ��Ϣ���ڴ��������ڲ�����
		UISendMessage(pRootObj, UI_WM_OBJECTLOADED);
	}

	return bRet;
}

//
//	-- ����
//
bool  CXmlLayoutParse::ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild)
{
	Object*  pObj = NULL;
	bool      bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. ��xml�ж�λ��pRootOb����������ݶ���xml���ƺͶ���id���ԣ�
		//
		if (false == m_xml.FindElem())              break;     // ����root��������
		if (false == m_xml.IntoElem())              break;

		if (false == m_xml.FindElem(XML_LAYOUT))    break;
		if (false == m_xml.IntoElem())              break;

		while (true)
		{
			if (false == m_xml.FindElem( pRootObj->GetObjectName()))
			{
				bRet = false;
				break;
			}

			if (m_xml.GetAttrib(XML_ID) != pRootObj->GetID())
			{
				continue;
			}
			else
			{
				bRet = true;
				break;
			}
		}
		if (!bRet)
		{
			UI_LOG_FATAL( _T("%s, δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),
				FUNC_NAME, pRootObj->GetObjectName(), pRootObj->GetID());
			break;
		}

		bRet = false;      // reset

		//
		//  2. ����ָ������pRootOb������
		//
		this->loadAttributeForCurrentObjectInXml(pRootObj, NULL, true);

		bRet = true;
	}
	while(false);

	//
	//	3. ���ڿ�ʼ�ݹ����������ڵĿؼ�������
	//
	if (true == bRet)
	{
		this->reloadObjects(pRootObj, listAllChild);
	}

	return bRet;
}
#if 0
IObject*  CXmlLayoutParse::LoadRootObject(const TCHAR* szTag, const TCHAR szId)
{
    if (NULL == szTag || NULL == szId)
        return NULL;

    IObject*  pObject = NULL;
    bool  bRet = false;

    String  strTag = szTag;
    String  strId = szId;

    this->m_xml.ResetPos();

    do
    {
        //
        //  1. ��xml�ж�λ��pRootOb����������ݶ���xml���ƺͶ���id���ԣ�
        //
        if (false == m_xml.FindElem())              break;     // ����root��������
        if (false == m_xml.IntoElem())              break;

        if (false == m_xml.FindElem(XML_LAYOUT))    break;
        if (false == m_xml.IntoElem())              break;

        while (true)
        {
            if (false == m_xml.FindElem(strTag))
            {
                bRet = false;
                break;
            }

            if (m_xml.GetAttrib(XML_ID) != strId)
            {
                continue;
            }
            else
            {
                bRet = true;
                break;
            }
        }
        if (!bRet)
        {
            UI_LOG_FATAL( _T("%s, δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),
                FUNC_NAME, XML_MENU, szId);
            break;
        }

        bRet = false;      // reset

        m_pUIApp->CreateInstanceByName(szTag, &pObject);

        //
        //  2. 
        //
        this->loadAttributeForCurrentObjectInXml(pMenu, NULL);

        bRet = true;
    }
    while(false);

    //
    //	3. ���ڿ�ʼ�ݹ����������ڵĿؼ�������
    //
    if (true == bRet )
    {
        this->loadMenuItems( pMenu );
    }

    return pMenu;
}
#endif
#if 0 // -- �ܹ�����
Menu* CXmlLayoutParse::LoadMenu(const String& strMenuId)
{
	Menu*    pMenu = NULL;
	bool     bRet = false;

	this->m_xml.ResetPos();

	do
	{
		//
		//  1. ��xml�ж�λ��pRootOb����������ݶ���xml���ƺͶ���id���ԣ�
		//
		if (false == m_xml.FindElem())              break;     // ����root��������
		if (false == m_xml.IntoElem())              break;

		if (false == m_xml.FindElem(XML_LAYOUT))    break;
		if (false == m_xml.IntoElem())              break;

		while (true)
		{
			if (false == m_xml.FindElem(XML_MENU))
			{
				bRet = false;
				break;
			}

			if (m_xml.GetAttrib(XML_ID) != strMenuId)
			{
				continue;
			}
			else
			{
				bRet = true;
				break;
			}
		}
		if (!bRet)
		{
			UI_LOG_FATAL( _T("%s, δ�ҵ�Ҫ���صĶ���name=%s, id=%s"),
				       FUNC_NAME, XML_MENU, strMenuId.c_str());
			break;
		}

		bRet = false;      // reset

		UICreateInstance(&pMenu, m_pUIApp);  // TODO:
		pMenu->ModifyStyle(MENU_STYLE_AUTO_DELETE_SUBMENU);

		//
		//  2. 
		//
		this->loadAttributeForCurrentObjectInXml(pMenu, NULL);

		bRet = true;
	}
	while(false);

	//
	//	3. ���ڿ�ʼ�ݹ����������ڵĿؼ�������
	//
	if (true == bRet )
	{
		this->loadMenuItems( pMenu );
	}

	return pMenu;
}

#endif
IStyleManager*  CXmlLayoutParse::getStyleMgr()
{
	if (NULL == m_pUIApp)
		return NULL;

	ISkinManager* pSkinMgr = NULL;
	m_pUIApp->GetSkinMgr(&pSkinMgr);
	if (NULL == pSkinMgr)
		return NULL;

	ISkinRes* pSkinRes = NULL;
	pSkinMgr->GetActiveSkin(&pSkinRes);
	if (NULL == pSkinRes)
		return NULL;

	IStyleManager* pStyleMgr = NULL;
	pSkinRes->GetStyleManager(&pStyleMgr);
	
	return pStyleMgr;
}

#if 0 // -- �ܹ�����
bool CXmlLayoutParse::loadMenuItems(MenuBase* pParentMenu)
{
	bool bRet = false;

	IStyleManager* pStyleMgr = this->getStyleMgr();
	if (NULL == pStyleMgr)
		return false;

	if (false == m_xml.IntoElem())
		return false;

	// ���������Ӷ���
	while(true)
	{
		bool bRet = this->m_xml.FindElem();
		if (false == bRet )     break;

		Object*  pObj = NULL;
		String   tagName = this->m_xml.GetTagName();
		if (_T("") == tagName)
		{
			UI_LOG_WARN( _T("%s, xml invalid tag name."), FUNC_NAME );
			return false;
		}

		//	������������
		ATTRMAP  mapAttrib;
		for( int j = 0; ; j++ )
		{
			String key = m_xml.GetAttribName(j);
			if (_T("") == key )
				break;

			mapAttrib[key]= m_xml.GetAttrib( key );
		}

		// ���⴦����չstyle
		String strTagName, strStyleClass, strID;

		ATTRMAP::iterator iter = mapAttrib.find(XML_ID); // id = Menu.id����Ϊ�����id��������ֵ
		if (mapAttrib.end() != iter)
		{
			strID = pParentMenu->GetObjectName();
			strID.push_back(XML_CHILD_SEPARATOR);
			strID.append(iter->second);
		}
		iter = mapAttrib.find(XML_STYLECLASS);
		if (mapAttrib.end() != iter)
		{
			strStyleClass = iter->second;
		}
		strTagName = pParentMenu->GetObjectName();   // tagName = Menu.String / Menu.Popup / Menu.Separator
		strTagName.push_back(XML_CHILD_SEPARATOR);
		strTagName.append(tagName);

		pStyleMgr->LoadStyle(strTagName, strStyleClass, strID, mapAttrib);

		// ���ز˵�����
		MenuItem* pItem = pParentMenu->LoadMenuItem(tagName, mapAttrib);
		if (pItem && pItem->IsPopup())
		{
			MenuBase* pSubMenu = pItem->GetSubMenu();
			this->loadAttributeForCurrentObjectInXml(pSubMenu, NULL);
			this->loadMenuItems(pSubMenu);
		}
	}

	m_xml.OutOfElem();

	return bRet;
}
#endif

Object*  CXmlLayoutParse::mapXmlToClass(const String& strXmlName)
{
	IObject* pIObject = NULL;
	m_pUIApp->CreateInstanceByName((BSTR)strXmlName.c_str(), &pIObject);
    if (NULL == pIObject)
        return NULL;

    Object* p = pIObject->GetObjectImpl();
	if (NULL == p && m_pUIApp->IsDesignMode())
	{
		TCHAR szText[128] = _T("");
		_stprintf(szText, _T("%s CreateInstance Failed, xmlName=%s. �ÿؼ������ڱ���ʱ����ʧ."), FUNC_NAME, strXmlName.c_str());
		::MessageBox(NULL, szText, _T("Error"), MB_OK|MB_ICONERROR);
	}
	return p;
}

//
//	Ϊm_mxl��ǰָ��Ķ��������������
//
bool CXmlLayoutParse::loadAttributeForCurrentObjectInXml(Object* pObj, LayoutRes* pResLayout, bool bReload/*=false*/)
{
	IStyleManager* pStyleMgr = this->getStyleMgr();
	if (NULL == pStyleMgr)
		return false;

	LayoutResItem* pResItem = NULL;
	if (pResLayout)
	{
		pResItem = pResLayout->GetResItem(pObj);
		if (NULL == pResItem)
			pResItem = pResLayout->AddResItem(pObj);
	}

    IMapAttribute* pMapAttrib = NULL;
    UICreateIMapAttribute(&pMapAttrib);
    UIASSERT(pMapAttrib);

	//
	//	������������
	//
	for (int j = 0; ; j++)
	{
		String key = m_xml.GetAttribName(j);
		if (_T("") == key)
			break;

        pMapAttrib->AddAttr(key.c_str(), m_xml.GetAttrib(key).c_str());
	}

	if (m_pUIApp->IsDesignMode() && NULL != pResItem)
	{
		pResItem->SetSelfAttr(pMapAttrib);
	}

	//
	// ���⴦����չstyle
	//

    String strID, strStyleClass;  // ����pMapAttrib->GetAttr������ʱ������
    {
	    const TCHAR*  szID = pMapAttrib->GetAttr(XML_ID, false);
        if (szID)
            strID = szID;

    	const TCHAR*  szStyleClass = pMapAttrib->GetAttr(XML_STYLECLASS, false);
        if (szStyleClass)
            strStyleClass = szStyleClass;
    }

	const TCHAR*  szTagName = pObj->GetObjectName();

	if (m_pUIApp->IsDesignMode() && NULL != pResItem)
	{
		//ATTRMAP attrMapStyle;
        IMapAttribute* pAttrMapStyle = NULL;
        UICreateIMapAttribute(&pAttrMapStyle);
		pStyleMgr->LoadStyle(szTagName, strStyleClass.c_str(), strID.c_str(), pAttrMapStyle);
		pResItem->SetStyleAttr(pAttrMapStyle);
        SAFE_RELEASE(pAttrMapStyle);

		pStyleMgr->LoadStyle(szTagName, strStyleClass.c_str(), strID.c_str(), pMapAttrib);
		pResItem->SetLastAttr(pMapAttrib);

#if 0 // -- �ܹ�����
		IUIBuilder* pUIBuilder = NULL;
		m_pUIApp->GetUIBuilderPtr(&pUIBuilder);
		if (pUIBuilder)
		{
			pUIBuilder->OnObjectAttributeLoad(pObj, static_cast<ILayoutResItem*>(pResItem));
		}
#endif
	}
	else
	{
		pStyleMgr->LoadStyle(szTagName, strStyleClass.c_str(), strID.c_str(), pMapAttrib);  // �Ǳ༭ģʽ�£�ֱ��ʹ�ø�mapattrib
	}

	// 
	//	�����key��value�������ɶ����Լ�ȥ����
	//
    if (bReload)
    {
        UISendMessage(pObj, UI_WM_RESETATTRIBUTE);
    }
    UISendMessage(pObj, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)bReload);
    SAFE_RELEASE(pMapAttrib);

// 	if (bReload)
// 	{
// 		pObj->ResetAttribute();
// 	}
// 	pObj->SetAttribute(mapAttrib, bReload);

	return true;
}

//
// �ݹ飬���������Ӷ����Ӷ��������
//
bool CXmlLayoutParse::loadObjects(Object* pObjParent, LayoutRes* pResLayout )
{
	bool bRet = false;

	if (false == m_xml.IntoElem())
		return false;

	// ���������Ӷ���
	while (true)
	{
		bool bRet = this->m_xml.FindElem();
		if (false == bRet)     break;

		Object*  pObj = NULL;
		String   tagName = this->m_xml.GetTagName();
		if (_T("") == tagName)
		{
			UI_LOG_WARN( _T("LayoutXmlParse::loadObjects, xml invalid tag name.") );
			return false;
		}
		else if (XML_RENDERCHAIN == tagName)
		{
			m_xml.IntoElem();
			this->loadRenderChain(pObjParent);
			m_xml.OutOfElem();
			continue;
		}

		// ����ǩ��ת��Ϊ��
		pObj = this->mapXmlToClass(tagName);
		if (NULL == pObj)
		{
			continue;
		}

		// ����
		pObjParent->AddChild(pObj);

		// ��������
		this->loadAttributeForCurrentObjectInXml(pObj, pResLayout);

		// Ĭ�Ͻ��ÿؼ������һ��notify objectΪ���ڶ��󣬲���msgmapid����Ϊ0
        WindowBase* pWindowBase = pObj->GetWindowObject();
        if (pWindowBase)
        {
		    pObj->SetNotify(pWindowBase->GetIMessage(), 0);
        }

		// �ݹ飬��������Ӷ�����Ӷ���
		this->loadObjects(pObj, pResLayout);

		// ����һ��֪ͨ��Ϣ���ڴ��������ڲ�����
        UISendMessage(pObj, UI_WM_OBJECTLOADED);
    }

    m_xml.OutOfElem();

    return bRet;
}


bool CXmlLayoutParse::reloadObjects(Object* pObjParent, list<Object*>& listAllChild)
{

	bool bRet = false;

	if (false == m_xml.IntoElem() )
		return false;

	// ���������Ӷ���
	while (true)
	{
		bool bRet = this->m_xml.FindElem();
		if (false == bRet)     break;

		Object*  pObj = NULL;
		String   tagName = this->m_xml.GetTagName();
		if (_T("") == tagName)
		{
			UI_LOG_WARN( _T("LayoutXmlParse::loadObjects, xml invalid tag name.") );
			continue;
		}
		String   strID = this->m_xml.GetAttrib( XML_ID );
		if (strID.empty())
		{
		}
		else
		{
			// ���� tagName + id ��listAllChild�в��Ҹö���
			list<Object*>::iterator  iter = listAllChild.begin();
			list<Object*>::iterator  iterEnd = listAllChild.end();

			for ( ; iter != iterEnd; iter++ )
			{
				Object* pTempObj = *iter;
				if (pTempObj->GetObjectName() == tagName && pTempObj->GetID() == strID)
				{
					pObj = pTempObj;
					break;
				}
			}
		}

		if (NULL == pObj)
		{
			// û���ҵ�
			// ֱ������������
			// ����ǩ��ת��Ϊ��
			pObj = this->mapXmlToClass( tagName );
			if (NULL == pObj)
			{
				continue;
			}

			// ����
			pObjParent->AddChild(pObj);

			// ��������
			this->loadAttributeForCurrentObjectInXml(pObj, false);

			// Ĭ�Ͻ��ÿؼ������һ��notify objectΪ���ڶ��󣬲���msgmapid����Ϊ0
            WindowBase* pWindowBase = pObj->GetWindowObject();
            if (pWindowBase)
            {
			    pObj->SetNotify(pWindowBase->GetIMessage(), 0);
            }

			// �ݹ飬��������Ӷ�����Ӷ���
			this->reloadObjects(pObj, listAllChild);
		}
		else
		{
			// ����
			pObjParent->AddChild(pObj);

			// ��������
			this->loadAttributeForCurrentObjectInXml(pObj, NULL, true);

			// �ݹ飬��������Ӷ�����Ӷ���
			this->reloadObjects(pObj, listAllChild);

			// ���б����Ƴ�
			list<Object*>::iterator iter = listAllChild.begin();
			list<Object*>::iterator iterEnd = listAllChild.end();
			for (; iter != iterEnd; iter++)
			{
				if (pObj == (*iter))
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
bool CXmlLayoutParse::loadRenderChain(Object* pObjParent)
{
	if (NULL == pObjParent || OBJ_WINDOW != pObjParent->GetObjectType())
		return false;

	IWindowBase* pWindow = (IWindowBase*)pObjParent;
	while (m_xml.FindElem())
	{
		if (m_xml.GetTagName() != XML_RENDERCHAIN_LAYER)
			continue;

        IMapAttribute*  pMapAttrib = NULL;
        UICreateIMapAttribute(&pMapAttrib);

		for (int j = 0; ; j++)
		{
			String key = m_xml.GetAttribName(j);
			if (_T("") == key)
				break;

			String value = m_xml.GetAttrib( key );
            pMapAttrib->AddAttr(key.c_str(), m_xml.GetAttrib(key).c_str());
		}
		pWindow->InsertRenderLayer(pMapAttrib);
        SAFE_RELEASE(pMapAttrib);
	}
	return true;
}

bool CXmlLayoutParse::LoadAllWindowNodeInfo(LayoutWindowNodeInfo* pInfoArray, int* pArraySize)
{
	m_xml.ResetPos();

	int nIndex = 0;
	do
	{
		//
		//  1. ��xml�ж�λ��pRootOb����������ݶ���xml���ƺͶ���id���ԣ�
		//
		if (false == m_xml.FindElem( ) )             break;     // ����root��������
		if (false == m_xml.IntoElem() )              break;

		if (false == m_xml.FindElem( XML_LAYOUT) )   break;
		if (false == m_xml.IntoElem() )              break;

		while (m_xml.FindElem())
		{
			if (pInfoArray)
			{
				String strTagName = m_xml.GetTagName();
				String strNodeID = m_xml.GetAttrib(XML_ID);

				_tcsncpy(pInfoArray[nIndex].szNodeName, strTagName.c_str(), MAX_STRING_LEN-1);
				_tcsncpy(pInfoArray[nIndex].szNodeID,   strNodeID.c_str(),   MAX_STRING_LEN-1);
			}
			nIndex++;
		}
	}
	while(0);

	if (NULL == pInfoArray && NULL != pArraySize)
	{
		*pArraySize = nIndex;
	}
	
	return true;
}
#endif