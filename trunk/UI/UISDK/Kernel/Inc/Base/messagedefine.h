#ifndef MESSAGEDEFINE_H_1F530C58_DD7E_4a28_9A4E_97613B69A20E
#define MESSAGEDEFINE_H_1F530C58_DD7E_4a28_9A4E_97613B69A20E

#include "UISDK\Kernel\Inc\Util\atlcrack.h"
#include <atltypes.h>
#define ATLTYPES_INLINE inline
#include <atltypes.inl>

//
//	����ϵͳ�ؼ���WM_COMMAND��UI�ؼ���WM_COMAAND����Ϣ�����ǲ�һ�µ�
//  Ϊ���Է����߻�����������±��������������Ϣ��code���������¶���
//

enum
{
    UI_WM_NOTIFY = WM_USER+WM_USER,

    //
    //	��ui��ʵ��post message���Ժ�����Ӧ��
    //	��CForwardPostMessageWindow
    //
    UI_WM_POSTMESSAGE,

#pragma region // UI
    //
    //	������ʾ��֪ͨ
    //		message:UI_WM_SHOW_TOOLTIP
    //		code:   0
    //		wparam:  
    //		lParam: 
    //		pMsgFrom: pWindow
    //
    //	������󲻴��������Ϣ�򷵻�FALSE���򽫵���Ĭ�ϵ���ʾ������ʾtooltip�������ݡ�
    //
    UI_WM_SHOW_TOOLTIP,

    //
    //	��itemdata��Ϊ��ʱ����ؼ���ȡitem����ʾ��Ϣ
    //		message:UI_WM_GET_TOOLTIPINFO
    //		code:   
    //		wparam:  TOOLTIPITEM*
    //		lParam:  IToolTipUI*
    //		pMsgFrom: 
    //
    //	����0��ʾʧ�ܣ�����ʾ��ʾ��������1��������ʾ��ʾ��
    //
    UI_WM_GET_TOOLTIPINFO,

    //
    //   ������ϢID
    //
    UI_WM_TRAYICON,

    //
    //  ����ʱ����Ӧ
    //     message:  UI_WM_ANIMATE_TICK
    //     code:     NA
    //     wParam:   nCount
    //     lparam:   ITimeline** ppArray
    //
    UI_WM_ANIMATE_TICK,

    //
    //  �ö���Ķ���timelineȫ��������
    //     message:  UI_WM_ANIMATE_OVER
    //     code:     NA
    //     wParam:   
    //     lparam:   
    //
    UI_WM_ANIMATE_OVER,

    //
    //	������λ�øı�֪ͨ
    //
    //		message:UI_WM_HSCROLL/UI_WM_VSCROLL
    //		code:   0
    //		wparam: Pos Contains the current scroll-box position if the scroll-bar code is SB_THUMBPOSITION or SB_THUMBTRACK; otherwise not used.
    //		lParam: scroll type, SB_LEFT SB_RIGHT SB_THUMBTRACK SB_ENDSCROLL ...
    //		pMsgFrom: this
    //
    UI_WM_HSCROLL,
    UI_WM_VSCROLL,

    //
    //	��ȡ��ǰ���Ƶ�ƫ����������OnDraw
    //
    //		message: UI_WM_GETSCROLLOFFSET
    //		code:
    //		wparam:  [out] int* xOffset
    //		lparam:  [out] int* yOffset 
    //
    //	Return
    //
    UI_WM_GETSCROLLOFFSET,

    //
    //	��ȡ��ǰ������Χ
    //
    //		message: UI_WM_GETSCROLLRANGE
    //		code:
    //		wparam:  [out] int* xRange
    //		lparam:  [out] int* yRange
    //
    //	Return
    //
    UI_WM_GETSCROLLRANGE,

    //
    //	����һ��������ָ���Ķ��������λ��
    //
    //		message: UI_WM_HITTEST
    //		code:
    //		wparam:  x (in window)
    //		lparam:  y (in window)
    //		pMsgFrom: 
    //
    //	Return
    //
    //		HTNOWHERE��ʾ���ڸö�������
    //
    UI_WM_HITTEST,

    //
    //	��ȡһ�����ڵĻ�������
    //
    //		message:  UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE
    //		code:
    //		wparam:
    //		lparam:
    //
    //	Return
    //		
    //		RENDER_TYPE_GDI,RENDER_TYPE_GDIPLUS����RENDER_TYPE_AUTO;
    //
    UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE,

    //
    //
    //
    UI_WM_GETRENDERFONT,

    //
    //	�����xml�м�����ϣ������ԡ��Ӷ��󡢸�����Ҳ�������
    //
    //		message: UI_WM_OBJECTLOADED
    //		code:
    //		wparam:  
    //		lparam:  
    //
    UI_WM_OBJECTLOADED,


    //
    //	MouseManager֪ͨһ���������ĵ�ǰ״̬�����ı�(hover,press...)
    //  ר������ˢ�´�����Ҫ�������Ϣ����������߼�������Ϊ���ܻᱻHOOK��������״̬����ȷ
    //
    //		message:  UI_WM_STATECHANGED
    //		code:
    //		wparam:   old state bits
    //		lparam:   new state bits
    //
    //	Return
    //
//    UI_WM_STATECHANGED,

    // wParam: nChangeMask
    UI_WM_STATECHANGED2, // ������ȡ��UI_WM_STATECHANGED

    //  
    //	��UI_WM_STATECHANGED֮ǰ�����ͣ����������һЩ�߼��������ˢ�·���
    //  UI_WM_STATECHANGED��Ϣ�С���������Combobox�еİ�ť�����Ӱ�ť���Ƶ�Combobox��,
    //  Combobox������STATECHANGED������ֹ��ťˢ�£�
    //
    UI_WM_STATECHANGING,

    // �����Ƿ��Ƿֲ㴰�ڸı���
    //		message: UI_WM_WINDOWLAYEREDCHANGED
    //		code:
    //		wparam:  bool bLayered
    //		lparam:   
    UI_WM_WINDOWLAYEREDCHANGED,

    //
    //  ����������֧�ֲ��֣����ز��ֶ���ָ�롣��OBJ_WINDOW,OBJ_PANEL,OBJ_COMPOUND_CONTROL
    //
    //		message: UI_WM_GETLAYOUT
    //		code:
    //		wparam: 
    //		lparam
    //
    //	Return:  ILayout*
    //
    UI_WM_GETLAYOUT,

    //  redraw object��ˢ���Լ���һ���Ǳ�����ģ��invalidateRect���ӳ�ˢ�£���uipostmessage����
    //
    //		message: UI_WM_REDRAWOBJECT
    //		code:
    //		wparam: NA.
    //		lparam: NA.
    //
    UI_WM_REDRAWOBJECT,

    //	�桢�������еĿɼ�״̬�����˸ı䣨��Ҫ�������ڸ���������ʱ�����Զ���HwndObj����Ҳ���أ�
    //		message: UI_WM_PARENT_VISIBLE_CHANGED
    //		code:    
    //		wparam:  BOOL bVisible, (������WM_SHOWWINDOWһ��)
    //		lparam:  Object*
    UI_WM_VISIBLE_CHANGED,

    //
    //  ��UI��Ϣ������::SendMessage����
    //  ����һ��ͬ���ƶ������¼�(��ӡ��޸ġ�ɾ��)
    //		wparam:  SYNC_WINDOW_EVENT_TYPE
    //		lparam:  ����鿴SYNC_WINDOW_EVENT_TYPE����
    UI_WM_SYNC_WINDOW,

    //
    //  ��UI��Ϣ������::SendMessage����
    //	���������ƶ�ʱ����SYNC_CUMTOM���͵Ĵ��ڷ���һ����Ϣȥͬ���Լ���λ��
    //		wparam:  HDWP
    //		lparam:  RECT* 
    //  return:  HWDP
    UI_WM_SYNC_WINDOWPOSCHANING,

    //
    //  ��������Ҫ��������non client region(������padding,border)����ʱ����
    //  non client child object���͸���Ϣ
    //       message: UI_WM_CALC_PARENT_NONCLIENTRECT
    //       wparam:  RECT*  prcNonClient.  ������������Ҳ�ʱ��prcNonClient.right+=20;
    //
    UI_WM_CALC_PARENT_NONCLIENTRECT,

#pragma endregion

#pragma region  // ����

    // ģ̬������ʾ֮ǰ�����ⲿһ�����ô�����ʾ���͵Ļ���
    //    message: UI_WM_SHOWMODALWINDOW
    //    wparam: na
    //    lparam: na
    //    lret: 1 �ⲿ�Ѵ��� 0 �ⲿδ������Ĭ����ʾ
    UI_WM_SHOWMODALWINDOW,

#pragma endregion

#pragma region // ����
    //
    //  �ڴ��ڻ���֮ǰ�������ڷ���һ��֪ͨ�����ڿ�����������ֹ�Լ����뻻��
    //		message: UI_WM_SKINCHANGING
    //		code:
    //		wparam:  BOOL* pbChangeSkin
    //		lparam: 
    //
    //	Return:
    //
    UI_WM_SKINCHANGING,

    //
    //	�ڴ��ڻ�����ɺ󣬸����ڷ���һ��֪ͨ
    //
    //		message: UI_WM_SKINCHANGED
    //		code:
    //		wparam:  
    //		lparam: 
    //
    //	Return:
    //
    UI_WM_SKINCHANGED,

    //
    //	�ڴ��ڵ�HLS�仯�󣬸����ڷ���һ��֪ͨ
    //
    UI_WM_SKINHLSCHANGED,

    //
    //	IRenderFont���屻�޸ĺ��listener��������֪ͨ�¼�
    //
    //		message: UI_WM_FONTMODIFIED
    //		code:
    //		wparam:  IRenderFont*
    //		lparam:  
    //
    //	Return:
    //
//    UI_WM_FONTMODIFIED, -- �������Ĳ��ã�����

    // ͼƬ����
    UI_WM_SKINMATERIALCHANGED,
#pragma endregion

   

#pragma region // Popup Window

    //
    //  PopupControlWindow���͸�LISTBOX��MENU����Ϣ��LISTBOXת����COMBOBOX����Ϣ��COMBOBOX/MENUת�������ڵ���ϢpMsgFrom��ָ����Ϣ���ͷ�
    //
    UI_WM_INITPOPUPCONTROLWINDOW,
    UI_WM_UNINITPOPUPCONTROLWINDOW,
#pragma endregion

    //////////////////////////////////////////////////////////////////////////
#pragma region // UIBuilder��ʹ�õ�֪ͨ��Ϣ

    // Ԥ�����ڱ������ˣ�֪ͨ�ⲿ�ͷ��ڴ棬��PostMessage(m_dwUserData)��ʽ����
    //   wParam:  IWindow*ָ��
    //   lParam:  NULL
    UI_WM_PREVIEW_WINDOW_DESTROY,

#pragma endregion

    //////////////////////////////////////////////////////////////////////////
    // UI_WM_OVERWRITE,  // ��Ҫ��д���麯����Ϣ���塣���¼ܹ��У��ⲿ�ؼ�������ֱ��
                      // �̳л�����麯������˸�Ϊ����Ϣ��ͳһ��ProcessMessage

    //
    //  ��ȡ����������С�������������Margin������Ҫ�Լ�ȥ����padding/border��
    //
    //  message : UI_WM_GETDESIREDSIZE
    //  code : NA
    //  wparam : [out] SIZE*
    //  lparam : GETDESIREDSIZEINFO* (Ŀǰ����ITextRenderBaseֵ��ΪNULL)
    //
    UI_WM_GETDESIREDSIZE,

    //
    //  ��UICreateInstanceʱ���ã�������һ���ڹ����г�ʼ�����󲢷��سɹ�ʧ�ܵķ���
    //  
    //  message : UI_WM_FINALCONSTRUCT
    //  code : NA
    //  wparam : IUIApplication*
    //  lparam : NA
    //
    UI_WM_FINALCONSTRUCT,

    //
    //  ��~UIObjCreator�е��ã�������������ǰ�����麯���ķ���
    //  
    //  message : UI_WM_FINALRELEASE
    //  code : NA
    //  wparam : NA
    //  lparam : NA
    //
    UI_WM_FINALRELEASE,

    //
    //  ��~UIObjCreator�е��ã�������������ǰ�����麯���ķ���
    //  
    //  message : UI_WM_GETOBJECTINFO
    //  code : NA
    //  wparam : TCHAR** ppszName
    //  lparam : int* pObjType
    //
    UI_WM_GETOBJECTINFO,

    //
    //  �ӿڲ�ѯ
    //
    //  message : UI_WM_QUERYINTERFACE
    //  code : NA
    //  wparam : GUID*   pguid
    //  lparam : void**  ppOut
    //
    UI_WM_QUERYINTERFACE,

    //
    //  RenderBase������Ϣ
    //    
    //  message : UI_WM_RENDERBASE_DRAWSTATE
    //  code : NA
    //  wparam : 
    //  lparam : RENDERBASE_DRAWSTATE*
    //
    UI_WM_RENDERBASE_DRAWSTATE,

    //
    //  RenderBase��ʼ��
    //    
    //  message : UI_WM_INITIALIZE
    //  code : NA
    //  wparam : 
    //  lparam :
    //
    UI_WM_INITIALIZE,

    //
    //  �޸Ĵ�����������
    //  
    //  message: CREATESTRUCT*
    //
    UI_WM_PRECREATEWINDOW,

    //
    //  ThemeRenderBase��ȡ��Theme����
    //    
    //  message : UI_WM_RENDERBASE_GETTHEMENAME
    //  code : NA
    //  wparam : [out] TCHAR** ppszName
    //  lparam :
    //
    UI_WM_RENDERBASE_GETTHEMENAME,

    //
    //  �ؼ��Ի�
    //
    //  wParam : UIOWNERDRAWSTRUCT
    //  lparam : NA.
    //
    UI_WM_OWNERDRAW,
};




//
//  ��XML�м��ض��󣬲����ö��������
//  message : UI_WM_SETATTRIBUTE
//  code : NA
//  wparam : IMapAttribute*
//  lparam : bool bReload
//
// #define  UI_WM_SETATTRIBUTE  139281918

//
//  ���ö�������ԣ���Я��һ��ǰ׺����
//  message : UI_WM_SETATTRIBUTE_PREFIX
//  code : NA
//  wparam : SetAttrPrefixData*
//  lparam : 
//
// #define  UI_WM_SETATTRIBUTE_PREFIX  139281919 (��������ʹ��UI_WM_SERIALIZE)

//
//  ����ǰ���ã��ͷ���������ʹ�õ�Ƥ����Դ��
//  Ŀǰ�ڶ�����ʱ��Ҳ�ᴥ��һ�Σ����ڳ�ʼ��
//  
//  message : UI_WM_RESETATTRIBUTE
//  code : NA
//  wparam : NA
//  lparam : NA
//
#define  UI_WM_RESETATTRIBUTE  139281920


namespace UI
{
interface IMapAttribute;
interface IListAttribute;
interface IUIEditor;
interface IUIEditorGroupAttribute;
}
// �������л���Ϣ������ȡ��WM_SETATTRIBUTE
enum SERIALIZEFLAG
{
	SERIALIZEFLAG_LOAD = 0x01,
	SERIALIZEFLAG_SAVE = 0x02,
//	SERIALIZEFLAG_GETLIST = 0x04,  // ��ȡ�����б����ʾ��Ϣ ���õ�������Ϣ��UIMSG_WM_EDITORGETATTRLIST

	// load ��ʶ
	SERIALIZEFLAG_RELOAD = 0x0100,
    SERIALIZEFLAG_LOAD_ERASEATTR = 0x0200,  // ��ȡ�����Ժ󣬽������Դ�mapattr��ɾ��

	// save ��ʶ
};
struct SERIALIZEDATA
{
	union{
		UI::IMapAttribute*  pMapAttrib;    // load [in] / getlist [out]
		UI::IListAttribute*  pListAttrib;  // save [out]
	};
    const TCHAR* szPrefix;      // ����ǰ׺
	UINT nFlag;
};

// wParam: SERIALIZEDATA
// lParam:
// return: 
#define  UI_WM_SERIALIZE 139281928

//
// ��ΪĿ��������ק�¼�
//  wParam : DROPTARGETEVENT_TYPE
//  lParam : DROPTARGETEVENTDATA*
//
#define UI_WM_DROPTARGETEVENT   136041933

// ��ȡ�������ꡢ���̹�������Ŀǰֻ�д��ں��б�ؼ����Լ��Ĺ�����
// Return: IMKMgr* 
#define UI_WM_GET_MOUSEKEYBOARD_MANAGER  136201143

// ���ô����ϵ�Ĭ�ϰ�ť (����Ϣ������ˢ�¶���)
// message:  UI_DM_SETDEFID  
// wParam:   IObject*
#define UI_DM_SETDEFID  138011443

// ��ȡ�����ϵ�Ĭ�ϰ�ť
// message: UI_DM_GETDEFID  
// return: IObject*
#define UI_DM_GETDEFID  138011444

// ScrollBarMgr֪ͨ�ؼ�ƽ�����������ڿؼ��Լ�ˢ�»�������������ʶ
// message: UI_WM_INERTIAVSCROLL
// wParam: nOldPos
// wParam: nNewPos
#define UI_WM_INERTIAVSCROLL  138181247

//
//	��ť����¼�
//
//		message:UI_WM_NOTIFY
//		code:   UI_BN_CLICKED
//		wparam: NA // -- POINT*������window
//		lParam: 
//
#define UI_BN_CLICKED  139222333

struct EDITORGETOBJECTATTRLISTDATA
{
    UI::IUIEditor*  pEditor;
	UI::IUIEditorGroupAttribute*  pGroupAttr;
	const TCHAR*  szPrefix;
};
// ��ȡ���������б�
// wParam: EDITORGETOBJECTATTRLISTDATA*
#define UI_EDITOR_GETOBJECTATTRLIST  139252026

//
// Ϊ�˿�������Ϣ��Ӧ������ֱ�ӷ��ʵ�ǰ��Ϣ�ṹ����m_pCurMsg��Ϊ��Ա�������з��ʻ�����
// Ϊ�˷�ֹ�ڴ���һ����ϢA�Ĺ����У������ٷ�������һ����ϢB������A��m_pCurMsg��B�۸ģ�
// ����������һ����ʱ�������浱ǰ��Ϣm_pCurMsg��������B�������m_pCurMsg�ָ���A
// Ϊ��ʵ�ֻ�ԭm_pCurMsg�����е���Ϣ����ֱ�ӷ��أ����ǽ�m_pCurMsg��ԭ֮����return
//
// fix bug: 20120831
//    Ϊ�˽�������ཫ��ϢCHAIN������ʱ��������UI_BEGIN_MSG_MAP�ֻ�������Ӧһ��DoHook��
//    ����Hook���յ�������Ϣ���������һ��ProcessMessageNoHook�������ڲ�������DoHook
//    ͬʱ��������CHAINʱ
// ps: 20130320
//    ��Ixxx��Σ��ṩIMessage::virtualProcessMessage�麯����xProcessMessage���麯����
//    ��xxx��Σ�Ҳ�ṩinnerVirtualProcessMessage(��Ҫ���ڷ�IMessage������,IMessageInnerProxy����)��xProcessMessage���麯��
//    ͬʱ�ⲿ�ؼ���ֱ�Ӽ̳���MessageProxy�ṩ��Ϣӳ��
//
#define UI_BEGIN_MSG_MAP                              \
    virtual BOOL innerVirtualProcessMessage(UI::UIMSG* pMsg, int nMsgMapID=0, bool bDoHook=false) \
    {                                                 \
        return this->nvProcessMessage(pMsg, nMsgMapID, bDoHook); \
    }                                                 \
    BOOL nvProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook) \
    {                                                 \
        UIASSERT (pMsg);                              \
        if (NULL == pMsg)                             \
            return FALSE;                             \
                                                      \
        UI::UIMSG*  pOldMsg  = this->GetCurMsg();     \
        BOOL bRet = this->xxProcessMessage(pMsg, nMsgMapID, bDoHook); \
        this->SetCurMsg(pOldMsg);                     \
                                                      \
        return bRet;                                  \
    }                                                 \
    BOOL xxProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook) \
    {                                                 \
        /*ȡ��wParam,lParam���Ա����Window��Ϣ�����е�wParam,lParam����*/\
        WPARAM wParam = pMsg->wParam;                 \
        LPARAM lParam = pMsg->lParam;                 \
        UINT   uMsg   = pMsg->message;                \
        UINT   code   = pMsg->nCode;                  \
        UI::IMessage* pMsgFrom = pMsg->pMsgFrom;      \
        LRESULT& lResult = pMsg->lRet; /* ����wtl */  \
                                                      \
        this->SetCurMsg(pMsg);                        \
                                                      \
        /*��ϢHOOK����*/                              \
        if (bDoHook && this->DoHook(pMsg, nMsgMapID)) \
            return TRUE;                              \
                                                      \
        switch (nMsgMapID)                            \
        {                                             \
        case 0:                                       


#define UI_END_MSG_MAP                                \
            break;                                    \
        }                                             \
        return FALSE;                                 \
    }

#define UI_END_MSG_MAP_CHAIN_PARENT(baseclass)        \
            break;                                    \
        }                                             \
        if (baseclass::nvProcessMessage(pMsg, nMsgMapID, false)) \
            return TRUE;                              \
        return FALSE;                                 \
    }

// ���ֻ��ֱ�ӵ���UICHAIN_MSG_MAP_POINT_MEMBER(static_cast<baseinterface*>(m_pI##classname))��ֻ����ӦnMapIDΪ0�������
// �������UI_END_MSG_MAP_CHAIN_PARENT_Ixxx�����ڽ���Ϣ���������� 
#define UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(classname, baseinterface) \
            break;                                    \
        }                                             \
        {                                             \
            if (m_pI##classname)                      \
                if (static_cast<baseinterface*>(m_pI##classname)->nvProcessMessage(pMsg, nMsgMapID, false)) \
                    return TRUE;                      \
        }                                             \
        return FALSE;                                 \
    }

#define UI_BEGIN_MSG_MAP_Ixxx(classname) \
    typedef UI::UIObjCreator<classname> _CreatorClass; \
    static void  CreateInstance(UI::IUIApplication* p, classname** pp) \
    { classname::_CreatorClass::UICreateInstance(p, pp); } \
    virtual BOOL virtualProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook) \
    { return nvProcessMessage(pMsg, nMsgMapID, bDoHook); } \
    UI_BEGIN_MSG_MAP

//
// ��Ϣ������
//
//	�ú��и�ȱ�㣬�����ܽ��յ���ǰ�ķ�֧��Ϣ�����ܴ���������֧��Ϣ����ʹ��UI_BEGIN_CHAIN_ALL_MSG_MAP�Ľ���

#define UICHAIN_MSG_MAP(theChainClass)                \
    if (theChainClass::xProcessMessage(pMsg, nMsgMapID, false)) \
        return TRUE;                     

#define UICHAIN_MSG_MAP_MEMBER(theChainMember)        \
    if (theChainMember.xProcessMessage(pMsg, nMsgMapID, false)) \
        return TRUE;

#define UICHAIN_MSG_MAP_POINT_MEMBER(pTheChainMember)  \
    if (pTheChainMember)                               \
    {                                                  \
        if (pTheChainMember->nvProcessMessage(pMsg, nMsgMapID, false)) \
        { \
            return TRUE;                               \
        } \
    }

// 
//  ǿ�н�����ǰ��Ϣ��֧��������Ϣȫ�����ݸ���Ҫ�Ķ���
//
//	TODO: �ú�ȱ�㣬ǿ�н����󣬲�֪�����ĸ���֧�ٽ��п�ʼ������Ĭ��ȡΪ0�����齫�ú������ǰ����������
//
#define UI_BEGIN_CHAIN_ALL_MSG_MAP			          \
            break;                                    \
        }

#define UI_END_CHAIN_ALL_MSG_MAP                      \
        switch (nMsgMapID)                            \
        {                                             \
            case 0: 

#define UIALT_MSG_MAP(msgMapID)                       \
        break;                                        \
    case msgMapID:                                    

////////////////////////////////////////////////////////////////////////////
//                                                                        //
//                               UI��Ϣ����                               //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
// ����Ҫ��������������
//	1. ����Ϣ�Ƿ񱻴������
//	2. ����Ϣ�ķ���ֵ
//	3. ����Ϣ�Ĳ�������


// int OnCreate()
// remark
//		WM_CREATE��Ϣ����ԭʼ��Windows��Ϣ������UI SDK�Լ�ģ�����Ϣ��
//		���Ҳ����Ĳ�������μ���Window::Create
#define UIMSG_WM_CREATE(func)                          \
    if (uMsg == WM_CREATE)                             \
    {                                                  \
        SetMsgHandled(TRUE);                           \
        pMsg->lRet = (long)func((LPCREATESTRUCT)lParam);\
        if (IsMsgHandled())                            \
            return TRUE;                               \
    }

// BOOL OnInitDialog(HWND hWnd, LPARAM lInitParam)
#define UIMSG_WM_INITDIALOG  MSG_WM_INITDIALOG

// void OnClose()
#define UIMSG_WM_CLOSE   MSG_WM_CLOSE

// void OnNcDestroy()
#define UIMSG_WM_NCDESTROY MSG_WM_NCDESTROY

// void OnDestroy()
#define UIMSG_WM_DESTROY   MSG_WM_DESTROY

// void OnSysCommand(UINT nID, CPoint point)
#define UIMSG_WM_SYSCOMMAND  MSG_WM_SYSCOMMAND

// void  OnEraseBkgnd(UI::IRenderTarget* pRenderTarget)
//	remark
//		����HDC�����Ѿ�������˫���崦���ˣ�����ֱ��ʹ�á�ԭʼ��WM_ERASEBKGND��Ϣ�ѱ�������
//		����ȥ���˷���ֵ���Է��������Լ�Ҳ��֪�����׸÷���ʲô����ʵ����ķ���ֵû����˼��
#define UIMSG_WM_ERASEBKGND(func)                     \
    if (uMsg == WM_ERASEBKGND)                        \
    {                                                 \
        SetMsgHandled(TRUE);                          \
		func((UI::IRenderTarget*)wParam);             \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnPaint(IRenderTarget* pRenderTarget)
#define UIMSG_WM_PAINT(func)                          \
    if (uMsg == WM_PAINT)                             \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UI::IRenderTarget*)wParam);             \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

#define UIMSG_WM_PAINT2(func)                         \
    if (uMsg == WM_PAINT)                             \
    {                                                 \
        SetMsgHandled(TRUE);                          \
		func((UI::IRenderTarget*)wParam, (UI::RenderContext*)lParam); \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnNcPaint( void )
#define UIMSG_WM_NCPAINT  MSG_WM_NCPAINT

// BOOL OnNcActivate( BOOL bActive )
#define UIMSG_WM_NCACTIVATE  MSG_WM_NCACTIVATE


// void OnShowWindow(BOOL bShow, UINT nStatus)
#define UIMSG_WM_SHOWWINDOW  MSG_WM_SHOWWINDOW

// UINT OnGetDlgCode(LPMSG lpMsg)
#define UIMSG_WM_GETDLGCODE  MSG_WM_GETDLGCODE

// LRESULT OnNcHitTest( POINT pt )
// remark
//		����pt�Ѵ�screen����ת����client����
#define UIMSG_WM_NCHITTEST(func)                      \
    if (uMsg == WM_NCHITTEST)                         \
    {                                                 \
        POINT pt = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };          \
        ::ScreenToClient(pMsg->pObjMsgTo->GetWindowObject()->m_hWnd, &pt );\
        SetMsgHandled(TRUE);                          \
        pMsg->lRet = (long)func( pt );                \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// UINT OnHitTest( POINT* p )
#define UIMSG_WM_HITTEST(func)                        \
    if (uMsg == UI_WM_HITTEST)                        \
    {                                                 \
        POINT pt = { (LONG)wParam, (LONG)lParam };    \
        SetMsgHandled(TRUE);                          \
        pMsg->lRet = (long)func( &pt );               \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// BOOL OnSetCursor(HWND hWnd, UINT nHitTest, UINT message);
#define UIMSG_WM_SETCURSOR  MSG_WM_SETCURSOR

//	void  OnSize(UINT nType, int cx, int cy);
#define UIMSG_WM_SIZE(func)                           \
    if (uMsg == WM_SIZE)                              \
    {                                                 \
        UINT nType = (UINT)wParam;                    \
        int  cx    = LOWORD(lParam);                  \
        int  cy    = HIWORD(lParam);                  \
        SetMsgHandled(TRUE);                          \
        func(nType, cx, cy);                          \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

//	void OnSizing( UINT nSize, LPRECT lpRect);
#define UIMSG_WM_SIZING MSG_WM_SIZING

// void OnMove(POINT ptPos)
#define UIMSG_WM_MOVE MSG_WM_MOVE

// void OnMoving(UINT fwSide, LPRECT pRect)
#define UIMSG_WM_MOVING MSG_WM_MOVING

// LRESULT OnNcCalcSize(BOOL bCalcValidRects, LPARAM lParam)
#define UIMSG_WM_NCCALCSIZE MSG_WM_NCCALCSIZE

// void OnWindowPosChanging(LPWINDOWPOS lpWndPos)
#define UIMSG_WM_WINDOWPOSCHANGING  MSG_WM_WINDOWPOSCHANGING
#define SWP_LAYEREDWINDOW_SIZEMOVE  0x80000000   // ��ʾ���Ǹ��ֲ㴰��ģ��Ĵ��ڴ�С�ı�
#define SWP_NOUPDATELAYOUTPOS       0x40000000   // ����SetObjectPosʱ�����²�������

// void OnWindowPosChanged(LPWINDOWPOS lpWndPos)
#define UIMSG_WM_WINDOWPOSCHANGED  MSG_WM_WINDOWPOSCHANGED

#define UIMSG_WM_GETMINMAXINFO MSG_WM_GETMINMAXINFO

// void OnStyleChanged(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
#define UIMSG_WM_STYLECHANGED  MSG_WM_STYLECHANGED

// void OnStyleChanging(int nStyleType, LPSTYLESTRUCT lpStyleStruct)
#define UIMSG_WM_STYLECHANGING MSG_WM_STYLECHANGING

// void  OnSetFocus(IObject* pOldFocusObj )
#define UIMSG_WM_SETFOCUS(func)                       \
    if (uMsg == WM_SETFOCUS)                          \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((IObject*)wParam);                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnKillFocus(IObject* pNewFocusObj)
#define UIMSG_WM_KILLFOCUS(func)                      \
    if (uMsg == WM_KILLFOCUS)                         \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((IObject*)wParam);                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnActivateApp(BOOL bActive, DWORD dwThreadID)
#define UIMSG_WM_ACTIVATEAPP MSG_WM_ACTIVATEAPP

// int OnMouseActivate(HWND wndTopLevel, UINT nHitTest, UINT message)
#define UIMSG_WM_MOUSEACTIVATE MSG_WM_MOUSEACTIVATE

// void OnCancelMode()
#define UIMSG_WM_CANCELMODE  MSG_WM_CANCELMODE
//
// ��ȡRender Font
//
// IRenderFont*  OnGetFont()
#define UIMSG_WM_GETRENDERFONT(func) \
    if (uMsg == UI_WM_GETRENDERFONT) \
    { \
        SetMsgHandled(TRUE); \
        lResult = (long)(IRenderFont*)func(); \
        if (IsMsgHandled()) \
            return TRUE; \
    }

// UI::GRAPHICS_RENDER_LIBRARY_TYPE OnGetGraphicsRenderType()
#define UIMSG_WM_GETGRAPHICSRENDERLIBRARYTYPE(func)       \
    if (uMsg == UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE)   \
    {                                                     \
        SetMsgHandled(TRUE);                              \
        pMsg->lRet = (long)func();                        \
        if (IsMsgHandled())                               \
            return TRUE;                                  \
    } 


// void OnCalcParentNonClientRect(CRegion4* prcNonClientRegion)
#define UIMSG_WM_CALCPARENTNONCLIENTRECT(func)            \
    if (uMsg == UI_WM_CALC_PARENT_NONCLIENTRECT)          \
    {                                                     \
        SetMsgHandled(TRUE);                              \
        func((CRegion4*)wParam);                          \
        if (IsMsgHandled())                               \
            return TRUE;                                  \
    } 

// point�����ʵ�ʴ���
// void OnMouseMove(UINT nFlags, POINT point)
#define UIMSG_WM_MOUSEMOVE MSG_WM_MOUSEMOVE

// void OnMouseLeave( )
#define UIMSG_WM_MOUSELEAVE MSG_WM_MOUSELEAVE

// void OnMouseHover(WPARAM wParam, CPoint ptPos)
#define UIMSG_WM_MOUSEHOVER(func)\
    if (uMsg == WM_MOUSEHOVER)   \
	{                            \
		SetMsgHandled(TRUE);     \
        POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) }; \
		func(wParam, &point);    \
		lResult = 0;             \
		if (IsMsgHandled())      \
			return TRUE;         \
	}

// BOOL  OnMouseWheel(UINT nFlags, short zDelta, POINT pt)
#define UIMSG_WM_MOUSEWHEEL MSG_WM_MOUSEWHEEL

// void  OnLButtonDown(UINT nFlags, POINT point)
#define UIMSG_WM_LBUTTONDOWN MSG_WM_LBUTTONDOWN

// void  OnRButtonDown(UINT nFlags, CPoint point)
#define UIMSG_WM_RBUTTONDOWN  MSG_WM_RBUTTONDOWN

// void  OnRButtonUp(UINT nFlags, CPoint point)
#define UIMSG_WM_RBUTTONUP    MSG_WM_RBUTTONUP

// void  OnLButtonDblClk(UINT nFlags, POINT point)
#define UIMSG_WM_LBUTTONDBLCLK MSG_WM_LBUTTONDBLCLK

// void  OnLButtonUp(UINT nFlags, POINT point)
#define UIMSG_WM_LBUTTONUP MSG_WM_LBUTTONUP

//void  OnNcMouseMove( UINT nHitTest, POINT point );
#define UIMSG_WM_NCMOUSEMOVE MSG_WM_NCMOUSEMOVE

// void  OnNcMouseHover( UINT nHitTest, POINT point ); 
#define UIMSG_WM_NCMOUSEHOVER(func)                   \
    if (uMsg == WM_NCMOUSEHOVER)                      \
    {                                                 \
        POINT pt = { GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam) }; \
        SetMsgHandled(TRUE);                          \
        func((UINT)wParam, pt);                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnNcMouseLeave();   
#define UIMSG_WM_NCMOUSELEAVE(func)                   \
    if (uMsg == WM_NCMOUSELEAVE)                      \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func( );                                      \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnNcLButtonDown( UINT nHitTest, POINT point );
#define UIMSG_WM_NCLBUTTONDOWN MSG_WM_NCLBUTTONDOWN

// void OnNcLButtonUp( UINT nHitTest, POINT point );
#define UIMSG_WM_NCLBUTTONUP MSG_WM_NCLBUTTONUP

// void OnNcLButtonDblClk( UINT nHitTest, POINT point );
#define UIMSG_WM_NCLBUTTONDBLCLK MSG_WM_NCLBUTTONDBLCLK

// void OnNcRButtonDown( UINT nHitTest, POINT point );
#define UIMSG_WM_NCRBUTTONDOWN MSG_WM_NCRBUTTONDOWN

// void OnNcRButtonUp( UINT nHitTest, POINT point );
#define UIMSG_WM_NCRBUTTONUP MSG_WM_NCRBUTTONUP

// void OnNcRButtonDblClk( UINT nHitTest, POINT point );
#define UIMSG_WM_NCRBUTTONDBLCLK MSG_WM_NCRBUTTONDBLCLK

// void OnNcMButtonDown( UINT nHitTest, POINT point );
#define UIMSG_WM_NCMBUTTONDOWN MSG_WM_NCMBUTTONDOWN

// void OnNcMButtonUp( UINT nHitTest, POINT point );
#define UIMSG_WM_NCMBUTTONUP MSG_WM_NCMBUTTONUP

// void OnNcMButtonDblClk( UINT nHitTest, POINT point );
#define UIMSG_WM_NCMBUTTONDBLCLK MSG_WM_NCMBUTTONDBLCLK

//  void OnChar( UINT nChar, UINT nRepCnt, UINT nFlags );
#define UIMSG_WM_CHAR MSG_WM_CHAR

// void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
#define UIMSG_WM_KEYDOWN MSG_WM_KEYDOWN

//  void OnKeyUp( UINT nChar, UINT nRepCnt, UINT nFlags );
#define UIMSG_WM_KEYUP MSG_WM_KEYUP

// void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
#define UIMSG_WM_SYSKEYDOWN MSG_WM_SYSKEYDOWN

// void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
#define UIMSG_WM_SYSKEYUP MSG_WM_SYSKEYUP

// OnThemeChanged()
#if(_WIN32_WINNT >= 0x0501)
#define UIMSG_WM_THEMECHANGED MSG_WM_THEMECHANGED
#else
#define UIMSG_WM_THEMECHANGED(fun)
#endif

// �ȴ���WM_TIMER����һ��LPARAM
// void  OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
#define UIMSG_WM_TIMER(func)                          \
    if (uMsg == WM_TIMER)                             \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UINT_PTR)wParam, lParam);               \
        lResult = 0;                                  \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }

// void OnContextMenu(HWND hWnd, POINT point)
#define UIMSG_WM_CONTEXTMENU  MSG_WM_CONTEXTMENU

//void OnInitPopupControlWindow()
#define UIMSG_WM_INITPOPUPCONTROLWINDOW(func)         \
    if (uMsg == UI_WM_INITPOPUPCONTROLWINDOW)         \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }

//void OnUnInitPopupControlWindow()
#define UIMSG_WM_UNINITPOPUPCONTROLWINDOW(func)       \
    if (uMsg == UI_WM_UNINITPOPUPCONTROLWINDOW)       \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }

// void  OnObjectLoaded();
#define UIMSG_WM_OBJECTLOADED(func)                   \
    if (uMsg == UI_WM_OBJECTLOADED)                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }



//
// UI_WM_NOTIFY
//


// TODO: 
//	warn �����objIDû��ʹ��ȫ·��ʽ����window/panel/loginbtn��������ֱ��ʹ����loginbtn��
//       �����������ͬ��ID������ô�������Լ�ȥ�ж�
// void OnBtnClicked( );
#define UIMSG_BN_CLICKED(OBJID, func)                 \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_BN_CLICKED &&                      \
        NULL != pMsgFrom   &&                         \
        0 == _tcscmp(((IObject*)pMsgFrom)->GetId(),OBJID)) \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func( );                                      \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }

// void OnBtnClicked( );
#define UIMSG_BN_CLICKED2(ptr, func)                  \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_BN_CLICKED &&                      \
        pMsgFrom == (UI::IMessage*)ptr)               \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func( );                                      \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }

// void  OnBnClicked(IMessage*  pMsgFrom);
#define UIMSG_BN_CLICKED3(func)                       \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_BN_CLICKED)                        \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func(pMsgFrom);                               \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }
// void OnBtnClicked(Object* pBtnObj, POINT* pt);
#define UIMSG_BN_CLICKED_EX(OBJID, func)              \
    if (uMsg == UI_WM_NOTIFY &&                       \
        code == UI_BN_CLICKED &&                      \
        NULL != pMsgFrom &&                           \
        0 == _tcscmp(((IObject*)pMsgFrom)->GetId(), OBJID)) \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((IObject*)pMsgFrom, (POINT*)wParam);     \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }

// LRESULT  OnNotify(WPARAM w, LPARAM l);
#define UIMSG_WM_NOTIFY(_code, func)                  \
    if (uMsg == UI_WM_NOTIFY && _code == code)        \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = func(wParam, lParam);               \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }
// LRESULT  OnNotify(WPARAM w, LPARAM l);
#define UIMSG_WM_NOTIFY_CODE_FROM(_code, _From, func) \
    if (uMsg == UI_WM_NOTIFY &&                       \
        _code == code &&                              \
        static_cast<IMessage*>(_From) == pMsgFrom)    \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = func(wParam, lParam);               \
        if(IsMsgHandled())                            \
            return TRUE;                              \
    }

// void  OnHScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
#define UIMSG_WM_HSCROLL(func)                        \
    if( uMsg == WM_HSCROLL )                          \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((int)wParam, (int)lParam, pMsgFrom);     \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnVScroll(int nSBCode, int nPos, IMessage* pMsgFrom)
#define UIMSG_WM_VSCROLL(func)                        \
    if (uMsg == WM_VSCROLL)                           \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((int)wParam, (int)lParam, pMsgFrom);     \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnStateChanged(int nOld, int nNew);
// #define UIMSG_WM_STATECHANGED(func)                   \
//     if (uMsg == UI_WM_STATECHANGED)                   \
//     {                                                 \
//         SetMsgHandled(TRUE);                          \
//         func((int)wParam, (int)lParam);               \
//         if (IsMsgHandled())                           \
//             return TRUE;                              \
//     }

// void  OnStateChanged(UINT nMask);
#define UIMSG_WM_STATECHANGED2(func)                  \
    if (uMsg == UI_WM_STATECHANGED2)                  \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UINT)wParam);                           \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnStateChanging(int nOld, int nNew);
#define UIMSG_WM_STATECHANGING(func)                  \
    if (uMsg == UI_WM_STATECHANGING)                  \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((int)wParam, (int)lParam);               \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnRedrawObject();
#define UIMSG_WM_REDRAWOBJECT(func)                   \
    if (uMsg == UI_WM_REDRAWOBJECT)                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnSkinChanging(BOOL* pbChange)
#define UIMSG_WM_SKINCHANGING(func)                   \
    if (uMsg == UI_WM_SKINCHANGING)                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((BOOL*)wParam);                          \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnSkinChanged()
#define UIMSG_WM_SKINCHANGED(func)                    \
    if (uMsg == UI_WM_SKINCHANGED)                    \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnSkinHLSChanged(BOOL* pbChange)
#define UIMSG_WM_SKINHLSCHANGED(func)                 \
    if (uMsg == UI_WM_SKINHLSCHANGED)                 \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

#if 0
// void  OnFontModifyed(IRenderFont* pFont);
#define UIMSG_WM_FONTMODIFIED(func)                   \
    if (uMsg == UI_WM_FONTMODIFIED)                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((IRenderFont*)wParam);                   \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }
#endif

// void  OnAnimateTick(int nCount, UI::IStoryboard** ppArray)
#define UIMSG_WM_ANIMATE_TICK(func)                   \
    if (uMsg == UI_WM_ANIMATE_TICK)                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((int)wParam, (UI::IStoryboard**)lParam); \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnAnimateOver()
#define UIMSG_WM_ANIMATE_OVER(func)                   \
    if (uMsg == UI_WM_ANIMATE_OVER)                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }


// void  OnGetScrollOffset(int* pxOffset, int* pyOffset);
#define UIMSG_WM_GETSCROLLOFFSET(func)                \
    if (uMsg == UI_WM_GETSCROLLOFFSET)                \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((int*)wParam, (int*)lParam);             \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnGetScrollRange(int* pxRange, int* pyRange);
#define UIMSG_WM_GETSCROLLRANGE(func)                 \
    if (uMsg == UI_WM_GETSCROLLOFFSET)                \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((int*)wParam, (int*)lParam);             \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnVisibleChanged(BOOL bVisible, IObject* pObjChanged)
#define UIMSG_WM_VISIBLE_CHANGED(func)                \
    if (uMsg == UI_WM_VISIBLE_CHANGED)                \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func(wParam?TRUE:FALSE, (UI::IObject*)lParam);\
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// // void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
// #define UIMSG_WM_SETATTRIBUTE(func)                   \
//     if (uMsg == UI_WM_SETATTRIBUTE)                   \
//     {                                                 \
//         SetMsgHandled(TRUE);                          \
//         func((UI::IMapAttribute*)wParam, lParam?true:false); \
//         if (IsMsgHandled())                           \
//             return TRUE;                              \
//     }
// void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
#define UIMSG_WM_SETATTRIBUTE(func)                   \
    if (uMsg == UI_WM_SERIALIZE)                      \
    {                                                 \
        SetMsgHandled(TRUE);                          \
		SERIALIZEDATA* pData = (SERIALIZEDATA*)wParam; \
		if (pData->nFlag&SERIALIZEFLAG_LOAD)           \
		{ \
			bool bReload = pData->nFlag&SERIALIZEFLAG_RELOAD ? true:false; \
			func((UI::IMapAttribute*)pData->pMapAttrib, bReload); \
			if (IsMsgHandled())                           \
	            return TRUE;                              \
		} \
    }

// void  OnSerialize(SERIALIZEDATA* pData);
#define UIMSG_WM_SERIALIZE(func)                      \
	if (uMsg == UI_WM_SERIALIZE)                      \
	{                                                 \
		SetMsgHandled(TRUE);                          \
		func((SERIALIZEDATA*)wParam);                 \
		if (IsMsgHandled())                           \
			return TRUE;                              \
	}

// void SetAttribute(SetAttrPrefixData* pData);
// #define UIMSG_WM_SETATTRIBUTE_PREFIX(func)            \
//     if (uMsg == UI_WM_SETATTRIBUTE_PREFIX)            \
//     {                                                 \
//         SetMsgHandled(TRUE);                          \
//         func((UI::SetAttrPrefixData*)wParam);         \
//         if (IsMsgHandled())                           \
//             return TRUE;                              \
//     }

// void SetAttribute(SetAttrPrefixData* pData);
#define UIMSG_WM_SETATTRIBUTE_PREFIX(func)            \
    if (uMsg == UI_WM_SERIALIZE)                      \
    {                                                 \
        SetMsgHandled(TRUE);                          \
		SERIALIZEDATA* pData = (SERIALIZEDATA*)wParam; \
		if (pData->nFlag&SERIALIZEFLAG_LOAD)          \
		{                                             \
			UI::SetAttrPrefixData data;               \
			data.pMapAttrib = pData->pMapAttrib;      \
			data.szPrefix = pData->szPrefix;          \
			data.bErase = pData->nFlag&SERIALIZEFLAG_LOAD_ERASEATTR ? true:false; \
			func(&data);                              \
			if (IsMsgHandled())                       \
	            return TRUE;                          \
		}                                             \
    }

// void  ResetAttribute();
#define UIMSG_WM_RESETATTRIBUTE(func)                 \
    if (uMsg == UI_WM_RESETATTRIBUTE)                 \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
#define UIMSG_WM_EDITORGETATTRLIST(func)              \
    if (uMsg == UI_EDITOR_GETOBJECTATTRLIST)          \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((EDITORGETOBJECTATTRLISTDATA*)wParam);   \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// HRESULT  FinalConstruct(IUIApplication* p);
#define UIMSG_WM_FINALCONSTRUCT(func)                 \
    if (uMsg == UI_WM_FINALCONSTRUCT)                 \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = func((UI::IUIApplication*)wParam);      \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  FinalRelease();
#define UIMSG_WM_FINALRELEASE(func)                   \
    if (uMsg == UI_WM_FINALRELEASE)                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnGetObjectINfo(TCHAR** ppszName, int* pObjType);
#define UIMSG_WM_GETOBJECTINFO(func)                  \
    if (uMsg == UI_WM_GETOBJECTINFO)                  \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((TCHAR**)wParam, (int*)lParam);          \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void*  QueryInterface(const IID* pIID);
#define UIMSG_WM_QUERYINTERFACE(func)                 \
    if (uMsg == UI_WM_QUERYINTERFACE)                 \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = (LRESULT)func((const IID*)wParam);  \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

struct GETDESIREDSIZEINFO
{
    const TCHAR* szText;
    int  nLimitWidth;
};

// void  GetDesiredSize(SIZE* pSize);
#define UIMSG_WM_GETDESIREDSIZE(func)                 \
    if (uMsg == UI_WM_GETDESIREDSIZE)                 \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((SIZE*)wParam);                          \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void GetDesiredSize(SIZE* pSize, GETDESIREDSIZEINFO* pInfo);
#define UIMSG_WM_GETDESIREDSIZE2(func)                \
    if (uMsg == UI_WM_GETDESIREDSIZE)                 \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((SIZE*)wParam, (GETDESIREDSIZEINFO*)lParam); \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
#define UIMSG_WM_RENDERBASE_DRAWSTATE(func)           \
    if (uMsg == UI_WM_RENDERBASE_DRAWSTATE)           \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((RENDERBASE_DRAWSTATE*)wParam);          \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnDrawState(TEXTRENDERBASE_DRAWSTATE* pDrawStruct);
#define UIMSG_WM_TEXTRENDERBASE_DRAWSTATE(func)       \
    if (uMsg == UI_WM_RENDERBASE_DRAWSTATE)           \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((TEXTRENDERBASE_DRAWSTATE*)wParam);      \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void  OnInitialize();
#define UIMSG_WM_INITIALIZE(func)                     \
    if (uMsg == UI_WM_INITIALIZE)                     \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func();                                       \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }


// BOOL  PreCreateWindow(CREATESTRUCT* pcs);
#define UIMSG_WM_PRECREATEWINDOW(func)                \
    if (uMsg == UI_WM_PRECREATEWINDOW)                \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = func((CREATESTRUCT*)wParam);        \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnGetThemeName(TCHAR** ppszName);
#define UIMSG_WM_RENDERBASE_GETTHEMENAME(func)        \
    if (uMsg == UI_WM_RENDERBASE_GETTHEMENAME)        \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((TCHAR**)wParam);                        \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }


// void OnLCNDbclick(IMessage* pMsgFrom, POINT pt, IListItemBase* pItem)
#define UIMSG_LCN_DBCLICK(func)                       \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_LCN_DBCLICK )                      \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        POINT pt = {LOWORD(wParam), HIWORD(lParam) }; \
        func(pMsgFrom, pt, (UI::IListItemBase*)lParam);\
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnLcnSelChanged(IMessage* pMsgFrom, IListItemBase* pOldSelItem, IListItemBase* pSelItem)
#define UIMSG_LCN_SELCHANGED(func)                    \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_LCN_SELCHANGED )                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func(pMsgFrom, (UI::IListItemBase*)wParam, (UI::IListItemBase*)lParam); \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnDropTargetEvent(UI::DROPTARGETEVENT_TYPE eType, UI::DROPTARGETEVENT_DATA* pData)
#define UIMSG_WM_DROPTARGETEVENT(func)                \
    if (uMsg == UI_WM_DROPTARGETEVENT)                \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UI::DROPTARGETEVENT_TYPE)wParam, (UI::DROPTARGETEVENT_DATA*)lParam); \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnSetDefId(IObject* pButton);
#define UIMSG_DM_SETDEFID(func)                       \
    if (uMsg == UI_DM_SETDEFID)                       \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UI::IObject*)wParam);                   \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// IObject* OnGetDefId();
#define UIMSG_DM_GETDEFID(func)                       \
    if (uMsg == DM_GETDEFID)                          \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = (LRESULT)func();                    \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }
//////////////////////////////////////////////////////////////////////////
//
//                         ATL�е���Ϣ����
//
//////////////////////////////////////////////////////////////////////////

// LRESULT  OnMessageHandlerEx(UINT uMsg, WPARAM wParam, LPARAM lParam)
#define UIMESSAGE_HANDLER_EX  MESSAGE_HANDLER_EX

// void  OnCommandHandlerEx(UINT uNotifyCode, int nID, HWND wndCtl)
#define UICOMMAND_HANDLER_EX  COMMAND_HANDLER_EX

// LRESULT  OnMessageRangeHandlerEx(UINT uMsg, WPARAM wParam, LPARAM lParam)
#define UIMESSAGE_RANGE_HANDLER_EX MESSAGE_RANGE_HANDLER_EX


#define VIRTUAL_BEGIN_MSG_MAP(theClass) \
        public: \
        virtual BOOL ProcessWindowMessage(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, LRESULT& lResult, DWORD dwMsgMapID = 0) \
        { \
            BOOL bHandled = TRUE; \
            (hWnd); \
            (uMsg); \
            (wParam); \
            (lParam); \
            (lResult); \
            (bHandled); \
            switch(dwMsgMapID) \
            { \
            case 0:

#if 0
#ifndef MESSAGE_HANDLER
#define MESSAGE_HANDLER(msg, func) \
    if(uMsg == msg) \
    { \
        bHandled = TRUE; \
        lResult = func(uMsg, wParam, lParam, bHandled); \
        if(bHandled) \
            return TRUE; \
    }
#endif


#ifndef ALT_MSG_MAP
#define ALT_MSG_MAP(msgMapID) \
                break; \
            case msgMapID:
#endif

#ifndef END_MSG_MAP
#define END_MSG_MAP() \
                break; \
            default: \
                break; \
            } \
            return FALSE; \
            }
#endif
#endif

#endif  MESSAGEDEFINE_H_1F530C58_DD7E_4a28_9A4E_97613B69A20E