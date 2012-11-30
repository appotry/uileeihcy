#pragma once

namespace UI
{

class UIAPI Panel :	public Object
{
public:
	Panel();
	~Panel();
protected:
	virtual void  DestroyUI();

public:
	UI_DECLARE_OBJECT( Panel, OBJ_PANEL )

	// 公共方法
public:
	Layout*      GetLayout();
	void         SetLayout(Layout*  pLayout);

	// 虚函数
public:
	virtual      void        ResetAttribute();
	virtual      bool        SetAttribute(ATTRMAP& mapAttrib, bool bReload);
	virtual      SIZE        GetDesiredSize();

	// 消息处理
protected:

protected:
	Layout*    m_pLayout;
	
};

}
