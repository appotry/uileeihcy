#pragma  once

//////////////////////////////////////////////////////////////////////////
//
//                             �б��ؼ�
//
//////////////////////////////////////////////////////////////////////////

namespace UI
{
	class ListCtrlBase;

	// �б������ݵ��ͷŷ�ʽ
// 	enum LIST_ITEM_DATA_DELETE_TYPE   
// 	{
// 		LIST_ITEM_DATA_DELETE_TYPE_NONE,
// 		LIST_ITEM_DATA_DELETE_TYPE_DELETE,
// 		LIST_ITEM_DATA_DELETE_TYPE_DELETE_ARRAY,
// 	};

	// �б���������б���С�Ĺ�ϵ
// 	enum LIST_CTRL_CONTENT_SIZE_TYPE 
// 	{
// 		LIST_CTRL_CONTENT_SIZE_NONE,  // 1. �ؼ���С�������޹�ϵ�������������������б��
// 		LIST_CTRL_CONTENT_2_SIZE,     // 2. ���ݾ����ؼ���С������˵��͵���ʽ�б��
// 		LIST_CTRL_SIZE_2_CONTENT,     // 3. �ؼ���С�������ݣ����粻��������������б��
// 	};

	//
	//	�б������ݣ��������б�����m_pData������ȥά��
	//
	class ListItemBase
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
		bool    IsDisable() { return m_bDisable; }
		bool    IsChecked() { return m_bChecked; }

		void*   GetData(){ return m_pData; }
		void    SetData(void* p) { m_pData = p; }
		void    SetDesiredSize(SIZE ps){ m_DesiredSize = ps; }
		SIZE    GetDesiredSize() { return m_DesiredSize; }
		int     GetLineIndex() { return m_nLineIndex; }
		void    SetLineIndex(int n) { m_nLineIndex = n; }

	protected:
		int            m_nLineIndex;      // ��¼����λ�ڵڼ���
//		int            m_nHeight;         // ���������ĸ߶ȣ�����ListBoxBase::m_bFixedItemHeight=false��Ч
		SIZE           m_DesiredSize;
		CRect          m_rcParent;        // �����б�ؼ���client ���� 

		ListItemBase*  m_pPrev;
		ListItemBase*  m_pNext;

		ListItemBase*  m_pNextSelection;  // ��һ����ѡ�еĶ���(�Լ��ѱ�ѡ��)
		ListItemBase*  m_pPrevSelection;  // ��һ����ѡ�еĶ���(�Լ��ѱ�ѡ��)

		bool           m_bDisable;        // �����Ƿ񱻽��ã���˵��
		bool           m_bChecked;        // �û��Ƿ񱻱�ǣ���˵��

		void*          m_pData;           // ��ÿһ��IListxxxRender��ʵ�����
		ListCtrlBase*  m_pCtrl;
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
		virtual  bool SetAttribute(ATTRMAP& mapAttrib, bool bReload);

		// �Լ���������鷽��
		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) = 0;
		virtual  SIZE OnMeasureItem( ListItemBase* p) = 0;
		virtual  void OnDeleteItem( ListItemBase* p ) = 0;

	public:
		// ���ýӿ�
		void    RemoveItem(int nIndex, bool bUpdate=true);
		void    RemoveAllItem();
		void    SetSortCompareProc( ListItemCompareProc p );
		int     GetItemCount() { return m_nItemCount; }
		void    SetFixedItemHeight(int nHeight, bool bUpdate=true);

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

		void    ReDrawItem( ListItemBase* pItem );
		bool    IsItemVisible( ListItemBase* pItem );
		bool    IsItemVisibleEx( ListItemBase* pItem, LISTITEM_VISIBLE_POS_TYPE& ePos );
		void    ItemRect2WindowRect( CRect* prc, CRect* prcRet );

		void    MeasureItem(ListItemBase* pItem);
		void    MeasureAllItem();
		int     GetMaxDisiredWidth(int* pDesiredHeight=NULL);

		// Selection �ӿ�

	protected:
		// ����
		int            m_nFixeddItemHeight;   // ���m_bFixedItemHeight=true���������ÿһ��ĸ߶�
		int            m_nItemGap;            // ��������֮��ļ��
		int            m_nItemCount;          // �б�������

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


	//////////////////////////////////////////////////////////////////////////
	// ϵͳ�б��ؼ�

	class ListBoxItemData
	{
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
			UIMSG_WM_RBUTTONDOWN(OnRButtonDown)
			UIMSG_WM_KEYDOWN(OnKeyDown)
			UICHAIN_MSG_MAP(ListCtrlBase)
		UI_END_MSG_MAP

	protected:
		virtual  bool SetAttribute(ATTRMAP& mapAttrib, bool bReload);

		virtual  void OnDrawItem( HRDC hRDC, ListItemBase* p ) ;
		virtual  SIZE OnMeasureItem( ListItemBase* p);
		virtual  void OnDeleteItem( ListItemBase* p );

		void     OnRButtonDown(UINT nFlags, CPoint point);
		void	 OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	public:
		bool     AddString(const String& strText, bool bUpdate=true);
		void     SetItemHeight(int nHeight);

		int      GetListBoxStyle();
		void     SetListBoxStyle(int n);

	protected:
		int      m_nItemHeight;
	};

	//////////////////////////////////////////////////////////////////////////
	// ģ��ǧǧ���������б���Ի�ؼ�

	class TTPlayerPlaylistItemData
	{
	public:
		String   m_strFilePath;
		String   m_strFileName;
		String   m_strFileTime;
	};
	class UIAPI TTPlayerPlaylistCtrl : public ListCtrlBase
	{
	public:
		UI_DECLARE_OBJECT( TTPlayerPlaylistCtrl, OBJ_CONTROL )
		
		void    AddFileItem(const String& strFilePath, bool bUpdate=true);

		virtual  void OnDrawItem(HRDC hRDC, ListItemBase* p);
		virtual  SIZE OnMeasureItem( ListItemBase* p);
		virtual  void OnDeleteItem( ListItemBase* p );
	};

	
}