#ifndef IWINDOWMOUSEMGR_H_41B30165_0499_4926_B519_26BF68817522
#define IWINDOWMOUSEMGR_H_41B30165_0499_4926_B519_26BF68817522

namespace UI
{
    class Object;
    interface IMKMgr : public IRootInterface
    {
        virtual void  OnObjectHideInd(Object* pObj) = 0;
        virtual void  OnObjectDeleteInd(Object* pObj) = 0;
    };
// 
// class MouseMgrBase;
// interface UISDKAPI IMouseMgrBase
// {
//     IMouseMgrBase();
//     ~IMouseMgrBase();
// 
//     IObject*  GetHoverObject();
//     IObject*  GetPressObject();
// 
//     void  OnObjectHideInd(IObject* pObj);
//     void  OnObjectDeleteInd(IObject* pObj);
// 
//     IObject*  GetObjectByPos(IObject* pObjParent, POINT* pt, bool bSkinBuilderInvoke=false);
// 
//     void  SetHoverObject(IObject* pNewHoverObj);
//     void  SetPressObject(IObject* pNewPressObj);
//     void  SetHoverObjectDirectNULL();
//     void  SetPressObjectDirectNULL();
// 
//     void  OnMouseMoveBase(IObject* pNewHover, WPARAM wParam, LPARAM lParam);
//     void  OnMouseLeaveBase();
//     void  OnLButtonDownBase(WPARAM wParam, LPARAM lParam, BOOL* pbHandled);
//     void  OnLButtonDBClickBase(WPARAM wParam, LPARAM lParam, BOOL* pbHandled);
// 
//     void  SetUIApplication(IUIApplication* p, IWindowBase* pWindow);
// protected:
//     MouseMgrBase*  m_pMouseMgrBase;
// };
// 
// class KeyboardMgrBase;
// interface UISDKAPI IKeyboardMgrBase
// {
//     IKeyboardMgrBase();
//     ~IKeyboardMgrBase();
// 
// //     void  Tab_2_NextControl();
// //     void  Tab_2_PrevControl();
// // 
//     void  SetFocusObject(IObject*  pObj);
//     IObject*  GetFocusObject();
// 
// protected:
//     KeyboardMgrBase*  m_pKeyboardMgrBase;
// };

}

#endif // IWINDOWMOUSEMGR_H_41B30165_0499_4926_B519_26BF68817522