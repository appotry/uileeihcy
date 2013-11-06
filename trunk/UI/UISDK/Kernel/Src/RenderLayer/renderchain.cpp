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

// layout.xml RenderChain��ǩ����
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
        pRenderLayer->SetWindowPtr(m_pWindow);  // ��ǰ����SetWindowPtr����֤�Ӷ����ܹ���������GetWindowObject
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

        pRenderLayer->SetWindowPtr(m_pWindow);   // ��ǰ����SetWindowPtr����֤�Ӷ����ܹ���������GetWindowObject
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
        if (pLayer != m_pWindowLayer)  // m_pWindowLayer��ʵ����һ��Window�������ⲿ�������٣������ﲻ�ܹ�
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
			if (p->GetZorder() >= pLayer->GetZorder())  // ZԽ��Խ�����棬����Խ��
			{
				if (NULL == pLayer->GetNextLayer())     //  ���������
				{
					pLayer->SetNextLayer(p);
					p->SetPrevLayer(pLayer);
					break;
				}
				pLayer = pLayer->GetNextLayer();
				continue;
			}

			// ����pLayerǰ��
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

	if (NULL == m_hMultiLayerMemDC && m_nLayerCount > 1)  // �����ڶ����ʱ���͵ô���һ������������ϸ�����
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
			pLayer->Commit(m_hMultiLayerMemDC);  // ��m_hMemDCΪ��ʱ������ֻ��һ��control layer����ʱ����Ҫ��ϸ���layer
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

    if (m_hMultiLayerMemDC || m_pWindow->IsTransparent())  // ˢ�»��棬����alpha����
    {
	    CRect rc(0,0, m_pWindow->GetWidth(), m_pWindow->GetHeight());
	    ::FillRect(GetMemoryDC(), &rc, (HBRUSH)::GetStockObject(BLACK_BRUSH));
    }

	RenderLayer* pLayer = m_pFirstLayer;
	while (pLayer)
	{
		if (pLayer->IsLayerEnable())
		{
			pLayer->Draw();  // ��m_hMemDCΪ��ʱ������ֻ��һ��control layer����ʱ����Ҫ��ϸ���layer
			pLayer->Commit(m_hMultiLayerMemDC);
		}
		pLayer = pLayer->GetNextLayer();
	}

    if (CanCommit())
        m_pWindow->CommitDoubleBuffet2Window(hDC, NULL);
}

void RenderChain::OnWindowResize(int nType, int nWidth, int nHeight)
{
	if (m_hMultiLayerMemDC)   // ������OnPaint���𴴽��Ի��壬�����ڷֲ㴰�ڸմ�����OnSize��ȥRedrawObject����ʱ��û����ӦOnInitWindow
	{
		this->CreateDoubleBuffer(nWidth, nHeight);
	}

    // window layer��ʵ���Ǵ��ڣ�����ʱռ���������ڡ�����layer������Զ���λ�úʹ�С
    // ����layer���ڲ���window���Ӷ�����Щ��Ҫ�������ֶ����֣�ֻ֧��canvas���֣�

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

    // �������������������ص���Ŀǰ���ػ�����޷���֤��ʾz�����ֱ��ˢ�¸������Ա�֤z index
    if (pObj->TestStyle(OBJECT_STYLE_ZINDEX_OVERLAP) && pObj->GetParentObject())
    {
        UpdateObject(pObj->GetParentObject(), bOnlyRedrawBkgnd, bUpdateNow);
        return;
    }
//    m_MultiLayerBuffer.Save(L"C:\\bbb.png", Gdiplus::ImageFormatPNG);

    // ����֮����ȥ����window���͵��ж�����Ϊ����һ�ֳ�����
    // �ҵ���pWindow->UpdateObject(false)��ˢ������All Layer�������ύ��window��ȥ��ʹ��Invalidate����������
//     int nObjType = pObj->GetObjectType();
//     if (OBJ_HWNDHOST == nObjType)
//     {
//         // ϵͳ�ؼ���ˢ�������Լ�������
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

    // ��ȡ�����ڲ��ϵĿɼ�����
     CRect rcObjVisible;
     if (false == pObj->GetObjectVisibleRect(&rcObjVisible, false))   
     { 
         return;  // �ö����ڴ����ϲ��ɼ���������
     }

    pLayer = pObj->GetRenderLayer();
    if (NULL == pLayer)
        return;
    
    if (!pLayer->IsLayerEnable() || !pLayer->CanRedraw())
        return;

    pRenderTarget = pLayer->GetRenderTarget();
    if (NULL == pRenderTarget)
        return;

    // ����Ҫά�ּ���������rendertarget begindrawһ���Ը㶨����
    RenderContext roc(&rcObjVisible, false);  

    // ��ֹ�ֲ㴰�ڵ�alpha�ص����
    bool bNeedClear = m_pWindow->IsTransparent() && pObj->IsTransparent();    

    // ������rcObjVisible���� ����Ѿ����ڻ��ƣ����ύ����
    bool bRet = pRenderTarget->BeginDraw(&rcObjVisible, 1, bNeedClear);
    bool bSelfTransparent = pObj->IsTransparent();
    if (bOnlyRedrawBkgnd)
    {
        // ���ֻˢ�����Ļ�����ô���뽫�Լ���ʶΪ͸���ĲŻ����parent
        bSelfTransparent = true;  
    }

    pObj->DrawObjectTransparentBkgnd(pRenderTarget, roc, bSelfTransparent);

    if (!bOnlyRedrawBkgnd)
    {
        // �ڲ�����ʱ��Ȼ��Ҫ�ü�
        roc.m_bUpdateClip = true;           
        pObj->DrawObject(pRenderTarget, roc);
    }

    if (bRet)
    {
        pRenderTarget->EndDraw();

        // ת�ɴ����ϵĿɼ�����
        rcObjVisible.OffsetRect(pLayer->GetParentRectL(), pLayer->GetParentRectT());

        HRGN hRgn = ::CreateRectRgnIndirect(&rcObjVisible);
        CombineAllLayer(hRgn);
        SAFE_DELETE_GDIOBJECT(hRgn);

        // ��ʾ
        if (bUpdateNow && CanCommit())
            m_pWindow->CommitDoubleBuffet2Window(NULL, &rcObjVisible);
    }
}



//
//	������LISTBOX ReDrawItem��ֻˢ��һ���֣�����������object
//
//  ����pbIsDrawing�������ߵ����ߣ���������������ˢ�£������߲��õ���EndRedrawObjectPart
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
    if (0 == nCount)  // ���� ListCtrlBase::RedrawItemByInnerCtrlʱ�����ٲü����Ѿ�����һ�����ƹ�������
    {
        ::GetClientRect(m_pWindow->m_hWnd, &rcClip);
    }
    else
    {
        for (int i = 0; i < nCount; i++)
            rcClip.UnionRect(&rcClip, &prcArray[i]);
    }
    RenderContext roc(&rcClip, false); // BeginDraw�Ѿ������˼������� �����rcClipֻ�����ڻ��ƹ������ж�һ�������Ƿ������������
    
    bool bNeedClear = m_pWindow->IsTransparent() && pRedrawObj->IsTransparent();
    bool bRet = pRenderTarget->BeginDraw(prcArray, nCount, bNeedClear);  // PS:20130110 ȥ���˷���ֵ���ж�.��Ϊ�б�ؼ��ڲ��ؼ���ˢ��ʱ������ROOTPANEL������redraw listitem���������ﷵ��FALSE����������ʧ�ܡ�����ȷſ��������

    if (pbIsDrawing)  // ��ʹ�Ѿ��ڻ������������ˣ�����Ҳǿ�ƻ���
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
//	Ҫ�ύ�������ϵ��������BeginDrawObjectʹ��
//  ����Ҫ�ύ���Rectʱ���Ƚ�bFinish����Ϊfalse�����һ������Ϊtrue�ͷ���Դ
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
//	����˫��������
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
//	�ͷ�˫��������
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