#pragma 
//
//	使用Tree结构实现Object之间的层次关系
//

namespace UI
{
	template<class T, class TBase>       
	class UIAPI ObjTree : public TBase
	{
	public:
		ObjTree()
		{
			m_pParent = m_pChild = m_pNcChild = m_pNext = m_pPrev = NULL;
		}
		~ObjTree()
		{
			this->DestroyUI();
		}

	protected:

		void DestroyUI()
		{
			// 1. Client Object

			T* pObj = NULL;
			while (pObj = EnumChildObject(pObj))
			{
				this->m_pChild = pObj->m_pNext;
				SAFE_DELETE(pObj);

				if (NULL == this->m_pChild)
					break;

				this->m_pChild->m_pPrev = NULL;
			}

			// 2. NC Object

			pObj = NULL;
			while (pObj = EnumNcChildObject(pObj))
			{
				this->m_pNcChild = pObj->m_pNext;
				SAFE_DELETE(pObj);

				if (NULL == this->m_pNcChild)
					break;

				this->m_pNcChild->m_pPrev = NULL;
			}

			//	3. 清理自己的邻居关系

			if (NULL != m_pPrev)
			{
				m_pPrev->m_pNext = this->m_pNext;
			}
			else
			{
				if (NULL != m_pParent) 
				{
					T* pThis = (T*)this;
					if (pThis->IsNcObject())
						m_pParent->m_pNcChild = this->m_pNext;
					else
						m_pParent->m_pChild = this->m_pNext;
				}
			}
			if (NULL != m_pNext)
			{
				m_pNext->m_pPrev = this->m_pPrev;
			}

			m_pParent = m_pChild = m_pNcChild = m_pNext = m_pPrev = NULL;
		}

	public:
		void AddChild(T* pObj)
		{
			if (NULL == this->m_pChild)
			{
				this->m_pChild = pObj;
				pObj->m_pParent = (T*)this;
			}
			else
			{
				Object* pObjEnum = NULL;
				while (pObjEnum = EnumChildObject(pObjEnum))
				{
					if (NULL == pObjEnum->m_pNext)
					{
						pObjEnum->m_pNext = pObj;
						pObj->m_pPrev = pObjEnum;
						pObj->m_pParent = (T*)this;
						break;
					}
				}
			}
		}

		void AddNcChild(T* pObj)  
		{
			if (NULL == this->m_pNcChild)
			{
				this->m_pNcChild = pObj;
				pObj->m_pParent = (T*)this;
			}
			else
			{
				T* pObjEnum = NULL;
				while(pObjEnum = EnumNcChildObject(pObjEnum))
				{
					if (NULL == pObjEnum->m_pNext)
					{
						pObjEnum->m_pNext = pObj;
						pObj->m_pPrev = pObjEnum;
						pObj->m_pParent = (T*)this;
						break;
					}
				}
			}
			pObj->SetAsNcObject();
		}

		
		//
		//	[public] T* EnumChildObject(T*)
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
		T* EnumChildObject(T* pObj)
		{
			if (NULL == pObj)
				return this->m_pChild;
			else
				return pObj->m_pNext;
		}
		T* REnumChildObject(T* pObj)
		{
			if (NULL == pObj)
			{
				T* pChild = NULL;
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
		
		T* EnumNcChildObject(T* pObj)
		{
			if (NULL == pObj)
				return this->m_pNcChild;
			else
				return pObj->m_pNext;
		}

		T* EnumAllChildObject(T* pObj)
		{
			if (NULL == pObj)
			{
				if (NULL != m_pNcChild)
					return this->m_pNcChild;
				else
					return this->m_pChild;
			}
			else
			{
				if (NULL == pObj->m_pNext && pObj->TestStyle(OBJECT_STYLE_NCOBJ))
					return m_pChild;

				return pObj->m_pNext;
			}
		}

		// 从自己的parent开始往上枚举自己的祖宗
		T* EnumParentObject(T* pObj)  
		{
			if (NULL == pObj )
				return this->m_pParent;
			else
				return pObj->m_pParent;
		}

		// 从自己的祖宗开始往下枚举，直到枚举到自己的parent
		T* REnumParentObject(T* pObj)  
		{
			if(NULL == pObj)
			{
				T* pParent = NULL;
				while( pParent = this->EnumParentObject( pParent ) )
				{
					if (pParent->m_pParent == NULL )
						break;
				}
				return pParent;
			}
			else
			{
				if (m_pParent == pObj)
					return NULL;

				T* pParent = NULL;
				while (pParent = this->EnumParentObject(pParent))
				{
					if (pParent->m_pParent == pObj)
						return pParent;
				}
				return NULL;
			}
			return NULL;
		}

		T* GetParentObject()
		{ 
			return this->m_pParent; 
		}
		T* GetChildObject()
		{
			return this->m_pChild;
		}
		T* GetNcChildObject()
		{
			return this->m_pNcChild;
		}
		T* GetLastChildObject()
		{
			return this->REnumChildObject(NULL);
		}
		T* GetNextObject()
		{
			return this->m_pNext;
		}
		T* GetPrevObject()
		{
			return this->m_pPrev;
		}
		T* GetNeighboutObject(bool bPrevOrNext)
		{
			if (bPrevOrNext)
				return GetPrevObject();
			else
				return GetNextObject();
		}

		//
		//	获取WindowBase中，该对象的下一个子结点控件，该控件可能位于另一个PANEL当中
		//
		//	优先级顺序
		//		1). 自己的子结点
		//		2). 自己的下一个结点
		//		3). 自己的父（祖）结点的下一个结点  -- 如果父结点没有next，继续查找父的父的next结点
		//
		T* GetNeighbourLeafNodeObject(bool bPrevOrNext)
		{
			T* p = (T*)this;
			while (1)
			{
				if (NULL != p->GetChildObject())
				{
					if (bPrevOrNext)
						p = p->GetLastChildObject();
					else
						p = p->GetChildObject();

					continue;
				}

				if (p != (T*)this)
					return p;

				if (NULL != p->GetNeighboutObject(bPrevOrNext))
				{
					p = p->GetNeighboutObject(bPrevOrNext);
					continue;
				}

				T* pParent = p->GetParentObject();
				while (1)
				{
					if (NULL == pParent)
						return NULL;

					if (NULL != pParent->GetNeighboutObject(bPrevOrNext))
					{
						p = pParent->GetNeighboutObject(bPrevOrNext);
						break;    // break to continue;
					}
					else
					{
						pParent = pParent->GetParentObject();
					}
				}
			}
			return NULL;
		}

		//	在整个WindowBase范围内循环获取下一个子结点。
		//	GetNextLeafNodeObject获取下一个子结点时，可能会搜索到结尾时，仍没有找到，这个时候要再从头搜索一次
		T* GetNeighbourLeafNodeObjectCycle(bool bPrevOrNext)
		{
			T* p = this->GetNeighbourLeafNodeObject(bPrevOrNext);
			if (NULL == p)
			{
				T* pThis = (T*)this;
				p = pThis->GetWindowObject()->GetNeighbourLeafNodeObject(bPrevOrNext);  
			}

			if (NULL == p || p == this)
				return NULL;

			return p;
		}

		T* GetNeighbourTabObject(bool bPrevOrNext)
		{
			T* p = (T*)this;
			T* pSearchStart = NULL;
			while(1)
			{
				p = p->GetNeighbourLeafNodeObjectCycle(bPrevOrNext);

				if (NULL == pSearchStart )
				{
					pSearchStart = p;
				}
				else if (pSearchStart == p)    // 防止死循环
				{
					return NULL;
				}

				if (NULL == p || p == this)
					return NULL;

				if (p->IsVisible() && p->IsEnable() && p->IsTabstop())
					return p;
			}
			return NULL;
		}

		T* GetNextTabObject()
		{
			return GetNeighbourTabObject(false);
		}

		T* GetPrevTabObject()
		{
			 return GetNeighbourTabObject(true);
		}

		void ClearTreeObject()
		{
			this->m_pParent = NULL;
			this->m_pNext = NULL;
			this->m_pPrev = NULL;

			T* pThis = (T*)this;
			if (OBJ_CONTROL != pThis->GetObjectType())
			{
				this->m_pChild = NULL;
				this->m_pNcChild = NULL;
			}
		}
	
	protected:
		T*      m_pParent;
		T*      m_pChild;
		T*      m_pNcChild;     // 非客户区的子对象，主要用于实现滚动时，不跟随偏移
		T*      m_pNext;
		T*      m_pPrev;
	};
}



