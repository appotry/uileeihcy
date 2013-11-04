#pragma once

namespace UI
{

class ImageData;
class GDIRenderBitmap;
class GdiplusRenderBitmap;

//
//	image中的一项信息数据
//
class ImageResItem
{
public:
	ImageResItem();
	~ImageResItem();

	bool  ModifyImage(const TCHAR*  szPath);
	IImageResItem*  GetIImageResItem(); 

private:
	String    m_strID;      // image id
	String    m_strPath;    // image path

	bool      m_bUseSkinHLS;         // 该图片是否参与皮肤色调改变 
	bool      m_bNeedAntiAliasing;   // 需要支持抗锯齿(gdi的alphablend缩放不支持SetStretchBltMode的HALFTONE)
	bool      m_bMustHasAlphaChannel;// 该图片是否需要创建alpha channel。1. 有alpha channel的图片一率采用gdi创建，采用alphablend绘制。2. 没有alpha channel的gdi图片将无法在分层窗口上面绘制

	IMapAttribute*   m_pMapAttrib;        // 为该图片配置的属性，例如imagelist的count，icon的width height
	IMAGE_ITEM_TYPE m_eType;         // 图片类型

	ImageData*            m_pOriginImageData;    // 该图片的原始数据（改变图片色调时使用）
	GDIRenderBitmap*      m_pGdiBitmap;          // 外部引用

	//  注：为了提高绘图效率，同时也避免一张图片加载多种形式，尽量使用GDI(AlphaBlend)来绘图。
	//	    需要注意的是：如果需要使用alpha通道时，必须强制创建一个32位的GDI BITMAP
	GdiplusRenderBitmap*  m_pGdiplusBitmap;      // 外部引用
	//	Direct2DRenderBitmap* m_pDirect2DBitmap;     // 外部引用

public:
	DECLARE_STRING_SETGET(ID);
	DECLARE_STRING_SETGET(Path);
	DECLARE_bool_SETGET(UseSkinHLS);
	DECLARE_bool_SETGET(NeedAntiAliasing)

	bool  ModifyHLS(short h, short l, short s, int nFlag);
	bool  ModifyHLS(IRenderBitmap* pBitmap, short h, short l, short s, int nFlag);
	bool  ModifyAlpha(int nAlphaPercent);
	IRenderBitmap*  GetImage(SkinRes* pSkinRes, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType = GRAPHICS_RENDER_LIBRARY_TYPE_GDI, bool* pbFirstTimeCreate=NULL);
	void  SetAttribute(const TCHAR* szType, IMapAttribute* pMapAttrib);

private:
	IImageResItem*  m_pIImageResItem;
};

//
//	image列表
//
class ImageRes
{
public:
	ImageRes(SkinRes*  pSkinRes);
	~ImageRes();

	IImageRes*  GetIImageRes();

	long  GetImageCount();
	IImageResItem*  GetImageResItem(long lIndex);
	IImageResItem*  GetImageResItem(const TCHAR* szID);
	bool ModifyImage(const TCHAR* szId, const TCHAR* szPath);

	HBITMAP  LoadBitmap(const TCHAR* szId);
	HRESULT  GetBitmap(const TCHAR* szImageID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderBitmap** pRenderBitmap);
private:
	vector<ImageResItem*>   m_vImages;

public:
	ImageResItem*  LoadItem(const TCHAR* szType, IMapAttribute* pMapAttrib, const TCHAR* szFullPath);

	ImageResItem*  GetImageItem2(int nIndex);
	ImageResItem*  GetImageItem2(const String& strID);

	ImageResItem*  InsertImage( const String& strID, const TCHAR* szPath);
	bool RemoveImage( const String& strID );
	bool Clear();
	bool ChangeSkinHLS(short h, short l, short s, int nFlag);
	bool ModifyImageItemAlpha(const String& strID, int nAlphaPercent);

private: 
	IImageRes*  m_pIImageRes;
	SkinRes*  m_pSkinRes;
};

}