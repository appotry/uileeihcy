#pragma once

namespace UI
{

class ImageData;
class GDIRenderBitmap;
class GdiplusRenderBitmap;

//
//	image�е�һ����Ϣ����
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

	bool      m_bUseSkinHLS;         // ��ͼƬ�Ƿ����Ƥ��ɫ���ı� 
	bool      m_bNeedAntiAliasing;   // ��Ҫ֧�ֿ����(gdi��alphablend���Ų�֧��SetStretchBltMode��HALFTONE)
	bool      m_bMustHasAlphaChannel;// ��ͼƬ�Ƿ���Ҫ����alpha channel��1. ��alpha channel��ͼƬһ�ʲ���gdi����������alphablend���ơ�2. û��alpha channel��gdiͼƬ���޷��ڷֲ㴰���������

	IMapAttribute*   m_pMapAttrib;        // Ϊ��ͼƬ���õ����ԣ�����imagelist��count��icon��width height
	IMAGE_ITEM_TYPE m_eType;         // ͼƬ����

	ImageData*            m_pOriginImageData;    // ��ͼƬ��ԭʼ���ݣ��ı�ͼƬɫ��ʱʹ�ã�
	GDIRenderBitmap*      m_pGdiBitmap;          // �ⲿ����

	//  ע��Ϊ����߻�ͼЧ�ʣ�ͬʱҲ����һ��ͼƬ���ض�����ʽ������ʹ��GDI(AlphaBlend)����ͼ��
	//	    ��Ҫע����ǣ������Ҫʹ��alphaͨ��ʱ������ǿ�ƴ���һ��32λ��GDI BITMAP
	GdiplusRenderBitmap*  m_pGdiplusBitmap;      // �ⲿ����
	//	Direct2DRenderBitmap* m_pDirect2DBitmap;     // �ⲿ����

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
//	image�б�
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