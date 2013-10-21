#include "stdafx.h"
#include "label.h"
#include "UISDK\Kernel\Inc\Util\igifimage.h"

namespace UI
{

Label::Label()
{
    m_pILabel = NULL;
}

Label::~Label()
{
}


const TCHAR*  Label::GetText()
{
	return this->m_strText.c_str();
}
void  Label::SetText(const TCHAR* pszText, bool bRedraw)
{
	if (NULL == pszText)
		m_strText.clear();
	else
		m_strText = pszText;

	if (bRedraw)
		m_pILabel->UpdateObject();
}


void Label::OnPaint(IRenderTarget* pRenderTarget)
{
	CRect  rcText;
	m_pILabel->GetClientRect(&rcText);

    ITextRenderBase* p = m_pILabel->GetTextRender();
    if (p)
	    p->DrawState(pRenderTarget, &rcText, 0, m_strText.c_str());
}


void Label::OnLButtonUp(UINT nFlags, POINT point)
{
    IObject* pParent = m_pILabel->GetParentObject();
    if (NULL == pParent)
        return;

    POINT ptObj = {0,0};
    pParent->WindowPoint2ObjectClientPoint(&point, &ptObj);

    // UI_WM_HITTEST���ж����Լ���m_rcParent�Ľ��������Ҫ����������ת����parent��client����
    UINT nHitTest = UISendMessage(m_pILabel, UI_WM_HITTEST, ptObj.x, ptObj.y);
    if (HTNOWHERE != nHitTest)
    {
        this->OnClicked(&point);   // ��ע������DoNotify���ܵ��µ�ǰpress hover�������ı䣬ʹ�ñ��ؼ���ʧˢ��
    }
}


void Label::OnClicked(POINT* pt)
{
    // ֪ͨ��Ϣ
    UIMSG   msg;
    msg.message = UI_WM_NOTIFY;
    msg.nCode   = UI_BN_CLICKED;
    msg.wParam  = NULL; //(WPARAM)pt;
    msg.lParam  = NULL;
    msg.pMsgFrom = this->m_pILabel;

    msg.pMsgTo = NULL;
    m_pILabel->DoNotify(&msg);
}


void Label::ResetAttribute()
{
	DO_PARENT_PROCESS(ILabel, IControl);

	m_pILabel->SetTabstop(false);   // Label �����н���
	m_pILabel->ModifyStyle(OBJECT_STYLE_REJECT_MOUSE_MSG_ALL|OBJECT_STYLE_TRANSPARENT, 0, 0);  // Ĭ�ϲ����������Ϣ
}
void  Label::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	DO_PARENT_PROCESS(ILabel, IControl);

    IUIApplication* pUIApp = NULL;
    pUIApp = m_pILabel->GetUIApplication();

    ITextRenderBase* pTextRender = NULL;
    pMapAttrib->GetAttr_TextRenderBase(XML_TEXTRENDER_TYPE, true, pUIApp, static_cast<IObject*>(m_pILabel), &pTextRender);
    if (pTextRender)
    {
        m_pILabel->SetTextRender(pTextRender);
        pTextRender->Release();
    }
    if (NULL == m_pILabel->GetTextRender())
    {
        m_pILabel->GetUIApplication()->CreateTextRenderBase(TEXTRENDER_TYPE_NORMAL, m_pILabel, &pTextRender);
        if (pTextRender)
        {
            pTextRender->SetAttribute(pMapAttrib, NULL, true);
            m_pILabel->SetTextRender(pTextRender);
            pTextRender->Release();
        }
    }

	// ����
    if (false==bReload)
    {
	    const TCHAR* szText = pMapAttrib->GetAttr(XML_TEXT, true);
	    if (szText)
		    this->m_strText = szText;
        else
            this->m_strText.clear();
    }
}

void Label::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = pSize->cy = 0;

    ITextRenderBase* pTextRender = m_pILabel->GetTextRender();
	if (pTextRender)
	{
		*pSize = pTextRender->GetDesiredSize(m_strText.c_str());
	}
	pSize->cx += m_pILabel->GetNonClientW();
	pSize->cy += m_pILabel->GetNonClientH();
}

 
PictureCtrl::PictureCtrl()
{
    m_pIPictureCtrl = NULL;
}
PictureCtrl::~PictureCtrl()
{
}

void PictureCtrl::ResetAttribute()
{
	DO_PARENT_PROCESS(IPictureCtrl, IControl);

	m_pIPictureCtrl->SetTabstop(false);   // Picture �����н���
	m_pIPictureCtrl->ModifyStyle(OBJECT_STYLE_REJECT_MOUSE_MSG_ALL, 0, 0);  // Ĭ�ϲ����������Ϣ
}

void  PictureCtrl::GetDesiredSize(SIZE* pSize)
{
	pSize->cx = pSize->cy = 0;
    IRenderBase* p = NULL;

    p = m_pIPictureCtrl->GetBkRender();
	if (p)
		*pSize = p->GetDesiredSize();
	
    p = m_pIPictureCtrl->GetForeRender();
    if (p)
        *pSize = p->GetDesiredSize();
}

void PictureCtrl::OnPaint(IRenderTarget* pRenderTarget)
{
    IRenderBase* p = m_pIPictureCtrl->GetForeRender();;
	if (p)
	{
		CRect rc;
		m_pIPictureCtrl->GetClientRectAsWin32(&rc);
		p->DrawState(pRenderTarget, &rc, 0);   // TODO: ���������չΪ֧��hover/press״̬
	}
}

//////////////////////////////////////////////////////////////////////////

GifCtrl::GifCtrl()
{
	m_pGifRender = NULL;
}
GifCtrl::~GifCtrl()
{
	SAFE_RELEASE(m_pGifRender);
}

void  GifCtrl::OnGetDesiredSize(SIZE* pSize)
{
    pSize->cx = pSize->cy = 0;
	if (m_pGifRender)
	{
		pSize->cx = m_pGifRender->GetWidth();
		pSize->cy = m_pGifRender->GetHeight();
	}
}

void GifCtrl::ResetAttribute()
{
	DO_PARENT_PROCESS(IGifCtrl, IControl);

	m_pIGifCtrl->SetTabstop(false);   // GifPicture �����н���
	m_pIGifCtrl->ModifyStyle(OBJECT_STYLE_REJECT_MOUSE_MSG_ALL, OBJECT_STYLE_TRANSPARENT, false);  // Ĭ�ϲ����������Ϣ// Ĭ�ϲ�͸��
}

void GifCtrl::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
	DO_PARENT_PROCESS(IGifCtrl, IControl);

	const TCHAR*  szText = pMapAttrib->GetAttr(XML_GIFCTRL_GIF, true);
	if (szText)
	{
		IGifImage*  pGifImage = NULL;

		IGifRes* pGifRes = m_pIGifCtrl->GetUIApplication()->GetActiveSkinGifRes();
		if (pGifRes)
			pGifRes->GetGifImage(szText, &pGifImage);

		if (pGifImage)
		{
			SAFE_RELEASE(m_pGifRender);

//			POINT pt = this->GetRealPosInWindow();
//			Gif_Timer_Notify notify(GetHWND(), pt.x + m_rcNonClient.left, pt.y + m_rcNonClient.top);

            Gif_Timer_Notify notify;
            memset(&notify, 0, sizeof(notify));
            notify.eType = Gif_Timer_Notify_Post_Thread_Msg;
            notify.notify_ui_msg.pNotifyMsgObj = static_cast<IMessage*>(m_pIGifCtrl);
            notify.notify_ui_msg.nTimerID = 1;

			m_pGifRender = pGifImage->AddRender(&notify);
		}
		else
		{
			UI_LOG_WARN(_T("%s load gif image failed. id=%s"), FUNC_NAME, szText);
		}
	}
}

void  GifCtrl::OnPaint(IRenderTarget* pRenderTarget)
{
	if (m_pGifRender)
	{
//      if (GIF_DRAW_STATUS_STOP == m_pGifRender->GetStatus()) // ������gif���ص�ʱ���ڻ���
//          m_pGifRender->Start();

		// GIF����ֱ�Ӵ�alphaͨ���ģ�������ԭʼHDC���ơ�����ʹ��Gdipulus::Graphics.GetHDC
		// ���󽵵ͻ���Ч��
		HDC hDC = pRenderTarget->GetBindHDC(); 
		                     
        if (pRenderTarget->GetGraphicsRenderLibraryType() == GRAPHICS_RENDER_LIBRARY_TYPE_GDI)
        {
		    m_pGifRender->OnPaint(hDC, 0,0);  // ��ΪHDC���Ѿ���ƫ�����ģ����ֱ�ӻ�����0,0����
        }
        else
        {
            m_pGifRender->OnAlphaPaint(hDC, 0,0);  
        }
	}
}

void  GifCtrl::OnMove(CPoint ptPos)
{
	if (m_pGifRender)
	{
// 		POINT pt = this->GetRealPosInWindow();
// 		Gif_Timer_Notify notify(GetHWND(),pt.x + m_rcNonClient.left, pt.y + m_rcNonClient.top);
// 		m_pGifRender->ModifyRenderParam(&notify);
	}
}

void  GifCtrl::OnTimer(UINT_PTR nIDEvent, LPARAM lParam)
{
    // ����жϺ���û�б�Ҫ��ֱ��ˢ�°�
//	if (m_pGifRender && m_pGifRender->GetStatus() == GIF_DRAW_STATUS_START)
		m_pIGifCtrl->UpdateObject();
}

bool  GifCtrl::Start()
{
    if (NULL == m_pGifRender)
        return false;

    m_pGifRender->Start();
    return true;
}
bool  GifCtrl::Pause()
{
    if (NULL == m_pGifRender)
        return false;

    m_pGifRender->Pause();
    return true;
}
bool  GifCtrl::Stop()
{
    if (NULL == m_pGifRender)
        return false;

    m_pGifRender->Stop();
    return true;
}
}