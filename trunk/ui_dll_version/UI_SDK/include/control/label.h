#pragma  once

namespace UI
{
/*
**    文字控件
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


	// 消息处理
protected:
	void        OnPaint( HRDC hDC );

	
public:
	// 从Object继承来的虚函数
	virtual      void        ResetAttribute();
	virtual      bool        SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	
	// 从Control继承过来的虚函数
	virtual      SIZE        GetAutoSize( HRDC hDC );  


public:
	String  GetText();
	void    SetText( const String& strText, bool bRedraw=true );

	// 属性
protected:
	String      m_strText;
//	UIColor*    m_pColor;
};


//
//	静态图片控件(支持ico、... )
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

	// virtual 函数
	SIZE   GetAutoSize( HRDC hDC );
	void   OnPaint( HRDC hRDC );

private:
};

//
//	动画图片
//
//	TODO: 增加PNG序列动画类型
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

	// virtual 函数
	SIZE   GetAutoSize( HRDC hDC );
	bool   SetAttribute( ATTRMAP& mapAttrib, bool bReload=false );

	void   OnPaint( HRDC hRDC );

private:
	GifImage*   m_pGifImage;
};
}