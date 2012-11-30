#pragma  once

//
//  Ĭ��ÿһ��Ŀ�ȡ��߶ȶ�һ��
//

class UIAPI LEDCtrlBase : public Control
{
public:
	LEDCtrlBase();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)	
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

public:
	void     OnPaint(HRDC);

	virtual  void  ResetAttribute();
	virtual  bool  SetAttribute( ATTRMAP& mapAttrib, bool bReload=false );
	virtual  SIZE  GetAutoSize();

public:
	void     SetIndexMap(const String& str);
	void     SetText(const String& str, bool bUpdate=true);

protected:
	ImageListRender*  m_pImagelist;  // �ô�ֱ�ӽ���m_foregndRender�����Ҫ�����ļ���Ӧ�ý�foregnd render����Ϊimage list �б�
	map<TCHAR,int>    m_mapIndex;    // �ַ�����ͼƬ������ӳ��
	
	vector<int>       m_vecIndex;    // ��ǰҪ��ʾ����������
	String            m_strText; 
};


class UIAPI LEDCtrl : public LEDCtrlBase
{
public:

	UI_DECLARE_OBJECT(LEDCtrl, OBJ_CONTROL)
};
