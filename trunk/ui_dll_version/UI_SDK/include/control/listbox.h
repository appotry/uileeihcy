#pragma  once

//////////////////////////////////////////////////////////////////////////
//
//                             �б��ؼ�
//
//////////////////////////////////////////////////////////////////////////

namespace UI
{

	//
	//	�б������ݣ��������б�����m_pData������ȥά��
	//
	class ListItemBase
	{
	public:
		ListItemBase();
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
		bool    IsDisable() { return m_bDisable; }
		bool    IsChecked() { return m_bChecked; }
		int     GetDesiredHeight() { return m_nHeight; }

	protected:
		int            m_nHeight;         // ���������ĸ߶ȣ�����ListBoxBase::m_bFixedItemHeight=false��Ч
		CRect          m_rcParent;        // �����б�ؼ���client ���� 

		ListItemBase*  m_pPrev;
		ListItemBase*  m_pNext;

		ListItemBase*  m_pNextSelection;  // ��һ����ѡ�еĶ���(�Լ��ѱ�ѡ��)
		ListItemBase*  m_pPrevSelection;  // ��һ����ѡ�еĶ���(�Լ��ѱ�ѡ��)

		bool           m_bDisable;        // �����Ƿ񱻽��ã���˵��
		bool           m_bChecked;        // �û��Ƿ񱻱�ǣ���˵��
	};

	class TreeListItemBase : public ListItemBase
	{
	public:
		TreeListItemBase();
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
	enum LISTITEM_SORT_TYPE{ 
		LISTITEM_SORT_DISABLE,            // ������
		LISTITEM_SORT_ASCEND,             // ����
		LISTITEM_SORT_DESCEND             // ����
	};
	enum LISTITEM_VISIBLE_POS_TYPE{
		LISTITEM_UNVISIBLE_TOP = 1,      
		LISTITEM_VISIBLE_COVERTOP,       
		LISTITEM_VISIBLE,
		LISTITEM_VISIBLE_COVERBOTTOM,
		LISTITEM_UNVISIBLE_BOTTOM,
	};

// 	class IScrollObject
// 	{
// 	public:
// 		virtual void SetScrollOffset(int x, int y) = 0;
// 		virtual void GetScrollOffset(int* px, int* py) = 0;
// 		virtual void SetScrollSize(int nxSize, int nySize) = 0;
// 		virtual void SetScrollPage(int nxPage, int nyPage) = 0;
// 		virtual void GetScrollClientRect(RECT* prc) = 0;
// 	};

	class UIAPI ListBoxBase : public Control//, public IScrollObject
	{
	public:
		ListBoxBase();
		~ListBoxBase();

		UI_BEGIN_MSG_MAP
			UIMSG_WM_MOUSEMOVE(OnMouseMove)
			UIMSG_WM_PAINT(OnPaint)
			UIMSG_WM_MOUSELEAVE(OnMouseLeave)
			UIMSG_WM_LBUTTONDOWN(OnLButtonDown)
			UIMSG_WM_LBUTTONUP(OnLButtonUp)
			UIMSG_WM_KEYDOWN(OnKeyDown)
			UIMSG_WM_SIZE(OnSize)
			UICHAIN_MSG_MAP_MEMBER(m_MgrScrollbar)
			UICHAIN_MSG_MAP(Control)
		UI_END_MSG_MAP

	protected:
		// ��Ϣ����
		void     OnPaint(HRDC hRDC);
		void     OnMouseMove(UINT nFlags, POINT point);
		void     OnMouseLeave();
		void     OnLButtonDown(UINT nFlags, POINT point);
		void     OnLButtonUp(UINT nFlags, POINT point);
		void     OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
		void     OnSize( UINT nType, int cx, int cy );

	public:
		// �麯��
		virtual  SIZE GetAutoSize( HRDC hRDC );
		virtual  void ResetAttribute();
		virtual  bool SetAttribute(map<String,String>& mapAttrib, bool bReload);

		// �Լ���������鷽��
		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ){};

	public:
		// ���ýӿ�
		void    RemoveItem(int nIndex, bool bUpdate=true);
		void    RemoveAllItem();
		void    SetSort( LISTITEM_SORT_TYPE eSortType, ListItemCompareProc p );
		int     GetItemCount() { return m_nItemCount; }
		void    SetFixedItemHeight(int nHeight, bool bUpdate=true);

		ListItemBase* HitTest(POINT ptWindow);
		ListItemBase* Index2Item(int nIndex);

	protected:
		// ����ӿ�
		void    AddItem( ListItemBase*  pItem );
		void    InsertItem( ListItemBase*  pItem, ListItemBase* pInsertAfter );
		void    RemoveItem(ListItemBase* pItem, bool bUpdate=true);
		
		void    UpdateItemRect( ListItemBase* pStart );
		void    SetSelectedItem(ListItemBase* pItem, bool& bNeedUpdateObject );

		void    SetScrollY( int nY, bool& bNeedUpdateObject );
		void    SetScrollX( int nX, bool& bNeedUpdateObject );
		void    SetScrollPoint( int nX, int nY, bool& bNeedUpdateObject );

		void    ReDrawItem( ListItemBase* pItem );
		bool    IsItemVisible( ListItemBase* pItem );
		bool    IsItemVisibleEx( ListItemBase* pItem, LISTITEM_VISIBLE_POS_TYPE& ePos );
		void    ItemRect2WindowRect( CRect* prc, CRect* prcRet );
		// Selection �ӿ�

	protected:
		// ����
		bool           m_bFixedItemHeight;    // ÿһ���и��Ƿ�һ��, TODO: �ĳ�m_nStyle��ʵ�� 

		int            m_nFixeddItemHeight;   // ���m_bFixedItemHeight=true���������ÿһ��ĸ߶�
		int            m_nItemGap;            // ��������֮��ļ��
		
		LISTITEM_SORT_TYPE   m_eSortType;     // ����ʽ
		ListItemCompareProc  m_pCompareProc;  // ������

		int            m_nItemCount;          // �б�������

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


	class ListBoxItem : public ListItemBase
	{
	public:
		String   m_strText;
	};
	class UIAPI ListBox : public ListBoxBase/*ListBoxBase_Scrollbar*/
	{
	public:
		ListBox();
		~ListBox();

		UI_DECLARE_OBJECT( ListBox, OBJ_CONTROL )

		UI_BEGIN_MSG_MAP
			UIMSG_WM_RBUTTONDOWN(OnRButtonDown)
			UICHAIN_MSG_MAP(ListBoxBase)
		UI_END_MSG_MAP

	protected:
		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) ;

		void   OnRButtonDown(UINT nFlags, CPoint point);

	public:
		bool   AddString(const String& strText, bool bUpdate=true);

		
	};

	
}