#pragma once
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"

namespace UI{

class Object;

class CanvasLayoutParam : public LayoutParamBase
{
public:
    CanvasLayoutParam();
    ~CanvasLayoutParam();

    virtual void  UpdateByRect();
    virtual void  Serialize(SERIALIZEDATA* pData);
    virtual LAYOUTTYPE  GetLayoutType() { return LAYOUT_TYPE_CANVAS; }
    virtual SIZE  CalcDesiredSize();
    virtual void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

    static int  ParseAlignAttr(const TCHAR*);
public:
    DECLARE_INT_SETGET(ConfigWidth)
    DECLARE_INT_SETGET(ConfigHeight)
    DECLARE_INT_SETGET(ConfigLayoutFlags)
    DECLARE_INT_SETGET(ConfigLeft)
    DECLARE_INT_SETGET(ConfigRight)
    DECLARE_INT_SETGET(ConfigTop)
    DECLARE_INT_SETGET(ConfigBottom)

private:
    int      m_nConfigWidth;   // ����Ŀ�ȣ���ȡֵ�� ��ֵ | "auto" . ������window����width ��ָclient����Ĵ�С�������������ڵĴ�С��width����padding����������margin��
    int      m_nConfigHeight;  // ����ĸ߶ȣ���ȡֵ�� ��ֵ | "auto" . ������window����height��ָclient����Ĵ�С�������������ڵĴ�С��height����padding����������margin��
    // ��������Ҫ˵�������������ռ�õĿ��= margin.left + width + margin.right
    // Ҳ����˵�����width = padding.left + padding.right + content.width
    int      m_nConfigLeft;
    int      m_nConfigRight;
    int      m_nConfigTop;
    int      m_nConfigBottom;
    int      m_nConfigLayoutFlags;
};


//
//	ָ���������Ե��λ��
//
class CanvasLayout : public LayoutBase<CanvasLayout, CanvasLayoutParam, LAYOUT_TYPE_CANVAS>
{
public:
    virtual void  SetAttribute(IMapAttribute* pMapAttrib) {};
    virtual SIZE  Measure();
    virtual void  Arrange(IObject* pObjToArrage=NULL, bool bUpdate=false);

public:
    static void  ArrangeObject(Object*  pChild, const int& nWidth, const int& nHeight);
};

}
