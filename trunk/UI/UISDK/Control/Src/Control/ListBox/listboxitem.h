#pragma once 
#include "UISDK\Control\Inc\Interface\ilistbox.h"

namespace UI
{
    class ListBoxItem : public MessageProxy
    {
    public:
        ListBoxItem(){ m_pIListBoxItem = NULL; };

        UI_BEGIN_MSG_MAP
            UIMSG_WM_PAINT(OnPaint)
            UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
            UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
            UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ListBoxItem, IListItemBase)

        void  SetIListBoxItem(IListBoxItem* p) { m_pIListBoxItem = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

    public:
        void  GetDesiredSize(SIZE* pSize);
        void  OnLButtonDown(UINT nFlags, POINT point);
        void  OnLButtonUp(UINT nFlags, POINT point);
        void  OnPaint(IRenderTarget* pRenderTarget);

    protected:
        IListBoxItem*  m_pIListBoxItem;
    };
}