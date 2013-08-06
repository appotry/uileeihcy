#pragma once
#include "UISDK\Control\Inc\Interface\iprogressctrl.h"

namespace UI
{

//   ������
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

	// ����
protected:
    IProgressCtrl*  m_pIProgressCtrl;

	int  m_nMin;        // ��Сֵ
	int  m_nMax;        // ���ֵ
	int  m_nCur;        // ��ǰλ��
	int  m_nLine;       // ÿ�ΰ���ʱ�������ľ���
	int  m_nPage;       // һ��ҳ��Ĵ�С
	PROGRESS_SCROLL_DIRECTION_TYPE   m_eDirectionType;       // ����������,����
};


}