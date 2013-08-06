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


    // 文字排版
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

	// 内容
    if (!bReload)
    {
	    szText = pMapAttr->GetAttr(XML_TEXT, true);
	    if (szText)
		    SetText(szText);
    }

	// 边框
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
	// 将文字区域裁剪出来
	//
	HRGN hRgnOld = pRenderTarget->GetClipRgn();

	CRect rc = m_rcTextClipInWindow;
	POINT pt = m_pIGroupBox->GetRealPosInWindow();
	rc.OffsetRect(pt.x, pt.y);

	HRGN hRgnClip = ::CreateRectRgnIndirect(&rc);  // 剪裁区域是基于窗口的，而不是基于偏移
	pRenderTarget->SelectClipRgn(hRgnClip, RGN_DIFF);
	::DeleteObject(hRgnClip);

	//
	//	绘制边框
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
	//	还原剪裁区域
	//
	pRenderTarget->SelectClipRgn(hRgnOld);
	::DeleteObject(hRgnOld);

	//
	//	绘制文字，放在onerasebkgnd中可以直接画在ncclient区域，不受限制
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

    // 这里只作简短文字的测量，对于长文字分两行不支持
    if (m_nTextAlign & ALIGN_VCENTER)
        m_sizeText = pTextRender->GetDesiredSize(m_strText.c_str(), 1);
    else
        m_sizeText = pTextRender->GetDesiredSize(m_strText.c_str());
}


// 注：CalcBorderRect不能仅放在OnEraseBkgnd中调用，因为CalcBorderRect会导致自己的client size改变，
//     导致内部控件位置需要进行调整。（例如内部一个EDIT layout.left/right/top/bottom=10）
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

            if (m_nTextAlign & ALIGN_CENTER)   // 右侧居中
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()-m_sizeText.cy)/2);
            else if (m_nTextAlign & ALIGN_BOTTOM) // 右侧靠下
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()- m_nTextIndent - m_rcTextMargin.bottom - m_sizeText.cy));
            else                               // 右侧靠上
                m_rcText.OffsetRect(0, m_nTextIndent + m_rcTextMargin.top);
        }
        else
        {
            m_rcBorderForImage.left = (m_sizeText.cx + m_rcTextMargin.left + m_rcTextMargin.right)/2;
            m_rcText.SetRect(0, 0, m_sizeText.cx, m_sizeText.cy);
            m_rcText.OffsetRect(m_rcTextMargin.left, 0);

            if (m_nTextAlign & ALIGN_CENTER)   // 左侧居中
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()-m_sizeText.cy)/2);
            else if (m_nTextAlign & ALIGN_BOTTOM) // 左侧靠下
                m_rcText.OffsetRect(0, (m_pIGroupBox->GetHeight()- m_nTextIndent - m_rcTextMargin.bottom - m_sizeText.cy));
            else                               // 左侧靠上
                m_rcText.OffsetRect(0, m_nTextIndent + m_rcTextMargin.top);
        }
    }
    else if (m_nTextAlign & ALIGN_BOTTOM)      // 下层
    {
        m_rcBorderForImage.bottom = rcGroupBox.bottom - ((m_sizeText.cy + m_rcTextMargin.top + m_rcTextMargin.bottom)/2);
        m_rcText.SetRect(0, rcGroupBox.bottom-m_sizeText.cy, m_sizeText.cx, rcGroupBox.bottom);
        m_rcTextMargin.OffsetRect(0, -m_rcTextMargin.bottom);

        if (m_nTextAlign & ALIGN_CENTER)       // 下层居中
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()-m_sizeText.cx)/2, 0);
        else if (m_nTextAlign & ALIGN_RIGHT)   // 下层靠右
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()- m_nTextIndent - m_rcTextMargin.right - m_sizeText.cx), 0);
        else                                   // 下层靠左
            m_rcText.OffsetRect(m_nTextIndent + m_rcTextMargin.left, 0);
    }
    else // if (m_nTextAlign & ALIGN_TOP)      // 上层
    {
        m_rcBorderForImage.top = (m_sizeText.cy + m_rcTextMargin.top + m_rcTextMargin.bottom)/2;
        m_rcText.SetRect(0, 0, m_sizeText.cx, m_sizeText.cy);
        m_rcText.OffsetRect(0, m_rcTextMargin.top);

        if (m_nTextAlign & ALIGN_CENTER)        // 上层居中
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()-m_sizeText.cx)/2, 0);
        else if (m_nTextAlign & ALIGN_RIGHT)    // 上层靠右
            m_rcText.OffsetRect((m_pIGroupBox->GetWidth()- m_nTextIndent - m_rcTextMargin.right - m_sizeText.cx), 0);
        else                                    // 上层靠左
            m_rcText.OffsetRect(m_nTextIndent + m_rcTextMargin.left, 0);
    }

    // 计算需要的裁剪区域
    m_rcTextClipInWindow.CopyRect(&m_rcText);
    m_rcTextClipInWindow.InflateRect(m_rcTextMargin.left, m_rcTextMargin.top, m_rcTextMargin.right, m_rcTextMargin.bottom);

    // 设置控件边框
    CRegion4 rBorder(2,2,2,2);  // 设置控件的border区域
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