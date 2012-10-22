// TODO 用于管理顶层窗口，例如发送换肤消息

namespace UI
{
class TopWindowManager
{
public:
	void      AddTopWindowObject( WindowBase* );
	void      RemoveTopWindowObject( WindowBase* );
	void      ClearTopWindowObject( );
	int       GetTopWindowCount() { return (int)m_lTopWindowObject.size(); }
	WindowBase* GetWindowBase(HWND hWnd); 
				
	void      SendMessage( UIMSG* pMsg );
	void      ChangeSkin(SkinManager* pNewSkinMgr);
	void      OnSkinHLSChanged();
	bool      InvalidateWindow();

protected:
	void      GetAllChildIntoList(Object* pParent, list<Object*>& listObjects);

private:
	list<WindowBase*>   m_lTopWindowObject;
};
}