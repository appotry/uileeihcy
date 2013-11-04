#include "stdafx.h"
#include "animatemgr.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\window3danimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowupdownalphashowanimate.h"
#include "UISDK\Kernel\Src\Animate\windowanimate\windowalphashowanimate.h"
#include "UISDK\Kernel\Src\Animate\controlanimate\slideanimate.h"
#include "UISDK\Kernel\Src\Base\Applicatoin\uiapplication.h"

AnimateManager::AnimateManager()
{
    m_pIAnimateManager = NULL;
	m_hTimer = NULL;

	m_bHandlingTimerCallback = false;
	m_nFps = 50;
	m_pUIApplication = NULL;
}

AnimateManager::~AnimateManager()
{
	UIASSERT(NULL == m_hTimer && 0 == m_listObjStoryboard.size());

	// ע��close timer handle������UnCoInitialize�����
    Destroy();
    SAFE_DELETE(m_pIAnimateManager);
}

IAnimateManager*  AnimateManager::GetIAnimateManager()
{
    if (NULL == m_pIAnimateManager)
        m_pIAnimateManager = new IAnimateManager(this);

    return m_pIAnimateManager;
}
int  AnimateManager::SetFps(int n) 
{ 
    int nOld = m_nFps;
    m_nFps = n; 
    if (m_nFps > 1000)
        m_nFps = 1000;
    return nOld;
}
void AnimateManager::Destroy()
{
	EndAnimate();

	ObjectStoryboardIter iter = m_listObjStoryboard.begin();
	ObjectStoryboardIter iterEnd = m_listObjStoryboard.end();
	for (; iter != iterEnd; iter++)
	{
		ObjectStoryboard* p = *iter;
		delete p;
	}
	m_listObjStoryboard.clear();
}

IStoryboard*  AnimateManager::CreateStoryboard()
{
    IStoryboard* p = new IStoryboard;
    p->CreateImpl();
    p->GetImpl()->SetAnimateMgr(this);

    return p;
}

IWindowAnimateBase* AnimateManager::CreateWindowAnimateInstance(E_WINDOW_ANIMATE_TYPE eType, IWindowBase* pWindow)
{
	if (E_WINDOW_ANIMATE_3D_ROTATE == eType)
	{
		IWindow3DAnimate* p = new IWindow3DAnimate;
        p->CreateImpl();

        Window3DAnimate* pImpl = p->GetImpl();
        pImpl->SetAnimateMgr(this);
		pImpl->SetAnimateWindow(pWindow);
        pImpl->Initialize();
		return p;
	}
    else if (E_WINDOW_ANIMATE_UPDOWNALPHASHOW == eType)
    {
        IWindowUpDownAlphaShowAnimate* p = new IWindowUpDownAlphaShowAnimate;
        p->CreateImpl();

        WindowUpDownAlphaShowAnimate* pImpl = p->GetImpl();
        pImpl->SetAnimateMgr(this);
        pImpl->SetAnimateWindow(pWindow);
        pImpl->Initialize();
        return p;
    }
    else if (E_WINDOW_ANIMATE_ALPHASHOW == eType)
    {
        IWindowAlphaShowAnimate* p = new IWindowAlphaShowAnimate;
        p->CreateImpl();

        WindowAlphaShowAnimate* pImpl = p->GetImpl();
        pImpl->SetAnimateMgr(this);
        pImpl->SetAnimateWindow(pWindow);
        pImpl->Initialize();
        return p;
    }

	return NULL;
}

IControlAnimateBase*  AnimateManager::CreateControlAnimateInstance(E_CONTROL_ANIMATE_TYPE eType)
{
    if (E_CONTROL_ANIMATE_SLIDE == eType)
    {
        ISlideAnimate* p = new ISlideAnimate;
        p->CreateImpl();

        SlideAnimate* pImpl = p->GetImpl();
        pImpl->SetAnimateMgr(this);
        return p;
    }

    return NULL;
}

ObjectStoryboard::ObjectStoryboard(IMessage* p)
{
	m_pNotifyObject = p;
	m_pStoryboardList = NULL; 
	m_nCount = 0;
}
ObjectStoryboard::~ObjectStoryboard()
{
	if (m_pStoryboardList)
	{
		for (int i = 0; i < m_nCount; i++)
		{
			SAFE_DELETE_Ixxx(m_pStoryboardList[i]);
		}
		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_nCount = 0;
	}
}

void AnimateManager::RemoveStoryboard(IStoryboard* p)
{
	if (NULL == p)
		return;

    if (m_bHandlingTimerCallback)  // ������ڶ�m_listObjTimeline���б����У��������������list��erase����
    {
        p->GetImpl()->SetFinishFlag();
    }
    else
    {
        ObjectStoryboardIter iter = this->FindObjectStoryboardIter(p->GetNotifyObj());
        if (iter == m_listObjStoryboard.end())
            return;

        ObjectStoryboard* pObjTimeline = *iter;
        pObjTimeline->RemoveStoryboard(p);

        if (0 == pObjTimeline->m_nCount)
        {
            m_listObjStoryboard.erase(iter);
            SAFE_DELETE(pObjTimeline);

            if (0 == m_listObjStoryboard.size())
                KillTimer();
        }
    }
}
void AnimateManager::ClearStoryboardOfNotify(IMessage* pMsg)
{
	if (NULL == pMsg)
		return;

	ObjectStoryboardIter iter = this->FindObjectStoryboardIter(pMsg);
	if (iter == m_listObjStoryboard.end())
		return;

	if (m_bHandlingTimerCallback)  // ������ڶ�m_listObjTimeline���б����У��������������list��erase����
	{
		ObjectStoryboard* pObjTimeline = *iter;
		for (int i = 0; i < pObjTimeline->m_nCount; i++)
		{
			pObjTimeline->m_pStoryboardList[i]->GetImpl()->SetFinishFlag();;
		}
	}
	else
	{
		SAFE_DELETE(*iter);
		m_listObjStoryboard.erase(iter);

		if (0 == m_listObjStoryboard.size())
		{
			KillTimer();
		}
	}
}

void AnimateManager::AddStoryboard(IStoryboard* p)
{
	if (NULL == p)
		return;

	ObjectStoryboard* pObjStoryboard = this->FindObjectStoryboard(p->GetNotifyObj());
	if (NULL == pObjStoryboard)
	{
		pObjStoryboard = new ObjectStoryboard(p->GetNotifyObj());
		m_listObjStoryboard.push_back(pObjStoryboard);
	}
	pObjStoryboard->AddStoryboard(p);

	if (NULL == m_hTimer)
		StartAnimate();

    p->GetImpl()->OnAnimateStart();
}

// ������ͬID��time line��Ҫ���ʱ��������ָ�ID�Ѿ����ڣ�����ȡ��ǰһ��time line
int ObjectStoryboard::FindStoryboard(int nID)
{
	for (int i = 0; i < m_nCount; i++)
	{
		if (m_pStoryboardList[i]->GetId() == nID)
			return i;
	}
	return -1;
}

void ObjectStoryboard::AddStoryboard(IStoryboard* p)
{
	if (NULL == p)
		return;

	int nIndex = -1;
	if (0 != p->GetId())
	{
		nIndex = this->FindStoryboard(p->GetId());
	}
	if (-1 == nIndex)
	{
		IStoryboard** ppArray = new IStoryboard*[m_nCount+1];
		memcpy(ppArray, m_pStoryboardList, sizeof(ITimeline*)*m_nCount);
		ppArray[m_nCount] = p;

		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_pStoryboardList = ppArray;
		m_nCount++;
	}
	else
	{
		// �滻
		SAFE_DELETE_Ixxx(m_pStoryboardList[nIndex]);
		m_pStoryboardList[nIndex] = p;
	}
}

void ObjectStoryboard::RemoveStoryboard(IStoryboard* p)
{
	UIASSERT(m_nCount > 0);
	if (NULL == p)
		return;

	if (m_nCount > 1)
	{
		IStoryboard** ppArray = new IStoryboard*[m_nCount-1];

		int nIndex = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			if (m_pStoryboardList[i] == p)
			{
				SAFE_DELETE_Ixxx(m_pStoryboardList[i]);
			}
			else
			{
				ppArray[nIndex++] = m_pStoryboardList[i];
			}
		}
		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_pStoryboardList = ppArray;

		m_nCount--;
	}
	else if (m_nCount == 1)
	{
		SAFE_DELETE_Ixxx(m_pStoryboardList[0]);
		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_nCount--;
	}
}

void ObjectStoryboard::CheckFinishFlag()
{
	int nAliveCount = 0;
	for (int i = 0; i < m_nCount; i++)
	{
		if (!m_pStoryboardList[i]->IsFinish())
			nAliveCount++;
	}

	if (0 == nAliveCount)
	{
		if (m_pStoryboardList)
		{
			for (int i = 0; i < m_nCount; i++)
			{
				SAFE_DELETE_Ixxx(m_pStoryboardList[i]);
			}
			SAFE_ARRAY_DELETE(m_pStoryboardList);
		}
	}
	else
	{
		IStoryboard** ppArray = new IStoryboard*[nAliveCount];

		int nIndex = 0;
		for (int i = 0; i < m_nCount; i++)
		{
			if (m_pStoryboardList[i]->IsFinish())
			{
				SAFE_DELETE_Ixxx(m_pStoryboardList[i]);				
			}
			else
			{
				ppArray[nIndex++] = m_pStoryboardList[i];
			}
		}
		SAFE_ARRAY_DELETE(m_pStoryboardList);
		m_pStoryboardList = ppArray;
	}

	m_nCount = nAliveCount;
}

ObjectStoryboard* AnimateManager::FindObjectStoryboard(IMessage* pNotify)
{
	ObjectStoryboardIter iter = m_listObjStoryboard.begin();
	ObjectStoryboardIter iterEnd = m_listObjStoryboard.end();
	for (; iter != iterEnd; iter++)
	{
		if ((*iter)->m_pNotifyObject == pNotify)
			return (*iter);
	}
	return NULL;
}
ObjectStoryboardIter AnimateManager::FindObjectStoryboardIter(IMessage* pNotify)
{
	ObjectStoryboardIter iter = m_listObjStoryboard.begin();
	ObjectStoryboardIter iterEnd = m_listObjStoryboard.end();
	for (; iter != iterEnd; iter++)
	{
		if ((*iter)->m_pNotifyObject == pNotify)
			return iter;
	}
	return m_listObjStoryboard.end();
}

void AnimateManager::OnWaitForHandleObjectCallback(HANDLE h, LPARAM l)
{ 
	m_bHandlingTimerCallback = true;

	ObjectStoryboardIter iter = m_listObjStoryboard.begin();
	ObjectStoryboardIter iterEnd = m_listObjStoryboard.end();

	bool bFinish = false;
	for (; iter != iterEnd;)
	{
//		BOOL bNeedCheckFinish = FALSE;

		ObjectStoryboard* pObjStoryboard = *iter;
		for (int i = 0; i < pObjStoryboard->m_nCount; i++)
		{
			bFinish = false;
			pObjStoryboard->m_pStoryboardList[i]->GetImpl()->OnTick(&bFinish);
			if (bFinish)
			{
				pObjStoryboard->m_pStoryboardList[i]->GetImpl()->SetFinishFlag();
//              bNeedCheckFinish = TRUE;
			}
		}

		// ͳһ������Ϣ���п���һ��pNotify����˶�����Ա仯�Ķ�������ֻ�ܷ���һ��֪ͨ������֪������ֵ�����ս������
		if (pObjStoryboard->m_pNotifyObject)
		{
			// ע�� ����Ϣ�����У��п��ܶ���ĳ��objtimeline��remove����clear�ˣ����½�������iter��Ч�����ձ�����
			UISendMessage(pObjStoryboard->m_pNotifyObject, UI_WM_ANIMATE_TICK, (WPARAM)pObjStoryboard->m_nCount, (LPARAM)pObjStoryboard->m_pStoryboardList);
		}

// 		if (bNeedCheckFinish)   ��Ϊ�п�����UISendMessage�е�����RemoveStoryboard��������ﲻ�ж�bNeedCheckFinish��
// 		{
			pObjStoryboard->CheckFinishFlag();
			if (0 == pObjStoryboard->m_nCount)
			{
				// ֪ͨ�������Ķ���������
				if (pObjStoryboard->m_pNotifyObject)
				{
					UISendMessage(pObjStoryboard->m_pNotifyObject, UI_WM_ANIMATE_OVER);
				}

				iter = m_listObjStoryboard.erase(iter);
				SAFE_DELETE(pObjStoryboard);

				if (0 == m_listObjStoryboard.size())
					KillTimer();
			}
			else
			{
				iter++;
			}
// 		}
// 		else
// 		{
// 			iter++;
// 		}
	}

	m_bHandlingTimerCallback = false;
}

void AnimateManager::SetTimer()
{
	if (m_hTimer)
		return;

	if (0 == m_nFps)
		m_nFps = 10;

	int nPeriod = 1000/m_nFps;  // ��ʱ������

	m_hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	IWaitForHandlesMgr* pMgr = NULL;
	m_pUIApplication->GetWaitForHandlesMgr(&pMgr);
	if (pMgr)
	{
		pMgr->AddHandle(m_hTimer, static_cast<IWaitForHandleCallback*>(this), 0);
	}

    LARGE_INTEGER liDueTime;
    liDueTime.QuadPart = -1000*10*nPeriod;   // ��һ����Ӧ�ӳ�ʱ�䡣��ֵ��ʾһ����Ե�ʱ�䣬������100����Ϊ��λ�����ʱ�䣬������������5ms��������Ϊ-50000��

    // ע�� ��Ҫʹ��TimerAPCProc,�÷�ʽ��Ҫ���̴߳���alertable״̬��
    //      ������SleepEx(x, TRUE)���߳̽��еȴ�״̬
    // LONG lPeriod�����ö�ʱ�������Ե����Ҽ������ò����ĵ�λΪ���롣
    // ���Ϊ0�����ʾ��ʱ��ֻ����һ���źţ�����0ʱ����ʱ��û��һ��ʱ
    // ���Զ����¼���һ����ʱ����ֱ��ȡ����ʱ��ʹ��
    if (!SetWaitableTimer(m_hTimer, &liDueTime, nPeriod, NULL, NULL, 0))  
    {
        UIASSERT(0);
    }
}
void AnimateManager::KillTimer()
{
	if (m_hTimer)
	{
		IWaitForHandlesMgr* pMgr = NULL;
		m_pUIApplication->GetWaitForHandlesMgr(&pMgr);
		if (pMgr)
			pMgr->RemoveHandle(m_hTimer);

		// CancelWaitableTimer 
		CloseHandle(m_hTimer);
		m_hTimer = NULL;
	}
}
void AnimateManager::StartAnimate()
{
	KillTimer();
	SetTimer();
}
void AnimateManager::EndAnimate()
{
	KillTimer();
}




