#ifndef UISTRUCTDEFINE_H_C42EA7D6_C2D3_4baf_BBE8_1383191ADCEA
#define UISTRUCTDEFINE_H_C42EA7D6_C2D3_4baf_BBE8_1383191ADCEA

namespace UI
{
//
//	让外界通过指针来获取颜色资源，这样每次颜色变化时，可经立即通过获取指针指向的内容来刷新界面
//

class UISDKAPI Color
{
public:
    Color();
    Color(int r, int g, int b, int a);
    Color(DWORD rgba);
    static Color* CreateInstance(COLORREF color);

    operator COLORREF();
    union
    {
        COLORREF   m_col;
        struct  
        {
            byte   r;
            byte   g;
            byte   b;
            byte   a;
        };
    };

    int   GetR();
    int   GetG();
    int   GetB();
    int   GetA();

    void  SetR(byte n);
    void  SetG(byte n);
    void  SetB(byte n);
    void  SetA(byte n);

    // 保留ALPHAW值颜色拷贝
    void  ReplaceRGB(COLORREF c);
    void  ReplaceRGB(BYTE r1, BYTE g1, BYTE b1);
    void  SetColor(COLORREF c);
    COLORREF  GetGDICompatibleValue();

    // 在外部使用Color对象时，既有可能是通过GetColor获取到的Color*指针，也有可能外部自己直接设置
    // 一个COLORREF值，这个时候就得new一个Color*，使得逻辑与GetColor保持一致。为了达到释放逻辑一
    // 致的目的，将释放函数都写成Release()
    long  AddRef();
    long  Release();

private:
    long  lRef;
};
typedef Color* ColorPtr;

//
// 9宫拉伸区域定义
//
class  Image9Region
{
public:
    Image9Region(){ Set(0); }
	void Set( WORD w );
	void Set( WORD wLeftRight, WORD wTopBottom );
	void Set( WORD wLeft, WORD wTop, WORD wRight, WORD wBottom );

	WORD topleft;
	WORD top;
	WORD topright;
	WORD left;
	WORD right;
	WORD bottomleft;
	WORD bottom;
	WORD bottomright;
};

inline void Image9Region::Set( WORD w )
{
	topleft = top = topright = left = right = bottomleft = bottom = bottomright = w;
}

inline void Image9Region::Set( WORD wLeftRight, WORD wTopBottom )
{
	topleft = topright = left = right = bottomleft = bottomright = wLeftRight;
	top = bottom = wTopBottom;
}

inline void Image9Region::Set( WORD wLeft, WORD wTop, WORD wRight, WORD wBottom )
{
	topleft = left = bottomleft = wLeft;
	top = wTop;
	topright = right = bottomright = wRight;
	bottom = wBottom;
}

class IRenderTarget;
struct RENDERBASE_DRAWSTATE
{
    IRenderTarget*  pRenderTarget;
    CRect  rc;
    int    nState;
};

struct TEXTRENDERBASE_DRAWSTATE
{
    RENDERBASE_DRAWSTATE  ds_renderbase;
    const TCHAR*  szText;
    int  nDrawTextFlag;
};


enum
{
    TOOLTIP_ACTION_FLAG_NORMAL = 0,              // 正常系统提示条，悬浮控件时，计时显示；移开控件时，立即隐藏
    TOOLTIP_ACTION_FLAG_CAN_MOUSE_HOVER = 1,     // 悬浮控件时，计时显示；移开控件时，计时隐藏；鼠标放在提示窗口上是保持显示
    TOOLTIP_ACTION_FLAG_INPLACE = 2,             // 用于显示列表控件一行显示不下的文字，立即显示，立即隐藏
    TOOLTIP_ACTION_FLAG_MESSAGEBOX = 4,          // 用于错误/信息提示，允许弹出多个，立即显示，能够跟随窗口移动，一般是带根箭头的气泡
};

interface IObject;
// Show的参数
struct TOOLTIPITEM
{
    TOOLTIPITEM() { memset(this, 0, sizeof(TOOLTIPITEM)); }

    IObject*  pNotifyObj;   // 要显示提示条的控件，以及消息通知的对象
    void*     pItemData;    // 如果是要显示控件内部一个item的提示信息，可将该值设置为合适的值，然后mgr会向pNotifyObj发送UI_WM_GET_TOOLTIPINFO消息
    int       nToolTipFlag; // 提示条的行为标志。 （20120928 现在只支持normal）
};


interface IListItemBase;
struct UIOWNERDRAWSTRUCT
{
    IRenderTarget*  pRenderTarget;  

    IObject*        pObjDraw;   // 要绘制的对象
    IListItemBase*  pItemDraw;  // 如果是一个listctrl，则表示要绘制的ITEM
};


// ps: stl中的list存在一个问题：在遍历的过程中，如果去删除一个对象会崩溃。
//     因此重新写一个list，在遍历中删除也没有问题
template <class T>
struct UIListItem
{
    UIListItem() {m_pData = NULL; m_pPrev = m_pNext = NULL;}
    T  m_pData;
    UIListItem*  m_pPrev;
    UIListItem*  m_pNext;
};
template <class T>
class UIList
{
public:
    UIList()
    {
        m_pFirst = NULL;
        m_pLast = NULL;
        m_nCount = 0;
    }
    ~UIList()
    {
        UIListItem<T>* p = m_pFirst;
        UIListItem<T>* pNext = NULL;

        while (p)
        {
            pNext = p->m_pNext;    
            delete p;
            p = pNext;
        }
        m_nCount = 0;
        m_pFirst = m_pLast = NULL;
    }

    UIListItem<T>*  Find(T t)
    {
        UIListItem<T>* pItem = m_pFirst;
        while (pItem)
        {
            if (pItem->m_pData == t)
                return pItem;

            pItem = pItem->m_pNext;
        }
        return NULL;
    }

    T  At(int nPos)
    {
        if (nPos < 0 || nPos >= m_nCount)
            return NULL;

        UIListItem<T>* pItem = m_pFirst;
        int i = 0;
        while (pItem)
        {
            if (nPos == i++)
                return pItem->m_pData;

            pItem = pItem->m_pNext;
        }
        return (T)NULL;
    }

    void  Remove(T t)
    {
        UIListItem<T>* pItem = Find(t);
        if (pItem)
        {
            if (pItem->m_pPrev)
                pItem->m_pPrev->m_pNext = pItem->m_pNext;
            else
                m_pFirst = pItem->m_pNext;

            if (pItem->m_pNext)
                pItem->m_pNext->m_pPrev = pItem->m_pPrev;
            else
                m_pLast = pItem->m_pPrev;

            delete pItem;
            m_nCount--;
        }
    }

    void  Add(T t)
    {
        UIListItem<T>* p = new UIListItem<T>;
        p->m_pData = t;

        if (m_pLast)
        {
            m_pLast->m_pNext = p;
            p->m_pPrev = m_pLast;
        }
        m_pLast = p;
        if (NULL == m_pFirst)
            m_pFirst = p;

        m_nCount++;
    }

    int  GetCount() { return m_nCount; }

    void  DELETE_ALL()
    {
        UIListItem<T>* p = m_pFirst;
        UIListItem<T>* pNext = NULL;

        while (p)
        {
            pNext = p->m_pNext;
            SAFE_DELETE(p->m_pData);
            delete p;
            p = pNext;
        }
        m_nCount = 0;
        m_pFirst = m_pLast = NULL;
    }
    void  RELEASE_ALL()
    {
        UIListItem<T>* p = m_pFirst;
        UIListItem<T>* pNext = NULL;

        while (p)
        {
            pNext = p->m_pNext;
            SAFE_RELEASE(p->m_pData);
            delete p;
            p = pNext;
        }
        m_nCount = 0;
        m_pFirst = m_pLast = NULL;
    }

public:
    UIListItem<T>*  m_pFirst;
    UIListItem<T>*  m_pLast;
private:
    int  m_nCount;
};


struct TimerItem;
typedef void  (*UITimerProc)(UINT_PTR nTimerID, TimerItem* pTimerItem);

interface IMessage;
struct TimerItem
{
    int  nRepeatCount;   // 响应次数,-1表示无限
    IMessage* pNotify;    // 响应WM_TIMER的对象
    UITimerProc  pProc;
    int  nId;
    WPARAM   wParam;
    LPARAM   lParam; 

    TimerItem()
    {
        nRepeatCount = -1;
        pNotify = NULL;
        pProc = NULL;
        wParam = 0;
        lParam = 0;
        nId = 0;
    }
};
}
#endif // UISTRUCTDEFINE_H_C42EA7D6_C2D3_4baf_BBE8_1383191ADCEA