#pragma once
#include "UISDK\Kernel\Src\UIObject\Panel\panel.h"
#include "UISDK\Kernel\Inc\Interface\irenderlayer.h"

namespace UI{
class RenderChain;
    
class RenderLayer : public Panel
{
public:
	RenderLayer();
	virtual ~RenderLayer();

    UI_DECLARE_OBJECT(RenderLayer, OBJ_LAYER);
    UI_BEGIN_MSG_MAP
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_OBJECTLOADED(OnObjectLoaded)
    UI_END_MSG_MAP_CHAIN_PARENT(Panel)

    void  SetIRenderLayer(IRenderLayer* p) { m_pIRenderLayer = p; }
    IRenderLayer*  GetIRenderLayer() { return m_pIRenderLayer; }

public:
	RenderLayer* GetNextLayer();
	RenderLayer* GetPrevLayer();
	void  SetNextLayer(RenderLayer* p);
	void  SetPrevLayer(RenderLayer* p);

	void  SetWindowPtr(WindowBase* p) { m_pWindow = p; }
    WindowBase*  GetWindowPtr() { return m_pWindow; }
	void  SetRenderChainPtr(RenderChain* p) { m_pRenderChain = p; }
    RenderChain*  GetRenderChainPtr() { return m_pRenderChain; }

	int   GetZorder() { return m_nZorder; }
	void  SetLayerEnable(bool b) { m_bLayerEnable = b; }
	bool  IsLayerEnable() { return m_bLayerEnable; }

    HDC   GetMemoryLayerDC(){ return m_hMemDC; };
    IRenderTarget*  GetRenderTarget() { return m_pRenderTarget; }

    virtual void  Draw(){};
    virtual void  Commit(HDC hDstDC){};

protected:
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
    void  OnObjectLoaded();

protected:
    IRenderLayer*  m_pIRenderLayer;
	WindowBase*   m_pWindow;
	RenderChain*  m_pRenderChain;

	RenderLayer*  m_pNextLayer;
	RenderLayer*  m_pPrevLayer;

	int     m_nZorder;
	bool    m_bLayerEnable;   // 当为FALSE，不进行渲染

	// 注：
	//   为什么不可以每次绘制或者redrawobject时再创建RenderTarget?
	//   在绘制windowless richedit时发现，会出现背景显示为左上角的数据。后来查到原因为rendertarget重复创建，
	//   导致m_hMemDC的属性在第二次创建的rendertarget中被修改，再返回到第一次创建的rendertarget中时，属性已全乱了。
	//   因此重新将RenderTarget做成一个成员变量
	//
	IRenderTarget*   m_pRenderTarget;  // 在第一次绘制时创建
    HDC  m_hMemDC;
};

}