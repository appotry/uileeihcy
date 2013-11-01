#ifndef MESSAGEDEFINE_H_1F530C58_DD7E_4a28_9A4E_97613B69A20E
#define MESSAGEDEFINE_H_1F530C58_DD7E_4a28_9A4E_97613B69A20E

#include "UISDK\Kernel\Inc\Util\atlcrack.h"
#include <atltypes.h>
#define ATLTYPES_INLINE inline
#include <atltypes.inl>

//
//	由于系统控件的WM_COMMAND与UI控件的WM_COMAAND的消息参数是不一致的
//  为了以防两者互相混淆而导致崩溃，在这里对消息的code进行了重新定义
//

enum
{
    UI_WM_NOTIFY = WM_USER+WM_USER,

    //
    //	在ui中实现post message（稍后再响应）
    //	见CForwardPostMessageWindow
    //
    UI_WM_POSTMESSAGE,

#pragma region // UI
    //
    //	弹出提示条通知
    //		message:UI_WM_SHOW_TOOLTIP
    //		code:   0
    //		wparam:  
    //		lParam: 
    //		pMsgFrom: pWindow
    //
    //	如果对象不处理这个消息或返回FALSE，则将弹出默认的提示条，显示tooltip属性内容。
    //
    UI_WM_SHOW_TOOLTIP,

    //
    //	当itemdata不为空时，向控件获取item的提示信息
    //		message:UI_WM_GET_TOOLTIPINFO
    //		code:   
    //		wparam:  TOOLTIPITEM*
    //		lParam:  IToolTipUI*
    //		pMsgFrom: 
    //
    //	返回0表示失败，不显示提示条。返回1将立即显示提示条
    //
    UI_WM_GET_TOOLTIPINFO,

    //
    //   托盘消息ID
    //
    UI_WM_TRAYICON,

    //
    //  动画时间响应
    //     message:  UI_WM_ANIMATE_TICK
    //     code:     NA
    //     wParam:   nCount
    //     lparam:   ITimeline** ppArray
    //
    UI_WM_ANIMATE_TICK,

    //
    //  该对象的动画timeline全部结束了
    //     message:  UI_WM_ANIMATE_OVER
    //     code:     NA
    //     wParam:   
    //     lparam:   
    //
    UI_WM_ANIMATE_OVER,

    //
    //	滚动条位置改变通知
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
    //	获取当前绘制的偏移量，用于OnDraw
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
    //	获取当前滚动范围
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
    //	测试一个坐标在指定的对象上面的位置
    //
    //		message: UI_WM_HITTEST
    //		code:
    //		wparam:  x (in window)
    //		lparam:  y (in window)
    //		pMsgFrom: 
    //
    //	Return
    //
    //		HTNOWHERE表示不在该对象上面
    //
    UI_WM_HITTEST,

    //
    //	获取一个窗口的绘制类型
    //
    //		message:  UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE
    //		code:
    //		wparam:
    //		lparam:
    //
    //	Return
    //		
    //		RENDER_TYPE_GDI,RENDER_TYPE_GDIPLUS或者RENDER_TYPE_AUTO;
    //
    UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE,

    //
    //
    //
    UI_WM_GETRENDERFONT,

    //
    //	对象从xml中加载完毕，其属性、子对象、父对象也设置完毕
    //
    //		message: UI_WM_OBJECTLOADED
    //		code:
    //		wparam:  
    //		lparam:  
    //
    UI_WM_OBJECTLOADED,


    //
    //	MouseManager通知一个对象，它的当前状态发生改变(hover,press...)
    //  专门用于刷新处理，不要在这个消息中添加其它逻辑处理，因为可能会被HOOK掉，导致状态不正确
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
    UI_WM_STATECHANGED2, // 考虑逐步取代UI_WM_STATECHANGED

    //  
    //	在UI_WM_STATECHANGED之前被发送，可用于添加一些逻辑处理。最后将刷新放在
    //  UI_WM_STATECHANGED消息中。（场景：Combobox中的按钮，鼠标从按钮上移到Combobox上,
    //  Combobox拦截了STATECHANGED，并阻止按钮刷新）
    //
    UI_WM_STATECHANGING,

    // 窗口是否是分层窗口改变了
    //		message: UI_WM_WINDOWLAYEREDCHANGED
    //		code:
    //		wparam:  bool bLayered
    //		lparam:   
    UI_WM_WINDOWLAYEREDCHANGED,

    //
    //  如果这个对象支持布局，返回布局对象指针。如OBJ_WINDOW,OBJ_PANEL,OBJ_COMPOUND_CONTROL
    //
    //		message: UI_WM_GETLAYOUT
    //		code:
    //		wparam: 
    //		lparam
    //
    //	Return:  ILayout*
    //
    UI_WM_GETLAYOUT,

    //  redraw object，刷新自己。一般是被用于模仿invalidateRect，延迟刷新，用uipostmessage发送
    //
    //		message: UI_WM_REDRAWOBJECT
    //		code:
    //		wparam: NA.
    //		lparam: NA.
    //
    UI_WM_REDRAWOBJECT,

    //	祖、父对象中的可见状态发生了改变（主要是用于在父对象隐藏时，能自动将HwndObj对象也隐藏）
    //		message: UI_WM_PARENT_VISIBLE_CHANGED
    //		code:    
    //		wparam:  BOOL bVisible, (保持与WM_SHOWWINDOW一致)
    //		lparam:  Object*
    UI_WM_VISIBLE_CHANGED,

    //
    //  非UI消息，采用::SendMessage发送
    //  处理一个同步移动窗口事件(添加、修改、删除)
    //		wparam:  SYNC_WINDOW_EVENT_TYPE
    //		lparam:  具体查看SYNC_WINDOW_EVENT_TYPE定义
    UI_WM_SYNC_WINDOW,

    //
    //  非UI消息，采用::SendMessage发送
    //	当主窗口移动时，向SYNC_CUMTOM类型的窗口发送一个消息去同步自己的位置
    //		wparam:  HDWP
    //		lparam:  RECT* 
    //  return:  HWDP
    UI_WM_SYNC_WINDOWPOSCHANING,

    //
    //  父对象需要计算它的non client region(不包括padding,border)区域时，给
    //  non client child object发送该消息
    //       message: UI_WM_CALC_PARENT_NONCLIENTRECT
    //       wparam:  RECT*  prcNonClient.  例如滚动条在右侧时，prcNonClient.right+=20;
    //
    UI_WM_CALC_PARENT_NONCLIENTRECT,

#pragma endregion

#pragma region  // 窗口

    // 模态窗口显示之前，给外部一个设置窗口显示类型的机会
    //    message: UI_WM_SHOWMODALWINDOW
    //    wparam: na
    //    lparam: na
    //    lret: 1 外部已处理 0 外部未处理，按默认显示
    UI_WM_SHOWMODALWINDOW,

#pragma endregion

#pragma region // 换肤
    //
    //  在窗口换肤之前，给窗口发送一个通知。窗口可以在这里阻止自己参与换肤
    //		message: UI_WM_SKINCHANGING
    //		code:
    //		wparam:  BOOL* pbChangeSkin
    //		lparam: 
    //
    //	Return:
    //
    UI_WM_SKINCHANGING,

    //
    //	在窗口换肤完成后，给窗口发送一个通知
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
    //	在窗口的HLS变化后，给窗口发送一个通知
    //
    UI_WM_SKINHLSCHANGED,

    //
    //	IRenderFont字体被修改后给listener发出来的通知事件
    //
    //		message: UI_WM_FONTMODIFIED
    //		code:
    //		wparam:  IRenderFont*
    //		lparam:  
    //
    //	Return:
    //
//    UI_WM_FONTMODIFIED, -- 机制做的不好，废弃

    // 图片换肤
    UI_WM_SKINMATERIALCHANGED,
#pragma endregion

   

#pragma region // Popup Window

    //
    //  PopupControlWindow发送给LISTBOX、MENU的消息，LISTBOX转发给COMBOBOX的消息，COMBOBOX/MENU转发给窗口的消息pMsgFrom将指向消息发送方
    //
    UI_WM_INITPOPUPCONTROLWINDOW,
    UI_WM_UNINITPOPUPCONTROLWINDOW,
#pragma endregion

    //////////////////////////////////////////////////////////////////////////
#pragma region // Combobox
    UI_CBN_SELCHANGED,

    // 
    //  点击了下拉按钮或者ComboBox，通知子类去弹出下拉列表
    //      message: UI_WM_NOTIFY
    //      wParam:
    //      lParam:
    //      lRet:   0:失败  1:成功
    //
    UI_CBN_DROPDOWN,

    //  要求关闭下拉列表
    //      message: UI_WM_NOTIFY
    //      code: UI_CBN_CLOSEUP
    //      
    UI_CBN_CLOSEUP,
#pragma endregion

    //////////////////////////////////////////////////////////////////////////
#pragma region // Mene

    //	点击菜单项
    //		message: UI_WM_NOTIFY
    //		code:    UI_MENU_CLICK 
    //      wparam:  
    //      lparam: 
    //
    UI_MENU_CLICK,
#pragma endregion

    //////////////////////////////////////////////////////////////////////////
#pragma region // UIBuilder中使用的通知消息

    // 预览窗口被销毁了，通知外部释放内存，以PostMessage(m_dwUserData)形式发送
    //   wParam:  IWindow*指针
    //   lParam:  NULL
    UI_WM_PREVIEW_WINDOW_DESTROY,

#pragma endregion

    //////////////////////////////////////////////////////////////////////////
    // UI_WM_OVERWRITE,  // 需要重写的虚函数消息定义。在新架构中，外部控件不可能直接
                      // 继承基类的虚函数，因此改为发消息，统一走ProcessMessage

    //
    //  获取对象期望大小，不包含对象的Margin，但需要自己去计算padding/border等
    //
    //  message : UI_WM_GETDESIREDSIZE
    //  code : NA
    //  wparam : [out] SIZE*
    //  lparam : GETDESIREDSIZEINFO* (目前仅在ITextRenderBase值不为NULL)
    //
    UI_WM_GETDESIREDSIZE,

    //
    //  在UICreateInstance时调用，给对象一次在构造中初始化对象并返回成功失败的方法
    //  
    //  message : UI_WM_FINALCONSTRUCT
    //  code : NA
    //  wparam : IUIApplication*
    //  lparam : NA
    //
    UI_WM_FINALCONSTRUCT,

    //
    //  在~UIObjCreator中调用，给对象在析构前调用虚函数的方法
    //  
    //  message : UI_WM_FINALRELEASE
    //  code : NA
    //  wparam : NA
    //  lparam : NA
    //
    UI_WM_FINALRELEASE,

    //
    //  在~UIObjCreator中调用，给对象在析构前调用虚函数的方法
    //  
    //  message : UI_WM_GETOBJECTINFO
    //  code : NA
    //  wparam : TCHAR** ppszName
    //  lparam : int* pObjType
    //
    UI_WM_GETOBJECTINFO,

    //
    //  接口查询
    //
    //  message : UI_WM_QUERYINTERFACE
    //  code : NA
    //  wparam : GUID*   pguid
    //  lparam : void**  ppOut
    //
    UI_WM_QUERYINTERFACE,

    //
    //  RenderBase绘制消息
    //    
    //  message : UI_WM_RENDERBASE_DRAWSTATE
    //  code : NA
    //  wparam : 
    //  lparam : RENDERBASE_DRAWSTATE*
    //
    UI_WM_RENDERBASE_DRAWSTATE,

    //
    //  RenderBase初始化
    //    
    //  message : UI_WM_INITIALIZE
    //  code : NA
    //  wparam : 
    //  lparam :
    //
    UI_WM_INITIALIZE,

    //
    //  修改创建窗口数据
    //  
    //  message: CREATESTRUCT*
    //
    UI_WM_PRECREATEWINDOW,

    //
    //  ThemeRenderBase获取该Theme名称
    //    
    //  message : UI_WM_RENDERBASE_GETTHEMENAME
    //  code : NA
    //  wparam : [out] TCHAR** ppszName
    //  lparam :
    //
    UI_WM_RENDERBASE_GETTHEMENAME,

    //
    //  控件自绘
    //
    //  wParam : UIOWNERDRAWSTRUCT
    //  lparam : NA.
    //
    UI_WM_OWNERDRAW,
};




//
//  从XML中加载对象，并设置对象的属性
//  message : UI_WM_SETATTRIBUTE
//  code : NA
//  wparam : IMapAttribute*
//  lparam : bool bReload
//
// #define  UI_WM_SETATTRIBUTE  139281918

//
//  设置对象的属性，但携带一个前缀参数
//  message : UI_WM_SETATTRIBUTE_PREFIX
//  code : NA
//  wparam : SetAttrPrefixData*
//  lparam : 
//
// #define  UI_WM_SETATTRIBUTE_PREFIX  139281919 (废弃，再使用UI_WM_SERIALIZE)

//
//  换肤前调用，释放现在正在使用的皮肤资源。
//  目前在对象构造时，也会触发一次，用于初始化
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
// 对象序列化消息。用于取代WM_SETATTRIBUTE
enum SERIALIZEFLAG
{
	SERIALIZEFLAG_LOAD = 0x01,
	SERIALIZEFLAG_SAVE = 0x02,
//	SERIALIZEFLAG_GETLIST = 0x04,  // 获取属性列表和提示信息 采用单独的消息：UIMSG_WM_EDITORGETATTRLIST

	// load 标识
	SERIALIZEFLAG_RELOAD = 0x0100,
    SERIALIZEFLAG_LOAD_ERASEATTR = 0x0200,  // 获取到属性后，将该属性从mapattr中删除

	// save 标识
};
struct SERIALIZEDATA
{
	union{
		UI::IMapAttribute*  pMapAttrib;    // load [in] / getlist [out]
		UI::IListAttribute*  pListAttrib;  // save [out]
	};
    const TCHAR* szPrefix;      // 属性前缀
	UINT nFlag;
};

// wParam: SERIALIZEDATA
// lParam:
// return: 
#define  UI_WM_SERIALIZE 139281928

//
// 作为目标对象的拖拽事件
//  wParam : DROPTARGETEVENT_TYPE
//  lParam : DROPTARGETEVENTDATA*
//
#define UI_WM_DROPTARGETEVENT   136041933

// 获取对象的鼠标、键盘管理器，目前只有窗口和列表控件有自己的管理器
// Return: IMKMgr* 
#define UI_WM_GET_MOUSEKEYBOARD_MANAGER  136201143

// 设置窗口上的默认按钮 (该消息不负责刷新对象)
// message:  UI_DM_SETDEFID  
// wParam:   IObject*
#define UI_DM_SETDEFID  138011443

// 获取窗口上的默认按钮
// message: UI_DM_GETDEFID  
// return: IObject*
#define UI_DM_GETDEFID  138011444

// ScrollBarMgr通知控件平滑滚动，用于控件自己刷新或者设置其它标识
// message: UI_WM_INERTIAVSCROLL
// wParam: nOldPos
// wParam: nNewPos
#define UI_WM_INERTIAVSCROLL  138181247

//
//	按钮点击事件
//
//		message:UI_WM_NOTIFY
//		code:   UI_BN_CLICKED
//		wparam: NA // -- POINT*，基于window
//		lParam: 
//
#define UI_BN_CLICKED  139222333

struct EDITORGETOBJECTATTRLISTDATA
{
    UI::IUIEditor*  pEditor;
	UI::IUIEditorGroupAttribute*  pGroupAttr;
	const TCHAR*  szPrefix;
};
// 获取对象属性列表
// wParam: EDITORGETOBJECTATTRLISTDATA*
#define UI_EDITOR_GETOBJECTATTRLIST  139252026

//
// 为了可以在消息响应函数中直接访问当前消息结构，将m_pCurMsg作为成员函数进行访问或设置
// 为了防止在处理一个消息A的过程中，必须再发送另外一个消息B，导致A的m_pCurMsg被B篡改，
// 必须再增加一个临时变量保存当前消息m_pCurMsg，这样当B处理完后，m_pCurMsg恢复成A
// 为了实现还原m_pCurMsg，所有的消息不能直接返回，而是将m_pCurMsg还原之后再return
//
// fix bug: 20120831
//    为了解决派生类将消息CHAIN给父类时，父类在UI_BEGIN_MSG_MAP又会重新响应一次DoHook，
//    导致Hook方收到两次消息。因此增加一个ProcessMessageNoHook函数，内部将不再DoHook
//    同时派生类在CHAIN时
// ps: 20130320
//    在Ixxx层次，提供IMessage::virtualProcessMessage虚函数和xProcessMessage非虚函数，
//    在xxx层次，也提供innerVirtualProcessMessage(主要用于非IMessage派生类,IMessageInnerProxy调用)和xProcessMessage非虚函数
//    同时外部控件可直接继承于MessageProxy提供消息映射
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
        /*取出wParam,lParam，以便兼容Window消息处理中的wParam,lParam参数*/\
        WPARAM wParam = pMsg->wParam;                 \
        LPARAM lParam = pMsg->lParam;                 \
        UINT   uMsg   = pMsg->message;                \
        UINT   code   = pMsg->nCode;                  \
        UI::IMessage* pMsgFrom = pMsg->pMsgFrom;      \
        LRESULT& lResult = pMsg->lRet; /* 兼容wtl */  \
                                                      \
        this->SetCurMsg(pMsg);                        \
                                                      \
        /*消息HOOK处理*/                              \
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

// 如果只是直接调用UICHAIN_MSG_MAP_POINT_MEMBER(static_cast<baseinterface*>(m_pI##classname))则只能响应nMapID为0的情况了
// 因此增加UI_END_MSG_MAP_CHAIN_PARENT_Ixxx宏用于将消息传给父对象 
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
// 消息链传递
//
//	该宏有个缺点，就是能接收到当前的分支消息，不能传递其它分支消息。可使用UI_BEGIN_CHAIN_ALL_MSG_MAP改进宏

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
//  强行结束当前消息分支，而将消息全部传递给需要的对象
//
//	TODO: 该宏缺点，强行结束后，不知道用哪个分支再进行开始，这里默认取为0。建议将该宏放在最前面或者最后面
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
//                               UI消息定义                               //
//                                                                        //
////////////////////////////////////////////////////////////////////////////
// 这里要处理的任务包括：
//	1. 该消息是否被处理过了
//	2. 该消息的返回值
//	3. 该消息的参数解析


// int OnCreate()
// remark
//		WM_CREATE消息不是原始的Windows消息，而是UI SDK自己模拟的消息，
//		并且不关心参数，请参见：Window::Create
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
//		参数HDC，是已经做过了双缓冲处理了，可以直接使用。原始的WM_ERASEBKGND消息已被过滤了
//		这里去掉了返回值，以防调用者自己也不知道到底该返回什么（其实这里的返回值没有意思）
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
//		参数pt已从screen坐标转换成client坐标
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
#define SWP_LAYEREDWINDOW_SIZEMOVE  0x80000000   // 表示这是个分层窗口模拟的窗口大小改变
#define SWP_NOUPDATELAYOUTPOS       0x40000000   // 调用SetObjectPos时不更新布局属性

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
// 获取Render Font
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

// point相对于实际窗口
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

// 比传递WM_TIMER增加一个LPARAM
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
//	warn 这里的objID没有使用全路径式（如window/panel/loginbtn），而是直接使用了loginbtn，
//       如果程序中有同名ID对象，那么必须再自己去判断
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
//                         ATL中的消息兼容
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