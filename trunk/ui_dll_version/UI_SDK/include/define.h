#pragma once
#include "common/comdef.h"

// 导入导出定义
#ifdef UI_DLL_EXPORTS
#define UIAPI __declspec(dllexport)
#else
#define UIAPI __declspec(dllimport)
#endif

typedef RECT  REGION4;
typedef CRect CRegion4;

typedef map<String,String>  ATTRMAP;

#define FUNC_NAME  _T(__FUNCTION__)


// 句柄定义
typedef void *HSKIN;
typedef HRESULT (*s_UICreateInstancePtr)(void** pOut);

// 窗口类名称
#define WND_CLASS_NAME                  _T("UI")
#define WND_ANIMATE_CLASS_NAME          _T("UI_Animate")
#define WND_POPUP_CONTROL_SHADOW_NAME   _T("UI_PopupControlShadow") // 带系统阴影功能
#define WND_POPUP_CONTROL_NAME          _T("UI_PopupControl")

// 临时文件后缀名
#define TEMP_FILE_EXT    _T(".tmp")

// 特殊含义常量
#define NDEF  -1                               // 没有定义的变量
#define AUTO  -2                               // 自动调整（如width/height=auto）

#define MAX_STRING_LEN    256                  // 定义一些字符串的最大长度，例如皮肤名称，ID等等


/*  附表（HSL/V/B在各种环境下的取值范围）：
Applications       Space       H Range      S Range      L/V/B Range

Paint Shop Pro     HSL         0 - 255      0 - 255      L  0 - 255
Gimp               HSV         0 - 360°     0 - 100      V  0 - 100
Photoshop          HSV         0 - 360°     0 - 100%     B  0 - 100%
Windows            HSL         0 - 240      0 - 240      L  0 - 240
Linux / KDE        HSV         0 - 360°     0 - 255      V  0 - 255
GTK                HSV         0 - 360°     0 - 1.0      V  0 - 1.0
Java (awt.Color)   HSV         0 - 1.0      0 - 1.0      B  0 - 1.0
Apple              HSV         0 - 360°     0 - 100%     L  0 - 100%
*/
#define MIN_HUE_VALUE           0              // 色调  HLS范围定义
#define MAX_HUE_VALUE           240
#define MIN_LUMINANCE_VALUE     0              // 亮度
#define MAX_LUMINANCE_VALUE     240            
#define MIN_SATURATION_VALUE    0              // 饱和度
#define MAX_SATURATION_VALUE    240            



#define CHANGE_SKIN_HLS_FLAG_H             1
#define CHANGE_SKIN_HLS_FLAG_L             2
#define CHANGE_SKIN_HLS_FLAG_S             4
#define CHANGE_SKIN_HLS_FLAG_HLS           (CHANGE_SKIN_HLS_FLAG_H|CHANGE_SKIN_HLS_FLAG_L|CHANGE_SKIN_HLS_FLAG_S)
#define CHANGE_SKIN_HLS_FALG_REPLACE_MODE  8 // 着色模式

//////////////////////////////////////////////////////////////////////////
// enum

// 资源类型定义
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
// helper (#include <windowsx.h>即可，但我们只需要这个)

#define GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))

// 日志输出，使用方法参见ui.cpp
#define LOG_LEVEL(pLog,module,level,content)         \
	if( NULL == pLog )                               \
		return;                                      \
	long lRet = 0;                                   \
	pLog->TestCanLog(module,level, &lRet);           \
	if (0 == lRet)                                   \
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
//                                UI自定义消息定义                             //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////


/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
//                                XML中的tag定义                               //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#define XML_SEPARATOR             _T(',')            // xml中的分隔符，如表示颜色、区域
#define XML_PATH_SEPARATOR        _T('/')            // xml中的路径分隔符（如样式路径，子对象路径）
#define XML_MULTI_SEPARATOR       _T(';')            // xml中的分隔符,如分隔一个控件的多个style.
#define XML_FLAG_SEPARATOR        _T('|')            // xml中的分隔符，如文本对齐方式: left|top|vcenter
#define XML_CHILD_SEPARATOR       _T('.')            // xml中父子之间的分隔符，如Menu.String
#define XML_HEADER                _T("<?xml version=\"1.0\" encoding=\"utf-8\" ?>")
#define XML_PROJECT_XMLFILE_EXT   _T(".uiproj")       // 工程描述文件的后缀名

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

#define XML_IMAGE_USESKINHLS          _T("useskinhls")    // 改变皮肤色调时，该图片是否参与 0/1，默认为1
#define XML_IMAGE_ITEM_TYPE           _T("type")          // image类型
#define XML_IMAGE_ITEM_TYPE_IMAGE     _T("image")         // 默认值
#define XML_IMAGE_ITEM_TYPE_ICON      _T("icon")          // 图标 
#define XML_IMAGE_ITEM_TYPE_IMAGELIST _T("imagelist")     // 图片列表
#define XML_IMAGE_ICON_WIDTH          _T("width")         // 要加载的图标宽度
#define XML_IMAGE_ICON_HEIGHT         _T("height")        // 要加载的图标高度
#define XML_IMAGE_IMAGELIST_COUNT     _T("count")         // 图片列表项的数目 
#define XML_IMAGE_IMAGELIST_LAYOUT    _T("layout")        // 图片列表项的排布方式
#define XML_IMAGE_IMAGELIST_LAYOUT_H  _T("h")  
#define XML_IMAGE_IMAGELIST_LAYOUT_V  _T("v")

enum IMAGE_ITEM_TYPE
{
	IMAGE_ITEM_TYPE_IMAGE,
	IMAGE_ITEM_TYPE_ICON,
	IMAGE_ITEM_TYPE_IMAGE_LIST,

	// 将GIF不作为image type，而是做为一类control来实现
//	IMAGE_ITEM_TYPE_GIF,  
//	IMAGE_ITEM_TYPE_IMAGE_LIST_GIF
};

#define XML_COLOR_USESKINHLS          _T("useskinhls")    // 改变皮肤色调时，该颜色是否参与 0/1，默认为1

// 工程配置
#define XML_PROJECT_NAME       _T("projectname")     // 工程名称
#define XML_PROJECT_SELECT     _T("select")          // 当前工程使用了哪款皮肤名称
#define XML_PROJECT_SKIN       _T("skin")            // 工程中一款皮肤
#define XML_PROJECT_SKIN_NAME  _T("name")            // 皮肤名称
#define XML_PROJECT_SKIN_PATH  _T("path")            // 皮肤ui.xml所在路径
#define XML_PROJECT_SKIN_HLS   _T("hls")             // 该皮肤的色调信息,如"10,0,0"


//
// layout.xml
//

// Object对象属性

//#define XML_ID                   _T("id")            // 每个对象的标识符
#define XML_STYLECLASS           _T("styleclass")    // 对象的样式类型
#define XML_WIDTH                _T("width")         // 对象的宽度，可以为 auto | 数值
#define XML_HEIGHT               _T("height")        // 对象的高度，可以为 auto | 数值
#define XML_MARGIN               _T("margin")        // 对象的外边距
#define XML_PADDING              _T("padding")       // 对象的内边距
#define XML_BORDER               _T("border")        // 对象的边框
#define XML_MINWIDTH             _T("minwidht")      // 对象的最小宽度
#define XML_MINHEIGHT            _T("minheight")     // 对象的最小高度
#define XML_MAXWIDTH             _T("maxwidth")      // 对象的最大宽度
#define XML_MAXHEIGHT            _T("maxheight")     // 对象的最大高度
#define XML_AUTO                 _T("auto")          // 对象的宽和高设置为由内部内容决定
#define XML_ASTERISK             _T("*")             // 对象的宽和高设置为平分

// EraseBkgnd

#define XML_BACKGND_IS_TRANSPARENT       _T("istransparent")            // 是否需要进行透明处理（刷上父对象的背景）0/1

#define XML_RENDER_TYPE                   _T("render.type")             // 设置渲染类型
#define XML_RENDER_TYPE_GRADIENT_H        _T("gradienth")               // 横向渐变 Color + Color
#define XML_RENDER_TYPE_GRADIENT_V        _T("gradientv")               // 纵向渐变 Color + Color
#define XML_RENDER_TYPE_IMAGE             _T("image")                   // 贴图Image + Color
#define XML_RENDER_TYPE_IMAGELISTITEM     _T("imagelistitem")           // 图片列表中的一项，需要指定其索引
#define XML_RENDER_TYPE_IMAGELIST         _T("imagelist")               // 图片列表
#define XML_RENDER_TYPE_COLOR             _T("color")                   // 纯色填充 Color
#define XML_RENDER_TYPE_COLORLIST         _T("colorlist")               // 类似office 2003工具栏式按钮
#define XML_RENDER_TYPE_NOTHEME           _T("notheme")                 // win2000主题控件
#define XML_RENDER_TYPE_THEME             _T("theme")                   // 系统主题控件
#define XML_RENDER_TYPE_NULL              _T("null")                    // 不绘制该render，但也会实例化该render指针，避免控件默认使用theme主题


#define XML_RENDER_COLOR                  _T("render.color")            // 填充颜色
#define XML_RENDER_BORDERCOLOR            _T("render.bordercolor")      // 边框颜色
#define XML_RENDER_COLORFROM              _T("render.colorfrom")        // 渐变起始色
#define XML_RENDER_COLORTO                _T("render.colorto")          // 渐变终点色
#define XML_RENDER_IMAGE                  _T("render.image")            // 背景图片
#define XML_RENDER_IMAGE9REGION           _T("render.9region")          // 背景伸缩方式
#define XML_RENDER_LIST_STATEMAPINDEX     _T("render.list.statemap")    // 设置控件绘制状态对应的图片项，例如0:0,1:0,2:0,3:0,4:1 表示有两个图片项，但selection用第二张图片，其它用第一张图片
#define XML_RENDER_COLORLIST_COUNT        _T("render.colorlist.count")  // 颜色列表的数量
#define XML_RENDER_IMAGELIST_COUNT        _T("render.imagelist.count")  // 图片列表中的图片数量
#define XML_RENDER_IMAGELIST_LAYOUT       _T("render.imagelist.layout") // 图片排列方向(默认为横向)
#define XML_RENDER_IMAGELIST_LAYOUT_H     _T("h")                       // 图片横向排列
#define XML_RENDER_IMAGELIST_LAYOUT_V     _T("v")                       // 图片纵向排列

#define XML_RENDER_IMAGELISTITEM_INDEX    _T("render.imagelistitem.index")  // 图片列表项的索引值

#define XML_RENDER_IMAGE_DRAWTYPE         _T("render.image.drawtype")   // 图片绘制方式
#define XML_RENDER_IMAGE_DRAWTYPE_SIMPLE  _T("simple")                  // (0,0)位置开始贴图Image + Color
#define XML_RENDER_IMAGE_DRAWTYPE_TILE    _T("tile")                    // (0,0)位置开始平铺Image
#define XML_RENDER_IMAGE_DRAWTYPE_STRETCH _T("stretch")                 // 拉伸模式Image + Image9Region*
#define XML_RENDER_IMAGE_DRAWTYPE_ADAPT   _T("adapt")                   // 自适应居中，不会超出绘制范围
#define XML_RENDER_IMAGE_DRAWTYPE_CENTER  _T("center")                  // 居中，可能超出绘制范围

#define XML_BACKGND_RENDER_PREFIX         _T("bkgnd.")                  // 背景渲染属性前缀，如bkgnd.render.type="" bkgnd.render.image=""
#define XML_FOREGND_RENDER_PREFIX         _T("foregnd.")                // 前景
#define XML_ICON_RENDER_PREFIX            _T("icon.")                   // 图标属性渲染前缀

enum RENDER_TYPE
{
	RENDER_TYPE_NULL,             
	RENDER_TYPE_COLOR,        
	RENDER_TYPE_GRADIENTH,    
	RENDER_TYPE_GRADIENTV,    
	RENDER_TYPE_IMAGE,  
	RENDER_TYPE_IMAGELISTITEM,
	RENDER_TYPE_COLORLIST,
	RENDER_TYPE_IMAGELIST,
	
	RENDER_TYPE_NOTHEME,    // 使用系统无主题样式的样式（win2000样式）
	RENDER_TYPE_THEME_FIRST,
	RENDER_TYPE_THEME,
	RENDER_TYPE_THEME_MENUSTRINGITEM,
	RENDER_TYPE_THEME_MENUSEPERATOR,
	RENDER_TYPE_THEME_MENUPOPUPTRIANGLE,
	RENDER_TYPE_THEME_MENUCHECKICON,
	RENDER_TYPE_THEME_MENURADIOICON,
	RENDER_TYPE_THEME_MENURADIOCHECKICONBK,
	RENDER_TYPE_THEME_VSCROLLBARBACKGND,
	RENDER_TYPE_THEME_HSCROLLBARBACKGND,
	RENDER_TYPE_THEME_SCROLLBAR_SIZEBOX,
	RENDER_TYPE_THEME_PROGRESS_BKGND,
	RENDER_TYPE_THEME_PROGRESS_FOREGND,
	RENDER_TYPE_THEME_LAST,
};

enum IMAGELIST_LAYOUT_TYPE
{
	IMAGELIST_LAYOUT_TYPE_H,
	IMAGELIST_LAYOUT_TYPE_V
};

#define XML_TEXTRENDER_TYPE                 _T("textrender.type")       // 设置文字渲染类型
#define XML_TEXTRENDER_TYPE_NORMAL          _T("normal")                // color + font
#define XML_TEXTRENDER_TYPE_COLORLIST       _T("colorlist")             // color list + font
#define XML_TEXTRENDER_TYPE_FONTLIST        _T("fontlist")              // font list + color
#define XML_TEXTRENDER_TYPE_FONTCOLORLIST   _T("fontcolorlist")         // font list + color list

#define XML_TEXTRENDER_COLOR                _T("textrender.color")      // 字体颜色
#define XML_TEXTRENDER_FONT                 _T("textrender.font")       // 字体
#define XML_TEXTRENDER_COLORLIST_COUNT      _T("textrender.colorlist.count")
#define XML_TEXTRENDER_FONTCOLORLIST_COUNT  _T("textrender.fontcolorlist.count")  // 字体、颜色数量
#define XML_TEXTRENDER_ALIGN                _T("textrender.align")
#define XML_TEXTRENDER_PADDING              _T("textrender.padding")

#define XML_TEXTRENDER_ALIGN_TOP            _T("top")
#define XML_TEXTRENDER_ALIGN_BOTTOM         _T("bottom")
#define XML_TEXTRENDER_ALIGN_LEFT           _T("left")
#define XML_TEXTRENDER_ALIGN_RIGHT          _T("right")
#define XML_TEXTRENDER_ALIGN_CENTER         _T("center")
#define XML_TEXTRENDER_ALIGN_VCENTER        _T("vcenter")

enum TEXTRENDER_TYPE
{
	TEXTRENDER_TYPE_NULL,
	TEXTRENDER_TYPE_NORMAL,
	TEXTRENDER_TYPE_COLORLIST,
	TEXTRENDER_TYPE_FONTLIST,
	TEXTRENDER_TYPE_FONTCOLORLIST
};


// #define  XML_BACK_RENDER_COLOR_FLAG                  _T("bkrenderflag")       // 需要绘制哪些状态的背景
// #define  XML_BACK_RENDER_COLOR_DRAW_NORMAL   0x0001
// #define  XML_BACK_RENDER_COLOR_DRAW_HOVER    0x0002
// #define  XML_BACK_RENDER_COLOR_DRAW_PRESS    0x0004
// #define  XML_BACK_RENDER_COLOR_DRAW_DISALBE  0x0008
// #define  XML_BACK_RENDER_COLOR_DRAW_2STATE   (XML_BACK_RENDER_COLOR_DRAW_HOVER|XML_BACK_RENDER_COLOR_DRAW_PRESS)
// #define  XML_BACK_RENDER_COLOR_DRAW_4STATE   (XML_BACK_RENDER_COLOR_DRAW_NORMAL|XML_BACK_RENDER_COLOR_DRAW_HOVER|XML_BACK_RENDER_COLOR_DRAW_PRESS|XML_BACK_RENDER_COLOR_DRAW_DISALBE)   

// #define  XML_BACK_RENDER_COLOR_BK_NORMAL_COLOR       _T("bknormalcol")      // 背景颜色
// #define  XML_BACK_RENDER_COLOR_BK_HVOER_COLOR        _T("bkhovercol")       // 背景颜色
// #define  XML_BACK_RENDER_COLOR_BK_PRESS_COLOR        _T("bkpresscol")       // 背景颜色
// #define  XML_BACK_RENDER_COLOR_BK_DISABLE_COLOR      _T("bkdisablecol")     // 背景颜色
// 
// #define  XML_BACK_RENDER_COLOR_BORDER_NORMAL_COLOR   _T("bordernormalcol")  // 边框颜色
// #define  XML_BACK_RENDER_COLOR_BORDER_HVOER_COLOR    _T("borderpresscol")   // 边框颜色
// #define  XML_BACK_RENDER_COLOR_BORDER_PRESS_COLOR    _T("borderhovercol")   // 边框颜色
// #define  XML_BACK_RENDER_COLOR_BORDER_DISABLE_COLOR  _T("borderdisablecol") // 边框颜色

// window rgn
#define  XML_WINDOW_TRANSPARENT_TYPE                    _T("transparent.type")
#define  XML_WINDOW_TRANSPARENT_PART                    _T("transparent.part") // 窗口透明区域类型
#define  XML_WINDOW_TRANSPARENT_PART_9REGION            _T("transparent.part.9region") // 窗口透明区域参数（9Region类型，可为4，8，9三种）

#define  XML_WINDOW_TRANSPARENT_TYPE_LAYERED            _T("layered")          // 使用分层窗口进行透明
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR          _T("maskcolor")        // 使用颜色值
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA          _T("maskalpha")        // 使用透明值

#define  XML_WINDOW_TRANSPARENT_PART_NULL               _T("")                 // RECT，不设置窗口异形 [默认]
#define  XML_WINDOW_TRANSPARENT_PART_4_CORNER           _T("4corner")          // 从背景图中设置四个角落形状
#define  XML_WINDOW_TRANSPARENT_PART_8_BORDER           _T("8border")          // 从背景图中设置边框需要异形
#define  XML_WINDOW_TRANSPARENT_PART_FULL               _T("full")             // 需要对整个窗口范围进行形状设置
#define  XML_WINDOW_TRANSPARENT_PART_CUSTOM_CORNER      _T("customcorner")     // 自定义四个角落

#define  XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR_VALUE    _T("transparent.maskcolor")  // 窗口透明色
#define  XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA_VALUE    _T("transparent.maskalpha")  // 将窗口中小于alpha值通过的抠掉

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

// 其它通用属性
#define XML_CURSOR                           _T("cursor")       // 对象的鼠标样式
#define XML_TOOLTIP                          _T("tooltip")      // 对象的提示信息，自定义的提示内容需要代码自行添加
#define XML_CONTROL_STYLE_GROUP              _T("group")
#define XML_CONTROL_STYLE_TABABLE            _T("tabable")

// window
// #define  XML_WINDOW_BK_TYPE                    _T("bktype")     // 窗口背景绘制类型
// #define  XML_WINDOW_BK_TYPE_COLOR              _T("color")      // 窗口背景绘制类型--颜色，画刷画笔
// #define  XML_WINDOW_BK_TYPE_TILE               _T("tile")       // 窗口背景绘制类型--平铺
// #define  XML_WINDOW_BK_TYPE_SIMPLE             _T("simple")     // 窗口背景绘制类型--简单，(0,0)bitblt
// #define  XML_WINDOW_BK_TYPE_CENTER             _T("center")     // 窗口背景绘制类型--居中
// #define  XML_WINDOW_BK_TYPE_STRETCH            _T("stretch")    // 窗口背景绘制类型--拉伸
// #define  XML_WINDOW_BK_TYPE_FRAME              _T("frame")      // 窗口背景绘制类型--边框
// #define  XML_WINDOW_BK_IMAGE                   _T("bkimage")    // 窗口背景图片名称
// #define  XML_WINDOW_BK_TYPE_COLOR_PEN          _T("bkpencol")   // bktype=color，窗口边框色
// #define  XML_WINDOW_BK_TYPE_COLOR_BRUSH        _T("bkbrushcol") // bktype=color，窗口背景色
// #define  XML_WINDOW_BK_TYPE_FRAME_9REGION      _T("bk9region")  // bktype=frame，窗口切割范围


// hwnd host
#define XML_HWNDHOST_CONTROL_ID   _T("controlid")     // 控件ID数值，整形

//#define XML_LAYOUT               _T("layout")        // 对象的布局类型
#define XML_LAYOUT_STACK         _T("stack")         // 布局类型：stack
#define XML_LAYOUT_NULL          _T("null")          // 布局类型：null
#define XML_LAYOUT_GRID          _T("grid")          // 布局类型：grid
#define XML_LAYOUT_DOCK          _T("dock")          // 布局类型：dock
#define XML_LAYOUT_CANVAS        _T("canvas")        // 布局类型：canvas
#define XML_LAYOUT_CARD          _T("card")          // 布局类型：card

// canvas布局中的子元素的属性
#define XML_LAYOUT_CANVAS_LEFT          _T("layout.left")
#define XML_LAYOUT_CANVAS_TOP           _T("layout.top")
#define XML_LAYOUT_CANVAS_RIGHT         _T("layout.right")
#define XML_LAYOUT_CANVAS_BOTTOM        _T("layout.bottom")

// stack布局的属性
#define XML_LAYOUT_STACK_DIRECTION      _T("layout.direction")  // stack布局的方向
#define XML_LAYOUT_STACK_LEFTTORIGHT    _T("lefttoright")       // xml中可配置的值
#define XML_LAYOUT_STACK_RIGHTTOLEFT    _T("righttoleft")       // xml中可配置的值
#define XML_LAYOUT_STACK_LEFTANDRIGHT   _T("leftandright")      // xml中可配置的值
#define XML_LAYOUT_STACK_TOPTOBOTTOM    _T("toptobottom")       // xml中可配置的值
#define XML_LAYOUT_STACK_BOTTOMTOTOP    _T("bottomtotop")       // xml中可配置的值
#define XML_LAYOUT_STACK_TOPANDBOTTOM   _T("topandbottom")      // xml中可配置的值
#define LAYOUT_STACK_LEFTTORIGHT        1                       // 在代码中的取值
#define LAYOUT_STACK_RIGHTTOLEFT        2                       // 在代码中的取值
#define LAYOUT_STACK_LEFTANDRIGHT       3                       // 在代码中的取值
#define LAYOUT_STACK_TOPTOBOTTOM        4                       // 在代码中的取值
#define LAYOUT_STACK_BOTTOMTOTOP        5                       // 在代码中的取值
#define LAYOUT_STACK_TOPANDBOTTOM       6                       // 在代码中的取值

// stack布局中的子元素的属性
#define XML_LAYOUT_STACK_DOCK           _T("layout.dock")       // 当layout.direction=leftandright||topandbottom时，需要指定子元素到底停靠在哪一侧
#define XML_LAYOUT_STACK_DOCK_LEFT      _T("left")
#define XML_LAYOUT_STACK_DOCK_RIGHT     _T("right")
#define XML_LAYOUT_STACK_DOCK_TOP       _T("top")
#define XML_LAYOUT_STACK_DOCK_BOTTOM    _T("bottom")

// dock布局的属性

// dock布局中的子元素的属性
#define XML_LAYOUT_DOCK_DOCK            _T("layout.dock")       // 子元素停靠在哪一个方向
#define XML_LAYOUT_DOCK_DOCK_LEFT       _T("left")
#define XML_LAYOUT_DOCK_DOCK_RIGHT      _T("right")
#define XML_LAYOUT_DOCK_DOCK_TOP        _T("top")
#define XML_LAYOUT_DOCK_DOCK_BOTTOM     _T("bottom")
#define XML_LAYOUT_DOCK_DOCK_CENTER     _T("center")

// grid布局的属性
#define XML_LAYOUT_GRID_WIDTH           _T("layout.width")      // grid布局的各列的宽度定义
#define XML_LAYOUT_GRID_HEIGHT          _T("layout.height")     // grid布局的各列的高度定义

// grid布局中的子元素的属性
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
#define  XML_STYLE_SELECTOR_TYPE_ID            _T("id")       // id 选择器
#define  XML_STYLE_SELECTOR_TYPE_CLASS         _T("class")    // class选择器
#define  XML_STYLE_INHERIT                     _T("inherit")  // 继承属性

#define  STYLE_ID_PREFIX                       _T('#')        // id选择器前缀
#define  STYLE_CLASS_PREFIX                    _T('.')        // class选择器前缀

// Control

// Label
#define  XML_LABEL_COLOR                       _T("color")

// TODO: delete below
// #define  XML_LABEL_ALIGN                       _T("align")
// 
// #define XML_LABEL_ALIGN_TOP                    _T("top")
// #define XML_LABEL_ALIGN_BOTTOM                 _T("bottom")
// #define XML_LABEL_ALIGN_LEFT                   _T("left")
// #define XML_LABEL_ALIGN_RIGHT                  _T("right")
// #define XML_LABEL_ALIGN_CENTER                 _T("center")
// #define XML_LABEL_ALIGN_VCENTER                _T("vcenter")

// typedef enum  // 废弃，直接使用DT_LEFT等系统字段
// {
// 	LABEL_ALIGN_TOP = 0,
// 	LABEL_ALIGN_LEFT = 0,
// 	LABEL_ALIGN_RIGHT = 0x2,
// 	LABEL_ALIGN_BOTTOM = 0x4,
// 	LABEL_ALIGN_CENTER = 0x8,
// 	LABEL_ALIGN_VCENTER = 0x10,
// }LABEL_ALIGN;

// TODO: delete below
// Picture
// #define  XML_PICTURE_IMAGE                     _T("image")

// GroupBox
#define  XML_GROUPBOX_BORDERIMAGE              _T("borderimage")        // 边框图片
#define  XML_GOURPBOX_BORDERIMAGE_9REGION      _T("borderimage9region") // 边框图片拉伸参数

//////////////////////////////////////////////////////////////////////////
// Button

#define  XML_BUTTON_RENDER_DRAW_FOCUS_FLAG     _T("drawfocus")        // 如何绘制焦点
typedef enum
{
	BUTTON_RENDER_DRAW_FOCUS_TYPE_NONE,                               // 不绘制
	BUTTON_RENDER_DRAW_FOCUS_TYPE_DOT,                                // 使用虚线点绘制
}BUTTON_RENDER_DRAW_FOCUS_TYPE;

#define  XML_BUTTON_RENDER_AUTOSIZE_TYPE           _T("autosize")     // 当没有指定按钮大小时，如何计算其大小
#define  XML_BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE   _T("bkimage")      // 背景图片的大小决定按钮大小
#define  XML_BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT   _T("content")      // 背景图片的内容(图标+文本)决定按钮大小
#define  XML_BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH _T("bkimagehcontentw")  // 图片的高度+内容的宽度
enum BUTTON_RENDER_AUTO_SIZE_TYPE
{
	BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGE,
	BUTTON_RENDER_AUTOSIZE_TYPE_CONTENT,
	BUTTON_RENDER_AUTOSIZE_TYPE_BKIMAGEHEIGHT_CONTENTWIDTH
};

#define  XML_BUTTON_ICON                             _T("icon")             // 按钮图标
#define  XML_BUTTON_ICON_RENDER_TYPE                 _T("icontype")         // 按钮图标绘制类型
#define  XML_BUTTON_ICON_RENDER_TYPE_STATIC          _T("static")           // 静态图标
#define  XML_BUTTON_ICON_RENDER_TYPE_8STATE          _T("8state")           // 8态图标
#define  XML_BUTTON_ICON_RENDER_TYPE_THEME_RADIO     _T("themeradio")       // 主题单选图标
#define  XML_BUTTON_ICON_RENDER_TYPE_THEME_CHECK     _T("themecheck")       // 主题复选图标
typedef enum
{
	BUTTON_ICON_RENDER_TYPE_NONE,        // 不绘制图标
	BUTTON_ICON_RENDER_TYPE_STATIC,
	BUTTON_ICON_RENDER_TYPE_GIF,
	BUTTON_ICON_RENDER_TYPE_4STATE,
	BUTTON_ICON_RENDER_TYPE_8STATE,
	BUTTON_ICON_RENDER_TYPE_THEME_RADIO,
	BUTTON_ICON_RENDER_TYPE_THEME_CHECK
}BUTTON_ICON_RENDER_TYPE;

#define  XML_BUTTON_TEXT                       _T("text")             // 按钮文本内容
#define  XML_BUTTON_TEXT_RENDER_TYPE           _T("texttype")         // 文本绘制类型
#define  XML_BUTTON_TEXT_RENDER_TYPE_SINGLELINE _T("singleline")      // 单行文本
#define  XML_BUTTON_TEXT_RENDER_TYPE_HYPERLINK  _T("hyperlink")       // 超链接模式
typedef enum
{
	BUTTON_TEXT_RENDER_TYPE_NONE,         // 不绘制文本
	BUTTON_TEXT_RENDER_TYPE_SINGLELINE,   // 单行文本
	BUTTON_TEXT_RENDER_TYPE_HYPERLINK,    // 超链接

}BUTTON_TEXT_RENDER_TYPE;


// HyperLink
#define  XML_HYPERLINK_HREF              _T("href")     // 超链接


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

//#define  XML_BUTTON_BK_TYPE                    _T("bktype")     // 按钮背景绘制类型
//#define  XML_BUTTON_BK_TYPE_COLOR              _T("color")    

//////////////////////////////////////////////////////////////////////////
// ImageSliderCtrl
#define  XML_PROGRESSCTRL_DIRECTION            _T("direction")       // 滚动条的方向，可取值为 x,y，默认为x
#define  XML_PROGRESSCTRL_DIRECTION_H          _T("h")               // 图片横向排列
#define  XML_PROGRESSCTRL_DIRECTION_V          _T("v")               // 图片纵向排列
#define  PROGRESSCTRL_FLAG_DIRECTION_V         0x0001                // 纵向滚动条，默认0为横向

#define  XML_SLIDERCTRL_BUTTON_ATTR_PRIFIX     _T("trackbtn.")       // 滑动控件的按钮属性前缀

#define  XML_IMAGEPROGRESSCTRL_FOREIMAGE       _T("foreimage")       // 前景图片（一张）
#define  XML_IMAGEPROGRESSCTRL_BKGNDIMAGE      _T("bkgndimage")      // 背景图片（一张）

// Edit
#define  XML_EDIT_TEXT                        _T("text")             // 文字内容
#define  XML_EDIT_COLOR                       _T("color")            // 正常文字显示颜色
#define  XML_EDIT_SELECT_COLOR                _T("selectcolor")      // 文字被选中时的颜色
#define  XML_EDIT_SELECT_BK_COLOR             _T("selectbkcolor")    // 文字被选中时背景的颜色
#define  XML_EDIT_MAX_LENGTH                  _T("maxlength")        // 内容的最长值
//#define  XML_EDIT_MAX_INT

// Scroll Bar
#define  XML_SCROLLBAR_NO_LINEBTN                 _T("nolinebtn")   // 是否显示lineup/linedown/lineleft/lineright按钮 0/1
#define  XML_SCROLLBAR_LINE_BUTTON1_ATTR_PRIFIX   _T("linebtn1.")   // 滚动条按钮1的属性前缀
#define  XML_SCROLLBAR_LINE_BUTTON2_ATTR_PRIFIX   _T("linebtn2.")   // 滚动条按钮2的属性前缀
#define  XML_SCROLLBAR_THUMB_BUTTON_ATTR_PRIFIX   _T("thubmbtn.")     // 滚动条拖动条属性前缀

#define  XML_HSCROLLBAR                           _T("hscrollbar")  // 横向滚动条类型
#define  XML_VSCROLLBAR                           _T("vscrollbar")  // 纵向滚动条类型
#define  XML_SCROLLBAR_NONE                       _T("none")        // 该方向无滚动条
#define  XML_SCROLLBAR_AUTO                       _T("auto")        // 该方向上的滚动条在需要时显示
#define  XML_SCROLLBAR_ALWAYSSHOW                 _T("show")        // 该方向上的滚动条保持显示
#define  XML_SCROLLBAR_ALWAYSHIDE                 _T("hide")        // 该方向上支持滚动，但不显示滚动条

enum SCROLLBAR_VISIBLE_TYPE
{
	SCROLLBAR_VISIBLE_NONE,          // 不使用
	SCROLLBAR_VISIBLE_AUTO,          // 当需要滚动的时候再显示
	SCROLLBAR_VISIBLE_SHOW_ALWAYS,   // 一直显示
	SCROLLBAR_VISIBLE_HIDE_ALWAYS,   // 一直隐藏
};
enum SCROLLBAR_DIRECTION_TYPE
{
	HSCROLLBAR = WM_HSCROLL,
	VSCROLLBAR = WM_VSCROLL
};

#define  XML_SCROLLBAR_TYPE                       _T("type")        // 前接XML_xxSCROLLBAR_PRIFIX，滚动条的类型
#define  XML_SCROLLBAR_TYPE_SYSTEM                _T("system")      // 系统样式的滚动条，两个按钮+一个拖动条
#define  XML_SCROLLBAR_TYPE_SLIDER                _T("slider")      // 只有拖动条，没有line button
#define  XML_SCROLLBAR_TYPE_LOGIC                 _T("logic")       // 无滚动条显示，但维护绑定对象进行滚动的数据
#define  XML_SCROLLBAR_TYPE_MENU                  _T("menu")        // 

enum SCROLLBAR_TYPE
{
	SCROLLBAR_TYPE_SYSTEM,
	SCROLLBAR_TYPE_SLIDER,
	SCROLLBAR_TYPE_LOGIC,
	SCROLLBAR_TYPE_MENU,
};

#define  XML_HSCROLLBAR_PRIFIX                    _T("hscrollbar.") // 横向滚动条的属性前缀
#define  XML_VSCROLLBAR_PRIFIX                    _T("vscrollbar.") // 纵向滚动条的属性前缀

// listbox
#define  XML_LISTCTRL_ITEM_HEIGHT                 _T("itemheight")  // 列表控件每一项的高度

#define  XML_MENU_SEPERATOR_HEIGHT                _T("seperatorheight")  // 菜单的分隔条高度

// combobox
#define  XML_COMBOBOX_EDIT_PRIFIX                 _T("edit.")       // 编辑框的属性前缀
#define  XML_COMBOBOX_BUTTON_PRIFIX               _T("button.")     // 按钮的属性前缀
#define  XML_COMBOBOX_LISTBOX_PRIFIX              _T("listbox.")    // 下拉列表的属性前缀

// ledctrl
#define  XML_LEDCTRL_INDEXMAP                     _T("indexmap")    // 图片索引映射关系

// menu
#define  XML_MENU                 _T("Menu")      // layout.xml中菜单结点的tag name
#define  XML_MENU_STRINGITEM      _T("String")    //
#define  XML_MENU_POPUPITEM       _T("Popup")     //
#define  XML_MENU_SEPARATORITEM   _T("Separator") // 
#define  XML_MENU_DISABLE         _T("disable")   // menu item是否可用，为1或者true表示被禁用

#define  XML_MENU_ICONGUTTERWIDTH     _T("icongutterwidth")  // 菜单图标列的宽度
#define  XML_MENU_POPUPTRIANGLEWIDTH  _T("trianglewidth")    // 右侧三角形的占用宽度
#define  XML_MENU_SEPARATORHEIGHT     _T("separatorheight")  // 分隔条的高度
#define  XML_MENU_RADIO_ICON_RENDER_PREFIX   _T("radioicon.")   // 选中图标绘制render前缀
#define  XML_MENU_CHECK_ICON_RENDER_PREFIX   _T("checkicon.")   // 选中图标绘制render前缀
#define  XML_MENU_RADIOCHECK_ICONBK_RENDER_PREFIX   _T("radiocheckiconbk.") // 选中图标背景绘制render前缀

#define  XML_MENU_ITEM_ICON_RENDER_PREFIX    _T("icon.")     // 菜单项的图标

#define  XML_MENU_SEPARATOR_ID    -1
#define  XML_MENU_POPUP_ID        0
