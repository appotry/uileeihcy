#ifndef LISTITEMLAYOUT_H_6F45FF31_A2CF_4c4d_B411_899A5CC12D61
#define LISTITEMLAYOUT_H_6F45FF31_A2CF_4c4d_B411_899A5CC12D61
#include "UISDK\Kernel\Inc\Interface\ilistctrlbase.h"

namespace UI
{
interface IListItemBase;
interface IListCtrlBase;

enum LISTCTRL_ITEM_LAYOUT_TYPE
{
    // 1. ��ͨϵͳlistbox. [�̶��и�]
    //    ��������  ����NONE������AUTO
    //    ��  �֣�  ����ռ��client width��������������
    //    AUTOSIZE: ����0(��֧��AUTO), ����total height
    LISTCTRL_ITEM_LAYOUT_TYPE_1,

    // 2. ��ͨϵͳlistbox��֧�ֺ������. [�̶��и�]
    //    ��������  ����AUTO������AUTO
    //    ��  �֣�  ����ȡmax width������client width��������������
    //    AUTOSIZE: ����max width, ����total height
    LISTCTRL_ITEM_LAYOUT_TYPE_2,
};


class  ListCtrlLayoutMgr : public IListCtrlLayoutMgr
{
public:
    ListCtrlLayoutMgr()
    {
        m_pIListCtrl = NULL;
        m_pLayout = NULL;
    }
    virtual ~ListCtrlLayoutMgr() 
    {
        SAFE_RELEASE(m_pLayout);
    };

    void  SetIListCtrlBase(IListCtrlBase* pListCtrlBase) 
    {
        m_pIListCtrl = pListCtrlBase; 
        if (m_pLayout)
            m_pLayout->SetIListCtrlBase(m_pIListCtrl);
    }
    virtual  IListCtrlLayout*  GetLayout()
    {
        return m_pLayout;
    }
    virtual void  SetLayout(IListCtrlLayout* p) 
    {
        SAFE_RELEASE(m_pLayout);
        m_pLayout = p;
    }

    //	���´�pStart��ʼ�����ÿһ��item�� m_rcParent�������²���һ��pItem
    //  �ú���������ÿһ��item��rect�����ܹ�����������
    virtual bool  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent) 
    {
        if (NULL == m_pLayout)
            return false;

        m_pLayout->Arrange(pStartToArrange, pSizeContent);
        return true;
    }
    virtual void  Measure(SIZE* pSize);

protected:
    IListCtrlBase*  m_pIListCtrl;
    IListCtrlLayout*  m_pLayout;
};


class ListCtrlItemLayout1 : public IListCtrlLayout
{
public:
    ListCtrlItemLayout1();

    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};

class ListCtrlItemLayout2 : public IListCtrlLayout
{
public:
    ListCtrlItemLayout2();

    virtual void  SetIListCtrlBase(IListCtrlBase* p) { m_pIListCtrlBase = p; }
    virtual void  Arrange(IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release() { delete this; }

protected:
    IListCtrlBase*  m_pIListCtrlBase;
};


IListCtrlLayout*  CreateListCtrlLayout(int nType, IListCtrlBase* p);
}

#endif // LISTITEMLAYOUT_H_6F45FF31_A2CF_4c4d_B411_899A5CC12D61