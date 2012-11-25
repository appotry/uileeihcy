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


class GifOleObject : public IGifOleObject, public RichEditOleObjectItem_Inner
{
public:
	GifOleObject(CPojo_Gif* pGifMgr, Message* pNotifyObj);
	~GifOleObject();

// public:
// 	static  HRESULT CreateInstance(REFIID riid, void** ppv);

#pragma  region // igifoleobject
	virtual HRESULT __stdcall LoadGif(const TCHAR* szPath);
	virtual HRESULT __stdcall Refresh();
#pragma endregion

#pragma region // iviewobject
	virtual HRESULT OnDraw(HDC hDC, RECT* prc);
	virtual HRESULT OnGetSize(SIZE* pSize);
	virtual HRESULT GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj);
#pragma endregion

protected:
	GifImageRenderItem*    m_pGifRenderItem;
	CPojo_Gif*       m_pGifMgr;
	Message*         m_pNotifyMsg;
};


}