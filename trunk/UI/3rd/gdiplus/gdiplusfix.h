#ifndef GDIPLUSFIX_H_3A47716E_9489_41f8_83CF_1B38CE313702
#define GDIPLUSFIX_H_3A47716E_9489_41f8_83CF_1B38CE313702

// �����ڴ�й¶�����Ƶ�new�ض�����Gdiplusbase�ж����new�г�ͻ�������������Ҫ���θù���
// ����֮����Ȼ�ܼ������ڴ�й¶�������޷���ʾ����������
// #undef new

// Ϊ�˽��GdiplusBase�е� new �������ڴ�й¶�������е�new�����ͻ�����⣬��GdiplusBase�������޸�
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