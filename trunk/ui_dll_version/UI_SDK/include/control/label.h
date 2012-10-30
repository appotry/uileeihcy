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
};


//
//	��̬ͼƬ�ؼ�(֧��ico��... )
//
class UIAPI Picture : public Control
{
public:
	Picture();
	~Picture();

	UI_DECLARE_OBJECT( Picture, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

	// virtual ����
	SIZE   GetAutoSize( HRDC hDC );
	void   OnPaint( HRDC hRDC );

private:
};

//
//	����ͼƬ
//
//	TODO: ����PNG���ж�������
//
class UIAPI GifPicture : public Control
{
public:
	GifPicture();
	~GifPicture();

	UI_DECLARE_OBJECT( GifPicture, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UIMSG_WM_PAINT(OnPaint)
	UI_END_MSG_MAP_CHAIN_PARENT(Control)

	// virtual ����
	SIZE   GetAutoSize( HRDC hDC );
	bool   SetAttribute( ATTRMAP& mapAttrib, bool bReload=false );

	void   OnPaint( HRDC hRDC );

private:
	GifImage*   m_pGifImage;
};
}