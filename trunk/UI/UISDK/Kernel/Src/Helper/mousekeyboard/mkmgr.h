#pragma once

// mouse keyboard manager
namespace UI
{
class Object;
class UIApplication;
class WindowBase;
// 
// class MKMgrBase 
// {
// public:
//     BOOL  ProcessMessage(UIMSG* pMsg);
// 
// private:
//     virtual BOOL  virtualProcessMessage(UIMSG* pMsg) = 0;
// 
// protected:
//     Object*   m_pObjHover;
//     Object*   m_pObjPress;
//     Object*   m_pObjFocus;
// 
//     UIApplication*  m_pUIApplication;
//     WindowBase*     m_pWindowBase;
// };
// 
// class WindowMKMgr : public MKMgrBase
// {
// 
// };
// 
// class UIEditorWindowMKMgr : public MKMgrBase
// {
// 
// };


    template <class T>
    void _SetHoverObject(Object* pNewHoverObj, T* pThis)
    {
        if (NULL == pThis->m_pUIApplication)
            return;

        if (pThis->m_pObjHover == pNewHoverObj)
            return;

        pThis->m_pUIApplication->HideToolTip();
        if (pNewHoverObj != pThis->m_pObjHover && NULL != pNewHoverObj)
        {
            TOOLTIPITEM  item;
            item.pNotifyObj = pNewHoverObj->GetIObject();
            pThis->m_pUIApplication->ShowToolTip(&item);
        }

        // 注意： 先改变所有对象的状态，再发送消息，避免在第一个对象响应消息的时候拿不到第二个对象的状态
        int nOldStateBits1 = 0;
        int nOldStateBits2 = 0;
        if (pThis->m_pObjHover && pThis->m_pObjHover != pNewHoverObj)
        {
            nOldStateBits1 = pThis->m_pObjHover->GetStateBit();
            pThis->m_pObjHover->SetHover(false);
        }
        if (pNewHoverObj)
        {
            nOldStateBits2 = pNewHoverObj->GetStateBit();
            pNewHoverObj->SetHover(true);
        }

        pThis->m_pObjHover = pNewHoverObj;
    }

    template <class T>
    void _SetPressObject(Object* pNewPressObj, T* pThis)
    {
        if (pThis->m_pObjPress == pNewPressObj)
            return;

        int nOldStateBits1 = 0;
        int nOldStateBits2 = 0;
        if (pThis->m_pObjPress && pThis->m_pObjPress != pNewPressObj)
        {
            nOldStateBits1 = pThis->m_pObjPress->GetStateBit();
            pThis->m_pObjPress->SetPress(false);
        }
        if (pNewPressObj)
        {
            nOldStateBits2 = pNewPressObj->GetStateBit();
            pNewPressObj->SetPress(true);
        }

        pThis->m_pObjPress = pNewPressObj;
        pThis->m_pUIApplication->HideToolTip();
    }

    template <class T>
    void _OnMouseMove(Object* pObj, WPARAM wParam, LPARAM lParam, T* pThis)
    {
        // 将所有的情况都列出来进行判断

        if (NULL == pThis->m_pObjPress)
        {
            if (NULL == pThis->m_pObjHover)
            {
                if (pObj == pThis->m_pObjHover)   // 鼠标还是在外面移动
                {
                }
                else                              // 鼠标移动到了pObj上
                {
                    pThis->SetHoverObject(pObj);
                    ::UISendMessage(pThis->m_pObjHover, WM_MOUSEMOVE, wParam, lParam);
                }
            }
            else
            {
                if (pObj == pThis->m_pObjHover)   // 鼠标在A上面移动
                {
                    ::UISendMessage(pThis->m_pObjHover, WM_MOUSEMOVE, wParam, lParam);
                }
                else                              // 鼠标刚才放在A上面，但现在又移出去了，可能移到B上面，也可能没有移动任何对象上
                {
                    ::UISendMessage(pThis->m_pObjHover, WM_MOUSELEAVE, (WPARAM) 0, 0);

                    // pThis->m_pObjHover = pObj;
                    pThis->SetHoverObject(pObj);
                    if (pObj)
                    {
                        ::UISendMessage(pObj, WM_MOUSEMOVE, wParam, lParam);
                    }
                }
            }
        }
        else
        {
            if (NULL == pThis->m_pObjHover)
            {
                if (pObj == pThis->m_pObjPress)  // 鼠标按在了A对象上，但鼠标在A外面。现在又移动回来了
                {
                    //m_pObjHover = m_pObjPress;
                    pThis->SetHoverObject(pThis->m_pObjPress);

                    ::UISendMessage(pThis->m_pObjPress, WM_MOUSEMOVE, wParam, lParam);

                }	
                else                    // 鼠标在A对象上按下了，但鼠标现在不再A上面
                {
                    ::UISendMessage(pThis->m_pObjPress, WM_MOUSEMOVE, wParam, lParam);
                }
            }
            else
            {
                if (pObj == pThis->m_pObjPress)  // 鼠标按在了A对象上，现在在A上移动
                {
                    UIASSERT(pThis->m_pObjPress == pThis->m_pObjHover);
                    UIASSERT(pThis->m_pObjHover == pObj);

                    ::UISendMessage(pThis->m_pObjPress, WM_MOUSEMOVE, wParam, lParam);
                }
                else  // 鼠标刚才按在了A对象上，但现在鼠标移出来了。
                {
                    UIASSERT(pThis->m_pObjPress==pThis->m_pObjHover);	

                    // m_pObjHover = NULL;
                    pThis->SetHoverObject(NULL);
                    ::UISendMessage(pThis->m_pObjPress, WM_MOUSEMOVE, wParam, lParam);
                }
            }
        }
    }



    // 注：当有一个HwndHost对象时，鼠标移到它上面时将触发WM_MOUSELEAVE消息，而不是MOUSEMOVE
    //     因此HwndHost对象还没法实现Tooltip;
    //
    template <class T>
    void _OnMouseLeave(T* pThis)
    {
        // 为了防止在对象在处理WM_LBUTTONUP消息时MouseManager的状态发生了改变,先保存状态
        Object*  pSaveObjPress = pThis->m_pObjPress;
        Object*  pSaveObjHover = pThis->m_pObjHover;

        if (pThis->m_pObjHover)
        {
            UIMSG msg;
            msg.pMsgTo = pThis->m_pObjHover->GetIMessage();
            msg.message = WM_MOUSELEAVE;
            msg.wParam  = 0;
            msg.lParam  = 0;
            ::UISendMessage(&msg);
        }

        if (pSaveObjPress && pSaveObjPress != pSaveObjHover)
        {
            ::UISendMessage(pSaveObjPress, WM_MOUSELEAVE);
        }

        pThis->SetHoverObject(NULL);
        pThis->SetPressObject(NULL);
    }

    template <class T>
    void _OnLButtonDown(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis)
    {
        if (pThis->m_pObjHover)
        {
            pThis->SetPressObject(pThis->m_pObjHover);

            UIMSG msg;
            msg.pMsgTo = pThis->m_pObjHover->GetIMessage();
            msg.message = WM_LBUTTONDOWN;
            msg.wParam  = wParam;
            msg.lParam  = lParam;
            ::UISendMessage(&msg, 0, pbHandled);      // 有可能导致m_pObjPress为NULL了

            if (pThis->m_pObjPress && pThis->m_pObjPress->CanTabstop())
            {
                pThis->SetFocusObject(pThis->m_pObjPress);
            }
        }
    }

    template <class T>
    void _OnLButtonDBClick(WPARAM wParam, LPARAM lParam, BOOL* pbHandled, T* pThis)
    {
        if (pThis->m_pObjPress)
        {
            UIMSG msg;
            msg.pMsgTo = pThis->m_pObjPress->GetIMessage();
            msg.message = WM_LBUTTONDBLCLK;
            msg.wParam  = wParam;
            msg.lParam  = lParam;

            ::UISendMessage(&msg, 0, pbHandled);
            if (pThis->m_pObjPress && pThis->m_pObjPress->IsTabstop())
            {
                pThis->SetFocusObject(pThis->m_pObjPress);
            }
        }
        else if (pThis->m_pObjHover)
        {
            pThis->SetPressObject(pThis->m_pObjHover);

            UIMSG msg;
            msg.pMsgTo = pThis->m_pObjHover->GetIMessage();
            msg.message = WM_LBUTTONDBLCLK;
            msg.wParam  = wParam;
            msg.lParam  = lParam;

            ::UISendMessage(&msg, 0, pbHandled);
            if (pThis->m_pObjPress && pThis->m_pObjPress->IsTabstop())
            {
                pThis->SetFocusObject(pThis->m_pObjPress);
            }
        }
    }


    template <class T>
    void _OnLButtonUp(WPARAM w, LPARAM l, T* pThis)
    {
        if (pThis->m_pObjPress)
        {
            int xPos = GET_X_LPARAM ( l );
            int yPos = GET_Y_LPARAM ( l );

            // 为了防止在对象在处理WM_LBUTTONUP消息时MouseManager的状态发生了改变,先保存状态
            Object*  pSaveObjPress = pThis->m_pObjPress;
            Object*  pSaveObjHover = pThis->m_pObjHover;

            if (pThis->m_pObjPress)
            {
                UIMSG  msg;
                msg.pMsgTo = pThis->m_pObjPress->GetIMessage();
                msg.message = WM_LBUTTONUP;
                msg.wParam  = w;
                msg.lParam  = l;

                ::UISendMessage(&msg);
                pThis->SetPressObject(NULL);
            }
        }
    }

}