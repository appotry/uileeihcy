#ifndef TOPWINDOWMANAGER_H_657F2C1C_0ADD_4576_8DCF_4DE41057B53D
#define TOPWINDOWMANAGER_H_657F2C1C_0ADD_4576_8DCF_4DE41057B53D

// 用于管理顶层窗口，例如换肤

namespace UI
{
class Object;
class WindowBase;
class TopWindowManager
{
public:
    TopWindowManager(UIApplication* p);
    ~TopWindowManager();
    ITopWindowManager*  GetITopWindowManager();

    typedef list<WindowBase*>  _MyList;
    typedef list<WindowBase*>::iterator _MyIter;

	HRESULT  AddTopWindowObject(WindowBase*);
	HRESULT  RemoveTopWindowObject(WindowBase*);
	
	void  ClearTopWindowObject( );
	int   GetTopWindowCount() { return (int)m_lTopWindowObject.size(); }
	IWindowBase* GetWindowBase(HWND hWnd); 
				
	void  ChangeSkin(SkinRes* pNewSkinRes);
	bool  UpdateAllWindow();

    void  SendMessage2AllWnd(UIMSG* pMsg);
    void  PostMessage2AllWnd(UIMSG* pMsg);
    void  ForwardMessage2AllObj(UIMSG*  pMsg);

protected:
	void  GetAllChildIntoList(Object* pParent, list<Object*>& listObjects);

private:
    ITopWindowManager*  m_pITopWindowManager;
	list<WindowBase*>  m_lTopWindowObject;
    UIApplication*  m_pUIApplication;
};
}

#endif // TOPWINDOWMANAGER_H_657F2C1C_0ADD_4576_8DCF_4DE41057B53D