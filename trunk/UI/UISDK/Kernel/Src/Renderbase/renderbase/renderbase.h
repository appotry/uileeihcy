#pragma  once

#include "UISDK\Kernel\Src\Base\Message\message.h"

namespace UI
{
const UINT  WINDOW_BKGND_RENDER_STATE_ACTIVE  = 0;   // ���ڼ�����ʽ
const UINT  WINDOW_BKGND_RENDER_STATE_INACTIVE = 1;   // ���ڷǼ�����ʽ

// ���ڱ������ƻ��࣬����ʵ�ָ�����ʽ���ڱ����Ļ��ƣ��磺ƽ�̣����У����죬9����
class RenderBase : public Message
{
public:
	RenderBase();
	virtual ~RenderBase(){};
    
    UI_BEGIN_MSG_MAP
    UI_END_MSG_MAP


    void  SetIRenderBase(IRenderBase* p) { m_pIRenderBase = p; }
    
	//
	//	����Render���Ա����ںܶ�ط����米����ͼ��ȵȡ�Ϊ���������õ����ĸ����ֵ�Render���ԣ�
	//	����Render����ǰ�����һ��ǰ׺����bkgnd.render.type  icon.render.type
	//	��SetAttribute�У���Ҫͬʱ����ǰ׺+�����������в���
	//
	//	����IRender����Ҫ�ṩRetAttribute��������ΪObject��Reset��ʱ��һ����delete irender
	//	�������Ҳû����� bool bReload ����
	//
    void  SetAttribute(SetAttrPrefixData* pData){ return; };

    bool  IsThemeRender() {return m_nRenderType > RENDER_TYPE_THEME_FIRST && m_nRenderType < RENDER_TYPE_THEME_LAST; }
    void  CheckThemeChanged();

	void     SetObject(IUIApplication* pUIApp, Object* pObject) { m_pUIApplication = pUIApp; m_pObject = pObject; }
    Object*  GetObject() { return m_pObject; }

	void  SetRenderType( const RENDER_TYPE& nType ){ m_nRenderType = nType ; }
	RENDER_TYPE  GetRenderType() { return m_nRenderType; }
	IColorRes*  GetActiveSkinColorRes();
	IImageRes*  GetActiveSkinImageRes();
	

protected:
    IUIApplication*  m_pUIApplication;
    IRenderBase*     m_pIRenderBase;

    Object*  m_pObject;  // �󶨵Ķ���Ҫ����˭�ı���
	RENDER_TYPE   m_nRenderType;  // �Լ�������
};

// ��bkgnd.render.type=null��ʾ��ʹ��Ĭ�ϱ���
class NullRender : public RenderBase
{
public:
    NullRender() { m_pINullRender = NULL; }
    UI_DECLARE_RENDERBASE(NullRender, XML_RENDER_TYPE_NULL, RENDER_TYPE_NULL)

    UI_BEGIN_MSG_MAP
        UIMSG_WM_QUERYINTERFACE(QueryInterface)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

    void  SetINullRender(INullRender* p) { m_pINullRender = p; }

    INullRender*  m_pINullRender;
};

//	ϵͳ������Ⱦ����
class ThemeRenderBase : public RenderBase
{
public:
	ThemeRenderBase();
	~ThemeRenderBase();

    UI_BEGIN_MSG_MAP
        UIMSG_WM_THEMECHANGED(OnThemeChanged)
        UIMSG_WM_SETATTRIBUTE_PREFIX(SetAttribute)
        UIMSG_WM_EDITORGETATTRLIST(OnEditorGetAttrList)
        UIMSG_WM_INITIALIZE(OnInit)
    UI_END_MSG_MAP_CHAIN_PARENT(RenderBase)

    void  SetIThemeRenderBase(IThemeRenderBase* p) { m_pIThemeRenderBase = p; } 
    void  SetAttribute(SetAttrPrefixData* pData);
    void  OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA* pData);

    void  OnInit();
    void  OnThemeChanged();

public:
	void  CreateTheme();
	void  SetNoTheme() { m_bNoTheme = true; CreateTheme(); }
    HTHEME GetTHEME() { return m_hTheme; }

protected:
    IThemeRenderBase*  m_pIThemeRenderBase;
	HTHEME  m_hTheme;
	bool    m_bNoTheme;  // �Ƿ�ǿ��ʹ�����������ʽ��
};


}
