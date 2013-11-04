#include "stdafx.h"
#include "customwindow.h"
#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Inc\Interface\imapattr.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"
#include "UISDK\Kernel\Src\Util\dwm\dwmhelper.h"

#define WINDOWS_MINIMIZED_POINT  -32000  // ������С����window���ݸ����ǵ�λ��

CustomWindow::CustomWindow()
{
    m_pICustomWindow = NULL;
	m_bNeedToSetWindowRgn = true;   // �״���ʾʱ����Ҫ����һ�´�����״
	m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_FULL;     
	m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_NULL;
	m_pColMask = NULL;
	m_nAlphaMask = 1;                    // ��alphaֵΪ0ʱ�����ٳ�
	m_TransparentRgn9Region.Set(0);

	m_hRgn_topleft = NULL;
	m_hRgn_topright = NULL;
	m_hRgn_bottomleft = NULL;
	m_hRgn_bottomright = NULL;

	this->m_nResizeBit = WRSB_CAPTION;
	this->m_pLayeredWindowWrap = NULL;		
}
CustomWindow::~CustomWindow()
{
	SAFE_DELETE_GDIOBJECT(m_hRgn_topleft);
	SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
	SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
	SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);
	SAFE_RELEASE(m_pColMask);
	SAFE_DELETE(m_pLayeredWindowWrap);
}

BOOL  CustomWindow::PreCreateWindow(CREATESTRUCT* pcs)
{
    __super::nvProcessMessage(GetCurMsg(), 0, 0);
    if (FALSE == GetCurMsg()->lRet)
        return FALSE;

	pcs->style = DS_SETFONT | WS_POPUP | WS_SYSMENU /*| WS_THICKFRAME*/;
	if (m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->PreCreateWindow(pcs);
	}

	return TRUE;
}

void  CustomWindow::OnObjectLoaded()
{
    // ������SetAttribute�н��Լ�����Ϊ�ֲ㴰��ʱ���Ӷ��󻹶�û�д�����������Լ�������ɺ�
    // ֪ͨ�����е��Ӵ���
    if (m_pLayeredWindowWrap)
    {
        UIMSG  msg;
        msg.message = UI_WM_WINDOWLAYEREDCHANGED;
        msg.wParam = 1;
        ForwardMessageToChildObject(this, &msg);
    }
    SetMsgHandled(FALSE);
}
void CustomWindow::OnInnerInitWindow( )
{
	Window::OnInnerInitWindow();

	LONG dwStyleEx = GetWindowLong(m_hWnd, GWL_EXSTYLE );
	dwStyleEx &= ~ WS_EX_WINDOWEDGE;
	LONG n = SetWindowLong( m_hWnd, GWL_EXSTYLE, dwStyleEx );

	if (m_pLayeredWindowWrap)
		m_pLayeredWindowWrap->InitLayeredWindow();
}

GRAPHICS_RENDER_LIBRARY_TYPE  CustomWindow::OnGetGraphicsRenderType()
{
    if (m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_AREO ||
        m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_LAYERED ||
        this->IsTransparent())  //  ��������Ⱦ�Ŀؼ��㣬��Ҫ��alphablend��������
        return GRAPHICS_RENDER_LIBRARY_TYPE_GDIPLUS;

    return GRAPHICS_RENDER_LIBRARY_TYPE_GDI; 
}

LRESULT CustomWindow::_OnNcDestroy( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	if (m_pLayeredWindowWrap)
		m_pLayeredWindowWrap->ReleaseLayeredWindow();
	
	return 0;
}


//
// ����WM_NCACTIVATE��Ϣ��������win7�ϱ������ʧ����б߿�(������Ϊ���ڲ�ʹ��WS_THICKFRAME��ʵ����ק��Ҳû�����
// �����ˣ�������Ӧ����Ϣ��Ϊ��ʵ�ִ���ˢ�£�
// 20121127 ��ע:ǿ�Ʒ���1�����´�һ��ģ̬customwindow���ٵ���һ��ģ̬����ʱ����CFileDialog�����µ����Ĵ��ڽ���Զ���ܱ����
//               ò������ģ̬���ڻ��򸸴��ڷ��������Ϣ���м��
//
// Remark
//	Return Nonzero if Windows should proceed with default processing; 0 to prevent the caption bar or icon from being deactivated.
//
//	���ﲻ�ܷ���0������ᵼ���������ڳ��ֺܶ����⣨--������
//	
LRESULT CustomWindow::_OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	this->SetActive(wParam?true:false);

	bHandled = FALSE;
	return 0;

// 	this->UpdateObject();
// 	return 1;
}


//
// ���ڷֲ㴰�ڣ�����ʾ��ʱ����Ҫ���¸���һ�Ρ���Ϊ���ֲ㴰�����ص�ʱ���ڴ�λͼû�еõ����£�invisible false��ֱ��return��
//
// ע��������û��ʹ����ӦWM_SHOWWINDOW��ͨ���ֲ㴰��ˢ�£���Ϊ����ӦWM_SHOWWINDOW��ʱ��IsWindowVisible����FALSE
//     ��˸���OnWindowPosChanged���õ�������ʾ��ʱ����ͨ���ֲ㴰��ˢ��
//   
LRESULT CustomWindow::_OnWindowPosChanging( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;

	if (m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnWindowPosChanging(lpWndPos);
	}
	return 0;
}
//
//	֪ͨ�ֲ㴰���µ�λ�úʹ�С
//
LRESULT CustomWindow::_OnWindowPosChanged( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;
	LPWINDOWPOS lpWndPos = (LPWINDOWPOS)lParam;

	if (m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnWindowPosChanged(lpWndPos);
	}
	return 0;
}
LRESULT CustomWindow::_OnCancelMode( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = FALSE;

	if (m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnCancelMode();
	}
	return 0;
}

LRESULT  CustomWindow::_OnDwmCompositionChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if (m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_AREO)
    {
        this->SetWindowAreo(true);  // �ɸú����ڲ�ȥ�ж�areo�Ƿ����
        m_bNeedToSetWindowRgn = true;
        this->UpdateObject();
    }
    return 0;
}

void CustomWindow::ResetAttribute()
{
	__super::ResetAttribute();

	m_bNeedToSetWindowRgn = true;  // ����ʱ�����¸��´�����״
	m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_NULL;
	m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_FULL;
	m_TransparentRgn9Region.Set(0);

	SAFE_RELEASE(m_pColMask);
	m_nAlphaMask = 255;

	SAFE_DELETE_GDIOBJECT(m_hRgn_topleft);
	SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
	SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
	SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);
}
void CustomWindow::SetAttribute(IMapAttribute* pMapAttrib, bool bReload )
{
    const TCHAR* szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_TYPE, true);
	if (szText)
	{
        if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_AREO, szText))
        {
            this->SetWindowAreo(true);
        }
		else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_LAYERED, szText))
		{
			m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_LAYERED;
			this->SetWindowLayered(true);
		}
		else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR, szText))
		{	
			m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_MASKCOLOR;
		}	
		else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA, szText))
		{
			m_nWindowTransparentMaskType = WINDOW_TRANSPARENT_TYPE_MASKALPHA;
		}

		// ͸��ɫ
        szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR_VALUE, true);
		if (szText)
		{
			IColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
			if (pColorRes)
				pColorRes->GetColor((BSTR)szText, &m_pColMask);

			m_nWindowTransparentMaskType |= WINDOW_TRANSPARENT_TYPE_MASKCOLOR;
		}

		// ͸����
        szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA_VALUE, true);
		if (szText)
		{
			m_nAlphaMask = _ttoi(szText);
			m_nWindowTransparentMaskType |= WINDOW_TRANSPARENT_TYPE_MASKALPHA;
		}
	}

    szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_PART, true);
    if (szText)
    {
        if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_PART_4_CORNER, szText))
        {
            m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_4_CORNER;
        }
        else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_PART_8_BORDER, szText))
        {
            m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_8_BORDER;
        }
        else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_PART_FULL, szText))
        {
            m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_FULL;
        }
        else if (0 == _tcscmp(XML_WINDOW_TRANSPARENT_PART_CUSTOM_CORNER, szText))
        {
            m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_CUSTOM_CORNER;
        }
        else
        {
            m_eTransparentRgnType = WINDOW_TRANSPARENT_PART_NULL;
        }
    }

    // ��ȡ���ò���
    if (m_eTransparentRgnType == WINDOW_TRANSPARENT_PART_4_CORNER ||
        m_eTransparentRgnType == WINDOW_TRANSPARENT_PART_8_BORDER ||
        m_eTransparentRgnType == WINDOW_TRANSPARENT_PART_CUSTOM_CORNER ||
        m_nWindowTransparentMaskType == WINDOW_TRANSPARENT_TYPE_AREO)
    {
        szText = pMapAttrib->GetAttr(XML_WINDOW_TRANSPARENT_PART_9REGION, true);
        if (szText)
        {
            Util::TranslateImage9Region(szText, &m_TransparentRgn9Region);
        }
        else
        {
            m_TransparentRgn9Region.Set(0);
        }
    }

	// ע��������Ҫ�����÷ֲ㴰�ڵ����Է���ǰ�档��Ϊobject�кܶ౳��ͼ�����ڴ�������
	Window::SetAttribute(pMapAttrib, bReload);
}


void  CustomWindow::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData)
{
    __super::OnEditorGetAttrList(pData);

    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

    IUIEditorGroupAttribute*  pWindowGroup = pEditor->CreateGroupAttribute(pData->pGroupAttr, CustomWindow::GetXmlName(), NULL);
    pEditor->CreateComboBoxAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_TYPE, szPrefix)
        ->AddOption(L"", L"")
        ->AddOption(XML_WINDOW_TRANSPARENT_TYPE_LAYERED)
        ->AddOption(XML_WINDOW_TRANSPARENT_TYPE_AREO)
        ->AddOption(XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA)
        ->AddOption(XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR);
    pEditor->CreateTextAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_TYPE_MASKCOLOR_VALUE, szPrefix);
    pEditor->CreateTextAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_TYPE_MASKALPHA_VALUE, szPrefix);
    pEditor->CreateTextAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_PART, szPrefix);
    pEditor->CreateTextAttribute(pWindowGroup, XML_WINDOW_TRANSPARENT_PART_9REGION, szPrefix);
}


// [Virtual]
// 1. ʵ��EraseBkgnd֮��UpdateWindowRgn
// 2. ��������һЩ���������ʱҲ������Ҫ���ƣ���ֲ㴰��...)
bool CustomWindow::DrawObject(IRenderTarget* pRenderTarget, RenderContext roc)
{
    //if (this->IsVisible())  // ���ڷֲ㴰�����´���ʾʱ�����ȸ�������ʾ����������ʱ����ˢ�¶�û������˽����������ȥ��
    {
        ::UISendMessage(this->GetIMessage(), WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&roc );  // ��lparam��Ϊ1����ԭʼ��Ϣ��������

        // �������ô���͸����״ 
        // ע��
        //    1. �����öδ������OnErasebkgnd�е�ԭ���ǣ�ˢ��һ����ťʱҲ���ߵ�
        // OnEraseBkgnd�У�������ʱ�ı���ͼƬ�б�����ֻʣ��һ���ؼ���update window rgn����
        //    2. ͬ��Ҳ���ܷ���CommitDoubleBuffet2Window�У���ΪҲ�п����ǳ�ʼ��ˢ����һ������
        //    3. �������_OnPaint���еĻ�����Ὣ�����ϵ��������ڲ���GDI����alpha���0��Ҳ�ᱻ�ٵ�

        if (m_bNeedToSetWindowRgn)
        {
            this->UpdateWindowRgn();
        }

        this->_drawNcChildObject(pRenderTarget, roc);

        roc.DrawClient(m_pIObject);
        roc.Scroll(m_pIObject);
        roc.Update(pRenderTarget);

        ::UISendMessage(this, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)&roc);       // ��lparam��Ϊ1����ԭʼ��Ϣ��������
        this->DrawChildObject(pRenderTarget, roc);
    }

    return true;
}


// ����WM_PAINT��Ϣ������Ҫ����Nc (����������CustomExWindow��ʱ������thickframe )
LRESULT CustomWindow::_OnNcPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled )
{
	bHandled = TRUE;
	return 0;
}

void CustomWindow::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	if (m_pBkgndRender || m_pForegndRender || m_pMaterialRender)
	{
		CRect rc(0,0, GetWidth(), GetHeight());

		int nState = this->IsActive()?WINDOW_BKGND_RENDER_STATE_ACTIVE:WINDOW_BKGND_RENDER_STATE_INACTIVE;

        if (m_pBkgndRender)
        {
            CRect rcBkgnd(&rc);
            rcBkgnd.DeflateRect(&m_rcBkgndRenderRegion);
            m_pBkgndRender->DrawState(pRenderTarget, &rcBkgnd, nState);
        }

        if (m_pMaterialRender)
        {
            CRect rcTextureRegion(&rc);
            rcTextureRegion.DeflateRect(&m_rcMaterialRenderRegion);
            m_pMaterialRender->DrawState(pRenderTarget, &rcTextureRegion, nState);
        }

        if (m_pForegndRender)
        {
            CRect rcForegnd(&rc);
            rcForegnd.DeflateRect(&m_rcForegndRenderRegion);
            m_pForegndRender->DrawState(pRenderTarget, &rcForegnd, nState);
        }
	}
	else
	{
		__super::OnEraseBkgnd(pRenderTarget);
	}
}

void CustomWindow::UpdateWindowRgn()
{
	this->m_bNeedToSetWindowRgn = false;

#if 0
	BYTE* pBits = LockBits(pRenderTarget);
	if (NULL == pBits)
		return;

	this->UpdateWindowRgn(pBits);

	UnlockBits(pRenderTarget);
#else

	HDC hMemDC = m_pRenderChain->GetMemoryDC();
	if (NULL == hMemDC)
		return;

	DIBSECTION  dibsection;
	HBITMAP hBitmap = (HBITMAP)::GetCurrentObject(hMemDC, OBJ_BITMAP);
	if (NULL == hBitmap)
		return;

	::GetObject(hBitmap, sizeof(DIBSECTION), &dibsection);
	this->UpdateWindowRgn((BYTE*)dibsection.dsBm.bmBits);

#endif
}

void CustomWindow::UpdateWindowRgn(BYTE* pBits)
{
	this->m_bNeedToSetWindowRgn = false;
	if (NULL == pBits)
	{
		return;
	}
	if (WINDOW_TRANSPARENT_TYPE_NULL == m_nWindowTransparentMaskType)
	{
		return;
	}
	if (WINDOW_TRANSPARENT_TYPE_LAYERED & m_nWindowTransparentMaskType ||
		WINDOW_TRANSPARENT_TYPE_NULL == m_nWindowTransparentMaskType)
	{
		return;   // �ֲ㴰�ڲ���Ҫ�ֶ����ô�����״
	}
    if (WINDOW_TRANSPARENT_TYPE_AREO & m_nWindowTransparentMaskType)
    {
        DwmHelper*  pDwm = DwmHelper::GetInstance();
        if (pDwm->IsEnable())
        {
            if (pDwm->pDwmEnableBlurBehindWindow)
            {
                CRect  rcClient;
                ::GetClientRect(m_hWnd, &rcClient);
                rcClient.DeflateRect(m_TransparentRgn9Region.left, m_TransparentRgn9Region.top, m_TransparentRgn9Region.right, m_TransparentRgn9Region.bottom);

                HRGN hRgn = CreateRectRgnIndirect(&rcClient);
                DWM_BLURBEHIND blurbehind = {0};
                blurbehind.dwFlags = DWM_BB_ENABLE|DWM_BB_BLURREGION|DWM_BB_TRANSITIONONMAXIMIZED;
                blurbehind.fEnable = TRUE;
                blurbehind.hRgnBlur = hRgn;
                blurbehind.fTransitionOnMaximized = TRUE;
                pDwm->pDwmEnableBlurBehindWindow(m_hWnd, &blurbehind);
                DeleteObject(hRgn);
            }
        }
        return;
    }

	switch (m_eTransparentRgnType)
	{
	case WINDOW_TRANSPARENT_PART_NULL:
		return;

	case WINDOW_TRANSPARENT_PART_FULL:
		{
			RECT rc = { 0, 0, this->GetWidth(), this->GetHeight() };
			HRGN hRgn = this->GetExcludeRgn(pBits, rc, false);
			if (hRgn)
			{
				HRGN hRgnFull = CreateRectRgnIndirect(&rc);
				::CombineRgn(hRgnFull,hRgnFull,hRgn, RGN_DIFF);
				::SetWindowRgn(m_hWnd,hRgnFull,::IsWindowVisible(m_hWnd));
				::DeleteObject(hRgn);
				::DeleteObject(hRgnFull);
			}
		}
		return;

	case WINDOW_TRANSPARENT_PART_4_CORNER:
		{
			int nWindowW = this->GetWidth();
			int nWindowH = this->GetHeight();

			if (NULL == m_hRgn_topleft)  // ��һ�γ�ʼ���ĸ��������״����
			{
				SAFE_DELETE_GDIOBJECT(m_hRgn_topleft);
				SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
				SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
				SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);

				RECT topleft = {0,0,m_TransparentRgn9Region.topleft, m_TransparentRgn9Region.top};
				m_hRgn_topleft = GetExcludeRgn(pBits, topleft, true);

				RECT topright = { nWindowW-m_TransparentRgn9Region.topright, 0, nWindowW, m_TransparentRgn9Region.top };
				m_hRgn_topright = GetExcludeRgn(pBits, topright, true);

				RECT bottomleft = { 0, nWindowH-m_TransparentRgn9Region.bottom, m_TransparentRgn9Region.bottomleft, nWindowH };
				m_hRgn_bottomleft = GetExcludeRgn(pBits, bottomleft, true);

				RECT bottomright = { nWindowW-m_TransparentRgn9Region.bottomright, nWindowH-m_TransparentRgn9Region.bottom, nWindowW, nWindowH };
				m_hRgn_bottomright = GetExcludeRgn(pBits, bottomright, true);
			}
 
			HRGN hRgnTopRight = ::CreateRectRgn(0,0,0,0);
			HRGN hRgnBottomLeft = ::CreateRectRgn(0,0,0,0);
			HRGN hRgnBottomRight = ::CreateRectRgn(0,0,0,0);

			// m_hRgn_xxx���ǻ���(0,0)����ģ���Ҫ���ݵ�ǰ���ڴ�С����offset����, m_hRgn_topleft����Ҫoffset
			::CombineRgn(hRgnTopRight,m_hRgn_topright, hRgnTopRight, RGN_COPY);
			::OffsetRgn(hRgnTopRight,nWindowW-m_TransparentRgn9Region.topright, 0);

			::CombineRgn(hRgnBottomLeft,m_hRgn_bottomleft, hRgnBottomLeft, RGN_COPY);
			::OffsetRgn(hRgnBottomLeft, 0, nWindowH-m_TransparentRgn9Region.bottom);

			::CombineRgn(hRgnBottomRight,m_hRgn_bottomright, hRgnBottomRight, RGN_COPY);
			::OffsetRgn(hRgnBottomRight, nWindowW-m_TransparentRgn9Region.bottomright, nWindowH-m_TransparentRgn9Region.bottom);

			HRGN hRgn = ::CreateRectRgn(0,0,nWindowW,nWindowH);
			::CombineRgn(hRgn, hRgn, m_hRgn_topleft,  RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnTopRight,    RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnBottomLeft,  RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnBottomRight, RGN_DIFF );

		//	this->SetCanRedraw(false);
			// �������Ĳ�����ȥˢ�µĻ����ͻ�����Ļ�ϲ�������Щ���õ���region����
			// MSDN: Typically, you set bRedraw to TRUE if the window is visible. 
			::SetWindowRgn(m_hWnd,hRgn, ::IsWindowVisible(m_hWnd));  
			::DeleteObject(hRgn);
		//	this->SetCanRedraw(true);

			::DeleteObject(hRgnTopRight);
			::DeleteObject(hRgnBottomLeft);
			::DeleteObject(hRgnBottomRight);
		}
		return;

	case WINDOW_TRANSPARENT_PART_8_BORDER:
		{
			// TODO: ���б�Ҫ���н�һ�����Ż�
			int nWindowW = this->GetWidth();
			int nWindowH = this->GetHeight();

			if (NULL == m_hRgn_topleft)  // ��һ�γ�ʼ���ĸ��������״����
			{
				SAFE_DELETE_GDIOBJECT(m_hRgn_topleft);
				SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
				SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
				SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);

				RECT topleft = {0,0,m_TransparentRgn9Region.topleft, m_TransparentRgn9Region.top};
				m_hRgn_topleft = GetExcludeRgn(pBits, topleft, true);

				RECT topright = { nWindowW-m_TransparentRgn9Region.topright, 0, nWindowW, m_TransparentRgn9Region.top };
				m_hRgn_topright = GetExcludeRgn(pBits, topright, true);

				RECT bottomleft = { 0, nWindowH-m_TransparentRgn9Region.bottom, m_TransparentRgn9Region.bottomleft, nWindowH };
				m_hRgn_bottomleft = GetExcludeRgn(pBits, bottomleft, true);

				RECT bottomright = { nWindowW-m_TransparentRgn9Region.bottomright, nWindowH-m_TransparentRgn9Region.bottom, nWindowW, nWindowH };
				m_hRgn_bottomright = GetExcludeRgn(pBits, bottomright, true);
			}

			RECT top = { m_TransparentRgn9Region.topleft, 0, nWindowW-m_TransparentRgn9Region.topright, m_TransparentRgn9Region.top };
			HRGN hRgn_top = GetExcludeRgn(pBits, top, false);
			RECT left = {0, m_TransparentRgn9Region.top, m_TransparentRgn9Region.left, nWindowH-m_TransparentRgn9Region.bottom};
			HRGN hRgn_left = GetExcludeRgn(pBits, left, false);
			RECT right = { nWindowW-m_TransparentRgn9Region.right, m_TransparentRgn9Region.top, nWindowW, nWindowH-m_TransparentRgn9Region.bottom };
			HRGN hRgn_right = GetExcludeRgn(pBits, right, false);
			RECT bottom = { m_TransparentRgn9Region.bottomleft, nWindowH-m_TransparentRgn9Region.bottomleft, nWindowW-m_TransparentRgn9Region.bottomright, nWindowH };
			HRGN hRgn_bottom = GetExcludeRgn(pBits, bottom, false);

			HRGN hRgnTopRight = ::CreateRectRgn(0,0,0,0);
			HRGN hRgnBottomLeft = ::CreateRectRgn(0,0,0,0);
			HRGN hRgnBottomRight = ::CreateRectRgn(0,0,0,0);

			// m_hRgn_xxx���ǻ���(0,0)����ģ���Ҫ���ݵ�ǰ���ڴ�С����offset����, m_hRgn_topleft����Ҫoffset
			::CombineRgn(hRgnTopRight,m_hRgn_topright, hRgnTopRight, RGN_COPY);
			::OffsetRgn(hRgnTopRight,nWindowW-m_TransparentRgn9Region.topright, 0);
			::CombineRgn(hRgnBottomLeft,m_hRgn_bottomleft, hRgnBottomLeft, RGN_COPY);
			::OffsetRgn(hRgnBottomLeft, 0, nWindowH-m_TransparentRgn9Region.bottom);
			::CombineRgn(hRgnBottomRight,m_hRgn_bottomright, hRgnBottomRight, RGN_COPY);
			::OffsetRgn(hRgnBottomRight, nWindowW-m_TransparentRgn9Region.bottomright, 0);

			HRGN hRgn = ::CreateRectRgn(0,0,nWindowW,nWindowH);
			::CombineRgn(hRgn, hRgn, m_hRgn_topleft,  RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnTopRight,    RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnBottomLeft,  RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgnBottomRight, RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgn_top,        RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgn_left,       RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgn_right,      RGN_DIFF );
			::CombineRgn(hRgn, hRgn, hRgn_bottom,     RGN_DIFF );

			::SetWindowRgn(m_hWnd,hRgn, ::IsWindowVisible(m_hWnd));
			::DeleteObject(hRgn);

			::DeleteObject(hRgnTopRight);
			::DeleteObject(hRgnBottomLeft);
			::DeleteObject(hRgnBottomRight);
			::DeleteObject(hRgn_top);
			::DeleteObject(hRgn_left);
			::DeleteObject(hRgn_right);
			::DeleteObject(hRgn_bottom);
			
		}
		return;

	case WINDOW_TRANSPARENT_PART_CUSTOM_CORNER:
		{
			int nWindowW = this->GetWidth();
			int nWindowH = this->GetHeight();

			if (NULL == m_hRgn_topleft && m_TransparentRgn9Region.topleft != 0 && m_TransparentRgn9Region.top != 0)
			{
				m_hRgn_topleft = ::CreateEllipticRgn( 0,0, m_TransparentRgn9Region.topleft, m_TransparentRgn9Region.top );
				HRGN hRgnRect = ::CreateRectRgn( 0,0, m_TransparentRgn9Region.topleft/2, m_TransparentRgn9Region.top/2 );
				::CombineRgn( m_hRgn_topleft, hRgnRect, m_hRgn_topleft, RGN_DIFF );
				::DeleteObject(hRgnRect);
			}
			if (NULL == m_hRgn_topright && m_TransparentRgn9Region.topright != 0 && m_TransparentRgn9Region.left != 0)
			{
				RECT rcE = {0,0, m_TransparentRgn9Region.topright, m_TransparentRgn9Region.left};
				::OffsetRect(&rcE, nWindowW-m_TransparentRgn9Region.topright, 0);
				m_hRgn_topright = ::CreateEllipticRgnIndirect( &rcE );

				RECT rcR = {0,0, m_TransparentRgn9Region.topright/2, m_TransparentRgn9Region.left/2};
				::OffsetRect(&rcR, nWindowW-m_TransparentRgn9Region.topright/2, 0 );
				HRGN hRgnRect = ::CreateRectRgnIndirect( &rcR );

				::CombineRgn( m_hRgn_topright, hRgnRect, m_hRgn_topright, RGN_DIFF );
				::DeleteObject(hRgnRect);
			}
			if (NULL == m_hRgn_bottomleft && m_TransparentRgn9Region.right != 0 && m_TransparentRgn9Region.bottomleft != 0)
			{
				RECT rcE = {0,0, m_TransparentRgn9Region.right, m_TransparentRgn9Region.bottomleft};
				::OffsetRect(&rcE, 0, nWindowH-m_TransparentRgn9Region.bottomleft);
				m_hRgn_bottomleft = ::CreateEllipticRgnIndirect(&rcE);

				RECT rcR = {0,0, m_TransparentRgn9Region.right/2, m_TransparentRgn9Region.bottomleft/2};
				::OffsetRect(&rcR, 0, nWindowH-m_TransparentRgn9Region.bottomleft/2);
				HRGN hRgnRect = ::CreateRectRgnIndirect( &rcR );
				::CombineRgn( m_hRgn_bottomleft, hRgnRect, m_hRgn_bottomleft, RGN_DIFF );
				::DeleteObject(hRgnRect);
			}
			if (NULL == m_hRgn_bottomright && m_TransparentRgn9Region.bottom != 0 && m_TransparentRgn9Region.bottomright != 0 )
			{
				RECT rcE = {0,0, m_TransparentRgn9Region.bottom, m_TransparentRgn9Region.bottomright};
				::OffsetRect(&rcE, nWindowW-m_TransparentRgn9Region.bottom, nWindowH-m_TransparentRgn9Region.bottomright );
				m_hRgn_bottomright = ::CreateEllipticRgnIndirect( &rcE );

				RECT rcR = {0,0, m_TransparentRgn9Region.bottom/2, m_TransparentRgn9Region.bottomright/2};
				::OffsetRect(&rcR,nWindowW-m_TransparentRgn9Region.bottom/2, nWindowH-m_TransparentRgn9Region.bottomright/2 );
				HRGN hRgnRect = ::CreateRectRgnIndirect( &rcR );

				::CombineRgn( m_hRgn_bottomright, hRgnRect, m_hRgn_bottomright, RGN_DIFF );
				::DeleteObject(hRgnRect);
			}

			HRGN hRgn = ::CreateRectRgn(0,0,this->GetWidth(), this->GetHeight());
			if (m_hRgn_topleft)
			{
				::CombineRgn(hRgn, hRgn, m_hRgn_topleft,  RGN_DIFF );
			}
			if (m_hRgn_topright)
			{
				::CombineRgn(hRgn, hRgn, m_hRgn_topright,  RGN_DIFF );
			}
			if (m_hRgn_bottomleft)
			{
				::CombineRgn(hRgn, hRgn, m_hRgn_bottomleft,  RGN_DIFF );
			}
			if (m_hRgn_bottomright)
			{
				::CombineRgn(hRgn, hRgn, m_hRgn_bottomright,  RGN_DIFF );
			}
			::SetWindowRgn(m_hWnd,hRgn, ::IsWindowVisible(m_hWnd));
			SAFE_DELETE_GDIOBJECT(m_hRgn_topright);
			SAFE_DELETE_GDIOBJECT(m_hRgn_bottomleft);
			SAFE_DELETE_GDIOBJECT(m_hRgn_bottomright);
		}
		return;

	}
}


//
//	��һ��pBitsȡ����������RC��������Ҫ���ٵ��ķ�Χ
//
//	bOffsetToOrigin
//		[in]	Ҫ����Ҫ�����ص�HRGN��ԭ������Ϊ(0,0)��ʼ --- ��ȡ�ĸ������HRGNʱ����
//
HRGN CustomWindow::GetExcludeRgn( BYTE* pBits, const RECT& rc, bool bOffsetToOrigin )
{
	if(NULL == pBits)
		return NULL;

	int nWindowW = this->GetWidth();
	int nWindowH = this->GetHeight();

	BYTE* p = pBits;
	vector<RECT>  vRectRgnData;
	for (LONG i = rc.top; i < rc.bottom; i++ )   
	{
		p = pBits + nWindowW*4*i + rc.left*4;
		for (LONG j = rc.left; j < rc.right; j++)      
		{
			POINT pt = { j, nWindowH-i-1 };   // ������m_hMemBitmap�Ƿ����

			BYTE b = *p++;
			BYTE g = *p++;
			BYTE r = *p++;
			BYTE a = *p++;

			bool bExclude = false;
			if (((int)m_nWindowTransparentMaskType)& WINDOW_TRANSPARENT_TYPE_MASKCOLOR) 
			{
				if (NULL != m_pColMask && m_pColMask->m_col == RGB(r,g,b))
				{
					bExclude = true;
				}
			}
			if (((int)m_nWindowTransparentMaskType) & WINDOW_TRANSPARENT_TYPE_MASKALPHA)
			{
				if (a < m_nAlphaMask)
				{
					bExclude = true;
				}
			}
			if (bExclude)
			{
				RECT rcPixel = { pt.x, pt.y, pt.x+1, pt.y+1 };  // ͳһ����Ϊ��0,0λ�ÿ�ʼ�����ں����OffsetRgn
				if (bOffsetToOrigin)
				{
					::OffsetRect(&rcPixel, -rc.left, -rc.top);
				}

				vRectRgnData.push_back(rcPixel);
			}
		}
	}

	int nCount = (int)vRectRgnData.size();
	RGNDATA*   pRgnData      = (RGNDATA*)new BYTE[ sizeof(RGNDATAHEADER) + nCount*sizeof(RECT) ];
	pRgnData->rdh.dwSize     = sizeof(RGNDATAHEADER);
	pRgnData->rdh.iType      = RDH_RECTANGLES;
	pRgnData->rdh.nCount     = nCount;
	pRgnData->rdh.nRgnSize   = nCount*sizeof(RECT);
	for (int k = 0; k < nCount; k++)
	{
		RECT* prc = (RECT*)pRgnData->Buffer;
		prc[k] = vRectRgnData[k];
	}

	HRGN hRgn = ::ExtCreateRegion(NULL, sizeof(RGNDATAHEADER) + nCount*sizeof(RECT), pRgnData);
	SAFE_ARRAY_DELETE(pRgnData);

	return hRgn;
}

// void CustomWindow::EndRedrawObjectPart(IRenderTarget* pRenderTarget, RECT* prcArray, int nCount)
// {
// 	if (m_pLayeredWindowWrap)
// 	{
//         pRenderTarget->EndDraw();
//         m_pLayeredWindowWrap->Commit2LayeredWindow();
// 	}
// 	else
// 	{
//         __super::EndRedrawObjectPart(pRenderTarget, prcArray, nCount);		
// 	}
// }
void CustomWindow::CommitDoubleBuffet2Window(HDC hDCWnd, RECT* prcCommit, int nRectCount)
{
	if (m_pLayeredWindowWrap)
    {
        m_pLayeredWindowWrap->Commit2LayeredWindow();
    }
	else
    {
        __super::CommitDoubleBuffet2Window(hDCWnd, prcCommit, nRectCount);		
    }
}

//
//	��ȡһ��POINT��CustomWindow�����λ��
//
UINT CustomWindow::OnHitTest( POINT* pt )
{
	if (m_nResizeBit == WRSB_NONE)
	{
		return HTCLIENT;
	}
 	else if (m_nResizeBit == WRSB_CAPTION)
	{
		if (NULL == GetHoverObject() && NULL == GetPressObject()  && !IsZoomed(m_hWnd))
			return HTCAPTION;
	}

	if (NULL == pt)
		return HTERROR;

	UINT nHitTest = HTCLIENT;
	if (IsZoomed(m_hWnd))
		return HTCLIENT;

	CONST int BORDER = GetSystemMetrics(SM_CYDLGFRAME)*2;

	RECT rc;
	::GetClientRect( m_hWnd, &rc );
	int nWidth  = rc.right - rc.left;
	int nHeight = rc.bottom - rc.top;

	if (pt->x < BORDER)
	{
		if (pt->y < BORDER && this->TestResizeBit(WRSB_TOPLEFT))
			nHitTest = HTTOPLEFT;
		else if (pt->y > nHeight - BORDER  && this->TestResizeBit(WRSB_BOTTOMLEFT))
			nHitTest = HTBOTTOMLEFT;
		else if (this->TestResizeBit(WRSB_LEFT))
			nHitTest = HTLEFT;
	}
	else if (pt->x > nWidth-BORDER)
	{
		if (pt->y < BORDER && this->TestResizeBit(WRSB_TOPRIGHT))
			nHitTest = HTTOPRIGHT;
		else if (pt->y > nHeight - BORDER && this->TestResizeBit(WRSB_BOTTOMRIGHT))
			nHitTest = HTBOTTOMRIGHT;
		else if (this->TestResizeBit(WRSB_RIGHT))
			nHitTest = HTRIGHT;
	}
	else if (pt->y < BORDER && this->TestResizeBit(WRSB_TOP))
	{
		nHitTest = HTTOP;
	}
	else if (pt->y > nHeight - BORDER && this->TestResizeBit(WRSB_BOTTOM))
	{
		nHitTest = HTBOTTOM;
	}
 	else
 	{
 		if (m_nResizeBit & WRSB_CAPTION && NULL == GetHoverObject() && NULL == GetPressObject() && !IsZoomed(m_hWnd))
 		{
 			nHitTest = HTCAPTION;
 		}
 	}

	return nHitTest;

}

//
//	ע�������Ϣ�����п�������MOUSEMOVE֮���ֶ�POST������һ����Ϣ
//
BOOL CustomWindow::OnSetCursor( HWND hWnd, UINT nHitTest, UINT message )
{
	POINT pt;
	::GetCursorPos(&pt);
	::MapWindowPoints(NULL,m_hWnd,&pt,1);
	nHitTest = this->OnHitTest(&pt);
	switch(nHitTest)
	{
	case HTTOPLEFT:
	case HTBOTTOMRIGHT:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZENWSE) );
		break;

	case HTTOP:
	case HTBOTTOM:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZENS) );
		break;

	case HTLEFT:
	case HTRIGHT:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZEWE) );
		break;

	case HTTOPRIGHT:
	case HTBOTTOMLEFT:
		::SetCursor( ::LoadCursor(NULL, IDC_SIZENESW) );
		break;

	case HTCAPTION:
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		break;

	default:
		::SetCursor( ::LoadCursor(NULL, IDC_ARROW) );
		break;	
	}
	
	return TRUE;
}

//
//	ʹ��ͨ���� NCHitTest�з���HTCAPTIONҲ��ʹ�õ����ק��Ч������ȱ���ǻ�һֱ����һ��WM_MOUSELEAVE��Ϣ����Ϊ��겻�ڿͻ�������
//
//	ʹ��WS_THICKFRAMEҲ����ֱ��ʵ�ֱ߿���קЧ����������Ҫȥ��ӦNCCACLSIZE�����һ�����ԣ����ܻᵼ�´�����WIN7������ʾ͸���߿�
//	
//  ����UpdateLayeredWindow֧�ָ�ƽ���Ĵ�������Ч������˶��ڷֲ㴰�ڵ�����������һ���߼�ʵ�ֵ�
//
void CustomWindow::OnLButtonDown(UINT nFlags, POINT pt)
{
	SetMsgHandled(FALSE);

	UINT nHitTest = this->OnHitTest(&pt);

	switch(nHitTest)
	{
	case HTTOPLEFT:
		if (m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPLEFT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTTOP:
		if (m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOP, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTTOPRIGHT:
		if (m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_TOPRIGHT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTLEFT:
		if (m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_LEFT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTRIGHT:
		if (m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_RIGHT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTBOTTOMLEFT:
		if (m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMLEFT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTBOTTOM:
		if (m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOM, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTBOTTOMRIGHT:
		if (m_pLayeredWindowWrap)
			m_pLayeredWindowWrap->OnLButtonDown(nHitTest);
		else
			::PostMessage( m_hWnd, WM_SYSCOMMAND, SC_SIZE|WMSZ_BOTTOMRIGHT, MAKELPARAM(pt.x,pt.y) );
		break;

	case HTCAPTION:
		{
			::PostMessage( m_hWnd, WM_NCLBUTTONDOWN, HTCAPTION, MAKELPARAM(pt.x,pt.y) );
		}
		break;
	}
}

void CustomWindow::OnLButtonUp(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);
	if (m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnLButtonUp();
	}
}

void  CustomWindow::OnMouseMove(UINT nFlags, POINT point)
{
	SetMsgHandled(FALSE);

	if (m_pLayeredWindowWrap)
	{
		m_pLayeredWindowWrap->OnMouseMove();
	}
}

// ע����Ҫ��ӦUIMSG��WM_SIZE����Ϊ��WindowBase::_OnSize�о��Ѿ���ʼ���´����ˣ���
//     ����Ҫ����֮ǰ��m_bNeedToSetWindowRgn��־���ϡ����򽫹����ϱ�־��ȴ�����OnEndEraseBknd
LRESULT  CustomWindow::_OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;

	// ��С���ʹ���С����ԭʱ������Ҫ���¼��㴰�ڵ���״
	if (SIZE_MINIMIZED == wParam || (SIZE_RESTORED==wParam && ::IsIconic(m_hWnd)))
		return 0;

	this->m_bNeedToSetWindowRgn = true;
    return 0;
}

//
//	���ô����Ƿ������ק
//
void CustomWindow::SetWindowResizeType( UINT nType )
{
	m_nResizeBit = nType;
}

//
// �жϴ��ڵ�����״̬λ
//
bool  CustomWindow::TestResizeBit( int nBit )
{
	if (m_nResizeBit & nBit)
		return true;
	else 
		return false;
}


bool  CustomWindow::IsWindowLayered()
{
	return (NULL!=m_pLayeredWindowWrap);
}
//
//	����/ȡ��һ������Ϊ�ֲ㴰��
//
//	Remark
//		���һ��������Ҫ�ڷֲ���Ƿֲ�֮������л��Ļ����������Ӧ UI_WM_GET_GRAPHICS_RENDER_LIBRARY_TYPE ��Ϣ��
//		������RENDER_TYPE_GDIPLUS���͡���ΪĿǰ��֧���л�Ϊ�ֲ㴰��ʱ�����ô�������ʹ�õ���
//		ͼƬ��������Դ��ͬʱת����GDIPLUS���͵ġ�
//
void  CustomWindow::SetWindowLayered(bool b)
{
	if (b && NULL != m_pLayeredWindowWrap)
		return;
	if (!b && NULL == m_pLayeredWindowWrap)
		return;

	if (b)
	{
		m_pLayeredWindowWrap = new LayeredWindowWrap(this);
		m_nWindowTransparentMaskType |= WINDOW_TRANSPARENT_TYPE_LAYERED;
		if (0 == (m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_MASKALPHA) &&
            0 == (m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_MASKCOLOR))
		{
			m_nWindowTransparentMaskType |= WINDOW_TRANSPARENT_TYPE_MASKALPHA;  // �������������ø������Commit��ʱ���������ȷ
			m_nAlphaMask = 255;
		}

		// ʹ�÷ֲ㴰�ڵĻ����Ͳ�Ҫ����ʾ������Ӱ�ˡ����������˵�����Ĭ���Ǵ�SHADOW�ģ�ʹ�÷ֲ㴰��ʱ�Ͳ���Ҫ�����Ӱ�ˣ��Լ�ģ�⼴�ɣ�
		SetClassLong(m_hWnd, GCL_STYLE, GetClassLong(m_hWnd, GCL_STYLE)&~CS_DROPSHADOW);

		SetWindowLong(m_hWnd, 
			GWL_EXSTYLE,
			GetWindowLong(m_hWnd, GWL_EXSTYLE) | WS_EX_LAYERED);

		SetWindowRgn(m_hWnd, NULL, FALSE);   // ȡ�����ڵ����Σ��ɷֲ㴰���Լ��������ֲ㴰�ڽ����յ�һ��WINDOWPOSCHANGED��Ϣ����SWP_NOSIZE����˻���Ҫ���ⷢ��һ��
		                                     // ���ڴ�С����Ϣ���߷ֲ㴰�ڵ�ǰ��С
		m_pRenderChain->GetWindowLayer()->ReCreateRenderTarget();  //����InitLayeredWindow��ǰ�棬������InitLayeredWindow->Invalidate->WM_PAINT�д�����RenderTarget�ֱ�������
		m_pLayeredWindowWrap->InitLayeredWindow();
	}
	else
	{
		m_nWindowTransparentMaskType &= ~WINDOW_TRANSPARENT_TYPE_LAYERED;
		// ע��ֱ��ȥ��WS_EX_LAYERED���ԣ��ᵼ�´����ȱ�ڣ�Ȼ����ˢ��һ�Ρ�
		//     ���������ȥ���ֲ�����֮��ֱ�ӽ�mem bitmap�������Ȼ��������������������⡣
		//     ͬʱΪ�˽�����������ϵ�������rgn����Ĳ��֣���ȥ���ֲ�����֮ǰ���ȼ���
		//     ���ڵ���״
		
		this->UpdateWindowRgn();

		// Remove WS_EX_LAYERED from this window styles
		SetWindowLong(m_hWnd, 
			GWL_EXSTYLE,
			GetWindowLong(m_hWnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);

		HDC hDC = ::GetDC(m_hWnd);
		::BitBlt(hDC,0,0, GetWidth(), GetHeight(), m_pRenderChain->GetMemoryDC(), 0,0, SRCCOPY);
		ReleaseDC(m_hWnd, hDC);

		m_pLayeredWindowWrap->ReleaseLayeredWindow();
		SAFE_DELETE(m_pLayeredWindowWrap);

		// Ask the window and its children to repaint (Bitblt�������ˢ�¹���)
// 		RedrawWindow(m_hWnd, 
// 			NULL, 
// 			NULL, 
// 			RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
		
		m_pRenderChain->GetWindowLayer()->ReCreateRenderTarget();
	}
    if (m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_LAYERED || 
        m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_AREO)
        this->SetTransparent(true); // ���øñ�ʶλ���ⲿͨ���ж�IsTransparent���õ������Ƿ��Ƿֲ㴰�ڡ�����ÿ���Ƿ���Ҫ��ջ��棬����alpha����
    else
        this->SetTransparent(false);

    UIMSG  msg;
    msg.message = UI_WM_WINDOWLAYEREDCHANGED;
    msg.wParam = (WPARAM)b;
    msg.lParam = 0;
    Message::ForwardMessageToChildObject(this, &msg);
}

void  CustomWindow::SetWindowAreo(bool b)
{
    DwmHelper* pDwm = DwmHelper::GetInstance();
    if (b)
    {
        m_nWindowTransparentMaskType |= WINDOW_TRANSPARENT_TYPE_AREO;
        if (pDwm->IsEnable())
        {
            // �ָ���֧��areo�������ˣ����Բ���ʹ��layered
            if (m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_LAYERED)
            {
                SetWindowLayered(false);
            }

            UpdateWindowRgn();
        }
        else
        {
			if (pDwm->pDwmEnableBlurBehindWindow)
			{
				DWM_BLURBEHIND blurbehind = {0};
				blurbehind.dwFlags = DWM_BB_ENABLE;
				blurbehind.fEnable = FALSE;
				pDwm->pDwmEnableBlurBehindWindow(m_hWnd, &blurbehind);
			}

            // ��֧��areo�� �÷ֲ㴰�ڴ���
            SetWindowLayered(true);
        }
    }
    else
    {
        if (m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_AREO)
        {
            m_nWindowTransparentMaskType &= ~WINDOW_TRANSPARENT_TYPE_AREO;

            if (pDwm->pDwmEnableBlurBehindWindow)
            {
                DWM_BLURBEHIND blurbehind = {0};
                blurbehind.dwFlags = DWM_BB_ENABLE;
                blurbehind.fEnable = FALSE;
                pDwm->pDwmEnableBlurBehindWindow(m_hWnd, &blurbehind);
            }
        }
    }
}

void  CustomWindow::SetWindowTransparentMaskType(int type)
{
	m_nWindowTransparentMaskType = type;
}
int   CustomWindow::GetWindowTransparentMaskType()
{
    return m_nWindowTransparentMaskType;
}
void  CustomWindow::SetWindowTransparentColMask( COLORREF col )
{
    SAFE_RELEASE(m_pColMask);
    m_pColMask = Color::CreateInstance(col);
}
void  CustomWindow::SetWindowTransparentColMask( const String& strColdID )
{
	if (NULL == m_pUIApplication)
		return ;

	IColorRes* pColorRes = m_pUIApplication->GetActiveSkinColorRes();
	if (pColorRes)
		pColorRes->GetColor((BSTR)strColdID.c_str(), &m_pColMask);
}
void  CustomWindow::SetWindowTransparentAlphaMask( int nAlpha )
{
	m_nAlphaMask = nAlpha;
}

// -> ע��ÿ��ǿ�Ʒ���HTCAPTION֮�󣬽���������WM_MOUSELEAVE��Ϣ��������ǿͻ����� )
// ����겻�ٴ��ڿؼ��ϵ�λ���϶�ʱ��Ĭ���϶�����
//LRESULT  CustomWindow::OnNcHitTest( POINT pt )
//{
// 	if (NULL == MouseManager::GetObjectByPos( this->GetWindowObject(), &pt))
// 	{
// 		__super::SetMsgHandled(TRUE);
// 		return HTCAPTION;
// 	}
// 	return HTCLIENT;
//}


/*  ע��Removed. �Ľ�������ͨ��ʹ��OnNcHitTest����HTCAPTION��ʵ����ק
	Ŀ�ģ���ʹ��CustomWindow/CustomExWindow��ʱ���������϶���client����û�������ؼ�
	      �� OnNcHitTest �᷵�� HTCAPTION ��
		  �����ͻ����һ�����⣺����HTCAPTION֮�󣬾Ͳ������ WM_MOUSEMOVE��Ϣ�����ǻ�
		  ���� WM_NCMOUSEMOVE ��Ϣ����͸� MouseManager �ڴ��� WM_MOUSEMOVE ��Ϣʱ����
		  �˴�������б�Ҫ�� Custom(Ex)Window�н� WM_NCXXX��Ϣ��ת���� WM_XXX��Ϣ
*/

// void CustomWindow::OnNcMouseMove( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MOUSEMOVE, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMouseHover( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MOUSEHOVER, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMouseLeave()
// {
// 	SetMsgHandled(FALSE);
// 	::SendMessage( m_hWnd, WM_MOUSELEAVE, (WPARAM)0, (LPARAM)0 );
// }
// void CustomWindow::OnNcLButtonDown( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_LBUTTONDOWN, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcLButtonUp( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_LBUTTONUP, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcLButtonDblClk( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_LBUTTONDBLCLK, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcRButtonDown( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_RBUTTONDOWN, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcRButtonUp( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_RBUTTONUP, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcRButtonDblClk( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_RBUTTONDBLCLK, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMButtonDown( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MBUTTONDOWN, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMButtonUp( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MBUTTONUP, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }
// void CustomWindow::OnNcMButtonDblClk( UINT nHitTest, POINT point )
// {
// 	SetMsgHandled(FALSE);
// 	::MapWindowPoints( NULL, m_hWnd, &point, 1 );
// 	::SendMessage( m_hWnd, WM_MBUTTONDBLCLK, (WPARAM)nHitTest, MAKELPARAM(point.x,point.y) );
// }


//////////////////////////////////////////////////////////////////////////


LayeredWindowWrap::LayeredWindowWrap(CustomWindow* pWindow)
{
	m_pWindow = pWindow;

	m_nHitTestFlag = 0;
	m_ptStartSizeMove.x = 0;
	m_ptStartSizeMove.y = 0;
	m_ptWindowOld.x = 0;
	m_ptWindowOld.y = 0;
	m_sizeWindowOld.cx = 0;
	m_sizeWindowOld.cy = 0;
	m_ptWindow.x = NDEF;
	m_ptWindow.y = NDEF;
	m_sizeWindow.cx = NDEF;
	m_sizeWindow.cy = NDEF;
}
LayeredWindowWrap::~LayeredWindowWrap()
{
	m_pWindow = NULL;
}
BOOL  LayeredWindowWrap::PreCreateWindow(CREATESTRUCT* pcs)
{
	// ��ӷֲ����� 
	pcs->dwExStyle |= WS_EX_LAYERED;
	return TRUE;
}


void  LayeredWindowWrap::InitLayeredWindow()
{
	CRect rc;
	::GetWindowRect(m_pWindow->m_hWnd, &rc );

	// ������Ϊ��SetLayerWindowd(true)֮ǰ����˴�������SIZE��WindowPosChanged��Ϣ��
	// ��������һ��
	if (NDEF == m_sizeWindow.cx && NDEF == m_sizeWindow.cy)
	{
		m_sizeWindow.cx = rc.Width();
		m_sizeWindow.cy = rc.Height();
	}
	if (NDEF == m_ptWindow.x && NDEF == m_ptWindow.y)
	{
		m_ptWindow.x = rc.left;
		m_ptWindow.y = rc.top;
	}
}
void  LayeredWindowWrap::ReleaseLayeredWindow()
{
}

void  LayeredWindowWrap::OnWindowPosChanging(LPWINDOWPOS lpWndPos)
{
// 	if (lpWndPos->flags & SWP_SHOWWINDOW)  // ������ʾ����������ʱ��DrawObject��ʧ�ܣ�
// 	{
// 		this->RedrawObject(m_pWindow, TRUE);
// 	}
}
void  LayeredWindowWrap::OnWindowPosChanged(LPWINDOWPOS lpWndPos)
{
    // ��С���ˣ�������һ��m_ptWindow�����ں����ж��Ƿ�����С�������޸�m_sizeWindow�����ô���
    if (lpWndPos->x == WINDOWS_MINIMIZED_POINT && lpWndPos->y == WINDOWS_MINIMIZED_POINT)
    {
        m_ptWindow.x = WINDOWS_MINIMIZED_POINT;
        m_ptWindow.y = WINDOWS_MINIMIZED_POINT;
    }
    else
    {
	    if (!(lpWndPos->flags & SWP_NOMOVE))
	    {
		    m_ptWindow.x = lpWndPos->x;
		    m_ptWindow.y = lpWndPos->y;
	    }
	    if (!(lpWndPos->flags & SWP_NOSIZE))
	    {
		    m_sizeWindow.cx = lpWndPos->cx;
		    m_sizeWindow.cy = lpWndPos->cy;
	    }
	    if (lpWndPos->flags & SWP_SHOWWINDOW)  // ������ʾ����������ʱ��DrawObject��ʧ�ܣ�
	    {
            ::InvalidateRect(m_pWindow->m_hWnd, NULL, TRUE);
	    }
    }
}

// ģ����ק�����������
void  LayeredWindowWrap::OnLButtonDown(UINT nHitTest)
{
	OnEnterSizeMove(nHitTest);
}
void  LayeredWindowWrap::OnLButtonUp()
{
	OnExitSizeMove();
}
void  LayeredWindowWrap::OnCancelMode()
{
	OnExitSizeMove();
}
void  LayeredWindowWrap::OnMouseMove()
{
	if (0 == m_nHitTestFlag)
		return;

	POINT ptCursor;
	GetCursorPos(&ptCursor);

	int nxMoveDiff = 0;
	int nyMoveDiff = 0;

	int oldCX = m_sizeWindow.cx;
	int oldCY = m_sizeWindow.cy;

	// ���㴰�ڵ������� (ע����������/������ʱ���������������/�ߣ����ڼ�������ʱ��Ҫ��������/�������㣬���������λ��)
	switch(m_nHitTestFlag)
	{
	case HTLEFT:
		nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;
	
		if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
			m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
		if (m_pWindow->m_nMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_nMinWidth)
			m_sizeWindow.cx = m_pWindow->m_nMinWidth;

		m_ptWindow.x = m_ptWindowOld.x + m_sizeWindowOld.cx - m_sizeWindow.cx;
		
		break;

	case HTRIGHT:
		nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;
		break;

	case HTTOP:
		nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;

		if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
			m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
		if (m_pWindow->m_nMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_nMinHeight)
			m_sizeWindow.cy = m_pWindow->m_nMinHeight;

		m_ptWindow.y = m_ptWindowOld.y+m_sizeWindowOld.cy - m_sizeWindow.cy;

		break;

	case HTBOTTOM:
		nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
		break;

	case HTTOPLEFT:
		nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

		nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;

		if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
			m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
		if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
			m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
		if (m_pWindow->m_nMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_nMinWidth)
			m_sizeWindow.cx = m_pWindow->m_nMinWidth;
		if (m_pWindow->m_nMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_nMinHeight)
			m_sizeWindow.cy = m_pWindow->m_nMinHeight;

		m_ptWindow.y = m_ptWindowOld.y+m_sizeWindowOld.cy - m_sizeWindow.cy;
		m_ptWindow.x = m_ptWindowOld.x + m_sizeWindowOld.cx - m_sizeWindow.cx;
		
		break;

	case HTTOPRIGHT:
		nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

		nyMoveDiff = m_ptStartSizeMove.y - ptCursor.y;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;

		if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
			m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
		if (m_pWindow->m_nMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_nMinHeight)
			m_sizeWindow.cy = m_pWindow->m_nMinHeight;

		m_ptWindow.y = m_ptWindowOld.y+m_sizeWindowOld.cy - m_sizeWindow.cy;
		break;

	case HTBOTTOMLEFT:
		nxMoveDiff = m_ptStartSizeMove.x - ptCursor.x;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

		if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
			m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
		if (m_pWindow->m_nMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_nMinWidth)
			m_sizeWindow.cx = m_pWindow->m_nMinWidth;

		m_ptWindow.x = m_ptWindowOld.x + m_sizeWindowOld.cx - m_sizeWindow.cx;

		nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
		break;

	case HTBOTTOMRIGHT:
		nxMoveDiff = ptCursor.x - m_ptStartSizeMove.x;
		m_sizeWindow.cx = m_sizeWindowOld.cx + nxMoveDiff;

		nyMoveDiff = ptCursor.y - m_ptStartSizeMove.y;
		m_sizeWindow.cy = m_sizeWindowOld.cy + nyMoveDiff;
		break;
	}

	// ���ƴ��ڴ�С
	if (m_pWindow->m_nMaxWidth != NDEF && m_sizeWindow.cx > m_pWindow->m_nMaxWidth)
		m_sizeWindow.cx = m_pWindow->m_nMaxWidth;
	if (m_pWindow->m_nMaxHeight != NDEF && m_sizeWindow.cy > m_pWindow->m_nMaxHeight)
		m_sizeWindow.cy = m_pWindow->m_nMaxHeight;
	if (m_pWindow->m_nMinWidth != NDEF && m_sizeWindow.cx < m_pWindow->m_nMinWidth)
		m_sizeWindow.cx = m_pWindow->m_nMinWidth;
	if (m_pWindow->m_nMinHeight != NDEF && m_sizeWindow.cy < m_pWindow->m_nMinHeight)
		m_sizeWindow.cy = m_pWindow->m_nMinHeight;

	if (oldCX == m_sizeWindow.cx && oldCY == m_sizeWindow.cy)
	{
		return;
	}


	m_pWindow->SetCanRedraw(false);  // ���汻��գ���ֹ���ڼ���object redraw,�ύ������ʱ�����ݲ�������
	//m_pWindow->CreateDoubleBuffer(m_sizeWindow.cx, m_sizeWindow.cy);
	m_pWindow->m_pRenderChain->OnWindowResize(0, m_sizeWindow.cx, m_sizeWindow.cy);

	// ע�⣺m_rcParent�ĸ���ǧ����ʹ��GetWindowRect����Ϊ���ڵĴ�С���ھ�û�б�
	//       ��������Ҳ��û�в���SendMessage(WM_SIZE)�ķ���
	SetRect(&m_pWindow->m_rcParent, 0,0, m_sizeWindow.cx, m_sizeWindow.cy);
	m_pWindow->UpdateLayout(false);

	m_pWindow->SetCanRedraw(true);
	m_pWindow->UpdateObject();

	// ģ�ⴰ�ڴ�С�ı���Ϣ
	// PS: 20130108 17:07 ����Ϣ���͵�ʱ���Ƶ�OnSize֮��
	//     ��Ҫ����Ϊ��OnSize�У��ᴥ��richedit��setcaretpos֪ͨ��Ȼ���ʱ���ڵĴ�С��û�������ı䣬
	//     �����CCaretWindow::SetCaretPos�е���MapWindowPoints��ȡ���Ĺ����Ļλ�û��Ǿɵģ����¹��
	//     ���洰���ƶ�ʧ�ܡ���˽�CCaretWindow::OnSyncWindowPosChanging�Ӻ�������������⡣
	{
		MSG  msg;
		msg.hwnd = m_pWindow->m_hWnd;
		msg.message = WM_WINDOWPOSCHANGING;
		msg.wParam = 0;

		WINDOWPOS  wndpos;
		memset(&wndpos, 0, sizeof(&wndpos));
		wndpos.hwnd = msg.hwnd;
		wndpos.flags = SWP_LAYEREDWINDOW_SIZEMOVE|SWP_NOZORDER|SWP_NOACTIVATE|SWP_NOSENDCHANGING;
		wndpos.x = m_ptWindow.x;
		wndpos.y = m_ptWindow.y;
		wndpos.cx = m_sizeWindow.cx;
		wndpos.cy = m_sizeWindow.cy;
		msg.lParam = (LPARAM)&wndpos;
		::SendMessage(msg.hwnd, msg.message, msg.wParam, msg.lParam);
	}
}
void  LayeredWindowWrap::OnEnterSizeMove(UINT nHitTest)
{
	SetCapture(m_pWindow->m_hWnd);
	m_nHitTestFlag = nHitTest;

	POINT ptWindow;
	GetCursorPos(&ptWindow);

	m_ptStartSizeMove.x = ptWindow.x;
	m_ptStartSizeMove.y = ptWindow.y;

	m_ptWindowOld.x = m_ptWindow.x;
	m_ptWindowOld.y = m_ptWindow.y;
	m_sizeWindowOld.cx = m_sizeWindow.cx;
	m_sizeWindowOld.cy = m_sizeWindow.cy;
}

void  LayeredWindowWrap::OnExitSizeMove()
{
	if (0 == m_nHitTestFlag)
		return;

	if (m_pWindow->m_hWnd == GetCapture())
		ReleaseCapture();

	m_nHitTestFlag = 0;
	m_ptStartSizeMove.x = 0;
	m_ptStartSizeMove.y = 0;

	m_ptWindowOld.x = 0;
	m_ptWindowOld.y = 0;
	m_sizeWindowOld.cx = 0;
	m_sizeWindowOld.cy = 0;
}

// ��������С���˵�ʱ������ٴ���ԭpoint/size, Commit���ᵼ�´����ָֻ�����С��֮ǰ��λ��
// �������һ����־�����������С���ˣ����޸Ĵ���λ�ã�����ԭ���ڴ�С�����ύ
bool  LayeredWindowWrap::IsMinimized()
{
    if (m_ptWindow.x == WINDOWS_MINIMIZED_POINT && m_ptWindow.y == WINDOWS_MINIMIZED_POINT)
        return true;

    return false;
};

void  LayeredWindowWrap::Commit2LayeredWindow()
{
	// ��Ҫ��Ϊ�˷�ֹ�ڷֲ㴰�ڴ�С�ı�ʱ����Ҫ���´������棬
	// �ڻ�������������һ��֮ǰ��ֹ�ύ��������
	if (!m_pWindow->CanRedraw())  
		return; 

	POINT ptMemDC  = {0,0};
	int   nFlag = ULW_OPAQUE;
	DWORD dwColorMask = 0;

	BLENDFUNCTION bf;
	bf.BlendOp     = AC_SRC_OVER ;
	bf.AlphaFormat = AC_SRC_ALPHA;        // AC_SRC_ALPHA �ᵼ�´��ڱ�§��,AC_SRC_OVER��ʹ��͸��
	bf.BlendFlags  = 0;                   // Must be zero. 
	bf.SourceConstantAlpha = 255;         // 0~255 ͸����

	// TODO [ע����xp��win7��ʹ���������������£�COLOR�������⣬��˽���ȫ��ʹ��PNG��ʵ��͸��
	if (m_pWindow->m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_MASKALPHA)
	{
		nFlag &= ~ULW_OPAQUE;
		nFlag |= ULW_ALPHA;
		bf.SourceConstantAlpha = m_pWindow->m_nAlphaMask;
	}
	else if (m_pWindow->m_nWindowTransparentMaskType & WINDOW_TRANSPARENT_TYPE_MASKCOLOR) 
	{
		if (m_pWindow->m_pColMask)
		{
			dwColorMask = m_pWindow->m_pColMask->m_col;
			nFlag = ULW_COLORKEY;
		}
	}

    // ��С����ҲҪˢ�£������ԭ֮��ᷢ�ֻ��治�����ˡ���Ϊ����ʾ��������һ˲�䣬������С��ǰ�Ļ���
     BOOL  bRet = ::UpdateLayeredWindow(
                        m_pWindow->m_hWnd, NULL, 
                        IsMinimized() ? NULL : &m_ptWindow, 
                        &m_sizeWindow, 
                        m_pWindow->m_pRenderChain->GetMemoryDC(), 
                        &ptMemDC, dwColorMask, &bf, nFlag); 

//  m_pWindow->m_pRenderChain->GetMemoryBuffer()->Save(L"C:\\aaa.png", Gdiplus::ImageFormatPNG);
	if (FALSE == bRet)
	{
		UI_LOG_ERROR(_T("%s UpdateLayeredWindow Failed."), FUNC_NAME);
	}
}
