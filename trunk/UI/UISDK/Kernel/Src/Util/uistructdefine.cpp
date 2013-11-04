#include "stdafx.h"
#include "UISDK\Kernel\Inc\Util\uistructdefine.h"

namespace UI
{
Color::Color()
{
    m_col = 0;
    lRef = 0;
}
Color::Color(int _r, int _g, int _b, int _a)
{
    lRef = 0;
    this->a = _a;
    this->r = _r;
    this->g = _g;
    this->b = _b;
}
Color::Color(DWORD rgba)
{
    lRef = 0;
    m_col = rgba;
}
Color* Color::CreateInstance(COLORREF color)
{
    Color* p = new Color(color);
    p->AddRef();
    return p;
}

Color::operator COLORREF()
{
    COLORREF c = m_col;
    c &= 0x00FFFFFF;
    return c;
}

int   Color::GetR() { return r; }
int   Color::GetG() { return g; }
int   Color::GetB() { return b; }
int   Color::GetA() { return a; }

void  Color::SetR(byte n) { r = n; }
void  Color::SetG(byte n) { g = n; }
void  Color::SetB(byte n) { b = n; }
void  Color::SetA(byte n) { a = n; }

// ±£ÁôALPHAWÖµÑÕÉ«¿½±´
void  Color::ReplaceRGB(COLORREF c)
{
    byte a_old = a;
    m_col = c;
    a = a_old;
}
void  Color::ReplaceRGB(BYTE r1, BYTE g1, BYTE b1)
{
    r = r1; g = g1; b = b1;
}
void  Color::SetColor(COLORREF c)
{
    m_col = c;
    a = 255;
}

COLORREF  Color::GetGDICompatibleValue()
{
    COLORREF gdiColor = 0;
    if (255 == a)
    {
        gdiColor = RGB(r, g, b);
    }
    else if (0 == a)
    {
    }
    else
    {
        Color c;
        byte br = r * a >> 8;
        byte bg = g * a >> 8;
        byte bb = b * a >> 8;

        gdiColor = RGB(c.r, c.g, c.b);
    }

    return gdiColor;
}

long  Color::AddRef() { ++ lRef;  return lRef; }
long  Color::Release()
{
    --lRef;
    if (lRef == 0)
    {
        delete this;
        return 0;
    }
    return lRef;
}

}