#ifndef IIMAGE_H_FCD7CA75_5E1A_4165_9FFC_E37EA138A5B1
#define IIMAGE_H_FCD7CA75_5E1A_4165_9FFC_E37EA138A5B1

namespace UI
{
class Image;
interface UISDKAPI IImage
{
    IImage();
    ~IImage();
    Image*  GetImpl();

    static const DWORD createAlphaChannel = 0x01;

    void  Attach(HBITMAP hBitmap);
    HBITMAP  Detach();
    bool  Load(const TCHAR*  szPath);

    BOOL  Create(int nWidth, int nHeight, int nBPP, DWORD dwFlags = 0);
    void  Destroy();
    bool  IsNull();

    int   GetWidth();
    int   GetHeight();
    int   GetPitch();
    byte* GetBits();
    int   GetBPP();

    HDC   BeginDrawToMyself();
    void  EndDrawToMyself();
    BOOL  BitBlt(HDC hDestDC, int xDest, int yDest, int nDestWidth, int nDestHeight, int xSrc, int ySrc, DWORD dwROP = SRCCOPY);
    void  Draw(HDC hDestDC, int x, int y);

    void  AlphaBottomEdge(int nSpeed=2);
    void  AlphaTopEdge(int nSpeed=2);
    void  AlphaLeftEdge(int nSpeed=2);
    void  AlphaRightEdge(int nSpeed=2);

    DWORD  GetAverageColor();
    COLORREF  GetPixel(int x, int y);
    void  SaveAsPng(const TCHAR* szPath);

private:
    Image*  m_pImageImpl;
};
}

#endif // IIMAGE_H_FCD7CA75_5E1A_4165_9FFC_E37EA138A5B1