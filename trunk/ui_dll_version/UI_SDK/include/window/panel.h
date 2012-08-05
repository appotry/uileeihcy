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

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP( Object )
	UI_END_MSG_MAP

	// ��������
public:
	Layout*      GetLayout();
	void         SetLayout(Layout*  pLayout);

	// �麯��
public:
	virtual      void        ResetAttribute();
	virtual      bool        SetAttribute( map<String,String>& mapAttrib, bool bReload );
	virtual      SIZE        GetDesiredSize( HRDC hRDC );

	// ��Ϣ����
protected:

protected:
	Layout*    m_pLayout;
	
};

}
