#pragma  once

namespace UI
{
/*
**	�û�ˢ�����Ʊ����İ�ť���ɴ���ͼ������֡�
**  ģ����VS2005��������ť��OFFICE2003�޽��乤������ť
*/
class UIAPI BrushButton : public _Button
{
public:
	BrushButton();
	~BrushButton();

	UI_DECLARE_OBJECT( BrushButton, OBJ_CONTROL )

	UI_BEGIN_MSG_MAP
		UI_CHAIN_MSG_MAP(_Button)
	UI_END_MSG_MAP


	// ����
protected:
	String      text;
	UIImage*    pImgIcon;

	COLORREF    bkBrushHoverCol;
	COLORREF    bkBrushPressCol;
	COLORREF    bkBrushDisableCol;

	COLORREF    borderPenHoverCol;
	COLORREF    borderPenPressCol;
	COLORREF    borderPenDisableCol;

	COLORREF    textNormalCol;
	COLORREF    textHoverCol;
	COLORREF    textPressCol;
	COLORREF    textDisableCol;


public:
	virtual bool SetAttribute( map<String,String>& mapAttrib );
	virtual      SIZE        GetAutoSize( HDC hDC );

public:
	virtual void DrawDisable( HDC hDC );
	virtual void DrawNormal( HDC hDC ) ;
	virtual void DrawHover( HDC hDC )  ;
	virtual void DrawPress( HDC hDC )  ;
	virtual void DrawFocus( HDC hDC )  ;

public:
	
};

}