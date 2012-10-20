#pragma once
namespace UI
{
	enum PROGRESS_SCROLL_DIRECTION_TYPE
	{
		PROGRESS_SCROLL_LEFT_2_RIGHT,
		PROGRESS_SCROLL_BOTTOM_2_TOP,
		PROGRESS_SCROLL_RIGHT_2_LEFT,
		PROGRESS_SCROLL_TOP_2_BOTTOM
	};

//   ������
class UIAPI ProgressCtrlBase : public Control
{
public:
	ProgressCtrlBase();

public:
	// ��Object�̳������麯��
	virtual      bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);

	// ��Control�̳й������麯��
	virtual      SIZE  GetAutoSize(HRDC hRDC);

	// �Լ����麯��
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
	int         GetRange() { return m_nMax - m_nMin - m_nPage; }

	void        SetDirectionType(PROGRESS_SCROLL_DIRECTION_TYPE eType) { m_eDirectionType = eType; }
	int         GetProgressStyle();
	void        SetProgressStyle(int n);

	// ����
protected:

	int         m_nMin;        // ��Сֵ
	int         m_nMax;        // ���ֵ
	int         m_nCur;        // ��ǰλ��
	int         m_nLine;       // ÿ�ΰ���ʱ�������ľ���
	int         m_nPage;       // һ��ҳ��Ĵ�С

	PROGRESS_SCROLL_DIRECTION_TYPE   m_eDirectionType;       // ����������,����
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
	void     OnPaint(IRenderDC* pDC);
private:
};

}