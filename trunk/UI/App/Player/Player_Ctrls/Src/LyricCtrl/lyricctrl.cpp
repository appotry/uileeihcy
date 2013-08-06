#include "stdafx.h"
#include "lyricctrl.h"

LyricCtrl::LyricCtrl()
{
    m_pILyricCtrl = NULL;
    m_pLyricItemArray = NULL;
    m_nLyricLineArraySize = 0;

    m_pColor = NULL;
    m_pRenderFont = NULL;
    m_nItemHeight = 0;
    m_nVScroll = -50;
    m_nCurItemIndex = 0;
}

LyricCtrl::~LyricCtrl()
{
    SAFE_RELEASE(m_pColor);
    SAFE_RELEASE(m_pRenderFont);
    SAFE_ARRAY_DELETE(m_pLyricItemArray);
    m_nLyricLineArraySize = 0;
}
void  LyricCtrl::SetAttribute(UI::IMapAttribute* pMapAttrib, bool bReload)
{
    DO_PARENT_PROCESS(ILyricCtrl, UI::IControl);

    UI::IColorRes* pColorRes = m_pILyricCtrl->GetUIApplication()->GetActiveSkinColorRes();
    UI::IFontRes*  pFontRes = m_pILyricCtrl->GetUIApplication()->GetActiveSkinFontRes();

    // 颜色
    const TCHAR* szText = pMapAttrib->GetAttr(XML_TEXTRENDER_COLOR, true);
    if (szText)
        pColorRes->GetColor((BSTR)szText, &m_pColor);

    szText = pMapAttrib->GetAttr(XML_TEXTRENDER_FONT, true);
    if (szText)
        pFontRes->GetFont((BSTR)szText, GetRenderLibraryType(m_pILyricCtrl), &m_pRenderFont);

    if (NULL == m_pRenderFont)
    {
        UI::IRenderFont* pFont = m_pILyricCtrl->GetRenderFont();
        if (pFont)
        {
            m_pRenderFont = m_pILyricCtrl->GetRenderFont();
            m_pRenderFont->AddRef();
        }
        else
        {
            // 可能是没有窗口对象，比如是一个 popup listbox或者menu，窗口还没有创建。获取默认字体
            pFontRes->GetDefaultFont(GetRenderLibraryType(m_pILyricCtrl), &m_pRenderFont);
        }
    }
}

void  LyricCtrl::ResetAttribute()
{
    DO_PARENT_PROCESS(ILyricCtrl, UI::IControl);

    SAFE_RELEASE(m_pColor);
    SAFE_RELEASE(m_pRenderFont);

    m_nItemHeight = 20;
}

void  LyricCtrl::OnPaint(UI::IRenderTarget* pRenderTarget)
{
    int nY = 0;

    CRect  rc;
    m_pILyricCtrl->GetClientRectAsWin32(&rc);
    rc.top -= m_nVScroll;
    rc.bottom = rc.top + m_nItemHeight;

    UI::DRAWTEXTPARAM param;
    for (int i = 0; i < m_nLyricLineArraySize; i++)
    {
        if (i == m_nCurItemIndex)
            param.color.ReplaceRGB(RGB(0,255,255));
        else
            param.color.ReplaceRGB(RGB(0,255,0));

        param.nFormatFlag = DT_CENTER|DT_VCENTER|DT_SINGLELINE;
        param.szText = m_pLyricItemArray[i]->m_strLyric.c_str();
        param.prc = &rc;
        pRenderTarget->DrawString(m_pRenderFont, &param);
        rc.top += m_nItemHeight;
        rc.bottom += m_nItemHeight;
    }
}

bool  LyricCtrl::LoadLyric(const TCHAR*  szBuffer)
{
    // 语法解析
    if (FAILED(m_lyricData.Analyze(szBuffer)))
        return false;

    // 创建数组 
    m_nLyricLineArraySize = (int)m_lyricData.m_mapLyricLine.size();
    if (0 == m_nLyricLineArraySize)
        return false;

    m_nVScroll = -50;
    m_nCurItemIndex = 0;
    SAFE_ARRAY_DELETE(m_pLyricItemArray);
    m_pLyricItemArray = new CLyricItemData*[m_nLyricLineArraySize];

    _LyricLineMap::iterator iter = m_lyricData.m_mapLyricLine.begin();
    for (int i = 0; iter != m_lyricData.m_mapLyricLine.end(); iter++, i++)
    {
        m_pLyricItemArray[i] = iter->second;
    }

    return true;
}

void  LyricCtrl::ShowLyric(bool bShow)
{
    
}

void  LyricCtrl::OnProgressInd(int ms)
{
    if (NULL == m_pLyricItemArray ||
        0 == m_nLyricLineArraySize)
        return;

    CalcCurItemByTime(ms);
}

// 通过时间计算当前歌词行。
// 注：1. 有可能是直接定位当之前的进度，因此不能只往后面搜索
//     2. 有可能第一句歌词直接就是[00:02.68]，导致之前的时候对应不上index，这时默认为0
void  LyricCtrl::CalcCurItemByTime(int ms)
{
    int nOldCurItem = m_nCurItemIndex;

    int nTryForward = _CalcCurItemByTime(m_nCurItemIndex, ms, true);
    if (nTryForward < m_nCurItemIndex)
    {
        int nTryBack = _CalcCurItemByTime(m_nCurItemIndex, ms, false);
        if (nTryBack == m_nCurItemIndex)
            return;

        m_nCurItemIndex = nTryBack;
    }
    else
    {
        m_nCurItemIndex = nTryForward;
    }

    if (m_nCurItemIndex < 0)
        m_nCurItemIndex = 0;
    if (m_nCurItemIndex >= m_nLyricLineArraySize)
        m_nLyricLineArraySize = m_nLyricLineArraySize-1;

    if (m_nCurItemIndex == nOldCurItem)
        return;

    m_nVScroll = m_nCurItemIndex*m_nItemHeight - 50;
    m_pILyricCtrl->UpdateObject();
}
int  LyricCtrl::_CalcCurItemByTime(int nCur, int ms, bool bForward)
{
    if (bForward)
    {
        while (nCur < m_nLyricLineArraySize)
        {
            if (m_pLyricItemArray[nCur]->m_nTime > ms)
            {
                nCur--;
                break;
            }

            nCur++;
        }
    }
    else
    {
        while (nCur >= 0)
        {
            if (m_pLyricItemArray[nCur]->m_nTime < ms)
                break;

            nCur--;
        }
    }

    return nCur;
}

void  LyricCtrl::Start()
{
}
void  LyricCtrl::Pause()
{
}
void  LyricCtrl::Stop()
{
    SAFE_ARRAY_DELETE(m_pLyricItemArray);
    m_nVScroll = -50;
    m_nCurItemIndex = 0;
    m_nLyricLineArraySize = 0;
    m_pILyricCtrl->UpdateObject();
}