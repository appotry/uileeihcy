#pragma once
namespace UI
{


//   进度条
class UIAPI ProgressCtrlBase : public Control
{
public:
	ProgressCtrlBase();

public:
	// 从Object继承来的虚函数
	virtual      bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);

	// 从Control继承过来的虚函数
	virtual      SIZE  GetAutoSize();

	// 自己的虚函数
	virtual     int    SetPos(int nPos, bool bUpdate=true);
	virtual     int    SetPage(int nPage, bool bUpdate=true);
	virtual     void   SetRange(int nLower, int nUpper, bool bUpdate=true);
	virtual     void   SetScrollInfo(LPCSCROLLINFO lpsi, bool bUpdate=true);

public:
	void        SetLine(int nLine){ m_nLine = nLine; }
	int         GetPos(){ return m_nCur; }
	int         GetPage(){ return m_nPage; }
	int         GetLine(){ return m_nLine; }
	void        GetRange(int& nLower, int& nUpper){ nLower = m_nMin; nUpper = m_nMax; }
	int         GetRange() { return m_nMax - m_nMin; }

	void        SetDirectionType(PROGRESS_SCROLL_DIRECTION_TYPE eType) { m_eDirectionType = eType; }
	PROGRESS_SCROLL_DIRECTION_TYPE GetDirectionType() { return m_eDirectionType; }
	int         GetProgressStyle();
	void        SetProgressStyle(int n);

	// 属性
protected:

	int         m_nMin;        // 最小值
	int         m_nMax;        // 最大值
	int         m_nCur;        // 当前位置
	int         m_nLine;       // 每次按键时，滚动的距离
	int         m_nPage;       // 一个页面的大小

	PROGRESS_SCROLL_DIRECTION_TYPE   m_eDirectionType;       // 滚动条方向,类型
};

class UIAPI ProgressCtrl : public ProgressCtrlBase
{
public:
	UI_DECLARE_OBJECT( ProgressCtrl, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
	UI_END_MSG_MAP_CHAIN_PARENT(ProgressCtrlBase)
public:
	virtual  bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);
public:
	void     OnPaint(IRenderTarget* pDC);
private:


};

}