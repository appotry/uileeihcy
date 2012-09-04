#pragma  once

namespace UI
{
/*
**    ���ֿؼ�
*/
class UIAPI Label : public Control
{
public:
	Label();
	~Label();

	UI_DECLARE_OBJECT( Label, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)


	// ��Ϣ����
protected:
	void        OnPaint( HRDC hDC );

	
public:
	// ��Object�̳������麯��
	virtual      void        ResetAttribute();
	virtual      bool        SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	
	// ��Control�̳й������麯��
	virtual      SIZE        GetAutoSize( HRDC hDC );  


public:
	String  GetText();
	void    SetText( const String& strText, bool bRedraw=true );

	// ����
protected:
	String      m_strText;
//	UIColor*    m_pColor;
	int         m_eTextAlign;   /*LABEL_ALIGN*/
};


//
//	��̬ͼƬ�ؼ�(֧��ico��... )
//
class UIAPI PictureCtrl : public Control
{
public:
	PictureCtrl();
	~PictureCtrl();

	UI_DECLARE_OBJECT( PictureCtrl, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

	// virtual ����
	SIZE   GetAutoSize( HRDC hDC );
	bool   SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	void   OnPaint( HRDC hRDC );

private:
//	HRBITMAP   m_hBitmap;
};
}