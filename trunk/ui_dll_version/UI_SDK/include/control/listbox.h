#pragma  once

//////////////////////////////////////////////////////////////////////////
//
//                             �б��ؼ�
//
//////////////////////////////////////////////////////////////////////////

namespace UI
{
	class ListCtrlBase;

	//
	//	�б������ݣ��������б�����m_pData������ȥά��
	//
	class UIAPI ListItemBase
	{
	public:
		ListItemBase(ListCtrlBase* pCtrl);
		virtual ~ListItemBase();

	public:
		ListItemBase*    GetNextItem() { return m_pNext; }
		ListItemBase*    GetPrevItem() { return m_pPrev; }
		ListItemBase*    GetPrevSelection() { return m_pPrevSelection; }
		ListItemBase*    GetNextSelection() { return m_pNextSelection; }
		void    SetNextItem(ListItemBase* p) { m_pNext = p; }
		void    SetPrevItem(ListItemBase* p) { m_pPrev = p; }
		void    SetNextSelection(ListItemBase* p){ m_pNextSelection = p; }
		void    SetPrevSelection(ListItemBase* p){ m_pPrevSelection = p; }

		CRect   GetParentRect() { return m_rcParent; }
		void    GetParentRect(CRect* prc){ prc->CopyRect(&m_rcParent); }
		void    SetParentRect(CRect* prc){ m_rcParent.CopyRect(prc); }

		void*   GetData(){ return m_pData; }
		void    SetData(void* p) { m_pData = p; }
		void    SetDesiredSize(SIZE ps){ m_DesiredSize = ps; }
		SIZE    GetDesiredSize() { return m_DesiredSize; }
		int     GetLineIndex() { return m_nLineIndex; }
		void    SetLineIndex(int n) { m_nLineIndex = n; }

	public:
		virtual  bool    OnMouseMove(POINT pt, UINT nFlag);
		virtual  bool    OnMouseEnter();
		virtual  bool    OnMouseLeave();

		virtual  bool    OnLButtonDown(POINT pt, UINT nFlag);
		virtual  bool    OnRButtonDown(POINT pt, UINT nFlag);
		virtual  bool    OnLButtonUp(POINT pt, UINT nFlag);
		virtual  bool    OnRButtonUp(POINT pt, UINT nFlag);
		virtual  bool    OnLButtonDBClick(POINT pt, UINT nFlag);
		virtual  bool    OnRButtonDBClick(POINT pt, UINT nFlag);

		virtual  void    OnDrawItem(HRDC hRDC) {}
		virtual  bool    IsDisable() { return false; }  // ������ȥ��������ʵ��(����˵�����MF_DISABLE�������Ŀ�����m_bDisable)

	protected:
		int            m_nLineIndex;      // ��¼����λ�ڵڼ���
//		int            m_nHeight;         // ���������ĸ߶ȣ�����ListBoxBase::m_bFixedItemHeight=false��Ч
		SIZE           m_DesiredSize;
		CRect          m_rcParent;        // �����б�ؼ���client ���� 

		ListItemBase*  m_pPrev;
		ListItemBase*  m_pNext;

		ListItemBase*  m_pNextSelection;  // ��һ����ѡ�еĶ���(�Լ��ѱ�ѡ��)
		ListItemBase*  m_pPrevSelection;  // ��һ����ѡ�еĶ���(�Լ��ѱ�ѡ��)

		void*          m_pData;           
		ListCtrlBase*  m_pListCtrl;
	};

	class TreeListItemBase : public ListItemBase
	{
	public:
		TreeListItemBase(ListCtrlBase* pCtrl);
		~TreeListItemBase();

		ListItemBase*    GetParentItem() { return m_pParent; }
		ListItemBase*    GetChildItem()  { return m_pChild; }
		void    SetParentItem(TreeListItemBase* p) { m_pParent = p; }
		void    SetChildItem(TreeListItemBase* p)  { m_pChild = p; }

	protected:
		TreeListItemBase*  m_pParent;
		TreeListItemBase*  m_pChild;
	};


	typedef int (*ListItemCompareProc)(ListItemBase* p1, ListItemBase* p2);
// 	enum LISTITEM_SORT_TYPE{ 
// 		LISTITEM_SORT_DISABLE,            // ������
// 		LISTITEM_SORT_ASCEND,             // ����
// 		LISTITEM_SORT_DESCEND             // ����
// 	};
	enum LISTITEM_VISIBLE_POS_TYPE{
		LISTITEM_UNVISIBLE_TOP = 1,      
		LISTITEM_VISIBLE_COVERTOP,       
		LISTITEM_VISIBLE,
		LISTITEM_VISIBLE_COVERBOTTOM,
		LISTITEM_UNVISIBLE_BOTTOM,
	};

	class UIAPI ListCtrlBase : public Control//, public IScrollObject
	{
	public:
		ListCtrlBase();
		~ListCtrlBase();

		void InitialRelease();
	public:

		UI_BEGIN_MSG_MAP
			UIMSG_WM_MOUSEMOVE(OnMouseMove)
			UIMSG_WM_PAINT(OnPaint)
			UIMSG_WM_MOUSELEAVE(OnMouseLeave)
			UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
			UIMSG_WM_LBUTTONUP(OnLButtonUp)
			UIMSG_WM_LBUTTONDBLCLK(OnDBClick)
			UIMSG_WM_KEYDOWN(OnKeyDown)
			UIMSG_WM_SIZE(OnSize) 
			UIMSG_WM_GETRENDERTYPE(OnGetRenderType)
			UICHAIN_MSG_MAP_MEMBER(m_MgrScrollbar)
		UI_END_MSG_MAP_CHAIN_PARENT(Control)

	protected:
		// ��Ϣ����
		void     OnPaint(HRDC hRDC);
		void     OnMouseMove(UINT nFlags, POINT point);
		void     OnMouseLeave();
		void     OnLButtonDown(UINT nFlags, POINT point);
		void     OnLButtonUp(UINT nFlags, POINT point);
		void     OnDBClick(UINT nFlags, POINT point);
		void     OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
		void     OnSize( UINT nType, int cx, int cy );
		LRESULT  OnGetRenderType(){return GRAPHICS_RENDER_TYPE_GDI;}  // ����popup listbox��ȡ����
	public:
		// �麯��
		virtual  SIZE GetAutoSize( HRDC hRDC );
		virtual  void ResetAttribute();
		virtual  bool SetAttribute(ATTRMAP& mapAttrib, bool bReload);

		// �Լ���������鷽��
		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) = 0;
		virtual  SIZE OnMeasureItem( ListItemBase* p) = 0;
		virtual  void OnDeleteItem( ListItemBase* p ){};

	public:
		// ���ýӿ�
		void    RemoveItem(int nIndex, bool bUpdate=true);
		void    RemoveAllItem();
		void    SetSortCompareProc( ListItemCompareProc p );
		int     GetItemCount() { return m_nItemCount; }
		ListItemBase* GetItemBase(int i);
		ListItemBase* GetFirstItem() { return m_pFirstItem; }
		ListItemBase* GetLastItem() { return m_pLastItem; }
		void    SetItemHeight(int nHeight, bool bUpdate=true);

		ListItemBase* HitTest(POINT ptWindow);
		ListItemBase* Index2Item(int nIndex);

		void    UpdateItemRect( ListItemBase* pStart );

		ScrollBarMgr* GetScrollBarMgr() { return &m_MgrScrollbar; }

	protected:
		// ����ӿ�
		void    AddItem(ListItemBase*  pItem, bool bUpdate=true);
		void    InsertItem(ListItemBase*  pItem, ListItemBase* pInsertAfter);
		void    RemoveItem(ListItemBase* pItem, bool bUpdate=true);
		void    SetSelectedItem(ListItemBase* pItem, bool& bNeedUpdateObject );

		void    SetScrollY( int nY, bool& bNeedUpdateObject );
		void    SetScrollX( int nX, bool& bNeedUpdateObject );
		void    SetScrollPoint( int nX, int nY, bool& bNeedUpdateObject );

		void    ReDrawItem(ListItemBase* pItem, ListItemBase* pItem2 = NULL);
		bool    IsItemVisible(ListItemBase* pItem);
		bool    IsItemVisibleEx(ListItemBase* pItem, LISTITEM_VISIBLE_POS_TYPE& ePos);
		void    ItemRect2WindowRect(CRect* prc, CRect* prcRet);
		void    MakeItemVisible(ListItemBase* pItem, bool& bNeedUpdateObject);

		void    MeasureItem(ListItemBase* pItem);
		void    MeasureAllItem();
		int     GetMaxDisiredWidth(int* pDesiredHeight=NULL);

		// Selection �ӿ�
		void    SetHoverItem(ListItemBase* pItem);
		void    SetPressItem(ListItemBase* pItem, const POINT& pt, UINT nFlags);

	protected:
		// ����
		int            m_nItemGap;            // ��������֮��ļ��
		int            m_nItemCount;          // �б�������
		int            m_nItemHeight;

		ListItemCompareProc  m_pCompareProc;  // ������

		// ���ݵ���
		ListItemBase*  m_pFirstItem;
		ListItemBase*  m_pLastItem;
		
		ListItemBase*  m_pFirstVisibleItem;
		ListItemBase*  m_pLastVisibleItem;

		ListItemBase*  m_pFirstSelectedItem;

		ListItemBase*  m_pHoverItem;
		ListItemBase*  m_pPressItem;

		ScrollBarMgr   m_MgrScrollbar;  
	};


	// ֧����һ������������Ԫ�أ���ť��COMBOBOX��Progress)���б�ؼ�
	class AdvListCtrlBase : public ListItemBase
	{
	public:

	};


	//////////////////////////////////////////////////////////////////////////
	// ϵͳ�б��ؼ�

	class ListBoxItem : public ListItemBase
	{
	public:
		ListBoxItem(ListCtrlBase* pCtrl):ListItemBase(pCtrl){};

	public:
		String   m_strText;
	};
	class UIAPI ListBox : public ListCtrlBase
	{
	public:
		ListBox();
		~ListBox();

		UI_DECLARE_OBJECT( ListBox, OBJ_CONTROL )

		UI_BEGIN_MSG_MAP
			UIMSG_WM_LBUTTONUP(OnLButtonUp)
			UIMSG_WM_INITPOPUPCONTROLWINDOW(OnInitPopupControlWindow)
			UIMSG_WM_UNINITPOPUPCONTROLWINDOW(OnUnInitPopupControlWindow)
		UI_END_MSG_MAP_CHAIN_PARENT(ListCtrlBase)

	public:
		virtual  void ResetAttribute();
		virtual  bool SetAttribute(ATTRMAP& mapAttrib, bool bReload);

	protected:
		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) ;
		virtual  SIZE OnMeasureItem( ListItemBase* p);

		void     OnLButtonUp(UINT nFlags, CPoint point);
		void     OnInitPopupControlWindow(Object* pObjMsgFrom);
		void     OnUnInitPopupControlWindow(Object* pObjMsgFrom);

	public:
		ListBoxItem*  AddString(const String& strText, bool bUpdate=true);

		int      GetListBoxStyle();
		void     SetListBoxStyle(int n);
		void     SetBindObject(Object* pCombobox);

		void     DropDown();
		void     CloseUp();

	protected:
		Object*  m_pBindObject;   // �����combobox���������б��m_pBindObject��ָ��Combobox*

		PopupListBoxWindow* m_pPopupWrapWnd;
	};
}