#ifndef GDIPLUSFIX_H_3A47716E_9489_41f8_83CF_1B38CE313702
#define GDIPLUSFIX_H_3A47716E_9489_41f8_83CF_1B38CE313702

// 由于内存泄露检测机制的new重定义与Gdiplusbase中定义的new有冲突，因此在这里需要屏蔽该功能
// 屏蔽之后，虽然能检测出来内存泄露，但是无法显示代码行所在
// #undef new

// 为了解决GdiplusBase中的 new 定义与内存泄露检查机制中的new定义冲突的问题，将GdiplusBase进行了修改
#pragma push_macro("new")

#undef new

#ifndef _GDIPLUSBASE_H
#define _GDIPLUSBASE_H
#endif

namespace Gdiplus 
{ 
class GdiplusBase{ };
}

#include <gdiplus.h>

#pragma pop_macro("new")

#endif  // GDIPLUSFIX_H_3A47716E_9489_41f8_83CF_1B38CE313702