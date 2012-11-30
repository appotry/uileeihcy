#pragma once
namespace UI
{


//   ������
class UIAPI ProgressCtrlBase : public Control
{
public:
	ProgressCtrlBase();

public:
	// ��Object�̳������麯��
	virtual      bool  SetAttribute(ATTRMAP& mapAttrib, bool bReload=false);

	// ��Control�̳й������麯��
	virtual      SIZE  GetAutoSize();

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
	int         GetRange() { return m_nMax - m_nMin; }

	void        SetDirectionType(PROGRESS_SCROLL_DIRECTION_TYPE eType) { m_eDirectionType = eType; }
	PROGRESS_SCROLL_DIRECTION_TYPE GetDirectionType() { return m_eDirectionType; }
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
	void     OnPaint(IRenderTarget* pDC);
private:


};

}