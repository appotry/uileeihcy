#include "stdafx.h"
#include "LEDExCtrl.h"

namespace UI
{

    LEDExCtrl::LEDExCtrl()
    {
        m_pILEDExCtrl = NULL;
        m_bSameWidth = true;
    }

    void  LEDExCtrl::ResetAttribute()
    {
        __super::ResetAttribute();
        m_pILEDExCtrl->ModifyStyle(OBJECT_STYLE_TRANSPARENT, 0, false);
    }
    void  LEDExCtrl::SetAttribute(IMapAttribute* pMapAttr, bool bReload)
    {
        __super::SetAttribute(pMapAttr, bReload);

        String strIndexMap;
        String strPosMap;

        m_bSameWidth = false;
        pMapAttr->GetAttr_bool(XML_LEDEXCTRL_SAMEWIDTH, true, &m_bSameWidth);
        const TCHAR* szText = pMapAttr->GetAttr(XML_LEDCTRL_INDEXMAP, true);
        if (szText)
            strIndexMap = szText;

        if (m_bSameWidth)
        {
            SetItemInfo3(szText);
        }
        else
        {
            szText = pMapAttr->GetAttr(XML_LEDEXCTRL_POSMAP, true);
            if (szText)
                strPosMap = szText;
            SetItemInfo2(strIndexMap.c_str(), strPosMap.c_str());
        }

        if (false == bReload)
        {
            szText = pMapAttr->GetAttr(XML_TEXT, true);
            if (szText)
                this->SetText(szText, false);
            else
                this->SetText(_T(""), false);
        }
    }

    void  LEDExCtrl::GetDesiredSize(SIZE* pSize)
    {
        pSize->cx = m_pILEDExCtrl->GetNonClientW();
        pSize->cy = m_pILEDExCtrl->GetNonClientH();

        IRenderBase* pForegndRender = m_pILEDExCtrl->GetForeRender();
        if (!pForegndRender)
            return;

        pSize->cy += pForegndRender->GetDesiredSize().cy;
        for (size_t i = 0; i < m_vecPos.size(); i++)
        {
            pSize->cx += m_vecPos[i].w;
        }
    }


    // 设置要显示的文本
    void LEDExCtrl::SetText(const TCHAR* szText, bool bUpdate)
    {
        m_vecPos.clear();
        m_strText.clear();
        if (szText)
            m_strText = szText;

        int nLength = m_strText.length();
        for (int i = 0; i < nLength; i++)
        {
            if (m_mapIndex.count(m_strText[i]))
            {
                m_vecPos.push_back(m_mapIndex[m_strText[i]]);
            }
            else
            {
                UI_LOG_WARN(_T("%s 无效的字符索引:%c"), FUNC_NAME, m_strText[i]);
            }
        }	

        if (bUpdate)
            m_pILEDExCtrl->UpdateObject();
    }

    void LEDExCtrl::OnPaint(IRenderTarget* pRenderTarget)
    {
        IRenderBase* pForegndRender = m_pILEDExCtrl->GetForeRender();
        if (!pForegndRender)
            return;

        IRenderBitmap* pBitmap = NULL;
        int nBitmapDrawType = DRAW_BITMAP_BITBLT;
        IImageRender* pImageRender = (IImageRender*)pForegndRender->QueryInterface(IID_UI_IImageRender);
        if (NULL == pImageRender)
        {
            IImageListRender* pImageListRender = (IImageListRender*)pForegndRender->QueryInterface(IID_UI_IImageListRender);
            if (pImageListRender)
            {
                pBitmap = pImageListRender->GetRenderBitmap();
                nBitmapDrawType = pImageListRender->GetImageDrawType();
            }
        }
        else
        {
            pBitmap = pImageRender->GetRenderBitmap();
            nBitmapDrawType = pImageRender->GetImageDrawType();
        }
        
        if (NULL == pBitmap)
            return;

        int nSize = (int)m_vecPos.size();
        CRect  rcClient;
        m_pILEDExCtrl->GetClientRectAsWin32(&rcClient);

        CRect  rcDest = rcClient;
        rcDest.right = rcDest.left;
        for (int i = 0; i < nSize; i++)
        {
            int nWidth = m_vecPos[i].w;
            rcDest.right = rcDest.left + nWidth;

            DRAWBITMAPPARAM  param;
            param.xDest = rcDest.left;
            param.yDest = rcDest.top;
            param.wDest = rcDest.Width();
            param.hDest = rcDest.Height();
            param.xSrc = m_vecPos[i].x;
            param.ySrc = 0;
            param.wSrc = m_vecPos[i].w;
            param.hSrc = pBitmap->GetHeight();
            param.nFlag = nBitmapDrawType;
            pRenderTarget->DrawBitmap(pBitmap, &param);

            rcDest.OffsetRect(nWidth, 0);
        }	
    }

    void  LEDExCtrl::SetItemInfo(LEDEX_ITEM_INFO* pInfoArray, int nCount)
    {
        if (NULL == pInfoArray)
            return;

        for (int i = 0; i < nCount; i++)
        {
            POSRANGE pos = {pInfoArray[i].nPos, pInfoArray[i].nWidth};
            m_mapIndex[pInfoArray[i].character] = pos;
        }
    }

    void  LEDExCtrl::SetItemInfo2(const TCHAR* szCharIndex, const TCHAR* szPosIndex)
    {
        if (NULL == szPosIndex || NULL == szCharIndex)
            return;
    
        int nCount = _tcslen(szCharIndex);
        LEDEX_ITEM_INFO*  pInfo = new LEDEX_ITEM_INFO[nCount];

        Util::ISplitStringEnum*  pEnum = NULL;
        bool bOk = false;
        do 
        {            
            int nCount2 = Util::SplitString(szPosIndex, XML_MULTI_SEPARATOR, &pEnum);
            if (nCount2 != nCount)
                break;

            int i = 0;
            for (; i < nCount; i++)
            {
                const TCHAR* szPosPair = pEnum->GetText(i);

                Util::ISplitStringEnum* pEnumPos = NULL;
                int nPairCount = Util::SplitString(szPosPair, XML_SEPARATOR, &pEnumPos);
                if (2 != nPairCount)
                {
                    SAFE_RELEASE(pEnumPos);
                    break;
                }

                int left = _ttoi(pEnumPos->GetText(0));
                int width = _ttoi(pEnumPos->GetText(1));
                SAFE_RELEASE(pEnumPos);

                pInfo[i].character = szCharIndex[i];
                pInfo[i].nPos = left;
                pInfo[i].nWidth = width;
            }

            if (i == nCount)
            {
                bOk = true;
            }
        } while (0);

        if (bOk)
        {
            SetItemInfo(pInfo, nCount);
        }
        else
        {
            UI_LOG_WARN(_T("%s Invalid Format, char:%s, pos:%s"), FUNC_NAME, szCharIndex, szPosIndex);
        }
        SAFE_RELEASE(pEnum);
        SAFE_ARRAY_DELETE(pInfo);
    }

    // 每一项大小相同时调用
    void  LEDExCtrl::SetItemInfo3(const TCHAR* szCharIndex)
    {
        if (!m_bSameWidth || NULL == szCharIndex)
            return;

        int nCount = _tcslen(szCharIndex);       
        if (0 == nCount)
            return;

        IRenderBase* pForegndRender = m_pILEDExCtrl->GetForeRender();
        if (!pForegndRender)
            return;

        int nItemWidth = 0;
        switch (pForegndRender->GetRenderType())
        {
        case RENDER_TYPE_IMAGE:
            {
                SIZE size = pForegndRender->GetDesiredSize();
                nItemWidth = size.cx / nCount;
            }
            break;

        case RENDER_TYPE_IMAGELIST:
            {
                nItemWidth = pForegndRender->GetDesiredSize().cx;
            }
            break;

        default:
            return;
        }

        
        LEDEX_ITEM_INFO*  pInfo = new LEDEX_ITEM_INFO[nCount];

        int nLeft = 0;
        for (int i = 0; i < nCount; i++)
        {
            pInfo[i].character = szCharIndex[i];
            pInfo[i].nPos = nLeft;
            pInfo[i].nWidth = nItemWidth;

            nLeft += nItemWidth;
        }
        SetItemInfo(pInfo, nCount);
        SAFE_ARRAY_DELETE(pInfo);
    }

}