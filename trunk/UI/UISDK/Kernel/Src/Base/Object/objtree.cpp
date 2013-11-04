#include "stdafx.h"
#include "objtree.h"
#include "UISDK\Kernel\Src\Base\Object\object.h"

// 将自己（包括自己的子结点）在树结构中移除
void ObjTree::RemoveMeInTheTree()
{
	if (m_pPrev)
	{
		m_pPrev->m_pNext = this->m_pNext;
	}
	else
	{
		if (m_pParent) 
		{
			if (this->IsNcObject())
				m_pParent->m_pNcChild = this->m_pNext;
			else
				m_pParent->m_pChild = this->m_pNext;
		}
	}

	if (m_pNext)
	{
		m_pNext->m_pPrev = this->m_pPrev;
	}

	m_pParent = m_pNext = m_pPrev = NULL;
}

// 调用情况：
// 1. WindowBase::_OnNcDestroy
// 2. Object::FinalRelease
void ObjTree::DestroyChildObject()
{
	// 1. Client Object
	Object* pObj = NULL;
	while (pObj = EnumChildObject(pObj))
	{
		this->m_pChild = pObj->m_pNext;
        if (pObj->GetIObject())
        {
		    pObj->GetIObject()->delete_this();
            pObj = NULL;
        }

		if (NULL == this->m_pChild)
			break;

		this->m_pChild->m_pPrev = NULL;
	}
	this->m_pChild = NULL;

	// 2. NC Object

	pObj = NULL;
	while (pObj = EnumNcChildObject(pObj))
	{
		this->m_pNcChild = pObj->m_pNext;
        if (pObj->GetIObject())
        {
            pObj->GetIObject()->delete_this();
            pObj = NULL;
        }

		if (NULL == this->m_pNcChild)
			break;

		this->m_pNcChild->m_pPrev = NULL;
	}
	this->m_pNcChild = NULL;

	//	3. 清理自己的邻居关系
	RemoveMeInTheTree();
}


void ObjTree::AddChild(Object* pObj)
{
    if (NULL == pObj)
        return;
    if (pObj->m_pParent == this)
        return;

	if (NULL == this->m_pChild)
	{
		this->m_pChild = pObj;
		pObj->m_pParent = static_cast<Object*>(this);
	}
	else
	{
		Object* pObjEnum = NULL;
		while (pObjEnum = EnumChildObject(pObjEnum))
		{
			if (pObjEnum == pObj)
				return;

			if (NULL == pObjEnum->m_pNext)
			{
				pObjEnum->m_pNext = pObj;
				pObj->m_pPrev = pObjEnum;
				pObj->m_pParent = static_cast<Object*>(this);
				break;
			}
		}
	}
    pObj->SetAsNcObject(false);
}


void ObjTree::AddNcChild(Object* pObj)  
{
    if (NULL == pObj)
        return;
    if (pObj->m_pParent == static_cast<Object*>(this))
        return;

	if (NULL == this->m_pNcChild)
	{
		this->m_pNcChild = pObj;
		pObj->m_pParent = (static_cast<Object*>(this));
	}
	else
	{
		Object* pObjEnum = NULL;
		while(pObjEnum = EnumNcChildObject(pObjEnum))
		{
			if (pObjEnum == pObj)
				return;

			if (NULL == pObjEnum->m_pNext)
			{
				pObjEnum->m_pNext = pObj;
				pObj->m_pPrev = pObjEnum;
				pObj->m_pParent = (static_cast<Object*>(this));
				break;
			}
		}
	}
	pObj->SetAsNcObject(true);
}


void ObjTree::InsertAfter(Object* pInsertAfter)
{
    if (NULL == pInsertAfter)
        return;

    Object* pNextSave = pInsertAfter->m_pNext;
    
    this->m_pParent = pInsertAfter->m_pParent;
    this->m_pPrev = pInsertAfter;
    this->m_pNext = pNextSave;

    pInsertAfter->m_pNext = static_cast<Object*>(this);
    if (pNextSave)
        pNextSave->m_pPrev = static_cast<Object*>(this);

    this->SetAsNcObject(pInsertAfter->IsNcObject());
}


void ObjTree::InsertBefore(Object* pInsertBefore)
{
    if (NULL == pInsertBefore)
        return;

    Object* pPrev = pInsertBefore->m_pPrev;

    this->m_pParent = pInsertBefore->m_pParent;
    this->m_pNext = pInsertBefore;
    this->m_pPrev = pPrev;

    pInsertBefore->m_pPrev = static_cast<Object*>(this);

    bool bNcObj = pInsertBefore->IsNcObject();
    if (pPrev)
    {
        pPrev->m_pNext = static_cast<Object*>(this);
    }
    else if (this->m_pParent)
    {
        if (bNcObj)
            this->m_pParent->m_pNcChild = static_cast<Object*>(this);
        else
            this->m_pParent->m_pChild = static_cast<Object*>(this);
    }
    this->SetAsNcObject(bNcObj);
}

bool ObjTree::SwapObject(Object* pObj1, Object* pObj2)
{
    if (NULL == pObj1 || NULL == pObj2)
        return false;

    Object* pParentSave = pObj1->m_pParent;
    Object* pPrevSave = pObj1->m_pPrev;
    Object* pNextSave = pObj1->m_pNext;
    bool bNcSave = pObj1->IsNcObject();
    
    // 设置obj1
    pObj1->m_pParent = pObj2->m_pParent;
    pObj1->m_pPrev = pObj2->m_pPrev;
    pObj1->m_pNext = pObj2->m_pNext;
    pObj1->SetAsNcObject(pObj2->IsNcObject());
    if (pObj1->m_pPrev)
        pObj1->m_pPrev->m_pNext = pObj1;
    if (pObj1->m_pNext)
        pObj1->m_pNext->m_pPrev = pObj1;

    if (NULL == pObj1->m_pPrev && pObj1->m_pParent)
    {
        if (pObj1->IsNcObject())
            pObj1->m_pParent->m_pNcChild = pObj1;
        else
            pObj1->m_pParent->m_pChild = pObj1;
    }

    // 设置obj2
    pObj2->m_pParent = pParentSave;
    pObj2->m_pPrev = pPrevSave;
    pObj2->m_pNext = pNextSave;
    pObj2->SetAsNcObject(bNcSave);
    if (pPrevSave)
        pPrevSave->m_pNext = pObj2;
    if (pNextSave)
        pNextSave->m_pPrev = pObj2;

    if (NULL == pPrevSave && pParentSave)
    {
        if (bNcSave)
            pParentSave->m_pNcChild = pObj2;
        else
            pParentSave->m_pChild = pObj2;
    }

    return true;
}

bool ObjTree::IsMyChild(Object* pChild, bool bFindInGrand)	
{
	if (NULL == pChild)
		return false;

	if (bFindInGrand)
	{
		Object* pParent = pChild->m_pParent;
		while (pParent)
		{
			if (pParent == this)
				return true;

			pParent = pParent->m_pParent;
		}
	}
	else
	{
		if (pChild->m_pParent == static_cast<Object*>(this))
			return true;
		else
			return false;
	}

	return false;
}


bool ObjTree::RemoveChildInTree(Object* pChild)
{
	if (NULL == pChild)
		return false;

	if (!IsMyChild(pChild, false))
		return false;

	if (m_pChild == pChild)
	{
		m_pChild = pChild->m_pNext;
		if (m_pChild)
		{
			m_pChild->m_pPrev = NULL;
		}
	}
	else
	{
		Object* pPrev = pChild->m_pPrev;
		Object* pNext = pChild->m_pNext;

		pPrev->m_pNext = pNext;
		if (pNext)
		{
			pNext->m_pPrev = pPrev;
		}
	}

	pChild->m_pParent = NULL;
	pChild->m_pNext = NULL;
	pChild->m_pPrev = NULL;

	return true;
}

//
//	[public] Object* EnumChildObject(Object*)
//
//	Parameter
//		pObj
//			[in]迭代变量，用NULL开始，用返回值作为下一个迭代参数
//
//	Return
//		返回下一个子对象
//
//	Remark
//		枚举自己的子对象，例如：
//		Object* pObj = NULL;
//		while( pObj = EnumChildObject(pObj) )
//		{
//			// TODO something with pObj
//		}
//

Object* ObjTree::EnumChildObject(Object* pObj)
{
	if (NULL == pObj)
		return this->m_pChild;
	else
		return pObj->m_pNext;
}

Object* ObjTree::REnumChildObject(Object* pObj)
{
	if (NULL == pObj)
	{
		Object* pChild = NULL;
		while (pChild = this->EnumChildObject(pChild))
		{
			if (NULL == pChild->m_pNext)
				break;
		}
		return pChild;
	}
	else
	{
		return pObj->m_pPrev;
	}
}


Object* ObjTree::EnumNcChildObject(Object* pObj)
{
	if (NULL == pObj)
		return this->m_pNcChild;
	else
		return pObj->m_pNext;
}


Object* ObjTree::EnumAllChildObject(Object* pObj)
{
	if (NULL == pObj)
	{
		if (m_pNcChild)
			return this->m_pNcChild;
		else
			return this->m_pChild;
	}
	else
	{
		if (NULL == pObj->m_pNext && pObj->IsNcObject())
			return m_pChild;

		return pObj->m_pNext;
	}
}

// 从自己的parent开始往上枚举自己的祖宗

Object* ObjTree::EnumParentObject(Object* pObj)  
{
	if (NULL == pObj )
		return this->m_pParent;
	else
		return pObj->m_pParent;
}

// 从自己的祖宗开始往下枚举，直到枚举到自己的parent

Object* ObjTree::REnumParentObject(Object* pObj)  
{
	if(NULL == pObj)
	{
		Object* pParent = NULL;
		while (pParent = this->EnumParentObject(pParent))
		{
			if (NULL == pParent->m_pParent)
				break;
		}
		return pParent;
	}
	else
	{
		if (m_pParent == pObj)
			return NULL;

		Object* pParent = NULL;
		while (pParent = this->EnumParentObject(pParent))
		{
			if (pParent->m_pParent == pObj)
				return pParent;
		}
		return NULL;
	}
	return NULL;
}

Object* ObjTree::GetRootObject()
{
	Object* p = static_cast<Object*>(this);
	Object* pRet = NULL;
	while (pRet = p->m_pParent)
	{
		p = pRet;
	}

	return p;
}

// 按钮父->左->右的顺序获取遍历下一个结点

Object* ObjTree::GetNextTreeItemObject()
{
	Object* p = (Object*)this;

	// 有子结点
	if (p->m_pChild) 
	{
		return p->m_pChild;
	}

	// 没有子结点，获取下一个结点
	if (p->m_pNext)
	{
		return p->m_pNext;
	}

	// 也没有下一个子结点，获取父对象的下一个结点
	Object* pParent = p->m_pParent;
	while (pParent)
	{
		Object* pNext = pParent->GetNextObject();
		if (pNext)
			return pNext;

		pParent = pParent->m_pParent;
	}

	return NULL;
}

// 按钮右->左->父的顺序获取遍历下一个结点

Object* ObjTree::GetPrevTreeItemObject()
{
	Object* p = (Object*)this;

	// 获取左侧结点的最后一个子结点
	if (p->m_pPrev)
	{
		Object* pLeft = p->m_pPrev;
		Object* pChild = NULL;
		while (pChild = pLeft->GetLastChildObject())
		{
			pLeft = pChild;
		}
		return pLeft;
	}

	// 有父结点
	if (p->m_pParent) 
	{
		return p->m_pParent;
	}

	return NULL;
}


Object* ObjTree::GetNextTreeTabstopItemObject()
{
	Object* p = (Object*)this;

	Object* pRet = NULL;
	while (pRet = p->GetNextTreeItemObject())
	{
		if (pRet->CanTabstop())
			return pRet;

		else
			p = pRet;
	}

	// 再重头开始一次
	p = (Object*)this->GetRootObject();
	if (p == this)
		return NULL;

	return p->GetNextTreeTabstopItemObject();
}


Object* ObjTree::GetPrevTreeTabstopItemObject()
{
	Object* p = (Object*)this;

	Object* pRet = NULL;
	while (pRet = p->GetPrevTreeItemObject())
	{
		if (pRet->CanTabstop())
			return pRet;

		else
			p = pRet;
	}

	// 再重尾开始一次
	p = (Object*)this->GetRootObject();
	pRet = NULL;
	while (pRet = p->GetLastChildObject())
	{
		p = pRet;
	}
	if (NULL == p || p == this)
		return NULL;

	if (p->CanTabstop())
		return p;

	return p->GetPrevTreeTabstopItemObject();
}


void  ObjTree::MoveToAsFirstChild()
{
    Object*  pParent = this->m_pParent;
    if (NULL == pParent)
        return;

    if (pParent->m_pChild == static_cast<Object*>(this))
        return;

    // 连接自己的前后
    this->m_pPrev->m_pNext = this->m_pNext;
    if (m_pNext)
    {
        m_pNext->m_pPrev = this->m_pPrev;
    }

    Object* pThis = static_cast<Object*>(this);

    // 设置为第一个
    this->m_pPrev = NULL;
    this->m_pNext = pParent->m_pChild;
    pParent->m_pChild->m_pPrev = pThis;
    pParent->m_pChild = pThis;
}

void  ObjTree::MoveToAsLastChild()
{
    Object*  pParent = this->m_pParent;
    if (NULL == pParent)
        return;

    Object*  pLast = pParent->GetLastChildObject();
    if (pLast == this)
        return;

    // 连接自己的前后
    if (this->m_pPrev)
        this->m_pPrev->m_pNext = this->m_pNext;
    else
        pParent->m_pChild = this->m_pNext;

    m_pNext->m_pPrev = this->m_pPrev;

    Object* pThis = static_cast<Object*>(this);

    // 设置为最后一个
    this->m_pPrev = pLast;
    this->m_pNext = NULL;
    pLast->m_pNext = pThis;
}