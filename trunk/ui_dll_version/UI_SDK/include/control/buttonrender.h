#pragma once
#include "uxtheme.h"
#include "Vsstyle.h"
#pragma comment(lib, "uxtheme.lib")

//
//	libo 20110817 (hangzhou xingyao jiari jiudian 8332)
//	
//	Ϊ��ʵ����һ��ť�������ɸ�����ʽ��ť��չ�֣�ͬʱ���ܽ���ť���߼��ͻ��Ʒֿ���
//	��˴�_Button�������һ��ButtonBkRender��ָ�룬���ݲ�ͬ��������ʵ�ֲ�ͬ��չ��
//
//	����render��һ�飬��Ϊbk��icontext��Ȼ��icontext��Ϊicon��text����ֿ���ÿһ��
//	����Ԫ�صĶ�����
//
namespace UI
{
class ButtonBase;
class Edit;
class GroupBox;


class _ButtonIconRender : public Message
{
public:
	_ButtonIconRender( ButtonBase* pButton );
	virtual ~_ButtonIconRender();

	UI_BEGIN_MSG_MAP
	UI_END_MSG_MAP

public:

	virtual  void  ResetAttribute();
	virtual  bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	virtual  SIZE  GetAutoSize( HRDC hRDC );

	virtual void DrawDisable( HRDC hRDC, RECT* prc ){};
	virtual void DrawNormal( HRDC hRDC, RECT* prc ){};
	virtual void DrawHover( HRDC hRDC, RECT* prc ){};
	virtual void DrawPress( HRDC hRDC, RECT* prc ){};

protected:
	HRBITMAP    m_hIcon;    
	ButtonBase*    m_pButton;
};

//
//	��̬һ��ͼ��Ļ���
//
class ButtonIconStaticRender : public _ButtonIconRender
{
public:
	ButtonIconStaticRender( ButtonBase* pButton );
	~ButtonIconStaticRender();

	void DrawDisable( HRDC hRDC, RECT* prc );
	void DrawNormal( HRDC hRDC, RECT* prc );
	void DrawHover( HRDC hRDC, RECT* prc );
	void DrawPress( HRDC hRDC, RECT* prc );

};

//
//	radio/check��ť��8̬ͼ�����
//
//  Ĭ��8̬ͼƬ��˳��Ϊ:
//		normal      hover    press    disable
//		check_n     check_h  check_p  chech_d
//
class ButtonIcon8StateRender : public _ButtonIconRender
{
public:
	ButtonIcon8StateRender( ButtonBase* pButton );
	~ButtonIcon8StateRender();

	virtual  bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	virtual  SIZE  GetAutoSize( HRDC hRDC );

	void DrawDisable( HRDC hRDC, RECT* prc );
	void DrawNormal( HRDC hRDC, RECT* prc );
	void DrawHover( HRDC hRDC, RECT* prc );
	void DrawPress( HRDC hRDC, RECT* prc );

protected:
	void draw_item( HRDC hRDC, CRect* prc, int xIcon, int yIcon );

private:
	int  m_nIconItemWidth;   // 8��״̬�е�һ��״̬�ĸߺͿ�
	int  m_nIconItemHeight;
};

//
//	������ʽ�ĵ�ѡͼ��
//
class ButtonIconThemeRadioRender : public _ButtonIconRender
{
public:
	ButtonIconThemeRadioRender( ButtonBase* pButton );
	~ButtonIconThemeRadioRender();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(_ButtonIconRender)
	UI_END_MSG_MAP

	void DrawDisable( HRDC hRDC, RECT* prc );
	void DrawNormal( HRDC hRDC, RECT* prc );
	void DrawHover( HRDC hRDC, RECT* prc );
	void DrawPress( HRDC hRDC, RECT* prc );

	void     OnThemeChanged();
	virtual  SIZE  GetAutoSize( HRDC hRDC );

private:
	HTHEME    m_hTheme;
};
//
//	������ʽ�ĸ�ѡͼ��
//
class ButtonIconThemeCheckRender : public _ButtonIconRender
{
public:
	ButtonIconThemeCheckRender( ButtonBase* pButton );
	~ButtonIconThemeCheckRender();

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(_ButtonIconRender)
	UI_END_MSG_MAP

	void DrawDisable( HRDC hRDC, RECT* prc );
	void DrawNormal( HRDC hRDC, RECT* prc );
	void DrawHover( HRDC hRDC, RECT* prc );
	void DrawPress( HRDC hRDC, RECT* prc );

	void     OnThemeChanged();
	virtual  SIZE  GetAutoSize( HRDC hRDC );

private:
	HTHEME    m_hTheme;
};

class _ButtonTextRender
{
public:
	_ButtonTextRender( ButtonBase* pButton );
	~_ButtonTextRender();

	virtual  bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	virtual  SIZE  GetAutoSize( HRDC hRDC );

	virtual void DrawDisable( HRDC hRDC, RECT* prc ){};
	virtual void DrawNormal( HRDC hRDC, RECT* prc ){};
	virtual void DrawHover( HRDC hRDC, RECT* prc ){};
	virtual void DrawPress( HRDC hRDC, RECT* prc ){};

protected:
	ButtonBase*   m_pButton;

	// ������ɫ
	COLORREF   m_colNormal;
	COLORREF   m_colHover;
	COLORREF   m_colPress;
	COLORREF   m_colDisable;

	// �ı�����
	String     m_text;
};

class ButtonTextSinglelineRender : public _ButtonTextRender
{
public:
	ButtonTextSinglelineRender( ButtonBase*  pButton );
	~ButtonTextSinglelineRender();

public:
	virtual void DrawDisable( HRDC hRDC, RECT* prc );
	virtual void DrawNormal( HRDC hRDC, RECT* prc );
	virtual void DrawHover( HRDC hRDC, RECT* prc );
	virtual void DrawPress( HRDC hRDC, RECT* prc );
};

class ButtonTextHyperLinkRender : public ButtonTextSinglelineRender
{
public:
	ButtonTextHyperLinkRender( ButtonBase*  pButton );
	~ButtonTextHyperLinkRender();

public:
	virtual void DrawDisable( HRDC hRDC, RECT* prc );
	virtual void DrawNormal( HRDC hRDC, RECT* prc );
	virtual void DrawHover( HRDC hRDC, RECT* prc );
	virtual void DrawPress( HRDC hRDC, RECT* prc );

protected:
//	HFONT   CreateFontUnderLine( HRDC );

};

#define BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_LEFT    0x00     // ��ťλ���������
#define BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_TOP     0x01     // ��ťλ�������ϲ�
#define BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_RIGHT   0x02     // ��ťλ�������Ҳ�
#define BUTTON_ICONTEXT_RENDER_FLAG_ICON_POS_BOTTOM  0x04     // ��ťλ�������²�

class ButtonIconTextRender : public Message
{
public:
	ButtonIconTextRender( ButtonBase* pButton );
	~ButtonIconTextRender();

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP_POINT_MEMBER(m_pIconRender)
	UI_END_MSG_MAP

public:
	
	void ResetAttribute();
	bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );
	void DrawFocus( HRDC hRDC, RECT* prcIcon, RECT* prcText );

	SIZE GetAutoSize(HRDC hRDC);

	void SetTextRenderType( BUTTON_TEXT_RENDER_TYPE eType );

private:
	void analyzeIconTextRect( HRDC hRDC, RECT& rcIcon, RECT& rcText);

public:
	_ButtonIconRender*   m_pIconRender;
	_ButtonTextRender*   m_pTextRender;

protected:
	ButtonBase*  m_pButton;

	int     m_nFlag;             // ͼ�����ı���λ�ù�ϵ
	int     m_nIconMarginText;   // ͼ������ı��Ĵ�С
	int     m_nDrawFocusType;    // ��λ��ƽ�����ʽ 
};

//
//	������Ⱦ����
//
//	Remark 
//		��Message�̳У���Ϊ�˴���Theme�仯ʱ�����ո���Ϣ
//
class BackRenderBase : public Message
{
public:
	BackRenderBase( Object* m_pObject );

	UI_BEGIN_MSG_MAP
	UI_END_MSG_MAP

public:
	static BackRenderBase*  CreateInstance(const String& strType, Object* pObj); 

	void ResetAttribute(){};
	virtual bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false ){return true;};

	virtual void DrawDisable( HRDC hRDC  )=0;
	virtual void DrawNormal( HRDC hRDC )=0;
	virtual void DrawHover( HRDC hRDC )=0;
	virtual void DrawPress( HRDC hRDC )=0;

	virtual SIZE GetAutoSize( HRDC hRDC ){ SIZE s={0,0}; return s; };  // ��ť��ť������С��Ŀǰֻ����ͼƬ��������

protected:
	Object*   m_pObject;
};

#if 0
//
//	ֱ��ʹ����ɫֵ��Ϊ����
//
//	Q: ���ʵ�ֲ���Ҫ������״̬�����繤������ť������״̬����û�б�����?
//	A: ��״̬λ��ʵ�֣�������ĳ��״̬����ɫֵ���ͱ�ʾ��״̬��Ҫ���Ʊ���������Ͳ�����
//
class ColorBackRender : public BackRenderBase
{
public:
	ColorBackRender( Object* pObj );

	bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	
private:
	void drawbk( HRDC hRDC, COLORREF colbrush, COLORREF colpen );

private:
	COLORREF   m_colNormalPen;
	COLORREF   m_colNormalBrush;

	COLORREF   m_colHoverPen;
	COLORREF   m_colHoverBrush;

	COLORREF   m_colPressPen;
	COLORREF   m_colPressBrush;

	COLORREF   m_colDisablePen;
	COLORREF   m_colDisableBrush;

	int        m_nFlag;
};

//
//	ͼƬ������ť
//
class ImageListRender : public BackRenderBase
{
public:
	ImageListRender( Object* pObj );
	~ImageListRender( );

	bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	

	virtual SIZE GetAutoSize( HRDC hRDC );

private:
	HRBITMAP   m_hBitmap;

	int        m_nImageDirection;
	int        m_nItemWidth;          // ͼƬ��Ŀ��
	int        m_nItemHeight;         // ͼƬ��ĸ߶�
};

//
//	���ⱳ����ť
//
class ButtonBkThemeRender : public BackRenderBase
{
public:
	ButtonBkThemeRender( ButtonBase* pButton );
	~ButtonBkThemeRender( );

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(BackRenderBase)
	UI_END_MSG_MAP
public:

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	

	void OnThemeChanged();
private:
	HTHEME    m_hTheme;
};


//
//	���ⱳ����ť
//
class EditBkThemeRender : public BackRenderBase
{
public:
	EditBkThemeRender( Edit* pEdit );
	~EditBkThemeRender( );

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(BackRenderBase)
	UI_END_MSG_MAP
public:

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	

	void OnThemeChanged();
private:
	HTHEME    m_hTheme;
};

class GroupBoxBkThemeRender : public BackRenderBase
{
public:
	GroupBoxBkThemeRender( GroupBox* pGroupBox );
	~GroupBoxBkThemeRender( );

	UI_BEGIN_MSG_MAP
		UIMSG_WM_THEMECHANGED( OnThemeChanged )
		UICHAIN_MSG_MAP(BackRenderBase)
	UI_END_MSG_MAP
public:

	void DrawDisable( HRDC hRDC  );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );	

	void OnThemeChanged();
private:
	HTHEME    m_hTheme;
};

#endif
//
//	��ť����Ⱦ�࣬��������Ԫ�ص���Ⱦ��������ͼ�ꡢ���֡���_Button����
//
class ButtonRender : public Message
{
public:
	ButtonRender( );
	~ButtonRender();

	UI_BEGIN_MSG_MAP
		UICHAIN_MSG_MAP_POINT_MEMBER(m_pBkRender)
		UICHAIN_MSG_MAP_POINT_MEMBER(m_pIconTextRender)
	UI_END_MSG_MAP

public:

	void Draw(HRDC hRDC);   // �ַ����ֻ���״̬

	void DrawDisable( HRDC hRDC );
	void DrawNormal( HRDC hRDC );
	void DrawHover( HRDC hRDC );
	void DrawPress( HRDC hRDC );

	void ResetAttribute();
	bool SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
	void SetButtonPtr( ButtonBase* ptr ) { m_pButton = ptr; }
	SIZE GetAutoSize(HRDC hRDC);

	void SetTextRenderType( BUTTON_TEXT_RENDER_TYPE eType );
	_ButtonTextRender*    GetTextRender();
	_ButtonIconRender*    GetIconRender();
	ButtonIconTextRender* GetIconTextRender();

private:
	ButtonBase*            m_pButton;              // Ϊ�˷������ð�ť��һЩ����
	BackRenderBase*        m_pBkRender;            // ������Ⱦ��
	ButtonIconTextRender*  m_pIconTextRender;

	int                    m_nAutoSizeType;        // ��ť�Զ�������С����
};

}

