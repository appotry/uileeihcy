#pragma once
namespace UI
{


// class IAnimateTarget
// {
// public:
// 	void OnStart(){};
// 	void OnEnd(){};
// };

// 一个窗口的动画
class UIAPI AnimateBase
{
public:
	AnimateBase()
	{
		m_bTerminateTread = false;
		m_hTargetWnd = false;
	}
	virtual void Start(HWND hWnd)
	{
		m_hTargetWnd = hWnd;
		m_bTerminateTread = false;

		HANDLE hThread = ::CreateThread( NULL, 0, &AnimateBase::AnimateThreadProc, this, 0, 0 );
		CloseHandle(hThread);
	}
	virtual void End()
	{
		m_bTerminateTread = true;
	}
	static DWORD __stdcall AnimateThreadProc(LPVOID lp);
	virtual void OnTick() = 0;


protected:
	int     m_nID;
	HWND    m_hTargetWnd;
protected:
	bool    m_bTerminateTread;
};


// 两个窗口的动画
class UIAPI AnimateBase2Windows
{
public:

};


class UIAPI AnimateBase1Windows : public AnimateBase
{
protected:
	AnimateBase1Windows(){};
public:


	virtual void Start(HWND hWnd);
	virtual void End() ;
	virtual void OnTick() = 0;
	

protected:
	POINT   ptStart;
	POINT   ptEnd;
	HWND    m_hTargetWnd;
	HWND    m_hWinLayered;
//	HDC     m_hScreenDC;
	Image   m_layeredImage;
};



#define UIAW_HIDE_KUOSAN      1        // 向外扩散隐藏
//#define UIAW_HIDE_DISAPPEAR   2        // 渐渐消失

class Animate1W_Hide_Kuosan : public AnimateBase1Windows
{
public:
	void OnTick();
};


//
// 类工厂
//
class AnimateBase1WindowsFactory 
{
public:
	static AnimateBase1Windows* Create( UINT nType )
	{	
		switch( nType )
		{
		case UIAW_HIDE_KUOSAN:
			return new Animate1W_Hide_Kuosan();
		default:
			return NULL;
		}

		return NULL;
		
	}
};


UIAPI void  UIAnimateWindow ( HWND  hWnd,  UINT  nFlag, void* pParam );
UIAPI void  UIAnimateWindow2( HWND  hWnd1, HWND hWnd2,  UINT nFlags, void* pParam );

}
