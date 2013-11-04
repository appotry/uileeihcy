#include "stdafx.h"
#include "UISDK\Kernel\Inc\Util\rendercontext.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"
#include "UISDK\Kernel\Inc\Interface\ilistitembase.h"

namespace UI
{

// pInitRectһ����ȡ window����� &m_rcParent.�����ֻˢ��һ���ؼ�����ֱ��ָΪNULL�����ˣ���begindraw��
// ��ָ���ö���ļ�������
RenderContext::RenderContext(RECT* prcRenderRegion, bool bClip)
{
	m_ptOffset.x = 0;
	m_ptOffset.y = 0;

    if (prcRenderRegion)
    {
        ::CopyRect(&m_rcDrawRegion, prcRenderRegion);
    }
	else
	{
		m_rcDrawRegion.SetRectEmpty();
	}
    m_bUpdateClip = bClip;
}
void RenderContext::Reset(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return;

	pRenderTarget->SelectClipRgn(NULL);
	pRenderTarget->SetViewportOrgEx(0,0,NULL);

    m_ptOffset.x = m_ptOffset.y = 0;
    ::SetRectEmpty(&m_rcDrawRegion);
}
void RenderContext::Update(IRenderTarget* pRenderTarget)
{
	if (NULL == pRenderTarget)
		return;

	pRenderTarget->SetViewportOrgEx(m_ptOffset.x, m_ptOffset.y, NULL);

	if (m_bUpdateClip)
	{
		HRGN hRgn = CreateRectRgnIndirect(&m_rcDrawRegion);
		pRenderTarget->SelectClipRgn(hRgn);
		SAFE_DELETE_GDIOBJECT(hRgn);
	}
}

void RenderContext::DrawClient(IObject* pObject)
{
    if (NULL == pObject)
        return;

    CRegion4 r;
    pObject->GetNonClientRegion(&r);

    m_ptOffset.x += r.left;
    m_ptOffset.y += r.top;

    if (m_bUpdateClip)
    {
        m_rcDrawRegion.DeflateRect(r.left, r.top, r.right, r.bottom);
    }
}

void RenderContext::Scroll(IObject* pObjScroll)
{
	if (NULL == pObjScroll)
		return;
	
	int xOffset = 0, yOffset = 0;
	if (pObjScroll->GetScrollOffset(&xOffset, &yOffset))
	{
		m_ptOffset.x -= xOffset;
		m_ptOffset.y -= yOffset;
	}
}


//////////////////////////////////////////////////////////////////////////
// ����falseʱ����ʾ�����������ȫ������ǰ���������ˣ�����Ҫ�ٻ���
bool RenderContext::DrawChild(IObject* pChild)
{
    if (NULL == pChild)
        return false;

    CRect rcParent;
    pChild->GetParentRect(&rcParent);

    if (m_bUpdateClip)
    { 
        CRect rcIntersect;
        CRect rcInWindow(&rcParent);
        rcInWindow.OffsetRect(m_ptOffset.x, m_ptOffset.y);

        if (!rcIntersect.IntersectRect(&m_rcDrawRegion, &rcInWindow))
        {
            m_rcDrawRegion.SetRectEmpty();
            return false;
        }

        m_rcDrawRegion = rcIntersect;
    }

    m_ptOffset.x += rcParent.left;
    m_ptOffset.y += rcParent.top;

    return true;
}

bool  RenderContext::DrawListItem(IListItemBase* pItem)
{
	if (NULL == pItem)
		return false;

	CRect rcParent;
	pItem->GetParentRect(&rcParent);

	if (m_bUpdateClip)
	{ 
		CRect rcIntersect;
		CRect rcInWindow(&rcParent);
		rcInWindow.OffsetRect(m_ptOffset.x, m_ptOffset.y);

		if (!rcIntersect.IntersectRect(&m_rcDrawRegion, &rcInWindow))
		{
			m_rcDrawRegion.SetRectEmpty();
			return false;
		}

		m_rcDrawRegion = rcIntersect;
	}

	m_ptOffset.x += rcParent.left;
	m_ptOffset.y += rcParent.top;

	return true;
}


}