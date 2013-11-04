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

 // 注意：不要在构造或者析构函数中调用虚函数

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
	// 防止调用中途或者换肤过程中，一些对象突然被销毁，导致的野指针错误
	WindowBase* pWindow = GetWindowObject();
	if (pWindow)  // 必须在DestroyChildObject前调用，否则parent关系被清空
	{
		pWindow->OnObjectDeleteInd(this);
	}

	this->DestroyChildObject();  // 先删除子对象

	IUIEditor* pEditor = m_pUIApplication->GetUIEditorPtr();
	if (pEditor)
	{
		// 由子往父进行通知，便于从skinbuilder的tree中一个个删除
		pEditor->OnObjectDeleteInd(m_pIObject);
	}

	m_rcParent.left = m_rcParent.right = m_rcParent.top = m_rcParent.bottom = 0;
	SAFE_RELEASE(m_pBkgndRender);
	SAFE_RELEASE(m_pForegndRender);
	SAFE_RELEASE(m_pTextRender);
	SAFE_RELEASE(m_pCursor);
	SAFE_DELETE_GDIOBJECT(m_hRgn);
}

// 注：如果在其它模块直接调用 pCtrl->m_strID=L"..."的话，在对象释放时将会崩溃
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
//	重绘这个对象(一般是鼠标移上移下，状态变化等时候的重绘)
//
//	1. 不透明的控件，直接在原处绘即可，包括了背景为COLOR、Image的情况
//		TODO: 考虑将ButtonBkRender再抽象出来，给所有的控件使用
//	2. 透明的控件...是否需要考虑下优化为直接取到自己在父窗口下的背景，而不是刷新整个窗口
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

// 当对象隐藏/移动的时候，刷新背景

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
//	当对象显示/隐藏，或者大小发生改变时，重新刷新自己所在layout的布局
//
//	1. 获取自己的所在Layout
//	2. 测量Layout的大小是否发生改变，如果这个Layout大小改变，再获取上一层Layout
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

        // pParent的大小发生了变化，继续往上
        if (sizeOld.cx != size.cx || sizeOld.cy != size.cy)
        {
            if (pParent->GetParentObject())
            {
                pObjectToUpdate = pParent;
                pParent = pParent->GetParentObject();  // 有点乱... 当是窗口的时候size为window rect，sizeOld为client rect，因此肯定不一样
                continue;                              // 所以在这里如果发现是window( parent==null )则继续往下处理
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

// 获取对象所在渲染层
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

// 获取一个控件所在窗口句炳
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
**	[public] 获取某一具体路径下的子对象，但这里的子对象也可是子对象的子对象。
**
**	parameter
**		objIdPath
**			[in]	要获取的对象的路径，不包括自己本身，路径以/分隔
**	
**	return
**		成功返回该对象，失败返回NULL。失败可查看日志输出
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
// 		UI_LOG_ERROR( _T("Object::GetObjectItem，无效的参数:%s"), objIdPath.c_str() );
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
// 		// 没有找到指定的对象
// 		if (NULL == pTempObj )
// 		{
// 			UI_LOG_ERROR( _T("Object::GetObjectItem，未找到指定的对象在%s下："), this->m_strID.c_str(), objIdPath.c_str() );
// 			return NULL;
// 		}
// 	}
// 
// 	return pObjRet;
// }

//
//	在所有的子子孙孙中查找id为objId的对象，只返回第一个找到的对象
//
//	Parameter
//		objId
//			[in]	要查找的对象id
//
//	Return
//		成功返回对象地址，失败返回NULL
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

    // 布局属性
    if (m_pLayoutParam)
    {
        m_pLayoutParam->Serialize(pData);
    }
}
bool Object::SetAttribute(IMapAttribute* pMapAttrib, bool bReload)
{
    // 保存属性，用于扩展。
    SAFE_RELEASE(m_pIMapAttributeRemain);
    m_pIMapAttributeRemain = pMapAttrib;
    if (m_pIMapAttributeRemain)
        m_pIMapAttributeRemain->AddRef();

	if (NULL == m_pUIApplication || NULL == pMapAttrib)
	{
		UI_LOG_ERROR(_T("%s Invalid Argument."), FUNC_NAME);
		return false;
	}

	//	设置id属性
    const TCHAR* szText = NULL;
    szText = pMapAttrib->GetAttr(XML_ID, true);
    if (szText)
        m_strID = szText;
	
    // 可见性
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

	// 设置背景渲染器
    pMapAttrib->GetAttr_RenderBase(XML_BACKGND_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, this->GetIObject(), &m_pBkgndRender);

	// 设置前景绘制
    pMapAttrib->GetAttr_RenderBase(XML_FOREGND_RENDER_PREFIX, XML_RENDER_TYPE, true, m_pUIApplication, this->GetIObject(), &m_pForegndRender);

// 	// 字体  -- 将字体属性移出。谁需要字体属性谁自己负责解析。不是所有的对象都需要字体
	//          另window的font属性在windowbase::setattribute中将解析"font"属性得到
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
	
	// 刷新属性
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
	
	// 鼠标样式
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
 	pEditor->CreateTextAttribute(pObjGroup, XML_ID, szPrefix, NULL, L"控件ID");
    pEditor->CreateBoolAttribute(pObjGroup, XML_VISIBLE, true, szPrefix, NULL, L"是否可见。默认可见");
	pEditor->CreateBoolAttribute(pObjGroup, XML_DISABLE, false, szPrefix, NULL, L"是否被禁用。默认可用");
    pEditor->CreateBoolAttribute(pObjGroup, XML_BACKGND_IS_TRANSPARENT, false, szPrefix, NULL, L"控件背景是否透明。如果控件背景透明，在绘制时则需要再绘制父控件背景。默认不透明");
    pEditor->CreateBoolAttribute(pObjGroup, XML_REJEST_MOUSE_MSG, false, szPrefix, NULL, L"是否屏蔽鼠标消息。默认接受");
	pEditor->CreateTextAttribute(pObjGroup, XML_MARGIN, szPrefix, NULL, L"外间距");
	pEditor->CreateTextAttribute(pObjGroup, XML_PADDING, szPrefix, NULL, L"内间距");
	pEditor->CreateTextAttribute(pObjGroup, XML_BORDER, szPrefix, NULL, L"边框");
    pEditor->CreateTextAttribute(pObjGroup, XML_CURSOR, szPrefix, NULL, L"鼠标样式");

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
    // 解析样式
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
//    UISendMessage(m_pIObject, UI_WM_SETATTRIBUTE, (WPARAM)pMatAttrib, (LPARAM)bReload); // 即使为空，也要通知过去
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
//     // 解析样式
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
// 	bool bRet = pChildObj->SetAttribute(pMapChildObjAttrib, bReload);  // 即使为空，也要通知过去
//     SAFE_RELEASE(pMapChildObjAttrib);
//     return bRet;
// }


// 获取一个未解析的属性。如果bErase==true，则将返回一个临时的字符串指针，调用者应该尽快保存或者仅临时使用
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
//	获取对象自己期望的大小
//
SIZE Object::GetDesiredSize()
{
    if (!m_pLayoutParam)
    {
       CreateLayoutParam();

       if (!m_pLayoutParam) // 还是创建不成功，例如Combobox中的Button，它没有父Panel-Layout
       {
           m_pLayoutParam = CanvasLayout::s_CreateLayoutParam(m_pIObject);
       }
    }

    return m_pLayoutParam->CalcDesiredSize();
}


// 设置padding的值，同时更新非客户区的大小
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

// 要绘制该对象之前，获取该对象在窗口中的实现位置，用于设置偏移量和裁剪区

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

// 获取一个对象在窗口上的可视区域。例如用于绘制该对象时的裁剪
// Param:
//    bCalcInLayer: 当对象位于一个layer上时，重绘该对象需要获取的时该对象在层上的区域，而不是
//                  在窗口上的区域。但层自己也有一个相对于窗口的位置，这个位置需要区分对待
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
		if (NULL == pObjParent)  // 最后还需要再计算一次自己在父对象中的范围
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

			// 减掉非客户区域
			if (!pObjParent->IsNcObject())
			{
				CRegion4 rcNonRect;
				pObjParent->GetParentObject()->GetNonClientRegion(&rcNonRect);
				Util::DeflatRect(&rcClip, &rcNonRect);

				xOffset += rcNonRect.left; 
				yOffset += rcNonRect.top;

				// pObjParent在它的父中的直接位置（减掉了滚动区域)
				int xScroll = 0, yScroll= 0;
				pObjParent->GetParentObject()->GetScrollOffset(&xScroll, &yScroll);
				::OffsetRect(&rcParent, -xScroll, -yScroll);
			}

			// 计算出pObjParent在窗口中的位置
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
// 递归. 将该对象及其子对象绘制到pRenderTarget上面
// 因为子控件的范围可能要比parent还大，因此在这里必须取子和父的交集clip rgn进行绘制  
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
// 很多情况下面不需要判断自己的可见性，因些将绘制代码从DrawObject抽出来
void  Object::RealDrawObject(IRenderTarget* pRenderTarget, RenderContext roc)
{
    ::UISendMessage(this, WM_ERASEBKGND, (WPARAM)pRenderTarget, (LPARAM)&roc);  // 将lparam置为1，与原始消息进行区分
    this->_drawNcChildObject(pRenderTarget, roc);

    roc.DrawClient(m_pIObject);
    roc.Scroll(m_pIObject);
    roc.Update(pRenderTarget);

    ::UISendMessage(this, WM_PAINT, (WPARAM)pRenderTarget, (LPARAM)&roc);       // 将lparam置为1，与原始消息进行区分
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

// 如果是透明背景的话，需要获取父窗口中的背景
// 但即使是不透明的object，也需要一层层遍历下来获取它的剪裁区域
//
//   为了适应于对象隐藏的时候能够刷新自己的父对象背景，需要增加一个场景来忽略自己的透明属性
//   因此增加bSelfTransparent，当普通刷新的时候传递this->IsTransparen()，其它时候直接传递false

void Object::DrawObjectTransparentBkgnd(IRenderTarget* pRenderTarget, RenderContext& roc, bool bSelfTransparent)
{
	bool    bTransparent = bSelfTransparent;
	Object* pObjFirstParentToDrawBk = NULL;
	
	if (bTransparent)
	{
		// 检查祖父对象中有没有不透明的，从该父对象画起
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
		pObjFirstParentToDrawBk = this;   // 虽然不需要绘制，但还是需要依据父对象进行偏移
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
			if (pObjParent != pObjFirstParentToDrawBk)  // 从pObjFirstParentToDrawBk开始绘制
				continue;

			pObjFirstParentToDrawBk = NULL;             // 清除标记
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
// 获取该对象的偏移量
//
// ReturnQ
//		返回false表示该对象无滚动数据
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
//	设置Object的默认鼠标样式
//
//	Remark:
//		如果不实现该消息，可能导致鼠标移动到一个Object上面时，仍然显示的是Window的OnSetCursor
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

//	获取自己的字体,这里返回的对象只是一个临时对象，
//	如果需要保存使用，则需要调用AddRef
IRenderFont* Object::GetRenderFont()
{
    IRenderFont* pRenderFont = NULL;

    // 向自己的textrender获取
    if (m_pTextRender)
        pRenderFont = m_pTextRender->GetRenderFont();

    if (pRenderFont)
        return pRenderFont;

    // 向窗口获取默认
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
//			[in]  要试探的位置，通常为鼠标当前位置
//
//	Return
//		鼠标不在对象所有警围内返回HTNOWHERE，否则返回HTCLIENT，或其它（子类可扩展）
//
//	Remark
//		这个函数是用来解决一个问题，是为Object配备一个RECT对象表示范围，还是给一个更强大的RGN表示范围?
//		由于RGN更加消耗内存，而有时候就得使用RGN来实现某些功能，因此在这里用这个函数来告诉调用者，
//		你这个位置处于我什么部位。同时给Object配备一个RECT，来表示每一个对象的范围，因为大部分时候使用的都是RECT区域。
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

#if 0 // -- 架构改造
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
//         // 防止调用中途或者换肤过程中，一些对象突然被销毁，导致的野指针错误
//         WindowBase* pWindow = GetWindowObject();
//         if (pWindow)  // 必须在DestroyChildObject前调用，否则parent关系被清空
//         {
//             pWindow->OnObjectHideInd(this);
//         }
//     }

    // 如果隐藏的对象是一个焦点对象，则将焦点重新切回到第一个对象
    // 注，这里的object也可能是位于一个listctrl中，则其对应的mouse/keyboard
    //     manager则不再是窗口的，而是listctrl自己的
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

// 默认实现为立即刷新。
// 该消息可用于通过PostMessage进行延迟刷新，或者只能拿到一个IMessage对象时调用
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
**			[in] 要测试的比特位，如OSB_VISIBLE、OSB_DISABLE、CSB_TABABLE、CSB_DEFAULT
**				 CSB_FOCUS、CSB_READONLY、CSB_HOVER、CSB_PRESS
**	
**	return
**		该位为1，返回true，该位为0，返回false
**
**	remark
**		为了节省内存占用，将一个控件的多种状态集中为bit位进行处理
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
	// 判断父对象不可见的情况 
	else
	{
		if (OBJ_WINDOW == m_pParent->GetObjectType())
		{ 
			return true;           // 注：在这里不对最外层的窗口进行判断的原因是：在类似于窗口初始化的函数里面，虽然窗口暂时是不可见的，但里面的对象的IsVisible应该是返回true才好处理
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
//	备注：分层窗口中，隐藏一个对象时，不能直接调用::InvalidateRect(&rc..)
//        必须调用 GetWindowObject(); pWindow->InvalidateObject(GetWindowObject(),...);
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

	// 通知子对象
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

// 注：这里的clientrect 并不是0,0起点的，而是相对于Object左上角而言

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

// 注：这里的clientrect 是0,0起点的

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

// clientrect在窗口中的坐标
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
// 遍历自己的nc object来更新自己的non client region
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

    //  TODO: 加上这一段代码会导致SetVisible中的布局出现问题，如果pp -> p -> o，o发生变化
    //  让p去重新布局，然后p再检测自己的desiredsize变化了，再向pp请求重新布局，结果pp的大
    //  小没有发生变化，导致p不会去重新布局，o刷新失败。
//     if (bMove && 
//         x == m_rcParent.left &&
//         y == m_rcParent.top && 
//         bSize && 
//         cx == m_rcParent.Width() && 
//         cy == m_rcParent.Height())
//         return;

	nFlag |= SWP_NOZORDER;   // 该函数不提供修改ZORDER的功能
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
		// 刷新移动前的区域位置
		if (!(nFlag & SWP_NOREDRAW))
		{
			this->GetObjectVisibleRect(&rcOldVisibleRect, true);  // 获取下当前会刷新的区域范围，放在后面进行提交
			this->UpdateObjectBkgnd(false);  // 这里不能立即刷新，否则后面再调用UpdateObject可能造成闪烁（移动后的位置有重合的情况下）
		}
	}

    int nObjType = this->GetObjectType();
	if (nObjType == OBJ_WINDOW)
	{
		// 对于窗口来说，这里设置的是非客户区的大小
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
	// 在这里我们暂时只先发送WM_MOVE/WM_SIZE消息
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
		// 刷新移动后的区域位置
		if (!(nFlag & SWP_NOREDRAW))
		{
			this->UpdateObject();

			// 将移动之前的区域数据提交到窗口上，避免闪烁
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

// 根据m_rcParent更新 m_nConfigLeft/m_nConfigRight/m_nConfigTop/m_nConfigBottom/m_nConfigLayoutFlags
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

// 当手动创建一个对象（非从xml中加载时，可以调用该函数完全默认属性的加载)
void  Object::InitDefaultAttrib()
{
    IMapAttribute* pMapAttrib = NULL;
    UICreateIMapAttribute(&pMapAttrib);

    // 解析样式
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
//                               消息映射                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


//
//   窗口收到WM_DESTROY消息之后，传递给所有的子对象
//
//void Object::OnDestroy()
//{
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// 向子对象发送WM_DESTROY消息
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}


//void Object::OnPaint( HDC hDC )


// 在接收到OnSize消息的时候，需要改变自己的pos,rectP,rectW成员，
// 并通知自己的子对象去更新
//void Object::OnSize( UINT nType, int cx, int cy )
//{
//	this->UpdateRect();
//
//	Object*  pObj = NULL;
//	while( pObj = EnumChildObject(pObj) )
//	{
//		// 向子对象发送WM_ERASEBKGND消息
//		UIMsg  cloneMsg = *(this->m_pCurMsg);
//		cloneMsg.pObjMsgTo = pObj;
//		::UISendMessage( &cloneMsg );
//	}
//}

// 注：该函数不负责将关联的对象的相应指针置空，例如不会将parent中的child中的自己清除
//     目前仅是在换肤时被调用

void Object::ClearMyTreeRelationOnly()
{
	this->m_pParent = NULL;
	this->m_pNext = NULL;
	this->m_pPrev = NULL;

	if (OBJ_CONTROL != this->GetObjectType()) // 控件类型在xml中只占一个结点，不能清除 
	{
		this->m_pChild = NULL;
		this->m_pNcChild = NULL;
	}

	// 不要清除hook/notify，因为换肤时需要保留这些信息
	// 			pThis->ClearHook();
	// 			pThis->ClearNotify();
}


//
//  根据对象所在的背景，计算对象的文本颜色。用于图片换肤
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

    HDC hDC = pWindowBase->GetMemoryLayerDC();  // 仅获取控件层DC （目前做不到全部layer）当前缓冲图片上并不包含文字内容，但包含object背景
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

    // 将alpha值应用上
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