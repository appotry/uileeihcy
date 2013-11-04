#include "stdafx.h"
#include "imagerender.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Inc\Interface\ianimate.h"


ImageRender::ImageRender()
{
    m_pIImageRender = NULL;

	m_pBitmap = NULL;
	m_pColorBk = NULL;
	m_nImageDrawType = DRAW_BITMAP_BITBLT;
	m_pRegion = NULL;
    m_nAlpha = 255;
    m_prcSrc = NULL;
    m_eBkColorFillType = BKCOLOR_FILL_ALL;
}
ImageRender::~ImageRender()
{
	SAFE_RELEASE(m_pBitmap);
	SAFE_RELEASE(m_pColorBk);
	SAFE_DELETE(m_pRegion);
    SAFE_DELETE(m_prcSrc);
}

void  ImageRender::SetAlpha(int nAlpha)
{
    m_nAlpha = nAlpha;
}
int   ImageRender::GetAlpha()
{
    return m_nAlpha;
}

Color  ImageRender::GetColor()
{
    if (NULL == m_pColorBk)
        return Color(0);
    else
        return *m_pColorBk;
}

void  ImageRender::SetImageDrawType(int n)
{
    m_nImageDrawType = n;
}
int   ImageRender::GetImageDrawType()
{
    return m_nImageDrawType;
}

void ImageRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

    __super::SetAttribute(pData);
    
	IImageRes* pImageRes = GetActiveSkinImageRes();
	IColorRes* pColorRes = GetActiveSkinColorRes();
	if (NULL == pImageRes || NULL == pColorRes)
		return;

    String  strPrefix;
    if (szPrefix)
        strPrefix = szPrefix;

    // 背景图片
	String strAttrib = strPrefix;
	strAttrib.append(XML_RENDER_IMAGE);
	const TCHAR* szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
    {
        if (m_pObject)
		    pImageRes->GetBitmap((BSTR)szText, ::GetRenderLibraryType(m_pObject->GetIObject()), &m_pBitmap);
        else
            pImageRes->GetBitmap((BSTR)szText, UI::GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS, &m_pBitmap);  // TODO: 
    }

    // 背景颜色
	strAttrib = strPrefix;  
	strAttrib.append(XML_RENDER_COLOR);

	szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
		pColorRes->GetColor((BSTR)szText, &m_pColorBk);

    // 拉伸区域
	strAttrib = strPrefix;
	strAttrib.append(XML_RENDER_IMAGE9REGION);
	szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
	{
		SAFE_DELETE(m_pRegion);
		m_pRegion = new Image9Region;
		Util::TranslateImage9Region(szText, (void*)m_pRegion );
	}

    // 绘制类型
	strAttrib = strPrefix;
	strAttrib.append(XML_RENDER_IMAGE_DRAWTYPE);
    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
    if (szText)
	{
		if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_BITBLT, szText))
			this->SetImageDrawType(DRAW_BITMAP_BITBLT);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_TILE, szText))
			this->SetImageDrawType(DRAW_BITMAP_TILE);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_STRETCH, szText))
			this->SetImageDrawType(DRAW_BITMAP_STRETCH);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_ADAPT, szText))
			this->SetImageDrawType(DRAW_BITMAP_ADAPT);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_CENTER, szText))
			this->SetImageDrawType(DRAW_BITMAP_CENTER);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_STRETCH_BORDER, szText))
			this->SetImageDrawType(DRAW_BITMAP_STRETCH_BORDER);
        else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_BITBLT_RIGHTTOP, szText))
            this->SetImageDrawType(DRAW_BITMAP_BITBLT_RIGHTTOP);
        else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_BITBLT_LEFTBOTTOM, szText))
            this->SetImageDrawType(DRAW_BITMAP_BITBLT_LEFTBOTTOM);
        else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_BITBLT_RIGHTBOTTOM, szText))
            this->SetImageDrawType(DRAW_BITMAP_BITBLT_RIGHTBOTTOM);
	}

    // 绘制透明度
    strAttrib = strPrefix;
    strAttrib.append(XML_RENDER_IMAGE_ALPHA);
    pMapAttrib->GetAttr_int(strAttrib.c_str(), pData->bErase, &m_nAlpha);

    // 源区域
    strAttrib = strPrefix;
    strAttrib .append(XML_RENDER_IMAGE_SRC_REGION);
    CRect  rc;
    if (MAPATTR_RET_OK == pMapAttrib->GetAttr_CRegion4(strAttrib.c_str(), pData->bErase, &rc))
    {
        SAFE_DELETE(m_prcSrc);
        m_prcSrc = new CRect;
        m_prcSrc->CopyRect(&rc);
    }
} 

void  ImageRender::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_IMAGE, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_COLOR, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_IMAGE9REGION, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_IMAGE_DRAWTYPE, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_IMAGE_ALPHA, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_IMAGE_SRC_REGION, szPrefix);
}

void ImageRender::SetRenderBitmap(IRenderBitmap* pBitmap)
{
	SAFE_RELEASE(m_pBitmap);
	m_pBitmap = pBitmap;
	if (m_pBitmap)
		m_pBitmap->AddRef();
}

void  ImageRender::SetColor(Color c)
{
    SAFE_RELEASE(m_pColorBk);
    m_pColorBk = Color::CreateInstance(c);
}

void ImageRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
    CRect* prc = &pDrawStruct->rc;

	if (m_pColorBk && m_eBkColorFillType == BKCOLOR_FILL_ALL)
    {
        Color c = m_pColorBk->m_col;
        c.a = m_nAlpha;

        pDrawStruct->pRenderTarget->FillRect(prc, &c);
    }

    CRect rcRealDraw(0, 0, 0, 0);
	if (m_pBitmap)
	{
		DRAWBITMAPPARAM param;
		param.nFlag = m_nImageDrawType;
		param.xDest = prc->left;
		param.yDest = prc->top;
		param.wDest = prc->Width();
		param.hDest = prc->Height();
        if (m_prcSrc)
        {
		    param.xSrc  = m_prcSrc->left;
		    param.ySrc  = m_prcSrc->top;
		    param.wSrc  = m_prcSrc->Width();
		    param.hSrc  = m_prcSrc->Height();
        }
        else if (m_pBitmap)
        {
            param.xSrc  = 0;
            param.ySrc  = 0;
            param.wSrc  = m_pBitmap->GetWidth();
            param.hSrc  = m_pBitmap->GetHeight();
        }
		param.pRegion = m_pRegion;
        param.nAlpha  = m_nAlpha;

		if (pDrawStruct->nState & RENDER_STATE_DISABLE)
		{
			param.nFlag |= DRAW_BITMAP_DISABLE;
		}

        if (m_pColorBk && m_eBkColorFillType == BKCOLOR_FILL_EMPTY)
        {
            param.prcRealDraw = &rcRealDraw;
        }
		pDrawStruct->pRenderTarget->DrawBitmap(m_pBitmap, &param);
	}

    if (m_pColorBk && m_eBkColorFillType == BKCOLOR_FILL_EMPTY)
    {
        Color c = m_pColorBk->m_col;
        c.a = m_nAlpha;

        // Top
        {
            CRect rc(prc->left, prc->top, prc->right, rcRealDraw.top);
            if (rc.Width() > 0 && rc.Height() > 0)
            {
                pDrawStruct->pRenderTarget->FillRect(&rc, &c);
            }
        }
        // Left
        {
            CRect rc(prc->left, rcRealDraw.top, rcRealDraw.left, rcRealDraw.bottom);
            if (rc.Width() > 0 && rc.Height() > 0)
            {
                pDrawStruct->pRenderTarget->FillRect(&rc, &c);
            }
        }
        // Right
        {
            CRect rc(rcRealDraw.right, rcRealDraw.top, prc->right, rcRealDraw.bottom);
            if (rc.Width() > 0 && rc.Height() > 0)
            {
                pDrawStruct->pRenderTarget->FillRect(&rc, &c);
            }
        }
        // Bottom
        {
            CRect rc(prc->left, rcRealDraw.bottom, prc->right, prc->bottom);
            if (rc.Width() > 0 && rc.Height() > 0)
            {
                pDrawStruct->pRenderTarget->FillRect(&rc, &c);
            }
        }
    }
}
void ImageRender::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = pSize->cy = 0;
	if (NULL == m_pBitmap)
		return;

	pSize->cx = m_pBitmap->GetWidth();
	pSize->cy = m_pBitmap->GetHeight();
}

//////////////////////////////////////////////////////////////////////////

ImageListItemRender::ImageListItemRender()
{
    m_pIImageListItemRender = NULL;
	m_nImagelistIndex = -1;
	m_pImageList = NULL;
}
ImageListItemRender::~ImageListItemRender()
{
	m_nImagelistIndex = 0;
	m_pImageList = NULL;
}

void ImageListItemRender::SetAttribute(SetAttrPrefixData* pData) 
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

	__super::SetAttribute(pData);

    String strAttrib;
    if (szPrefix)
        strAttrib = szPrefix;

    strAttrib.append(XML_RENDER_IMAGELISTITEM_INDEX);
    pMapAttrib->GetAttr_int(strAttrib.c_str(), pData->bErase, &m_nImagelistIndex);
    
	if (m_pBitmap)
    {
        if (m_pBitmap->GetImageType() != IMAGE_ITEM_TYPE_IMAGE_LIST)
            SAFE_RELEASE(m_pBitmap);
    }
    if (m_pBitmap)
    {
		m_pImageList = static_cast<IImageListRenderBitmap*>(m_pBitmap);

        SAFE_DELETE(m_prcSrc);
        m_prcSrc = new CRect;

        POINT pt = {0,0};
        m_pImageList->GetIndexPos(m_nImagelistIndex, &pt);
        m_prcSrc->left = pt.x;
        m_prcSrc->top = pt.y;

        SIZE s;
        this->GetDesiredSize(&s);
        m_prcSrc->right = m_prcSrc->left + s.cx;
        m_prcSrc->bottom = m_prcSrc->top + s.cy;
    }
}

void  ImageListItemRender::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = 0;
    pSize->cy = 0;
	if (NULL == m_pImageList )
		return;

	pSize->cx = m_pImageList->GetItemWidth();
	pSize->cy = m_pImageList->GetItemHeight();
}

void ImageListItemRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
	if (DRAW_BITMAP_TILE == m_nImageDrawType)
	{
		UI_LOG_WARN(_T("%s image list item donot support tile draw"), FUNC_NAME);
		UIASSERT(0 && _T("TODO:"));
		return;
	}

    if (-1 == m_nImagelistIndex && m_pImageList)
    {
        POINT pt = {0, 0};
        SIZE  s = {0, 0};

        if (false == m_pImageList->GetIndexPos(pDrawStruct->nState, &pt))
            return;
        this->GetDesiredSize(&s);

        if (NULL == m_prcSrc)
            m_prcSrc = new CRect;

        m_prcSrc->SetRect(pt.x, pt.y, pt.x+s.cx, pt.y+s.cy);
    }

	SetMsgHandled(FALSE);
}


//////////////////////////////////////////////////////////////////////////////////////////
//                                                                                      //
//                               ImageListRender                                        //
//                                                                                      //
//////////////////////////////////////////////////////////////////////////////////////////

ImageListRender::ImageListRender( )
{
    m_pIImageListRender = NULL;

//	m_eImageLayout = IMAGELIST_LAYOUT_TYPE_H;
	m_pImageList = NULL;
//	m_nItemWidth = m_nItemHeight = 0;
//	m_nCount = 0;
	m_p9Region = NULL;
	m_nImageDrawType = DRAW_BITMAP_BITBLT;

	m_nPrevState = RENDER_STATE_NORMAL;
	m_nCurrentAlpha = 255;
	m_bIsAnimate = false;
	m_bUseAlphaAnimate = false;
}
ImageListRender::~ImageListRender( )
{
	SAFE_RELEASE(m_pImageList);
	SAFE_DELETE(m_p9Region);

	// 删除动画通知，避免崩溃
	if (m_bUseAlphaAnimate && m_bIsAnimate)
	{
		IUIApplication* pUIApp = m_pObject->GetUIApplication();
		IAnimateManager* pAnimateMgr = pUIApp->GetAnimateMgr();
		pAnimateMgr->ClearStoryboardOfNotify(static_cast<IMessage*>(m_pIImageListRender));
		m_bIsAnimate = false;
	}
}

void ImageListRender::SetAttribute(SetAttrPrefixData* pData)
{
    IMapAttribute* pMapAttrib = pData->pMapAttrib;
    const TCHAR* szPrefix = pData->szPrefix;

	IImageRes* pImageRes = GetActiveSkinImageRes();
	if (NULL == pImageRes)
		return;

	RenderBase::SetAttribute(pData);

// 	String strAttrib = strPrefix; strAttrib.append(XML_RENDER_IMAGELIST_COUNT);
// 	ATTRMAP::iterator iter = mapAttrib.find(strAttrib);
// 	if (mapAttrib.end() != iter)
// 	{
// 		m_nCount = _ttoi(iter->second.c_str());
// 		mapAttrib.erase(iter);
// 	}
// 
// 	if (0 == m_nCount)
// 		return true;

    String strPrefix;
    if (szPrefix)
        strPrefix = szPrefix;

	String strAttrib = strPrefix;
	strAttrib.append(XML_RENDER_IMAGELIST_ALPHA_ANIMATE);
    pMapAttrib->GetAttr_bool(strAttrib.c_str(), pData->bErase, &m_bUseAlphaAnimate);
	
// 	strAttrib = strPrefix;
// 	strAttrib.append(XML_RENDER_IMAGELIST_ALPHA_ANIMATE_HOVER_ON_NORMAL);
//     pMapAttrib->GetAttr_bool(strAttrib.c_str(), pData->bErase, &m_bHoverBaseOnNormal);

	strAttrib = strPrefix;
	strAttrib.append(XML_RENDER_IMAGE);
	const TCHAR* szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
	{
		SAFE_RELEASE(m_pImageList);
		IRenderBitmap* pBitmap = NULL;
		pImageRes->GetBitmap((BSTR)szText, ::GetRenderLibraryType(m_pObject->GetHWND()), &pBitmap);
		if (NULL == pBitmap)
			return;

        if (pBitmap->GetImageType() != IMAGE_ITEM_TYPE_IMAGE_LIST)
        {
            UI_LOG_WARN(_T("%s ImageType != IMAGE_ITEM_TYPE_IMAGE_LIST. Bitmap: %s"), FUNC_NAME, szText);
            SAFE_RELEASE(pBitmap);
            return;
        }
		m_pImageList = static_cast<IImageListRenderBitmap*>(pBitmap);

		// 如果图片没有alpha通道，则不能支持alpha动画
		if (m_pImageList)
		{
			if (m_pImageList->GetBPP() != 32)
			{
				m_bUseAlphaAnimate = false;
			}
		}
	}

    strAttrib = strPrefix;
    strAttrib.append(XML_RENDER_LIST_STATEMAPINDEX);
    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
    if (szText)
    {
        m_mapState2Index.clear();

        Util::ISplitStringEnum* pEnum = NULL;
        int nCount = Util::SplitString(szText, XML_SEPARATOR, &pEnum);
        for (int i = 0; i < nCount; i++)
        {
            const TCHAR* szStateIndex = pEnum->GetText(i);
            
            Util::ISplitStringEnum* pEnumInner = NULL;
            if (2 != Util::SplitString(szStateIndex, _T(':'), &pEnumInner))
            {
                UI_LOG_WARN(_T("%s invalid state index: %s"), FUNC_NAME, szStateIndex);
                SAFE_RELEASE(pEnumInner);
                continue;
            }
            int nState = _ttoi(pEnumInner->GetText(0));
            int nIndex = _ttoi(pEnumInner->GetText(1));
            m_mapState2Index[nState] = nIndex;
            SAFE_RELEASE(pEnumInner);
        }
        SAFE_RELEASE(pEnum);
    }

	if (NULL == m_pImageList)
		return;

	strAttrib = strPrefix;
	strAttrib.append(XML_RENDER_IMAGE9REGION);
	szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
	if (szText)
	{
		SAFE_DELETE(m_p9Region);
		m_p9Region = new Image9Region;
		Util::TranslateImage9Region(szText, (void*)m_p9Region );
	}

	strAttrib = strPrefix;
	strAttrib.append(XML_RENDER_IMAGE_DRAWTYPE);
    szText = pMapAttrib->GetAttr(strAttrib.c_str(), pData->bErase);
    if (szText)
	{
		if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_BITBLT, szText))
			this->SetImageDrawType(DRAW_BITMAP_BITBLT);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_TILE, szText))
			this->SetImageDrawType(DRAW_BITMAP_TILE);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_STRETCH, szText))
			this->SetImageDrawType(DRAW_BITMAP_STRETCH);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_ADAPT, szText))
			this->SetImageDrawType(DRAW_BITMAP_ADAPT);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_CENTER, szText))
			this->SetImageDrawType(DRAW_BITMAP_CENTER);
		else if (0 == _tcscmp(XML_RENDER_IMAGE_DRAWTYPE_STRETCH_BORDER, szText))
			this->SetImageDrawType(DRAW_BITMAP_STRETCH_BORDER);
	}
}

void  ImageListRender::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_IMAGELIST_ALPHA_ANIMATE, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_IMAGE, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_LIST_STATEMAPINDEX, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_IMAGE9REGION, szPrefix);
    pEditor->CreateTextAttribute(pData->pGroupAttr, XML_RENDER_IMAGE_DRAWTYPE, szPrefix);
}

void  ImageListRender::SetIImageListRenderBitmap(IImageListRenderBitmap* pBitmap)
{
    SAFE_RELEASE(m_pImageList);
    m_pImageList = pBitmap;
    if (m_pImageList)
        m_pImageList->AddRef();
}
IRenderBitmap*  ImageListRender::GetRenderBitmap()
{
    return m_pImageList;
}
void  ImageListRender::OnAnimateTick(int nCount, IStoryboard** ppArray)
{
    if (NULL == ppArray)
        return;

    if (NULL == ppArray[0])
        return;

    ppArray[0]->FindTimeline(0)->GetCurrentValue(&m_nCurrentAlpha);

    bool bFinish = ppArray[0]->IsFinish();
    m_pObject->UpdateObject();  // 注：在listbox这种控件中，会使用当前foregnd render连续绘制所有的item，导致Imagelistrender中的动画被不断的删除和添加，最后崩溃

    if (bFinish)
        m_bIsAnimate = false;
}

void  ImageListRender::DrawState(RENDERBASE_DRAWSTATE* pDrawStruct)
{
	if (NULL == m_pImageList)
		return;

    IRenderTarget*  pRenderTarget = pDrawStruct->pRenderTarget;
    CRect*  prc = &pDrawStruct->rc;
    int&  nState = pDrawStruct->nState;

	int nRenderState = (pDrawStruct->nState) & RENDER_STATE_MASK;
	int nRealIndex = LOWORD(nState);
// 	if (nRealIndex >= m_pImageList->GetItemCount())  // 用GetStateIndex获取
// 		nRealIndex = 0;

	if (false == m_bUseAlphaAnimate)
	{
		DrawIndexWidthAlpha(pRenderTarget, prc, nRealIndex, 255);
		return;
	}

	// 从Normal->Hover或者Hover->Normal时，开启动画计时
	if ((m_nPrevState&(RENDER_STATE_NORMAL|RENDER_STATE_DEFAULT)) && (nRenderState&RENDER_STATE_HOVER))
	{
		IUIApplication* pUIApp = m_pObject->GetUIApplication();
		IAnimateManager* pAnimateMgr = pUIApp->GetAnimateMgr();
        pAnimateMgr->ClearStoryboardOfNotify(static_cast<IMessage*>(m_pIImageListRender));

        IStoryboard*  pStoryboard = pAnimateMgr->CreateStoryboard();
        pStoryboard->SetNotifyObj(static_cast<IMessage*>(m_pIImageListRender));

		IIntLinearMove* pMoveAlgo = NULL;
        IIntTimeline* pTimeline = (IIntTimeline*)pStoryboard->CreateTimeline(
            TLV_INT, 0, ITMA_Linear, (IMoveAlgorithm**)&pMoveAlgo);
        pMoveAlgo->SetParam1(0, 255, 200);

		m_nCurrentAlpha = 0;  // 避免在第一次Tick响应之前被控件强制刷新了，结果此时的m_nCurrentAlpha不是计算得到的值。
		pStoryboard->Begin();

		m_bIsAnimate = true;
		DrawIndexWidthAlpha(pRenderTarget, prc, LOWORD(m_nPrevState), 255);
	}
	else if ((nRenderState&(RENDER_STATE_NORMAL|RENDER_STATE_DEFAULT)) && (m_nPrevState&RENDER_STATE_HOVER))
	{
		IUIApplication* pUIApp = m_pObject->GetUIApplication();
		IAnimateManager* pAnimateMgr = pUIApp->GetAnimateMgr();
        pAnimateMgr->ClearStoryboardOfNotify(static_cast<IMessage*>(m_pIImageListRender));

        IStoryboard*  pStoryboard = pAnimateMgr->CreateStoryboard();
        pStoryboard->SetNotifyObj(static_cast<IMessage*>(m_pIImageListRender));

        IIntLinearMove* pMoveAlgo = NULL;
		IIntTimeline* pTimeline = (IIntTimeline*)pStoryboard->CreateTimeline(
            TLV_INT, 0, ITMA_Linear, (IMoveAlgorithm**)&pMoveAlgo);
        pMoveAlgo->SetParam1(255, 0, 200);
	
        m_nCurrentAlpha = 255;
        pStoryboard->Begin();

		m_bIsAnimate = true;
		DrawIndexWidthAlpha(pRenderTarget, prc, LOWORD(m_nPrevState), 255);
	}
	else
	{
		if (m_bIsAnimate)
		{
			if (0==(nRenderState & (RENDER_STATE_NORMAL|RENDER_STATE_DEFAULT|RENDER_STATE_HOVER)))
			{
				// 在动画过程中按下了，应该立即停止动画
				IUIApplication* pUIApp = m_pObject->GetUIApplication();
				IAnimateManager* pAnimateMgr = pUIApp->GetAnimateMgr();
				pAnimateMgr->ClearStoryboardOfNotify(static_cast<IMessage*>(m_pIImageListRender));
				m_bIsAnimate = false;

				DrawIndexWidthAlpha(pRenderTarget, prc, nRealIndex, 255);
			}
			else
			{
				bool bSelected = (nRenderState & RENDER_STATE_SELECTED)?true:false;
                if (bSelected)  // checkbox
                {
                    DrawIndexWidthAlpha(pRenderTarget, prc, 4, 255-m_nCurrentAlpha);
                    DrawIndexWidthAlpha(pRenderTarget, prc, 5, m_nCurrentAlpha);
                }
                else   // pushbutton/defaultbutton
                {
                    bool bDefault = (nRenderState & RENDER_STATE_DEFAULT)?true:false;

                    DrawIndexWidthAlpha(pRenderTarget, prc, bDefault?4:0, 255-m_nCurrentAlpha); 
                    DrawIndexWidthAlpha(pRenderTarget, prc, 1, m_nCurrentAlpha);
                }
			}
		}
		else
		{
			DrawIndexWidthAlpha(pRenderTarget, prc, nRealIndex, 255);
		}
	}
	m_nPrevState = nState;
}

void  ImageListRender::DrawIndexWidthAlpha(IRenderTarget* pRenderTarget, const CRect* prc, int nIndex, byte bAlpha)
{
	if (NULL == m_pImageList)
		return;

//    UI_LOG_ERROR(_T("%s  nIndex=%d, nAlpha=%d"), FUNC_NAME, nIndex, bAlpha);
    int nRealIndex = GetStateIndex(nIndex);
    if (nRealIndex < 0)
        return;

	DRAWBITMAPPARAM param;
	param.nFlag = m_nImageDrawType;
	param.xDest = prc->left;
	param.yDest = prc->top;
	param.wDest = prc->Width();
	param.hDest = prc->Height();
	param.wSrc = m_pImageList->GetItemWidth();
	param.hSrc = m_pImageList->GetItemHeight();
	param.pRegion = m_p9Region;
	param.nAlpha = bAlpha;

	POINT pt = {0, 0};
	m_pImageList->GetIndexPos(nRealIndex, &pt);
	param.xSrc = pt.x;
	param.ySrc = pt.y;
// 	if (m_eImageLayout == IMAGELIST_LAYOUT_TYPE_H)
// 	{
// 		param.xSrc = nIndex*m_nItemWidth;
// 		param.ySrc = 0;
// 	}
// 	else
// 	{
// 		param.xSrc = 0;
// 		param.ySrc = nIndex*m_nItemHeight;
// 	}
	pRenderTarget->DrawBitmap(m_pImageList, &param);
}

void  ImageListRender::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = pSize->cy = 0;
	if (NULL == m_pImageList)
		return;

	pSize->cx = m_pImageList->GetItemWidth();
	pSize->cy = m_pImageList->GetItemHeight();
}

int  ImageListRender::GetItemWidth()
{
	if (NULL == m_pImageList)
		return 0;

	return m_pImageList->GetItemWidth();
}
int  ImageListRender::GetItemHeight()
{
	if (NULL == m_pImageList)
		return 0;

	return m_pImageList->GetItemHeight();
}
int   ImageListRender::GetItemCount()
{
    if (NULL == m_pImageList)
        return 0;

    return m_pImageList->GetItemCount();
}

void  ImageListRender::SetImageStretch9Region(Image9Region* p)
{
    if (NULL == p)
    {
        SAFE_DELETE(m_p9Region);
        return;
    }

    if (NULL == m_p9Region)
    {
        m_p9Region = new Image9Region;
    }
    *m_p9Region = *p;
}
//
//	获取指定状态对应的图片项
//
int  ImageListRender::GetStateIndex(int nState)
{
	if (NULL == m_pImageList)
		return -1;

	if (m_pImageList->GetItemCount() <= 0)
		return -1;

	map<int, int>::iterator iter = m_mapState2Index.end();
    if (m_mapState2Index.size() > 0)
        iter = m_mapState2Index.find(nState);

	if (m_mapState2Index.end() == iter)
	{
		// 没有配置该状态或者没有配置
		if (nState < m_pImageList->GetItemCount())  // 如果图片数量大于当前状态，直接取当前状态值对应的图片
		{
			return nState;
		}
		else                    // 状态值大于图片数量，取默认值0
		{
			return 0;
		}
	}
	else
	{
		int& nRet = iter->second;
		if (nRet < 0 || nRet >= m_pImageList->GetItemCount())
			return -1;

		return nRet;
	}
	return -1;
}

