#pragma once

// IUIApplication接口
namespace UI
{
	interface  IToolTipUI;
	interface  IWindowBase;
	interface  ISkinManager;
	interface  ISkinRes;
	struct     TOOLTIPITEM;
    interface  IUICursor;
	interface  IRenderBitmap;
	interface  IRenderFont;
	interface  IImageManager;
	interface  IImageRes;
	interface  IFontManager;
	interface  IFontRes;
	interface  IColorManager;
	interface  IColorRes;
	interface  IStyleManager;
	interface  IStyleRes;
	interface  ILayoutManager;
	interface  ILayoutRes;
	interface  ICursorRes;
	interface  IGifRes;
	interface  ISkinConfigRes;
	interface  IImageResItem;
	interface  IColorResItem;
	interface  IFontResItem;
	interface  ISkinConfigManager;
	interface  ITopWindowManager;
	interface  IAnimateManager;
	interface  IWaitForHandlesMgr;
	class      IRenderTarget;
	interface  ITrayIcon;
	interface  IMessageFilterMgr;
    interface  IMapAttribute;
    interface  IObject;
    interface  IUIDocument;

	// 句柄定义
	interface IUIApplication;

    class SkinManager;
	interface UISDKAPI ISkinManager : public IRootInterface
	{
        ISkinManager(SkinManager*);
        SkinManager*  GetImpl();

		IUIApplication*  GetUIApplication();
		UINT  GetSkinCount();
        void  GetSkinDirection(TCHAR*  szOut);
        ISkinRes*  AddSkin(const TCHAR*  szName);

        HRESULT  ChangeSkin(ISkinRes*  pSkinRes, bool bSync=true); 
		HRESULT  ChangeSkinHLS(short h, short l, short s, int nFlag);

		HRESULT  SetActiveSkin(ISkinRes* pSkin);
		ISkinRes*  GetActiveSkin();
        ISkinRes*  GetSkinResByIndex(long lIndex);

		bool  Save(ISkinRes* pSkinRes);

    private:
        SkinManager*  m_pImpl;
	};

    struct LayoutWindowNodeInfo  // layout.xml中一个结点的信息，用于UI编辑器显示
    {
        TCHAR szNodeName[MAX_STRING_LEN]; // 如CustomWindow, Window, Menu
        TCHAR szNodeID[MAX_STRING_LEN];   // 结点ID，用于创建
    };
    
    interface ISkinDataSource;
    class SkinRes;
    // 反向Pimpl模式。由SkinRes来创建ISkinRes
    interface UISDKAPI ISkinRes : public IRootInterface
    {
        ISkinRes(SkinRes*);
        SkinRes*  GetImpl();
        
        IUIApplication*  GetUIApplication();
        ISkinManager*    GetSkinManager();
        IImageManager*   GetImageManager();
        IColorManager*   GetColorManager();
        IFontManager*    GetFontManager();
        IStyleManager*   GetStyleManager();
        ILayoutManager*  GetLayoutManager();
        IImageRes*       GetImageRes();
        IFontRes*        GetFontRes();
        IColorRes*       GetColorRes();
        IStyleRes*       GetStyleRes();
    
        ISkinDataSource*  GetDataSource();

        UINT  GetXmlDocCount();
        const TCHAR*  GetXmlDoc(UINT nIndex, IUIDocument** pp);

        const TCHAR*  GetName();
        const TCHAR*  GetPath();
        void  OnNewUIDocument(IUIDocument* pDoc, const TCHAR* szPath);

        bool  Load();
        bool  ChangeSkinHLS(short h, short l, short s, int nFlag);
        bool  LoadAllWindowNodeInfo(LayoutWindowNodeInfo* pInfoArray, int* pArraySize);

    private:
        SkinRes*  m_pImpl;
    };

    class TopWindowManager;
    interface UISDKAPI ITopWindowManager : public IRootInterface
    {
        ITopWindowManager(TopWindowManager* p);
        TopWindowManager* GetImpl();

        HRESULT  AddTopWindowObject(IWindowBase*);
        HRESULT  RemoveTopWindowObject(IWindowBase*);
        void     ChangeSkin(ISkinRes* pNewSkinRes);

        bool  UpdateAllWindow();
        void  SendMessage2AllWnd(UIMSG*  pMsg);
        void  PostMessage2AllWnd(UIMSG* pMsg);
        void  ForwardMessage2AllObj(UIMSG*  pMsg);

    private:
        TopWindowManager*  m_pImpl;
    };

    class ColorManager;
    interface UISDKAPI IColorManager : public IRootInterface
    {
        IColorManager(ColorManager* p);
        ColorManager*  GetImpl();

        IColorRes*  GetColorRes();

        HRESULT  InsertColorItem(const TCHAR* szID, const TCHAR* szColor);
        HRESULT  ModifyColorItem(const TCHAR* szID, const TCHAR* szColor);
        HRESULT  RemoveColorItem(const TCHAR* szID);
    private:
        ColorManager*  m_pImpl;
    };

    class ImageManager;
    interface UISDKAPI IImageManager : public IRootInterface	
    {
        IImageManager(ImageManager* p);
        ImageManager*  GetImpl();

        IImageRes*   GetImageRes();
        ICursorRes*  GetCursorRes();
        IGifRes*     GetGifRes();

        IImageResItem*  InsertImageItem(const TCHAR* szID, const TCHAR* szPath);
        HRESULT  ModifyImageItem(const TCHAR* szID, const TCHAR* szPath);
        HRESULT  RemoveImageItem(const TCHAR* szID);

        HRESULT  ModifyImageItemInRunTime(const TCHAR* szID, const TCHAR* szPath);
        HRESULT  ModifyImageItemAlpha(const TCHAR* szID, int nAlphaPercent);
    private:
        ImageManager*  m_pImpl;
    };

    class FontManager;
    interface UISDKAPI IFontManager : public IRootInterface 
    {
        IFontManager(FontManager* p);
        FontManager*  GetImpl();

        IFontRes*  GetFontRes();
        HRESULT  InsertFontItem(const TCHAR* szID, LOGFONT* pLogFont);
        HRESULT  ModifyFontItem(const TCHAR* szID, LOGFONT* pLogFont);
        HRESULT  RemoveFontItem(const TCHAR* szID);
    private:
        FontManager*  m_pImpl;
    };

    class StyleManager;
    interface UISDKAPI IStyleManager : public IRootInterface
    {
        IStyleManager(StyleManager*);
        StyleManager*  GetImpl();

        IStyleRes*  GetStyleRes();

        HRESULT  InsertStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szInherit);
        HRESULT  ModifyStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szInherit);
        HRESULT  RemoveStyleItem(STYLE_SELECTOR_TYPE type, const TCHAR* szID);

        HRESULT  InsertStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey, const TCHAR* szValue);
        HRESULT  ModifyStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey, const TCHAR* szValue);
        HRESULT  RemoveStyleAttribute(STYLE_SELECTOR_TYPE type, const TCHAR* szID, const TCHAR* szKey);

        HRESULT  LoadStyle(const TCHAR* szTagName, const TCHAR* szStyleClass, const TCHAR* szID, IMapAttribute* pMapStyle);
        HRESULT  ParseStyle(const TCHAR* szObjName, IMapAttribute* pMapAttrib);
    private:
        StyleManager*  m_pImpl;
    };

    class LayoutManager;
    interface UISDKAPI ILayoutManager : public IRootInterface
    {
        ILayoutManager(LayoutManager*);
        LayoutManager*  GetImpl();

        ILayoutRes*  GetLayoutRes();
        HRESULT  LoadRootLayout(IObject* pRootObj, const TCHAR* szId);
        HRESULT  LoadRootLayout(const TCHAR* szObjName, const TCHAR* szId, IObject** ppObj);
        HRESULT  LoadLayout(IUIElement* pUIElement, IObject* pParent, IObject** ppObj);
        HRESULT  LoadObjAttrAndChild(IObject* pRootObj, IUIElement* pUIElement);

    private:
        LayoutManager*  m_pImpl;
    };

    class ImageRes;
    interface UISDKAPI IImageRes : public IRootInterface
    {
        IImageRes(ImageRes* p);
        ImageRes*  GetImpl();

        long  GetImageCount();
        IImageResItem*  GetImageResItem(long lIndex);
        IImageResItem*  GetImageResItem(const TCHAR* szId);
        bool  ModifyImage(const TCHAR* szId, const TCHAR* szPath);
        HRESULT  GetBitmap(const TCHAR* szImageId, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderBitmap** ppRenderBitmap);
        HBITMAP  LoadBitmap(const TCHAR* szId);

    private:
        ImageRes*  m_pImpl;
    };


    class ImageResItem;
    interface UISDKAPI IImageResItem : public IRootInterface
     {
        IImageResItem(ImageResItem* p);
        ImageResItem*  GetImpl(); 

        const TCHAR*  GetID();
        const TCHAR*  GetPath();
        bool  GetUseSkinHLS();
        bool  GetNeedAntiAliasing();
        void  SetUseSkinHLS(bool b);
        void  SetNeedAntiAliasing(bool b);
        bool  ModifyImage(const TCHAR*  szPath);

    private:
        ImageResItem*  m_pImpl;
    };

    class CursorRes;
    interface UISDKAPI ICursorRes : public IRootInterface
    {
        ICursorRes(CursorRes* p);
        CursorRes*  GetImpl(); 

        void  GetCursor(const TCHAR*  szCursorID, IUICursor** pp);
    private:
        CursorRes*  m_pImpl;
    };

    class CursorResItem;
    interface UISDKAPI ICursorResItem : public IRootInterface
    {
        ICursorResItem(CursorResItem* p);
        CursorResItem*  GetImpl(); 

    private:
        CursorResItem*  m_pImpl;
    };

    class ColorRes;
    interface UISDKAPI IColorRes : public IRootInterface
    {
        IColorRes(ColorRes* p);
        ColorRes*  GetImpl();

        long  GetColorCount();
        IColorResItem*  GetColorResItem(long lIndex);
        void  GetColor(const TCHAR* szColorId, Color** pp);

    private:
        ColorRes*  m_pImpl;
    };

    class  ColorResItem;
    interface UISDKAPI IColorResItem : public IRootInterface
    {
        IColorResItem(ColorResItem* p);
        ColorResItem*  GetImpl();

        const TCHAR* GetID();
        const TCHAR* GetColorString();
    private:
        ColorResItem*  m_pImpl;
    };

    class FontResItem;
    interface UISDKAPI IFontResItem : public IRootInterface
    {
        IFontResItem(FontResItem*);
        FontResItem*  GetImpl();

        const TCHAR* GetID();
        LOGFONT*   GetLogFont();
    private:
        FontResItem*  m_pImpl;
    };

    class GifRes;
    class IGifImage;
    interface UISDKAPI IGifRes : public IRootInterface
    {
        IGifRes(GifRes* p);
        GifRes*  GetImpl();

        HRESULT  GetGifImage(const TCHAR*  szId, IGifImage** ppGifImage);
    private:
        GifRes*  m_pImpl;
    };

    class GifResItem;
    interface UISDKAPI IGifResItem : public IRootInterface
    {
        IGifResItem(GifResItem* p);
        GifResItem*  GetImpl();

    private:
        GifResItem*  m_pImpl;
    };

    class FontRes;
    interface UISDKAPI IFontRes : public IRootInterface
    {
        IFontRes(FontRes* p);
        FontRes*  GetImpl();

        long     GetFontCount();
        HRESULT  GetFontResItem(long lIndex, IFontResItem** ppResItem);
        HRESULT  GetFont(BSTR bstrFontID, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppOut);
        HRESULT  GetDefaultFont(GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont);
        HRESULT  GetFontEx(IRenderFont* pFont, WPARAM w, LPARAM l, GRAPHICS_RENDER_LIBRARY_TYPE eRenderType, __out IRenderFont** ppFont);
    private:
        FontRes*  m_pImpl;
    };

    class StyleResItem;
    interface UISDKAPI IStyleResItem : public IRootInterface
    {
        IStyleResItem(StyleResItem* p);
        StyleResItem*  GetImpl();

        STYLE_SELECTOR_TYPE GetSelectorType();
        const TCHAR* GetID();
        const TCHAR* GetInherits();

        void  GetAttributeMap(IMapAttribute** ppMapAttrib);
    private:
        StyleResItem*  m_pImpl;
    };

    class StyleRes;
    interface UISDKAPI IStyleRes : public IRootInterface
    {
        IStyleRes(StyleRes* p);
        StyleRes*  GetImpl();

        long     GetStyleCount();
        IStyleResItem*  GetStyleResItem(long lIndex);
        IStyleResItem*  GetStyleResItem2(STYLE_SELECTOR_TYPE type, const TCHAR* szId);
    private:
        StyleRes*  m_pImpl;
    };

#if 0
    class LayoutResItem;
    interface UISDKAPI ILayoutResItem : public IRootInterface
    {
        ILayoutResItem(LayoutResItem* p);
        LayoutResItem*  GetImpl();

        HRESULT  GetSelfAttr(IMapAttribute** ppMapAttribute);
        HRESULT  GetStyleAttr(IMapAttribute** ppMapAttribute);
        HRESULT  GetLastAttr(IMapAttribute** ppMapAttribute);
    private:
        LayoutResItem*  m_pImpl;
    };
#endif

    class LayoutRes;
    interface UISDKAPI ILayoutRes : public IRootInterface
    {
        ILayoutRes(LayoutRes* p);
        LayoutRes*  GetImpl();

    private:
        LayoutRes*  m_pImpl;
    };

    //////////////////////////////////////////////////////////////////////////

	//
	// 各种提示条绘制的抽象类	
	//
	interface IToolTipUI
	{
	public:
		virtual ~IToolTipUI() = 0 {};
		virtual bool  Create()  = 0;
		virtual bool  Destroy() = 0;
		virtual bool  SetText(const TCHAR* szText) = 0;
		virtual bool  SetTitle(const TCHAR* szText) = 0;
		virtual bool  Show() = 0;
		virtual bool  Hide() = 0;
		virtual void  SetAttribute(const ATTRMAP& mapAttrib) = 0;
		virtual bool  SetUIApplication(IUIApplication* p) = 0;
	};

	interface IWaitForHandleCallback
	{
		virtual void OnWaitForHandleObjectCallback(HANDLE, LPARAM) = 0;
	};
	interface IWaitForHandlesMgr : public IRootInterface
	{
		virtual bool  AddHandle(HANDLE h, IWaitForHandleCallback* pCB, LPARAM l) = 0;
		virtual bool  RemoveHandle(HANDLE h) = 0;
	};

	interface IFlashInvalidateListener
	{
		virtual void OnInvalidateRect(LPCRECT, BOOL) = 0;
	};

	interface IPreTranslateMessage  // IMessageFilter 被系统其它地方已经定义过了
	{
	public:
		virtual BOOL PreTranslateMessage(MSG* pMsg) = 0;
	};

	interface IMessageFilterMgr : public IRootInterface
	{
		virtual void  AddMessageFilter(IPreTranslateMessage* p) = 0;
		virtual void  RemoveMessageFilter(IPreTranslateMessage* p) = 0;
	};

    //
    // 用于监听每次窗口调用CommitDoubleBuffet2Window(里面的bitblt)时，向该接口对象发送pre/post通知
    // 原本是用于实现 Caret/DrawFocusRect这种会直接画在窗口上面，又是采用异或方向绘制的对象，在提
    // 交之前选隐藏，在提前之后再显示一次，避免提交数据导致的顺序错乱
    //
    // 以后可用于扩展实时获取数据窗口变化的接口
    //
    interface ICommitWindowBufferListener
    {
        virtual void  PreCommitWindowBuffer(HDC hDC, HDC hMemDC, IWindowBase* pWindow, RECT* prcCommit, int nRectCount) = 0;
        virtual void  PostCommitWindowBuffer(HDC hDC, HDC hMemDC, IWindowBase* pWindow, RECT* prcCommit, int nRectCount) = 0;

        // 用于标明该listener只一次有效。在检测完一次后就将该listener删除
        virtual bool  OnlyListenOnce() = 0; 
    };
}

