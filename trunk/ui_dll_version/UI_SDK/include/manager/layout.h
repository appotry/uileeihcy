#pragma once

namespace UI
{
class Layout
{
public:
	Layout(void){};
	virtual ~Layout(void){};

public:
	SIZE  Measure();                                               // 返回 m_pPanel 所需要的大小；内部调用MeasureChildObject
	void  Arrange(Object* pObjToArrage=NULL, bool bReDraw=false);  // 安排 m_pPanel 的子控件的位置和大小；内部调用ArrangeChildObject

	virtual bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false ){ return true; };

	virtual SIZE  MeasureChildObject() = 0; // 子类需实现；返回子控件所需要的控件
	virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false) = 0; // 子类需实现；安排子控件的rectW，rectP

	// 子控件大小发生改变时，需要重新整理布局。（如一个对象隐藏、变大、变小）
	//virtual void  OnChildObjectWindowPosChaned( Object* pObj ){};

public:
	Panel*   m_pPanel;                         // 与该布局关联的panel
	
};

class StackLayout : public Layout
{
public:
	StackLayout();
	virtual SIZE  MeasureChildObject();
	virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false );
	virtual bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
public:
	LAYOUT_STACK_DIRECTION  direction;    // 堆栈排布的方向
};


//
//	指定布局离边缘的位置
//
class CanvasLayout : public Layout
{
public:
	CanvasLayout();
	virtual SIZE  MeasureChildObject();
	virtual void  ArrangeChildObject(Object* pObjToArrage, bool bReDraw=false );

public:
};


//
enum GridWHType
{
	GWHT_AUTO,
	GWHT_ASTERISK,  // *
	GWHT_VALUE
};

// 表格布局的一行或一列的大小 描述
class GridWH
{
public:
	GridWH();

	int          last;  // 最终大小
	int          xml;   // 在xml中的大小描述，该值仅对*类型有用
	GridWHType   type;  // auto, *, value
};

class GridLayout : public Layout
{
public:
	~GridLayout();

	virtual SIZE  MeasureChildObject();
	virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false);
	virtual bool  SetAttribute(map<String,String>& mapAttrib, bool bReload=false);

	int     getColPos(int nCol);
	int     getRowPos(int nRow);

private:
	vector<GridWH>   widths;
	vector<GridWH>   heights;
};

/*
**	注意：DockLayout只允许有一个center对象，但该对象可以是一个Panel、Object
**		  如果在配置文件中有多个center对象，则只取第一个对象
*/
class DockLayout : public Layout
{
public:
	virtual SIZE  MeasureChildObject();
	virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false);
};

class CardLayout : public Layout
{
public:
	virtual SIZE  MeasureChildObject();
	virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false);
public:
	
};

// 桌面布局管理器，专门用于设置顶层窗口在屏幕中的大小与位置
// 桌面布局有些特殊，它直接管理窗口的位置，但窗口和控件、Panel有很多不一样的地方，
// 如窗口分客户区和非客户区。而且DesktopLayout只需要管理一个对象，即Window，没有其他
// 子对象，因此这里不将DesktopLayout派生自LayoutManager
class DesktopLayout   
{
public:
	DesktopLayout();

	virtual void  Arrange();
public:
	WindowBase*  m_pWindow; // 于LayoutManager::m_pPanel性质不一样，m_pPanel的layout布局管理器就是LayoutManager
                            // 而这里，m_pWindow的layout布局管理器就不是DesktopLayout
	                        // 所以DesktopLayout类不需要Measure函数，因为m_pWindow的Measure由m_pWindow->m_pLayout来获取

	// 坐标值。如未指定表示居中
	int        left;
	int        top;
	int        right;
	int        bottom;

};


class LayoutManagerFactory 
{
public:
	static Layout* GetLayout( String strType, Panel* pPanel );
};

}