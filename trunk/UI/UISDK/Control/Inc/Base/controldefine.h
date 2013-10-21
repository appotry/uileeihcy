#ifndef CONTROLDEFINE_H_92376BDC_456B_4a0a_9C9F_3656DEDD4F79
#define CONTROLDEFINE_H_92376BDC_456B_4a0a_9C9F_3656DEDD4F79

// 导入导出定义
#ifdef UICTRLS_EXPORTS
#define UICTRLAPI __declspec(dllexport)
#else
#define UICTRLAPI __declspec(dllimport)
#endif

namespace UI
{
// 最后两位是Control Sub Type标识

// button style
#define BUTTON_STYLE_HAVE_TEXT_FIRST    0x00    // 标志有内容的按钮。这些按钮才需要创建字体textrender
#define BUTTON_STYLE_PUSHBUTTON         0x00    // 普通按钮
#define BUTTON_STYLE_RADIOBUTTON        0x01    // 单选按钮
#define BUTTON_STYLE_CHECKBUTTON        0x02    // 复选按钮
#define BUTTON_STYLE_HYPERLINK          0x03    // 超链接按钮
#define BUTTON_STYLE_MENUBUTTON         0x04    // 菜单按钮
#define BUTTON_STYLE_SPLITMENUBUTTON    0x05    // 按钮+菜单
#define BUTTON_STYLE_TABCTRLBUTTON      0x06    // TAB控件上的迎宾馆
#define BUTTON_STYLE_HAVE_TEXT_LAST     0x80

#define BUTTON_STYLE_COMBOBOX           0x81    // 组合框中的按钮
#define BUTTON_STYLE_SCROLLLINEUP       0x82    // 滚动条按钮
#define BUTTON_STYLE_SCROLLLINEDOWN     0x83
#define BUTTON_STYLE_SCROLLLINELEFT     0x84
#define BUTTON_STYLE_SCROLLLINERIGHT    0x85
#define BUTTON_STYLE_HSCROLLTHUMB       0x86    // 滚动条拖拽按钮
#define BUTTON_STYLE_VSCROLLTHUMB       0x87    // 滚动条拖拽按钮
#define BUTOTN_STYLE_SLIDERTRACKBUTTON  0x88    // 滑动条上面的按钮

#define BUTTON_STYLE_NOTDEFPUSH         0x0100  // 不成为默认按钮
#define BUTTON_STYLE_CLICK_ONDOWN       0x0200  // 当鼠标按下时就触发click，而不是鼠标弹起时触发

#define SLIDER_STYLE_POINT_LEFT         0x0100    // 滑块按钮箭头指针左侧 默认为无指向
#define SLIDER_STYLE_POINT_TOP          0x0100    // 滑块按钮箭头指针上侧
#define SLIDER_STYLE_POINT_RIGHT        0x0200    // 滑块按钮箭头指针右侧
#define SLIDER_STYLE_POINT_BOTTOM       0x0200    // 滑块按钮箭头指针下侧
#define SLIDER_STYLE_SHOWTICKS          0x0400    // 显示刻度，默认只显示第一个和最后一个值的刻度
#define SLIDER_STYLE_AUTOTICKS          0x0800    // 自动显示刻度，默认每增一个值显示一个刻度

    // edit style
#define EDIT_STYLE_MASK                 0x00FF    // 编辑框类型使用的位
#define EDIT_STYLE_COMBOBOX             0x0001    // 组合框中使用的编辑框
#define EDIT_STYLE_WANTTAB              0x0100    // 接收TAB键消息
#define EDIT_STYLE_READONNLY            0x0200    // 只读

    // tree style
#define TREE_STYLE_HASLINE              0x0100    // 绘制虚线

    // listbox style

    // menu style
#define MENU_STYLE_AUTO_DELETE_SUBMENU  0x1000    // 父窗口销毁时，是否自动释放子菜单内存。主要用于从xml中load的菜单
    // 因为是内部创建的子菜单，内部负责销毁子菜单。而外部自己create出来
    // combobox style
#define COMBOBOX_STYLE_DROPDOWN         0x0100    // 不可编辑下拉列表
#define COMBOBOX_STYLE_EDITDROPDOWNLIST 0x0200    // 可编辑下拉列表

    // headerctrl style
#define HEADERCTRL_STYLE_CANDRAG        0x0100    // 可拖拽各列宽度

// tab ctrl
#define TABCTRL_STYLE_BOTTOM            0x0100    // 按钮位于下侧

}
#endif  // CONTROLDEFINE_H_92376BDC_456B_4a0a_9C9F_3656DEDD4F79