#pragma once
#include "UISDK\Control\Inc\Interface\isplitterbar.h"

namespace UI
{

class SplitterBar : public MessageProxy
{
public:
    SplitterBar();
    ~SplitterBar();

    UI_DECLARE_OBJECT3(SplitterBar, OBJ_PANEL|CONTROL_SPLITTERBAR, _T("UICtrl/Container"));

public:
    UI_BEGIN_MSG_MAP
        UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
        UIMSG_WM_LBUTTONUP(OnLButtonUp)
        UIMSG_WM_MOUSEMOVE(OnMouseMove)
        UIMSG_WM_CANCELMODE(OnCancelMode)
        UIMSG_WM_SIZE(OnSize)

        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_OBJECTLOADED(OnObjectLoaded)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(SplitterBar, IPanel);

    void  SetISplitterBar(ISplitterBar* p)
    {
        m_pISplitterBar = p;
        SetIMessageProxy(static_cast<IMessage*>(p));
    }
    ISplitterBar*  GetISplitterBar()
    {
        return m_pISplitterBar;
    }

public:
    void  SetDirection(SPLITTERBAR_DIRECTION eDir);
    void  SetAlign(SPLITTERBAR_ALIGN eAligh);
    bool  IsVert();
    bool  IsHorz();

protected:
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
    void  OnObjectLoaded();
    void  OnSize(UINT nType, int cx, int cy);

    void  OnLButtonDown(UINT nFlags, POINT point);
    void  OnLButtonUp(UINT nFlags, POINT point);
    void  OnCancelMode();
    void  OnMouseMove(UINT nFlags, POINT point);

    void  OnDraging(POINT ptInParent);
    bool  TestAvailableDragPos(POINT point, POINT* ptAvailable);
    void  UpdateLeftRightCtrlPos(int nPos);
    void  UpdateUpBottomCtrlPos(int nPos);
    void  OnParentSizeChanged();

    void  CalcAvailableRegion();

private:
    ISplitterBar*   m_pISplitterBar;
    
    SPLITTERBAR_DIRECTION  m_eDirection;   // 子对象排布的方向
    SPLITTERBAR_ALIGN      m_eAlign;       // 拖拽条对齐方向

    union
    {
        struct
        {
            IObject*  m_pObjLeft;                // 注：这两个对象必须配置在splitterbar后面，否则在splitterbar::onobjloaded中查找不到
            IObject*  m_pObjRight;

            int  m_nLeftMin;
            int  m_nLeftMax;
            int  m_nRightMin;
            int  m_nRightMax;
        };
        struct
        {
            IObject*  m_pObjTop;            
            IObject*  m_pObjBottom;

            int  m_nTopMin;
            int  m_nTopMax;
            int  m_nBottomMin;
            int  m_nBottomMax;
        };
    };

    bool   m_bButtonDown;
    int    m_nDeviation;      // 鼠标按下时的偏差
    POINT  m_ptLButtonDown;   // 窗口坐标
    POINT  m_ptLastMouseMove; // 窗口坐标

    CRect  m_rcAvailable;     // 在鼠标按下与弹起之间，该值有效
};

}