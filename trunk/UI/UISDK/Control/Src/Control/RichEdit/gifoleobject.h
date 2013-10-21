#pragma 
#include "UISDK\Kernel\Inc\Util\igifimage.h"
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
    virtual HRESULT __stdcall LoadSkinGif(const TCHAR* szId) = 0;
	virtual HRESULT __stdcall Refresh() = 0;
};


class GifOleObject : public IGifOleObject, public RichEditOleObjectItem_Inner
{
public:
	GifOleObject(IUIApplication* pUIApp, IMessage* pNotifyObj);
	~GifOleObject();

// public:
// 	static  HRESULT CreateInstance(REFIID riid, void** ppv);

#pragma  region // igifoleobject
	virtual HRESULT __stdcall LoadGif(const TCHAR* szPath);
    virtual HRESULT __stdcall LoadSkinGif(const TCHAR* szId);
	virtual HRESULT __stdcall Refresh();
#pragma endregion

#pragma region // iviewobject
	virtual HRESULT OnDraw(HDC hDC, RECT* prc);
	virtual HRESULT OnGetSize(SIZE* pSize);
	virtual HRESULT GetClipboardData(CHARRANGE FAR * lpchrg, DWORD reco, LPDATAOBJECT FAR * lplpdataobj);
#pragma endregion

#pragma region // RichEditOleObject
    virtual HRESULT  GetEncodeText(BSTR* pbstr);
#pragma endregion
protected:

    IUIApplication*  m_pUIApp;
    IGifImage*  m_pGifImage;         // 从文件加载的图片, 从skin加载时，m_pGifImage为空不使用，这样也不用管释放 了
    IGifImageRender*  m_pGifRender;  // gif绘制接口

	IMessage*   m_pNotifyMsg;
	String      m_strPath;
};


}