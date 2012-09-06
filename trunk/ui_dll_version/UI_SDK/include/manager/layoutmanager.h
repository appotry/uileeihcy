#pragma once
namespace UI
{

	//////////////////////////////////////////////////////////////////////////
	//                                                                      //
	//                          xml布局                                     //
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
		// 数据持久层
		ILayoutParse*    m_pLayoutParse;

		// 对象属性
		CPojo_Layout     m_pojoLayout;
	};;

}