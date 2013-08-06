#ifndef MESSAAGEDEFINE_H_B52AA12F_B7E5_42a0_B221_96414D9503F0
#define MESSAAGEDEFINE_H_B52AA12F_B7E5_42a0_B221_96414D9503F0

namespace UI
{
interface IListItemBase;

// void OnTrbnPosChanged( int nPos, int nScrollType );
#define UIMSG_TRBN_POSCHANGED_ID(OBJID, func)         \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_TRBN_POSCHANGED &&                 \
        NULL != pMsgFrom &&                           \
        0 == _tcscmp(((IObject*)pMsgFrom)->GetID(), OBJID)) \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func ((int)wParam, (int)lParam);              \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnTrbnPosChanged( int nPos, int nScrollType );
#define UIMSG_TRBN_POSCHANGED( p, func )              \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_TRBN_POSCHANGED &&                 \
        NULL != pMsgFrom &&                           \
        (pMsgFrom) == (UI::IMessage*)p)               \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func( (int)wParam, (int)lParam );             \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }


// void OnCbnSelChanged(Message* pMsgFrom, ListItemBase* pOldSelItem, ListItemBase* pSelItem)
#define UIMSG_CBN_SELCHANGED(func)                    \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_CBN_SELCHANGED )                   \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func(pMsgFrom, (UI::IListItemBase*)wParam, (UI::IListItemBase*)lParam); \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnMenuClick(UINT nMenuID);
#define UIMSG_MENU_CLICK(func)                        \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_MENU_CLICK )                       \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((UINT)wParam);                           \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// BOOL OnHeaderCtrlTracking(int nColumnIndex, int* pNewWidth);
#define UIMSG_HDCN_TRACKING(func)                     \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_HDCN_TRACKING )                    \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = func((int)wParam, (int*)lParam);    \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// void OnHeaderCtrlTotalWidthChanged(int nTotalWidth);
#define UIMSG_HDCN_TOTAL_WIDTH_CHANGED(func)          \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_HDCN_TOTAL_WIDTH_CHANGED)          \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        func((int)wParam);                            \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }

// LRESULT OnHeaderCtrlBeginTrack(int nColumnIndex);
#define UIMSG_HDCN_BEGIN_TRACK(func)                  \
    if (uMsg == UI_WM_NOTIFY  &&                      \
        code == UI_HDCN_BEGIN_TRACK)                  \
    {                                                 \
        SetMsgHandled(TRUE);                          \
        lResult = func((int)wParam);                  \
        if (IsMsgHandled())                           \
            return TRUE;                              \
    }


}

#endif // MESSAAGEDEFINE_H_B52AA12F_B7E5_42a0_B221_96414D9503F0