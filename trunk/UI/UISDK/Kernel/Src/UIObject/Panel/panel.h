#ifndef PANEL_H_A91AD9AE_6579_4060_90A1_4FAD6D27D94B
#define PANEL_H_A91AD9AE_6579_4060_90A1_4FAD6D27D94B

#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

namespace UI
{

class Panel : public Object
{
public:
	Panel();
	~Panel();

	UI_DECLARE_OBJECT3(Panel, OBJ_PANEL, _T("Kernel/Container"))

	UI_BEGIN_MSG_MAP
		UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_SIZE(OnSize)
        UIMSG_WM_GETDESIREDSIZE(OnGetDesiredSize)
		UIMESSAGE_HANDLER_EX(UI_WM_GETLAYOUT, OnGetLayoutPtr)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
		UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
	UI_END_MSG_MAP_CHAIN_PARENT(Object)

    IPanel* GetIPanel() { return m_pIPanel; }
    void    SetIPanel(IPanel* p) { m_pIPanel = p; }

public:
    ILayout*  GetLayout();
    void  SetLayoutType(LAYOUTTYPE eLayoutType);

    void  SetMaterialRender(IRenderBase* p);
    IRenderBase*  GetMaterialRender();

    virtual void  RealDrawObject(IRenderTarget* pRenderTarget, RenderContext roc);

protected:
    void  OnSize(UINT nType, int cx, int cy);
	void  OnEraseBkgnd(IRenderTarget*);
	LRESULT  OnGetLayoutPtr(UINT uMsg, WPARAM wParam, LPARAM lParam);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  OnGetDesiredSize(SIZE* pSize);  // 命名为On,避免与object::GetDesiredSize冲突 
	void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

protected:
    IPanel*    m_pIPanel;
	ILayout*   m_pLayout;
	CRegion4   m_rcBkgndRenderRegion;
	CRegion4   m_rcForegndRenderRegion;

    // 用于支持换肤功能的皮肤图片  Texture(纹理-用foregnd实现) + Material(材质) = Surface(表面) ?
    IRenderBase*   m_pMaterialRender;
    CRegion4       m_rcMaterialRenderRegion;

    // 最上面的遮罩层
//     IRenderBase*   m_pMaskRender;  
//     CRegion4       m_rcMaskRenderRegion;
};

}

#endif // PANEL_H_A91AD9AE_6579_4060_90A1_4FAD6D27D94B