#pragma once


//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             图片                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

namespace UI
{


// 在CUIApplication中被声明
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
	// 数据持久层
	IImageParse*    m_pImageParse;

	// 对象属性
	CPojo_Image     m_pojoImage;    // 界面图片
	CPojo_Cursor    m_pojoCursor;   // 鼠标样式图片

};

}