#ifndef IRENDERLAYER_H_8500D1A2_903C_4c76_B3B0_5248E65E95C3
#define IRENDERLAYER_H_8500D1A2_903C_4c76_B3B0_5248E65E95C3
#include "UISDK\Kernel\Inc\Interface\ipanel.h"

namespace UI
{
class  IRenderTarget;

class RenderLayer;
interface UISDKAPI IRenderLayer : public IPanel
{
    UI_DECLARE_Ixxx_INTERFACE(IRenderLayer, RenderLayer);

    HDC   GetMemoryLayerDC();
    IRenderTarget*  GetRenderTarget();

    void  SetLayerEnable(bool b);
    bool  IsLayerEnable();
};

class BufferRenderLayer;
interface UISDKAPI IBufferRenderLayer : public IRenderLayer
{
    UI_DECLARE_Ixxx_INTERFACE(IBufferRenderLayer, BufferRenderLayer);

    bool  SaveLayer(const TCHAR*  szPath);
};

class ControlRenderLayer;
interface UISDKAPI IControlRenderLayer : public IBufferRenderLayer
{
    UI_DECLARE_Ixxx_INTERFACE(IControlRenderLayer, ControlRenderLayer);
};

class WindowRenderLayer;
interface UISDKAPI IWindowRenderLayer : public IBufferRenderLayer
{
    UI_DECLARE_Ixxx_INTERFACE(IWindowRenderLayer, WindowRenderLayer);
};

class DirectRenderLayer;
interface UISDKAPI IDirectRenderLayer : public IRenderLayer
{
    UI_DECLARE_Ixxx_INTERFACE(IDirectRenderLayer, DirectRenderLayer)
};


class RenderChain;
class RenderContext;
interface UISDKAPI IRenderChain
{
    IRenderChain();
    ~IRenderChain();
    RenderChain*  GetImpl();

    IRenderLayer*  FindLayer(const TCHAR*  szLyerID);

    IRenderTarget*  BeginRedrawObjectPart(IObject* pRedrawObj, RECT* prcArray, int nCount, bool* pbIsDrawing, RenderContext* pOutRenderContext);
    void  EndRedrawObjectPart(IRenderTarget* pRenderTarget, RECT* prcArray, int nCount);

    void  SetCanCommit(bool b);
    bool  CanCommit();

private:
    RenderChain*  m_pImpl;
};
}

#endif // IRENDERLAYER_H_8500D1A2_903C_4c76_B3B0_5248E65E95C3