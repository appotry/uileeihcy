#ifndef IIMAGERENDER_H_7A0B277E_3586_47a9_8C41_91AE173EBB0C
#define IIMAGERENDER_H_7A0B277E_3586_47a9_8C41_91AE173EBB0C
#include "UISDK\Kernel\Inc\Interface\irenderbase.h"

namespace UI
{

class ImageRender;
interface UISDKAPI IImageRender : public IRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IImageRender, ImageRender);

    void  SetAlpha(int nAlpha);
    int   GetAlpha();
    void  SetColor(Color c);
    Color GetColor();
    void  SetDrawType(int n);
    int   GetDrawType();

    IRenderBitmap*  GetRenderBitmap();
    void  SetRenderBitmap(IRenderBitmap* pBitmap);
};

class ImageListItemRender;
interface UISDKAPI IImageListItemRender : public IImageRender
{
    UI_DECLARE_Ixxx_INTERFACE(IImageListItemRender, ImageListItemRender)

};

class ImageListRender;
interface IImageListRenderBitmap;
interface UISDKAPI IImageListRender : public IRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IImageListRender, ImageListRender)

    void  SetIImageListRenderBitmap(IImageListRenderBitmap* pBitmap);
    void  SetImageDrawType(int n);
    int   GetImageDrawType();
    void  SetImageStretch9Region(Image9Region* p);
    int   GetItemWidth();
    int   GetItemHeight();
    int   GetItemCount();
};


}

#endif // IIMAGERENDER_H_7A0B277E_3586_47a9_8C41_91AE173EBB0C