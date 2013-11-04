#pragma once
#include "renderlayer.h"

namespace UI
{

class DirectRenderLayer : public RenderLayer
{
public:
    DirectRenderLayer();
    ~DirectRenderLayer();

    UI_DECLARE_OBJECT(DirectRenderLayer, OBJ_LAYER|LAYER_DIRECT)
    UI_BEGIN_MSG_MAP
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderLayer)

    void  SetIDirectRenderLayer(IDirectRenderLayer* p) { m_pIDirectRenderLayer = p; }

public:
    virtual void  Draw() {};
	virtual void  Commit(HDC hDstDC);

protected:
    IDirectRenderLayer*  m_pIDirectRenderLayer;
};

}