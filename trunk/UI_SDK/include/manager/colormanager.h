#pragma once
namespace UI
{


	//////////////////////////////////////////////////////////////////////////
	//                                                                      //
	//                             ÑÕÉ«                                     //
	//                                                                      //
	//////////////////////////////////////////////////////////////////////////

// 	class ColorItem
// 	{
// 	public:
// 		ColorItem()
// 		{
// 			strName = _T("default");
// 			rgb     = RGB(0,0,0);
// 		}
// 
// 		String     strName;      //  COLÃû³Æ
// 		COLORREF   rgb;
// 	};

	class ColorManager
	{
	public:
		ColorManager(void);
		~ColorManager(void);

	public:
		bool       Create(const String& strXmlPath );
		bool       Load( const String& strXmlPath );
		bool       Save();
		bool       Reload();
		void       Clear();
		int        GetColorCount();
		bool       GetColorItemInfo( int nIndex, IColorItemInfo** ppColorItemInfo );
		bool       GetColor( const String& strName, UIColor** pCol );
		bool       InsertColorItem( const String& strName, const String& strColor );
		bool       ModifyColorItem( const String& strName, const String& strColor );
		bool       RemoveColorItem( const String& strID );

		IColorParse*   GetColorParse() { return m_pColorParse; }

	private:
		IColorParse*    m_pColorParse;
		CPojo_Color     m_pojoColor;              

	};
}