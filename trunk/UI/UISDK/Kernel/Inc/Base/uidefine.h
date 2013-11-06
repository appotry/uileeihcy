#ifndef UIDEFINE_H_4C746653_0625_41a4_8DE7_F18E7116CCFF
#define UIDEFINE_H_4C746653_0625_41a4_8DE7_F18E7116CCFF

#include <map>
using std::map;

// ���뵼������
#ifdef UI_DLL_EXPORTS
#define UISDKAPI __declspec(dllexport)
#else
#define UISDKAPI __declspec(dllimport)
#endif

namespace UI
{
    
typedef map<String,String>  ATTRMAP;

// ����������
#define WND_CLASS_NAME                  _T("UI")
#define WND_ANIMATE_CLASS_NAME          _T("UI_Animate")
#define WND_POPUP_CONTROL_SHADOW_NAME   _T("UI_PopupControlShadow") // ��ϵͳ��Ӱ����
#define WND_POPUP_CONTROL_NAME          _T("UI_PopupControl")

// ���⺬�峣��
#define NDEF  -1                               // û�ж���ı���
#define AUTO  -2                               // �Զ���������width/height=auto��

#define MAX_STRING_LEN    256                  // ����һЩ�ַ�������󳤶ȣ�����Ƥ�����ƣ�ID�ȵ�

// ��ʱ�ļ���׺��
#define TEMP_FILE_EXT    _T(".tmp")

//////////////////////////////////////////////////////////////////////////
// enum

// ��Դ���Ͷ���
typedef enum
{
	UI_RESOURCE_NULL       = 0,
	UI_RESOURCE_PROJECT    = 0x0001,
	UI_RESOURCE_SKIN       = 0x0002,
	UI_RESOURCE_IMAGE      = 0x0004,
	UI_RESOURCE_FONT       = 0x0008,
	UI_RESOURCE_COLOR      = 0x0010,
	UI_RESOURCE_STYLE      = 0x0020,
	UI_RESOURCE_LAYOUT     = 0x0040,
	UI_RESOURCE_I18N       = 0x0080,
	UI_RESOURCE_LOG        = 0x0100,
	UI_RESOURCE_LOGMODULE  = 0x0200,
	UI_RESOURCE_SOURCEFILE = 0x0400,  // ��Դ�������ڵ��ļ�������UIBuilder�鿴�ļ�
}
UI_RESOURCE_TYPE;

typedef enum 
{
	GRAPHICS_RENDER_LIBRARY_TYPE_AUTO = 0,   // <- ���ݴ������;������ֲ㴰��ʹ��GDIPLUS,��ͨ����ʹ��GDI
	GRAPHICS_RENDER_LIBRARY_TYPE_GDI,
	GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS,
	GRAPHICS_RENDER_LIBRARY_TYPE_DIRECT2D
}
GRAPHICS_RENDER_LIBRARY_TYPE;

// ��־�����ʹ�÷����μ�ui.cpp
#define LOG_LEVEL(pLog,cookie,level,content)         \
	if (NULL == pLog)  return;                       \
                                                     \
	VARIANT_BOOL bRet = VARIANT_FALSE;               \
    va_list argList;                                 \
                                                     \
	pLog->TestCanLog(cookie,level, &bRet);           \
	if (VARIANT_FALSE == bRet)                       \
		return;                                      \
	                                                 \
    BSTR  bstr = NULL;                               \
	va_start(argList, content);                      \
    UI::Util::FormatV(content, argList, bstr);       \
	pLog->Log(cookie, level, bstr);                  \
	::SysFreeString(bstr);                           \
	va_end(argList); 

// 32λ�Ķ��������У���Ϊ�����֣�OBJ_TYPE ռ��8λ��Ŀǰ��Ҫֻ��3�����͡�
// CONTROL_TYPE ռ�� 16 λ��CONTROL_SUB_TYPE ռ�� 8 λ
#define  OBJ_TYPE_OFFSET         24
#define  CONTROL_TYPE_OFFSET     8
#define  CONTROL_SUB_TYPE_OFFSET 0
enum OBJ_TYPE
{
    OBJ_NULL             = 0 << OBJ_TYPE_OFFSET,
    OBJ_CONTROL          = 1 << OBJ_TYPE_OFFSET,
    OBJ_HWNDHOST         = 2 << OBJ_TYPE_OFFSET,
    OBJ_PANEL            = 3 << OBJ_TYPE_OFFSET,
    OBJ_WINDOW           = 4 << OBJ_TYPE_OFFSET,
    OBJ_COMPOUND_CONTROL = 5 << OBJ_TYPE_OFFSET,  // ���Ͽؼ�
    OBJ_COMPOUND_ITEM    = 6 << OBJ_TYPE_OFFSET,  // �����б���
    OBJ_LAYER            = 7 << OBJ_TYPE_OFFSET,  // ���ڷֲ�
};
enum CONTROL_TYPE
{
    // Extent Type
    CONTROL_BUTTON       = 1 << CONTROL_TYPE_OFFSET,
    CONTROL_GROUPBOX     = 2 << CONTROL_TYPE_OFFSET,
    CONTROL_CHECKBUTTON  = 3 << CONTROL_TYPE_OFFSET,
    CONTROL_RADIOBUTTON  = 4 << CONTROL_TYPE_OFFSET,
    CONTROL_HYPERLINK    = 5 << CONTROL_TYPE_OFFSET,
    CONTROL_LABEL        = 6 << CONTROL_TYPE_OFFSET,
    CONTROL_PICTURE      = 7 << CONTROL_TYPE_OFFSET,
    CONTROL_EDIT         = 8 << CONTROL_TYPE_OFFSET,
    CONTROL_PROGRESSCTRL = 9 << CONTROL_TYPE_OFFSET,
    CONTROL_SLIDERCTRL   = 10 << CONTROL_TYPE_OFFSET,
    CONTROL_LISTBOX      = 11 << CONTROL_TYPE_OFFSET,
    CONTROL_POPUPLISTBOX = 12 << CONTROL_TYPE_OFFSET,
    CONTROL_HEADERCTRL   = 13 << CONTROL_TYPE_OFFSET,
    CONTROL_LISTVIEWCTRL = 14 << CONTROL_TYPE_OFFSET,
    CONTROL_HSCROLLBAR   = 15 << CONTROL_TYPE_OFFSET,
    CONTROL_VSCROLLBAR   = 16 << CONTROL_TYPE_OFFSET,
    CONTROL_COMBOBOX     = 17 << CONTROL_TYPE_OFFSET,
    CONTROL_RICHEDIT     = 18 << CONTROL_TYPE_OFFSET,
    CONTROL_LEDCTRL      = 19 << CONTROL_TYPE_OFFSET,
    CONTROL_GIFCTRL      = 20 << CONTROL_TYPE_OFFSET,
    CONTROL_MENU         = 21 << CONTROL_TYPE_OFFSET,
//    CONTROL_TOOLTIPWINDOW= 22 << CONTROL_TYPE_OFFSET, -> WINDOW_TOOLTIP
    CONTROL_TREEVIEWCTRL = 23 << CONTROL_TYPE_OFFSET,
    CONTROL_LISTTREEVIEWCTRL = 24 << CONTROL_TYPE_OFFSET,
    CONTROL_FLASH        = 25 << CONTROL_TYPE_OFFSET,
    CONTROL_IE           = 26 << CONTROL_TYPE_OFFSET,
    CONTROL_LISTCTRLITEM = 27 << CONTROL_TYPE_OFFSET, 
    CONTROL_TABCTRL      = 28 << CONTROL_TYPE_OFFSET, 
    CONTROL_SPLITTERBAR  = 29 << CONTROL_TYPE_OFFSET,
};

enum WINDOW_TYPE
{
    WINDOW_TOOLTIP = 1 << CONTROL_TYPE_OFFSET
};

enum PANEL_TYPE
{
    PANEL_SCROLL           = 1 << CONTROL_TYPE_OFFSET,
    PANEL_LISTCTRLITEMROOT = 2 << CONTROL_TYPE_OFFSET,
};

enum LAYER_TYPE
{
    LAYER_DIRECT   = 1 << CONTROL_TYPE_OFFSET,
    LAYER_BUFFER   = 2 << CONTROL_TYPE_OFFSET,
};

inline OBJ_TYPE GetObjectMajorType(int type)  { return  (OBJ_TYPE)((type)&0xFF000000);  } 
inline CONTROL_TYPE GetObjectExtentType(int type) { return  (CONTROL_TYPE)((type)&0x00FFFF00);  }  

#pragma region // Object, RenderBase ��չ��

#define UI_DECLARE_OBJECT3(className, type, category)  \
    UI_DECLARE_OBJECT_INFO3(className, type, category) \
    UI_DECLARE_QUERY_INTERFACE(className)

#define UI_DECLARE_OBJECT(className, type)  \
    UI_DECLARE_OBJECT_INFO(className, type) \
    UI_DECLARE_QUERY_INTERFACE(className)

#define UI_DECLARE_OBJECT_INFO(className, type)   \
    static TCHAR*  GetXmlName() { return _T(#className);  } \
    static TCHAR*  GetCategory() { return _T("Default"); }  \
    static UINT    GetObjType() { return type; }            \
    void  OnGetObjectInfo(TCHAR** ppszName, int* pObjType)  \
    {                                                       \
        if (ppszName)*ppszName = _T(#className);            \
        if (pObjType)*pObjType = type;                      \
    }                                                       \

#define UI_DECLARE_OBJECT_INFO3(className, type, category)  \
    static TCHAR*  GetXmlName() { return _T(#className);  } \
    static TCHAR*  GetCategory() { return category; }       \
    static UINT    GetObjType() { return type; }            \
    void  OnGetObjectInfo(TCHAR** ppszName, int* pObjType)  \
    {                                                       \
        if (ppszName)*ppszName = _T(#className);            \
        if (pObjType)*pObjType = type;                      \
    }     

#define UI_DECLARE_QUERY_INTERFACE(className)               \
    void*  QueryInterface(const IID* pIID)                  \
    {                                                       \
        if (IsEqualIID(*pIID, uiiidof(I##className)))       \
        {                                                   \
            return (void*)m_pI##className;                  \
        }                                                   \
        SetMsgHandled(FALSE);                               \
        return NULL;                                        \
    }                                                       \

#define UI_DECLARE_RENDERBASE(className, xml, rendertype)   \
    static const TCHAR*  GetXmlName() { return xml; }       \
    static int  GetRenderType() { return rendertype; }      \
    static int  GetControlType() { return -1; }             \
    static int  GetControlSubType() { return -1; }          \
    UI_DECLARE_QUERY_INTERFACE(className)                   \
    typedef UI::UIObjCreator<I##className> _CreatorClass;


// ���궨����Ҫ������theme���͵�renderbase��Ҫ���ݿؼ����ͽ��д���
#define UI_DECLARE_RENDERBASE2(className, xml, rendertype, controltype, subtype) \
    static const TCHAR*  GetXmlName() { return xml; }       \
    static int  GetRenderType() { return rendertype; }      \
    static int  GetControlType() { return controltype; }    \
    static int  GetControlSubType() { return subtype; }     \
    typedef UI::UIObjCreator<className> _CreatorClass;  // ע������û����I##className����ΪĬ������themeû��Ixxx����


#define UI_DECLARE_TEXTRENDERBASE(className, xml, rendertype) \
    static const TCHAR*  GetXmlName() { return xml; }       \
    static int  GetRenderType() { return rendertype; }      \
    static int  GetControlType() { return -1; }             \
    static int  GetControlSubType() { return -1; }          \
    UI_DECLARE_QUERY_INTERFACE(className)                   \
    typedef UI::UIObjCreator<I##className> _CreatorClass;

#define UI_DECLARE_TEXTRENDERBASE2(className, xml, rendertype, controltype, subtype) \
    static const TCHAR*  GetXmlName() { return xml; }       \
    static int  GetRenderType() { return rendertype; }      \
    static int  GetControlType() { return controltype; }    \
    static int  GetControlSubType() { return subtype; }     \
    typedef UI::UIObjCreator<className> _CreatorClass;   // ע������û����I##className����ΪĬ������themeû��Ixxx����

#define REGISTER_UI_OBJECT(pUIApp, classname)   \
        pUIApp->RegisterUIObjectCreateData(     \
                classname::GetXmlName(),        \
                classname::GetCategory(),       \
                classname::GetObjType(),        \
                uiclsidof(classname),           \
                (UI::funcUICreateInstancePtr)I##classname::_CreatorClass::UICreateInstance);

#define REGISTER_UI_RENDERBASE(pUIApp, classname) \
        pUIApp->RegisterUIRenderBaseCreateData(   \
                classname::GetXmlName(),          \
                classname::GetRenderType(),       \
                classname::GetControlType(),      \
                classname::GetControlSubType(),   \
                (UI::funcUICreateRenderBasePtr)classname::_CreatorClass::UICreateInstance2);

#define REGISTER_UI_TEXTRENDERBASE(pUIApp, classname) \
        pUIApp->RegisterUITextRenderBaseCreateData( \
                classname::GetXmlName(),          \
                classname::GetRenderType(),       \
                classname::GetControlType(),      \
                classname::GetControlSubType(),   \
                (UI::funcUICreateTextRenderBasePtr)classname::_CreatorClass::UICreateInstance2);

#define REGISTER_UI_LAYOUT(pUIApp, layoutname, layouttype, func) \
        pUIApp->RegisterLayoutCreateData(layoutname, layouttype, func);

#pragma endregion

// object��state bit��״̬λ
#define OSB_UNVISIBLE     0x0001        // �Լ��Ƿ�ɼ�
#define OSB_COLLAPSED     0x0002        // listitemΪ������������ʾ�ӽ�㣨TODO:�Ժ��������չΪ���ز��Ҳ�ռ�ò���?��
#define OSB_DISABLE       0x0004        // �Լ��Ƿ����

#define OSB_PRESS         0x0010
#define OSB_HOVER         0x0020
#define OSB_FORCEPRESS    0x0040
#define OSB_FORCEHOVER    0x0080         
// #define OSB_READONLY      0x0100     // �����ɸ����ؼ��Լ�ʵ�ָ���һЩ��
#define OSB_FOCUS         0x0200
#define OSB_DEFAULT       0x0400
#define OSB_SELECTED      0x0800         // ��ѡ��
#define OSB_CHECKED       0x1000
#define OSB_RADIOCHECKED  0x2000

#define WSB_ACTIVE        0x1000         // ����Ϊactive

// STYLE��ʽռλ˵��
//   8.7.6.5:  OBJECT ͨ����ʽ
//	 4.3.2.1:  CONTROL/WINDOWͨ����ʽ
// STYLEEX �ؼ��Լ�����ʽ

// STYLEEX���ھ���ؼ���ʽ��ͬʱ��STYLEEX�������λ���ڱ�ʶ�ؼ����ͣ����簴ť����;
#define GETCONTROLSUBTYPE(nStyleEx)  (nStyleEx & 0x000000FF)
#define MAKECONTROLSUBTYPE(nStyleEx, subStyle)  ((nStyleEx & 0xFFFFFF00) | subStyle)

// object style
#define OBJECT_STYLE_TRANSPARENT        0x00010000    // �����Ƿ�͸������
#define OBJECT_STYLE_HSCROLL            0x00020000    // ����������
#define OBJECT_STYLE_VSCROLL            0x00040000    // �����������
#define OBJECT_STYLE_NCOBJ              0x00080000    // �ö���λ�ڸ�����ķǿͻ�����������ƫ��
#define OBJECT_STYLE_REJECT_MOUSE_MSG_ALL   0x00100000    // �ö�������Լ����Ӷ��󲻽��������Ϣ����label
#define OBJECT_STYLE_REJECT_MOUSE_MSG_SELF  0x00200000    // ���ö��󲻽��������Ϣ�����Ӷ����ǽ��������Ϣ�ģ���panel
#define OBJECT_STYLE_RECEIVE_DRAGDROPEVENT  0x00400000    // �ö��������ק��Ϣ
#define OBJECT_STYLE_ENABLE_IME         0x00800000    // �ö�����Ҫ�������뷨����EDIT,RICHEDIT
#define OBJECT_STYLE_ZINDEX_OVERLAP     0x01000000    // �ö�����ͬһ����������������ص��ˣ�����ˢ��ʱ�쳣�����ϸ���ʽ�󣬽�ֱ��ˢ�¸�����

// window style
#define WINDOW_STYLE_DESTROYED          0x00001000    // ��ʾ�ô����Ѿ���������(WM_NCDESTROY)�����ڴ���OnFinalMessage
#define WINDOW_STYLE_ATTACH             0x00002000    // ��ʾ�ô�����attach�ģ��������������ⲿ������

// control style
#define CONTROL_STYLE_TABSTOP           0x00001000    // �Ƿ����TAB��ݼ��Ľ���
#define CONTROL_STYLE_GROUP             0x00002000    // �Ƿ���һ������Ŀ�ʼ
#define CONTROL_STYLE_OWNERDRAW         0x00004000    // �Ƿ��Ի�

// listctrl style(ex)
#define LISTCTRLBASE_STYLE_SORT_ASCEND        0x0100    // ��������
#define LISTCTRLBASE_STYLE_SORT_DESCEND       0x0200    // ��������
#define LISTCTRLBASE_STYLE_MULTIPLE_SEL       0x0400    // �Ƿ�֧�ֶ�ѡ
#define LISTCTRLBASE_STYLE_POPUPLISTBOX       0x0800    // ����ʽlistboxʽ����������1.��갴��ʱ��ѡ����굯��ʱѡ�� 2. ��û��hoverʱ��ʾsel 3. ����hoverʱ��ʾhover 4. ����Ƴ��ؼ�ʱ��ȡ��hover
#define LISTCTRLBASE_STYLE_MENU               0x1000    // �˵�ʽ����������1. ��굯��ʱ���������pressitem�ϣ��򲻴���click

// ��ɫ
#define MIN_HUE_VALUE           0              // ɫ��  HLS��Χ����
#define MAX_HUE_VALUE           240
#define MIN_LUMINANCE_VALUE     0              // ����
#define MAX_LUMINANCE_VALUE     240            
#define MIN_SATURATION_VALUE    0              // ���Ͷ�
#define MAX_SATURATION_VALUE    240            

#define CHANGE_SKIN_HLS_FLAG_H             1
#define CHANGE_SKIN_HLS_FLAG_L             2
#define CHANGE_SKIN_HLS_FLAG_S             4
#define CHANGE_SKIN_HLS_FLAG_HLS           (CHANGE_SKIN_HLS_FLAG_H|CHANGE_SKIN_HLS_FLAG_L|CHANGE_SKIN_HLS_FLAG_S)
#define CHANGE_SKIN_HLS_FALG_REPLACE_MODE  8 // ��ɫģʽ


// UI_WM_NOTIFY ��Ϣ��lParam��ʹ�õĽṹ��
struct UINMHDR
{
    int reverse;
};


// UI_WM_DROPTARGETEVENT ��Ϣ����
enum DROPTARGETEVENT_TYPE
{
    _DragEnter,
    _DragOver,
    _DragLeave,
    _Drop,
};
struct DROPTARGETEVENT_DATA
{
    IDataObject*  pDataObj;
    DWORD         grfKeyState;
    POINTL        pt;
    DWORD*        pdwEffect;
};



}


#pragma region // Ixxx�ӿ���ʵ�ֺ�

// ����һ���ӿ���Ļ�����Ա�ͷ�����������������Ϊprotected��Ա��ǿ���ⲿʹ��
// SAFE_DELETE_Ixxx���ͷŸ�����
#define UI_DECLARE_Ixxx_INTERFACE(interfacename, classname)  \
    UI_DECLARE_Ixxx_INTERFACE_Construct(interfacename, classname) \
    UI_DECLARE_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    UI_DECLARE_Ixxx_INTERFACE_CreateImpl(interfacename, classname)

#define UI_DECLARE_Ixxx_INTERFACE_Construct(interfacename, classname) \
public:                                    \
    typedef UI::UIObjCreator<interfacename> _CreatorClass; \
    static void  CreateInstance(UI::IUIApplication* p, interfacename** pp); \
    interfacename();                       \
protected:                                 \
    ~interfacename();                      \
protected:                                 \
    classname*  m_p##classname##Impl;      \
    BOOL  m_bCreate##classname##Impl;      \
public:

// ��UIObject����Ҳ����pimplģʽ(���ᱻ�̳�)��
#define UI_DECLARE_Ixxx_INTERFACE_Construct2(interfacename, classname) \
public:                                    \
    interfacename();                       \
    void  delete_this();                   \
protected:                                 \
    ~interfacename();                      \
protected:                                 \
    classname*  m_p##classname##Impl;      \
    BOOL  m_bCreate##classname##Impl;      \
public:

#define UI_DECLARE_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    BOOL  nvProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook); \
private:                                   \
    BOOL  virtualProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook); \
public:

#define UI_DECLARE_Ixxx_INTERFACE_CreateImpl(interfacename, classname) \
    void  CreateImpl(classname* p=NULL);   \
    classname*  GetImpl();                 \

//
// ʵ��һ���ӿ���
// ע��
//   1. ��Щ�ӿ�Ҫ�ڹ��캯���г�ʼ����ĳ�Ա��������long m_lRef����˽����캯���������
//   2. ������IButtonBase���ֽӿڣ�����ButtonBaseû��ֱ�Ӵ�Control��̳У������Ҫ��
//      IButtonBase::CreateImpl�е�������һ��IControl::CreateImpl(NULL)����˽�CreateImpl�������
//
#define UI_IMPLEMENT_Ixxx_INTERFACE(interfacename, classname, basename)  \
    UI_IMPLEMENT_Ixxx_INTERFACE_Construct(interfacename, classname)      \
    UI_IMPLEMENT_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(interfacename, classname, basename)

#define UI_IMPLEMENT_Ixxx_INTERFACE2(interfacename, classname, baseinterface)  \
    UI_IMPLEMENT_Ixxx_INTERFACE_Construct(interfacename, classname)      \
    UI_IMPLEMENT_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl2(interfacename, classname, baseinterface)

#define UI_IMPLEMENT_Ixxx_INTERFACE_Construct(interfacename, classname) \
    interfacename::interfacename()                          \
    {                                                       \
        m_p##classname##Impl = NULL;                        \
        m_bCreate##classname##Impl = FALSE;                 \
    }                                                       \
    interfacename::~interfacename()                         \
    {                                                       \
        if (m_bCreate##classname##Impl)                     \
            SAFE_DELETE(m_p##classname##Impl);              \
    }                                                       \
    void  interfacename::CreateInstance(UI::IUIApplication* p, interfacename** pp) \
    {                                                       \
        interfacename::_CreatorClass::UICreateInstance(p, pp); \
    }

#define UI_IMPLEMENT_Ixxx_INTERFACE_Construct2(interfacename, classname) \
    interfacename::interfacename()                          \
    {                                                       \
        m_p##classname##Impl = NULL;                        \
        m_bCreate##classname##Impl = FALSE;                 \
    }                                                       \
    interfacename::~interfacename()                         \
    {                                                       \
        if (m_bCreate##classname##Impl)                     \
            SAFE_DELETE(m_p##classname##Impl);              \
    }                                                       \
    void  interfacename::delete_this() { delete this; }

#define UI_IMPLEMENT_Ixxx_INTERFACE_ProcessMessage(interfacename, classname) \
    BOOL  interfacename::nvProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook) \
    {                                                       \
        return m_p##classname##Impl->nvProcessMessage(pMsg, nMsgMapID, bDoHook); \
    }                                                       \
    /*virtual*/ BOOL  interfacename::virtualProcessMessage(UI::UIMSG* pMsg, int nMsgMapID, bool bDoHook) \
    {                                                       \
        return m_p##classname##Impl->nvProcessMessage(pMsg, nMsgMapID, bDoHook); \
    }
#define UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl(interfacename,classname,baseclass) \
    void  interfacename::CreateImpl(classname* p)           \
    {                                                       \
        if (p)                                              \
        {                                                   \
            m_p##classname##Impl = p;                       \
        }                                                   \
        else                                                \
        {                                                   \
            m_p##classname##Impl = new classname;           \
            m_bCreate##classname##Impl = TRUE;              \
        }                                                   \
        m_p##classname##Impl->SetI##classname(this);        \
        I##baseclass::CreateImpl(static_cast<baseclass*>(m_p##classname##Impl)); \
    }                                                       \
    classname*  interfacename::GetImpl()                    \
    {                                                       \
        return m_p##classname##Impl;                        \
    }

#define UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl2(interfacename,classname,baseinterface) \
    void  interfacename::CreateImpl(classname* p)           \
    {                                                       \
        baseinterface::CreateImpl(NULL);                    \
        if (p)                                              \
        {                                                   \
            m_p##classname##Impl = p;                       \
        }                                                   \
        else                                                \
        {                                                   \
            m_p##classname##Impl = new classname;           \
            m_bCreate##classname##Impl = TRUE;              \
        }                                                   \
        m_p##classname##Impl->SetI##classname(this);        \
    }                                                       \
    classname*  interfacename::GetImpl()                    \
    {                                                       \
        return m_p##classname##Impl;                        \
    }

// û�и����������ʹ��
#define UI_IMPLEMENT_Ixxx_INTERFACE_CreateImpl3(interfacename,classname) \
    void  interfacename::CreateImpl(classname* p)           \
    {                                                       \
        if (p)                                              \
        {                                                   \
            m_p##classname##Impl = p;                       \
        }                                                   \
        else                                                \
        {                                                   \
            m_p##classname##Impl = new classname;           \
            m_bCreate##classname##Impl = TRUE;              \
        }                                                   \
        m_p##classname##Impl->SetI##classname(this);        \
    }                                                       \
    classname*  interfacename::GetImpl()                    \
    {                                                       \
        return m_p##classname##Impl;                        \
    }
#pragma endregion

#define uiiidof(xxx)   IID_UI_##xxx
#define uiclsidof(xxx) CLSID_UI_##xxx



#endif // UIDEFINE_H_4C746653_0625_41a4_8DE7_F18E7116CCFF