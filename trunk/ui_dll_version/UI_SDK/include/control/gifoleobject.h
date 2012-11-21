#pragma 
#include "../util/GifImage.h"
#include "richeditolemgr.h"

namespace UI
{
class GifImageItem;
class GifImageItemMgr;

// {2EAE75F5-D78F-43ca-811D-8F8B01CCE05B}
extern const GUID IID_IGifOleObject; 
class IGifOleObject
{
public:
	virtual HRESULT __stdcall LoadGif(const TCHAR* szPath) = 0;
	virtual HRESULT __stdcall Refresh() = 0;
};


class GifOleObject : public IGifOleObject, public RichEditOleObjectItem
{
public:
	GifOleObject(GifImageItemMgr* pMgr);
	~GifOleObject();

// public:
// 	static  HRESULT CreateInstance(REFIID riid, void** ppv);

#pragma  region // igifoleobject
	virtual HRESULT __stdcall LoadGif(const TCHAR* szPath);
	virtual HRESULT __stdcall Refresh();
#pragma endregion

#pragma region // iviewobject
	virtual HRESULT STDMETHODCALLTYPE Draw(DWORD dwDrawAspect, LONG lindex,  void *pvAspect,  DVTARGETDEVICE *ptd, HDC hdcTargetDev, HDC hdcDraw, LPCRECTL lprcBounds, LPCRECTL lprcWBounds, BOOL ( STDMETHODCALLTYPE *pfnContinue )(ULONG_PTR dwContinue), ULONG_PTR dwContinue);
#pragma endregion

protected:
	GifImageItem*    m_pGifItem;
	GifImageItemMgr* m_pGifItemMgr;
};


// gifͼ����������ͬһgif�ļ������Σ���ֻ����һ���ڴ�
enum GifImageLoadType
{
	GifImageLoadType_None,
	GifImageLoadType_File,
	GifImageLoadType_ImageID,
};

// һ��GIF�ļ���Ӧһ��GifImageItem
// ���ܶ��GifOleObject����һ��GifImageItem::GifImage����
class GifImageItem
{
public:
	GifImageItem();
	~GifImageItem();
	int       AddRef();
	int       Release();

protected:
	// ��GigImageItemMgr���õĺ���
	bool      LoadGifByPath(const TCHAR* szFilePath); 

public:
	TCHAR*             m_szFileIDorName;
	GifImageLoadType   m_eFileLoadType;  
	GifImage*          m_pGif;

protected:
	int       m_dwRef;

	friend class GifImageItemMgr;
};

// ���� GifImageItem
class GifImageItemMgr
{
public:
	GifImageItemMgr(){};
	~GifImageItemMgr(){};

	bool   LoadGifByPath(const TCHAR* szFilePath, GifImageItem** ppGifImageItem);
	bool   LoadGifByID(const TCHAR* szImageID, GifImageItem** ppGifImageItem);

protected:
	GifImageItem*  FindItem(GifImageLoadType eType, const TCHAR* szFileIDorName);

protected:
	list<GifImageItem*>   m_listGifImageFile;
};
}