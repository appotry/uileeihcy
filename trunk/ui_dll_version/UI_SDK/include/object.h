#pragma once


namespace UI
{
 
typedef map<String,String>  ATTRMAP;

#define UI_DECLARE_OBJECT( className, type )             \
	static  String XmlName()                             \
	/*warning:该函数不支持多态，仅用于CLASS::XXX来调用*/ \
	{                                                    \
		String str =  _T(#className);                    \
		return str;                                      \
	}                                                    \
	virtual String GetObjectName()                       \
	{                                                    \
		return _T(#className);                           \
	}                                                    \
	virtual OBJ_TYPE GetObjectType()                     \
	{                                                    \
		return  type;                                    \
	}

#define UI_DECLARE_EMPTY_OBJECT()                        \
	/* 派生类请使用UI_DECLARE_OBJECT进行声明*/           \
	static       String      XmlName(){return _T("Object"); }   \
	virtual      String      GetObjectName() = 0;        \
	virtual      OBJ_TYPE    GetObjectType() = 0;


// 对象类型
enum OBJ_TYPE
{
	OBJ_NULL,
	OBJ_CONTROL,
	OBJ_HWNDHOST,
	OBJ_PANEL,
	OBJ_WINDOW
};


//
// object的state bit，状态位
// 0000 0000 0000 0000
//
#define OSB_UNVISIBLE     0x0001        // 自己是否可见
#define OSB_DISABLE       0x0002        // 自己是否可用

#define CSB_PREVENTREDRAW 0x0800        // 不可进行自绘
#define CSB_DEFAULT       0x0400
#define CSB_FOCUS         0x0200
#define CSB_READONLY      0x0100
#define CSB_FORCEHOVER    0x0080         
#define CSB_FORCEPRESS    0x0040
#define CSB_HOVER         0x0020
#define CSB_PRESS         0x0010

#define WSB_ACTIVE        0x1000         // 窗口为active

// STYLE样式占位说明
//   31-24:  OBJECT 通用样式
//	 23-16:  CONTROL通用样式
//   15-1 :  具体控件自己的样式
//      0 :  表示该控件类型

// object style
#define OBJECT_STYLE_TRANSPARENT        0x01000000    // 对象是否透明处理
#define OBJECT_STYLE_HSCROLL            0x02000000    // 对象横向滚动
#define OBJECT_STYLE_VSCROLL            0x04000000    // 对象纵向滚动
#define OBJECT_STYLE_NCOBJ              0x08000000    // 该对象位于父对象的非客户区，不接受偏移

// control style
#define CONTROL_STYLE_TABSTOP           0x00010000    // 是否接受TAB快捷键的焦点
#define CONTROL_STYLE_GROUP             0x00020000    // 是否是一个新组的开始

// button style
#define BUTTON_STYLE_MASK               0x000F    // 按钮类型使用的位
#define BUTTON_STYLE_PUSHBUTTON         0x0000    // 普通按钮
#define BUTTON_STYLE_RADIOBUTTON        0x0001    // 单选按钮
#define BUTTON_STYLE_CHECKBUTTON        0x0002    // 复选按钮
#define BUTTON_STYLE_HYPERLINK          0x0003    // 超链接按钮
#define BUTTON_STYLE_MENUBUTTON         0x0004    // 菜单按钮
#define BUTTON_STYLE_SPLITMENUBUTTON    0x0005    // 按钮+菜单
#define BUTTON_STYLE_COMBOBOX           0x0006    // 组合框中的按钮

// progress style
#define PROGRESS_STYLE_MASK             0x000F    // 进度条类型使用的位
#define PROGRESS_STYLE_PROGRESS         0x0000    // 普通进度条
#define PROGRESS_STYLE_SLIDER           0x0001    // 可拖动的slider ctrl
#define PROGRESS_STYLE_SCROLLBAR        0x0002    // 作为滚动条的一部分

// edit style
#define EDIT_STYLE_MASK                 0x000F    // 编辑框类型使用的位
#define EDIT_STYLE_COMBOBOX             0x0001    // 组合框中使用的编辑框

// listctrl style
#define LISTCTRLBASE_ITEM_VARIABLE_HEIGHT 0x0010  // 列表控件各个子项高度不是相等的
#define LISTCTRLBASE_SORT_ASCEND        0x0020    // 升序排序
#define LISTCTRLBASE_SORT_DESCEND       0x0040    // 降序排序
#define LISTCTRLBASE_MULTIPLE_SEL       0x0080    // 是否支持多选

// listbox style
#define LISTBOX_STYLE_MASK              0x000F    // 编辑框类型使用的位
#define LISTBOX_STYLE_COMBOBOX          0x0001    // 组合框中使用的列表框





// 所有UI类对象的统一基类 
class UIAPI Object : public ObjTree<Object, Message>
{
public:
	Object(void);
	virtual ~Object(void) = 0;

	UI_DECLARE_EMPTY_OBJECT();

protected:
	void DestroyUI();
	
public:
	String   m_strID;          // 该对象在XML中的标识
protected:
	CRect    m_rcParent;       // 该对象的范围，相对于parent.对于Window对象客户区域位置，即左上角为0，0
	
	CRegion4 m_rcNonClient;    // 如果在没有滚动条、Header等其它占用空间的控件，m_rcNonClient即为m_rcPadding
	CRegion4 m_rcMargin;
	CRegion4 m_rcPadding;

	HRGN     m_hRgn;           // 如果该对象是一个不规则区域，必须设置该值，该值对window类型对象无效. rgn是相对于窗口左上角的。

	int      m_nConfigWidth;   // 对象的宽度，可取值： 数值 | "auto" . （对于window对象，width 是指client区域的大小，不是整个窗口的大小；width包括padding，但不包括margin）
	int      m_nConfigHeight;  // 对象的高度，可取值： 数值 | "auto" . （对于window对象，height是指client区域的大小，不是整个窗口的大小；height包括padding，但不包括margin）
						       // 在这里需要说明，对象的最终占用的宽度= margin.left + width + margin.right
							   // 也就是说这里的width = padding.left + padding.right + content.width
// 	int     minwidth;
// 	int     minheight;
// 	int     maxwidth;
// 	int     maxheight;

	UINT     m_nStateBit;      // 对象的状态，如visible/disable
	UINT     m_nDropDragBit;

	UINT     m_nStyle;         // 普通样式
	UINT     m_nStyleEx;       // 扩展样式

	RenderBase*          m_pBkgndRender;   // 背景渲染
	RenderBase*          m_pForegndRender; // 前景渲染
	TextRenderBase*      m_pTextRender;    // 文字渲染
	UICursor*            m_pCursor;        // 对象的鼠标样式
	ATTRMAP              m_mapAttribute;   // 剩余的其他暂未解析的属性，用于扩展，TODO：优化

public:
	WindowBase*  GetWindowObject();
	HWND         GetHWND();
	Object*      FindChildObject( const String& objId);
//	Object*      GetChildObject( const String&  objIdPath );  -- 不提供，因为换肤后，路径则不是固定的

//	void         UpdateRect();
	void         UpdateObject( bool bUpdateNow = true);
//	void         UpdateObject( RECT* prc, bool bUpdateNow = true);
	void         UpdateLayout( bool bUpdate );
//	int          GetParentList( vector<Object*>& vParents );  -- 用不上，直接使用EnumParentObject即可

public:
	// 区域操作函数
	void         GetNonClientRegion( CRegion4* prc ) { prc->CopyRect(m_rcNonClient); }
	void         SetNonClientRegion( CRegion4* prc );
	int          GetNonClientL() { return m_rcNonClient.left; }
	int          GetNonClientT() { return m_rcNonClient.top; }
	int          GetNonClientR() { return m_rcNonClient.right; }
	int          GetNonClientB() { return m_rcNonClient.bottom; }
	int          GetNonClientW() { return m_rcNonClient.left + m_rcNonClient.right; }
	int          GetNonClientH() { return m_rcNonClient.top + m_rcNonClient.bottom; }
	void         GetPaddingRegion( CRegion4* prc ) { prc->CopyRect(&m_rcPadding); }
	void         SetPaddingRegion( CRegion4* prc );
	int          GetPaddingL() { return m_rcPadding.left; }
	int          GetPaddingT() { return m_rcPadding.top; }
	int          GetPaddingR() { return m_rcPadding.right; }
	int          GetPaddingB() { return m_rcPadding.bottom; }
	int          GetPaddingW() { return m_rcPadding.left + m_rcPadding.right; }
	int          GetPaddingH() { return m_rcPadding.top + m_rcPadding.bottom; }
	void         GetMarginRegion( CRegion4* prc) { prc->CopyRect(&m_rcMargin); }
	int          GetMarginL() { return m_rcMargin.left; }
	int          GetMarginT() { return m_rcMargin.top; }
	int          GetMarginR() { return m_rcMargin.right; }
	int          GetMarginB() { return m_rcMargin.bottom; }
	int          GetMarginW() { return m_rcMargin.left + m_rcMargin.right; }
	int          GetMarginH() { return m_rcMargin.top + m_rcMargin.bottom; }
	void         GetClientRect( CRect* prc );
	void         GetClientRectAsWin32( CRect* prc );
	void         GetParentRect( CRect* prc );
	int          GetParentRectL() { return m_rcParent.left; }
	int          GetParentRectT() { return m_rcParent.top; }
	int          GetParentRectR() { return m_rcParent.right; }
	int          GetParentRectB() { return m_rcParent.bottom; }

	int          GetWidth();
	int          GetHeight();
	int          GetConfigWidth() { return m_nConfigWidth; }
	int          GetConfigHeight() { return m_nConfigHeight; }
	void         SetObjectPos( int x, int y, int cx, int cy, int nFlag=0 );
	void         SetObjectPos( CRect* prc, int nFlag=0 );
	HRGN         GetRgn() { return m_hRgn; }
	HRFONT       GetFont();
	RenderBase*  GetBkRender() { return m_pBkgndRender; }
	RenderBase*  GetForeRender() { return m_pForegndRender; }
	void         SetBkRender(RenderBase* p);
	void         SetForeRender(RenderBase* p);

	POINT        GetRealPosInWindow();
	void         GetWindowRect(CRect* lprc);
	void         WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj);
	void         WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	void         ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	bool         GetScrollOffset(int* pxOffset, int* pyOffset);
	
	// 样式操作
	int          GetStateBit() { return m_nStateBit; }
	bool         IsFocus();
	void         SetFocus( bool b );
	bool         IsTabstop();
	bool         IsGroup();
	void         SetTabstop( bool b );
	void         SetGroup( bool b );
	bool         IsVisible();
	bool         IsCollapsed();
	bool         IsMySelfVisible();
	bool         IsEnable();
	void         SetVisible( bool b, bool bUpdateNow=true );
	void         SetEnable( bool b );
	bool         IsDefault();
	bool         IsReadonly();
	bool         IsHover();
	bool         IsPress();
	void         SetDefault( bool b );
	void         SetReadonly( bool b );
	void         SetForceHover( bool b );
	void         SetForcePress( bool b );
	void         SetHover( bool b );
	void         SetPress( bool b );
	void         ModifyStyle( UINT add, UINT remove = 0 );
	bool         TestStyle(UINT nStyle){ if(m_nStyle&nStyle)return true; return false; }
	bool         IsTransparent();
	void         SetActive(bool b);
	bool         IsActive();
	void         SetAsNcObject() { this->ModifyStyle(OBJECT_STYLE_NCOBJ); } // ObjTree中有引用
	bool         IsNcObject() { return TestStyle(OBJECT_STYLE_NCOBJ); }     // ObjTree中有引用

	// 扩展属性操作
	void         EraseAttribute(const String& strKey) { m_mapAttribute.erase(strKey); }
	void         AddAttribute(const String& strKey, const String& strValue) { m_mapAttribute[strKey] = strValue; }
	bool         GetAttribute(const String& strKey, String& strValue);
	
	// 绘制
	void         DrawObject(HRDC hRDC, RenderOffsetClipHelper roc);   
	void         DrawObjectTransparentBkgnd(HRDC hRDC, RenderOffsetClipHelper& roc);

	// 其它
	HRGN         GetClipRgnInWindow();
	void         SetReDraw( bool bReDraw );

private:
	Object*      _findChildObjectItem( const String& objId );
	void         _drawChildObject(HRDC hRDC, RenderOffsetClipHelper& roc);
	void         _drawNcChildObject(HRDC hRDC, RenderOffsetClipHelper& roc);
public:

	// 在布局过程中返回自己所需要的大小(包括padding+margin)，GetDesiredSize在子类中表现不同，
	// 具体为： 在Panel中，会去调用 m_pLayout->Measure，而LayoutManager 又会去再调用LayoutManager子类的MeasureChildObject虚函数
	//          在Control中，会去调用GetAutoSize虚函数
	virtual      SIZE        GetDesiredSize( HRDC hDC ) = 0;  

	// 将xml用的配置转成对象的属性，注意，子类重载该函数时，必须先调用父类的该方法
	// bReload表示为换肤时调用，此时不能再给某些属性赋值，例如text属性
	virtual      bool        SetAttribute(ATTRMAP& mapAttrib, bool bReload);

	// 在初始化Object和换肤时，将触发该函数。因为换肤时会先销毁所有资源，因此在ResetAttribute中要释放所有资源对象
	// 注意，子类重载该函数时，必须先调用父类的该方法
	virtual      void        ResetAttribute();

public:
	bool   SetChildObjectAttribute( Object* pChildObj, const String& strPrifix, map<String,String>& mapAttrib, bool bReload );

public:
	// 消息处理
	UI_BEGIN_MSG_MAP
		UIMSG_WM_SETCURSOR( OnSetCursor )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
		UIMSG_WM_HITTEST( OnHitTest )
		UIMSG_WM_GETFONT( OnGetFont )
	UI_END_MSG_MAP

protected:
	BOOL    OnEraseBkgnd( HRDC );
	BOOL    OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
	HRFONT  OnGetFont();
	UINT    OnHitTest( POINT* pt );

	inline  bool testStateBit( UINT bit );
	inline  void setStateBit( UINT bit );
	inline  void clearStateBit( UINT bit );

};


}
