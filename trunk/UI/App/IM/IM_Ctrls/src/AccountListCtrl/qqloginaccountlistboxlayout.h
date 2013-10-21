#pragma once

class QQLoginAccountListBoxLayout : public UI::IListCtrlLayout
{
public:
    QQLoginAccountListBoxLayout() { m_pIListCtrlBase = NULL; }

    virtual void  SetIListCtrlBase(UI::IListCtrlBase* p){ m_pIListCtrlBase = p; };
    virtual void  Arrange(UI::IListItemBase* pStartToArrange, SIZE* pSizeContent);
    virtual void  Measure(SIZE* pSize);
    virtual void  Release();

private:
    UI::IListCtrlBase*  m_pIListCtrlBase;
};

