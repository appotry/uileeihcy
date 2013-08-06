#pragma once
#include "UISDK\Control\Inc\Interface\iprogressctrl.h"

namespace UI
{

//   进度条
class ProgressCtrl : public MessageProxy
{
public:
	ProgressCtrl();

    UI_DECLARE_OBJECT3(ProgressCtrl, OBJ_CONTROL|CONTROL_PROGRESSCTRL, _T("UICtrl/Control"))
    
    UI_BEGIN_MSG_MAP
        UIMSG_WM_PAINT(OnPaint)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_GETDESIREDSIZE(GetDesiredSize)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
        UIMSG_WM_SETATTRIBUTE(SetAttribute)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
    UI_END_MSG_MAP_CHAIN_PARENT_Ixxx(ProgressCtrl, IControl)

protected:
    HRESULT  FinalConstruct(IUIApplication* );
    void  OnPaint(IRenderTarget* pDC);
    void  ResetAttribute();
    void  SetAttribute(IMapAttribute* pMapAttr, bool bReload);
    void  GetDesiredSize(SIZE* pSize);

public:
    void  SetIProgressCtrl(IProgressCtrl* p) { m_pIProgressCtrl = p; SetIMessageProxy(static_cast<IMessage*>(p)); }

	int   SetPos(int nPos, bool bUpdate=true);
	int   SetPage(int nPage, bool bUpdate=true);
	void  SetRange(int nLower, int nUpper, bool bUpdate=true);
	void  SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

	void  SetLine(int nLine){ m_nLine = nLine; }
	int   GetPos(){ return m_nCur; }
	int   GetPage(){ return m_nPage; }
	int   GetLine(){ return m_nLine; }
	void  GetRange(int& nLower, int& nUpper){ nLower = m_nMin; nUpper = m_nMax; }
	int   GetRange() { return m_nMax - m_nMin; }

	void  SetDirectionType(PROGRESS_SCROLL_DIRECTION_TYPE eType) { m_eDirectionType = eType; }
	PROGRESS_SCROLL_DIRECTION_TYPE  GetDirectionType() { return m_eDirectionType; }

	int   GetProgressStyle();
	void  SetProgressStyle(int n);

	// 属性
protected:
    IProgressCtrl*  m_pIProgressCtrl;

	int  m_nMin;        // 最小值
	int  m_nMax;        // 最大值
	int  m_nCur;        // 当前位置
	int  m_nLine;       // 每次按键时，滚动的距离
	int  m_nPage;       // 一个页面的大小
	PROGRESS_SCROLL_DIRECTION_TYPE   m_eDirectionType;       // 滚动条方向,类型
};


}