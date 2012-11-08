#pragma once


namespace UI
{
 
#define UI_DECLARE_OBJECT( className, type )             \
	static  TCHAR* XmlName()                             \
	/*warning:该函数不支持多态，仅用于CLASS::XXX来调用*/ \
	{                                                    \
		return _T(#className);                           \
	}                                                    \
	virtual TCHAR* GetObjectName()                       \
	{                                                    \
		return _T(#className);                           \
	}                                                    \
	virtual OBJ_TYPE GetObjectType()                     \
	{                                                    \
		return  type;                                    \
	}                                                    \
	typedef UIObjCreator<className> _CreatorClass; 

// 注：这里不能使用String代替TCHAR*返回。因为一个对象可能是在
//     其它DLL中继续Object实现的，在UIDLL中再调用其GetObjectName
//	   会导致heap exception.
#define UI_DECLARE_EMPTY_OBJECT()                        \
	/* 派生类请使用UI_DECLARE_OBJECT进行声明*/           \
	static       TCHAR*      XmlName(){return _T("Object"); }   \
	virtual      TCHAR*      GetObjectName() = 0;        \
	virtual      OBJ_TYPE    GetObjectType() = 0;


// 当控件中有一个renderbase变量时，就应响应onthemechanged消息，然后调用该宏
#define ON_RENDER_THEME_CHANGED(pRender)  \
	if (NULL != pRender && pRender->IsThemeRender())  \
	{ \
		ThemeRenderBase* p = dynamic_cast<ThemeRenderBase*>(pRender); \
		p->OnThemeChanged(); \
	}

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
//	 23-16:  CONTROL/WINDOW通用样式
//   15-1 :  具体控件自己的样式
//      0 :  表示该控件类型

// object style
#define OBJECT_STYLE_TRANSPARENT        0x01000000    // 对象是否透明处理
#define OBJECT_STYLE_HSCROLL            0x02000000    // 对象横向滚动
#define OBJECT_STYLE_VSCROLL            0x04000000    // 对象纵向滚动
#define OBJECT_STYLE_NCOBJ              0x08000000    // 该对象位于父对象的非客户区，不接受偏移

// window style
#define WINDOW_STYLE_DESTROYED          0x00010000    // 表示该窗口已经被销毁了(WM_NCDESTROY)，用于触发OnFinalMessage

// control style
#define CONTROL_STYLE_TABSTOP           0x00010000    // 是否接受TAB快捷键的焦点
#define CONTROL_STYLE_GROUP             0x00020000    // 是否是一个新组的开始

// button style
#define BUTTON_STYLE_MASK               0x000F    // 按钮类型使用的位
#define BUTTON_STYLE_PUSHBUTTON         0x0000    // 普通按钮
#define BUTTON_STYLE_HAVE_TEXT_FIRST    BUTTON_STYLE_PUSHBUTTON  // 标志有内容的按钮。这些按钮才需要创建字体textrender
#define BUTTON_STYLE_RADIOBUTTON        0x0001    // 单选按钮
#define BUTTON_STYLE_CHECKBUTTON        0x0002    // 复选按钮
#define BUTTON_STYLE_HYPERLINK          0x0003    // 超链接按钮
#define BUTTON_STYLE_MENUBUTTON         0x0004    // 菜单按钮
#define BUTTON_STYLE_SPLITMENUBUTTON    0x0005    // 按钮+菜单
#define BUTTON_STYLE_HAVE_TEXT_LAST    BUTTON_STYLE_SPLITMENUBUTTON
#define BUTTON_STYLE_COMBOBOX           0x0006    // 组合框中的按钮
#define BUTTON_STYLE_SCROLLLINEUP       0x0007    // 滚动条按钮
#define BUTTON_STYLE_SCROLLLINEDOWN     0x0008
#define BUTTON_STYLE_SCROLLLINELEFT     0x0009
#define BUTTON_STYLE_SCROLLLINERIGHT    0x000A
#define BUTTON_STYLE_HSCROLLTHUMB       0x000B    // 滚动条拖拽按钮
#define BUTTON_STYLE_VSCROLLTHUMB       0x000C    // 滚动条拖拽按钮
#define BUTOTN_STYLE_SLIDERTRACKBUTTON  0x000D    // 滑动条上面的按钮

// progress style
#define PROGRESS_STYLE_MASK             0x000F    // 进度条类型使用的位
#define PROGRESS_STYLE_PROGRESS         0x0000    // 普通进度条
#define PROGRESS_STYLE_SLIDER           0x0001    // 可拖动的slider ctrl

#define SLIDER_STYLE_POINT_LEFT         0x0100    // 滑块按钮箭头指针左侧 默认为无指向
#define SLIDER_STYLE_POINT_TOP          0x0100    // 滑块按钮箭头指针上侧
#define SLIDER_STYLE_POINT_RIGHT        0x0200    // 滑块按钮箭头指针右侧
#define SLIDER_STYLE_POINT_BOTTOM       0x0200    // 滑块按钮箭头指针下侧
#define SLIDER_STYLE_SHOWTICKS          0x0400    // 显示刻度，默认只显示第一个和最后一个值的刻度
#define SLIDER_STYLE_AUTOTICKS          0x0800    // 自动显示刻度，默认每增一个值显示一个刻度

// edit style
#define EDIT_STYLE_MASK                 0x000F    // 编辑框类型使用的位
#define EDIT_STYLE_COMBOBOX             0x0001    // 组合框中使用的编辑框

// listctrl style
//#define LISTCTRLBASE_ITEM_VARIABLE_HEIGHT 0x0010  // 列表控件各个子项高度不是相等的
#define LISTCTRLBASE_SORT_ASCEND        0x0020    // 升序排序
#define LISTCTRLBASE_SORT_DESCEND       0x0040    // 降序排序
#define LISTCTRLBASE_MULTIPLE_SEL       0x0080    // 是否支持多选
#define LISTCTRLBASE_SELECT_AS_HOVER_MODE  0x0100 // 例如弹出式listbox，1.鼠标按下时不选择，鼠标弹起时选择 2. 当没有hover时显示sel 3. 当有hover时显示hover 4. 鼠标移出控件时不取消hover
#define LISTCTRLBASE_SIZE_2_CONTENT     0x0200    // 控件大小决定内容，例如不带横向滚动条的列表框
#define LISTCTRLBASE_CONTENT_2_SIZE     0x0400    // 内容决定控件大小，例如菜单和弹出式列表框

// listbox style
#define LISTBOX_STYLE_MASK              0x000F    // 编辑框类型使用的位
#define LISTBOX_STYLE_COMBOBOX          0x0001    // 组合框中使用的列表框

// menu style
#define MENU_STYLE_AUTO_DELETE_SUBMENU  0x1000    // 父窗口销毁时，是否自动释放子菜单内存。主要用于从xml中load的菜单
                                                  // 因为是内部创建的子菜单，内部负责销毁子菜单。而外部自己create出来
                                                  // 的菜单自己要去再创建子菜单，因此外部自己去释放内存

// combobox style
#define COMBOBOX_STYLE_DROPDOWN         0x0010    // 可编辑下拉列表
#define COMBOBOX_STYLE_DROPDOWNLIST     0x0020    // 不可编辑下拉列表

// 所有UI类对象的统一基类 
class UIAPI Object : public ObjTree<Object, Message>
{
public:
	Object(void);
	virtual ~Object(void) = 0;

	UI_DECLARE_EMPTY_OBJECT();
	virtual HRESULT InitialConstruct(){return S_OK;}   // 子类在实现该虚函数必须先调用父类的实现
	virtual HRESULT FinalConstruct(){return S_OK;}     // 子类在实现该虚函数时不必调用父类的实现
	virtual void    InitialRelease();                  // 子类在实现该虚函数必须先调用父类的实现
	virtual void    FinalRelease(){};                  // 子类在实现该虚函数时不必调用父类的实现
	virtual void    ObjectMustCreateByUIObjCreator() = 0;   // 创建一个虚函数，该函数仅UIObjCreator实现。其它对象在直接构建时将报错

protected:
	virtual void    DestroyUI();
	
public:
	String   m_strID;          // 该对象在XML中的标识
protected:
	CRect    m_rcParent;       // 该对象的范围，相对于parent.对于Window对象客户区域位置，即左上角为0，0
	
	CRegion4 m_rcNonClient;    // 如果没有滚动条、Header等其它占用空间的控件，m_rcNonClient即为m_rcPadding+m_rcBorder
	CRegion4 m_rcMargin;
	CRegion4 m_rcPadding;
	CRegion4 m_rcBorder;       

	HRGN     m_hRgn;           // （未使用）如果该对象是一个不规则区域，必须设置该值，该值对window类型对象无效. rgn是相对于窗口左上角的。

	int      m_nConfigWidth;   // 对象的宽度，可取值： 数值 | "auto" . （对于window对象，width 是指client区域的大小，不是整个窗口的大小；width包括padding，但不包括margin）
	int      m_nConfigHeight;  // 对象的高度，可取值： 数值 | "auto" . （对于window对象，height是指client区域的大小，不是整个窗口的大小；height包括padding，但不包括margin）
						       // 在这里需要说明，对象的最终占用的宽度= margin.left + width + margin.right
							   // 也就是说这里的width = padding.left + padding.right + content.width

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
	void         UpdateObjectBkgnd( bool bUpdateNow = true );
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
	void         GetBorderRegion( CRegion4* prc ) { prc->CopyRect(&m_rcBorder); }
	void         SetBorderRegion( CRegion4* prc );
	void         GetPaddingRegion( CRegion4* prc ) { prc->CopyRect(&m_rcPadding); }
	void         SetPaddingRegion( CRegion4* prc );
	int          GetPaddingL() { return m_rcPadding.left; }
	int          GetPaddingT() { return m_rcPadding.top; }
	int          GetPaddingR() { return m_rcPadding.right; }
	int          GetPaddingB() { return m_rcPadding.bottom; }
	int          GetPaddingW() { return m_rcPadding.left + m_rcPadding.right; }
	int          GetPaddingH() { return m_rcPadding.top + m_rcPadding.bottom; }
	void         SetMarginRegion( CRegion4* prc) { m_rcMargin.CopyRect(prc); }
	void         GetMarginRegion( CRegion4* prc) { prc->CopyRect(&m_rcMargin); }
	int          GetMarginL() { return m_rcMargin.left; }
	int          GetMarginT() { return m_rcMargin.top; }
	int          GetMarginR() { return m_rcMargin.right; }
	int          GetMarginB() { return m_rcMargin.bottom; }
	int          GetMarginW() { return m_rcMargin.left + m_rcMargin.right; }
	int          GetMarginH() { return m_rcMargin.top + m_rcMargin.bottom; }
	void         GetClientRect( CRect* prc );
	void         GetClientRectAsWin32( CRect* prc );
	void         GetClientRectInWindow( CRect* prc );
	void         GetParentRect( CRect* prc );
	int          GetParentRectL() { return m_rcParent.left; }
	int          GetParentRectT() { return m_rcParent.top; }
	int          GetParentRectR() { return m_rcParent.right; }
	int          GetParentRectB() { return m_rcParent.bottom; }

	POINT        GetRealPosInWindow();
	void         GetWindowRect(CRect* lprc);
	void         WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj);
	void         WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	void         ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	bool         GetScrollOffset(int* pxOffset, int* pyOffset);

	int          GetWidth();
	int          GetHeight();
	int          GetConfigWidth() { return m_nConfigWidth; }
	int          GetConfigHeight() { return m_nConfigHeight; }
	void         SetObjectPos( int x, int y, int cx, int cy, int nFlag=0 );
	void         SetObjectPos( CRect* prc, int nFlag=0 );
	HRGN         GetRgn() { return m_hRgn; }
	HRFONT       GetFont();

	TextRenderBase* GetTextRender(){ return m_pTextRender; }
	RenderBase*     GetBkRender() { return m_pBkgndRender; }
	RenderBase*     GetForeRender() { return m_pForegndRender; }
	void            SetBkRender(RenderBase* p);
	void            SetForeRender(RenderBase* p);
	void            SetTextRender(TextRenderBase* p);

	
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
	void         SetEnable( bool b, bool bUpdateNow=true );
	bool         IsDefault();
	bool         IsReadonly();
	bool         IsHover();
	bool         IsPress();
	bool         IsForePress();
	bool         IsForeHover();
	void         SetDefault( bool b );
	void         SetReadonly( bool b );
	void         SetForceHover( bool b );
	void         SetForcePress( bool b );
	void         SetHover( bool b );
	void         SetPress( bool b );
	void         ModifyStyle( UINT add, UINT remove = 0, bool bNotify = true);
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
	void         DrawObjectTransparentBkgnd(HRDC hRDC, RenderOffsetClipHelper& roc, bool bSelfTransparent);

	// 其它
	HRGN         GetClipRgnInWindow();
	void         SetReDraw( bool bReDraw );

protected:
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
	bool   SetChildObjectAttribute( Object* pChildObj, const String& strPrefix, map<String,String>& mapAttrib, bool bReload );

public:
	// 消息处理
	UI_BEGIN_MSG_MAP
		UIMSG_WM_SETCURSOR( OnSetCursor )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
		UIMSG_WM_HITTEST( OnHitTest )
		UIMSG_WM_GETFONT( OnGetFont )
		UIMSG_WM_THEMECHANGED(OnThemeChanged)
	UI_END_MSG_MAP

protected:
	BOOL    OnEraseBkgnd( HRDC );
	BOOL    OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
	HRFONT  OnGetFont();
	UINT    OnHitTest( POINT* pt );
	void    OnThemeChanged();

	inline  bool testStateBit( UINT bit );
	inline  void setStateBit( UINT bit );
	inline  void clearStateBit( UINT bit );

};


}
