#pragma once
namespace UI
{

	//////////////////////////////////////////////////////////////////////////
	//                                                                      //
	//                             ��ʽ                                     //
	//                                                                      //
	//////////////////////////////////////////////////////////////////////////

	class StyleManager
	{
	public:
		StyleManager(void);
		~StyleManager(void);
		
		bool     Create( const String& strXmlPath );
		bool     Load( const String& strXmlPath );
		bool     Reload();
		bool     Save();
		void     Clear();
		int      GetStyleCount( );
		bool     GetStyleItemInfo( int nIndex, IStyleItemInfo** ppStyleItemInfo );
		bool     InsertStyleItem( STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit );
		bool     ModifyStyleItem( STYLE_SELECTOR_TYPE  type, const String& strID, const String& strInherit );
		bool     RemoveStyleItem( STYLE_SELECTOR_TYPE  type, const String& strID );
		bool     InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
		bool     ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey, const String& strValue );
		bool     RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const String& strID, const String& strKey );

		bool     LoadStyle( const String& strTagName, const String& strStyleClass, const String& strID, map<String,String>& mapStyle );

		IStyleParse*    GetStyleParse() { return m_pStyleParse; }

		
	private:
		// ���ݳ־ò�
		IStyleParse*    m_pStyleParse;

		// ��������
		CPojo_Style     m_pojoStyle;
	};;

}