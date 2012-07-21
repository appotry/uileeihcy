#pragma once
namespace UI
{

	//////////////////////////////////////////////////////////////////////////
	//                                                                      //
	//                             ����                                     //
	//                                                                      //
	//////////////////////////////////////////////////////////////////////////


// 	class FontItem
// 	{
// 	public:
// 		FontItem()
// 		{
// 			nHeight      = 180;
// 			nOrientation = 0;
// 			bBold        = false;
// 			bUnderline   = false;
// 			bStrikeout   = false;
// 			strFacename  = _T("����");
// 		}
// 
// 		String   strID;      //  font����
// 
// 		int      nHeight;      //  ��С
// 		int      nOrientation; //  ��ת�Ƕ�
// 		bool     bBold;        //  �Ƿ�Ϊ����//int  nWeight;0-1000, 0ΪĬ��	400ΪNORMAL 700ΪBOLD
// 		bool     bItalic;      //  �Ƿ�Ϊб��
// 		bool     bUnderline;   //  �Ƿ����»���
// 		bool     bStrikeout;   //  �Ƿ���ɾ����
// 		String   strFacename;  //  ��������
// 
// 	};

	// ��CUIApplication�б�����
	class FontManager
	{
	public:
		FontManager(void);
		~FontManager(void);

	public:
		bool       Create(const String& strXmlPath );
		bool       Load( const String& strXmlPath );
		bool       Reload();
		bool       Save();
		void       Clear();
		HRFONT     GetFont( const String& strID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		HRFONT     GetFontEx( HRFONT hRFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
		int        GetFontCount();
		bool       GetFontItemInfo( int nIndex, IFontItemInfo** ppFontItemInfo );
		bool       InsertFontItem( const String& strID, LOGFONT* pLogFont );
		bool       ModifyFontItem( const String& strID, LOGFONT* pLogFont );
		bool       RemoveFontItem( const String& strID );
		
		IFontParse*   GetFontParse() { return m_pFontParse; }
		void          SetFontParse( IFontParse* p );

	private:
		/*map<String, HFONT>   m_map;*/

		// ���ݳ־ò�
		IFontParse*    m_pFontParse;

		// ��������
		CPojo_Font     m_pojoFont;
	};

}