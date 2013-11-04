#pragma once

namespace UI
{

class Cell;

//
// ���񲼾����
//
class GridPanel : public Panel
{
public:
	GridPanel(void);
	~GridPanel(void);

	UI_DECLARE_OBJECT( GridPanel, OBJ_PANEL )

	UI_BEGIN_MSG_MAP
		UI_ON_WM_PAINT( OnPaint )
		UI_CHAIN_MSG_MAP(Panel)
	UI_END_MSG_MAP

	// ��������
public:
	bool             Initialize( );
	bool             AddRow();
	bool             AddCol();

	// ˽�з���
private:
	void             UpdateRect();

private:
	Cell*            m_pData;

	vector<int>      m_vWidths;
	vector<int>      m_vHeights;
	vector<String>   m_vWidthInXml;
	vector<String>   m_vHeightInXml;

	bool             m_bDrawBorder;           // �Ƿ���Ʊ߿�
	RECT             m_rcMargin;               // ���׼��
	

	// ��Ϣ����
protected:
	void             OnPaint( HDC hDC );
};


// grid �ĵ�Ԫ����
class Cell
{
public:
	Cell();

public:
	int       m_nRow;         // ��Ԫ����к�
	int       m_nCol;         // ��Ԫ����к�
	bool      m_bCombined;    // �õ�Ԫ���Ƿ񱻺ϲ���

	Object*   m_pObject;      // �õ�Ԫ�������
	Cell*     m_pObjCombined; // ����õ�Ԫ���Ǳ��ϲ��ģ���ô���ĸ���Ԫ��ϲ����Լ�


	// �Ա����б���
	Cell*     m_pLeft;        
	Cell*     m_pRight;
	Cell*     m_pTop;
	Cell*     m_pBottom;
};

}
