#ifndef IUIAPPLICATION_H_61E30001_8C5F_4e34_907B_5218915BC4C2
#define IUIAPPLICATION_H_61E30001_8C5F_4e34_907B_5218915BC4C2

namespace UI
{
class UIApplication;
interface ILayout;
interface IUIEditor;

interface UISDKAPI IUIApplication : IUnknown
{
    IUIApplication();
    virtual ~IUIApplication();
    UIApplication*  GetImpl();

    HRESULT  SetSkinDirection(const TCHAR* szDir);
    HRESULT  LoadSkin(const TCHAR* szSkinName);

    HRESULT  SetLog(ILog* pLog, const TCHAR* szLogXmlPath);
    HRESULT  GetLog(ILog** ppLog);

    HRESULT  SetDesignMode(VARIANT_BOOL b, IUIEditor* pUIBuilder);
    bool     IsDesignMode();
    IUIEditor*  GetUIEditorPtr();

    ISkinManager*       GetSkinMgr();
    ITopWindowManager*  GetTopWindowMgr();
    IAnimateManager*    GetAnimateMgr();
    HRESULT  GetWaitForHandlesMgr(IWaitForHandlesMgr** ppMgr);
    HRESULT  GetMessageFilterMgr(IMessageFilterMgr** ppMgr);

    ISkinRes*       GetActiveSkinRes();
    IImageManager*  GetActiveSkinImageMgr();
    IImageRes*      GetActiveSkinImageRes();
    ICursorRes*     GetActiveSkinCursorRes();
    IGifRes*        GetActiveSkinGifRes();
    IFontManager*   GetActiveSkinFontMgr();
    IFontRes*       GetActiveSkinFontRes();
    IColorManager*  GetActiveSkinColorMgr();
    IColorRes*      GetActiveSkinColorRes();
    IStyleManager*  GetActiveSkinStyleMgr();
    IStyleRes*      GetActiveSkinStyleRes();
    ILayoutManager* GetActiveSkinLayoutMgr();
    ILayoutRes*     GetActiveSkinLayoutRes();

    void  RestoreRegisterUIObject();
    HRESULT  RegisterLayoutTagParseFunc(const TCHAR* szTag, funcUIParseLayoutElement func);
    HRESULT  GetSkinTagParseFunc(const TCHAR* szTag, funcUIParseSkinElement* pFunc);
    HRESULT  GetLayoutTagParseFunc(const TCHAR* szTag, funcUIParseLayoutElement* pFunc);
    
    HRESULT  CreateInstanceByName(const TCHAR* szName, IObject** pOut);
    HRESULT  CreateInstanceByClsid(REFCLSID clsid, void** pOut);
    HRESULT  RegisterUIObjectCreateData(const TCHAR* szName, const TCHAR* szCategory, UINT nObjType, REFCLSID guid, funcUICreateInstancePtr pfunc);
    void     LoadUIObjectListToToolBox();

    HRESULT  RegisterUIRenderBaseCreateData(const TCHAR* szName, int nType, int nControlType, int nControlSubType, funcUICreateRenderBasePtr pfunc);
    HRESULT  CreateRenderBaseByName(const TCHAR* szName, IObject* pObject, IRenderBase** ppOut);
    HRESULT  CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut);

    HRESULT  RegisterUITextRenderBaseCreateData(const TCHAR* szName, int nType, int nControlType, int nControlSubType, funcUICreateTextRenderBasePtr pfunc);
    HRESULT  CreateTextRenderBaseByName(const TCHAR* szName, IObject* pObject, ITextRenderBase** ppOut);
    HRESULT  CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut);

    HRESULT  RegisterLayoutCreateData(const TCHAR* szName, int nType, funcUICreateLayoutPtr);
    HRESULT  CreateLayoutByName(const TCHAR* szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut);
    HRESULT  CreateLayout(int nType, IObject* pObject, ILayout** ppOut);

    BOOL     IsDialogMessage(MSG* pMsg);
    HRESULT  MsgHandleLoop(bool* pbQuitLoopRef=NULL);
    HRESULT  RedrawTopWindows();

    HRESULT  SetTooltipsUI(IToolTipUI* pUI);
    HRESULT  UseInnerTooltipsUI(const TCHAR* szWndID);
    HRESULT  ShowToolTip(TOOLTIPITEM* pItem);
    HRESULT  HideToolTip();
    UINT_PTR SetTimer(int nElapse, IMessage* pNotify);
    UINT_PTR SetTimerById(int nElapse, int nId, IMessage* pNotify);
    UINT_PTR SetTimer(int nElapse, TimerItem* pTimerItem);
    void     KillTimer(UINT_PTR& nTimerID);
    void     KillTimerById(int nId, IMessage* pNotify);

    HDC      GetCacheDC();
    void     ReleaseCacheDC(HDC hDC);

    bool     IsUnderXpOS();
    bool     IsVistaOrWin7etc();

    bool     IsUIObjectAvailable(IMessage* p);
    HRESULT  AddUIObject(IMessage* p);
    HRESULT  RemoveUIObject(IMessage* p);

    HWND     GetForwardPostMessageWnd();

private:
    UIApplication*  m_pImpl;
};

}

#endif