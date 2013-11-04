#pragma once
#include "UISDK\Kernel\Src\SkinParse\skinparseengine.h"
#include "UISDK\Kernel\Src\Resource\uiresource.h"

//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                             图片                                     //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

namespace UI
{

class ImageManager
{
public:
	ImageManager(SkinRes* pSkinRes, IUIApplication* pUIApp);
	~ImageManager(void);

    IImageManager*  GetIImageManager();

	IImageRes*   GetImageRes();
	ICursorRes*  GetCursorRes();
	IGifRes*     GetGifRes();

	IImageResItem*  InsertImageItem(const TCHAR* szID, const TCHAR* szPath);
	HRESULT  ModifyImageItem(const TCHAR* szID, const TCHAR* szPath);
	HRESULT  RemoveImageItem(const TCHAR* szID);

	HRESULT  ModifyImageItemInRunTime(const TCHAR* szID, const TCHAR* szPath);
	HRESULT  ModifyImageItemAlpha(const TCHAR* szID, int nAlphaPercent);

    static HRESULT  UIParseSkinElement(IUIElement*, ISkinRes* pSkinRes);

public:
	bool  Create( const String& strXmlPath );
	bool  Load( const String& strXmlPath );
	bool  Reload();
	void  Clear();

	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);

//--	bool       GetCursor(const String& strID, UICursor** ppCursor);
//--	IRenderBitmap*   GetImage( const String& strID, GRAPHICS_RENDER_TYPE eRenderType = GRAPHICS_RENDER_TYPE_GDI );
//--	GifImageBase* GetGifImage( const String& strID);

	int  GetImageCount( );
	IImageResItem*  GetImageItemInfo(int nIndex);

protected:
    // ISkinTagParse
    HRESULT  ParseNewElement(IUIElement* pElem);
    void  OnNewChild(IUIElement* pElem);

private:
	// 数据持久层
    IImageManager*  m_pIImageManager;
    IUIElement*  m_pUIElement;

	// 对象属性
	ImageRes        m_resImage;    // 界面图片
	CursorRes       m_resCursor;   // 鼠标样式图片
	GifRes          m_resGif;      // 动画

	SkinRes*        m_pSkinRes;
};

} 