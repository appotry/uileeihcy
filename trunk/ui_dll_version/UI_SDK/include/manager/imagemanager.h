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
	ImageManager(SkinManager* pSkinMgr);
	~ImageManager(void);

public:
	bool       Create( const String& strXmlPath );
	bool       Load( const String& strXmlPath );
	bool       Reload();
	bool       Save();
	void       Clear();
//	bool       GetImage( const String& strID, UIImage** ppImage );
	bool       GetCursor(const String& strID, UICursor** ppCursor);
	bool       InsertImageItem(const String& strID, const String& strPath);
	bool       ModifyImageItem(const String& strID, const String& strPath);
	bool       RemoveImageItem(const String& strID);
	bool       ChangeSkinHLS(short h, short l, short s, int nFlag);

	bool       ModifyImageItemInRunTime(const String& strID, const String& strPath);
	bool       ModifyImageItemAlpha(const String& strID, int nAlphaPercent);

	HRBITMAP   GetImage( const String& strID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
	GifImageBase* GetGifImage( const String& strID);

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
	CPojo_Gif       m_pojoGif;      // ����

	SkinManager*    m_pSkinMgr;
};

}