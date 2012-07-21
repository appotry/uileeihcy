#pragma 
//
//	ʹ��Tree�ṹʵ��Object֮��Ĳ�ι�ϵ
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

			//	3. �����Լ����ھӹ�ϵ

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
		//			[in]������������NULL��ʼ���÷���ֵ��Ϊ��һ����������
		//
		//	Return
		//		������һ���Ӷ���
		//
		//	Remark
		//		ö���Լ����Ӷ������磺
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

		// ���Լ���parent��ʼ����ö���Լ�������
		T* EnumParentObject(T* pObj)  
		{
			if (NULL == pObj )
				return this->m_pParent;
			else
				return pObj->m_pParent;
		}

		// ���Լ������ڿ�ʼ����ö�٣�ֱ��ö�ٵ��Լ���parent
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
		//	��ȡWindowBase�У��ö������һ���ӽ��ؼ����ÿؼ�����λ����һ��PANEL����
		//
		//	���ȼ�˳��
		//		1). �Լ����ӽ��
		//		2). �Լ�����һ�����
		//		3). �Լ��ĸ����棩������һ�����  -- ��������û��next���������Ҹ��ĸ���next���
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

		//	������WindowBase��Χ��ѭ����ȡ��һ���ӽ�㡣
		//	GetNextLeafNodeObject��ȡ��һ���ӽ��ʱ�����ܻ���������βʱ����û���ҵ������ʱ��Ҫ�ٴ�ͷ����һ��
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
				else if (pSearchStart == p)    // ��ֹ��ѭ��
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
		T*      m_pNcChild;     // �ǿͻ������Ӷ�����Ҫ����ʵ�ֹ���ʱ��������ƫ��
		T*      m_pNext;
		T*      m_pPrev;
	};
}



