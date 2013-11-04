#pragma once

namespace UI
{

class GifImageBase;
class GifResItem
{
public:
	GifResItem(IUIApplication* pUIApp);
	~GifResItem();

private:
	String      m_strID;      // image id
	String      m_strPath;    // image path
	bool        m_bUseSkinHLS;         // 该图片是否参与皮肤色调改变 
	ImageData*  m_pOriginImageData;    // 该图片的原始数据（改变图片色调时使用）

public:
	bool  ModifyGif(const String& strPath) { UIASSERT(0); return false; } // TODO:
	bool  ModifyHLS(short h, short l, short s, int nFlag){ UIASSERT(0); return false; } // TODO:
	void  SetAttribute(IMapAttribute* pMapAttrib);
	GifImageBase*  GetGifImage(SkinRes* pSkinRes);
	IGifResItem*  GetIGifResItem();

	DECLARE_STRING_SETGET( ID );
	DECLARE_STRING_SETGET( Path );
	DECLARE_bool_SETGET(UseSkinHLS);

public:
	IMapAttribute*  m_pMapAttrib;
	GifImageBase*   m_pGifImage;
	IUIApplication* m_pUIApp;
	IGifResItem*    m_pIGifResItem;
};

class GifRes
{
public:
	GifRes(SkinRes* p);
	~GifRes();

	IGifRes*  GetIGifRes();

	HRESULT  GetGifImage(const TCHAR*  szId, __out IGifImage** ppGifImage);

public:
	GifResItem*  LoadItem(IMapAttribute* pMapAttrib, const String& strFullPath);
	void SetUIApplication(IUIApplication* p)
	{ m_pUIApp = p; }

	int  GetGifCount(); 
	GifResItem* GetGifItem(int nIndex);
	GifResItem* GetGifItem(const TCHAR*  szId);

	bool InsertGif(const String& strID, const String& strPath, GifResItem** pRet);
	bool ModifyGif(const String& strID, const String& strPath);
	bool RemoveGif(const String& strID);
	bool Clear();
	bool ChangeSkinHLS(short h, short l, short s, int nFlag){UIASSERT(0); return false;};  // TODO:

private:
	IGifRes*  m_pIGifRes;
	vector<GifResItem*>   m_vGifs;
	IUIApplication*       m_pUIApp;
	SkinRes*  m_pSkinRes;
};


}