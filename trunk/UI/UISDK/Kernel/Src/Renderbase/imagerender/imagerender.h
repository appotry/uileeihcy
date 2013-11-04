#pragma once 
#include "UISDK\Kernel\Src\Base\Message\message.h"
#include "UISDK\Kernel\Inc\Interface\iimagerender.h"
#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"

namespace UI
{

class ImageRender : public RenderBase
{
public:
	ImageRender();
	~ImageRender();

    UI_DECLARE_RENDERBASE(ImageRender, XML_RENDER_TYPE_IMAGE, RENDER_TYPE_IMAGE)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

    void  SetIImageRender(IImageRender* p) { m_pIImageRender = p; }

public:
    void  SetAlpha(int nAlpha);
    int   GetAlpha();
    void  SetColor(Color c);
    Color  GetColor();
    void  SetImageDrawType(int n);
    int   GetImageDrawType();

    BKCOLOR_FILL_TYPE  GetBkColorFillType() { return m_eBkColorFillType; }
    void  SetBkColorFillType(BKCOLOR_FILL_TYPE eType) { m_eBkColorFillType = eType; }

    IRenderBitmap*  GetRenderBitmap() { return m_pBitmap; }
    void  SetRenderBitmap(IRenderBitmap* pBitmap);

protected:
    void  SetAttribute(SetAttrPrefixData* pData);
    void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
	void  GetDesiredSize(SIZE* pSize);
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

protected:
    IImageRender*     m_pIImageRender;

	IRenderBitmap*    m_pBitmap;
	Color*            m_pColorBk;
	Image9Region*     m_pRegion;   // 拉伸绘制时才用
    CRect*            m_prcSrc;     // 绘制区域(为NULL表示未设置)，有可能只是绘制图片的一部分.子类可以通过修改该值实现某些需求
	int               m_nImageDrawType;
    BKCOLOR_FILL_TYPE  m_eBkColorFillType;
    int               m_nAlpha;

};

// 绘制图片列表中指定索引的一项
class ImageListItemRender : public ImageRender 
{
public:
	ImageListItemRender();
	~ImageListItemRender();

    UI_DECLARE_RENDERBASE(ImageListItemRender, XML_RENDER_TYPE_IMAGELISTITEM, RENDER_TYPE_IMAGELISTITEM)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(ImageRender)

    void  SetIImageListItemRender(IImageListItemRender* p) { m_pIImageListItemRender = p; }

	void  SetAttribute(SetAttrPrefixData* pData);
	void  GetDesiredSize(SIZE* pSize);
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);

protected:
    IImageListItemRender*  m_pIImageListItemRender;

	IImageListRenderBitmap*  m_pImageList;
	int  m_nImagelistIndex;
};


//
//	图片背景按钮
//
interface IStoryboard;
class ImageListRender : public RenderBase
{
public:
	ImageListRender( );
	~ImageListRender( );

    UI_DECLARE_RENDERBASE(ImageListRender, XML_RENDER_TYPE_IMAGELIST, RENDER_TYPE_IMAGELIST)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_RENDERBASE_DRAWSTATE(DrawState)
        UIMSG_WM_ANIMATE_TICK(OnAnimateTick)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

    void  SetIImageListRender(IImageListRender* p) { m_pIImageListRender = p; }
    void  SetIImageListRenderBitmap(IImageListRenderBitmap* pBitmap);
    IRenderBitmap*  GetRenderBitmap();

    void  SetImageDrawType(int n) { m_nImageDrawType = n; }
    int   GetImageDrawType() { return m_nImageDrawType; }
    void  SetImageStretch9Region(Image9Region* p);

	void  SetAttribute(SetAttrPrefixData* pData);
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
	void  DrawState(RENDERBASE_DRAWSTATE* pDrawStruct);
	void  GetDesiredSize(SIZE* pSize);
    void  OnAnimateTick(int nCount, IStoryboard** ppArray);

	int   GetItemWidth();
	int   GetItemHeight();
    int   GetItemCount();

	int   GetStateIndex(int nState);
protected:
	void  DrawIndexWidthAlpha(IRenderTarget*, const CRect* prc, int nIndex, byte bAlpha);

protected:
    IImageListRender*  m_pIImageListRender;

	IImageListRenderBitmap*   m_pImageList;
	int  m_nImageDrawType; 
	Image9Region*  m_p9Region;

	int  m_nPrevState;        // 用于判断当前状态变化是否需要进行渐变
	int  m_nCurrentAlpha;     // 动画过程中的alpha值
	bool m_bIsAnimate;        // 当前是否正在动画过程中
	bool m_bUseAlphaAnimate;  // 是否启用alpha渐变动画
	
	map<int,int>   m_mapState2Index;    // 要绘制的状态对应图片的哪个item
};

}