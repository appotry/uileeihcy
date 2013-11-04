#pragma once
#include "UISDK\Kernel\Src\RenderLayer\bufferrenderlayer.h"

namespace UI
{

class WindowRenderLayer : public BufferRenderLayer
{
public:
    void  SetIWindowRenderLayer(IWindowRenderLayer*){}

    virtual void  Draw();
};

}