#pragma once

namespace UI
{
class Layout
{
public:
	Layout(void){};
	virtual ~Layout(void){};

public:
	SIZE  Measure();                                               // ���� m_pPanel ����Ҫ�Ĵ�С���ڲ�����MeasureChildObject
	void  Arrange(Object* pObjToArrage=NULL, bool bReDraw=false);  // ���� m_pPanel ���ӿؼ���λ�úʹ�С���ڲ�����ArrangeChildObject

	virtual bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false ){ return true; };

	virtual SIZE  MeasureChildObject() = 0; // ������ʵ�֣������ӿؼ�����Ҫ�Ŀؼ�
	virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false) = 0; // ������ʵ�֣������ӿؼ���rectW��rectP

	// �ӿؼ���С�����ı�ʱ����Ҫ���������֡�����һ���������ء���󡢱�С��
	//virtual void  OnChildObjectWindowPosChaned( Object* pObj ){};

public:
	Panel*   m_pPanel;                         // ��ò��ֹ�����panel
	
};

class StackLayout : public Layout
{
public:
	StackLayout();
	virtual SIZE  MeasureChildObject();
	virtual void  ArrangeChildObject(Object* pObjToArrage = NULL, bool bReDraw=false );
	virtual bool  SetAttribute( map<String,String>& mapAttrib, bool bReload=false );
public:
	LAYOUT_STACK_DIRECTION  direction;    // ��ջ�Ų��ķ���
};


//
//	ָ���������Ե��λ��
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

// ��񲼾ֵ�һ�л�һ�еĴ�С ����
class GridWH
{
public:
	GridWH();

	int          last;  // ���մ�С
	int          xml;   // ��xml�еĴ�С��������ֵ����*��������
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
**	ע�⣺DockLayoutֻ������һ��center���󣬵��ö��������һ��Panel��Object
**		  ����������ļ����ж��center������ֻȡ��һ������
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

// ���沼�ֹ�������ר���������ö��㴰������Ļ�еĴ�С��λ��
// ���沼����Щ���⣬��ֱ�ӹ����ڵ�λ�ã������ںͿؼ���Panel�кܶ಻һ���ĵط���
// �細�ڷֿͻ����ͷǿͻ���������DesktopLayoutֻ��Ҫ����һ�����󣬼�Window��û������
// �Ӷ���������ﲻ��DesktopLayout������LayoutManager
class DesktopLayout   
{
public:
	DesktopLayout();

	virtual void  Arrange();
public:
	WindowBase*  m_pWindow; // ��LayoutManager::m_pPanel���ʲ�һ����m_pPanel��layout���ֹ���������LayoutManager
                            // �����m_pWindow��layout���ֹ������Ͳ���DesktopLayout
	                        // ����DesktopLayout�಻��ҪMeasure��������Ϊm_pWindow��Measure��m_pWindow->m_pLayout����ȡ

	// ����ֵ����δָ����ʾ����
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