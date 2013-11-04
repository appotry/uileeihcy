#pragma once
#include "renderlayer.h"
#include "UISDK\Kernel\Src\Atl\image.h"

namespace UI
{
    
class BufferRenderLayer : public RenderLayer
{
public:
    BufferRenderLayer();
    ~BufferRenderLayer();

    UI_DECLARE_OBJECT(BufferRenderLayer, OBJ_LAYER|LAYER_BUFFER)
    UI_BEGIN_MSG_MAP
        UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderLayer)

    void  SetIBufferRenderLayer(IBufferRenderLayer* p) { m_pIBufferRenderLayer = p; }

public:
    bool  SaveLayer(const TCHAR*  szPath);
    void  ReCreateRenderTarget();
    Image*  GetMemoryLayerBuffer() { return &m_buffer; }

    virtual void  Commit(HDC hDstDC);
    
protected:
    void  OnSize(UINT nType, int cx, int cy);

protected:
    IBufferRenderLayer*  m_pIBufferRenderLayer;
	Image    m_buffer;
	HBITMAP  m_hOldBitmap;
};

class ControlRenderLayer : public BufferRenderLayer
{
public:
    ControlRenderLayer() { m_pIControlRenderLayer = NULL; }
    UI_DECLARE_OBJECT(ControlRenderLayer, OBJ_LAYER|LAYER_BUFFER)
        
    UI_BEGIN_MSG_MAP
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(BufferRenderLayer)

    void  SetIControlRenderLayer(IControlRenderLayer* p){ m_pIControlRenderLayer = p; }

    virtual void  Draw();
    
private:
    IControlRenderLayer*  m_pIControlRenderLayer;
};


}

