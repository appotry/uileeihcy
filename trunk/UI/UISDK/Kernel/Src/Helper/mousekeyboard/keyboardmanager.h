#pragma once

namespace UI
{
class Object;
class WindowBase;
class KeyboardMgrBase
{
public:
    KeyboardMgrBase();
    

};

//
//	1. 如果要设置默认第一个控件的焦点，必须让OnInitDialog返回FALSE；
//
class WindowKeyboardMgr : public KeyboardMgrBase
{
public:
	WindowKeyboardMgr( );
	~WindowKeyboardMgr(void);

public:
	LRESULT  HandleMessage( UINT msg, WPARAM w, LPARAM l );
	

private:
	

private:
};

}