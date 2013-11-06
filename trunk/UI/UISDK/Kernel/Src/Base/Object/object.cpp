#include "stdafx.h"
#include "object.h"

#include "UISDK\Kernel\Inc\Interface\iwindow.h"
#include "UISDK\Kernel\Inc\Interface\ilayout.h"
#include "UISDK\Kernel\Inc\Interface\ipanel.h"
#include "UISDK\Kernel\Src\Resource\uiresource.h"
#include "UISDK\Kernel\Src\Renderbase\renderbase\renderbase.h"
#include "UISDK\Kernel\Src\Renderbase\textrenderbase\textrender.h"
#include "UISDK\Kernel\Src\UIObject\Window\windowbase.h"
#include "UISDK\Kernel\Src\UIObject\HwndHost\HwndHost.h"
#include "UISDK\Kernel\Src\Renderlibrary\uicursor.h"
#include "UISDK\Kernel\Src\RenderLayer\renderlayer.h"
#include "UISDK\Kernel\Src\RenderLayer\renderchain.h"
#include "UISDK\Kernel\Src\Helper\layout\layout.h"
#include "UISDK\Kernel\Src\Helper\layout\canvaslayout.h"

namespace UI
{

Object::Object()
{
    m_pIObject = NULL;
	::SetRectEmpty(&m_rcParent);
	::SetRectEmpty(&m_rcNonClient);
	::SetRectEmpty(&m_rcMargin);
	::SetRectEmpty(&m_rcPadding);
	::SetRectEmpty(&m_rcBorder);

	this->m_nCanRedrawRef = 0;
	
	this->m_hRgn = NULL;
	this->m_nStateBit = 0;
	this->m_nStyle2 = 0;
	this->m_nStyle  = 0;
	this->m_pUserData = NULL;
	this->m_pUIApplication = NULL;
	this->m_ppOutRef = NULL;

	m_pBkgndRender = NULL;
	m_pForegndRender = NULL;
	m_pTextRender = NULL;
	m_pCursor = NULL;
    m_pIMapAttributeRemain = NULL;
    m_pLayoutParam = NULL;
}

 // ע�⣺��Ҫ�ڹ���������������е����麯��

Object::~Object(void)
{
	if (m_ppOutRef)
		*m_ppOutRef = NULL;

	if (m_pUIApplication)
		m_pUIApplication->RemoveUIObject(GetIMessage());

    SAFE_RELEASE(m_pIMapAttributeRemain);
    SAFE_RELEASE(m_pLayoutParam);
}

HRESULT Object::FinalConstruct(IUIApplication* p)
{
    m_pUIApplication = p;
    if (m_pUIApplication)
        m_pUIApplication->AddUIObject(this->GetIMessage()); 

    return S_OK;
}

void Object::FinalRelease()
{
	// ��ֹ������;���߻��������У�һЩ����ͻȻ�����٣����µ�Ұָ�����
	WindowBase* pWindow = GetWindowObject();
	if (pWindow)  // ������DestroyChildObjectǰ���ã�����parent��ϵ�����
	{
		pWindow->OnObjectDeleteInd(this);
	}

	this->DestroyChildObject();  // ��ɾ���Ӷ���

	IUIEditor* pEditor = m_pUIApplication->GetUIEditorPtr();
	if (pEditor)
	{
		// ������������֪ͨ�����ڴ�skinbuilder��tree��һ����ɾ��
		pEditor->OnObjectDeleteInd(m_pIObject);
	}

	m_rcParent.left = m_rcParent.right = m_rcParent.top = m_rcParent.bottom = 0;
	SAFE_RELEASE(m_pBkgndRender);
	SAFE_RELEASE(m_pForegndRender);
	SAFE_RELEASE(m_pTextRender);
	SAFE_RELEASE(m_pCursor);
	SAFE_DELETE_GDIOBJECT(m_hRgn);
}

// ע�����������ģ��ֱ�ӵ��� pCtrl->m_strID=L"..."�Ļ����ڶ����ͷ�ʱ�������
void Object::SetId(const TCHAR* szText)
{
	if (NULL == szText)
	{
		m_strID.clear();
		return;
	}

	m_strID = szText;
}

TCHAR* Object::GetObjectName()
{
    TCHAR* szName = NULL;
    int nType = 0;
    UISendMessage(this, UI_WM_GETOBJECTINFO, (WPARAM)&szName, (LPARAM)&nType);
    return szName;
}
OBJ_TYPE Object::GetObjectType()
{
    TCHAR* szName = NULL;
    int nType = 0;
    UISendMessage(this, UI_WM_GETOBJECTINFO, (WPARAM)&szName, (LPARAM)&nType);
    return GetObjectMajorType(nType);
}
CONTROL_TYPE Object::GetObjectExtentType()
{
    TCHAR* szName = NULL;
    int nType = 0;
    UISendMessage(this, UI_WM_GETOBJECTINFO, (WPARAM)&szName, (LPARAM)&nType);
    return ::GetObjectExtentType(nType);
}

//
//	�ػ��������(һ��������������£�״̬�仯��ʱ����ػ�)
//
//	1. ��͸���Ŀؼ���ֱ����ԭ���漴�ɣ������˱���ΪCOLOR��Image�����
//		TODO: ���ǽ�ButtonBkRender�ٳ�������������еĿؼ�ʹ��
//	2. ͸���Ŀؼ�...�Ƿ���Ҫ�������Ż�Ϊֱ��ȡ���Լ��ڸ������µı�����������ˢ����������
//

void Object::UpdateObject(bool bUpdateNow)
{
	if (IsVisible() && CanRedraw())
    {
	    RenderChain*  pRenderChain = GetRenderChain();
	    if (pRenderChain)
	    {
		    pRenderChain->UpdateObject(this, false, bUpdateNow);
	    }
    }
}

// ����������/�ƶ���ʱ��ˢ�±���

void Object::UpdateObjectBkgnd(bool bUpdateNow)
{
	if (!CanRedraw())
		return ;

    RenderChain*  pRenderChain = GetRenderChain();
    if (pRenderChain)
    {
        pRenderChain->UpdateObject(this, true, bUpdateNow);
    }
}

//
//	��������ʾ/���أ����ߴ�С�����ı�ʱ������ˢ���Լ�����layout�Ĳ���
//
//	1. ��ȡ�Լ�������Layout
//	2. ����Layout�Ĵ�С�Ƿ����ı䣬������Layout��С�ı䣬�ٻ�ȡ��һ��Layout
//
void Object::UpdateLayout(bool bUpdate)
{
    if (this->GetObjectType() == OBJ_WINDOW)
    {
        //UI_UpdateLayout((WindowBase*)this, bUpdate?TRUE:FALSE);

        if (((WindowBase*)this)->GetLayout())
            ((WindowBase*)this)->GetLayout()->Arrange(NULL);

        if (bUpdate)
            this->UpdateObject();

        return;
    }

    Object* pParent = this->GetParentObject();
    Object* pObjectToUpdate = this;

    while (pParent)
    {
        int nObjType = pParent->GetObjectType();
        if (OBJ_PANEL != nObjType && OBJ_WINDOW != nObjType)
            return;

        ILayout* pLayout = ((Panel*)pParent)->GetLayout();
        if (NULL == ((Panel*)pParent)->GetLayout())
            return;

        SIZE sizeOld = {pParent->GetWidth(), pParent->GetHeight()};
        SIZE size = pParent->GetDesiredSize();
        //UISendMessage(pParent->GetIMessage(), UI_WM_GETDESIREDSIZE, (WPARAM)&size);

        // pParent�Ĵ�С�����˱仯����������
        if (sizeOld.cx != size.cx || sizeOld.cy != size.cy)
        {
            if (pParent->GetParentObject())
            {
                pObjectToUpdate = pParent;
                pParent = pParent->GetParentObject();  // �е���... ���Ǵ��ڵ�ʱ��sizeΪwindow rect��sizeOldΪclient rect����˿϶���һ��
                continue;                              // �������������������window( parent==null )��������´���
            }
			else if (pParent->GetObjectType() == OBJ_WINDOW)
			{
				SetWindowPos(GetHWND(), 0, 0,0, size.cx, size.cy, SWP_NOZORDER|SWP_NOMOVE|SWP_NOACTIVATE);
				return;
			}
        }

        IObject* pIObjectToUpdate = NULL;
        if (pObjectToUpdate)
            pIObjectToUpdate = pObjectToUpdate->GetIObject();
        pLayout->Arrange(pIObjectToUpdate, bUpdate);

        break;
    }
}

void Object::SetCanRedraw( bool bReDraw )
{
	if (bReDraw)
	{
		m_nCanRedrawRef --;
	//	this->clearStateBit( CSB_PREVENTREDRAW );
	}
	else
	{
		m_nCanRedrawRef ++;
	//	this->setStateBit( CSB_PREVENTREDRAW );
	}
}


bool Object::CanRedraw()
{
	return 0==m_nCanRedrawRef; //!this->testStateBit(CSB_PREVENTREDRAW);
}


bool Object::SetCursor(const TCHAR* szCursorId)
{
	SAFE_RELEASE(m_pCursor);

	ICursorRes* pCursorRes = m_pUIApplication->GetActiveSkinCursorRes();
	if (NULL == pCursorRes)
		return false;

    pCursorRes->GetCursor(szCursorId, &m_pCursor); 
	if (NULL == m_pCursor)
	{
		UI_LOG_WARN(_T("%s get cursor failed. Object id=%s, cursor id=%s"), FUNC_NAME, m_strID.c_str(), szCursorId);	
		return false;
	}

	return true;
}

// ��ȡ����������Ⱦ��
RenderLayer*  Object::GetRenderLayer()
{
    Object* pObj = this;
    while (NULL != pObj)
    {
        OBJ_TYPE eType = pObj->GetObjectType();
        if (OBJ_LAYER == eType)
        {
            return static_cast<RenderLayer*>(pObj);
        }
        else if (OBJ_WINDOW == eType)
        {
            return static_cast<RenderLayer*>(pObj);
        }
        else
        {
            pObj = pObj->GetParentObject();
        }
    }
    return NULL;
}

RenderChain* Object::GetRenderChain()
{   
    RenderLayer*  pRenderLayer = this->GetRenderLayer();
    if (NULL == pRenderLayer)
        return NULL;

    return pRenderLayer->GetRenderChainPtr();
}

// ��ȡһ���ؼ����ڴ��ھ��
WindowBase* Object::GetWindowObject()
{
    RenderLayer* pRenderLayer= this->GetRenderLayer();
    if (pRenderLayer)
        return pRenderLayer->GetWindowPtr();

    return NULL;
}


HWND Object::GetHWND()
{
	WindowBase* pWindow = this->GetWindowObject();
	if (NULL == pWindow)
		return NULL;

	return pWindow->GetHWND();
}
/*
**	[public] ��ȡĳһ����·���µ��Ӷ��󣬵�������Ӷ���Ҳ�����Ӷ�����Ӷ���
**
**	parameter
**		objIdPath
**			[in]	Ҫ��ȡ�Ķ����·�����������Լ�����·����/�ָ�
**	
**	return
**		�ɹ����ظö���ʧ�ܷ���NULL��ʧ�ܿɲ鿴��־���
*/
// Object*  Object::GetChildObject( const String& objIdPath )
// {
// 	Object*   pObjRet  = this;
// 
// 	vector<String>  vPath;
// 	UI_Split( objIdPath, XML_PATH_SEPERATOR, vPath );
// 
// 	if (vPath.empty() )
// 	{
// 		UI_LOG_ERROR( _T("Object::GetObjectItem����Ч�Ĳ���:%s"), objIdPath.c_str() );
// 		return NULL;
// 	}
// 
// 	size_t  s = vPath.size();
// 	for( size_t i = 0; i < s; i++ )
// 	{
// 		Object* pTempObj = NULL;
// 		while( pTempObj = pObjRet->EnumChildObject( pTempObj ) )
// 		{
// 			if (vPath[i] == pTempObj->m_strID )
// 			{
// 				pObjRet = pTempObj;
// 				break;  // jump out of while
// 			}
// 		}
// 
// 		// û���ҵ�ָ���Ķ���
// 		if (NULL == pTempObj )
// 		{
// 			UI_LOG_ERROR( _T("Object::GetObjectItem��δ�ҵ�ָ���Ķ�����%s�£�"), this->m_strID.c_str(), objIdPath.c_str() );
// 			return NULL;
// 		}
// 	}
// 
// 	return pObjRet;
// }

//
//	�����е����������в���idΪobjId�Ķ���ֻ���ص�һ���ҵ��Ķ���
//
//	Parameter
//		objId
//			[in]	Ҫ���ҵĶ���id
//
//	Return
//		�ɹ����ض����ַ��ʧ�ܷ���NULL
//

Object*  Object::FindChildObject(const TCHAR* szObjId)
{
    if (NULL == szObjId)
        return NULL;

	Object* pRet = this->_findChildObjectItem(szObjId);
	if (NULL == pRet)
	{
		UI_LOG_WARN(_T("%s Find \"%s\" from \"%s\" failed."), FUNC_NAME, szObjId, this->m_strID.c_str() );
	}
	return pRet;
}


Object* Object::_findChildObjectItem(const TCHAR* szobjId)
{
    if (NULL == szobjId)
        return NULL;

	Object* pObjChild = NULL;
	while (pObjChild = this->EnumChildObject(pObjChild))
	{
		if (0 == _tcscmp(pObjChild->GetId(), szobjId))
        {
			return pObjChild;
        }
		else
		{
			Object* pRet = ((Object*)pObjChild)->_findChildObjectItem(szobjId);
			if (pRet )
				return pRet;
		}
	}	
	return NULL;
}


void Object::ResetAttribute()
{
	::SetRectEmpty(&m_rcMargin);
	::SetRectEmpty(&m_rcPadding);
	::SetRectEmpty(&m_rcNonClient);
	::SetRectEmpty(&m_rcBorder);

    SAFE_RELEASE(m_pLayoutParam);
	SAFE_RELEASE(m_pBkgndRender);
	SAFE_RELEASE(m_pForegndRender);
	SAFE_RELEASE(m_pTextRender);
	SAFE_RELEASE(m_pCursor);

}

void  Object::OnSerialize(SERIALIZEDATA* pData)
{
	if (pData->nFlag & SERIALIZEFLAG_SAVE)
	{

	}
	else if (pData->nFlag & SERIALIZEFLAG_LOAD)
	{
		SetAttribute(pData->pMapAttrib, pData->nFlag&SERIALIZEFLAG_RELOAD?true:false);
	}

    // ��������
    if (m_pLayoutParam)
    {
        m_pLayoutParam->Serialize(pData);
    }
}
bool Object::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    // �������ԣ�������չ��
    SAFE_RELEASE(m_pIMapAttributeRemain);
    m_pIMapAttributeRemain = pMapAttrib;
    if (m_pIMapAttributeRemain)
        m_pIMapAttributeRemain->AddRef();

	if (NULL == m_pUIApplication || NULL == pMapAttrib)
	{
		UI_LOG_ERROR(_T("%s Invalid Argument."), FUNC_NAME);
		return false;
	}

	//	����id����
    const TCHAR* szText = NULL;
    szText = pMapAttrib->GetAttr(XML_ID, true);
    if (szText)
        m_strID = szText;
	
    // �ɼ���
	bool bVisible = true;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_VISIBLE, true, &bVisible))
    {
	    if (false == bVisible)
		    m_nStateBit |= OSB_UNVISIBLE;
        else
            m_nStateBit &= ~OSB_UNVISIBLE;
    }

    bool bDisable = false;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_DISABLE, true, &bDisable))
    {
        if (bDisable)
            m_nStateBit |= OSB_DISABLE;
        else
            m_nStateBit &= ~OSB_DISABLE;
    }

    pMapAttrib->GetAttr_CRegion4(XML_MARGIN, true, &m_rcMargin);

    CRegion4 region4;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_CRegion4(XML_PADDING, true, &region4))
        this->SetPaddingRegion(&region4);

    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_CRegion4(XML_BORDER, true, &region4))
        this->SetBorderRegion(&region4);

	// ���ñ�����Ⱦ��
    pMapAttrib->GetAttr_RenderBase(XML_BACKGND_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, this->GetIObject(), &m_pBkgndRender);

	// ����ǰ������
    pMapAttrib->GetAttr_RenderBase(XML_FOREGND_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, this->GetIObject(), &m_pForegndRender);

// 	// ����  -- �����������Ƴ���˭��Ҫ��������˭�Լ�����������������еĶ�����Ҫ����
	//          ��window��font������windowbase::setattribute�н�����"font"���Եõ�
// 	iter = m_mapAttribute.find(XML_TEXTRENDER_TYPE);
// 	if (m_mapAttribute.end() != iter)
// 	{
// 		SAFE_RELEASE(m_pTextRender);
// 		const String& strTextRenderType = iter->second;
// 		m_pTextRender = TextRenderFactory::GetTextRender(strTextRenderType, this);
// 		m_pTextRender->SetAttribute(_T(""),m_mapAttribute);
// 
// 		this->m_mapAttribute.erase(iter);
// 	}
// 	else if (NULL == m_pTextRender )
// 	{
// 		m_pTextRender = TextRenderFactory::GetTextRender(TEXTRENDER_TYPE_NORMAL, this);
// 		m_pTextRender->SetAttribute(_T(""),m_mapAttribute);
// 	}
	
	// ˢ������
	bool bTransparent = false;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_BACKGND_IS_TRANSPARENT, true, &bTransparent))
	{
		if (bTransparent)
			m_nStyle |= OBJECT_STYLE_TRANSPARENT;
		else
			 m_nStyle &= ~OBJECT_STYLE_TRANSPARENT;
	}

    bool bRejectMouseMsg = false;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_REJEST_MOUSE_MSG, true, &bRejectMouseMsg))
    {
        if (bRejectMouseMsg)
            m_nStyle |= OBJECT_STYLE_REJECT_MOUSE_MSG_ALL;
        else
            m_nStyle &= ~OBJECT_STYLE_REJECT_MOUSE_MSG_ALL;
    }

    bool bZindexOverlap = false;
    if (MAPATTR_RET_NOT_EXIST != pMapAttrib->GetAttr_bool(XML_ZINDEX_OVERLAP, true, &bZindexOverlap))
    {
        if (bZindexOverlap)
            m_nStyle |= OBJECT_STYLE_ZINDEX_OVERLAP;
        else
            m_nStyle &= ~OBJECT_STYLE_ZINDEX_OVERLAP;
    }
	
	// �����ʽ
	szText = pMapAttrib->GetAttr(XML_CURSOR, true);
	if (szText)
		this->SetCursor(szText);

	return true;
}


void  Object::OnEditorGetAttrList(EDITORGETOBJECTATTRLISTDATA*  pData)
{
    IUIEditor* pEditor = pData->pEditor;
    const TCHAR* szPrefix = pData->szPrefix;

	IUIEditorGroupAttribute*  pObjGroup = pEditor->CreateGroupAttribute(pData->pGroupAttr, Object::GetXmlName(), NULL);
 	pEditor->CreateTextAttribute(pObjGroup, XML_ID, szPrefix, NULL, L"�ؼ�ID");
    pEditor->CreateBoolAttribute(pObjGroup, XML_VISIBLE, true, szPrefix, NULL, L"�Ƿ�ɼ���Ĭ�Ͽɼ�");
	pEditor->CreateBoolAttribute(pObjGroup, XML_DISABLE, false, szPrefix, NULL, L"�Ƿ񱻽��á�Ĭ�Ͽ���");
    pEditor->CreateBoolAttribute(pObjGroup, XML_BACKGND_IS_TRANSPARENT, false, szPrefix, NULL, L"�ؼ������Ƿ�͸��������ؼ�����͸�����ڻ���ʱ����Ҫ�ٻ��Ƹ��ؼ�������Ĭ�ϲ�͸��");
    pEditor->CreateBoolAttribute(pObjGroup, XML_REJEST_MOUSE_MSG, false, szPrefix, NULL, L"�Ƿ����������Ϣ��Ĭ�Ͻ���");
    pEditor->CreateBoolAttribute(pObjGroup, XML_ZINDEX_OVERLAP, false, szPrefix, NULL, L"�Ƿ�������ؼ����ص�����Ĭ��û��");
	pEditor->CreateTextAttribute(pObjGroup, XML_MARGIN, szPrefix, NULL, L"����");
	pEditor->CreateTextAttribute(pObjGroup, XML_PADDING, szPrefix, NULL, L"�ڼ��");
	pEditor->CreateTextAttribute(pObjGroup, XML_BORDER, szPrefix, NULL, L"�߿�");
    pEditor->CreateTextAttribute(pObjGroup, XML_CURSOR, szPrefix, NULL, L"�����ʽ");

    IUIEditorGroupAttribute* pBkRenderGroup = pEditor->CreateGroupAttribute(pObjGroup, XML_BACKGND_RENDER_PREFIX, NULL);
	pEditor->CreateTextAttribute(pBkRenderGroup, XML_RENDER_TYPE, szPrefix, XML_BACKGND_RENDER_PREFIX);
    if (m_pBkgndRender)
    {
        String strPrefix;
        if (pData->szPrefix)
            strPrefix.append(pData->szPrefix);
        strPrefix.append(XML_BACKGND_RENDER_PREFIX);

        EDITORGETOBJECTATTRLISTDATA data;
        data.pEditor = pData->pEditor;
        data.pGroupAttr = pBkRenderGroup;
        data.szPrefix = strPrefix.c_str();
        UISendMessage(m_pBkgndRender, UI_EDITOR_GETOBJECTATTRLIST, (WPARAM)&data);
    }

    IUIEditorGroupAttribute* pForeRenderGroup = pEditor->CreateGroupAttribute(pObjGroup, XML_FOREGND_RENDER_PREFIX, NULL);
	pEditor->CreateTextAttribute(pForeRenderGroup, XML_RENDER_TYPE, szPrefix, XML_FOREGND_RENDER_PREFIX);
    if (m_pForegndRender)
    {
        String strPrefix;
        if (pData->szPrefix)
            strPrefix.append(pData->szPrefix);
        strPrefix.append(XML_FOREGND_RENDER_PREFIX);

        EDITORGETOBJECTATTRLISTDATA data;
        data.pEditor = pData->pEditor;
        data.pGroupAttr = pForeRenderGroup;
        data.szPrefix = strPrefix.c_str();
        UISendMessage(m_pForegndRender, UI_EDITOR_GETOBJECTATTRLIST, (WPARAM)&data);
    }

    IUIEditorGroupAttribute* pLayoutGroup = pEditor->CreateGroupAttribute(pObjGroup, XML_LAYOUT, NULL);
    if (m_pLayoutParam)
    {
        EDITORGETOBJECTATTRLISTDATA data = {0};
        data.pEditor = pEditor;
        data.pGroupAttr = pLayoutGroup;
        m_pLayoutParam->OnEditorGetAttrList(&data);
    }
}

void  Object::ParseStyleAndSetAttribute(IMapAttribute* pMatAttrib, bool bReload)
{
    // ������ʽ
    const TCHAR* szStyleClass = pMatAttrib->GetAttr(XML_STYLECLASS, false);

    if (m_pUIApplication)
    {
        ISkinRes* pSkinRes = m_pUIApplication->GetActiveSkinRes();
        if (pSkinRes)
        {
            IStyleManager* pStyleMgr = pSkinRes->GetStyleManager();
            if (pStyleMgr)
            {
                pStyleMgr->LoadStyle(GetObjectName(), szStyleClass, m_strID.c_str(), pMatAttrib);
            }
        }
    }

	SERIALIZEDATA data = {0};
	data.pMapAttrib = pMatAttrib;
	data.nFlag = SERIALIZEFLAG_LOAD;
	if (bReload)
		data.nFlag |= SERIALIZEFLAG_RELOAD;
	UISendMessage(m_pIObject, UI_WM_SERIALIZE, (WPARAM)&data);
//    UISendMessage(m_pIObject, UI_WM_SETATTRIBUTE, (WPARAM)pMatAttrib, (LPARAM)bReload); // ��ʹΪ�գ�ҲҪ֪ͨ��ȥ
}
void Object::SetAttributeByPrefix(const TCHAR* szPrefix, IMapAttribute* pMapAttrib, bool bReload, bool bErase)
{
    IMapAttribute* pMapChildObjAttrib = NULL;

    if (szPrefix)
    {
        if (false == pMapAttrib->ExtractMapAttrByPrefix(szPrefix, bErase, &pMapChildObjAttrib))
        {
            SAFE_RELEASE(pMapChildObjAttrib);
            return;
        }

        if (NULL == pMapChildObjAttrib)
            return;
    }
    else
    {
        pMapChildObjAttrib = pMapAttrib;
        pMapChildObjAttrib->AddRef();
    }

    ParseStyleAndSetAttribute(pMapChildObjAttrib, bReload);
    SAFE_RELEASE(pMapChildObjAttrib);
}
// bool Object::SetChildObjectAttribute(Object* pChildObj, const String& strPrifix, IMapAttribute* pMapAttrib, bool bReload)
// {
// 	if (NULL == pChildObj)
// 		return false;
// 
// 	IMapAttribute* pMapChildObjAttrib = NULL;
//     if (FALSE == pMapAttrib->ExtractMapAttrByPrefix(strPrifix.c_str(), true, &pMapChildObjAttrib))
//         return false;
//     if (NULL == pMapChildObjAttrib)
//         return false;
// 
//     // ������ʽ
// 	const TCHAR* szStyleClass = pMapChildObjAttrib->GetAttr(XML_STYLECLASS, false);
// 
// 	if (m_pUIApplication)
// 	{
// 		ISkinRes* pSkinRes = NULL;
// 		m_pUIApplication->GetActiveSkinRes(&pSkinRes);
// 		if (pSkinRes)
// 		{
// 			IStyleManager* pStyleMgr = NULL;
// 			pSkinRes->GetStyleManager(&pStyleMgr);
// 			if (pStyleMgr)
// 			{
// 				pStyleMgr->LoadStyle(pChildObj->GetObjectName(), szStyleClass, pChildObj->m_strID.c_str(), pMapAttrib);
// 			}
// 		}
// 	}
// 
// 	bool bRet = pChildObj->SetAttribute(pMapChildObjAttrib, bReload);  // ��ʹΪ�գ�ҲҪ֪ͨ��ȥ
//     SAFE_RELEASE(pMapChildObjAttrib);
//     return bRet;
// }


// ��ȡһ��δ���������ԡ����bErase==true���򽫷���һ����ʱ���ַ���ָ�룬������Ӧ�þ��챣����߽���ʱʹ��
const TCHAR*  Object::GetAttribute(const TCHAR* szKey, bool bErase)
{
    if (NULL == szKey || NULL == m_pIMapAttributeRemain)
        return NULL;

    return m_pIMapAttributeRemain->GetAttr(szKey, bErase);
}
void  Object::AddAttribute(const TCHAR* szKey, const TCHAR*  szValue)
{
    if (NULL == m_pIMapAttributeRemain) 
    {
        UICreateIMapAttribute(&m_pIMapAttributeRemain);
    }
    m_pIMapAttributeRemain->AddAttr(szKey, szValue);

}
void  Object::GetMapAttribute(IMapAttribute** ppMapAttribute)
{
    if (ppMapAttribute && m_pIMapAttributeRemain)
    {
        *ppMapAttribute = m_pIMapAttributeRemain;
        m_pIMapAttributeRemain->AddRef();
    }
}


//
//	��ȡ�����Լ������Ĵ�С
//
SIZE Object::GetDesiredSize()
{
    if (!m_pLayoutParam)
    {
       CreateLayoutParam();

       if (!m_pLayoutParam) // ���Ǵ������ɹ�������Combobox�е�Button����û�и�Panel-Layout
       {
           m_pLayoutParam = CanvasLayout::s_CreateLayoutParam(m_pIObject);
       }
    }

    return m_pLayoutParam->CalcDesiredSize();
}


// ����padding��ֵ��ͬʱ���·ǿͻ����Ĵ�С
void Object::SetPaddingRegion(CRegion4* prc)
{
	if (NULL == prc)
		return;

	REGION4 rcOther = { 
		m_rcNonClient.left   - m_rcPadding.left,
		m_rcNonClient.top    - m_rcPadding.top,
		m_rcNonClient.right  - m_rcPadding.right,
		m_rcNonClient.bottom - m_rcPadding.bottom };

	m_rcPadding.CopyRect(prc);
	m_rcNonClient.SetRect(
		m_rcPadding.left   + rcOther.left,
		m_rcPadding.top    + rcOther.top,
		m_rcPadding.right  + rcOther.right,
		m_rcPadding.bottom + rcOther.bottom );
}


void Object::SetBorderRegion( CRegion4* prc )
{
	if (NULL == prc)
		return;

	REGION4 rcOther = { 
		m_rcNonClient.left   - m_rcBorder.left,
		m_rcNonClient.top    - m_rcBorder.top,
		m_rcNonClient.right  - m_rcBorder.right,
		m_rcNonClient.bottom - m_rcBorder.bottom };

	m_rcBorder.CopyRect(prc);
	m_rcNonClient.SetRect(
		m_rcBorder.left   + rcOther.left,
		m_rcBorder.top    + rcOther.top,
		m_rcBorder.right  + rcOther.right,
		m_rcBorder.bottom + rcOther.bottom );
}

// Ҫ���Ƹö���֮ǰ����ȡ�ö����ڴ����е�ʵ��λ�ã���������ƫ�����Ͳü���

POINT Object::GetRealPosInWindow()
{
	int xOffset = 0;
	int yOffset = 0;

	Object* pObjParent = NULL;
	Object* pObjChild = this;
	while (pObjParent = this->EnumParentObject(pObjParent))
	{
        xOffset += pObjChild->GetParentRectL();
		yOffset += pObjChild->GetParentRectT();

		if (!pObjChild->IsNcObject())
		{
			xOffset += pObjParent->GetNonClientL();
			yOffset += pObjParent->GetNonClientT();

			int xScroll = 0, yScroll = 0;
			if (pObjParent->GetScrollOffset(&xScroll, &yScroll))
			{
				xOffset -= xScroll;
				yOffset -= yScroll;
			}
		}

		pObjChild = pObjParent;
	}

	POINT pt = {xOffset, yOffset};
	return pt;
}

// ��ȡһ�������ڴ����ϵĿ��������������ڻ��Ƹö���ʱ�Ĳü�
// Param:
//    bCalcInLayer: ������λ��һ��layer��ʱ���ػ�ö�����Ҫ��ȡ��ʱ�ö����ڲ��ϵ����򣬶�����
//                  �ڴ����ϵ����򡣵����Լ�Ҳ��һ������ڴ��ڵ�λ�ã����λ����Ҫ���ֶԴ�
bool Object::GetObjectVisibleRect(RECT* prc, bool bInWindowOrLayer)
{
	CRect  rcClip, rcTemp, rcParent;
	rcClip.SetRectEmpty();

	Object* pObjParent = NULL;;

	int xOffset = 0, yOffset = 0;

	bool bFirstParent = true;
	bool bBreak = false;
	while (!bBreak)
	{
		pObjParent = this->REnumParentObject(pObjParent);
		if (NULL == pObjParent)  // �����Ҫ�ټ���һ���Լ��ڸ������еķ�Χ
		{
			pObjParent = this;
			bBreak = true;
		}

		if (bFirstParent)
		{
            if (bInWindowOrLayer)
            {
			    pObjParent->GetParentRect(&rcClip);
            }
            else
            {
                rcClip.SetRect(0, 0, pObjParent->GetWidth(), pObjParent->GetHeight());
            }
			bFirstParent = false;
		}
		else
		{
			pObjParent->GetParentRect(&rcParent);

			// �����ǿͻ�����
			if (!pObjParent->IsNcObject())
			{
				CRegion4 rcNonRect;
				pObjParent->GetParentObject()->GetNonClientRegion(&rcNonRect);
				Util::DeflatRect(&rcClip, &rcNonRect);

				xOffset += rcNonRect.left; 
				yOffset += rcNonRect.top;

				// pObjParent�����ĸ��е�ֱ��λ�ã������˹�������)
				int xScroll = 0, yScroll= 0;
				pObjParent->GetParentObject()->GetScrollOffset(&xScroll, &yScroll);
				::OffsetRect(&rcParent, -xScroll, -yScroll);
			}

			// �����pObjParent�ڴ����е�λ��
			::OffsetRect(&rcParent, xOffset, yOffset);
			
			BOOL b = ::IntersectRect(rcTemp, &rcClip, &rcParent);
			if (false == b)
				return false;

			rcClip = rcTemp;

			xOffset = rcParent.left; 
			yOffset = rcParent.top;
		}
	}

	if (rcClip.Width() <= 0 || rcClip.Height() <= 0)
		return false;

	::CopyRect(prc, &rcClip);
	return true;
}

bool  Object::GetObjectVisibleClientRect(RECT* prc, bool bInWindowOrLayer)
{
    if (false == GetObjectVisibleRect(prc, bInWindowOrLayer))
        return false;

    prc->left += m_rcNonClient.left;
    prc->top  += m_rcNonClient.top;
    prc->right  -= m_rcNonClient.right;
    prc->bottom -= m_rcNonClient.bottom;
    return true;
}

HBITMAP  Object::TakeSnapshot()
{
    Image image;
    image.Create(m_rcParent.Width(), m_rcParent.Height(), 32, Image::createAlphaChannel);
    HDC hDC = image.BeginDrawToMyself();
    
    IRenderTarget*  pRenderTarget = ::UICreateRenderTarget(GetHWND());
    pRenderTarget->BindHDC(hDC);

    pRenderTarget->BeginDraw(NULL, 0);

    CRect rcRenderRegion(0, 0, image.GetWidth(), image.GetHeight());
    RenderContext roc(&rcRenderRegion, true);
    this->RealDrawObject(pRenderTarget, roc);
    pRenderTarget->EndDraw();

    pRenderTarget->Release();
    image.EndDrawToMyself();
    return image.Detach();
}

HBITMAP  Object::TakeBkgndSnapshot()
{
    Image image;
    image.Create(m_rcParent.Width(), m_rcParent.Height(), 32, Image::createAlphaChannel);
    HDC hDC = image.BeginDrawToMyself();

    IRenderTarget*  pRenderTarget = ::UICreateRenderTarget(GetHWND());
    pRenderTarget->BindHDC(hDC);

    pRenderTarget->BeginDraw(NULL, 0);

    CRect rcRenderRegion(0, 0, image.GetWidth(), image.GetHeight());
    RenderContext roc(NULL, false);
    CRect rcWindow;
    GetWindowRect(&rcWindow);
    roc.m_ptOffset.x = -rcWindow.left;
    roc.m_ptOffset.y = -rcWindow.top;
    roc.Update(pRenderTarget);
    roc.m_bUpdateClip = false;

    this->DrawObjectTransparentBkgnd(pRenderTarget, roc, true);
    pRenderTarget->EndDraw();

    pRenderTarget->Release();
    image.EndDrawToMyself();
    return image.Detach();
}

//
// �ݹ�. ���ö������Ӷ�����Ƶ�pRenderTarget����
// ��Ϊ�ӿؼ��ķ�Χ����Ҫ��parent����������������ȡ�Ӻ͸��Ľ���clip rgn���л���  
//

bool Object::DrawObject(IRenderTarget* pRenderTarget, RenderContext roc)
{
	if (this->IsMySelfVisible() && this->CanRedraw())
    {
        RealDrawObject(pRenderTarget, roc);
        return true;
    }
    return false;
}
// �ܶ�������治��Ҫ�ж��Լ��Ŀɼ��ԣ���Щ�����ƴ����DrawObject�����
void  Object::RealDrawObject(IRenderTarget* pRenderTarget, RenderContext roc)
{
    ::UISendMessage(this, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&roc);  // ��lparam��Ϊ1����ԭʼ��Ϣ��������
    this->_drawNcChildObject(pRenderTarget, roc);

    roc.DrawClient(m_pIObject);
    roc.Scroll(m_pIObject);
    roc.Update(pRenderTarget);

    ::UISendMessage(this, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)&roc);       // ��lparam��Ϊ1����ԭʼ��Ϣ��������
    this->DrawChildObject(pRenderTarget, roc);
}


void Object::DrawChildObject(IRenderTarget* pRenderTarget, RenderContext& roc)
{
	Object*  pChild = NULL;
	while (pChild = EnumChildObject(pChild))
	{
		RenderContext rocChild = roc;
		if (rocChild.DrawChild(pChild->GetIObject()))
        {
            rocChild.Update(pRenderTarget);
			pChild->DrawObject(pRenderTarget, rocChild);
        }
	}
}


void Object::_drawNcChildObject(IRenderTarget* pRenderTarget, RenderContext& roc)
{
	Object*  pChild = NULL;
	while (pChild = EnumNcChildObject(pChild))
	{
		RenderContext rocChild = roc;
		if (rocChild.DrawChild(pChild->GetIObject()))
        {
            rocChild.Update(pRenderTarget);
			pChild->DrawObject(pRenderTarget, rocChild);
        }
	}
}

// �����͸�������Ļ�����Ҫ��ȡ�������еı���
// ����ʹ�ǲ�͸����object��Ҳ��Ҫһ������������ȡ���ļ�������
//
//   Ϊ����Ӧ�ڶ������ص�ʱ���ܹ�ˢ���Լ��ĸ����󱳾�����Ҫ����һ�������������Լ���͸������
//   �������bSelfTransparent������ͨˢ�µ�ʱ�򴫵�this->IsTransparen()������ʱ��ֱ�Ӵ���false

void Object::DrawObjectTransparentBkgnd(IRenderTarget* pRenderTarget, RenderContext& roc, bool bSelfTransparent)
{
	bool    bTransparent = bSelfTransparent;
	Object* pObjFirstParentToDrawBk = NULL;
	
	if (bTransparent)
	{
		// ����游��������û�в�͸���ģ��Ӹø�������
		Object* pObjParent = NULL;
		while (pObjParent = this->EnumParentObject(pObjParent))
		{
			if (!pObjParent->IsTransparent())
			{
				pObjFirstParentToDrawBk = pObjParent;
				break;
			}
		}
	}
	else
	{
		pObjFirstParentToDrawBk = this;   // ��Ȼ����Ҫ���ƣ���������Ҫ���ݸ��������ƫ��
	}

	Object* pObjParent = NULL;
	while (pObjParent = this->REnumParentObject(pObjParent))
	{
        OBJ_TYPE eType = pObjParent->GetObjectType();
		if (OBJ_WINDOW != eType && OBJ_LAYER != eType)
		{
			if (!pObjParent->IsNcObject())
			{
                Object* pParent = pObjParent->GetParentObject();
                if (pParent)
                {
				    roc.Scroll(pParent->GetIObject());
				    roc.DrawClient(pParent->GetIObject());
                }
			}
			roc.DrawChild(pObjParent->GetIObject());
            roc.Update(pRenderTarget);
		}

		if (pObjFirstParentToDrawBk)
		{
			if (pObjParent != pObjFirstParentToDrawBk)  // ��pObjFirstParentToDrawBk��ʼ����
				continue;

			pObjFirstParentToDrawBk = NULL;             // ������
		}

		::UISendMessage(pObjParent, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&roc );
	}

	if (!this->IsNcObject())
	{
        Object* pParent = this->GetParentObject();
        if (pParent)
        {
		    roc.Scroll(pParent->GetIObject());
		    roc.DrawClient(pParent->GetIObject());
        }
	}
	roc.DrawChild(m_pIObject);
    roc.Update(pRenderTarget);
}

//
// ��ȡ�ö����ƫ����
//
// ReturnQ
//		����false��ʾ�ö����޹�������
//

bool Object::GetScrollOffset(int* pxOffset, int* pyOffset)
{
	if (NULL == pxOffset || NULL == pyOffset)
		return false;

	*pxOffset = 0; *pyOffset = 0;

	bool bHScroll = this->TestStyle(OBJECT_STYLE_HSCROLL);
	bool bVScroll = this->TestStyle(OBJECT_STYLE_VSCROLL);
	if (bHScroll || bVScroll)
	{
		::UISendMessage(this, UI_WM_GETSCROLLOFFSET, (WPARAM)pxOffset, (LPARAM)pyOffset);
		return true;
	}

	return false;
}


bool Object::GetScrollRange(int* pxRange, int* pyRange)
{
	if (NULL == pxRange || NULL == pyRange)
		return false;

	*pxRange = 0; *pyRange = 0;

	bool bHScroll = this->TestStyle(OBJECT_STYLE_HSCROLL);
	bool bVScroll = this->TestStyle(OBJECT_STYLE_VSCROLL);
	if (bHScroll || bVScroll)
	{
		::UISendMessage(this, UI_WM_GETSCROLLRANGE, (WPARAM)pxRange, (LPARAM)pyRange);
		return true;
	}

	return false;
}


void Object::WindowPoint2ObjectPoint(const POINT* ptWindow, POINT* ptObj)
{
	if (NULL == ptObj || NULL == ptWindow)
		return;

	POINT pt = this->GetRealPosInWindow();
	ptObj->x = ptWindow->x - pt.x;
	ptObj->y = ptWindow->y - pt.y;
}


void Object::WindowPoint2ObjectClientPoint(const POINT* ptWindow, POINT* ptClient)
{
	if (NULL == ptClient || NULL == ptWindow)
		return;

	POINT pt = this->GetRealPosInWindow();
	ptClient->x = ptWindow->x - pt.x - m_rcNonClient.left;
	ptClient->y = ptWindow->y - pt.y - m_rcNonClient.top;
}


void Object::WindowPoint2ObjectClientPoint_CalcScroll(const POINT* ptWindow, POINT* ptObj)
{
	if (NULL == ptObj || NULL == ptWindow)
		return;

	this->WindowPoint2ObjectClientPoint(ptWindow, ptObj);

	int x = 0, y = 0;
	this->GetScrollOffset(&x, &y);
	ptObj->x += x;
	ptObj->y += y;
}


void Object::ObjectPoint2ObjectClientPoint(const POINT* ptObj, POINT* ptClient)
{
	if (NULL == ptObj || NULL == ptClient)
		return;

	ptClient->x = ptObj->x - m_rcNonClient.left;
	ptClient->y = ptObj->y - m_rcNonClient.top;
}


void Object::ClientRect2ObjectRect(const RECT* rcClient, RECT* rcObj)
{
	if (NULL == rcClient || NULL == rcObj)
		return;

	CopyRect(rcObj, rcClient);
	::OffsetRect(rcObj, m_rcNonClient.left, m_rcNonClient.top);
}


void Object::GetWindowRect(CRect* lprc)
{
	if (NULL == lprc)
		return;
	
	lprc->SetRect(0,0,GetWidth(), GetHeight());
	POINT pt = this->GetRealPosInWindow();
	lprc->OffsetRect(pt.x, pt.y);
}

void*  Object::QueryInterface(const IID* pIID)
{
    if (IsEqualIID(*pIID, uiiidof(IObject)) ||
        IsEqualIID(*pIID, uiiidof(IMessage)) )
    {
        return m_pIObject;
    }

    return NULL;
}

void Object::OnEraseBkgnd(IRenderTarget* pRenderTarget)
{
	if (m_pBkgndRender)
	{
		CRect rc(0,0, this->GetWidth(), this->GetHeight());
		m_pBkgndRender->DrawState(pRenderTarget, &rc, 0);
	}
}

//
//	����Object��Ĭ�������ʽ
//
//	Remark:
//		�����ʵ�ָ���Ϣ�����ܵ�������ƶ���һ��Object����ʱ����Ȼ��ʾ����Window��OnSetCursor
//

BOOL Object::OnSetCursor(HWND hWnd, UINT nHitTest, UINT message)
{
	if (NULL == m_pCursor || NULL == m_pCursor->GetCursor())
	{
		::SetCursor(::LoadCursor(NULL, IDC_ARROW));
 		return TRUE;
	}

	::SetCursor(m_pCursor->GetCursor());
	return TRUE;
}

//	��ȡ�Լ�������,���ﷵ�صĶ���ֻ��һ����ʱ����
//	�����Ҫ����ʹ�ã�����Ҫ����AddRef
IRenderFont* Object::GetRenderFont()
{
    IRenderFont* pRenderFont = NULL;

    // ���Լ���textrender��ȡ
    if (m_pTextRender)
        pRenderFont = m_pTextRender->GetRenderFont();

    if (pRenderFont)
        return pRenderFont;

    // �򴰿ڻ�ȡĬ��
    WindowBase* pWindow = GetWindowObject();
    if (pWindow)
        return pWindow->GetWindowDefaultRenderFont();

    return NULL;
}

//
//	[public] [virtual]  UINT HitTest( POINT* pt )
//
//	Parameters
//		pt
//			[in]  Ҫ��̽��λ�ã�ͨ��Ϊ��굱ǰλ��
//
//	Return
//		��겻�ڶ������о�Χ�ڷ���HTNOWHERE�����򷵻�HTCLIENT�����������������չ��
//
//	Remark
//		����������������һ�����⣬��ΪObject�䱸һ��RECT�����ʾ��Χ�����Ǹ�һ����ǿ���RGN��ʾ��Χ?
//		����RGN���������ڴ棬����ʱ��͵�ʹ��RGN��ʵ��ĳЩ���ܣ������������������������ߵ����ߣ�
//		�����λ�ô�����ʲô��λ��ͬʱ��Object�䱸һ��RECT������ʾÿһ������ķ�Χ����Ϊ�󲿷�ʱ��ʹ�õĶ���RECT����
//

UINT Object::OnHitTest(POINT* pt)
{
	BOOL  bIn = FALSE;

	if (this->m_hRgn != NULL)
	{
		bIn = ::PtInRegion(m_hRgn, pt->x, pt->y);
		return HTCLIENT;
	}
	else
	{
		bIn = ::PtInRect(&m_rcParent, *pt);

		if (bIn)
		{
			CRect rcClient;
			this->GetClientRect(&rcClient);

			if (::PtInRect(&rcClient, *pt))
				return HTCLIENT;
			else
				return HTBORDER;
		}
	}

	return HTNOWHERE;
}

void Object::OnThemeChanged()
{
	SetMsgHandled(FALSE);
    
    if (m_pBkgndRender)
        m_pBkgndRender->CheckThemeChanged();
    if (m_pForegndRender)
        m_pForegndRender->CheckThemeChanged();

#if 0 // -- �ܹ�����
	ON_RENDER_THEME_CHANGED_TEXT(m_pTextRender);
#endif
}

LRESULT  Object::OnSkinMaterialChanged(UINT, WPARAM, LPARAM)
{
    if (m_pTextRender)
    {
        m_pTextRender->CheckSkinMaterialChanged();
    }
    return 0;
}

void  Object::OnVisibleChanged(BOOL bVisible, IObject* pObjChanged)
{
//     if (!bVisible)
//     {
//         // ��ֹ������;���߻��������У�һЩ����ͻȻ�����٣����µ�Ұָ�����
//         WindowBase* pWindow = GetWindowObject();
//         if (pWindow)  // ������DestroyChildObjectǰ���ã�����parent��ϵ�����
//         {
//             pWindow->OnObjectHideInd(this);
//         }
//     }

    // ������صĶ�����һ����������򽫽��������лص���һ������
    // ע�������objectҲ������λ��һ��listctrl�У������Ӧ��mouse/keyboard
    //     manager�����Ǵ��ڵģ�����listctrl�Լ���
    if (false == bVisible)
    {
        Object*  pParent = m_pParent;
        IMKMgr*  pMKMgr = NULL;
        while (pParent)
        {
            pMKMgr = (IMKMgr*)UISendMessage(pParent->GetIMessage(), UI_WM_GET_MOUSEKEYBOARD_MANAGER);
            if (pMKMgr)
            {
                pMKMgr->OnObjectHideInd(this);
                break;
            }

            pParent = pParent->GetParentObject();
        }
    }
}

// Ĭ��ʵ��Ϊ����ˢ�¡�
// ����Ϣ������ͨ��PostMessage�����ӳ�ˢ�£�����ֻ���õ�һ��IMessage����ʱ����
void  Object::OnRedrawObject()
{
    this->UpdateObject();
}
/*
**	[private] bool    testStateBit( BYTE nbit );
**	[private] bool    setStateBit( BYTE nbit );
**
**	parameters
**		nbit
**			[in] Ҫ���Եı���λ����OSB_VISIBLE��OSB_DISABLE��CSB_TABABLE��CSB_DEFAULT
**				 CSB_FOCUS��CSB_READONLY��CSB_HOVER��CSB_PRESS
**	
**	return
**		��λΪ1������true����λΪ0������false
**
**	remark
**		Ϊ�˽�ʡ�ڴ�ռ�ã���һ���ؼ��Ķ���״̬����Ϊbitλ���д���
*/

bool Object::testStateBit( UINT bit )
{
	if (m_nStateBit & bit )
		return true;
	else 
		return false;
}

void Object::setStateBit( UINT bit )
{
	m_nStateBit |= bit;
}

void Object::clearStateBit( UINT nbit )
{
	m_nStateBit &= ~nbit;
}


void Object::ModifyStyle( UINT nStyleAdd, UINT nStyleRemove, bool bNotify )
{
	STYLESTRUCT s;
	s.styleOld = m_nStyle;
	s.styleNew = m_nStyle;

	UINT nOldStyle = m_nStyle;
	if (nStyleAdd != 0)
	{
		s.styleNew |= nStyleAdd;
	}
	if (nStyleRemove != 0)
	{
		s.styleNew &= ~nStyleRemove;
	}

	if (s.styleNew == s.styleOld)
		return;

	if (bNotify)
	{
		::UISendMessage(this, WM_STYLECHANGING, GWL_STYLE, (LPARAM)&s);
	}

	m_nStyle = s.styleNew;

	if (bNotify)
	{
		s.styleOld = s.styleOld;
		::UISendMessage(this, WM_STYLECHANGED, GWL_STYLE, (LPARAM)&s);
	}
}

void Object::ModifyStyleEx(UINT nStyleAdd, UINT nStyleRemove, bool bNotify)
{
    STYLESTRUCT s;
    s.styleOld = m_nStyle2;
    s.styleNew = m_nStyle2;

    UINT nOldStyle = m_nStyle2;
    if (nStyleAdd != 0)
    {
        s.styleNew |= nStyleAdd;
    }
    if (nStyleRemove != 0)
    {
        s.styleNew &= ~nStyleRemove;
    }

    if (s.styleNew == s.styleOld)
        return;

    if (bNotify)
    {
        ::UISendMessage(this, WM_STYLECHANGING, GWL_EXSTYLE, (LPARAM)&s);
    }

    m_nStyle2 = s.styleNew;

    if (bNotify)
    {
        s.styleOld = s.styleOld;
        ::UISendMessage(this, WM_STYLECHANGED, GWL_EXSTYLE, (LPARAM)&s);
    }
}

bool Object::IsTransparent()
{
	if (m_nStyle & OBJECT_STYLE_TRANSPARENT)
		return true;

	return false;
}
void  Object::SetTransparent(bool b)
{
    if (b)
        m_nStyle |= OBJECT_STYLE_TRANSPARENT;
    else
        m_nStyle &= ~OBJECT_STYLE_TRANSPARENT;
}


bool Object::IsFocus()
{
	return this->testStateBit(OSB_FOCUS);
}


void Object::SetFocus(bool b, bool bNoitfy)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_FOCUS);
	else
		this->clearStateBit(OSB_FOCUS);
	
    if (bNoitfy && nOld != m_nStateBit)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FOCUS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }

}


bool Object::IsTabstop()
{
	return (m_nStyle & CONTROL_STYLE_TABSTOP)?true:false;
}


bool Object::CanTabstop()
{
	return this->IsTabstop() && this->IsEnable() && this->IsVisible();
}

bool Object::IsGroup()
{
	return  (m_nStyle&CONTROL_STYLE_GROUP)?true:false;
}


void Object::SetTabstop(bool b)
{
	if (b)
		this->ModifyStyle(CONTROL_STYLE_TABSTOP, 0, false);
	else
		this->ModifyStyle(0, CONTROL_STYLE_TABSTOP, false);
}

void Object::SetGroup(bool b)
{
	if (b )
		this->ModifyStyle(CONTROL_STYLE_GROUP, 0, false);
	else
		this->ModifyStyle(0, CONTROL_STYLE_GROUP, false);
}


bool Object::IsMySelfVisible()
{
	bool bVisible = !this->testStateBit( OSB_UNVISIBLE );
	return bVisible;
}

bool Object::IsVisible()
{
    int nType = this->GetObjectType();
	if (nType == OBJ_WINDOW)
	{
		return ::IsWindowVisible(((WindowBase*)this)->m_hWnd)?true:false;
	}
	if (nType == OBJ_HWNDHOST)
	{
		return ::IsWindowVisible(((HwndHost*)this)->m_hWnd)?true:false;
	}

	bool bVisible = !this->testStateBit(OSB_UNVISIBLE);
	if (false == bVisible || NULL == m_pParent)
	{
		return bVisible;
	}
	// �жϸ����󲻿ɼ������ 
	else
	{
		if (OBJ_WINDOW == m_pParent->GetObjectType())
		{ 
			return true;           // ע�������ﲻ�������Ĵ��ڽ����жϵ�ԭ���ǣ��������ڴ��ڳ�ʼ���ĺ������棬��Ȼ������ʱ�ǲ��ɼ��ģ�������Ķ����IsVisibleӦ���Ƿ���true�źô���
		}
		else
		{
			return m_pParent->IsVisible();
		}
	}
    return true;
}


bool Object::IsCollapsed()
{
	// TODO:  
	UIASSERT(0);
	return false;
}


bool Object::IsEnable()
{
    int nObjType = this->GetObjectType();
	if (nObjType == OBJ_WINDOW)
	{
		return ::IsWindowEnabled(((WindowBase*)this)->m_hWnd)?true:false;
	}
	if (nObjType == OBJ_HWNDHOST)
	{
		return ::IsWindowEnabled(((HwndHost*)this)->m_hWnd)?true:false;
	}
	return !this->testStateBit(OSB_DISABLE);
}

//
//	��ע���ֲ㴰���У�����һ������ʱ������ֱ�ӵ���::InvalidateRect(&rc..)
//        ������� GetWindowObject(); pWindow->InvalidateObject(GetWindowObject(),...);
//

void Object::SetVisible(bool b, bool bRedraw, bool bUpdateLayout)
{
	bool bOld = IsVisible();

    int nObjType = this->GetObjectType();
	if (nObjType == OBJ_WINDOW)
	{
		::ShowWindow(((WindowBase*)this)->m_hWnd, b?SW_SHOW:SW_HIDE);
		return;
	}
	else if (nObjType == OBJ_HWNDHOST)
	{
		::ShowWindow(((HwndHost*)this)->m_hWnd, b?SW_SHOW:SW_HIDE);
		return;
	}

	if (b)
		this->clearStateBit(OSB_UNVISIBLE);
	else
		this->setStateBit(OSB_UNVISIBLE);

	// ֪ͨ�Ӷ���
	UISendMessage(this, UI_WM_VISIBLE_CHANGED, b?TRUE:FALSE, (LPARAM)this);

    UIMSG msg;
    msg.message = UI_WM_VISIBLE_CHANGED;
    msg.wParam = b?TRUE:FALSE;
    msg.lParam = (LPARAM)this;
	Message::ForwardMessageToChildObject(this, &msg);

	if (b != bOld)
	{
		if (bUpdateLayout)
		{
			this->UpdateLayout(bRedraw);
		}
		else
		{
			if(bRedraw)
			{
				if (b)
					this->UpdateObject(); 
				else
					this->UpdateObjectBkgnd(true); 
			}
		}
	}
}

void Object::SetEnable(bool b, bool bUpdateNow, bool bNoitfy)
{
    int nObjType = this->GetObjectType();
	if (nObjType == OBJ_WINDOW )
	{
		::EnableWindow(((WindowBase*)this)->m_hWnd, b?TRUE:FALSE);
		return;
	}
	else if (nObjType == OBJ_HWNDHOST )
	{
		::EnableWindow(((HwndHost*)this)->m_hWnd, b?TRUE:FALSE);
		return;
	}

	bool bOld = IsEnable();
	int  nOldBits = m_nStateBit;
	if (b)
		this->clearStateBit(OSB_DISABLE);
	else
		this->setStateBit(OSB_DISABLE);
	
    if (bNoitfy && b != bOld)
	    UISendMessage(GetIMessage(), UI_WM_STATECHANGED2, (WPARAM)OSB_DISABLE);

	if (b != bOld && bUpdateNow)
		this->UpdateObject();
}


bool Object::IsDefault()
{
	return this->testStateBit( OSB_DEFAULT );
}

// bool Object::IsReadonly()
// {
// 	return this->testStateBit(OSB_READONLY);
// }

bool Object::IsHover()
{
	return this->testStateBit(OSB_FORCEHOVER) || this->testStateBit(OSB_HOVER);
}

bool Object::IsPress()
{
	return this->testStateBit(OSB_FORCEPRESS) || this->testStateBit(OSB_PRESS);
}

bool Object::IsForceHover()
{
	return this->testStateBit(OSB_FORCEHOVER);
}

bool Object::IsForcePress()
{
	return this->testStateBit(OSB_FORCEPRESS);
}
bool  Object::IsSelected()
{
    return this->testStateBit(OSB_SELECTED);
}

void  Object::SetDefault(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_DEFAULT);
	else
        this->clearStateBit(OSB_DEFAULT);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_DEFAULT;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void  Object::SetSelected(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
    if (b)
        this->setStateBit(OSB_SELECTED);
    else
        this->clearStateBit(OSB_SELECTED);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_SELECTED;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

// void Object::SetReadonly(bool b)
// {
// 	if (b)
// 		this->setStateBit(OSB_READONLY);
// 	else
// 		this->clearStateBit(OSB_READONLY);
// }


void Object::SetForceHover(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;

	if (b)
		this->setStateBit(OSB_FORCEHOVER);
	else
		this->clearStateBit(OSB_FORCEHOVER);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FORCEHOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetForcePress(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_FORCEPRESS);
	else
		this->clearStateBit(OSB_FORCEPRESS);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_FORCEPRESS;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetHover(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_HOVER);
	else
		this->clearStateBit(OSB_HOVER);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}


void Object::SetPress(bool b, bool bNotify)
{
    UINT nOld = m_nStateBit;
	if (b)
		this->setStateBit(OSB_PRESS);
	else
		this->clearStateBit(OSB_PRESS);

    if (bNotify && m_nStateBit != nOld)
    {
        UIMSG  msg;
        msg.message = UI_WM_STATECHANGED2;
        msg.wParam = OSB_HOVER;
        msg.pMsgTo = GetIMessage();
        UISendMessage(&msg);
    }
}

void  Object::SetAsNcObject(bool b) 
{
    if (b)
        this->ModifyStyle(OBJECT_STYLE_NCOBJ, 0, 0); 
    else
        this->ModifyStyle(0, OBJECT_STYLE_NCOBJ, 0); 
}
bool  Object::IsNcObject()   
{
    return TestStyle(OBJECT_STYLE_NCOBJ);
}
bool  Object::IsRejectMouseMsgAll()  
{
    return TestStyle(OBJECT_STYLE_REJECT_MOUSE_MSG_ALL); 
}
bool  Object::IsRejectMouseMsgSelf() 
{
    return TestStyle(OBJECT_STYLE_REJECT_MOUSE_MSG_SELF); 
}


int Object::GetWidth()
{
	return m_rcParent.Width();
}


int Object::GetHeight()
{
	return m_rcParent.Height();
}


int Object::GetWidthWithMargins()
{
	return m_rcParent.Width() + m_rcMargin.left + m_rcMargin.right;
}


int Object::GetHeightWithMargins()
{
	return m_rcParent.Height() + m_rcMargin.top + m_rcMargin.bottom;
}

// ע�������clientrect ������0,0���ģ����������Object���ϽǶ���

void Object::GetClientRect(CRect* prc)
{
	if (NULL == prc)
		return;

	prc->SetRect(
		m_rcNonClient.left, 
		m_rcNonClient.top, 
		this->GetWidth() - m_rcNonClient.right, 
		this->GetHeight()- m_rcNonClient.bottom); 
}

// ע�������clientrect ��0,0����

void Object::GetClientRectAsWin32( CRect* prc )
{
	if (NULL == prc)
		return;

	prc->SetRect(
		0, 
		0, 
		this->GetWidth() - m_rcNonClient.left - m_rcNonClient.right, 
		this->GetHeight()- m_rcNonClient.top - m_rcNonClient.bottom); 
}

// clientrect�ڴ����е�����
void Object::GetClientRectInWindow( CRect* prc )
{
	if (NULL == prc)
		return;

	this->GetWindowRect(prc);
	prc->left += m_rcNonClient.left;
	prc->top  += m_rcNonClient.top;
	prc->right  -= m_rcNonClient.right;
	prc->bottom -= m_rcNonClient.bottom;
}

//
// �����Լ���nc object�������Լ���non client region
//
void Object::UpdateObjectNonClientRegion()
{
	CRegion4  rcNonClient(0,0,0,0);

    UIMSG msg;
	msg.message = UI_WM_CALC_PARENT_NONCLIENTRECT;
	msg.wParam = (WPARAM)&rcNonClient;
	msg.pMsgFrom = this->GetIMessage();

	Object* pNcChild = NULL;
	while (pNcChild = this->EnumNcChildObject(pNcChild))
	{
		msg.pMsgTo = pNcChild->GetIMessage();
		UISendMessage(&msg);
	}

	this->SetNonClientRegionExcludePaddingBorder(&rcNonClient);
}


void Object::SetNonClientRegionExcludePaddingBorder( CRegion4* prc )
{
	m_rcNonClient.SetRect(
		prc->left   + m_rcPadding.left   + m_rcBorder.left,
		prc->top    + m_rcPadding.top    + m_rcBorder.top,
		prc->right  + m_rcPadding.right  + m_rcBorder.right,
		prc->bottom + m_rcPadding.bottom + m_rcBorder.bottom);
}


void Object::SetNonClientRegionExcludePaddingBorderL( int n )
{
	m_rcNonClient.left = n + m_rcPadding.left + m_rcBorder.left;
}

void Object::SetNonClientRegionExcludePaddingBorderT( int n )
{
	m_rcNonClient.top = n + m_rcPadding.top + m_rcBorder.top;
}

void Object::SetNonClientRegionExcludePaddingBorderB( int n )
{
	m_rcNonClient.bottom = n + m_rcPadding.bottom + m_rcBorder.bottom;
}

void Object::SetNonClientRegionExcludePaddingBorderR( int n )
{
	m_rcNonClient.right = n + m_rcPadding.right + m_rcBorder.right;
}


void Object::GetParentRect( CRect* prc )
{
	if (NULL == prc)
		return;

	prc->CopyRect(&m_rcParent);
}

ILayoutParam*  Object::GetLayoutParam()
{
    return m_pLayoutParam;
}
void  Object::CreateLayoutParam()
{
    SAFE_RELEASE(m_pLayoutParam);

    if (!m_pParent)
        return;

    ILayout* pLayout = (ILayout*)UISendMessage(m_pParent, UI_WM_GETLAYOUT);
    if (!pLayout)
        return;

    m_pLayoutParam = pLayout->CreateLayoutParam(m_pIObject);
}

void  Object::SetLayoutParam(ILayoutParam* p)
{
    SAFE_RELEASE(m_pLayoutParam);
    m_pLayoutParam = p;
}
int   Object::GetConfigWidth()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigWidth();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_WIDTH, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigHeight()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigHeight();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_HEIGHT, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigLayoutFlags()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return 0;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigLayoutFlags();
    }
    else
    {
        const TCHAR* szText = m_pIMapAttributeRemain->GetAttr(XML_LAYOUT_ITEM_ALIGN, false);
        if (!szText)
            return 0;

        return CanvasLayoutParam::ParseAlignAttr(szText);
    }
}
int   Object::GetConfigLeft()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigLeft();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_LEFT, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigRight()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigRight();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_RIGHT, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigTop()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigTop();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_TOP, false, &nRet);
        return nRet;
    }
}
int   Object::GetConfigBottom()
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() != LAYOUT_TYPE_CANVAS)
            return NDEF;
        else
            return static_cast<CanvasLayoutParam*>(m_pLayoutParam)->GetConfigBottom();
    }
    else
    {
        int nRet = NDEF;
        m_pIMapAttributeRemain->GetAttr_int(XML_LAYOUT_ITEM_BOTTOM, false, &nRet);
        return nRet;
    }
}

void  Object::SetConfigWidth(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigWidth(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigWidth(n);
    }
}
void  Object::SetConfigHeight(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigHeight(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigHeight(n);
    }
}
void  Object::SetConfigLayoutFlags(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigLayoutFlags(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigLayoutFlags(n);
    }
}
void  Object::SetConfigLeft(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigLeft(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigLeft(n);
    }
}
void  Object::SetConfigRight(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigRight(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigRight(n);
    }
}
void  Object::SetConfigTop(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigTop(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigTop(n);
    }
}
void  Object::SetConfigBottom(int n)
{
    if (m_pLayoutParam)
    {
        if (m_pLayoutParam->GetLayoutType() == LAYOUT_TYPE_CANVAS)
            static_cast<CanvasLayoutParam*>(m_pLayoutParam)->SetConfigBottom(n);
    }
    else
    {
        CanvasLayout::s_GetObjectLayoutParam(this)->SetConfigBottom(n);
    }
}

void Object::SetObjectPos(int x, int y, int cx, int cy, int nFlag)
{
    bool bMove = (nFlag&SWP_NOMOVE)?false:true;
    bool bSize = (nFlag&SWP_NOSIZE)?false:true;

    //  TODO: ������һ�δ���ᵼ��SetVisible�еĲ��ֳ������⣬���pp -> p -> o��o�����仯
    //  ��pȥ���²��֣�Ȼ��p�ټ���Լ���desiredsize�仯�ˣ�����pp�������²��֣����pp�Ĵ�
    //  Сû�з����仯������p����ȥ���²��֣�oˢ��ʧ�ܡ�
//     if (bMove && 
//         x == m_rcParent.left &&
//         y == m_rcParent.top && 
//         bSize && 
//         cx == m_rcParent.Width() && 
//         cy == m_rcParent.Height())
//         return;

	nFlag |= SWP_NOZORDER;   // �ú������ṩ�޸�ZORDER�Ĺ���
	WINDOWPOS wndpos =  {NULL, NULL, x, y, cx, cy, nFlag};
	UISendMessage(this, WM_WINDOWPOSCHANGING, 0, (LPARAM)&wndpos);
	x = wndpos.x;
	y = wndpos.y;
	cx = wndpos.cx;
	cy = wndpos.cy;
	nFlag = wndpos.flags;

	if (bMove&&bSize)
	{
		
	}
	else if (bMove)
	{
		cx = this->GetWidth();
		cy = this->GetHeight();
	}
	else if (bSize)
	{
		x = m_rcParent.left;
		y = m_rcParent.top;
	}
	else
	{
		return;  // DONOTHING
	}

	RECT rcOldVisibleRect = {0};
	if (bMove || bSize) 
	{
		// ˢ���ƶ�ǰ������λ��
		if (!(nFlag & SWP_NOREDRAW))
		{
			this->GetObjectVisibleRect(&rcOldVisibleRect, true);  // ��ȡ�µ�ǰ��ˢ�µ�����Χ�����ں�������ύ
			this->UpdateObjectBkgnd(false);  // ���ﲻ������ˢ�£���������ٵ���UpdateObject���������˸���ƶ����λ�����غϵ�����£�
		}
	}

    int nObjType = this->GetObjectType();
	if (nObjType == OBJ_WINDOW)
	{
		// ���ڴ�����˵���������õ��Ƿǿͻ����Ĵ�С
		WindowBase* pThis = (WindowBase*)this;
		::SetWindowPos(pThis->GetHWND(), NULL, x, y, cx, cy, SWP_NOZORDER|SWP_NOACTIVATE);
		::GetClientRect(pThis->GetHWND(), &m_rcParent);
		return;
	}
	else if (nObjType == OBJ_HWNDHOST)
	{
		HwndHost* pThis = (HwndHost*)this;
		::SetWindowPos(pThis->m_hWnd, NULL, x, y, cx, cy, SWP_NOZORDER|SWP_NOACTIVATE);
		::GetClientRect(pThis->m_hWnd, &m_rcParent);
		return;
	}
	else
	{
		::SetRect(&m_rcParent, x,y,x+cx,y+cy);
	}
    if (!(nFlag&SWP_NOUPDATELAYOUTPOS))
    {
        UpdateLayoutPos();
    }

	// MSDN: MoveWindow sends the WM_WINDOWPOSCHANGING, WM_WINDOWPOSCHANGED, WM_MOVE, WM_SIZE, and WM_NCCALCSIZE messages to the window. 
	// ������������ʱֻ�ȷ���WM_MOVE/WM_SIZE��Ϣ
    if (!(nFlag & SWP_NOSENDCHANGING))
    {
	    if (bMove)
		    UISendMessage(this, WM_MOVE, 0, MAKELPARAM(m_rcParent.left,m_rcParent.top));
	    if (bSize)
		    UISendMessage(this, WM_SIZE, 0, MAKELPARAM(m_rcParent.Width(),m_rcParent.Height()));

	    WINDOWPOS wndpos2 =  {NULL, NULL, x, y, cx, cy, nFlag};
	    UISendMessage(this, WM_WINDOWPOSCHANGED, NULL, (LPARAM)&wndpos2);
    }

	if (bMove || bSize)
	{
		// ˢ���ƶ��������λ��
		if (!(nFlag & SWP_NOREDRAW))
		{
			this->UpdateObject();

			// ���ƶ�֮ǰ�����������ύ�������ϣ�������˸
			this->GetWindowObject()->CommitDoubleBuffet2Window(NULL, &rcOldVisibleRect);
		}
	}
}


void Object::SetObjectPos(CRect* prc, int nFlag)
{
	if (NULL == prc)
		return;

	this->SetObjectPos(prc->left, prc->top, prc->Width(), prc->Height(), nFlag);
}

// ����m_rcParent���� m_nConfigLeft/m_nConfigRight/m_nConfigTop/m_nConfigBottom/m_nConfigLayoutFlags
void Object::UpdateLayoutPos()
{
//     if (!m_pParent)
//         return;
//     
//     ILayout* pLayout = (ILayout*)UISendMessage(m_pParent, UI_WM_GETLAYOUT);
//     if (!pLayout)
//         return;
// 
//     pLayout->UpdateObjectLayoutPos(m_pIObject);
    
    if (m_pLayoutParam)
    {
        m_pLayoutParam->UpdateByRect();
    }
}

// ���ֶ�����һ�����󣨷Ǵ�xml�м���ʱ�����Ե��øú�����ȫĬ�����Եļ���)
void  Object::InitDefaultAttrib()
{
    IMapAttribute* pMapAttrib = NULL;
    UICreateIMapAttribute(&pMapAttrib);

    // ������ʽ
    if (m_pUIApplication)
    {
        ISkinRes* pSkinRes = m_pUIApplication->GetActiveSkinRes();
        if (pSkinRes)
        {
            IStyleManager* pStyleMgr = pSkinRes->GetStyleManager();
            if (pStyleMgr)
            {
                pStyleMgr->LoadStyle(GetObjectName(), NULL, m_strID.c_str(), pMapAttrib);
            }
        }
    }

	SERIALIZEDATA data = {0};
	data.pMapAttrib = pMapAttrib;
	data.nFlag = SERIALIZEFLAG_LOAD;
	UISendMessage(m_pIObject, UI_WM_SERIALIZE, (WPARAM)&data);
//    UISendMessage(m_pIObject, UI_WM_SETATTRIBUTE, (WPARAM)pMapAttrib, 0);

    SAFE_RELEASE(pMapAttrib);
}
void Object::SetUserData(LPVOID p)
{
	m_pUserData = p;
}


LPVOID Object::GetUserData()
{
	return m_pUserData;
}


void  Object::SetOutRef(void** ppOutRef)
{
	m_ppOutRef = ppOutRef;
}


void  Object::SetUIApplication(IUIApplication* p)
{ 
	m_pUIApplication = p;
}


void Object::SetBkgndRender(IRenderBase* p)
{
	SAFE_RELEASE(m_pBkgndRender);
	m_pBkgndRender = p;

	if (m_pBkgndRender)
		m_pBkgndRender->AddRef();
}


void Object::SetForegndRender(IRenderBase* p)
{
	SAFE_RELEASE(m_pForegndRender);
	
	m_pForegndRender = p;
	if (m_pForegndRender)
		m_pForegndRender->AddRef();
}


void Object::SetTextRender(ITextRenderBase* p)
{
	SAFE_RELEASE(m_pTextRender);
	
	m_pTextRender = p;
	if (m_pTextRender)
		m_pTextRender->AddRef();
}

ITextRenderBase*  Object::GetTextRender()
{
    return m_pTextRender; 
}
IRenderBase*  Object::GetBkRender() 
{ 
    return m_pBkgndRender; 
}
IRenderBase*  Object::GetForeRender()
{
    return m_pForegndRender; 
}
//////////////////////////////////////////////////////////////////////////
//                                                                      //
//                               ��Ϣӳ��                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//
//   �����յ�WM_DESTROY��Ϣ֮�󣬴��ݸ����е��Ӷ���
//
//void Object::OnDestroy()
//{
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// ���Ӷ�����WM_DESTROY��Ϣ
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}


//void Object::OnPaint( HDC hDC )


// �ڽ��յ�OnSize��Ϣ��ʱ����Ҫ�ı��Լ���pos,rectP,rectW��Ա��
// ��֪ͨ�Լ����Ӷ���ȥ����
//void Object::OnSize( UINT nType, int cx, int cy )
//{
//	this->UpdateRect();
//
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// ���Ӷ�����WM_ERASEBKGND��Ϣ
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}

// ע���ú��������𽫹����Ķ������Ӧָ���ÿգ����粻�Ὣparent�е�child�е��Լ����
//     Ŀǰ�����ڻ���ʱ������

void Object::ClearMyTreeRelationOnly()
{
	this->m_pParent = NULL;
	this->m_pNext = NULL;
	this->m_pPrev = NULL;

	if (OBJ_CONTROL != this->GetObjectType()) // �ؼ�������xml��ֻռһ����㣬������� 
	{
		this->m_pChild = NULL;
		this->m_pNcChild = NULL;
	}

	// ��Ҫ���hook/notify����Ϊ����ʱ��Ҫ������Щ��Ϣ
	// 			pThis->ClearHook();
	// 			pThis->ClearNotify();
}


//
//  ���ݶ������ڵı��������������ı���ɫ������ͼƬ����
//
// Two colors provide good color visibility if the brightness difference and the color difference between the two colors are greater than a set range.
// Color brightness is determined by the following formula:
// ((Red value X 299) + (Green value X 587) + (Blue value X 114)) / 1000
// Color difference is determined by the following formula:
// (max (Red 1, Red 2) - min (Red 1, Red 2)) + (max (Green 1, Green 2) - min (Green 1, Green 2)) + (max (Blue 1, Blue 2) - min (Blue 1, Blue 2))
//  The rage for color brightness difference is 125. The range for color difference is 500.
// 
//  google text background color black white
//  http://juicystudio.com/services/aertcolourcontrast.php
//  http://www.colourlovers.com/web/blog/2010/03/11/calculating-color-contrast-for-legible-text
//
DWORD  Object::CalcContrastTextColor()
{
    WindowBase*  pWindowBase = GetWindowObject();
    if (NULL == pWindowBase)
        return 0;

    HDC hDC = pWindowBase->GetMemoryLayerDC();  // ����ȡ�ؼ���DC ��Ŀǰ������ȫ��layer����ǰ����ͼƬ�ϲ��������������ݣ�������object����
    if (NULL == hDC)
        return 0;

    HBITMAP  hBitmap = (HBITMAP)GetCurrentObject(hDC, OBJ_BITMAP);
    if (NULL == hBitmap)
        return 0;

    CRect rcWindow;
    GetWindowRect(&rcWindow);

    Image  image;
    image.Attach(hBitmap);
    DWORD  dwAverageColor = image.GetAverageColor(&rcWindow);
    image.Detach();

    // ��alphaֵӦ����
    int a = dwAverageColor >> 24;
    int r = GetRValue(dwAverageColor);
    int g = GetGValue(dwAverageColor);
    int b = GetBValue(dwAverageColor);

//     r = r * a >> 8;
//     g = g * a >> 8;
//     b = b * a >> 8;

    if (a)
    {
        r = (r<<8)/a;
        g = (g<<8)/a;
        b = (b<<8)/a;

        r = min(255, r);
        g = min(255, g);
        b = min(255, b);
    }

    int v = ((r * 299) + (g * 587) + (b * 114)) >> 10;
    if (v < 125)
        return 0xFFFFFFFF;
    else
        return 0xFF000000;
}


Object*  Object::GetObjectByPos(POINT* pt)
{
    WindowBase* pWindow = GetWindowObject();
    if (NULL == pWindow)
        return NULL;

    return pWindow->GetObjectByPos(this, pt, false);
}

bool  Object::SetMouseCapture(int nNotifyMsgId)
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->SetMouseCapture(m_pIObject, nNotifyMsgId);
    return true;
}
bool  Object::ReleaseMouseCapture()
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->ReleaseMouseCapture(m_pIObject);
    return true;
}
bool  Object::SetKeyboardCapture(int nNotifyMsgId)
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->SetKeyboardCapture(m_pIObject, nNotifyMsgId);
    return true;
}
bool  Object::ReleaseKeyboardCapture()
{
    WindowBase* pWindow = GetWindowObject();
    if (!pWindow)
        return false;

    pWindow->GetMouseMgr()->ReleaseKeyboardCapture(m_pIObject);
    return true;
}
}