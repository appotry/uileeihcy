#pragma once
namespace UI
{

	//////////////////////////////////////////////////////////////////////////
	//                                                                      //
	//                          xml����                                     //
	//                                                                      //
	//////////////////////////////////////////////////////////////////////////

	class LayoutManager
	{
	public:
		LayoutManager(void);
		~LayoutManager(void);
		
		bool     Create( const String& strXmlPath );
		bool     Load( const String& strXmlPath );
		bool     Save();
		void     Clear();
		bool     LoadLayout(Object* pRootObj);
		bool     ReLoadLayout( Object* pRootObj, list<Object*>& listAllChild );
		Menu*    LoadMenu( const String& strMenuId );

		ILayoutParse* GetLayoutParse() { return m_pLayoutParse; }

	private:
		// ���ݳ־ò�
		ILayoutParse*    m_pLayoutParse;

		// ��������
		CPojo_Layout     m_pojoLayout;
	};;

}