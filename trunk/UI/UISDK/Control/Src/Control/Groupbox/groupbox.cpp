#include "stdafx.h"
#include "groupbox.h"
#include "UISDK\Control\Src\Renderbase\renderbasedefine.h"

namespace UI
{

GroupBox::GroupBox()
{
    m_pIGroupBox = NULL;
    m_rcBorderForImage.SetRectEmpty();
    m_rcTextClipInWindow.SetRectEmpty();
    m_rcText.SetRectEmpty();
    m_sizeText.cx = m_sizeText.cy = 0;
    m_bNeedMeasureText = false;
}
GroupBox::~GroupBox()
{
}

void GroupBox::ResetAttribute()
{
	DO_PARENT_PROCESS(IGroupBox, IPanel);
    m_rcTextMargin.SetRectEmpty();
    m_nTextIndent = 10;
    m_nTextAlign = 0;
    m_pIGroupBox->ModifyStyle(OBJECT_STYLE_TRANSPARENT,0, 0);
    m_bNeedCalcRect = true;
}
void GroupBox::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
{
    DO_PARENT_PROCESS(IGroupBox, IPanel);

    IUIApplication*  pUIApp = m_pIGroupBox->GetUIApplication();
 
    ITextRenderBase* pTextRendr = NULL;
    pMapAttr->GetAttr_TextRenderBase(XML_TEXTRENDER_TYPE, true, pUIApp, m_pIGroupBox, &pTextRendr);
    if (pTextRendr)
    {
        pTextRendr->SetTextAlignment(DT_EDITCONTROL|DT_WORDBREAK);
        m_pIGroupBox->SetTextRender(pTextRendr);
        SAFE_RELEASE(pTextRendr);
    }
	else if (NULL == m_pIGroupBox->GetTextRender())
	{
        pTextRendr = NULL;
        pUIApp->CreateTextRenderBase(TEXTRENDER_TYPE_NORMAL, m_pIGroupBox, &pTextRendr);
        if (pTextRendr)
        {
            pTextRendr->SetTextAlignment(DT_EDITCONTROL|DT_WORDBREAK);
    		pTextRendr->SetAttribute(pMapAttr, NULL, true);
            m_pIGroupBox->SetTextRender(pTextRendr);
            SAFE_RELEASE(pTextRendr);
        }
	}


    // �����Ű�
    pMapAttr->GetAttr_int(XML_GROUPBOX_TEXTINDENT, true, &m_nTextIndent);
    pMapAttr->GetAttr_CRegion4(XML_GROUPBOX_TEXTMARGIN, true, &m_rcTextMargin);

    const TCHAR* szText = pMapAttr->GetAttr(XML_GROUPBOX_TEXTALIGN, true);
    if (szText)
    {
        Util::ISplitStringEnum* pEnum = NULL;
        int nSize = Util::SplitString(szText, XML_FLAG_SEPARATOR, &pEnum);
    
        for (int i = 0; i < nSize; i++)
        {
            const TCHAR*  szText = pEnum->GetText(i);
            if (NULL == szText)
                continue;

            if (0 == _tcscmp(szText, XML_ALIGN_LEFT))
                m_nTextAlign |= ALIGN_LEFT;
            else if (0 == _tcscmp(szText, XML_ALIGN_TOP))
                m_nTextAlign |= ALIGN_TOP;
            else if (0 == _tcscmp(szText, XML_ALIGN_RIGHT))
                m_nTextAlign |= ALIGN_RIGHT;
            else if (0 == _tcscmp(szText, XML_ALIGN_BOTTOM))
                m_nTextAlign |= ALIGN_BOTTOM;
            else if (0 == _tcscmp(szText, XML_ALIGN_CENTER))
                m_nTextAlign |= ALIGN_CENTER;
            else if (0 == _tcscmp(szText, XML_ALIGN_VCENTER))
                m_nTextAlign |= ALIGN_VCENTER;
        }
        SAFE_RELEASE(pEnum);
    }

	// ����
    if (!bReload)
    {
	    szText = pMapAttr->GetAttr(XML_TEXT, true);
	    if (szText)
		    SetText(szText);
    }

	// �߿�
	if (NULL == m_pIGroupBox->GetBkRender())
	{
        IRenderBase* p = NULL;
        pUIApp->CreateRenderBase(RENDER_TYPE_THEME_GROUPBOX_BKGND, m_pIGroupBox, &p);
        if (p)
        {
		    p->SetAttribute(pMapAttr, XML_BACKGND_RENDER_PREFIX, true);
            m_pIGroupBox->SetBkgndRender(p);
            SAFE_RELEASE(p);
        }
	}
}

void GroupBox::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
    if (m_bNeedMeasureText)
        this->MeasureText();
	if (m_bNeedCalcRect)
		this->CalcBorderRect();

    //
	// ����������ü�����
	//
	HRGN hRgnOld = pRenderTarget->GetClipRgn();

	CRect rc = m_rcTextClipInWindow;
	POINT pt = m_pIGroupBox->GetRealPosInWindow();
	rc.OffsetRect(pt.x, pt.y);

	HRGN hRgnClip = ::CreateRectRgnIndirect(&rc);  // ���������ǻ��ڴ��ڵģ������ǻ���ƫ��
	pRenderTarget->SelectClipRgn(hRgnClip, RGN_DIFF);
	::DeleteObject(hRgnClip);

	//
	//	���Ʊ߿�
	//
    IRenderBase* pBkgndRender = m_pIGroupBox->GetBkRender();
	if (pBkgndRender)
	{
		if (m_pIGroupBox->IsEnable())
			pBkgndRender->DrawState(pRenderTarget, &m_rcBorderForImage, GROUPBOX_BKGND_RENDER_STATE_NORMAL);
		else
			pBkgndRender->DrawState(pRenderTarget, &m_rcBorderForImage, GROUPBOX_BKGND_RENDER_STATE_DISABLE);
	}

	//
	//	��ԭ��������
	//
	pRenderTarget->SelectClipRgn(hRgnOld);
	::DeleteObject(hRgnOld);

	//
	//	�������֣�����onerasebkgnd�п���ֱ�ӻ���ncclient���򣬲�������
	//
    ITextRenderBase* pTextRender = m_pIGroupBox->GetTextRender();
	if (pTextRender)
		pTextRender->DrawState(pRenderTarget, &m_rcText, 0, m_strText.c_str(), -1);
}

void GroupBox::OnSize(UINT nType, int cx, int cy)
{
	m_bNeedCalcRect = true;
    SetMsgHandled(FALSE);
}

void  GroupBox::SetText(const TCHAR* szText)
{
    if (NULL == szText)
        m_strText.clear();
    else
        m_strText = szText;

    m_bNeedMeasureText = true;
    m_bNeedCalcRect = true;

    this->MeasureText();
    this->CalcBorderRect();
}

void  GroupBox::MeasureText()
{
    m_bNeedMeasureText = false;
    ITextRenderBase* pTextRender = m_pIGroupBox->GetTextRender();
    if (NULL == pTextRender)
        return;

    // ����ֻ��������ֵĲ��������ڳ����ַ����в�֧��
    if (m_nTextAlign & ALIGN_VCENTER)
        m_sizeText = pTextRender->GetDesiredSize(m_strText.c_str(), 1);
    else
        m_sizeText = pTextRender->GetDesiredSize(m_strText.c_str());
}


// ע��CalcBorderRect���ܽ�����OnEraseBkgnd�е��ã���ΪCalcBorderRect�ᵼ���Լ���client size�ı䣬
//     �����ڲ��ؼ�λ����Ҫ���е������������ڲ�һ��EDIT layout.left/right/top/bottom=10��
void  GroupBox::CalcBorderRect()
{
    m_bNeedCalcRect = false;

    RECT rcGroupBox = { 0, 0, m_pIGroupBox->GetWidth(), m_pIGroupBox->GetHeight() };
    CopyRect(m_rcBorderForImage, &rcGroupBox);

    if (m_nTextAlign & ALIGN_VCENTER)
    {
        if (m_nTextAlign & ALIGN_RIGHT)
        {
            m_rcBorderForImage.right = rcGroupBox.right - ((m_sizeText.cx + m_rcTextMargin.left + m_rcTextMargin.right)/2);
            m_rcText.SetRect(rcGroupBox.right-m_sizeText.cx, 0, rcGroupBox.right, m_sizeText.cy);
            m_rcText.OffsetRect(-m_rcTextMargin.right, 0);

            if (m_nTextAlign & ALIGN_CENTER)   // �Ҳ����
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()-m_sizeText.cy)/2);
            else if (m_nTextAlign & ALIGN_BOTTOM) // �Ҳ࿿��
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()- m_nTextIndent - m_rcTextMargin.bottom - m_sizeText.cy));
            else                               // �Ҳ࿿��
                m_rcText.OffsetRect(0, m_nTextIndent + m_rcTextMargin.top);
        }
        else
        {
            m_rcBorderForImage.left = (m_sizeText.cx + m_rcTextMargin.left + m_rcTextMargin.right)/2;
            m_rcText.SetRect(0, 0, m_sizeText.cx, m_sizeText.cy);
            m_rcText.OffsetRect(m_rcTextMargin.left, 0);

            if (m_nTextAlign & ALIGN_CENTER)   // ������
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()-m_sizeText.cy)/2);
            else if (m_nTextAlign & ALIGN_BOTTOM) // ��࿿��
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()- m_nTextIndent - m_rcTextMargin.bottom - m_sizeText.cy));
            else                               // ��࿿��
                m_rcText.OffsetRect(0, m_nTextIndent + m_rcTextMargin.top);
        }
    }
    else if (m_nTextAlign & ALIGN_BOTTOM)      // �²�
    {
        m_rcBorderForImage.bottom = rcGroupBox.bottom - ((m_sizeText.cy + m_rcTextMargin.top + m_rcTextMargin.bottom)/2);
        m_rcText.SetRect(0, rcGroupBox.bottom-m_sizeText.cy, m_sizeText.cx, rcGroupBox.bottom);
        m_rcTextMargin.OffsetRect(0, -m_rcTextMargin.bottom);

        if (m_nTextAlign & ALIGN_CENTER)       // �²����
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()-m_sizeText.cx)/2, 0);
        else if (m_nTextAlign & ALIGN_RIGHT)   // �²㿿��
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()- m_nTextIndent - m_rcTextMargin.right - m_sizeText.cx), 0);
        else                                   // �²㿿��
            m_rcText.OffsetRect(m_nTextIndent + m_rcTextMargin.left, 0);
    }
    else // if (m_nTextAlign & ALIGN_TOP)      // �ϲ�
    {
        m_rcBorderForImage.top = (m_sizeText.cy + m_rcTextMargin.top + m_rcTextMargin.bottom)/2;
        m_rcText.SetRect(0, 0, m_sizeText.cx, m_sizeText.cy);
        m_rcText.OffsetRect(0, m_rcTextMargin.top);

        if (m_nTextAlign & ALIGN_CENTER)        // �ϲ����
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()-m_sizeText.cx)/2, 0);
        else if (m_nTextAlign & ALIGN_RIGHT)    // �ϲ㿿��
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()- m_nTextIndent - m_rcTextMargin.right - m_sizeText.cx), 0);
        else                                    // �ϲ㿿��
            m_rcText.OffsetRect(m_nTextIndent + m_rcTextMargin.left, 0);
    }

    // ������Ҫ�Ĳü�����
    m_rcTextClipInWindow.CopyRect(&m_rcText);
    m_rcTextClipInWindow.InflateRect(m_rcTextMargin.left, m_rcTextMargin.top, m_rcTextMargin.right, m_rcTextMargin.bottom);

    // ���ÿؼ��߿�
    CRegion4 rBorder(2,2,2,2);  // ���ÿؼ���border����
    if (m_nTextAlign & ALIGN_VCENTER)
    {
        if (m_nTextAlign & ALIGN_RIGHT)
            rBorder.right = m_rcTextClipInWindow.Width();
        else
            rBorder.left = m_rcTextClipInWindow.Width();
    }
    else if (m_nTextAlign & ALIGN_BOTTOM)
    {
        rBorder.bottom = m_rcTextClipInWindow.Height();
    }
    else
    {
        rBorder.top = m_rcTextClipInWindow.Height();
    }
    m_pIGroupBox->SetBorderRegion(&rBorder);
}

}