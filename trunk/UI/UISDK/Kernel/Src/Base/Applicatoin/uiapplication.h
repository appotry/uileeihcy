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
//	UI���󴴽���Ϣ
//
struct UIOBJ_CREATE_INFO 
{
	String      m_strXmlName;        // ��xml�е���������
    GUID        m_guid;              // ����clskd
	funcUICreateInstancePtr  m_func;   // ָ��ObjCreator<T>::UICreateInstance
    String      m_strCategory;       // ��uictrl/listctrl��ָ��༶��������UIEditor�е�ToolBox����ʾ
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
//  PS: 20130110 ���ǵ���UIBuilder�л��������UIApplicationʵ������˲����ٽ�
//      UIApplication��Ϊȫ�ֶ�����ʵ���ˡ�Ϊ�˱����Ժ��COM�������������Ϊ
//      �ӿ��࣬�ⲿֻ�ܷ��ʵ�IUIApplication
//  PS: 20130317 ���ǵ��Ժ����չ�������������ӿ����ɴ��麯�������ǲ���Pimplʵ��
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
    static ILog*     s_pLog;  // ���е�app����һ��log. log��������ȫ�ֶ��󣬷��򲻺�ʵ�������������дlog
    static long      s_lUiLogCookie;       // UIģ����־�ı�ʶ
    static long      s_lAppCount;  // ��appcount��Ϊ0ʱ���ͷ�log

protected:
    typedef vector<UIOBJ_CREATE_INFO*>               UIOBJ_CREATE_DATA;
    typedef vector<UIRENDERBASE_CREATE_INFO*>        UIRENDERBASE_CREATE_DATA;
    typedef vector<UITEXTRENDERBASE_CREATE_INFO*>    UITEXTRENDERBASE_CREATE_DATA;
    typedef vector<UILAYOUT_CREATE_INFO*>            UILAYOUT_CREATE_DATA;
    typedef map<String, funcUIParseSkinElement>      UISKINTAGPARSE_DATA;
    typedef map<String, funcUIParseLayoutElement>    UILAYOUTTAGPARSE_DATA;

    IUIApplication*     m_pUIApplication;     // �����ṩ�Ľӿ�
    OSVERSIONINFOEX     m_osvi;               // ����ϵͳ�汾
	bool                m_bDesignMode;        // �Ƿ������ģʽ
	IUIEditor*          m_pUIEditor;          // �ⲿ�ı༭��ָ�룬������Ϣ֪ͨ�����ݻ�ȡ

    list<IMessage*>           m_aliveUIObject;      // ����UI�����б������ڷ�����Ϣǰ�ж�һ��ָ������Ƿ���Ч
	UIOBJ_CREATE_DATA         m_vecUICreateData;    // ����UI�����XML�ַ��������ڴ��ַ�������UI����
    UIRENDERBASE_CREATE_DATA  m_vecUIRenderBaseCreateData;  // ���ڴ���RenderBase
    UITEXTRENDERBASE_CREATE_DATA  m_vecUITextRenderBaseCreateData;  // ���ڴ��ַ�������TextRenderBase
    UILAYOUT_CREATE_DATA      m_vecUILayoutCreateData;
    UISKINTAGPARSE_DATA       m_mapSkinTagParseData;
    UILAYOUTTAGPARSE_DATA     m_mapLayoutTagParseData;

    TopWindowManager    m_TopWindowMgr; 
    SkinManager*        m_pSkinMgr;      // ��Ҫ��ǰ�ͷţ�����ָ��ģʽ
    AnimateManager*     m_pAnimateMgr;   // ��Ҫ��ǰ�ͷţ�����ָ��ģʽ

    WaitForHandlesMgr   m_WaitForHandlesMgr;
    CForwardPostMessageWindow  m_WndForwardPostMsg; 
    MessageFilterMgr    m_MsgFilterMgr;
    ToolTipManager      m_ToolTipMgr;
};

}

#endif  // UIAPPLICATION_H_369FA8A6_A4A8_47a0_9E84_082B3AABA2A1