#pragma once
#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Inc\Interface\iskindatasource.h"

namespace UI
{
class UIApplication;
class SkinManager;
class ImageManager;
class ColorManager; 
class FontManager;  
class StyleManager;
class LayoutManager;
class Object;
interface  IUIDocument;
interface  ISkinDataSource;

enum SKIN_RES_LOAD_STATE
{
    SKIN_RES_LOAD_STATE_NONE,     // 仅new SkinRes
//    SKIN_RES_LOAD_STATE_
    SKIN_RES_LOAD_STATE_LOADED,   // 已加载
    
};

// 皮肤配置文件信息
struct  UISkinResDocInfo
{
    IUIDocument*  m_pDoc;   // doc指针
    String  m_strDocPath;   // doc路径
};

//
//	一款皮肤资源管理
//	
class SkinRes
{
public:
    SkinRes();
	~SkinRes();

    ISkinRes*  GetISkinRes();

    IUIApplication*  GetUIApplication();
	ISkinManager*    GetISkinManager();
    IImageManager*   GetIImageManager();
    IColorManager*   GetIColorManager();
    IFontManager*    GetIFontManager();
    IStyleManager*   GetIStyleManager();
    ILayoutManager*  GetILayoutManager();
    IImageRes*       GetIImageRes();
    IFontRes*        GetIFontRes();
    IColorRes*       GetIColorRes();
    IStyleRes*       GetIStyleRes();

    UINT  GetXmlDocCount();
    const TCHAR*  GetXmlDoc(UINT nIndex, IUIDocument** pp);

    const TCHAR*  GetName();
    const TCHAR*  GetPath();
    void  OnNewUIDocument(IUIDocument* pDoc, const TCHAR* szPath);

    bool  Load();

public:
    void  SetParam(SKIN_PACKET_TYPE eType, const TCHAR* szName, const TCHAR* szPath);
    ISkinDataSource*  GetDataSource() { return m_pDataSource; }

	HRESULT  SetSkinMgr(SkinManager* p);
	SkinManager* GetSkinMgr() { return m_pSkinMgr; } //  内部调用

	ImageManager*    GetImageManager()    { return m_pImageMgr;  }
	ColorManager*    GetColorManager()    { return m_pColorMgr;  }
	FontManager*     GetFontManager()     { return m_pFontMgr;   }
	StyleManager*    GetStyleManager()    { return m_pStyleMgr;  }
	LayoutManager*   GetLayoutManager()   { return m_pLayoutMgr; }
    SKIN_HLS_INFO*   GetHLSInfo()         { return m_pHLSInfo; }

	bool  SetImageManager(ImageManager* pImageManager);
	bool  SetColorManager(ColorManager* pColorManager);
	bool  SetFontManager(FontManager* pFontManager);
	bool  SetStyleManager(StyleManager* pStyleManager);
	bool  SetLayoutManager(LayoutManager* pLayoutManager);

    bool  Unload();
	bool  Save();
	bool  ReloadResource(UI_RESOURCE_TYPE eResType);

	bool  ReLoadLayout(Object* pRootObj, list<Object*>& listAllChild);
	bool  ChangeSkinHLS(short h, short l, short s, int nFlag);
	
	// 编辑器专用函数
	bool  LoadAllWindowNodeInfo(LayoutWindowNodeInfo* pInfoArray, int* pArraySize);

	int   GetImageCount();
	IImageResItem*  GetImageItemInfo(int nIndex);
	int   GetColorCount();
	IColorResItem*  GetColorItemInfo(int nIndex);
	int   GetFontCount();
	IFontResItem*  GetFontItemInfo(int nIndex);
	int   GetStyleCount();

private:
    bool  _Load_Dir(const TCHAR*  szXml);
	bool  _Load_Zip(const TCHAR*  szZip);

private:
    ISkinRes*  m_pISkinRes;

    SKIN_RES_LOAD_STATE  m_eLoadState;

//  SKIN_PACKET_TYPE  m_ePacketType;
//  String   m_strName;
//  String   m_strPath;
    ISkinDataSource*  m_pDataSource;

	ImageManager*   m_pImageMgr;
	ColorManager*   m_pColorMgr;
	FontManager*    m_pFontMgr;
	StyleManager*   m_pStyleMgr;
	LayoutManager*  m_pLayoutMgr;

	// 指针保存
	SkinManager*    m_pSkinMgr;
   
    SKIN_HLS_INFO*  m_pHLSInfo;

    typedef vector<UISkinResDocInfo*> _DocList;
    _DocList  m_listDoc;   // 各个xml文档指针
};

}