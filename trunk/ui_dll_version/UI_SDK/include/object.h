#pragma once


namespace UI
{
 
typedef map<String,String>  ATTRMAP;

#define UI_DECLARE_OBJECT( className, type )             \
	static  String XmlName()                             \
	/*warning:�ú�����֧�ֶ�̬��������CLASS::XXX������*/ \
	{                                                    \
		String str =  _T(#className);                    \
		return str;                                      \
	}                                                    \
	virtual String GetObjectName()                       \
	{                                                    \
		return _T(#className);                           \
	}                                                    \
	virtual OBJ_TYPE GetObjectType()                     \
	{                                                    \
		return  type;                                    \
	}

#define UI_DECLARE_EMPTY_OBJECT()                        \
	/* ��������ʹ��UI_DECLARE_OBJECT��������*/           \
	static       String      XmlName(){return _T("Object"); }   \
	virtual      String      GetObjectName() = 0;        \
	virtual      OBJ_TYPE    GetObjectType() = 0;


// ��������
enum OBJ_TYPE
{
	OBJ_NULL,
	OBJ_CONTROL,
	OBJ_HWNDHOST,
	OBJ_PANEL,
	OBJ_WINDOW
};


//
// object��state bit��״̬λ
// 0000 0000 0000 0000
//
#define OSB_UNVISIBLE     0x0001        // �Լ��Ƿ�ɼ�
#define OSB_DISABLE       0x0002        // �Լ��Ƿ����

#define CSB_PREVENTREDRAW 0x0800        // ���ɽ����Ի�
#define CSB_DEFAULT       0x0400
#define CSB_FOCUS         0x0200
#define CSB_READONLY      0x0100
#define CSB_FORCEHOVER    0x0080         
#define CSB_FORCEPRESS    0x0040
#define CSB_HOVER         0x0020
#define CSB_PRESS         0x0010

#define WSB_ACTIVE        0x1000         // ����Ϊactive

// STYLE��ʽռλ˵��
//   31-24:  OBJECT ͨ����ʽ
//	 23-16:  CONTROLͨ����ʽ
//   15-1 :  ����ؼ��Լ�����ʽ
//      0 :  ��ʾ�ÿؼ�����

// object style
#define OBJECT_STYLE_TRANSPARENT        0x01000000    // �����Ƿ�͸������
#define OBJECT_STYLE_HSCROLL            0x02000000    // ����������
#define OBJECT_STYLE_VSCROLL            0x04000000    // �����������
#define OBJECT_STYLE_NCOBJ              0x08000000    // �ö���λ�ڸ�����ķǿͻ�����������ƫ��

// control style
#define CONTROL_STYLE_TABSTOP           0x00010000    // �Ƿ����TAB��ݼ��Ľ���
#define CONTROL_STYLE_GROUP             0x00020000    // �Ƿ���һ������Ŀ�ʼ

// button style
#define BUTTON_STYLE_MASK               0x000F    // ��ť����ʹ�õ�λ
#define BUTTON_STYLE_PUSHBUTTON         0x0000    // ��ͨ��ť
#define BUTTON_STYLE_RADIOBUTTON        0x0001    // ��ѡ��ť
#define BUTTON_STYLE_CHECKBUTTON        0x0002    // ��ѡ��ť
#define BUTTON_STYLE_HYPERLINK          0x0003    // �����Ӱ�ť
#define BUTTON_STYLE_MENUBUTTON         0x0004    // �˵���ť
#define BUTTON_STYLE_SPLITMENUBUTTON    0x0005    // ��ť+�˵�
#define BUTTON_STYLE_COMBOBOX           0x0006    // ��Ͽ��еİ�ť

// progress style
#define PROGRESS_STYLE_MASK             0x000F    // ����������ʹ�õ�λ
#define PROGRESS_STYLE_PROGRESS         0x0000    // ��ͨ������
#define PROGRESS_STYLE_SLIDER           0x0001    // ���϶���slider ctrl
#define PROGRESS_STYLE_SCROLLBAR        0x0002    // ��Ϊ��������һ����

// edit style
#define EDIT_STYLE_MASK                 0x000F    // �༭������ʹ�õ�λ
#define EDIT_STYLE_COMBOBOX             0x0001    // ��Ͽ���ʹ�õı༭��

// listctrl style
#define LISTCTRLBASE_ITEM_VARIABLE_HEIGHT 0x0010  // �б�ؼ���������߶Ȳ�����ȵ�
#define LISTCTRLBASE_SORT_ASCEND        0x0020    // ��������
#define LISTCTRLBASE_SORT_DESCEND       0x0040    // ��������
#define LISTCTRLBASE_MULTIPLE_SEL       0x0080    // �Ƿ�֧�ֶ�ѡ

// listbox style
#define LISTBOX_STYLE_MASK              0x000F    // �༭������ʹ�õ�λ
#define LISTBOX_STYLE_COMBOBOX          0x0001    // ��Ͽ���ʹ�õ��б��





// ����UI������ͳһ���� 
class UIAPI Object : public ObjTree<Object, Message>
{
public:
	Object(void);
	virtual ~Object(void) = 0;

	UI_DECLARE_EMPTY_OBJECT();

protected:
	void DestroyUI();
	
public:
	String   m_strID;          // �ö�����XML�еı�ʶ
protected:
	CRect    m_rcParent;       // �ö���ķ�Χ�������parent.����Window����ͻ�����λ�ã������Ͻ�Ϊ0��0
	
	CRegion4 m_rcNonClient;    // �����û�й�������Header������ռ�ÿռ�Ŀؼ���m_rcNonClient��Ϊm_rcPadding
	CRegion4 m_rcMargin;
	CRegion4 m_rcPadding;

	HRGN     m_hRgn;           // ����ö�����һ�����������򣬱������ø�ֵ����ֵ��window���Ͷ�����Ч. rgn������ڴ������Ͻǵġ�

	int      m_nConfigWidth;   // ����Ŀ�ȣ���ȡֵ�� ��ֵ | "auto" . ������window����width ��ָclient����Ĵ�С�������������ڵĴ�С��width����padding����������margin��
	int      m_nConfigHeight;  // ����ĸ߶ȣ���ȡֵ�� ��ֵ | "auto" . ������window����height��ָclient����Ĵ�С�������������ڵĴ�С��height����padding����������margin��
						       // ��������Ҫ˵�������������ռ�õĿ��= margin.left + width + margin.right
							   // Ҳ����˵�����width = padding.left + padding.right + content.width
// 	int     minwidth;
// 	int     minheight;
// 	int     maxwidth;
// 	int     maxheight;

	UINT     m_nStateBit;      // �����״̬����visible/disable
	UINT     m_nDropDragBit;

	UINT     m_nStyle;         // ��ͨ��ʽ
	UINT     m_nStyleEx;       // ��չ��ʽ

	RenderBase*          m_pBkgndRender;   // ������Ⱦ
	RenderBase*          m_pForegndRender; // ǰ����Ⱦ
	TextRenderBase*      m_pTextRender;    // ������Ⱦ
	UICursor*            m_pCursor;        // ����������ʽ
	ATTRMAP              m_mapAttribute;   // ʣ���������δ���������ԣ�������չ��TODO���Ż�

public:
	WindowBase*  GetWindowObject();
	HWND         GetHWND();
	Object*      FindChildObject( const String& objId);
//	Object*      GetChildObject( const String&  objIdPath );  -- ���ṩ����Ϊ������·�����ǹ̶���

//	void         UpdateRect();
	void         UpdateObject( bool bUpdateNow = true);
//	void         UpdateObject( RECT* prc, bool bUpdateNow = true);
	void         UpdateLayout( bool bUpdate );
//	int          GetParentList( vector<Object*>& vParents );  -- �ò��ϣ�ֱ��ʹ��EnumParentObject����

public:
	// �����������
	void         GetNonClientRegion( CRegion4* prc ) { prc->CopyRect(m_rcNonClient); }
	void         SetNonClientRegion( CRegion4* prc );
	int          GetNonClientL() { return m_rcNonClient.left; }
	int          GetNonClientT() { return m_rcNonClient.top; }
	int          GetNonClientR() { return m_rcNonClient.right; }
	int          GetNonClientB() { return m_rcNonClient.bottom; }
	int          GetNonClientW() { return m_rcNonClient.left + m_rcNonClient.right; }
	int          GetNonClientH() { return m_rcNonClient.top + m_rcNonClient.bottom; }
	void         GetPaddingRegion( CRegion4* prc ) { prc->CopyRect(&m_rcPadding); }
	void         SetPaddingRegion( CRegion4* prc );
	int          GetPaddingL() { return m_rcPadding.left; }
	int          GetPaddingT() { return m_rcPadding.top; }
	int          GetPaddingR() { return m_rcPadding.right; }
	int          GetPaddingB() { return m_rcPadding.bottom; }
	int          GetPaddingW() { return m_rcPadding.left + m_rcPadding.right; }
	int          GetPaddingH() { return m_rcPadding.top + m_rcPadding.bottom; }
	void         GetMarginRegion( CRegion4* prc) { prc->CopyRect(&m_rcMargin); }
	int          GetMarginL() { return m_rcMargin.left; }
	int          GetMarginT() { return m_rcMargin.top; }
	int          GetMarginR() { return m_rcMargin.right; }
	int          GetMarginB() { return m_rcMargin.bottom; }
	int          GetMarginW() { return m_rcMargin.left + m_rcMargin.right; }
	int          GetMarginH() { return m_rcMargin.top + m_rcMargin.bottom; }
	void         GetClientRect( CRect* prc );
	void         GetClientRectAsWin32( CRect* prc );
	void         GetParentRect( CRect* prc );
	int          GetParentRectL() { return m_rcParent.left; }
	int          GetParentRectT() { return m_rcParent.top; }
	int          GetParentRectR() { return m_rcParent.right; }
	int          GetParentRectB() { return m_rcParent.bottom; }

	int          GetWidth();
	int          GetHeight();
	int          GetConfigWidth() { return m_nConfigWidth; }
	int          GetConfigHeight() { return m_nConfigHeight; }
	void         SetObjectPos( int x, int y, int cx, int cy, int nFlag=0 );
	void         SetObjectPos( CRect* prc, int nFlag=0 );
	HRGN         GetRgn() { return m_hRgn; }
	HRFONT       GetFont();
	RenderBase*  GetBkRender() { return m_pBkgndRender; }
	RenderBase*  GetForeRender() { return m_pForegndRender; }
	void         SetBkRender(RenderBase* p);
	void         SetForeRender(RenderBase* p);

	POINT        GetRealPosInWindow();
	void         GetWindowRect(CRect* lprc);
	void         WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj);
	void         WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	void         ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	bool         GetScrollOffset(int* pxOffset, int* pyOffset);
	
	// ��ʽ����
	int          GetStateBit() { return m_nStateBit; }
	bool         IsFocus();
	void         SetFocus( bool b );
	bool         IsTabstop();
	bool         IsGroup();
	void         SetTabstop( bool b );
	void         SetGroup( bool b );
	bool         IsVisible();
	bool         IsCollapsed();
	bool         IsMySelfVisible();
	bool         IsEnable();
	void         SetVisible( bool b, bool bUpdateNow=true );
	void         SetEnable( bool b );
	bool         IsDefault();
	bool         IsReadonly();
	bool         IsHover();
	bool         IsPress();
	void         SetDefault( bool b );
	void         SetReadonly( bool b );
	void         SetForceHover( bool b );
	void         SetForcePress( bool b );
	void         SetHover( bool b );
	void         SetPress( bool b );
	void         ModifyStyle( UINT add, UINT remove = 0 );
	bool         TestStyle(UINT nStyle){ if(m_nStyle&nStyle)return true; return false; }
	bool         IsTransparent();
	void         SetActive(bool b);
	bool         IsActive();
	void         SetAsNcObject() { this->ModifyStyle(OBJECT_STYLE_NCOBJ); } // ObjTree��������
	bool         IsNcObject() { return TestStyle(OBJECT_STYLE_NCOBJ); }     // ObjTree��������

	// ��չ���Բ���
	void         EraseAttribute(const String& strKey) { m_mapAttribute.erase(strKey); }
	void         AddAttribute(const String& strKey, const String& strValue) { m_mapAttribute[strKey] = strValue; }
	bool         GetAttribute(const String& strKey, String& strValue);
	
	// ����
	void         DrawObject(HRDC hRDC, RenderOffsetClipHelper roc);   
	void         DrawObjectTransparentBkgnd(HRDC hRDC, RenderOffsetClipHelper& roc);

	// ����
	HRGN         GetClipRgnInWindow();
	void         SetReDraw( bool bReDraw );

private:
	Object*      _findChildObjectItem( const String& objId );
	void         _drawChildObject(HRDC hRDC, RenderOffsetClipHelper& roc);
	void         _drawNcChildObject(HRDC hRDC, RenderOffsetClipHelper& roc);
public:

	// �ڲ��ֹ����з����Լ�����Ҫ�Ĵ�С(����padding+margin)��GetDesiredSize�������б��ֲ�ͬ��
	// ����Ϊ�� ��Panel�У���ȥ���� m_pLayout->Measure����LayoutManager �ֻ�ȥ�ٵ���LayoutManager�����MeasureChildObject�麯��
	//          ��Control�У���ȥ����GetAutoSize�麯��
	virtual      SIZE        GetDesiredSize( HRDC hDC ) = 0;  

	// ��xml�õ�����ת�ɶ�������ԣ�ע�⣬�������ظú���ʱ�������ȵ��ø���ĸ÷���
	// bReload��ʾΪ����ʱ���ã���ʱ�����ٸ�ĳЩ���Ը�ֵ������text����
	virtual      bool        SetAttribute(ATTRMAP& mapAttrib, bool bReload);

	// �ڳ�ʼ��Object�ͻ���ʱ���������ú�������Ϊ����ʱ��������������Դ�������ResetAttribute��Ҫ�ͷ�������Դ����
	// ע�⣬�������ظú���ʱ�������ȵ��ø���ĸ÷���
	virtual      void        ResetAttribute();

public:
	bool   SetChildObjectAttribute( Object* pChildObj, const String& strPrifix, map<String,String>& mapAttrib, bool bReload );

public:
	// ��Ϣ����
	UI_BEGIN_MSG_MAP
		UIMSG_WM_SETCURSOR( OnSetCursor )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
		UIMSG_WM_HITTEST( OnHitTest )
		UIMSG_WM_GETFONT( OnGetFont )
	UI_END_MSG_MAP

protected:
	BOOL    OnEraseBkgnd( HRDC );
	BOOL    OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
	HRFONT  OnGetFont();
	UINT    OnHitTest( POINT* pt );

	inline  bool testStateBit( UINT bit );
	inline  void setStateBit( UINT bit );
	inline  void clearStateBit( UINT bit );

};


}
