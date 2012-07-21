#pragma  once

namespace UI
{
/*
**	用画刷来绘制背景的按钮，可带有图标和文字。
**  模仿自VS2005工具栏按钮和OFFICE2003无渐变工具栏按钮
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


	// 属性
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