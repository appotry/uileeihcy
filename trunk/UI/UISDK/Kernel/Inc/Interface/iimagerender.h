#ifndef IIMAGERENDER_H_7A0B277E_3586_47a9_8C41_91AE173EBB0C
#define IIMAGERENDER_H_7A0B277E_3586_47a9_8C41_91AE173EBB0C
#include "UISDK\Kernel\Inc\Interface\irenderbase.h"

namespace UI
{

enum BKCOLOR_FILL_TYPE
{
    BKCOLOR_FILL_ALL,    // 默认
    BKCOLOR_FILL_EMPTY   // 只在背景图没有绘制的地方进行填充。因为如果先fill all，再画透明位图的话，位图的背景色就被污染了。例如IM背景窗口皮肤图片与其背景色
};
class ImageRender;
interface UISDKAPI IImageRender : public IRenderBase
{
    UI_DECLARE_Ixxx_INTERFACE(IImageRender, ImageRender);

    void  SetAlpha(int nAlpha);
    int   GetAlpha();
    void  SetColor(Color c);
    Color GetColor();
    void  SetImageDrawType(int n);
    int   GetImageDrawType();

    BKCOLOR_FILL_TYPE  GetBkColorFillType();
    void  SetBkColorFillType(BKCOLOR_FILL_TYPE eType);

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
    IRenderBitmap*  GetRenderBitmap();
    void  SetImageDrawType(int n);
    int   GetImageDrawType();
    void  SetImageStretch9Region(Image9Region* p);
    int   GetItemWidth();
    int   GetItemHeight();
    int   GetItemCount();
};


}

#endif // IIMAGERENDER_H_7A0B277E_3586_47a9_8C41_91AE173EBB0C