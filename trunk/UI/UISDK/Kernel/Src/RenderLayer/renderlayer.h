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
	bool    m_bLayerEnable;   // ��ΪFALSE����������Ⱦ

	// ע��
	//   Ϊʲô������ÿ�λ��ƻ���redrawobjectʱ�ٴ���RenderTarget?
	//   �ڻ���windowless richeditʱ���֣�����ֱ�����ʾΪ���Ͻǵ����ݡ������鵽ԭ��Ϊrendertarget�ظ�������
	//   ����m_hMemDC�������ڵڶ��δ�����rendertarget�б��޸ģ��ٷ��ص���һ�δ�����rendertarget��ʱ��������ȫ���ˡ�
	//   ������½�RenderTarget����һ����Ա����
	//
	IRenderTarget*   m_pRenderTarget;  // �ڵ�һ�λ���ʱ����
    HDC  m_hMemDC;
};

}