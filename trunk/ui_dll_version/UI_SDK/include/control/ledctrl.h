#pragma  once

//
//  默认每一项的宽度、高度都一样
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
	ImageListRender*  m_pImagelist;  // 该处直接借用m_foregndRender。因此要配置文件中应该将foregnd render配置为image list 列表
	map<TCHAR,int>    m_mapIndex;    // 字符串到图片索引的映射
	
	vector<int>       m_vecIndex;    // 当前要显示的索引序列
	String            m_strText; 
};


class UIAPI LEDCtrl : public LEDCtrlBase
{
public:

	UI_DECLARE_OBJECT(LEDCtrl, OBJ_CONTROL)
};
