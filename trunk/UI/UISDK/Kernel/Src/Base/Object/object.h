#ifndef OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C
#define OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C

#include "UISDK\Kernel\Src\Base\Object\objtree.h"
#include "UISDK\Kernel\Inc\Util\rendercontext.h" 

namespace UI
{
interface IRenderFont;
interface ITextRenderBase;
interface IRenderBase;
class UICursor;
class RenderLayer;
class RenderChain;
class WindowBase;
interface ILayoutParam;

class Object : public ObjTree
{
public:
    Object();
    virtual ~Object();

    UI_DECLARE_OBJECT_INFO(Object, OBJ_NULL)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_SETCURSOR(OnSetCursor)
        UIMSG_WM_ERASEBKGND(OnEraseBkgnd)
        UIMSG_WM_HITTEST(OnHitTest)
        UIMSG_WM_GETRENDERFONT(GetRenderFont)
        UIMSG_WM_THEMECHANGED(OnThemeChanged)
        UIMESSAGE_HANDLER_EX(UI_WM_SKINMATERIALCHANGED, OnSkinMaterialChanged)
        UIMSG_WM_VISIBLE_CHANGED(OnVisibleChanged)
        UIMSG_WM_REDRAWOBJECT(OnRedrawObject)

		UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
        UIMSG_WM_RESETATTRIBUTE(ResetAttribute)
		UIMSG_WM_SERIALIZE(OnSerialize)
        UIMSG_WM_GETOBJECTINFO(OnGetObjectInfo)
        UIMSG_WM_FINALCONSTRUCT(FinalConstruct)
        UIMSG_WM_FINALRELEASE(FinalRelease)
    UI_END_MSG_MAP

    IObject* GetIObject() { return m_pIObject; }
    void     SetIObject(IObject* p) { m_pIObject = p; }

	HRESULT  FinalConstruct(IUIApplication* p);    // ������ʵ�ָ��麯�������ȵ��ø����ʵ��
	void     FinalRelease();                       // ������ʵ�ָ��麯�������ȵ��ø����ʵ��           

public:
	const  TCHAR*  GetId() { return m_strID.c_str(); }
	void     SetId(const TCHAR* szText);

    TCHAR*    GetObjectName();
    OBJ_TYPE  GetObjectType();
    CONTROL_TYPE  GetObjectExtentType();

	WindowBase*  GetWindowObject();
    RenderLayer* GetRenderLayer();
    RenderChain* GetRenderChain();
	HWND     GetHWND();
	Object*  FindChildObject(const TCHAR* szObjId);
    Object*  GetObjectByPos(POINT* pt);

	void  UpdateObject(bool bUpdateNow = true);
	void  UpdateObjectBkgnd(bool bUpdateNow);
	void  UpdateLayout(bool bUpdate);
	void  UpdateObjectNonClientRegion();
	void  ClearMyTreeRelationOnly();

public:
#pragma region // �����������
	void  GetNonClientRegion(CRegion4* prc) { prc->CopyRect(m_rcNonClient); }
	void  SetNonClientRegionExcludePaddingBorder( CRegion4* prc );
	void  SetNonClientRegionExcludePaddingBorderL( int n );
	void  SetNonClientRegionExcludePaddingBorderT( int n );
	void  SetNonClientRegionExcludePaddingBorderR( int n );
	void  SetNonClientRegionExcludePaddingBorderB( int n );
	int   GetNonClientL() { return m_rcNonClient.left; }
	int   GetNonClientT() { return m_rcNonClient.top; }
	int   GetNonClientR() { return m_rcNonClient.right; }
	int   GetNonClientB() { return m_rcNonClient.bottom; }
	int   GetNonClientW() { return m_rcNonClient.left + m_rcNonClient.right; }
	int   GetNonClientH() { return m_rcNonClient.top + m_rcNonClient.bottom; }
	void  GetBorderRegion( CRegion4* prc ) { prc->CopyRect(&m_rcBorder); }
	int   GetBorderL() { return m_rcBorder.left; }
	int   GetBorderT() { return m_rcBorder.top; }
	int   GetBorderR() { return m_rcBorder.right; }
	int   GetBorderB() { return m_rcBorder.bottom; }
	void  SetBorderRegion( CRegion4* prc );
	void  GetPaddingRegion( CRegion4* prc ) { prc->CopyRect(&m_rcPadding); }
	void  SetPaddingRegion( CRegion4* prc );
	int   GetPaddingL() { return m_rcPadding.left; }
	int   GetPaddingT() { return m_rcPadding.top; }
	int   GetPaddingR() { return m_rcPadding.right; }
	int   GetPaddingB() { return m_rcPadding.bottom; }
	int   GetPaddingW() { return m_rcPadding.left + m_rcPadding.right; }
	int   GetPaddingH() { return m_rcPadding.top + m_rcPadding.bottom; }
	void  SetMarginRegion( CRegion4* prc) { m_rcMargin.CopyRect(prc); }
	void  GetMarginRegion( CRegion4* prc) { prc->CopyRect(&m_rcMargin); }
	int   GetMarginL() { return m_rcMargin.left; }
	int   GetMarginT() { return m_rcMargin.top; }
	int   GetMarginR() { return m_rcMargin.right; }
	int   GetMarginB() { return m_rcMargin.bottom; }
	int   GetMarginW() { return m_rcMargin.left + m_rcMargin.right; }
	int   GetMarginH() { return m_rcMargin.top + m_rcMargin.bottom; }
	void  GetClientRect( CRect* prc );
	void  GetClientRectAsWin32(CRect* prc);
	void  GetClientRectInWindow(CRect* prc);
	void  GetParentRect( CRect* prc );
	int   GetParentRectL() { return m_rcParent.left; }
	int   GetParentRectT() { return m_rcParent.top; }
	int   GetParentRectR() { return m_rcParent.right; }
	int   GetParentRectB() { return m_rcParent.bottom; }

    ILayoutParam*  GetLayoutParam();
    void  CreateLayoutParam();
    void  SetLayoutParam(ILayoutParam* p);

    // Canvas����ר�ýӿ�
    int   GetConfigWidth();
    int   GetConfigHeight();
    int   GetConfigLayoutFlags();
    int   GetConfigLeft();
    int   GetConfigRight();
    int   GetConfigTop();
    int   GetConfigBottom();

    void  SetConfigWidth(int n);
    void  SetConfigHeight(int n);
    void  SetConfigLayoutFlags(int n);
    void  SetConfigLeft(int n);
    void  SetConfigRight(int n);
    void  SetConfigTop(int n);
    void  SetConfigBottom(int n);
#pragma endregion

#pragma region // pos width height
	POINT  GetRealPosInWindow();
	void  GetWindowRect(CRect* lprc);
	void  WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj);
	void  WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	void  WindowPoint2ObjectClientPoint_CalcScroll(const POINT* ptWindow, POINT* ptObj);
	void  ObjectPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptObj);
	void  ClientRect2ObjectRect(const RECT* rcClient, RECT* rcObj);
	bool  GetScrollOffset(int* pxOffset, int* pyOffset);
	bool  GetScrollRange(int* pxRange, int* pyRange);
	bool  GetObjectVisibleRect(RECT* prc, bool bInWindowOrLayer);
    bool  GetObjectVisibleClientRect(RECT* prc, bool bInWindowOrLayer);
//	HRGN  GetClipRgnInWindow();

	int   GetWidth();
	int   GetHeight();
	int   GetWidthWithMargins();
	int   GetHeightWithMargins();

	void  SetObjectPos( int x, int y, int cx, int cy, int nFlag=0 );
	void  SetObjectPos( CRect* prc, int nFlag=0 );
    void  UpdateLayoutPos();

#pragma endregion

#pragma region	// ��ʽ����
	int   GetStateBit() { return m_nStateBit; }
	bool  IsFocus();
	void  SetFocus(bool b, bool bNoitfy=true);
	bool  IsTabstop();
	bool  CanTabstop();
	bool  IsGroup();
	void  SetTabstop(bool b);
	void  SetGroup(bool b);
	bool  IsVisible();
	bool  IsCollapsed();
	bool  IsMySelfVisible();
	bool  IsEnable();
	void  SetVisible(bool b, bool bRedraw=true, bool bUpdateLayout=true);
	void  SetEnable(bool b, bool bUpdateNow=true, bool bNotify=true);
	bool  IsDefault();
//	bool  IsReadonly();
	bool  IsHover();
	bool  IsPress();
	bool  IsForcePress();
	bool  IsForceHover();
    bool  IsSelected();
    void  SetSelected(bool b, bool bNotify=true);
	void  SetDefault(bool b, bool bNotify=true);
//	void  SetReadonly(bool b);
	void  SetForceHover(bool b, bool bNotify=true);
	void  SetForcePress(bool b, bool bNotify=true);
	void  SetHover(bool b, bool bNotify=true);
	void  SetPress(bool b, bool bNotify=true);
	void  ModifyStyle(UINT add, UINT remove, bool bNotify);
    void  ModifyStyleEx(UINT add, UINT remove, bool bNotify);
    UINT  GetStyle() { return m_nStyle; }
    UINT  GetStyleEx() { return m_nStyle2; }
    void  SetStyle(UINT n) { m_nStyle = n; }
    void  SetStyleEx(UINT n) { m_nStyle2 = n; }
	bool  TestStyle(UINT nStyle){ if(m_nStyle&nStyle)return true; return false; }
    bool  TestStyleEx(UINT nStyleEx){ if(m_nStyle2&nStyleEx)return true; return false; }
	bool  IsTransparent();
    void  SetTransparent(bool b);

	void  SetAsNcObject(bool b);  // ObjTree��������
	bool  IsNcObject();           // ObjTree��������
	bool  IsRejectMouseMsgAll();
    bool  IsRejectMouseMsgSelf();

	void  SetCanRedraw(bool bReDraw);
	bool  CanRedraw();

	bool  testStateBit(UINT bit);
	void  setStateBit(UINT bit);
	void  clearStateBit(UINT bit);

public:
#pragma  endregion

	IRenderFont*  GetRenderFont();
	ITextRenderBase*  GetTextRender();
	IRenderBase*  GetBkRender();
	IRenderBase*  GetForeRender();
	void  SetBkgndRender(IRenderBase* p);
	void  SetForegndRender(IRenderBase* p);
	void  SetTextRender(ITextRenderBase* p);

	// ��չ���Բ���
    void  InitDefaultAttrib();
	void  SetUserData(LPVOID p);
	LPVOID  GetUserData();
	IUIApplication*  GetUIApplication() { return m_pUIApplication; }
	void  SetUIApplication(IUIApplication* p);
	void  SetOutRef(void** ppOutRef);
	HRGN  GetRgn() { return m_hRgn; }

	// ����
	virtual bool  DrawObject(IRenderTarget* pRenderTarget, RenderContext roc);  // CustomWindow��Ҫ�Ӹú������ж��ƣ�����HwndHostҲ�̳���
    virtual void  RealDrawObject(IRenderTarget* pRenderTarget, RenderContext roc);
	void  DrawChildObject(IRenderTarget* pRenderTarget, RenderContext& roc);
	void  DrawObjectTransparentBkgnd(IRenderTarget* pRenderTarget, RenderContext& roc, bool bSelfTransparent);
    HBITMAP  TakeSnapshot();
    HBITMAP  TakeBkgndSnapshot();

	// ����
	bool  SetCursor(const TCHAR* szCursorId);
    DWORD  CalcContrastTextColor();
    bool  SetMouseCapture(int nNotifyMsgId);
    bool  ReleaseMouseCapture();
    bool  SetKeyboardCapture(int nNotifyMsgId);
    bool  ReleaseKeyboardCapture();

protected:
	Object*  _findChildObjectItem(const TCHAR* szobjId);
	void  _drawNcChildObject(IRenderTarget*, RenderContext& roc);
	void  _parseLayoutAttribute(IMapAttribute* pMapAttrib);

public:
	// ��xml�õ�����ת�ɶ�������ԣ�ע�⣬�������ظú���ʱ�������ȵ��ø���ĸ÷���
	// bReload��ʾΪ����ʱ���ã���ʱ�����ٸ�ĳЩ���Ը�ֵ������text����
	bool  SetAttribute(IMapAttribute* pMapAttrib, bool bReload);
	void  OnSerialize(SERIALIZEDATA* pData);

	// ��UIObjCreator����Object�ͻ���ʱ���������ú�������Ϊ����ʱ��������������Դ�������ResetAttribute��Ҫ�ͷ�������Դ����
	// ע�⣬�������ظú���ʱ�������ȵ��ø���ĸ÷���
	void  ResetAttribute();

	// �ڲ��ֹ����з����Լ�����Ҫ�Ĵ�С(����padding+margin)�����û��ָ����С��
	// ������GetAutoSize��ȡ����Ӧ��С��
	// ��Panel::GetAutoSize��ȥ����m_pLayout->Measure��Layout�ֻ�ȥ�ٵ���
	// LayoutManager�����MeasureChildObject�麯��
	SIZE  GetDesiredSize();

	void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA*  pData);

protected:
	// ��û��ָ���ؼ��Ĵ�С(width or height)������£����Լ������Լ�����Ҫ�Ĵ�С��
	// ����ֵ������margin��margin�ļ�����GetDesiredSize��ɣ���padding�����Ӷ����Լ����
    // SIZE  GetAutoSize() { SIZE s = {0,0}; return s; }

public:
//	bool  SetChildObjectAttribute(Object* pChildObj, const String& strPrefix, IMapAttribute* pMapAttrib, bool bReload);
    void  SetAttributeByPrefix(const TCHAR* szPrefix, IMapAttribute* pMatAttrib, bool bReload, bool bErase);
    void  ParseStyleAndSetAttribute(IMapAttribute* pMatAttrib, bool bReload);

    const TCHAR*  GetAttribute(const TCHAR* szKey, bool bErase);
    void  AddAttribute(const TCHAR* szKey, const TCHAR*  szValue);
    void  GetMapAttribute(IMapAttribute** ppMapAttribute);  // ��Ҫrelease

protected:
    void*  QueryInterface(const IID* pIID);
	void  OnEraseBkgnd( IRenderTarget* );
	BOOL  OnSetCursor( HWND hWnd, UINT nHitTest, UINT message );
	UINT  OnHitTest( POINT* pt );
	void  OnThemeChanged();
    LRESULT  OnSkinMaterialChanged(UINT, WPARAM, LPARAM);
    void  OnVisibleChanged(BOOL bVisible, IObject* pObjChanged);
    void  OnRedrawObject();

//////////////////////////////////////////////////////////////////////////

protected:
    IObject* m_pIObject;
    String   m_strID;                        // �ö�����XML�еı�ʶ
    
#pragma region //�����������
    CRect     m_rcParent;                    // �ö���ķ�Χ�������parent.����Window�����ǿͻ�����λ�ã������Ͻ�Ϊ0��0
    CRegion4  m_rcNonClient;                 // ���û�й�������Header������ռ�ÿռ�Ŀؼ���m_rcNonClient��Ϊm_rcPadding+m_rcBorder
    CRegion4  m_rcMargin;
    CRegion4  m_rcPadding;
    CRegion4  m_rcBorder;       
    HRGN      m_hRgn;                        // ��δʹ�ã�����ö�����һ�����������򣬱������ø�ֵ����ֵ��window���Ͷ�����Ч. rgn������ڴ������Ͻǵġ�
    ILayoutParam*  m_pLayoutParam;           // ���ֲ�������Object�����ͷ�
#pragma endregion

    int      m_nCanRedrawRef;                // TODO: ���ڽ���ദ����SetRedraw(false)�󣬱������еط��ſ�SetRedraw(true)���ܽ��л���
    UINT     m_nStateBit;                    // �����״̬����visible/disable
    UINT     m_nStyle;                       // ��ͨ��ʽ
    UINT     m_nStyle2;                      // �ؼ���ʽ����Ĺ�ϵ
    LPVOID   m_pUserData;                    // �Զ������ݣ�Ŀǰ��������UIBuilder�����ݹ�����
    IMapAttribute*  m_pIMapAttributeRemain;  // ������չ��δ���������ԣ�����Tooltip

    IRenderBase*   m_pBkgndRender;           // ������Ⱦ
    IRenderBase*   m_pForegndRender;         // ǰ����Ⱦ
    ITextRenderBase*  m_pTextRender;         // ������Ⱦ
    IUICursor*  m_pCursor;                   // ����������ʽ
    
    void**  m_ppOutRef;                      // Ϊ�˽��һ�����Ա�����п��ܱ��Լ��ĸ�����ɾ���������ȴ��֪������ɾ���ö���ʱ������.
    IUIApplication*  m_pUIApplication;       // �ö������ڵ�App(Ϊ��ʵ�ֶ�ʵ������app������Ϊȫ�ֱ�����ÿ��Object�ܹ��������õ�app�ӿ�)
};

}

#endif  // OBJECT_H_763AA6BD_6435_456f_A41F_E5D7CCF39E7C

