#ifndef UISTRUCTDEFINE_H_C42EA7D6_C2D3_4baf_BBE8_1383191ADCEA
#define UISTRUCTDEFINE_H_C42EA7D6_C2D3_4baf_BBE8_1383191ADCEA

namespace UI
{
//
//	�����ͨ��ָ������ȡ��ɫ��Դ������ÿ����ɫ�仯ʱ���ɾ�����ͨ����ȡָ��ָ���������ˢ�½���
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

    // ����ALPHAWֵ��ɫ����
    void  ReplaceRGB(COLORREF c);
    void  ReplaceRGB(BYTE r1, BYTE g1, BYTE b1);
    void  SetColor(COLORREF c);
    COLORREF  GetGDICompatibleValue();

    // ���ⲿʹ��Color����ʱ�����п�����ͨ��GetColor��ȡ����Color*ָ�룬Ҳ�п����ⲿ�Լ�ֱ������
    // һ��COLORREFֵ�����ʱ��͵�newһ��Color*��ʹ���߼���GetColor����һ�¡�Ϊ�˴ﵽ�ͷ��߼�һ
    // �µ�Ŀ�ģ����ͷź�����д��Release()
    long  AddRef();
    long  Release();

private:
    long  lRef;
};
typedef Color* ColorPtr;

//
// 9������������
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
    TOOLTIP_ACTION_FLAG_NORMAL = 0,              // ����ϵͳ��ʾ���������ؼ�ʱ����ʱ��ʾ���ƿ��ؼ�ʱ����������
    TOOLTIP_ACTION_FLAG_CAN_MOUSE_HOVER = 1,     // �����ؼ�ʱ����ʱ��ʾ���ƿ��ؼ�ʱ����ʱ���أ���������ʾ�������Ǳ�����ʾ
    TOOLTIP_ACTION_FLAG_INPLACE = 2,             // ������ʾ�б�ؼ�һ����ʾ���µ����֣�������ʾ����������
    TOOLTIP_ACTION_FLAG_MESSAGEBOX = 4,          // ���ڴ���/��Ϣ��ʾ�������������������ʾ���ܹ����洰���ƶ���һ���Ǵ�����ͷ������
};

interface IObject;
// Show�Ĳ���
struct TOOLTIPITEM
{
    TOOLTIPITEM() { memset(this, 0, sizeof(TOOLTIPITEM)); }

    IObject*  pNotifyObj;   // Ҫ��ʾ��ʾ���Ŀؼ����Լ���Ϣ֪ͨ�Ķ���
    void*     pItemData;    // �����Ҫ��ʾ�ؼ��ڲ�һ��item����ʾ��Ϣ���ɽ���ֵ����Ϊ���ʵ�ֵ��Ȼ��mgr����pNotifyObj����UI_WM_GET_TOOLTIPINFO��Ϣ
    int       nToolTipFlag; // ��ʾ������Ϊ��־�� ��20120928 ����ֻ֧��normal��
};


interface IListItemBase;
struct UIOWNERDRAWSTRUCT
{
    IRenderTarget*  pRenderTarget;  

    IObject*        pObjDraw;   // Ҫ���ƵĶ���
    IListItemBase*  pItemDraw;  // �����һ��listctrl�����ʾҪ���Ƶ�ITEM
};


// ps: stl�е�list����һ�����⣺�ڱ����Ĺ����У����ȥɾ��һ������������
//     �������дһ��list���ڱ�����ɾ��Ҳû������
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
    int  nRepeatCount;   // ��Ӧ����,-1��ʾ����
    IMessage* pNotify;    // ��ӦWM_TIMER�Ķ���
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