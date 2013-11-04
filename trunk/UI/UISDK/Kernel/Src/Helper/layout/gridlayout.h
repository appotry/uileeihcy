#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

namespace UI
{
class Object;


class GridLayoutParam : public LayoutParamBase
{
public:
    GridLayoutParam();
    ~GridLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_GRID; }
    virtual SIZE  CalcDesiredSize();

public:
    DECLARE_INT_SETGET(ConfigWidth)
    DECLARE_INT_SETGET(ConfigHeight)
    DECLARE_INT_SETGET(ConfigLayoutFlags)
    DECLARE_INT_SETGET(Row)
    DECLARE_INT_SETGET(Col)
    DECLARE_INT_SETGET(RowSpan)
    DECLARE_INT_SETGET(ColSpan)

private:
    Object*  m_pObj;

    int  m_nConfigWidth;  
    int  m_nConfigHeight; 
    int  m_nConfigLayoutFlags;

    int  m_nRow;
    int  m_nCol;
    int  m_nRowSpan;
    int  m_nColSpan;
};


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

class GridLayout : public LayoutBase<GridLayout, GridLayoutParam, LAYOUT_TYPE_GRID>
{
public:
    GridLayout();
	~GridLayout();


    virtual void  SetAttribute(IMapAttribute* pMapAttrib);
    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);
    virtual SIZE  Measure();
    virtual void  Arrange(IObject* pObjToArrage=NULL, bool bUpdate=false);

	int     getColPos(int nCol);    
	int     getRowPos(int nRow);

private:
	vector<GridWH>   widths;
	vector<GridWH>   heights;
};

}