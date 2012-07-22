#pragma once

// ���뵼������
#ifdef UI_DLL_EXPORTS
#define UIAPI __declspec(dllexport)
#else
#define UIAPI __declspec(dllimport)
#endif

#ifdef _UNICODE
	typedef wstring String;
#else
	typedef string  String;
#endif

typedef RECT  REGION4;
typedef CRect CRegion4;


// �������
typedef void *HSKIN;

// ����������
#define WND_CLASS_NAME   _T("UI")
#define WND_ANIMATE_CLASS_NAME _T("UI_Animate")

// ��ʱ�ļ���׺��
#define TEMP_FILE_EXT    _T(".tmp")

// ���⺬�峣��
#define NDEF  -1                               // û�ж���ı���
#define AUTO  -2                               // �Զ���������width/height=auto��

#define MAX_STRING_LEN    256                  // ����һЩ�ַ�������󳤶ȣ�����Ƥ�����ƣ�ID�ȵ�

// ������д����
#define SAFE_DELETE(p) \
	{\
		if( NULL != p ) \
		{ \
			delete p; \
			p = NULL; \
		}\
	}

#define SAFE_ARRAY_DELETE(p) \
	{\
		if( NULL != p ) \
		{ \
			delete[] p; \
			p = NULL; \
		} \
	}

#define SAFE_RELEASE(p) \
	{\
		if( NULL != p ) \
		{ \
			p->Release(); \
			p = NULL; \
		}\
	}

#define SAFE_DELETE_GDIOBJECT(h) \
	{\
		if( NULL != h )\
		{\
			::DeleteObject(h);\
			h = NULL;\
		}\
	}

//////////////////////////////////////////////////////////////////////////
// enum

// ��Դ���Ͷ���
typedef enum
{
	UI_RESOURCE_NULL,
	UI_RESOURCE_PROJECT,
	UI_RESOURCE_SKIN,
	UI_RESOURCE_IMAGE,
	UI_RESOURCE_FONT,
	UI_RESOURCE_COLOR,
	UI_RESOURCE_STYLE,
	UI_RESOURCE_LAYOUT,
	UI_RESOURCE_I18N,
	UI_RESOURCE_LOG,
	UI_RESOURCE_LOGMODULE
}
UI_RESOURCE_TYPE;

//////////////////////////////////////////////////////////////////////////
// helper (#include <windowsx.h>���ɣ�������ֻ��Ҫ���)

#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

// ��־�����ʹ�÷����μ�ui.cpp
#define LOG_LEVEL(pLog,module,level,content)         \
	if( NULL == pLog )                               \
		return;                                      \
	if( 0 == pLog->TestCanLog(module,level) )        \
		return;                                      \
	va_list argList;                                 \
	va_start(argList, content );                     \
	BSTR  bstr;                                      \
	Util::FormatV(content, argList, bstr);           \
	pLog->Log( module, level, bstr );                \
	::SysFreeString(bstr);                           \
	va_end(argList); 

/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//                                UI�Զ�����Ϣ����                             //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//                                XML�е�tag����                               //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#define XML_SEPERATOR             _T(',')            // xml�еķָ��������ʾ��ɫ������
#define XML_PATH_SEPERATOR        _T('/')            // xml�е�·���ָ���������ʽ·�����Ӷ���·����
#define XML_MULTI_SEPERATOR       _T(';')            // xml�еķָ���,��ָ�һ���ؼ��Ķ��style.
#define XML_HEADER                _T("<?xml version=\"1.0\" encoding=\"utf-8\" ?>")
#define XML_PROJECT_XMLFILE_EXT   _T(".uiproj")       // ���������ļ��ĺ�׺��

#define XML_UI               _T("ui")            //root
#define XML_XML              _T("xml")
#define XML_LOG              _T("log")
#define XML_IMG              _T("img")
#define XML_FONT             _T("font")
#define XML_COLOR            _T("color")
#define XML_LAYOUT           _T("layout")
#define XML_STYLE            _T("style")
#define XML_I18N             _T("i18n")
#define XML_PROJECT          _T("project")

#define XML_ITEM             _T("item")
#define XML_ID               _T("id")
#define XML_TEXT             _T("text")
#define XML_TOOLTIP          _T("tooltip")

#define XML_FONT_FACENAME    _T("facename")
#define XML_FONT_HEIGHT      _T("size")
#define XML_FONT_ORIENTATION _T("orientation") 
#define XML_FONT_BOLD        _T("bold")
#define XML_FONT_ITALIC      _T("italic")
#define XML_FONT_UNDERLINE   _T("underline")
#define XML_FONT_STRIKEOUT   _T("strikeout")  

#define XML_IMAGE_USESKINHUE _T("useskinhue")    // �ı�Ƥ��ɫ��ʱ����ͼƬ�Ƿ���� 0/1


// ��������
#define XML_PROJECT_NAME       _T("projectname")     // ��������
#define XML_PROJECT_SELECT     _T("select")          // ��ǰ����ʹ�����Ŀ�Ƥ������
#define XML_PROJECT_SKIN       _T("skin")            // ������һ��Ƥ��
#define XML_PROJECT_SKIN_NAME  _T("name")            // Ƥ������
#define XML_PROJECT_SKIN_PATH  _T("path")            // Ƥ��ui.xml����·��


//
// layout.xml
//

// Object��������

//#define XML_ID                   _T("id")            // ÿ������ı�ʶ��
#define XML_STYLECLASS           _T("styleclass")    // �������ʽ����
#define XML_WIDTH                _T("width")         // ����Ŀ��ȣ�����Ϊ auto | ��ֵ
#define XML_HEIGHT               _T("height")        // ����ĸ߶ȣ�����Ϊ auto | ��ֵ
#define XML_MARGIN               _T("margin")        // �������߾�
#define XML_PADDING              _T("padding")       // ������ڱ߾�
#define XML_MINWIDTH             _T("minwidht")      // �������С����
#define XML_MINHEIGHT            _T("minheight")     // �������С�߶�
#define XML_MAXWIDTH             _T("maxwidth")      // �����������
#define XML_MAXHEIGHT            _T("maxheight")     // ��������߶�
#define XML_AUTO                 _T("auto")          // ����Ŀ��͸�����Ϊ���ڲ����ݾ���
#define XML_ASTERISK             _T("*")             // ����Ŀ��͸�����Ϊƽ��

// EraseBkgnd

#define XML_BACKGND_IS_TRANSPARENT       _T("istransparent")          // �Ƿ���Ҫ����͸��������ˢ�ϸ�����ı�����0/1

#define XML_RENDER_TYPE                   _T("render.type")           // ������Ⱦ����
#define XML_RENDER_TYPE_COLOR             _T("color")                 // ��ɫ��� Color
#define XML_RENDER_TYPE_GRADIENT_H        _T("gradienth")             // ���򽥱� Color + Color
#define XML_RENDER_TYPE_GRADIENT_V        _T("gradientv")             // ���򽥱� Color + Color
#define XML_RENDER_TYPE_IMAGE_SIMPLE      _T("imagesimple")           // (0,0)λ�ÿ�ʼ��ͼImage + Color
#define XML_RENDER_TYPE_IMAGE_TILE        _T("imagetile")             // (0,0)λ�ÿ�ʼƽ��Image
#define XML_RENDER_TYPE_IMAGE_STRETCH     _T("imagestretch")          // ����ģʽImage + Image9Region*
#define XML_RENDER_TYPE_COLORLIST         _T("colorlist")             // office 2003������ʽ��ť
#define XML_RENDER_TYPE_IMAGELIST         _T("imagelist")             // ͼƬ�б�
#define XML_RENDER_TYPE_IMAGELISTSTRETCH  _T("imageliststretch")      // ͼƬ�б��������
#define XML_RENDER_TYPE_THEME             _T("theme")                 // ϵͳ���ⰴť

#define XML_RENDER_COLOR                  _T("render.color")          // �����ɫ
#define XML_RENDER_BORDERCOLOR            _T("render.bordercolor")    // �߿���ɫ
#define XML_RENDER_COLORFROM              _T("render.colorfrom")      // ������ʼɫ
#define XML_RENDER_COLORTO                _T("render.colorto")        // �����յ�ɫ
#define XML_RENDER_IMAGE                  _T("render.image")          // ����ͼƬ
#define XML_RENDER_IMAGE9REGION           _T("render.9region")        // ����������ʽ
#define XML_RENDER_COLORLIST_COUNT        _T("render.colorlist.count")  // ��ɫ�б�������
#define XML_RENDER_IMAGELIST_COUNT        _T("render.imagelist.count")  // ͼƬ�б��е�ͼƬ����
#define XML_RENDER_IMAGELIST_LAYOUT       _T("render.imagelist.layout") // ͼƬ���з���(Ĭ��Ϊ����)
#define XML_RENDER_IMAGELIST_LAYOUT_H     _T("h")                     // ͼƬ��������
#define XML_RENDER_IMAGELIST_LAYOUT_V     _T("v")                     // ͼƬ��������


#define XML_BACKGND_RENDER_PRIFIX         _T("bkgnd.")                // ������Ⱦ����ǰ׺����bkgnd.render.type="" bkgnd.render.image=""
#define XML_FOREGND_RENDER_PRIFIX         _T("foregnd.")               // ǰ��
#define XML_ICON_RENDER_PRIFIX            _T("icon.")                 // ͼ��������Ⱦǰ׺

enum RENDER_TYPE
{
	RENDER_TYPE_NULL,             
	RENDER_TYPE_COLOR,        
	RENDER_TYPE_GRADIENTH,    
	RENDER_TYPE_GRADIENTV,    
	RENDER_TYPE_IMAGESIMPLE,  
	RENDER_TYPE_IMAGETILE,    
	RENDER_TYPE_IMAGESTRETCH,
	RENDER_TYPE_COLORLIST,
	RENDER_TYPE_IMAGELIST,
	RENDER_TYPE_IMAGELISTSTRETCH,
	RENDER_TYPE_THEME
};

enum IMAGELIST_LAYOUT_TYPE
{
	IMAGELIST_LAYOUT_TYPE_H,
	IMAGELIST_LAYOUT_TYPE_V
};

#define XML_TEXTRENDER_TYPE                 _T("textrender.type")       // ����������Ⱦ����
#define XML_TEXTRENDER_TYPE_NORMAL          _T("normal")                // color + font
#define XML_TEXTRENDER_TYPE_COLORLIST       _T("colorlist")             // color list + font
#define XML_TEXTRENDER_TYPE_FONTLIST        _T("fontlist")              // font list + color
#define XML_TEXTRENDER_TYPE_FONTCOLORLIST   _T("fontcolorlist")         // font list + color list

#define XML_TEXTRENDER_COLOR                _T("textrender.color")      // ������ɫ
#define XML_TEXTRENDER_FONT                 _T("textrender.font")       // ����
#define XML_TEXTRENDER_FONTCOLORLIST_COUNT  _T("textrender.fontcolorlist.count")  // ���塢��ɫ����

enum TEXTRENDER_TYPE
{
	TEXTRENDER_TYPE_NULL,
	TEXTRENDER_TYPE_NORMAL,
	TEXTRENDER_TYPE_COLORLIST,
	TEXTRENDER_TYPE_FONTLIST,
	TEXTRENDER_TYPE_FONTCOLORLIST
};


// #define  XML_BACK_RENDER_COLOR_FLAG                  _T("bkrenderflag")       // ��Ҫ������Щ״̬�ı���
// #define  XML_BACK_RENDER_COLOR_DRAW_NORMAL   0x0001
// #define  XML_BACK_RENDER_COLOR_DRAW_HOVER    0x0002
// #define  XML_BACK_RENDER_COLOR_DRAW_PRESS    0x0004
// #define  XML_BACK_RENDER_COLOR_DRAW_DISALBE  0x0008
// #define  XML_BACK_RENDER_COLOR_DRAW_2STATE   (XML_BACK_RENDER_COLOR_DRAW_HOVER|XML_BACK_RENDER_COLOR_DRAW_PRESS)
// #define  XML_BACK_RENDER_COLOR_DRAW_4STATE   (XML_BACK_RENDER_COLOR_DRAW_NORMAL|XML_BACK_RENDER_COLOR_DRAW_HOVER|XML_BACK_RENDER_COLOR_DRAW_PRESS|XML_BACK_RENDER_COLOR_DRAW_DISALBE)   

// #define  XML_BACK_RENDER_COLOR_BK_NORMAL_COLOR       _T("bknormalcol")      // ������ɫ
// #define  XML_BACK_RENDER_COLOR_BK_HVOER_COLOR        _T("bkhovercol")       // ������ɫ
// #define  XML_BACK_RENDER_COLOR_BK_PRESS_COLOR        _T("bkpresscol")       // ������ɫ
// #define  XML_BACK_RENDER_COLOR_BK_DISABLE_COLOR      _T("bkdisablecol")     // ������ɫ
// 
// #define  XML_BACK_RENDER_COLOR_BORDER_NORMAL_COLOR   _T("bordernormalcol")  // �߿���ɫ
// #define  XML_BACK_RENDER_COLOR_BORDER_HVOER_COLOR    _T("borderpresscol")   // �߿���ɫ
// #define  XML_BACK_RENDER_COLOR_BORDER_PRESS_COLOR    _T("borderhovercol")   // �߿���ɫ
// #define  XML_BACK_RENDER_COLOR_BORDER_DISABLE_COLOR  _T("borderdisablecol") // �߿���ɫ

// window rgn
#define  XML_WINDOW_TRANSPARENT_TYPE                    _T("transparent.type")
#define  XML_WINDOW_TRANSPARENT_PART                    _T("transparent.part") // ����͸����������
#define  XML_WINDOW_TRANSPARENT_PART_9REGION            _T("transparent.part.9region") // ����͸�����������9Region���ͣ���Ϊ4��8��9���֣�

#define  XML_WINDOW_TRANSPARENT_TYPE_LAYERED            _T("layered")          // ʹ�÷ֲ㴰�ڽ���͸��
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR          _T("maskcolor")        // ʹ����ɫֵ
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA          _T("maskalpha")        // ʹ��͸��ֵ

#define  XML_WINDOW_TRANSPARENT_PART_NULL               _T("")                 // RECT�������ô������� [Ĭ��]
#define  XML_WINDOW_TRANSPARENT_PART_4_CORNER           _T("4corner")          // �ӱ���ͼ�������ĸ�������״
#define  XML_WINDOW_TRANSPARENT_PART_8_BORDER           _T("8border")          // �ӱ���ͼ�����ñ߿���Ҫ����
#define  XML_WINDOW_TRANSPARENT_PART_FULL               _T("full")             // ��Ҫ���������ڷ�Χ������״����
#define  XML_WINDOW_TRANSPARENT_PART_CUSTOM_CORNER      _T("customcorner")     // �Զ����ĸ�����

#define  XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR_VALUE    _T("transparent.maskcolor")  // ����͸��ɫ
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA_VALUE    _T("transparent.maskalpha")  // ��������С��alphaֵͨ���Ŀٵ�

enum WINDOW_TRANSPARENT_PART_TYPE
{
	WINDOW_TRANSPARENT_PART_NULL,
	WINDOW_TRANSPARENT_PART_4_CORNER,
	WINDOW_TRANSPARENT_PART_8_BORDER,
	WINDOW_TRANSPARENT_PART_FULL,
	WINDOW_TRANSPARENT_PART_CUSTOM_CORNER
};

#define WINDOW_TRANSPARENT_TYPE_NULL       0
#define WINDOW_TRANSPARENT_TYPE_LAYERED    1    
#define WINDOW_TRANSPARENT_TYPE_MASKCOLOR  2
#define WINDOW_TRANSPARENT_TYPE_MASKALPHA  4

// ����ͨ������
#define XML_CURSOR                           _T("cursor")       // ����������ʽ
#define XML_TOOLTIP                          _T("tooltip")      // �������ʾ��Ϣ���Զ������ʾ������Ҫ������������
#define XML_CONTROL_STYLE_GROUP              _T("group")
#define XML_CONTROL_STYLE_TABABLE            _T("tabable")

// window
// #define  XML_WINDOW_BK_TYPE                    _T("bktype")     // ���ڱ�����������
// #define  XML_WINDOW_BK_TYPE_COLOR              _T("color")      // ���ڱ�����������--��ɫ����ˢ����
// #define  XML_WINDOW_BK_TYPE_TILE               _T("tile")       // ���ڱ�����������--ƽ��
// #define  XML_WINDOW_BK_TYPE_SIMPLE             _T("simple")     // ���ڱ�����������--�򵥣�(0,0)bitblt
// #define  XML_WINDOW_BK_TYPE_CENTER             _T("center")     // ���ڱ�����������--����
// #define  XML_WINDOW_BK_TYPE_STRETCH            _T("stretch")    // ���ڱ�����������--����
// #define  XML_WINDOW_BK_TYPE_FRAME              _T("frame")      // ���ڱ�����������--�߿�
// #define  XML_WINDOW_BK_IMAGE                   _T("bkimage")    // ���ڱ���ͼƬ����
// #define  XML_WINDOW_BK_TYPE_COLOR_PEN          _T("bkpencol")   // bktype=color�����ڱ߿�ɫ
// #define  XML_WINDOW_BK_TYPE_COLOR_BRUSH        _T("bkbrushcol") // bktype=color�����ڱ���ɫ
// #define  XML_WINDOW_BK_TYPE_FRAME_9REGION      _T("bk9region")  // bktype=frame�������иΧ


// hwnd host
#define XML_HWNDHOST_CONTROL_ID   _T("controlid")     // �ؼ�ID��ֵ������

//#define XML_LAYOUT               _T("layout")        // ����Ĳ�������
#define XML_LAYOUT_STACK         _T("stack")         // �������ͣ�stack
#define XML_LAYOUT_NULL          _T("null")          // �������ͣ�null
#define XML_LAYOUT_GRID          _T("grid")          // �������ͣ�grid
#define XML_LAYOUT_DOCK          _T("dock")          // �������ͣ�dock
#define XML_LAYOUT_CANVAS        _T("canvas")        // �������ͣ�canvas
#define XML_LAYOUT_CARD          _T("card")          // �������ͣ�card

// canvas�����е���Ԫ�ص�����
#define XML_LAYOUT_CANVAS_LEFT          _T("layout.left")
#define XML_LAYOUT_CANVAS_TOP           _T("layout.top")
#define XML_LAYOUT_CANVAS_RIGHT         _T("layout.right")
#define XML_LAYOUT_CANVAS_BOTTOM        _T("layout.bottom")

// stack���ֵ�����
#define XML_LAYOUT_STACK_DIRECTION      _T("layout.direction")  // stack���ֵķ���
#define XML_LAYOUT_STACK_LEFTTORIGHT    _T("lefttoright")       // xml�п����õ�ֵ
#define XML_LAYOUT_STACK_RIGHTTOLEFT    _T("righttoleft")       // xml�п����õ�ֵ
#define XML_LAYOUT_STACK_LEFTANDRIGHT   _T("leftandright")      // xml�п����õ�ֵ
#define XML_LAYOUT_STACK_TOPTOBOTTOM    _T("toptobottom")       // xml�п����õ�ֵ
#define XML_LAYOUT_STACK_BOTTOMTOTOP    _T("bottomtotop")       // xml�п����õ�ֵ
#define XML_LAYOUT_STACK_TOPANDBOTTOM   _T("topandbottom")      // xml�п����õ�ֵ
#define LAYOUT_STACK_LEFTTORIGHT        1                       // �ڴ����е�ȡֵ
#define LAYOUT_STACK_RIGHTTOLEFT        2                       // �ڴ����е�ȡֵ
#define LAYOUT_STACK_LEFTANDRIGHT       3                       // �ڴ����е�ȡֵ
#define LAYOUT_STACK_TOPTOBOTTOM        4                       // �ڴ����е�ȡֵ
#define LAYOUT_STACK_BOTTOMTOTOP        5                       // �ڴ����е�ȡֵ
#define LAYOUT_STACK_TOPANDBOTTOM       6                       // �ڴ����е�ȡֵ

// stack�����е���Ԫ�ص�����
#define XML_LAYOUT_STACK_DOCK           _T("layout.dock")       // ��layout.direction=leftandright||topandbottomʱ����Ҫָ����Ԫ�ص���ͣ������һ��
#define XML_LAYOUT_STACK_DOCK_LEFT      _T("left")
#define XML_LAYOUT_STACK_DOCK_RIGHT     _T("right")
#define XML_LAYOUT_STACK_DOCK_TOP       _T("top")
#define XML_LAYOUT_STACK_DOCK_BOTTOM    _T("bottom")

// dock���ֵ�����

// dock�����е���Ԫ�ص�����
#define XML_LAYOUT_DOCK_DOCK            _T("layout.dock")       // ��Ԫ��ͣ������һ������
#define XML_LAYOUT_DOCK_DOCK_LEFT       _T("left")
#define XML_LAYOUT_DOCK_DOCK_RIGHT      _T("right")
#define XML_LAYOUT_DOCK_DOCK_TOP        _T("top")
#define XML_LAYOUT_DOCK_DOCK_BOTTOM     _T("bottom")
#define XML_LAYOUT_DOCK_DOCK_CENTER     _T("center")

// grid���ֵ�����
#define XML_LAYOUT_GRID_WIDTH           _T("layout.width")      // grid���ֵĸ��еĿ��ȶ���
#define XML_LAYOUT_GRID_HEIGHT          _T("layout.height")     // grid���ֵĸ��еĸ߶ȶ���

// grid�����е���Ԫ�ص�����
#define XML_LAYOUT_GRID_ROW             _T("layout.row")        // 
#define XML_LAYOUT_GRID_COL             _T("layout.col")        // 
#define XML_LAYOUT_GRID_ROWSPAN         _T("layout.rowspan")    // 
#define XML_LAYOUT_GRID_COLSPAN         _T("layout.colspan")    // 

//
// style.xml ===>
//
enum STYLE_SELECTOR_TYPE{
	STYLE_SELECTOR_TYPE_ID,
	STYLE_SELECTOR_TYPE_CLASS,
	STYLE_SELECTOR_TYPE_TAG
};
#define  XML_STYLE_SELECTOR_TYPE_ID            _T("id")       // id ѡ����
#define  XML_STYLE_SELECTOR_TYPE_CLASS         _T("class")    // classѡ����
#define  XML_STYLE_INHERIT                     _T("inherit")  // �̳�����

#define  STYLE_ID_PREFIX                       _T('#')        // idѡ����ǰ׺
#define  STYLE_CLASS_PREFIX                    _T('.')        // classѡ����ǰ׺

// Control

// Label
#define  XML_LABEL_COLOR                       _T("color")
#define  XML_LABEL_ALIGN_H                     _T("halign")
#define  XML_LABEL_ALIGN_V                     _T("valign")

#define XML_LABEL_ALIGN_TOP                    _T("top")
#define XML_LABEL_ALIGN_BOTTOM                 _T("bottom")
#define XML_LABEL_ALIGN_LEFT                   _T("left")
#define XML_LABEL_ALIGN_RIGHT                  _T("right")
#define XML_LABEL_ALIGN_CENTER                 _T("center")

typedef enum 
{
	LABEL_ALIGN_TOP = 0,
	LABEL_ALIGN_LEFT = 0,
	LABEL_ALIGN_RIGHT = 0x2,
	LABEL_ALIGN_BOTTOM = 0x4,
	LABEL_ALIGN_CENTER = 0x8,
	LABEL_ALIGN_VCENTER = 0x10,
}LABEL_ALIGN;

// Picture
// #define  XML_PICTURE_IMAGE                     _T("image")

// GroupBox
#define  XML_GROUPBOX_BORDERIMAGE              _T("borderimage")        // �߿�ͼƬ
#define  XML_GOURPBOX_BORDERIMAGE_9REGION      _T("borderimage9region") // �߿�ͼƬ�������

//////////////////////////////////////////////////////////////////////////
// Button

#define  XML_BUTTON_RENDER_DRAW_FOCUS_FLAG     _T("drawfocus")        // ��λ��ƽ���
typedef enum
{
	BUTTON_RENDER_DRAW_FOCUS_TYPE_NULL,                               // ������
	BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT,                                // ʹ�����ߵ����
}BUTTON_RENDER_DRAW_FOCUS_TYPE;

#define  XML_BUTTON_RENDER_AUTOSIZE_TYPE           _T("autosize")     // ��û��ָ����ť��Сʱ����μ������С
#define  XML_BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE   _T("bkimage")      // ����ͼƬ�Ĵ�С������ť��С
#define  XML_BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT   _T("content")      // ����ͼƬ������(ͼ��+�ı�)������ť��С
#define  XML_BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH _T("bkimagehcontentw")  // ͼƬ�ĸ߶�+���ݵĿ���
#define  BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE   0
#define  BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT   1
#define  BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH 2

#define  XML_BUTTON_ICON                             _T("icon")             // ��ťͼ��
#define  XML_BUTTON_ICON_RENDER_TYPE                 _T("icontype")         // ��ťͼ���������
#define  XML_BUTTON_ICON_RENDER_TYPE_STATIC          _T("static")           // ��̬ͼ��
#define  XML_BUTTON_ICON_RENDER_TYPE_8STATE          _T("8state")           // 8̬ͼ��
#define  XML_BUTTON_ICON_RENDER_TYPE_THEME_RADIO     _T("themeradio")       // ���ⵥѡͼ��
#define  XML_BUTTON_ICON_RENDER_TYPE_THEME_CHECK     _T("themecheck")       // ���⸴ѡͼ��
typedef enum
{
	BUTTON_ICON_RENDER_TYPE_NONE,        // ������ͼ��
	BUTTON_ICON_RENDER_TYPE_STATIC,
	BUTTON_ICON_RENDER_TYPE_GIF,
	BUTTON_ICON_RENDER_TYPE_4STATE,
	BUTTON_ICON_RENDER_TYPE_8STATE,
	BUTTON_ICON_RENDER_TYPE_THEME_RADIO,
	BUTTON_ICON_RENDER_TYPE_THEME_CHECK
}BUTTON_ICON_RENDER_TYPE;

#define  XML_BUTTON_TEXT                       _T("text")             // ��ť�ı�����
#define  XML_BUTTON_TEXT_RENDER_TYPE           _T("texttype")         // �ı���������
#define  XML_BUTTON_TEXT_RENDER_TYPE_SINGLELINE _T("singleline")      // �����ı�
#define  XML_BUTTON_TEXT_RENDER_TYPE_HYPERLINK  _T("hyperlink")       // ������ģʽ
typedef enum
{
	BUTTON_TEXT_RENDER_TYPE_NONE,         // �������ı�
	BUTTON_TEXT_RENDER_TYPE_SINGLELINE,   // �����ı�
	BUTTON_TEXT_RENDER_TYPE_HYPERLINK,    // ������

}BUTTON_TEXT_RENDER_TYPE;


// HyperLink
#define  XML_HYPERLINK_HREF              _T("href")     // ������


// #define  XML_BRUSHBUTTON_BKHOVERCOL            _T("bkhovercol")
// #define  XML_BRUSHBUTTON_BKPRESSCOL            _T("bkpresscol")
// #define  XML_BRUSHBUTTON_BORDERHOVERCOL        _T("borderhovercol")
// #define  XML_BRUSHBUTTON_BORDERPRESSCOL        _T("borderpresscol")
// #define  XML_BRUSHBUTTON_TEXTNORMALCOL         _T("textnormalcol")
// #define  XML_BRUSHBUTTON_TEXTHOVERCOL          _T("texthovercol")
// #define  XML_BRUSHBUTTON_TEXTPRESSCOL          _T("textpresscol")
// #define  XML_BRUSHBUTTON_TEXTDISABLECOL        _T("textdisablecol")
// #define  XML_BRUSHBUTTON_TEXT                  _T("text")
// #define  XML_BRUSHBUTTON_ICON                  _T("icon")

//#define  XML_BUTTON_BK_TYPE                    _T("bktype")     // ��ť������������
//#define  XML_BUTTON_BK_TYPE_COLOR              _T("color")    

//////////////////////////////////////////////////////////////////////////
// ImageSliderCtrl
#define  XML_PROGRESSCTRL_DIRECTION            _T("direction")       // �������ķ��򣬿�ȡֵΪ x,y��Ĭ��Ϊx
#define  XML_PROGRESSCTRL_DIRECTION_X          _T("x")               // ͼƬ��������
#define  XML_PROGRESSCTRL_DIRECTION_Y          _T("y")               // ͼƬ��������
#define  PROGRESSCTRL_FLAG_DIRECTION_Y         0x0001                // �����������Ĭ��0Ϊ����

#define  XML_SLIDERCTRL_BUTTON_ATTR_PRIFIX     _T("trackbtn.")       // �����ؼ��İ�ť����ǰ׺

#define  XML_IMAGEPROGRESSCTRL_FOREIMAGE       _T("foreimage")       // ǰ��ͼƬ��һ�ţ�
#define  XML_IMAGEPROGRESSCTRL_BKGNDIMAGE      _T("bkgndimage")      // ����ͼƬ��һ�ţ�

// Edit
#define  XML_EDIT_TEXT                        _T("text")             // ��������
#define  XML_EDIT_COLOR                       _T("color")            // ����������ʾ��ɫ
#define  XML_EDIT_SELECT_COLOR                _T("selectcolor")      // ���ֱ�ѡ��ʱ����ɫ
#define  XML_EDIT_SELECT_BK_COLOR             _T("selectbkcolor")    // ���ֱ�ѡ��ʱ��������ɫ
#define  XML_EDIT_MAX_LENGTH                  _T("maxlength")        // ���ݵ��ֵ
//#define  XML_EDIT_MAX_INT

// Scroll Bar
//#define  XML_SCROLLBAR_HIDE_LINEBTN               _T("hidelinebtn") // �Ƿ���ʾ������ť 0:1
#define  XML_SCROLLBAR_LINE_BUTTON1_ATTR_PRIFIX   _T("linebtn1.")   // ��������ť1������ǰ׺
#define  XML_SCROLLBAR_LINE_BUTTON2_ATTR_PRIFIX   _T("linebtn2.")   // ��������ť2������ǰ׺
#define  XML_SCROLLBAR_THUMB_BUTTON_ATTR_PRIFIX   _T("thubmbtn.")     // �������϶�������ǰ׺

#define  XML_HSCROLLBAR                           _T("hscrollbar")  // �������������
#define  XML_VSCROLLBAR                           _T("vscrollbar")  // �������������
#define  XML_SCROLLBAR_NONE                       _T("none")        // �÷����޹�����
#define  XML_SCROLLBAR_AUTO                       _T("auto")        // �÷����ϵĹ���������Ҫʱ��ʾ
#define  XML_SCROLLBAR_ALWAYSSHOW                 _T("show")        // �÷����ϵĹ�����������ʾ
#define  XML_SCROLLBAR_ALWAYSHIDE                 _T("hide")        // �÷�����֧�ֹ�����������ʾ������

enum SCROLLBAR_VISIBLE_TYPE
{
	SCROLLBAR_VISIBLE_NONE,          // ��ʹ��
	SCROLLBAR_VISIBLE_AUTO,          // ����Ҫ������ʱ������ʾ
	SCROLLBAR_VISIBLE_SHOW_ALWAYS,   // һֱ��ʾ
	SCROLLBAR_VISIBLE_HIDE_ALWAYS,   // һֱ����
};
enum SCROLLBAR_DIRECTION_TYPE
{
	HSCROLLBAR = WM_HSCROLL,
	VSCROLLBAR = WM_VSCROLL
};

#define  XML_SCROLLBAR_TYPE                       _T("type")        // ǰ��XML_xxSCROLLBAR_PRIFIX��������������
#define  XML_SCROLLBAR_TYPE_SYSTEM                _T("system")      // ϵͳ��ʽ�Ĺ�������������ť+һ���϶���
#define  XML_SCROLLBAR_TYPE_SLIDER                _T("slider")      // ֻ���϶�����û��line button
#define  XML_SCROLLBAR_TYPE_LOGIC                 _T("logic")       // �޹�������ʾ����ά���󶨶�����й���������
#define  XML_SCROLLBAR_TYPE_MENU                  _T("menu")        // 

enum SCROLLBAR_TYPE
{
	SCROLLBAR_TYPE_SYSTEM,
	SCROLLBAR_TYPE_SLIDER,
	SCROLLBAR_TYPE_LOGIC,
	SCROLLBAR_TYPE_MENU,
};

#define  XML_HSCROLLBAR_PRIFIX                    _T("hscrollbar.") // ���������������ǰ׺
#define  XML_VSCROLLBAR_PRIFIX                    _T("vscrollbar.") // ���������������ǰ׺