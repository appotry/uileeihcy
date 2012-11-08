#pragma once


namespace UI
{
 
#define UI_DECLARE_OBJECT( className, type )             \
	static  TCHAR* XmlName()                             \
	/*warning:�ú�����֧�ֶ�̬��������CLASS::XXX������*/ \
	{                                                    \
		return _T(#className);                           \
	}                                                    \
	virtual TCHAR* GetObjectName()                       \
	{                                                    \
		return _T(#className);                           \
	}                                                    \
	virtual OBJ_TYPE GetObjectType()                     \
	{                                                    \
		return  type;                                    \
	}                                                    \
	typedef UIObjCreator<className> _CreatorClass; 

// ע�����ﲻ��ʹ��String����TCHAR*���ء���Ϊһ�������������
//     ����DLL�м���Objectʵ�ֵģ���UIDLL���ٵ�����GetObjectName
//	   �ᵼ��heap exception.
#define UI_DECLARE_EMPTY_OBJECT()                        \
	/* ��������ʹ��UI_DECLARE_OBJECT��������*/           \
	static       TCHAR*      XmlName(){return _T("Object"); }   \
	virtual      TCHAR*      GetObjectName() = 0;        \
	virtual      OBJ_TYPE    GetObjectType() = 0;


// ���ؼ�����һ��renderbase����ʱ����Ӧ��Ӧonthemechanged��Ϣ��Ȼ����øú�
#define ON_RENDER_THEME_CHANGED(pRender)  \
	if (NULL != pRender && pRender->IsThemeRender())  \
	{ \
		ThemeRenderBase* p = dynamic_cast<ThemeRenderBase*>(pRender); \
		p->OnThemeChanged(); \
	}

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
//	 23-16:  CONTROL/WINDOWͨ����ʽ
//   15-1 :  ����ؼ��Լ�����ʽ
//      0 :  ��ʾ�ÿؼ�����

// object style
#define OBJECT_STYLE_TRANSPARENT        0x01000000    // �����Ƿ�͸������
#define OBJECT_STYLE_HSCROLL            0x02000000    // ����������
#define OBJECT_STYLE_VSCROLL            0x04000000    // �����������
#define OBJECT_STYLE_NCOBJ              0x08000000    // �ö���λ�ڸ�����ķǿͻ�����������ƫ��

// window style
#define WINDOW_STYLE_DESTROYED          0x00010000    // ��ʾ�ô����Ѿ���������(WM_NCDESTROY)�����ڴ���OnFinalMessage

// control style
#define CONTROL_STYLE_TABSTOP           0x00010000    // �Ƿ����TAB��ݼ��Ľ���
#define CONTROL_STYLE_GROUP             0x00020000    // �Ƿ���һ������Ŀ�ʼ

// button style
#define BUTTON_STYLE_MASK               0x000F    // ��ť����ʹ�õ�λ
#define BUTTON_STYLE_PUSHBUTTON         0x0000    // ��ͨ��ť
#define BUTTON_STYLE_HAVE_TEXT_FIRST    BUTTON_STYLE_PUSHBUTTON  // ��־�����ݵİ�ť����Щ��ť����Ҫ��������textrender
#define BUTTON_STYLE_RADIOBUTTON        0x0001    // ��ѡ��ť
#define BUTTON_STYLE_CHECKBUTTON        0x0002    // ��ѡ��ť
#define BUTTON_STYLE_HYPERLINK          0x0003    // �����Ӱ�ť
#define BUTTON_STYLE_MENUBUTTON         0x0004    // �˵���ť
#define BUTTON_STYLE_SPLITMENUBUTTON    0x0005    // ��ť+�˵�
#define BUTTON_STYLE_HAVE_TEXT_LAST    BUTTON_STYLE_SPLITMENUBUTTON
#define BUTTON_STYLE_COMBOBOX           0x0006    // ��Ͽ��еİ�ť
#define BUTTON_STYLE_SCROLLLINEUP       0x0007    // ��������ť
#define BUTTON_STYLE_SCROLLLINEDOWN     0x0008
#define BUTTON_STYLE_SCROLLLINELEFT     0x0009
#define BUTTON_STYLE_SCROLLLINERIGHT    0x000A
#define BUTTON_STYLE_HSCROLLTHUMB       0x000B    // ��������ק��ť
#define BUTTON_STYLE_VSCROLLTHUMB       0x000C    // ��������ק��ť
#define BUTOTN_STYLE_SLIDERTRACKBUTTON  0x000D    // ����������İ�ť

// progress style
#define PROGRESS_STYLE_MASK             0x000F    // ����������ʹ�õ�λ
#define PROGRESS_STYLE_PROGRESS         0x0000    // ��ͨ������
#define PROGRESS_STYLE_SLIDER           0x0001    // ���϶���slider ctrl

#define SLIDER_STYLE_POINT_LEFT         0x0100    // ���鰴ť��ͷָ����� Ĭ��Ϊ��ָ��
#define SLIDER_STYLE_POINT_TOP          0x0100    // ���鰴ť��ͷָ���ϲ�
#define SLIDER_STYLE_POINT_RIGHT        0x0200    // ���鰴ť��ͷָ���Ҳ�
#define SLIDER_STYLE_POINT_BOTTOM       0x0200    // ���鰴ť��ͷָ���²�
#define SLIDER_STYLE_SHOWTICKS          0x0400    // ��ʾ�̶ȣ�Ĭ��ֻ��ʾ��һ�������һ��ֵ�Ŀ̶�
#define SLIDER_STYLE_AUTOTICKS          0x0800    // �Զ���ʾ�̶ȣ�Ĭ��ÿ��һ��ֵ��ʾһ���̶�

// edit style
#define EDIT_STYLE_MASK                 0x000F    // �༭������ʹ�õ�λ
#define EDIT_STYLE_COMBOBOX             0x0001    // ��Ͽ���ʹ�õı༭��

// listctrl style
//#define LISTCTRLBASE_ITEM_VARIABLE_HEIGHT 0x0010  // �б�ؼ���������߶Ȳ�����ȵ�
#define LISTCTRLBASE_SORT_ASCEND        0x0020    // ��������
#define LISTCTRLBASE_SORT_DESCEND       0x0040    // ��������
#define LISTCTRLBASE_MULTIPLE_SEL       0x0080    // �Ƿ�֧�ֶ�ѡ
#define LISTCTRLBASE_SELECT_AS_HOVER_MODE  0x0100 // ���絯��ʽlistbox��1.��갴��ʱ��ѡ����굯��ʱѡ�� 2. ��û��hoverʱ��ʾsel 3. ����hoverʱ��ʾhover 4. ����Ƴ��ؼ�ʱ��ȡ��hover
#define LISTCTRLBASE_SIZE_2_CONTENT     0x0200    // �ؼ���С�������ݣ����粻��������������б��
#define LISTCTRLBASE_CONTENT_2_SIZE     0x0400    // ���ݾ����ؼ���С������˵��͵���ʽ�б��

// listbox style
#define LISTBOX_STYLE_MASK              0x000F    // �༭������ʹ�õ�λ
#define LISTBOX_STYLE_COMBOBOX          0x0001    // ��Ͽ���ʹ�õ��б��

// menu style
#define MENU_STYLE_AUTO_DELETE_SUBMENU  0x1000    // ����������ʱ���Ƿ��Զ��ͷ��Ӳ˵��ڴ档��Ҫ���ڴ�xml��load�Ĳ˵�
                                                  // ��Ϊ���ڲ��������Ӳ˵����ڲ����������Ӳ˵������ⲿ�Լ�create����
                                                  // �Ĳ˵��Լ�Ҫȥ�ٴ����Ӳ˵�������ⲿ�Լ�ȥ�ͷ��ڴ�

// combobox style
#define COMBOBOX_STYLE_DROPDOWN         0x0010    // �ɱ༭�����б�
#define COMBOBOX_STYLE_DROPDOWNLIST     0x0020    // ���ɱ༭�����б�

// ����UI������ͳһ���� 
class UIAPI Object : public ObjTree<Object, Message>
{
public:
	Object(void);
	virtual ~Object(void) = 0;

	UI_DECLARE_EMPTY_OBJECT();
	virtual HRESULT InitialConstruct(){return S_OK;}   // ������ʵ�ָ��麯�������ȵ��ø����ʵ��
	virtual HRESULT FinalConstruct(){return S_OK;}     // ������ʵ�ָ��麯��ʱ���ص��ø����ʵ��
	virtual void    InitialRelease();                  // ������ʵ�ָ��麯�������ȵ��ø����ʵ��
	virtual void    FinalRelease(){};                  // ������ʵ�ָ��麯��ʱ���ص��ø����ʵ��
	virtual void    ObjectMustCreateByUIObjCreator() = 0;   // ����һ���麯�����ú�����UIObjCreatorʵ�֡�����������ֱ�ӹ���ʱ������

protected:
	virtual void    DestroyUI();
	
public:
	String   m_strID;          // �ö�����XML�еı�ʶ
protected:
	CRect    m_rcParent;       // �ö���ķ�Χ�������parent.����Window����ͻ�����λ�ã������Ͻ�Ϊ0��0
	
	CRegion4 m_rcNonClient;    // ���û�й�������Header������ռ�ÿռ�Ŀؼ���m_rcNonClient��Ϊm_rcPadding+m_rcBorder
	CRegion4 m_rcMargin;
	CRegion4 m_rcPadding;
	CRegion4 m_rcBorder;       

	HRGN     m_hRgn;           // ��δʹ�ã�����ö�����һ�����������򣬱������ø�ֵ����ֵ��window���Ͷ�����Ч. rgn������ڴ������Ͻǵġ�

	int      m_nConfigWidth;   // ����Ŀ�ȣ���ȡֵ�� ��ֵ | "auto" . ������window����width ��ָclient����Ĵ�С�������������ڵĴ�С��width����padding����������margin��
	int      m_nConfigHeight;  // ����ĸ߶ȣ���ȡֵ�� ��ֵ | "auto" . ������window����height��ָclient����Ĵ�С�������������ڵĴ�С��height����padding����������margin��
						       // ��������Ҫ˵�������������ռ�õĿ��= margin.left + width + margin.right
							   // Ҳ����˵�����width = padding.left + padding.right + content.width

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
	void         UpdateObjectBkgnd( bool bUpdateNow = true );
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
	void         GetBorderRegion( CRegion4* prc ) { prc->CopyRect(&m_rcBorder); }
	void         SetBorderRegion( CRegion4* prc );
	void         GetPaddingRegion( CRegion4* prc ) { prc->CopyRect(&m_rcPadding); }
	void         SetPaddingRegion( CRegion4* prc );
	int          GetPaddingL() { return m_rcPadding.left; }
	int          GetPaddingT() { return m_rcPadding.top; }
	int          GetPaddingR() { return m_rcPadding.right; }
	int          GetPaddingB() { return m_rcPadding.bottom; }
	int          GetPaddingW() { return m_rcPadding.left + m_rcPadding.right; }
	int          GetPaddingH() { return m_rcPadding.top + m_rcPadding.bottom; }
	void         SetMarginRegion( CRegion4* prc) { m_rcMargin.CopyRect(prc); }
	void         GetMarginRegion( CRegion4* prc) { prc->CopyRect(&m_rcMargin); }
	int          GetMarginL() { return m_rcMargin.left; }
	int          GetMarginT() { return m_rcMargin.top; }
	int          GetMarginR() { return m_rcMargin.right; }
	int          GetMarginB() { return m_rcMargin.bottom; }
	int          GetMarginW() { return m_rcMargin.left + m_rcMargin.right; }
	int          GetMarginH() { return m_rcMargin.top + m_rcMargin.bottom; }
	void         GetClientRect( CRect* prc );
	void         GetClientRectAsWin32( CRect* prc );
	void         GetClientRectInWindow( CRect* prc );
	void         GetParentRect( CRect* prc );
	int          GetParentRectL() { return m_rcParent.left; }
	int          GetParentRectT() { return m_rcParent.top; }
	int          GetParentRectR() { return m_rcParent.right; }
	int          GetParentRectB() { return m_rcParent.bottom; }

	POINT        GetRealPosInWindow();
	void         GetWindowRect(CRect* lprc);
	void         WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj);
	void         WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	void         ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	bool         GetScrollOffset(int* pxOffset, int* pyOffset);

	int          GetWidth();
	int          GetHeight();
	int          GetConfigWidth() { return m_nConfigWidth; }
	int          GetConfigHeight() { return m_nConfigHeight; }
	void         SetObjectPos( int x, int y, int cx, int cy, int nFlag=0 );
	void         SetObjectPos( CRect* prc, int nFlag=0 );
	HRGN         GetRgn() { return m_hRgn; }
	HRFONT       GetFont();

	TextRenderBase* GetTextRender(){ return m_pTextRender; }
	RenderBase*     GetBkRender() { return m_pBkgndRender; }
	RenderBase*     GetForeRender() { return m_pForegndRender; }
	void            SetBkRender(RenderBase* p);
	void            SetForeRender(RenderBase* p);
	void            SetTextRender(TextRenderBase* p);

	
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
	void         SetEnable( bool b, bool bUpdateNow=true );
	bool         IsDefault();
	bool         IsReadonly();
	bool         IsHover();
	bool         IsPress();
	bool         IsForePress();
	bool         IsForeHover();
	void         SetDefault( bool b );
	void         SetReadonly( bool b );
	void         SetForceHover( bool b );
	void         SetForcePress( bool b );
	void         SetHover( bool b );
	void         SetPress( bool b );
	void         ModifyStyle( UINT add, UINT remove = 0, bool bNotify = true);
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
	void         DrawObjectTransparentBkgnd(HRDC hRDC, RenderOffsetClipHelper& roc, bool bSelfTransparent);

	// ����
	HRGN         GetClipRgnInWindow();
	void         SetReDraw( bool bReDraw );

protected:
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
	bool   SetChildObjectAttribute( Object* pChildObj, const String& strPrefix, map<String,String>& mapAttrib, bool bReload );

public:
	// ��Ϣ����
	UI_BEGIN_MSG_MAP
		UIMSG_WM_SETCURSOR( OnSetCursor )
		UIMSG_WM_ERASEBKGND( OnEraseBkgnd )
		UIMSG_WM_HITTEST( OnHitTest )
		UIMSG_WM_GETFONT( OnGetFont )
		UIMSG_WM_THEMECHANGED(OnThemeChanged)
	UI_END_MSG_MAP

protected:
	BOOL    OnEraseBkgnd( HRDC );
	BOOL    OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
	HRFONT  OnGetFont();
	UINT    OnHitTest( POINT* pt );
	void    OnThemeChanged();

	inline  bool testStateBit( UINT bit );
	inline  void setStateBit( UINT bit );
	inline  void clearStateBit( UINT bit );

};


}
