#ifndef UIAPPLICATION_H_369FA8A6_A4A8_47a0_9E84_082B3AABA2A1
#define UIAPPLICATION_H_369FA8A6_A4A8_47a0_9E84_082B3AABA2A1

#include "UISDK\Kernel\Src\Animate\animatemgr.h"
#include "UISDK\Kernel\Src\Helper\topwindow\topwindowmanager.h"
#include "UISDK\Kernel\Src\Helper\tooltip\tooltipmanager.h"
#include "UISDK\Kernel\Src\Helper\msg\msghelper.h"
#include "UISDK\Kernel\Src\Resource\skinmanager.h"
#include "UISDK\Kernel\Src\Helper\MouseKeyboard\keyboardmanager.h"
#include "UISDK\Kernel\Src\Helper\MouseKeyboard\mousemanager.h"

namespace UI
{

//
//	UI对象创建信息
//
struct UIOBJ_CREATE_INFO 
{
	String      m_strXmlName;        // 在xml中的配置名称
    GUID        m_guid;              // 对象clskd
	funcUICreateInstancePtr  m_func;   // 指向ObjCreator<T>::UICreateInstance
    String      m_strCategory;       // 如uictrl/listctrl，指针多级。用于在UIEditor中的ToolBox中显示
    UINT        m_nObjType;
};

struct UIRENDERBASE_CREATE_INFO
{
    String  m_strName;
    UINT    m_nControlType;
    UINT    m_nControlSubType;
    int     m_nRenderType;
    funcUICreateRenderBasePtr m_func;
};

struct UITEXTRENDERBASE_CREATE_INFO
{
    String     m_strName;
    UINT       m_nControlType;
    UINT       m_nControlSubType;
    int        m_nRenderType;
    funcUICreateTextRenderBasePtr m_func;
};

struct UILAYOUT_CREATE_INFO
{
    String  m_strName;
    int     m_nLayoutType;
    GUID    m_guid;
    funcUICreateLayoutPtr  m_func;
};

//
//  PS: 20130110 考虑到在UIBuilder中会存在两份UIApplication实例，因此不能再将
//      UIApplication作为全局对象来实现了。为了便于以后的COM化，在这里改造为
//      接口类，外部只能访问到IUIApplication
//  PS: 20130317 考虑到以后的扩展和升级，不将接口做成纯虚函数，而是采用Pimpl实现
//
class UIApplication
{
public:
	UIApplication(IUIApplication* p);
	~UIApplication(void);

	// ---------- 
    HRESULT  SetSkinDirection(const TCHAR* szDir);
    HRESULT  LoadSkin(const TCHAR* szSkinName);

	HRESULT  SetLog(ILog* pLog, const TCHAR* szLogXmlPath);
	HRESULT  GetLog(ILog** ppLog);

	HRESULT  SetDesignMode(VARIANT_BOOL b, IUIEditor* pUIBuilder);
	bool     IsDesignMode() { return m_bDesignMode; }
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
	
	HRESULT  SetTooltipsUI(IToolTipUI* pUI);
	HRESULT  UseInnerTooltipsUI(const TCHAR* szWndID);
	HRESULT  ShowToolTip(TOOLTIPITEM* pItem);
	HRESULT  HideToolTip();

    void     RestoreRegisterUIObject();
    HRESULT  RegisterLayoutTagParseFunc(const TCHAR* szTag, funcUIParseLayoutElement func);
    HRESULT  GetSkinTagParseFunc(const TCHAR* szTag, funcUIParseSkinElement* pFunc);
    HRESULT  GetLayoutTagParseFunc(const TCHAR* szTag, funcUIParseLayoutElement* pFunc);

    HRESULT  RegisterUIObjectCreateData(const TCHAR* szXmlName, const TCHAR* szCategory, UINT nObjType, REFGUID guid, funcUICreateInstancePtr pfun);
	HRESULT  CreateInstanceByName(const TCHAR* szXmlName, IObject** pOut);
    HRESULT  CreateInstanceByClsid(REFCLSID clsid, void** pOut);
    void     LoadUIObjectListToToolBox();

    HRESULT  RegisterUIRenderBaseCreateData(const TCHAR* szName, int nType, int nControlType, int nControlSubType, funcUICreateRenderBasePtr pfunc);
    HRESULT  CreateRenderBaseByName(const TCHAR* szName, IObject* pObject, IRenderBase** ppOut);
    HRESULT  CreateRenderBase(int nType, IObject* pObject, IRenderBase** ppOut);

    HRESULT  RegisterUITextRenderBaseCreateData(const TCHAR* szName, int nType, int nControlType, int nControlSubType, funcUICreateTextRenderBasePtr pfunc);
    HRESULT  CreateTextRenderBaseByName(const TCHAR* szName, IObject* pObject, ITextRenderBase** ppOut);
    HRESULT  CreateTextRenderBase(int nType, IObject* pObject, ITextRenderBase** ppOut);
    
    HRESULT  RegisterLayoutCreateData(const TCHAR* szName, int nType, funcUICreateLayoutPtr);
    HRESULT  CreateLayout(int nType, IObject* pObject, ILayout** ppOut);
    HRESULT  CreateLayoutByName(const TCHAR* szName, IObject* pObject, bool bCreateDefault, ILayout** ppOut);

	BOOL     IsDialogMessage(MSG* pMsg);
	HRESULT  MsgHandleLoop(bool* pbQuitLoopRef);

	HRESULT  RedrawTopWindows();

	bool     IsUnderXpOS();
	bool     IsVistaOrWin7etc();

	bool     IsUIObjectAvailable(IMessage* p);
	HRESULT  AddUIObject(IMessage* p);
	HRESULT  RemoveUIObject(IMessage* p);

	HWND     GetForwardPostMessageWnd();

public:
	bool     GetUICreateInstanceFuncPtr(const TCHAR* szXmlName, funcUICreateInstancePtr* pOut);
    bool     GetUICreateInstanceFuncPtr(REFCLSID guid, funcUICreateInstancePtr* pOut);

    IUIApplication*  GetIUIApplication() { return m_pUIApplication; }

private:
    void     RegisterWndClass();
    HRESULT  InitLog(const TCHAR* szLogConfigFilePath);
    void     RegisterDefaultUIObject();
    void     ClearRegisterUIObject();

public:
    static ILog*     s_pLog;  // 所有的app共享一个log. log必须作成全局对象，否则不好实现在任意代码中写log
    static long      s_lUiLogCookie;       // UI模块日志的标识
    static long      s_lAppCount;  // 当appcount减为0时，释放log

protected:
    typedef vector<UIOBJ_CREATE_INFO*>               UIOBJ_CREATE_DATA;
    typedef vector<UIRENDERBASE_CREATE_INFO*>        UIRENDERBASE_CREATE_DATA;
    typedef vector<UITEXTRENDERBASE_CREATE_INFO*>    UITEXTRENDERBASE_CREATE_DATA;
    typedef vector<UILAYOUT_CREATE_INFO*>            UILAYOUT_CREATE_DATA;
    typedef map<String, funcUIParseSkinElement>      UISKINTAGPARSE_DATA;
    typedef map<String, funcUIParseLayoutElement>    UILAYOUTTAGPARSE_DATA;

    IUIApplication*     m_pUIApplication;     // 对外提供的接口
    OSVERSIONINFOEX     m_osvi;               // 操作系统版本
	bool                m_bDesignMode;        // 是否是设计模式
	IUIEditor*          m_pUIEditor;          // 外部的编辑器指针，用于消息通知和数据获取

    list<IMessage*>           m_aliveUIObject;      // 所有UI对象列表，用于在发送消息前判断一个指针对象是否有效
	UIOBJ_CREATE_DATA         m_vecUICreateData;    // 保存UI对象的XML字符串，用于从字符串创建UI对象
    UIRENDERBASE_CREATE_DATA  m_vecUIRenderBaseCreateData;  // 用于创建RenderBase
    UITEXTRENDERBASE_CREATE_DATA  m_vecUITextRenderBaseCreateData;  // 用于从字符串创建TextRenderBase
    UILAYOUT_CREATE_DATA      m_vecUILayoutCreateData;
    UISKINTAGPARSE_DATA       m_mapSkinTagParseData;
    UILAYOUTTAGPARSE_DATA     m_mapLayoutTagParseData;

    TopWindowManager    m_TopWindowMgr; 
    SkinManager*        m_pSkinMgr;      // 需要提前释放，做成指针模式
    AnimateManager*     m_pAnimateMgr;   // 需要提前释放，做成指针模式

    WaitForHandlesMgr   m_WaitForHandlesMgr;
    CForwardPostMessageWindow  m_WndForwardPostMsg; 
    MessageFilterMgr    m_MsgFilterMgr;
    ToolTipManager      m_ToolTipMgr;
};

}

#endif  // UIAPPLICATION_H_369FA8A6_A4A8_47a0_9E84_082B3AABA2A1