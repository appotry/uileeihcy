#pragma once


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             ͼƬ                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

namespace UI
{


// ��CUIApplication�б�����
class ImageManager
{
public:
	ImageManager(void);
	~ImageManager(void);

public:
	bool       Create( const String& strXmlPath );
	bool       Load( const String& strXmlPath );
	bool       Reload();
	bool       Save();
	void       Clear();
//	bool       GetImage( const String& strID, UIImage** ppImage );
	bool       GetCursor( const String& strID, UICursor** ppCursor );
	bool       InsertImageItem( const String& strID, const String& strPath );
	bool       ModifyImageItem( const String& strID, const String& strPath );
	bool       RemoveImageItem( const String& strID );
	bool       ChangeSkinHLS(short h, short l, short s, int nFlag);

	HRBITMAP   GetImage( const String& strID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );

	IImageParse*   GetImageParse() { return m_pImageParse; }
	void           SetImageParse( IImageParse* p );

	int            GetImageCount( );
	bool           GetImageItemInfo( int nIndex, IImageItemInfo** ppImageItemInfo );

private:
	// ���ݳ־ò�
	IImageParse*    m_pImageParse;

	// ��������
	CPojo_Image     m_pojoImage;    // ����ͼƬ
	CPojo_Cursor    m_pojoCursor;   // �����ʽͼƬ

};

}