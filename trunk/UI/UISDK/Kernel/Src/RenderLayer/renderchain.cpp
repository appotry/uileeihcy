#include "stdafx.h"
#include "renderchain.h"

#include "UISDK\Kernel\Inc\Interface\iobject.h"
#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Inc\Interface\imapattr.h"
#include "UISDK\Kernel\Inc\Interface\ixmlwrap.h"
#include "UISDK\Kernel\Src\RenderLayer\bufferrenderlayer.h"
#include "UISDK\Kernel\Src\RenderLayer\directrenderlayer.h"
#include "UISDK\Kernel\Src\Helper\layout\canvaslayout.h"

// layout.xml RenderChain标签解析
HRESULT  RenderChain::LoadRenderChain(IUIElement* pUIElement, IUIApplication*, IObject* pObjParent, IObject**)
{
    if (NULL == pUIElement || NULL == pObjParent || OBJ_WINDOW != pObjParent->GetObjectType())
        return E_INVALIDARG;

    IWindowBase*  pWindow = (IWindowBase*)pObjParent;
    RenderChain*  pRenderChain = pWindow->GetImpl()->GetRenderChain();
    if (NULL == pRenderChain)
        return E_FAIL;

    IUIChildNodeEnum*  pChildEnum = pUIElement->EnumChild();
    if (NULL == pChildEnum)
        return E_FAIL;
    
    IUIElement*  pChildElement = NULL;
    while (pChildElement = pChildEnum->NextElement())
    {
        pRenderChain->OnNewChildElement(pChildElement);
        SAFE_RELEASE(pChildElement);
    }
    SAFE_RELEASE(pChildEnum);
    return true;
}
HRESULT  RenderChain::OnNewChildElement(IUIElement* pChildElement)
{
    CComBSTR  bstrTagName;
    pChildElement->GetTagName(&bstrTagName);

    RenderLayer*  pRenderLayer = NULL;
    if (bstrTagName == DirectRenderLayer::GetXmlName())
    {
        IDirectRenderLayer* pi = NULL;
        IDirectRenderLayer::CreateInstance(m_pWindow->GetUIApplication(), &pi);
        if (NULL == pi)
            return false;

        pRenderLayer = pi->GetImpl();
        pRenderLayer->SetWindowPtr(m_pWindow);  // 提前调用SetWindowPtr，保证子对象能够正常调用GetWindowObject
        pRenderLayer->SetRenderChainPtr(this);

        IMapAttribute*  pMapAttrib = NULL;
        pChildElement->GetAttribList(&pMapAttrib);

		SERIALIZEDATA data = {0};
		data.nFlag = SERIALIZEFLAG_LOAD;
		data.pMapAttrib = pMapAttrib;
		UISendMessage(pi, UI_WM_SERIALIZE, (WPARAM)&data);
//        UISendMessage(pi, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, (LPARAM)false);

        SAFE_RELEASE(pMapAttrib);
    }
    else if (bstrTagName == ControlRenderLayer::GetXmlName())
    {
        IControlRenderLayer*  pILayer = NULL;
        IControlRenderLayer::CreateInstance(m_pWindow->GetUIApplication(), &pILayer);
        pRenderLayer = pILayer->GetImpl();

        pRenderLayer->SetWindowPtr(m_pWindow);   // 提前调用SetWindowPtr，保证子对象能够正常调用GetWindowObject
        pRenderLayer->SetRenderChainPtr(this);

        ILayoutManager*  pLayoutMgr = m_pWindow->GetUIApplication()->GetActiveSkinLayoutMgr();
        if (pLayoutMgr)
            pLayoutMgr->LoadObjAttrAndChild(static_cast<IObject*>(pILayer), pChildElement);
    }

    if (pRenderLayer)
    {
        this->InsertLayer(pRenderLayer);
        return S_OK;
    }

    return E_FAIL;
}

RenderChain::RenderChain(IRenderChain* pIRenderChain)
{
    m_pIRenderChain = pIRenderChain;

    m_pWindow = NULL;
	m_hOldBitmap = NULL;
//	m_hMemBitmap = NULL;
	m_hMultiLayerMemDC = NULL;
	m_nLayerCount = 0;
	m_pWindowLayer = NULL;
    m_pFirstLayer = NULL;
    m_lRefCanCommit = 0;

    m_bFirstTimePaintWindow = true;
    SetCanCommit(false);
}
RenderChain::~RenderChain()
{
	RenderLayer* pLayer = m_pFirstLayer;
	while (pLayer)
	{
		RenderLayer* pLayerTemp = pLayer->GetNextLayer();
        if (pLayer != m_pWindowLayer)  // m_pWindowLayer其实就是一个Window对象，由外部负责销毁，在这里不能管
        {
            pLayer->GetIRenderLayer()->delete_this();
        }
		pLayer = pLayerTemp;
	}
	m_nLayerCount = 0;
	this->DestroyDoubleBuffer();
}

void  RenderChain::Init(WindowBase* pWindow)
{
    m_pWindow = pWindow;

    m_pWindowLayer = static_cast<WindowRenderLayer*>(pWindow);
    m_pFirstLayer = m_pWindowLayer;
    m_pWindowLayer->SetWindowPtr(pWindow);
    m_pWindowLayer->SetRenderChainPtr(this);
    m_nLayerCount++;
}

HDC  RenderChain::GetMemoryDC()
{
	if (NULL == m_hMultiLayerMemDC && NULL != m_pWindowLayer)
	{
        return m_pWindowLayer->GetMemoryLayerDC();
	}
	else
	{
		return m_hMultiLayerMemDC;
	}
	return NULL;
}
Image*  RenderChain::GetMemoryBuffer()
{
    if (NULL == m_hMultiLayerMemDC && NULL != m_pWindowLayer)
        return m_pWindowLayer->GetMemoryLayerBuffer();
    else
        return &m_MultiLayerBuffer;
}
int  RenderChain::GetLayerCount()
{
	return m_nLayerCount;
}
RenderLayer* RenderChain::GetLastLayer()
{
	RenderLayer* p = m_pFirstLayer;
	if (NULL == p)
		return NULL;

	while (NULL != p->GetNextLayer())
	{
		p = p->GetNextLayer();
	}

	return p;
}

IRenderLayer*  RenderChain::FindLayer(const TCHAR*  szLyerID)
{
	if (NULL == szLyerID)
		return NULL;

	RenderLayer* p = m_pFirstLayer;
	while (p)
	{
		if (0 == wcscmp(szLyerID, p->GetId()))
			return p->GetIRenderLayer();

		p = p->GetNextLayer();
	}

	return NULL;
}

bool RenderChain::InsertLayer(RenderLayer*  p)
{
    if (NULL == p)
        return false;

	if (NULL == m_pFirstLayer)
	{
		m_pFirstLayer = p;
	}
	else
	{
		RenderLayer* pLayer = m_pFirstLayer;
		while (1)
		{
			if (p->GetZorder() >= pLayer->GetZorder())  // Z越大，越排上面，放在越后
			{
				if (NULL == pLayer->GetNextLayer())     //  插在最后面
				{
					pLayer->SetNextLayer(p);
					p->SetPrevLayer(pLayer);
					break;
				}
				pLayer = pLayer->GetNextLayer();
				continue;
			}

			// 插在pLayer前面
			if (pLayer->GetPrevLayer())
			{
				pLayer->GetPrevLayer()->SetNextLayer(p);
				p->SetPrevLayer(pLayer->GetPrevLayer());
			}
			p->SetNextLayer(pLayer);
			pLayer->SetPrevLayer(p);

			if (m_pFirstLayer == pLayer)
				m_pFirstLayer = p;

			break;
		}
	}

	m_nLayerCount++;

	if (NULL == m_hMultiLayerMemDC && m_nLayerCount > 1)  // 当存在多个层时，就得创建一个缓存用于组合各个层
	{
		CRect rc;
		::GetClientRect(m_pWindow->GetHWND(), &rc );
		CreateDoubleBuffer(rc.Width(), rc.Height());
	}
	return true;
}

void RenderChain::CombineAllLayer(HRGN hRgn)
{
	if (hRgn)
	{
		::SelectClipRgn(m_hMultiLayerMemDC, hRgn);
	}

    if (m_hMultiLayerMemDC)
    {
	    CRect rc(0,0, m_pWindow->GetWidth(), m_pWindow->GetHeight());
	    ::FillRect(m_hMultiLayerMemDC, &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
    }

	RenderLayer* pLayer = m_pFirstLayer;
	while (pLayer)
	{
		if (pLayer->IsLayerEnable())
		{
			pLayer->Commit(m_hMultiLayerMemDC);  // 当m_hMemDC为空时，表明只有一个control layer，这时不需要组合各个layer
		}
		pLayer = pLayer->GetNextLayer();
	}

	if (hRgn)
	{
		::SelectClipRgn(m_hMultiLayerMemDC, NULL);
	}
}
void RenderChain::OnWindowPaint(HDC hDC)
{
    if (m_bFirstTimePaintWindow)
    {
        m_bFirstTimePaintWindow = false;
        SetCanCommit(true);
    }

    if (m_hMultiLayerMemDC || m_pWindow->IsTransparent())  // 刷新缓存，避免alpha叠加
    {
	    CRect rc(0,0, m_pWindow->GetWidth(), m_pWindow->GetHeight());
	    ::FillRect(GetMemoryDC(), &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
    }

	RenderLayer* pLayer = m_pFirstLayer;
	while (pLayer)
	{
		if (pLayer->IsLayerEnable())
		{
			pLayer->Draw();  // 当m_hMemDC为空时，表明只有一个control layer，这时不需要组合各个layer
			pLayer->Commit(m_hMultiLayerMemDC);
		}
		pLayer = pLayer->GetNextLayer();
	}

    if (CanCommit())
        m_pWindow->CommitDoubleBuffet2Window(hDC, NULL);
}

void RenderChain::OnWindowResize(int nType, int nWidth, int nHeight)
{
	if (m_hMultiLayerMemDC)   // 首先由OnPaint负责创建以缓冲，避免在分层窗口刚创建的OnSize中去RedrawObject，此时还没有响应OnInitWindow
	{
		this->CreateDoubleBuffer(nWidth, nHeight);
	}

    // window layer其实就是窗口，布局时占满整个窗口。其它layer则可以自定义位置和大小
    // 其它layer由于不是window的子对象，因些需要在这里手动布局（只支持canvas布局）

	RenderLayer* pLayer = m_pFirstLayer;
	while (pLayer)
	{
        if (pLayer == m_pWindowLayer)
        {
            UISendMessage(pLayer, WM_SIZE, (WPARAM)nType, MAKELPARAM(nWidth, nHeight));
            pLayer->UpdateLayout(false);
        }
        else
        {
            CanvasLayout::ArrangeObject(pLayer, nWidth, nHeight);
        }

		pLayer = pLayer->GetNextLayer();
	}
}

void RenderChain::UpdateObject(Object* pObj, bool bOnlyRedrawBkgnd, bool bUpdateNow)
{
    if (NULL == pObj)
        return;

//    m_MultiLayerBuffer.Save(L"C:\\bbb.png", Gdiplus::ImageFormatPNG);

    // 这里之所以去掉对window类型的判断是因为存在一种场景：
    // 我调用pWindow->UpdateObject(false)想刷新立即All Layer，但不提交到window上去，使用Invalidate就做不到了
//     int nObjType = pObj->GetObjectType();
//     if (OBJ_HWNDHOST == nObjType)
//     {
//         // 系统控件的刷新由它自己本身负责
//         return;
//     }
//     else if (OBJ_WINDOW == nObjType)
//     {
//         HWND hWnd = pObj->GetHWND();
//         if (NULL == hWnd)
//             return;
// 
//         ::InvalidateRect(hWnd, NULL, TRUE);
//         if (bUpdateNow)
//             UpdateWindow(hWnd);
// 
//         return;
//     }

    RenderLayer*    pLayer = NULL;
    IRenderTarget*  pRenderTarget = NULL;

    // 获取对象在层上的可见区域
     CRect rcObjVisible;
     if (false == pObj->GetObjectVisibleRect(&rcObjVisible, false))   
     { 
         return;  // 该对象在窗口上不可见，不绘制
     }

    pLayer = pObj->GetRenderLayer();
    if (NULL == pLayer)
        return;
    
    if (!pLayer->IsLayerEnable() || !pLayer->CanRedraw())
        return;

    pRenderTarget = pLayer->GetRenderTarget();
    if (NULL == pRenderTarget)
        return;

    // 不需要维持剪裁区域，由rendertarget begindraw一次性搞定即可
    RenderContext roc(&rcObjVisible, false);  

    // 防止分层窗口的alpha重叠变黑
    bool bNeedClear = m_pWindow->IsTransparent() && pObj->IsTransparent();    

    // 将剪裁rcObjVisible区域。 如果已经正在绘制，则不提交数据
    bool bRet = pRenderTarget->BeginDraw(&rcObjVisible, 1, bNeedClear);
    bool bSelfTransparent = pObj->IsTransparent();
    if (bOnlyRedrawBkgnd)
    {
        // 如果只刷背景的话，那么必须将自己标识为透明的才会绘制parent
        bSelfTransparent = true;  
    }

    pObj->DrawObjectTransparentBkgnd(pRenderTarget, roc, bSelfTransparent);

    if (!bOnlyRedrawBkgnd)
    {
        // 内部绘制时仍然需要裁剪
        roc.m_bUpdateClip = true;           
        pObj->DrawObject(pRenderTarget, roc);
    }

    if (bRet)
    {
        pRenderTarget->EndDraw();

        // 转成窗口上的可见区域
        rcObjVisible.OffsetRect(pLayer->GetParentRectL(), pLayer->GetParentRectT());

        HRGN hRgn = ::CreateRectRgnIndirect(&rcObjVisible);
        CombineAllLayer(hRgn);
        SAFE_DELETE_GDIOBJECT(hRgn);

        // 显示
        if (bUpdateNow && CanCommit())
            m_pWindow->CommitDoubleBuffet2Window(NULL, &rcObjVisible);
    }
}



//
//	用于如LISTBOX ReDrawItem，只刷新一部分，而不是整个object
//
//  参数pbIsDrawing用来告诉调用者，有其它对象正在刷新，调用者不用调用EndRedrawObjectPart
//
IRenderTarget* RenderChain::BeginRedrawObjectPart(Object* pRedrawObj, RECT* prcArray, int nCount, bool* pbIsDrawing, RenderContext* pOutRenderContext)
{
    if (NULL == pRedrawObj)
        return NULL;

    RenderLayer* pRenderLayer = pRedrawObj->GetRenderLayer();
    if (NULL == pRenderLayer)
        return NULL;

    IRenderTarget*  pRenderTarget  =pRenderLayer->GetRenderTarget();
    if (NULL == pRenderTarget)
        return NULL;

    CRect rcClip;
    if (0 == nCount)  // 例如 ListCtrlBase::RedrawItemByInnerCtrl时不用再裁剪，已经处于一个绘制过程中了
    {
        ::GetClientRect(m_pWindow->m_hWnd, &rcClip);
    }
    else
    {
        for (int i = 0; i < nCount; i++)
            rcClip.UnionRect(&rcClip, &prcArray[i]);
    }
    RenderContext roc(&rcClip, false); // BeginDraw已经更新了剪裁区域， 这里的rcClip只是用于绘制过程中判断一个对象是否在这个区域内
    
    bool bNeedClear = m_pWindow->IsTransparent() && pRedrawObj->IsTransparent();
    bool bRet = pRenderTarget->BeginDraw(prcArray, nCount, bNeedClear);  // PS:20130110 去掉了返回值的判断.因为列表控件内部控件在刷新时，会在ROOTPANEL中重新redraw listitem，导致这里返回FALSE，背景绘制失败。因此先放开这个限制

    if (pbIsDrawing)  // 即使已经在绘制其它过程了，这里也强制绘制
        *pbIsDrawing = !bRet;

    pRedrawObj->DrawObjectTransparentBkgnd(pRenderTarget, roc, /*true*/pRedrawObj->IsTransparent());

    UISendMessage(pRedrawObj, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&roc);

    roc.DrawClient(pRedrawObj->GetIObject());
    roc.Scroll(pRedrawObj->GetIObject());
    roc.Update(pRenderTarget);

	if (pOutRenderContext)
		*pOutRenderContext = roc;

    return pRenderTarget;
}

//
//	要提交到窗口上的区域，配合BeginDrawObject使用
//  当需要提交多个Rect时，先将bFinish设置为false，最后一次设置为true释放资源
//
void  RenderChain::EndRedrawObjectPart(IRenderTarget* pRenderTarget, RECT* prcArray, int nCount)
{
    if (pRenderTarget) 
        pRenderTarget->EndDraw();

    this->CombineAllLayer(NULL);

    if (CanCommit())
        m_pWindow->CommitDoubleBuffet2Window(NULL, prcArray, nCount);
}

//
//	创建双缓冲数据
//
void  RenderChain::CreateDoubleBuffer(int nWidth, int nHeight)
{
	if (nWidth <= 0 || nHeight <=0)
	{
		UI_LOG_ERROR(_T("%s nWidth:%d, nHeight:%d"), FUNC_NAME, nWidth, nHeight);
		return;
	}

    if (m_hMultiLayerMemDC)
    {
	    ::SelectObject(m_hMultiLayerMemDC, m_hOldBitmap);
        m_MultiLayerBuffer.Destroy();
    }
    else
    {
    	m_hMultiLayerMemDC = ::CreateCompatibleDC(NULL);
	    ::SetBkMode(m_hMultiLayerMemDC, TRANSPARENT);
	    ::SetStretchBltMode(m_hMultiLayerMemDC, HALFTONE);
    }

	m_MultiLayerBuffer.Create(nWidth, nHeight, 32, Image::createAlphaChannel);
	m_hOldBitmap = (HBITMAP)::SelectObject(m_hMultiLayerMemDC, (HBITMAP)m_MultiLayerBuffer);
}
//
//	释放双缓冲数据
//
void  RenderChain::DestroyDoubleBuffer()
{
	if (m_hMultiLayerMemDC)
	{
		::SelectObject(m_hMultiLayerMemDC, m_hOldBitmap);
		::DeleteDC(m_hMultiLayerMemDC);
		m_hMultiLayerMemDC = NULL;
	}
    m_MultiLayerBuffer.Destroy();
}


void  RenderChain::SetCanCommit(bool b)
{
    if (b)
        m_lRefCanCommit --;
    else
        m_lRefCanCommit ++;
}