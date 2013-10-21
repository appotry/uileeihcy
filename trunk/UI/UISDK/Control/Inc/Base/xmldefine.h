#ifndef XMLDEFINE_H_68869DC6_EFDF_4e30_92B7_9C6E197378C6
#define XMLDEFINE_H_68869DC6_EFDF_4e30_92B7_9C6E197378C6

// UICtrl中的xml属性定义

#pragma region // listbox

#define XML_LISTBOX_TEXT_INDENT   _T("textindent")    // 文本缩进
#pragma endregion


#pragma region  // SplitterBar属性定义
#define XML_SPLITTERBAR_DIRECTION     _T("type")
#define XML_SPLITTERBAR_DIRECTION_H   _T("h")    // 横向拖拽条，控件上下分布
#define XML_SPLITTERBAR_DIRECTION_V   _T("v")

enum SPLITTERBAR_DIRECTION
{
    SPLITTERBAR_DIRECTION_H,
    SPLITTERBAR_DIRECTION_V
};

#define XML_SPLITTERBAR_ALIGN         _T("align")       // 当窗口大小改变时，以哪个对象作为基准
#define XML_SPLITTERBAR_ALIGN_LEFT    _T("left")
#define XML_SPLITTERBAR_ALIGN_TOP     _T("top")
#define XML_SPLITTERBAR_ALIGN_RIGHT   _T("right")
#define XML_SPLITTERBAR_ALIGN_BOTTOM  _T("bottom")

enum SPLITTERBAR_ALIGN
{
    SPLITTERBAR_ALIGN_LEFT   = 0,
    SPLITTERBAR_ALIGN_TOP    = 0,
    SPLITTERBAR_ALIGN_RIGHT  = 1,
    SPLITTERBAR_ALIGN_BOTTOM = 1,
};

#define XML_SPLITTERBAR_LEFT_PREFIX    _T("left.")
#define XML_SPLITTERBAR_RIGHT_PREFIX   _T("right.")
#define XML_SPLITTERBAR_TOP_PREFIX     _T("top.")
#define XML_SPLITTERBAR_BOTTOM_PREFIX  _T("bottom.")

#define XML_SPLITTERBAR_OBJ_ID       _T("id")       // 分隔条操纵的控件Id
#define XML_SPLITTERBAR_OBJ_MINSIZE  _T("minsize")  // 分隔条操纵的控件最小宽、高
#define XML_SPLITTERBAR_OBJ_MAXSIZE  _T("maxsize")  // 分隔条操纵的控件最大宽、高

#pragma endregion

#endif //XMLDEFINE_H_68869DC6_EFDF_4e30_92B7_9C6E197378C6