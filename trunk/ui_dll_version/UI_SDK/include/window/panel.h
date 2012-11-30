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

	// ��������
public:
	Layout*      GetLayout();
	void         SetLayout(Layout*  pLayout);

	// �麯��
public:
	virtual      void        ResetAttribute();
	virtual      bool        SetAttribute(ATTRMAP& mapAttrib, bool bReload);
	virtual      SIZE        GetDesiredSize();

	// ��Ϣ����
protected:

protected:
	Layout*    m_pLayout;
	
};

}
